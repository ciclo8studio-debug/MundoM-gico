#include "Cargo/CargoSimulationComponent.h"
#include "Data/CargoItemDataAsset.h"
#include "Blackline.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

namespace
{
	constexpr float GravityCmS2 = 980.665f;
}

UCargoSimulationComponent::UCargoSimulationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 1.f / 20.f; // 20 Hz; se reajusta en BeginPlay segun TickRateHz.
}

void UCargoSimulationComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickInterval(1.f / FMath::Max(1.f, TickRateHz));

	if (bReadVehicleMassOnBeginPlay)
	{
		if (const UPrimitiveComponent* Body = GetOwnerBody())
		{
			const float Mass = Body->GetMass();
			if (Mass > 1.f)
			{
				VehicleMassKg = Mass;
			}
		}
	}
	ApplyCenterOfMassToOwner();
}

UPrimitiveComponent* UCargoSimulationComponent::GetOwnerBody() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Cast<UPrimitiveComponent>(Owner->GetRootComponent()) : nullptr;
}

bool UCargoSimulationComponent::AddCargo(UCargoItemDataAsset* Item, FVector LocalOffsetCm)
{
	if (!Item)
	{
		UE_LOG(LogBlackline, Warning, TEXT("AddCargo: Item nulo."));
		return false;
	}

	const float OldIntegrity = GetCargoIntegrity01();

	FCargoSlot& NewSlot = Slots.AddDefaulted_GetRef();
	NewSlot.Item = Item;
	NewSlot.LocalOffsetCm = LocalOffsetCm;
	SlotIntegrity.Add(1.f);

	BroadcastIntegrityIfChanged(OldIntegrity);
	ApplyCenterOfMassToOwner();
	return true;
}

void UCargoSimulationComponent::ClearCargo()
{
	const float OldIntegrity = GetCargoIntegrity01();

	Slots.Reset();
	SlotIntegrity.Reset();
	CurrentSlideCm = 0.f;

	BroadcastIntegrityIfChanged(OldIntegrity);
	ApplyCenterOfMassToOwner();
}

float UCargoSimulationComponent::GetTotalCargoMassKg() const
{
	float Total = 0.f;
	for (const FCargoSlot& Slot : Slots)
	{
		if (Slot.Item)
		{
			Total += FMath::Max(0.f, Slot.Item->MassKg);
		}
	}
	return Total;
}

FVector UCargoSimulationComponent::GetCargoCenterOfMassLocalCm() const
{
	double TotalMass = 0.0;
	FVector Weighted = FVector::ZeroVector;
	for (const FCargoSlot& Slot : Slots)
	{
		if (Slot.Item)
		{
			const double M = FMath::Max(0.f, Slot.Item->MassKg);
			Weighted += (Slot.LocalOffsetCm + FVector(0.0, CurrentSlideCm, 0.0)) * M;
			TotalMass += M;
		}
	}
	return TotalMass > UE_KINDA_SMALL_NUMBER ? Weighted / TotalMass : FVector::ZeroVector;
}

float UCargoSimulationComponent::GetCargoIntegrity01() const
{
	double TotalMass = 0.0;
	double Weighted = 0.0;
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].Item && SlotIntegrity.IsValidIndex(i))
		{
			const double M = FMath::Max(0.f, Slots[i].Item->MassKg);
			Weighted += SlotIntegrity[i] * M;
			TotalMass += M;
		}
	}
	return TotalMass > UE_KINDA_SMALL_NUMBER ? static_cast<float>(Weighted / TotalMass) : 1.f;
}

float UCargoSimulationComponent::GetUnshieldedSignature01() const
{
	if (bCargoShielded)
	{
		return 0.f;
	}
	float Total = 0.f;
	for (const FCargoSlot& Slot : Slots)
	{
		if (Slot.Item && Slot.Item->bRequiresShielding)
		{
			Total += Slot.Item->ContrabandSignature01;
		}
	}
	return FMath::Clamp(Total, 0.f, 1.f);
}

void UCargoSimulationComponent::ApplyImpact(float ImpactSpeedMs, FVector LocalNormal)
{
	if (Slots.Num() == 0 || ImpactSpeedMs <= ImpactSpeedThresholdMs)
	{
		return;
	}

	const float OldIntegrity = GetCargoIntegrity01();
	const float Damage01 = FMath::Clamp((ImpactSpeedMs - ImpactSpeedThresholdMs) / ImpactReferenceSpeedMs, 0.f, 1.f);

	float Delta = 0.f;
	ApplyDamageToSlots(Damage01, Delta);

	// La carga se desplaza en sentido contrario a la normal del golpe.
	const float Severity = FMath::Clamp(ImpactSpeedMs / ImpactReferenceSpeedMs, 0.f, 1.f);
	const FVector Normal = LocalNormal.GetSafeNormal();
	CurrentSlideCm = FMath::Clamp(CurrentSlideCm - static_cast<float>(Normal.Y) * Severity * MaxSlideCm, -MaxSlideCm, MaxSlideCm);

	if (UWorld* World = GetWorld())
	{
		LastShiftEventTime = World->GetTimeSeconds();
	}
	OnCargoShifted.Broadcast(Severity);

	BroadcastIntegrityIfChanged(OldIntegrity);
	ApplyCenterOfMassToOwner();
}

void UCargoSimulationComponent::ApplyDamageToSlots(float Damage01, float& OutDelta)
{
	OutDelta = 0.f;
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].Item && SlotIntegrity.IsValidIndex(i))
		{
			const float Dmg = Damage01 * FMath::Clamp(Slots[i].Item->Fragility01, 0.f, 1.f);
			SlotIntegrity[i] = FMath::Clamp(SlotIntegrity[i] - Dmg, 0.f, 1.f);
		}
	}
}

void UCargoSimulationComponent::BroadcastIntegrityIfChanged(float OldIntegrity)
{
	const float NewIntegrity = GetCargoIntegrity01();
	const float Delta = NewIntegrity - OldIntegrity;
	if (!FMath::IsNearlyZero(Delta, 1e-5f))
	{
		OnCargoIntegrityChanged.Broadcast(NewIntegrity, Delta);
	}
}

void UCargoSimulationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DeltaTime <= UE_KINDA_SMALL_NUMBER)
	{
		return;
	}

	UpdateLateralG(DeltaTime);
	UpdateShift(DeltaTime);
	ApplyCenterOfMassToOwner();
}

void UCargoSimulationComponent::UpdateLateralG(float DeltaTime)
{
	const UPrimitiveComponent* Body = GetOwnerBody();
	if (!Body || !Body->IsSimulatingPhysics())
	{
		LateralG = FMath::FInterpTo(LateralG, 0.f, DeltaTime, GSmoothingSpeed);
		bHasPreviousVelocity = false;
		return;
	}

	const FTransform& Xf = Body->GetComponentTransform();
	const FVector LocalVel = Xf.InverseTransformVectorNoScale(Body->GetPhysicsLinearVelocity());
	const FVector AngVel = Body->GetPhysicsAngularVelocityInRadians(); // mundo, rad/s
	const float YawRate = static_cast<float>(AngVel.Z); // +Z = giro a la derecha en UE

	// Aceleracion centripeta: a = v_fwd * omega (cm/s^2).
	const float CentripetalCmS2 = static_cast<float>(LocalVel.X) * YawRate;

	float DerivativeCmS2 = CentripetalCmS2;
	if (bHasPreviousVelocity)
	{
		DerivativeCmS2 = (static_cast<float>(LocalVel.Y) - PreviousLateralVelocityCmS) / DeltaTime;
	}
	PreviousLateralVelocityCmS = static_cast<float>(LocalVel.Y);
	bHasPreviousVelocity = true;

	const float W = FMath::Clamp(LateralVelocityDerivativeWeight, 0.f, 1.f);
	const float TargetG = FMath::Lerp(CentripetalCmS2, DerivativeCmS2, W) / GravityCmS2;
	LateralG = FMath::FInterpTo(LateralG, TargetG, DeltaTime, GSmoothingSpeed);
}

void UCargoSimulationComponent::UpdateShift(float DeltaTime)
{
	if (Slots.Num() == 0)
	{
		CurrentSlideCm = FMath::FInterpTo(CurrentSlideCm, 0.f, DeltaTime, SlideInterpSpeed);
		return;
	}

	const float AbsG = FMath::Abs(LateralG);
	const float Range = FMath::Max(KINDA_SMALL_NUMBER, ShiftMaxG - ShiftThresholdG);
	const float Severity = FMath::Clamp((AbsG - ShiftThresholdG) / Range, 0.f, 1.f);

	// La carga se desliza contra la aceleracion lateral (inercia).
	const float TargetSlide = -FMath::Sign(LateralG) * Severity * MaxSlideCm;
	CurrentSlideCm = FMath::FInterpTo(CurrentSlideCm, TargetSlide, DeltaTime, SlideInterpSpeed);

	if (Severity <= 0.f)
	{
		return;
	}

	// Degradacion por desplazamiento, escalada por fragilidad de cada item.
	const float OldIntegrity = GetCargoIntegrity01();
	float Delta = 0.f;
	ApplyDamageToSlots(Severity * ShiftDamagePerSecond * DeltaTime, Delta);
	BroadcastIntegrityIfChanged(OldIntegrity);

	if (Severity >= ShiftEventMinSeverity)
	{
		if (const UWorld* World = GetWorld())
		{
			const float Now = World->GetTimeSeconds();
			if (Now - LastShiftEventTime >= ShiftEventCooldownSeconds)
			{
				LastShiftEventTime = Now;
				OnCargoShifted.Broadcast(Severity);
			}
		}
	}
}

void UCargoSimulationComponent::ApplyCenterOfMassToOwner()
{
	if (!bApplyCenterOfMass)
	{
		return;
	}
	UPrimitiveComponent* Body = GetOwnerBody();
	if (!Body)
	{
		return;
	}

	const float CargoMass = GetTotalCargoMassKg();
	FVector Nudge = FVector::ZeroVector;
	if (CargoMass > UE_KINDA_SMALL_NUMBER)
	{
		// Desplazamiento del CoM combinado respecto al CoM base del vehiculo vacio.
		const FVector CargoCoM = GetCargoCenterOfMassLocalCm();
		Nudge = (CargoCoM - VehicleBaseCenterOfMassLocalCm) * (CargoMass / (VehicleMassKg + CargoMass));
	}

	if (LastAppliedCoMNudge.X != FLT_MAX && FVector::DistSquared(Nudge, LastAppliedCoMNudge) < FMath::Square(CenterOfMassApplyToleranceCm))
	{
		return;
	}

	LastAppliedCoMNudge = Nudge;
	// SetCenterOfMass establece un offset (COMNudge) sobre el CoM calculado del cuerpo. VERIFY-5.8
	Body->SetCenterOfMass(Nudge);
}
