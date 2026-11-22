// Blackline: Night Haul - Checkpoint de inspeccion.
#include "Checkpoint/CheckpointController.h"

#include "Blackline.h"
#include "Cargo/CargoSimulationComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/ManifestDataAsset.h"
#include "Engine/World.h"
#include "Pursuit/PursuitDirectorSubsystem.h"
#include "TimerManager.h"
#include "Vehicle/BlacklineVanPawn.h"

ACheckpointController::ACheckpointController()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	InspectionZone = CreateDefaultSubobject<UBoxComponent>(TEXT("InspectionZone"));
	InspectionZone->SetupAttachment(Root);
	InspectionZone->SetBoxExtent(FVector(400.0f, 300.0f, 200.0f));
	InspectionZone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InspectionZone->SetCollisionObjectType(ECC_WorldStatic);
	InspectionZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	InspectionZone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InspectionZone->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
	InspectionZone->SetGenerateOverlapEvents(true);

	Barrier = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrier"));
	Barrier->SetupAttachment(Root);

	InspectionLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("InspectionLight"));
	InspectionLight->SetupAttachment(Root);
	InspectionLight->SetVisibility(false);

	GuardMarker = CreateDefaultSubobject<USceneComponent>(TEXT("GuardMarker"));
	GuardMarker->SetupAttachment(Root);
}

void ACheckpointController::BeginPlay()
{
	Super::BeginPlay();

	BarrierClosedRotation = Barrier->GetRelativeRotation();

	InspectionZone->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointController::OnZoneBeginOverlap);
	InspectionZone->OnComponentEndOverlap.AddDynamic(this, &ACheckpointController::OnZoneEndOverlap);

	if (!GuardActor && GuardClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Params.Owner = this;
		SpawnedGuard = GetWorld()->SpawnActor<AActor>(GuardClass, GuardMarker->GetComponentTransform(), Params);
		GuardActor = SpawnedGuard;
	}
}

void ACheckpointController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(PollTimer);
		TM.ClearTimer(VerdictTimer);
		TM.ClearTimer(BarrierTimer);
		TM.ClearTimer(ResetTimer);
	}
	Super::EndPlay(EndPlayReason);
}

float ACheckpointController::GetSuspicion01() const
{
	return FMath::Clamp(StaticSuspicion + DynamicSuspicion, 0.0f, 1.0f);
}

void ACheckpointController::SetState(ECheckpointState NewState)
{
	if (NewState == State)
	{
		return;
	}
	const ECheckpointState Old = State;
	State = NewState;
	UE_LOG(LogBlackline, Log, TEXT("Checkpoint %s: %d -> %d"), *GetName(), static_cast<int32>(Old), static_cast<int32>(NewState));
	OnCheckpointStateChanged.Broadcast(Old, NewState);
}

void ACheckpointController::OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(OtherActor);
	if (!Van || State != ECheckpointState::Idle)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(ResetTimer);

	ActiveVan = Van;
	PresentedManifest = nullptr;
	StopStartTime = -1.0f;
	ApproachPeakSpeedKmh = Van->GetSpeedKmh();
	CachedSpeedBeforeStopKmh = 0.0f;
	StaticSuspicion = 0.0f;
	DynamicSuspicion = 0.0f;
	bVerdictScheduled = false;
	bSecondaryPhase = false;

	SetState(ECheckpointState::Approaching);
	GetWorldTimerManager().SetTimer(PollTimer, this, &ACheckpointController::Poll, FMath::Max(0.02f, PollIntervalSeconds), true);
}

void ACheckpointController::OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor != ActiveVan)
	{
		return;
	}

	switch (State)
	{
	case ECheckpointState::Approaching:
	case ECheckpointState::Inspecting:
		if (bPursuitOnRunThrough)
		{
			DynamicSuspicion = 1.0f;
			ApplyVerdict(EInspectionVerdict::PursuitAlert);
		}
		else
		{
			ClearInspectionTimers();
			if (ActiveVan)
			{
				ActiveVan->OpenWindowForInspection(false);
			}
			ActiveVan = nullptr;
			SetLight(false, LightColorInspecting);
			SetState(ECheckpointState::Idle);
		}
		break;

	case ECheckpointState::Cleared:
		GetWorldTimerManager().SetTimer(ResetTimer, this, &ACheckpointController::ResetCheckpoint, FMath::Max(0.01f, ResetDelayAfterLeavingSeconds), false);
		break;

	default:
		break;
	}
}

void ACheckpointController::Poll()
{
	if (!ActiveVan)
	{
		ClearInspectionTimers();
		SetState(ECheckpointState::Idle);
		return;
	}

	const UWorld* World = GetWorld();
	const float Now = World->GetTimeSeconds();
	const float Speed = ActiveVan->GetSpeedKmh();

	if (State == ECheckpointState::Approaching)
	{
		ApproachPeakSpeedKmh = FMath::Max(ApproachPeakSpeedKmh, Speed);
		if (Speed <= StopSpeedKmh)
		{
			if (StopStartTime < 0.0f)
			{
				StopStartTime = Now;
			}
			if (Now - StopStartTime >= StopHoldSeconds)
			{
				BeginInspection();
			}
		}
		else
		{
			StopStartTime = -1.0f;
		}
		return;
	}

	if (State == ECheckpointState::Inspecting)
	{
		const float Dt = FMath::Max(0.02f, PollIntervalSeconds);

		if (Speed >= BreakAwaySpeedKmh)
		{
			DynamicSuspicion = 1.0f;
			ApplyVerdict(EInspectionVerdict::PursuitAlert);
			return;
		}
		if (Speed >= MoveDuringInspectionKmh)
		{
			DynamicSuspicion += MovePenaltyPerSecond * Dt;
		}
		if (ActiveVan->GetSuspicionSnapshot().bSignalJammerOn)
		{
			DynamicSuspicion += JammerAccumulationPerSecond * Dt;
		}
		DynamicSuspicion = FMath::Clamp(DynamicSuspicion, 0.0f, 1.0f);

		if (!bVerdictScheduled && Now - InspectionStartTime >= ManifestTimeoutSeconds)
		{
			UE_LOG(LogBlackline, Log, TEXT("Checkpoint %s: manifiesto no presentado a tiempo"), *GetName());
			ResolveInspection();
		}
	}
}

void ACheckpointController::BeginInspection()
{
	CachedSpeedBeforeStopKmh = FMath::Max(ApproachPeakSpeedKmh, ActiveVan->GetSuspicionSnapshot().SpeedBeforeStopKmh);
	InspectionStartTime = GetWorld()->GetTimeSeconds();

	SetState(ECheckpointState::Inspecting);
	ActiveVan->OpenWindowForInspection(true);
	SetLight(true, LightColorInspecting);

	if (bGuardFacesVan && GuardActor)
	{
		FVector ToVan = ActiveVan->GetActorLocation() - GuardActor->GetActorLocation();
		ToVan.Z = 0.0f;
		if (!ToVan.IsNearlyZero())
		{
			GuardActor->SetActorRotation(ToVan.Rotation());
		}
	}
}

void ACheckpointController::PresentManifest(UManifestDataAsset* Manifest)
{
	if (State != ECheckpointState::Inspecting || bVerdictScheduled)
	{
		UE_LOG(LogBlackline, Verbose, TEXT("Checkpoint %s: PresentManifest ignorado (estado %d)"), *GetName(), static_cast<int32>(State));
		return;
	}

	PresentedManifest = Manifest;
	bVerdictScheduled = true;
	GetWorldTimerManager().SetTimer(VerdictTimer, this, &ACheckpointController::ResolveInspection, FMath::Max(0.01f, VerdictDelaySeconds), false);
}

int32 ACheckpointController::ManifestLevelFromQuality(float ForgeryQuality01)
{
	return FMath::Clamp(FMath::CeilToInt(FMath::Clamp(ForgeryQuality01, 0.0f, 1.0f) * 3.0f), 1, 3);
}

void ACheckpointController::ApplyContract(const FBlacklineContractRow& Row)
{
	RequiredManifestLevel = FMath::Clamp(Row.RequiredManifestLevel, 1, 3);
	UE_LOG(LogBlackline, Log, TEXT("Reten %s: contrato %s exige documento nivel %d"),
		*GetName(), *Row.ContractID.ToString(), RequiredManifestLevel);
}

float ACheckpointController::ComputeStaticSuspicion(bool bScrutiny) const
{
	if (!ActiveVan)
	{
		return 1.0f;
	}

	FSuspicionSnapshot Snap = ActiveVan->GetSuspicionSnapshot();
	Snap.SpeedBeforeStopKmh = FMath::Max(Snap.SpeedBeforeStopKmh, CachedSpeedBeforeStopKmh);

	const float SpeedRange = FMath::Max(1.0f, SpeedSuspicionFullKmh - SpeedSuspicionStartKmh);
	const float SpeedFactor = FMath::Clamp((Snap.SpeedBeforeStopKmh - SpeedSuspicionStartKmh) / SpeedRange, 0.0f, 1.0f);

	float Score = 0.0f;
	Score += Weights.SpeedBeforeStop * SpeedFactor;
	Score += Weights.HeadlightsOff * (Snap.bHeadlightsOn ? 0.0f : 1.0f);
	Score += Weights.Damage * FMath::Clamp(Snap.DamageRatio01, 0.0f, 1.0f);
	Score += Weights.UnshieldedSignature * FMath::Clamp(Snap.UnshieldedSignature01, 0.0f, 1.0f);
	Score += Weights.SignalJammer * (Snap.bSignalJammerOn ? 1.0f : 0.0f);
	Score += Weights.CargoIntegrityLoss * FMath::Clamp(1.0f - Snap.CargoIntegrity01, 0.0f, 1.0f);

	// Manifiesto vs carga real.
	const float Scrutiny = bScrutiny ? SecondaryScrutinyMultiplier : 1.0f;
	if (!PresentedManifest)
	{
		Score += Weights.NoManifest * Scrutiny;
	}
	else
	{
		const float DeclaredMass = PresentedManifest->ComputeDeclaredMassKg();
		const float DeclaredSig = PresentedManifest->ComputeDeclaredSignature01();
		float ActualMass = 0.0f;
		float ActualSig = Snap.UnshieldedSignature01;
		if (ActiveVan->Cargo)
		{
			ActualMass = ActiveVan->Cargo->GetTotalCargoMassKg();
			ActualSig = ActiveVan->Cargo->GetUnshieldedSignature01();
		}

		const float MassDiffRatio = FMath::Abs(ActualMass - DeclaredMass) / FMath::Max(1.0f, FMath::Max(ActualMass, DeclaredMass));
		const float MassFactor = FMath::Clamp((MassDiffRatio - MassToleranceRatio) / FMath::Max(0.01f, MassMismatchFullRatio - MassToleranceRatio), 0.0f, 1.0f);

		// Solo cuenta la firma que NO esta declarada (contrabando oculto).
		const float SigFactor = FMath::Clamp((ActualSig - DeclaredSig) - SignatureToleranceDelta, 0.0f, 1.0f);

		const float ForgeryHide = 1.0f - FMath::Clamp(PresentedManifest->ForgeryQuality01, 0.0f, 1.0f) * ForgeryMitigation;
		Score += (Weights.MassMismatch * MassFactor + Weights.SignatureMismatch * SigFactor) * ForgeryHide * Scrutiny;

		// Contrato: el documento debe cubrir el nivel exigido (1-3).
		const int32 Deficit = RequiredManifestLevel - ManifestLevelFromQuality(PresentedManifest->ForgeryQuality01);
		if (Deficit > 0)
		{
			Score += ManifestLevelDeficitPenalty * static_cast<float>(Deficit) * Scrutiny;
		}
	}

	return FMath::Clamp(Score, 0.0f, 1.0f);
}

void ACheckpointController::ResolveInspection()
{
	if (State != ECheckpointState::Inspecting)
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(VerdictTimer);
	StaticSuspicion = ComputeStaticSuspicion(bSecondaryPhase);
	const float Suspicion = GetSuspicion01();

	UE_LOG(LogBlackline, Log, TEXT("Checkpoint %s: sospecha %.2f (estatica %.2f, dinamica %.2f, secundaria=%d)"),
		*GetName(), Suspicion, StaticSuspicion, DynamicSuspicion, bSecondaryPhase ? 1 : 0);

	if (Suspicion >= PursuitThreshold01)
	{
		ApplyVerdict(EInspectionVerdict::PursuitAlert);
		return;
	}

	if (!bSecondaryPhase && Suspicion >= SecondaryThreshold01)
	{
		bSecondaryPhase = true;
		bVerdictScheduled = true;
		OnInspectionVerdict.Broadcast(EInspectionVerdict::SecondaryInspection, Suspicion);
		OnGuardReact(GuardActor, EInspectionVerdict::SecondaryInspection, Suspicion);
		GetWorldTimerManager().SetTimer(VerdictTimer, this, &ACheckpointController::ResolveInspection, FMath::Max(0.01f, SecondaryInspectionSeconds), false);
		return;
	}

	ApplyVerdict(EInspectionVerdict::CleanPass);
}

void ACheckpointController::ApplyVerdict(EInspectionVerdict Verdict)
{
	ClearInspectionTimers();

	ABlacklineVanPawn* Van = ActiveVan;
	if (Van)
	{
		Van->OpenWindowForInspection(false);
	}

	const float Suspicion = GetSuspicion01();
	OnInspectionVerdict.Broadcast(Verdict, Suspicion);
	OnGuardReact(GuardActor, Verdict, Suspicion);

	if (Verdict == EInspectionVerdict::PursuitAlert)
	{
		SetLight(true, LightColorAlert);
		SetBarrierOpen(false);
		SetState(ECheckpointState::PursuitTriggered);
		if (UPursuitDirectorSubsystem* Pursuit = UPursuitDirectorSubsystem::Get(this))
		{
			if (Van)
			{
				Pursuit->StartPursuit(Van, PursuitLevel);
				Pursuit->ReportTargetVisible(true);
			}
		}
		else
		{
			UE_LOG(LogBlackline, Warning, TEXT("Checkpoint %s: sin PursuitDirectorSubsystem"), *GetName());
		}
	}
	else
	{
		SetLight(true, LightColorCleared);
		SetBarrierOpen(true);
		SetState(ECheckpointState::Cleared);
	}
}

void ACheckpointController::ResetCheckpoint()
{
	ClearInspectionTimers();
	GetWorldTimerManager().ClearTimer(ResetTimer);

	if (ActiveVan)
	{
		ActiveVan->OpenWindowForInspection(false);
	}
	ActiveVan = nullptr;
	PresentedManifest = nullptr;
	StaticSuspicion = 0.0f;
	DynamicSuspicion = 0.0f;
	bVerdictScheduled = false;
	bSecondaryPhase = false;
	StopStartTime = -1.0f;

	SetLight(false, LightColorInspecting);
	SetBarrierOpen(false);
	SetState(ECheckpointState::Idle);
}

void ACheckpointController::ClearInspectionTimers()
{
	FTimerManager& TM = GetWorldTimerManager();
	TM.ClearTimer(PollTimer);
	TM.ClearTimer(VerdictTimer);
}

void ACheckpointController::SetLight(bool bOn, const FLinearColor& Color)
{
	if (!InspectionLight)
	{
		return;
	}
	InspectionLight->SetLightColor(Color);
	InspectionLight->SetVisibility(bOn);
}

void ACheckpointController::SetBarrierOpen(bool bOpen)
{
	BarrierTarget = bOpen ? 1.0f : 0.0f;
	if (FMath::IsNearlyEqual(BarrierAlpha, BarrierTarget))
	{
		return;
	}
	constexpr float StepSeconds = 1.0f / 30.0f;
	GetWorldTimerManager().SetTimer(BarrierTimer, this, &ACheckpointController::StepBarrier, StepSeconds, true);
}

void ACheckpointController::StepBarrier()
{
	constexpr float StepSeconds = 1.0f / 30.0f;
	BarrierAlpha = FMath::FInterpConstantTo(BarrierAlpha, BarrierTarget, StepSeconds, 1.0f / FMath::Max(0.05f, BarrierMoveSeconds));

	FRotator Rot = BarrierClosedRotation;
	Rot.Pitch += BarrierOpenPitchDeg * BarrierAlpha;
	Barrier->SetRelativeRotation(Rot);

	if (FMath::IsNearlyEqual(BarrierAlpha, BarrierTarget))
	{
		GetWorldTimerManager().ClearTimer(BarrierTimer);
	}
}
