// Blackline: Night Haul - Furgoneta jugable.
#include "Vehicle/BlacklineVanPawn.h"

#include "Blackline.h"
#include "Audio/BlacklineProceduralAudio.h"
#include "World/RoadsidePOI.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Components/StaticMeshComponent.h"
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
	VanMesh->SetCollisionProfileName(TEXT("Vehicle"));
	VanMesh->BodyInstance.bSimulatePhysics = true; // sin esto el chasis queda congelado y Chaos no mueve las ruedas
	VanMesh->BodyInstance.bUseCCD = true;

	// Motor, caja y diferencial por defecto (docs/C_chaos_vehicles.md). Sin curva de torque Chaos desactiva
	// la simulacion mecanica y no crea el diferencial (crash al cambiar el reparto de traccion).
	if (UChaosWheeledVehicleMovementComponent* Move = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent()))
	{
		Move->Mass = 2200.f;

		Move->EngineSetup.MaxTorque = 800.f;
		Move->EngineSetup.MaxRPM = 4800.f;
		Move->EngineSetup.EngineIdleRPM = 800.f;
		Move->EngineSetup.EngineBrakeEffect = 0.08f;
		Move->EngineSetup.EngineRevUpMOI = 0.6f;
		Move->EngineSetup.EngineRevDownRate = 450.f;
		if (FRichCurve* Curve = Move->EngineSetup.TorqueCurve.GetRichCurve())
		{
			Curve->Reset();
			// X = RPM REALES (Chaos evalua la curva con RPM de 0 a MaxRPM, no normalizada), Y = fraccion del par maximo.
			// Con X en 0..1 la curva quedaba en su ultimo valor (0.48) y el motor daba menos de la mitad del par.
			static const float Points[][2] = {
				{0.f, 0.43f}, {0.167f, 0.62f}, {0.25f, 0.81f}, {0.375f, 1.0f},
				{0.52f, 1.0f}, {0.667f, 0.93f}, {0.833f, 0.74f}, {1.0f, 0.48f} };
			for (const float(&P)[2] : Points)
			{
				const FKeyHandle H = Curve->AddKey(P[0] * Move->EngineSetup.MaxRPM, P[1]);
				Curve->SetKeyInterpMode(H, RCIM_Cubic);
			}
		}

		Move->TransmissionSetup.bUseAutomaticGears = true;
		Move->TransmissionSetup.bUseAutoReverse = true;
		Move->TransmissionSetup.ChangeUpRPM = 4200;
		Move->TransmissionSetup.ChangeDownRPM = 1600;
		Move->TransmissionSetup.GearChangeTime = 0.2f;
		Move->TransmissionSetup.TransmissionEfficiency = 0.9f;
		Move->TransmissionSetup.FinalRatio = 3.5f;
		Move->TransmissionSetup.ForwardGearRatios = { 3.6f, 2.2f, 1.45f, 1.0f, 0.78f };
		Move->TransmissionSetup.ReverseGearRatios = { 3.2f };

		Move->DifferentialSetup.DifferentialType = EVehicleDifferential::AllWheelDrive;
		Move->DifferentialSetup.FrontRearSplit = 0.4f;
	}

	// Partes visuales sin colision (la fisica la lleva el esqueleto y el Physics Asset).
	auto MakeVisual = [this, VanMesh](const TCHAR* Name, FName Socket) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* C = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		C->SetupAttachment(VanMesh, Socket);
		C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		C->SetGenerateOverlapEvents(false);
		C->bCastDynamicShadow = true;
		return C;
	};
	BodyVisual = MakeVisual(TEXT("BodyVisual"), NAME_None);
	WheelVisualFL = MakeVisual(TEXT("WheelVisualFL"), TEXT("VisWheel_FL"));
	WheelVisualFR = MakeVisual(TEXT("WheelVisualFR"), TEXT("VisWheel_FR"));
	WheelVisualRL = MakeVisual(TEXT("WheelVisualRL"), TEXT("VisWheel_BL"));
	WheelVisualRR = MakeVisual(TEXT("WheelVisualRR"), TEXT("VisWheel_BR"));

	Cargo = CreateDefaultSubobject<UCargoSimulationComponent>(TEXT("Cargo"));
	ProceduralAudio = CreateDefaultSubobject<UBlacklineProceduralAudio>(TEXT("ProceduralAudio"));

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
	// El brazo chocaba con el propio chasis (perfil Vehicle) y se encogia a 0: la camara quedaba dentro del furgon.
	ChaseArm->bDoCollisionTest = false;
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
	HeadlightLeft->SetCastShadows(false); // las sombras del chasis daban manchas negras con borde de sierra
	HeadlightLeft->AttenuationRadius = 3000.f;

	HeadlightRight = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadlightRight"));
	HeadlightRight->SetupAttachment(VanMesh);
	HeadlightRight->SetRelativeLocation(FVector(230.f, 70.f, 70.f));
	HeadlightRight->OuterConeAngle = 40.f;
	HeadlightRight->Intensity = 8000.f;
	HeadlightRight->SetCastShadows(false);
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
	Fuel01 = StartFuel01;
	bHeadlightsOn = bStartHeadlightsOn;
	SetHeadlights(bHeadlightsOn);

	TargetPerspective = StartPerspective;
	CameraBlendAlpha = (StartPerspective == ECameraPerspective::ThirdPerson) ? 1.f : 0.f;

	CacheBaseWheelFriction();
	DriveMode = StartDriveMode;
	ApplyDriveConfig();

	if (USkeletalMeshComponent* VanMesh = GetMesh())
	{
		if (!VanMesh->IsSimulatingPhysics())
		{
			UE_LOG(LogBlackline, Warning, TEXT("Chasis sin simulacion de fisica (physics asset=%s): se fuerza SetSimulatePhysics(true)."),
				VanMesh->GetPhysicsAsset() ? *VanMesh->GetPhysicsAsset()->GetName() : TEXT("NINGUNO"));
			VanMesh->SetSimulatePhysics(true);
		}
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
		UE_LOG(LogBlackline, Log, TEXT("Input: RegisterIMC pendiente (controller=%s, IMC=%s)"),
			PC ? TEXT("si") : TEXT("no"), DefaultIMC ? TEXT("si") : TEXT("NO ASIGNADO"));
		return;
	}
	if (ULocalPlayer* LP = PC->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Sub->AddMappingContext(DefaultIMC, IMCPriority);
			IMCController = PC;
			UE_LOG(LogBlackline, Log, TEXT("Input: IMC %s registrado para %s"), *DefaultIMC->GetName(), *PC->GetName());
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

	UE_LOG(LogBlackline, Log, TEXT("Input: SetupPlayerInputComponent (Throttle=%s Steer=%s Brake=%s Camera=%s)"),
		IA_Throttle ? TEXT("ok") : TEXT("NULL"), IA_Steer ? TEXT("ok") : TEXT("NULL"),
		IA_Brake ? TEXT("ok") : TEXT("NULL"), IA_ToggleCamera ? TEXT("ok") : TEXT("NULL"));

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
	if (IA_ToggleHeadlights)
	{
		EIC->BindAction(IA_ToggleHeadlights, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnToggleHeadlightsInput);
	}
	if (IA_ToggleEngine)
	{
		EIC->BindAction(IA_ToggleEngine, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnToggleEngineInput);
	}
	if (IA_ToggleJammer)
	{
		EIC->BindAction(IA_ToggleJammer, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnToggleJammerInput);
	}
	if (IA_Recover)
	{
		EIC->BindAction(IA_Recover, ETriggerEvent::Started, this, &ABlacklineVanPawn::OnRecoverInput);
	}
}

void ABlacklineVanPawn::OnThrottle(const FInputActionValue& Value)
{
	if (UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
	{
		// Motor apagado: no hay traccion.
		LastThrottleInput = Value.Get<float>();
		Move->SetThrottleInput((bEngineOn && Fuel01 > 0.f) ? FMath::Clamp(Value.Get<float>(), 0.f, 1.f) : 0.f);
		UE_LOG(LogBlackline, Verbose, TEXT("Input: Throttle=%.2f motor=%d"), Value.Get<float>(), bEngineOn ? 1 : 0);
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

void ABlacklineVanPawn::OnToggleHeadlightsInput(const FInputActionValue&)
{
	ActivateDashControl(EDashControl::Headlights);
	ShowHudMessage(bHeadlightsOn ? NSLOCTEXT("Blackline", "LightsOn", "Luces encendidas") : NSLOCTEXT("Blackline", "LightsOff", "Luces apagadas (te ven menos)"), 2.f);
}

void ABlacklineVanPawn::OnToggleEngineInput(const FInputActionValue&)
{
	ActivateDashControl(EDashControl::Ignition);
	ShowHudMessage(bEngineOn ? NSLOCTEXT("Blackline", "EngineOn", "Motor encendido") : NSLOCTEXT("Blackline", "EngineOff", "Motor apagado (te oyen menos)"), 2.f);
}

void ABlacklineVanPawn::OnToggleJammerInput(const FInputActionValue&)
{
	ActivateDashControl(EDashControl::SignalJammer);
	ShowHudMessage(bSignalJammerOn ? NSLOCTEXT("Blackline", "JamOn", "Inhibidor GPS activo") : NSLOCTEXT("Blackline", "JamOff", "Inhibidor GPS apagado"), 2.f);
}

void ABlacklineVanPawn::OnRecoverInput(const FInputActionValue&)
{
	OnVanFlipped.Broadcast(); // la ruta lo devuelve a la carretera (misma logica que al volcar)
}

void ABlacklineVanPawn::OnInteract(const FInputActionValue&)
{
	// Primero: si el jugador esta en la zona de una bencinera/restaurante, E / Circulo interactua con el lugar.
	if (ARoadsidePOI* POI = ActivePOI.Get())
	{
		POI->TryInteract(this);
		return;
	}

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

	// Sin curva de torque Chaos no crea el diferencial y SetDifferentialFrontRearSplit revienta (aserto).
	if (Move->EngineSetup.TorqueCurve.GetRichCurveConst()->IsEmpty())
	{
		UE_LOG(LogBlackline, Warning, TEXT("ApplyDriveConfig: el motor no tiene curva de torque; no se aplica el reparto 2WD/4WD."));
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

float ABlacklineVanPawn::GetEngineRpm() const
{
	if (!bEngineOn)
	{
		return 0.f;
	}
	const UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement();
	return Move ? Move->GetEngineRotationSpeed() : 0.f;
}

float ABlacklineVanPawn::GetEngineRpm01() const
{
	const UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement();
	if (!Move || !bEngineOn)
	{
		return 0.f;
	}
	const float MaxRpm = FMath::Max(1.f, Move->GetEngineMaxRotationSpeed());
	return FMath::Clamp(Move->GetEngineRotationSpeed() / MaxRpm, 0.f, 1.f);
}

float ABlacklineVanPawn::GetStealth01() const
{
	// Pesos simples: el motor apagado y las luces apagadas son lo que mas enfria el rastro.
	float Stealth = 0.f;
	Stealth += bEngineOn ? 0.f : 0.45f;
	Stealth += bHeadlightsOn ? 0.f : 0.25f;
	Stealth += bSignalJammerOn ? 0.15f : 0.f;
	Stealth += 0.15f * (1.f - FMath::Clamp(GetSpeedKmh() / 80.f, 0.f, 1.f));
	return FMath::Clamp(Stealth, 0.f, 1.f);
}

void ABlacklineVanPawn::SampleSpeed()
{
	if (SpeedHistoryKmh.Num() == 0)
	{
		return;
	}
	SpeedHistoryKmh[SpeedHistoryHead] = GetSpeedKmh();
	SpeedHistoryHead = (SpeedHistoryHead + 1) % SpeedHistoryKmh.Num();

	// Volcado: de lado o patas arriba y casi parado por FlipResetSeconds. La velocidad es la del cuerpo (no la "hacia adelante",
	// que oscila con las ruedas girando en el aire) y el contador baja de a poco en vez de reiniciarse con una lectura mala.
	const float UpZ = static_cast<float>(GetActorUpVector().Z);
	const float BodySpeedKmh = static_cast<float>(GetVelocity().Size()) * BlacklineVanPrivate::CmPerSecToKmh;
	if (UpZ < FlipUpDotThreshold && BodySpeedKmh < 40.f)
	{
		FlippedSeconds += SpeedSampleIntervalSec;
		if (FlippedSeconds >= FlipResetSeconds)
		{
			FlippedSeconds = 0.f;
			OnVanFlipped.Broadcast();
		}
	}
	else
	{
		FlippedSeconds = FMath::Max(0.f, FlippedSeconds - SpeedSampleIntervalSec * 0.5f);
	}
	if (FlippedSeconds > 0.f && UpZ < FlipUpDotThreshold)
	{
		ShowHudMessage(FText::Format(NSLOCTEXT("Blackline", "Flipped", "VOLCASTE - recuperando en {0} s"),
			FText::AsNumber(FMath::Max(1, FMath::CeilToInt(FlipResetSeconds - FlippedSeconds)))), 0.6f);
	}

	// Consumo de combustible por distancia recorrida.
	if (bEngineOn && Fuel01 > 0.f)
	{
		const float KmDriven = SpeedHistoryKmh[(SpeedHistoryHead + SpeedHistoryKmh.Num() - 1) % SpeedHistoryKmh.Num()] * SpeedSampleIntervalSec / 3600.f;
		Fuel01 = FMath::Max(0.f, Fuel01 - KmDriven * FuelPerKm);
		if (Fuel01 <= 0.f && !bOutOfFuelWarned)
		{
			bOutOfFuelWarned = true;
			ShowHudMessage(NSLOCTEXT("Blackline", "OutOfFuel", "SIN COMBUSTIBLE - busca una bencinera"), 6.f);
		}
	}

	// Telemetria 1 vez por segundo (diagnostico de manejo; barata).
	if (++TelemetryCounter >= FMath::Max(1, FMath::RoundToInt(1.f / FMath::Max(0.05f, SpeedSampleIntervalSec))))
	{
		TelemetryCounter = 0;
		if (const UChaosWheeledVehicleMovementComponent* Move = GetChaosMovement())
		{
			int32 InAir = 0;
			for (const UChaosVehicleWheel* W : Move->Wheels)
			{
				InAir += (W && W->IsInAir()) ? 1 : 0;
			}
			UE_LOG(LogBlackline, Log, TEXT("Telemetria: v=%.1f km/h rpm=%.0f marcha=%d acel=%.2f motor=%d fisica=%d z=%.0f ruedas=%d aire=%d"),
				GetSpeedKmh(), GetEngineRpm(), Move->GetCurrentGear(), LastThrottleInput, bEngineOn ? 1 : 0,
				(GetMesh() && GetMesh()->IsSimulatingPhysics()) ? 1 : 0, GetActorLocation().Z, Move->Wheels.Num(), InAir);
		}
	}
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

// ---------------------------------------------------------------- Combustible, lugares, mensajes

void ABlacklineVanPawn::AddFuel(float Amount01)
{
	Fuel01 = FMath::Clamp(Fuel01 + Amount01, 0.f, 1.f);
	if (Fuel01 > 0.f)
	{
		bOutOfFuelWarned = false;
	}
}

void ABlacklineVanPawn::RepairDamage(float Amount01)
{
	DamageRatio01 = FMath::Clamp(DamageRatio01 - Amount01, 0.f, 1.f);
}

void ABlacklineVanPawn::SetActiveInteractable(ARoadsidePOI* POI)
{
	ActivePOI = POI;
}

void ABlacklineVanPawn::ShowHudMessage(const FText& Text, float Seconds)
{
	HudMessage = Text;
	HudMessageExpire = GetWorld() ? GetWorld()->GetTimeSeconds() + Seconds : 0.0;
}

FText ABlacklineVanPawn::GetHudMessage() const
{
	const UWorld* World = GetWorld();
	if (World && World->GetTimeSeconds() < HudMessageExpire)
	{
		return HudMessage;
	}
	return FText::GetEmpty();
}
