#include "Pursuit/BlacklinePatrolCar.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ABlacklinePatrolCar::ABlacklinePatrolCar()
{
	PrimaryActorTick.bCanEverTick = false; // sin Tick: la sirena va por timer

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->InitCapsuleSize(120.f, 90.f);
	if (GetMesh())
	{
		GetMesh()->SetVisibility(false); // el cuerpo visible es BodyMesh
	}

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
	BodyMesh->SetRelativeScale3D(FVector(4.4f, 1.9f, 1.3f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeFinder.Object);
	}

	Headlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Headlight"));
	Headlight->SetupAttachment(BodyMesh);
	Headlight->SetRelativeLocation(FVector(50.f, 0.f, 20.f));
	Headlight->SetIntensity(20000.f);
	Headlight->SetOuterConeAngle(32.f);
	Headlight->SetAttenuationRadius(5000.f);
	Headlight->SetCastShadows(false); // presupuesto: sin sombras en faros

	SirenRed = CreateDefaultSubobject<UPointLightComponent>(TEXT("SirenRed"));
	SirenRed->SetupAttachment(BodyMesh);
	SirenRed->SetRelativeLocation(FVector(0.f, -25.f, 60.f));
	SirenRed->SetLightColor(FLinearColor::Red);
	SirenRed->SetIntensity(4000.f);
	SirenRed->SetAttenuationRadius(1200.f);
	SirenRed->SetCastShadows(false);

	SirenBlue = CreateDefaultSubobject<UPointLightComponent>(TEXT("SirenBlue"));
	SirenBlue->SetupAttachment(BodyMesh);
	SirenBlue->SetRelativeLocation(FVector(0.f, 25.f, 60.f));
	SirenBlue->SetLightColor(FLinearColor::Blue);
	SirenBlue->SetIntensity(4000.f);
	SirenBlue->SetAttenuationRadius(1200.f);
	SirenBlue->SetCastShadows(false);
}

void ABlacklinePatrolCar::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = MaxSpeedCmPerSec;
		Move->MaxAcceleration = AccelerationCmPerSec2;
		Move->BrakingDecelerationWalking = AccelerationCmPerSec2;
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, TurnRateDegPerSec, 0.f);
	}
	bUseControllerRotationYaw = false;

	SetSirenActive(bSirenActive);
}

void ABlacklinePatrolCar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SirenTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void ABlacklinePatrolCar::SetSirenActive(bool bActive)
{
	bSirenActive = bActive;
	if (UWorld* World = GetWorld())
	{
		FTimerManager& TM = World->GetTimerManager();
		TM.ClearTimer(SirenTimer);
		if (bSirenActive)
		{
			TM.SetTimer(SirenTimer, this, &ABlacklinePatrolCar::FlashSiren, FMath::Max(0.05f, SirenFlashIntervalSeconds), true);
		}
	}
	ApplySirenState();
}

void ABlacklinePatrolCar::FlashSiren()
{
	bRedPhase = !bRedPhase;
	ApplySirenState();
}

void ABlacklinePatrolCar::ApplySirenState()
{
	if (SirenRed)
	{
		SirenRed->SetVisibility(bSirenActive && bRedPhase);
	}
	if (SirenBlue)
	{
		SirenBlue->SetVisibility(bSirenActive && !bRedPhase);
	}
}
