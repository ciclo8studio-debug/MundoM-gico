#include "Vehicle/BlacklineCabinFX.h"
#include "Blackline.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Contracts/WeatherDirector.h"
#include "Core/BlacklineShapes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"

// ============================ Cabina ============================

UBlacklineCabinComponent::UBlacklineCabinComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlacklineCabinComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (!bEnabled || !Owner || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}
	using namespace BlacklineShapes;
	USceneComponent* Root = Owner->GetRootComponent();

	// Posiciones relativas al furgon; el conductor va a la izquierda (Y negativo).
	Add(Owner, Root, EShape::Cube, FVector(78.f, 0.f, 96.f), FRotator::ZeroRotator, FVector(0.38f, 1.6f, 0.2f), FLinearColor(0.02f, 0.02f, 0.025f));      // tablero
	Add(Owner, Root, EShape::Cylinder, FVector(50.f, -35.f, 104.f), FRotator(-62.f, 0.f, 0.f), FVector(0.38f, 0.38f, 0.035f), FLinearColor(0.03f, 0.03f, 0.03f)); // volante
	Add(Owner, Root, EShape::Cylinder, FVector(62.f, 0.f, 136.f), FRotator::ZeroRotator, FVector(0.1f, 0.1f, 0.012f), FLinearColor(0.75f, 0.05f, 0.05f));     // collar de Pancito (colgado del espejo)
	Add(Owner, Root, EShape::Cube, FVector(62.f, 0.f, 144.f), FRotator::ZeroRotator, FVector(0.008f, 0.008f, 0.14f), FLinearColor(0.1f, 0.1f, 0.1f));        // cordon del espejo
	Add(Owner, Root, EShape::Sphere, FVector(62.f, 0.f, 128.f), FRotator::ZeroRotator, FVector(0.045f), FLinearColor(0.8f, 0.65f, 0.2f));                    // chapita del collar
	Add(Owner, Root, EShape::Sphere, FVector(18.f, 38.f, 70.f), FRotator::ZeroRotator, FVector(0.13f), FLinearColor(0.8f, 0.85f, 0.1f));                     // pelota en el asiento del copiloto
	Add(Owner, Root, EShape::Cube, FVector(72.f, 22.f, 112.f), FRotator(-20.f, 0.f, 0.f), FVector(0.01f, 0.16f, 0.11f), FLinearColor(0.85f, 0.8f, 0.65f));   // foto de Pancito sobre el tablero
}

// ============================ Lluvia ============================

UBlacklineRainComponent::UBlacklineRainComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBlacklineRainComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!bEnabled || !Owner || !World || !World->IsGameWorld())
	{
		return;
	}

	UStaticMesh* Mesh = BlacklineShapes::LoadShape(BlacklineShapes::EShape::Cube);
	if (!Mesh)
	{
		return;
	}
	Streaks = NewObject<UInstancedStaticMeshComponent>(Owner);
	Streaks->SetStaticMesh(Mesh);
	Streaks->SetMobility(EComponentMobility::Movable);
	Streaks->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Streaks->SetCastShadow(false);
	Streaks->SetUsingAbsoluteLocation(true);
	Streaks->SetUsingAbsoluteRotation(true);
	Streaks->SetUsingAbsoluteScale(true);
	Streaks->SetupAttachment(Owner->GetRootComponent());
	Streaks->RegisterComponent();
	Streaks->SetVisibility(false);
	if (UMaterialInterface* Base = Streaks->GetMaterial(0))
	{
		if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Streaks))
		{
			MID->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.55f, 0.65f, 0.8f));
			MID->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.55f, 0.65f, 0.8f));
			Streaks->SetMaterial(0, MID);
		}
	}

	Offsets.SetNum(StreakCount);
	Scratch.SetNum(StreakCount);
	for (int32 i = 0; i < StreakCount; ++i)
	{
		Offsets[i] = FVector(FMath::FRandRange(-HalfWidthCm, HalfWidthCm), FMath::FRandRange(-HalfWidthCm, HalfWidthCm), FMath::FRandRange(0.f, HeightCm));
		Streaks->AddInstance(FTransform::Identity, false);
	}

	World->GetTimerManager().SetTimer(WeatherTimer, this, &UBlacklineRainComponent::CheckWeather, 0.5f, true, 0.2f);
}

void UBlacklineRainComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WeatherTimer);
		World->GetTimerManager().ClearTimer(StepTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void UBlacklineRainComponent::CheckWeather()
{
	UWorld* World = GetWorld();
	if (!World || !Streaks)
	{
		return;
	}
	float Target = 0.f;
	if (const AWeatherDirector* Weather = Cast<AWeatherDirector>(UGameplayStatics::GetActorOfClass(this, AWeatherDirector::StaticClass())))
	{
		Target = Weather->GetPreset(Weather->GetCurrentWeather()).RainIntensity01;
	}
	Intensity = Target;
	const bool bRaining = Intensity > 0.05f;
	Streaks->SetVisibility(bRaining);
	FTimerManager& TM = World->GetTimerManager();
	if (bRaining && !TM.IsTimerActive(StepTimer))
	{
		TM.SetTimer(StepTimer, this, &UBlacklineRainComponent::StepRain, StepSeconds, true);
	}
	else if (!bRaining && TM.IsTimerActive(StepTimer))
	{
		TM.ClearTimer(StepTimer);
	}
}

void UBlacklineRainComponent::StepRain()
{
	const AActor* Owner = GetOwner();
	if (!Owner || !Streaks || Offsets.Num() != Scratch.Num())
	{
		return;
	}
	const FVector Center = Owner->GetActorLocation() - FVector(0.f, 0.f, 300.f);
	const float Fall = FallSpeedCmPerSec * StepSeconds * (0.7f + 0.6f * Intensity);
	const FVector Scale(0.012f, 0.012f, 0.35f + 0.55f * Intensity);
	const FRotator Slant(0.f, 0.f, 6.f + 10.f * Intensity); // el viento las inclina un poco
	for (int32 i = 0; i < Offsets.Num(); ++i)
	{
		Offsets[i].Z -= Fall;
		if (Offsets[i].Z < 0.f)
		{
			Offsets[i].Z += HeightCm;
		}
		Scratch[i] = FTransform(Slant, Center + Offsets[i], Scale);
	}
	Streaks->BatchUpdateInstancesTransforms(0, Scratch, true, true, true);
}
