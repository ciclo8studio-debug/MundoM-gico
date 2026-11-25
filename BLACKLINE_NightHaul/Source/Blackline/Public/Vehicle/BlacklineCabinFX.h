// Blackline: Night Haul - cabina propia (tablero, volante, collar, pelota y foto de Pancito) y lluvia visible.
// Todo con formas basicas del motor; sin Event Tick (la lluvia usa un timer solo mientras llueve).
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlacklineCabinFX.generated.h"

class UInstancedStaticMeshComponent;

/** Objetos de la cabina: lo que le recuerda a Enzo por que maneja. Se ven con la camara de cabina (V). */
UCLASS(ClassGroup = (Blackline), meta = (BlueprintSpawnableComponent))
class BLACKLINE_API UBlacklineCabinComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlacklineCabinComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cabin")
	bool bEnabled = true;

protected:
	virtual void BeginPlay() override;
};

/** Lluvia: rayas finas que acompanan al furgon. Se enciende segun el clima del contrato. */
UCLASS(ClassGroup = (Blackline), meta = (BlueprintSpawnableComponent))
class BLACKLINE_API UBlacklineRainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlacklineRainComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain")
	bool bEnabled = true;

	/** Cantidad de rayas (costo de GPU bajo, pero sube con el numero). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain", meta = (ClampMin = "20", ClampMax = "600"))
	int32 StreakCount = 160;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain", meta = (Units = "cm"))
	float HalfWidthCm = 1400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain", meta = (Units = "cm"))
	float HeightCm = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rain", meta = (Units = "cm/s"))
	float FallSpeedCmPerSec = 2800.f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void CheckWeather();
	void StepRain();

	UPROPERTY(Transient)
	TObjectPtr<UInstancedStaticMeshComponent> Streaks;

	TArray<FVector> Offsets;
	TArray<FTransform> Scratch;
	FTimerHandle WeatherTimer;
	FTimerHandle StepTimer;
	float Intensity = 0.f;
	static constexpr float StepSeconds = 0.05f;
};
