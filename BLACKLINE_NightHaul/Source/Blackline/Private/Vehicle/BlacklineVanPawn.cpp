// Blackline: Night Haul - Furgoneta jugable.
#include "Vehicle/BlacklineVanPawn.h"

#include "Blackline.h"
#include "Cargo/CargoSimulationComponent.h"
#include "Terrain/TerrainModifierVolume.h"
#include "Pursuit/PursuitDirectorSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "ChaosVehicleWheel.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"

namespace BlacklineVanPrivate
{
	constexpr float CameraTimerRate = 1.f / 60.f;
	constexpr float CmPerSecToKmh = 0.036f;
}

ABlacklineVanPawn::ABlacklineVanPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Sin Tick en el actor: todo va por timers y eventos.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Fisica del chasis (la malla/ruedas se asignan en el BP hijo).
	USkeletalMeshComponent* VanMesh = GetMesh();
	VanMesh->SetNotifyRigidBodyCollision(true);

	Cargo = CreateDefaultSubobject<UCargoSimulationComponent>(TEXT("Cargo"));

	CockpitCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CockpitCamera"));
	CockpitCamera->SetupAttachment(VanMesh);
	CockpitCamera->SetRelativeLocation(FVector(10.f, -35.f, 125.f));
	CockpitCamera->bUsePawnControlRotation = false;
	CockpitCamera->FieldOfView = 85.f;

	ChaseArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("ChaseArm"));
	ChaseArm->SetupAttachment(VanMesh);
	ChaseArm->TargetArmLength = 650.f;
	ChaseArm->SocketOffset = FVector(0.f, 0.f, 120.f);
	ChaseArm->bUsePawnControlRotation = true;
	ChaseArm->bInheritPitch = true;
	ChaseArm->bInheritYaw = true;
	ChaseArm->bInheritRoll = false;
	ChaseArm->bEnableCameraLag = true;
	ChaseArm->CameraLagSpeed = 6.f;

	ChaseCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	ChaseCamera->SetupAttachment(ChaseArm, USpringArmComponent::SocketName);
	ChaseCamera->bUsePawnControlRotation = false;

	HeadlightLeft = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightLeft"));
	HeadlightLeft->SetupAttachment(VanMesh);
	HeadlightLeft->SetRelativeLocation(FVector(230.f, -70.f, 70.f));
	HeadlightLeft->OuterConeAngle = 40.f;
	HeadlightLeft->Intensity = 8000.f;
	HeadlightLeft->AttenuationRadius = 3000.f;

	HeadlightRight = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightRight"));
	HeadlightRight->SetupAttachment(VanMesh);
	HeadlightRight->SetRelativeLocation(FVector(230.f, 70.f, 70.f));
	HeadlightRight->OuterConeAngle = 40.f;
	HeadlightRight->Intensity = 8000.f;
	HeadlightRight->AttenuationRadius = 3000.f;
}

UChaosWheeledVehicleMovementComponent* ABlacklineVanPawn::GetChaosMovement() const
{
	return Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
}

// ---------------------------------------------------------------- Ciclo de vida

void ABlacklineVanPawn::BeginPlay()
{
	Super::BeginPlay();

	// IMC (tambien se reintenta en NotifyControllerChanged por si la posesion llega despues).
	RegisterIMC();

	bEngineOn = bStartEngineOn;
	bHeadlightsOn = bStartHeadlightsOn;
	SetHeadlights(bHeadlightsOn);

	TargetPerspective = StartPerspective;
	CameraBlendAlpha = (StartPerspective == ECameraPerspective::ThirdPerson) ? 1.f : 0.f;

	CacheBaseWheelFriction();
	DriveMode = StartDriveMode;
	ApplyDriveConfig();

	if (USkeletalMeshComponent* VanMesh = GetMesh())
	{
		VanMesh->SetNotifyRigidBodyCollision(true);
		VanMesh->OnComponentHit.AddDynamic(this, &ABlacklineVanPawn::OnMeshHit);
	}

	if (Cargo)
	{
		// VERIFY-5.8: nombres de los delegados en UCargoSimulationComponent segun CONTRATO.md
		Cargo->OnCargoShifted.AddDynamic(this, &ABlacklineVanPawn::HandleCargoShifted);
		Cargo->OnCargoIntegrityChanged.AddDynamic(this, &ABlacklineVanPawn::HandleCargoIntegrityChanged);
	}

	const int32 HistoryLen = FMath::Max(1, FMath::RoundToInt(SpeedHistoryWindowSec / SpeedSampleIntervalSec));
	SpeedHistoryKmh.Init(0.f, HistoryLen);
	SpeedHistoryHead = 0;
	GetWorldTimerManager().SetTimer(SpeedSampleTimer, this, &ABlacklineVanPawn::SampleSpeed, SpeedSampleIntervalSec, true);

	NotifyPursuitEngineAndLights();
}

void ABlacklineVanPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(CameraBlendTimer);
	GetWorldTimerManager().ClearTimer(SpeedSampleTimer);
	UnregisterIMC();
	Super::EndPlay(EndPlayReason);
}

void ABlacklineVanPawn::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	UnregisterIMC();
	RegisterIMC();
}

// ---------------------------------------------------------------- Enhanced Input

void ABlacklineVanPawn::RegisterIMC()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !DefaultIMC)
	{
		return;
	}
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Sub->AddMappingContext(DefaultIMC, IMCPriority);
			IMCController = PC;
		}
	}
}

void ABlacklineVanPawn::UnregisterIMC()
{
	if (APlayerController* PC = IMCController.Get())
	{
		if (DefaultIMC)
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					Sub->RemoveMappingContext(DefaultIMC);
				}
			}
		}
	}
	IMCController.Reset();
}

void ABlacklineVanPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC)
	{
		UE_LOG(LogBlackline, Error, TEXT("BlacklineVanPawn: el InputComponent no es EnhancedInputComponent (revisar Project Settings > Input)."));
		return;
	}

	if (IA_Throttle)
	{
		EIC->BindAction(IA_Throttle, ETriggerEvent::Triggered, this, &ABlacklineVanPawn::OnThrottle);
		EIC->BindAction(IA_Throttle, ETriggerEvent::Completed, this, &ABlacklineVanPawn::OnThrottle);
	}
	if (IA_Steer)
	{
		EIC->BindAction(IA_Steer, ETriggerEvent::Triggered, this, &ABlacklineVanPawn::OnSteer);
		EIC->BindAction(IA_Steer, ETriggerEvent::Completed, this, &ABlacklineVanPawn::OnSteer);
	}
	if (IA_Brake)
	{
		EIC->BindAction(IA_Brake, ETriggerEvent::Triggered, this, &ABlacklineVanPawn::OnBrake);
		EIC->BindAction(IA_Brake, ETriggerEvent::Completed, this, &ABlacklineVanPawn::OnBrake);
	}
	if (IA_Handbrake)
	{
		EIC->BindAction(IA_Handbrake, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnHandbrakeStarted);
		EIC->BindAction(IA_Handbrake, ETriggerEvent::Completed, this, &ABlacklineVanPawn::OnHandbrakeCompleted);
	}
	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ABlacklineVanPawn::OnLook);
	}
	if (IA_ToggleCamera)
	{
		EIC->BindAction(IA_ToggleCamera, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnToggleCameraInput);
	}
	if (IA_Interact)
	{
		EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnInteract);
	}
	if (IA_DriveMode)
	{
		EIC->BindAction(IA_DriveMode, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnDriveModeInput);
	}
}

void ABlacklineVanPawn::OnThrottle(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		// Motor apagado: no hay traccion.
		Move->SetThrottleInput(bEngineOn ? FMath::Clamp(Value.Get<float>(), 0.f, 1.f) : 0.f);
	}
}

void ABlacklineVanPawn::OnSteer(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		Move->SetSteeringInput(FMath::Clamp(Value.Get<float>(), -1.f, 1.f));
	}
}

void ABlacklineVanPawn::OnBrake(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		Move->SetBrakeInput(FMath::Clamp(Value.Get<float>(), 0.f, 1.f));
	}
}

void ABlacklineVanPawn::OnHandbrakeStarted(const FInputActionValue&)
{
	bHandbrakeHeld = true;
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		Move->SetHandbrakeInput(true);
	}
}

void ABlacklineVanPawn::OnHandbrakeCompleted(const FInputActionValue&)
{
	bHandbrakeHeld = false;
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		// Con el motor apagado el freno de mano queda puesto.
		Move->SetHandbrakeInput(!bEngineOn);
	}
}

void ABlacklineVanPawn::OnLook(const FInputActionValue& Value)
{
	const FVector2D Look = Value.Get<FVector2D>() * LookSensitivity;
	if (CameraBlendAlpha < 0.5f)
	{
		// Cockpit: mover la cabeza, acotada.
		CockpitLookYaw = FMath::Clamp(CockpitLookYaw + Look.X, -CockpitMaxYawDeg, CockpitMaxYawDeg);
		CockpitLookPitch = FMath::Clamp(CockpitLookPitch + Look.Y, -CockpitMaxPitchDeg, CockpitMaxPitchDeg);
		ApplyCockpitLook();
	}
	else
	{
		// Chase: orbita via rotacion del controlador (ChaseArm usa PawnControlRotation).
		AddControllerYawInput(Look.X);
		AddControllerPitchInput(-Look.Y);
	}
}

void ABlacklineVanPawn::ApplyCockpitLook()
{
	if (CockpitCamera)
	{
		CockpitCamera->SetRelativeRotation(FRotator(CockpitLookPitch, CockpitLookYaw, 0.f));
	}
}

void ABlacklineVanPawn::OnToggleCameraInput(const FInputActionValue&)
{
	ToggleCamera();
}

void ABlacklineVanPawn::OnDriveModeInput(const FInputActionValue&)
{
	SetDriveMode(DriveMode == EDriveMode::RWD ? EDriveMode::FourWheel : EDriveMode::RWD);
}

void ABlacklineVanPawn::OnInteract(const FInputActionValue&)
{
	UWorld* World = GetWorld();
	if (!World || !CockpitCamera)
	{
		return;
	}

	// Las manos del conductor estan en el cockpit: la traza sale de esa camara (incluye giro de cabeza).
	const FVector Start = CockpitCamera->GetComponentLocation();
	const FVector End = Start + CockpitCamera->GetForwardVector() * InteractRangeCm;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(BlacklineDashTrace), false, this);
	FHitResult Hit;
	if (!World->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, Params))
	{
		return;
	}

	const UPrimitiveComponent* HitComp = Hit.GetComponent();
	if (!HitComp)
	{
		return;
	}

	static const TArray<TPair<FName, EDashControl>> TagMap = {
		{ FName(TEXT("Dash.Headlights")),    EDashControl::Headlights },
		{ FName(TEXT("Dash.Wipers")),        EDashControl::Wipers },
		{ FName(TEXT("Dash.Window")),        EDashControl::Window },
		{ FName(TEXT("Dash.SignalJammer")),  EDashControl::SignalJammer },
		{ FName(TEXT("Dash.DiffLock")),      EDashControl::DiffLock },
		{ FName(TEXT("Dash.Ignition")),      EDashControl::Ignition },
	};

	for (const TPair<FName, EDashControl>& Entry : TagMap)
	{
		if (HitComp->ComponentHasTag(Entry.Key))
		{
			ActivateDashControl(Entry.Value);
			return;
		}
	}
}

// ---------------------------------------------------------------- Camara

void ABlacklineVanPawn::ToggleCamera()
{
	TargetPerspective = (TargetPerspective == ECameraPerspective::Cockpit)
		? ECameraPerspective::ThirdPerson
		: ECameraPerspective::Cockpit;

	FTimerManager& TM = GetWorldTimerManager();
	if (!TM.IsTimerActive(CameraBlendTimer))
	{
		TM.SetTimer(CameraBlendTimer, this, &ABlacklineVanPawn::StepCameraBlend, BlacklineVanPrivate::CameraTimerRate, true);
	}
}

void ABlacklineVanPawn::StepCameraBlend()
{
	const float Goal = (TargetPerspective == ECameraPerspective::ThirdPerson) ? 1.f : 0.f;
	const float Step = BlacklineVanPrivate::CameraTimerRate / FMath::Max(CameraBlendTimeSec, 0.05f);
	CameraBlendAlpha = FMath::FInterpConstantTo(CameraBlendAlpha, Goal, 1.f, Step);

	if (FMath::IsNearlyEqual(CameraBlendAlpha, Goal))
	{
		CameraBlendAlpha = Goal;
		GetWorldTimerManager().ClearTimer(CameraBlendTimer);
	}
}

void ABlacklineVanPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (!CockpitCamera || !ChaseCamera)
	{
		Super::CalcCamera(DeltaTime, OutResult);
		return;
	}

	FMinimalViewInfo Cockpit;
	FMinimalViewInfo Chase;
	CockpitCamera->GetCameraView(DeltaTime, Cockpit);
	ChaseCamera->GetCameraView(DeltaTime, Chase);

	const float T = FMath::InterpEaseInOut(0.f, 1.f, CameraBlendAlpha, 2.f);
	if (T <= KINDA_SMALL_NUMBER)
	{
		OutResult = Cockpit;
	}
	else if (T >= 1.f - KINDA_SMALL_NUMBER)
	{
		OutResult = Chase;
	}
	else
	{
		OutResult = Cockpit;
		OutResult.Location = FMath::Lerp(Cockpit.Location, Chase.Location, T);
		OutResult.Rotation = FQuat::Slerp(Cockpit.Rotation.Quaternion(), Chase.Rotation.Quaternion(), T).Rotator();
		OutResult.FOV = FMath::Lerp(Cockpit.FOV, Chase.FOV, T);
	}
}

// ---------------------------------------------------------------- Traccion RWD / 4WD

void ABlacklineVanPawn::SetDriveMode(EDriveMode NewMode)
{
	if (DriveMode == NewMode)
	{
		return;
	}
	DriveMode = NewMode;

	// Sin 4WD el bloqueo diferencial no tiene sentido.
	if (DriveMode == EDriveMode::RWD && bDiffLocked)
	{
		bDiffLocked = false;
		BroadcastDash(EDashControl::DiffLock, false);
	}

	ApplyDriveConfig();
	RecomputeSurfaceFriction();
	OnDriveModeChanged.Broadcast(DriveMode);
}

void ABlacklineVanPawn::ApplyDriveConfig()
{
	UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement();
	if (!Move)
	{
		return;
	}

	// Verificado en 5.8.4: en runtime solo existe SetDifferentialFrontRearSplit (0 = todo el par atras).
	// El tipo de diferencial (AWD/RWD) se define en el BP del vehiculo como AllWheelDrive.
	const float Split = (DriveMode == EDriveMode::FourWheel)
		? (bDiffLocked ? FrontRearSplitLocked : FrontRearSplitUnlocked)
		: 0.f;
	Move->SetDifferentialFrontRearSplit(Split);
}

// ---------------------------------------------------------------- Superficies

void ABlacklineVanPawn::CacheBaseWheelFriction()
{
	BaseWheelFriction.Reset();
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		for (const UChaosVehicleWheel* Wheel : Move->Wheels)
		{
			BaseWheelFriction.Add(Wheel ? Wheel->FrictionForceMultiplier : 1.f);
		}
	}
}

void ABlacklineVanPawn::PushSurfaceModifier(ATerrainModifierVolume* Volume)
{
	if (!Volume)
	{
		return;
	}
	ActiveModifiers.AddUnique(Volume);
	RecomputeSurfaceFriction();
}

void ABlacklineVanPawn::PopSurfaceModifier(ATerrainModifierVolume* Volume)
{
	if (!Volume)
	{
		return;
	}
	ActiveModifiers.RemoveSingle(Volume);
	RecomputeSurfaceFriction();
}

void ABlacklineVanPawn::RecomputeSurfaceFriction()
{
	UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement();
	if (!Move)
	{
		return;
	}

	// Limpia referencias muertas y busca el volumen mas restrictivo.
	ActiveModifiers.RemoveAll([](const TWeakObjectPtr<ATerrainModifierVolume>& V) { return !V.IsValid(); });

	float Multiplier = 1.f;
	ATerrainModifierVolume* StuckVolume = nullptr;
	const bool bHas4WD = (DriveMode == EDriveMode::FourWheel);

	for (const TWeakObjectPtr<ATerrainModifierVolume>& WeakVol : ActiveModifiers)
	{
		if (const ATerrainModifierVolume* Vol = WeakVol.Get())
		{
			Multiplier = FMath::Min(Multiplier, Vol->FrictionMultiplier);
			if (Vol->bRequires4WD && !bHas4WD && !StuckVolume)
			{
				StuckVolume = WeakVol.Get();
			}
		}
	}

	if (StuckVolume)
	{
		Multiplier *= StuckFrictionFactor;
	}

	for (int32 i = 0; i < Move->Wheels.Num(); ++i)
	{
		const float Base = BaseWheelFriction.IsValidIndex(i) ? BaseWheelFriction[i] : 1.f;
		Move->SetWheelFrictionMultiplier(i, Base * Multiplier); // VERIFY-5.8: existencia/firma
	}

	const bool bStuckNow = (StuckVolume != nullptr);
	if (bStuckNow != bStuckWarningActive)
	{
		bStuckWarningActive = bStuckNow;
		if (bStuckNow)
		{
			UE_LOG(LogBlackline, Warning, TEXT("Atasco: terreno exige 4WD (%s)."), *GetNameSafe(StuckVolume));
		}
		OnVehicleStuckWarning.Broadcast(StuckVolume, bStuckNow);
	}
}

// ---------------------------------------------------------------- Tablero

void ABlacklineVanPawn::ActivateDashControl(EDashControl Control)
{
	switch (Control)
	{
	case EDashControl::Headlights:
		SetHeadlights(!bHeadlightsOn);
		NotifyPursuitEngineAndLights();
		BroadcastDash(Control, bHeadlightsOn);
		break;

	case EDashControl::Wipers:
		bWipersOn = !bWipersOn;
		BroadcastDash(Control, bWipersOn);
		break;

	case EDashControl::Window:
		OpenWindowForInspection(!bWindowOpen);
		break;

	case EDashControl::SignalJammer:
		bSignalJammerOn = !bSignalJammerOn;
		BroadcastDash(Control, bSignalJammerOn);
		break;

	case EDashControl::DiffLock:
		bDiffLocked = !bDiffLocked;
		if (bDiffLocked && DriveMode != EDriveMode::FourWheel)
		{
			// Bloquear el diferencial implica 4WD.
			SetDriveMode(EDriveMode::FourWheel);
		}
		else
		{
			ApplyDriveConfig();
		}
		BroadcastDash(Control, bDiffLocked);
		break;

	case EDashControl::Ignition:
		SetEngine(!bEngineOn);
		NotifyPursuitEngineAndLights();
		BroadcastDash(Control, bEngineOn);
		break;

	default:
		break;
	}
}

void ABlacklineVanPawn::BroadcastDash(EDashControl Control, bool bOn)
{
	OnDashControlChanged.Broadcast(Control, bOn);
	BP_OnDashVisual(Control, bOn);
}

void ABlacklineVanPawn::SetHeadlights(bool bOn)
{
	bHeadlightsOn = bOn;
	if (HeadlightLeft)
	{
		HeadlightLeft->SetVisibility(bOn);
	}
	if (HeadlightRight)
	{
		HeadlightRight->SetVisibility(bOn);
	}
}

void ABlacklineVanPawn::SetEngine(bool bOn)
{
	bEngineOn = bOn;
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		if (!bOn)
		{
			Move->SetThrottleInput(0.f);
			Move->SetHandbrakeInput(true);
		}
		else
		{
			Move->SetHandbrakeInput(bHandbrakeHeld);
		}
	}
}

void ABlacklineVanPawn::NotifyPursuitEngineAndLights() const
{
	if (UPursuitDirectorSubsystem* Pursuit = UPursuitDirectorSubsystem::Get(this))
	{
		Pursuit->ReportEngineAndLights(bEngineOn, bHeadlightsOn);
	}
}

void ABlacklineVanPawn::OpenWindowForInspection(bool bOpen)
{
	if (bWindowOpen == bOpen)
	{
		return;
	}
	bWindowOpen = bOpen;
	BroadcastDash(EDashControl::Window, bOpen);
}

// ---------------------------------------------------------------- Sospecha y carga

float ABlacklineVanPawn::GetSpeedKmh() const
{
	if (const UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		return FMath::Abs(Move->GetForwardSpeed()) * BlacklineVanPrivate::CmPerSecToKmh;
	}
	return 0.f;
}

void ABlacklineVanPawn::SampleSpeed()
{
	if (SpeedHistoryKmh.Num() == 0)
	{
		return;
	}
	SpeedHistoryKmh[SpeedHistoryHead] = GetSpeedKmh();
	SpeedHistoryHead = (SpeedHistoryHead + 1) % SpeedHistoryKmh.Num();
}

FSuspicionSnapshot ABlacklineVanPawn::GetSuspicionSnapshot() const
{
	FSuspicionSnapshot Snap;

	float MaxSpeed = GetSpeedKmh();
	for (const float S : SpeedHistoryKmh)
	{
		MaxSpeed = FMath::Max(MaxSpeed, S);
	}
	Snap.SpeedBeforeStopKmh = MaxSpeed;
	Snap.bHeadlightsOn = bHeadlightsOn;
	Snap.DamageRatio01 = DamageRatio01;
	Snap.bSignalJammerOn = bSignalJammerOn;
	if (Cargo)
	{
		Snap.UnshieldedSignature01 = Cargo->GetUnshieldedSignature01();
		Snap.CargoIntegrity01 = Cargo->GetCargoIntegrity01();
	}
	return Snap;
}

void ABlacklineVanPawn::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const float Mass = HitComp ? HitComp->GetMass() : 0.f;
	if (Mass <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// Impulso (kg*cm/s) / masa = cm/s; /100 = m/s.
	const float ImpactSpeedMs = (NormalImpulse.Size() / Mass) * 0.01f;
	if (ImpactSpeedMs < MinImpactSpeedMs)
	{
		return;
	}

	DamageRatio01 = FMath::Clamp(DamageRatio01 + ImpactSpeedMs * DamagePerImpactMs, 0.f, 1.f);

	if (Cargo)
	{
		const FVector LocalNormal = GetActorTransform().InverseTransformVectorNoScale(Hit.ImpactNormal);
		Cargo->ApplyImpact(ImpactSpeedMs, LocalNormal);
	}
}

void ABlacklineVanPawn::HandleCargoShifted(float Severity01)
{
	UE_LOG(LogBlackline, Log, TEXT("Carga desplazada, severidad %.2f"), Severity01);

	// Empujon lateral en el sentido de la G lateral actual (la carga tira hacia el exterior de la curva).
	if (USkeletalMeshComponent* VanMesh = GetMesh())
	{
		const float Sign = (Cargo && Cargo->GetLateralG() < 0.f) ? 1.f : -1.f;
		VanMesh->AddImpulse(GetActorRightVector() * Sign * Severity01 * CargoShiftImpulseScale);
	}
}

void ABlacklineVanPawn::HandleCargoIntegrityChanged(float Integrity01, float Delta)
{
	if (Delta < 0.f)
	{
		UE_LOG(LogBlackline, Verbose, TEXT("Integridad de carga %.2f (%.2f)"), Integrity01, Delta);
	}
}
