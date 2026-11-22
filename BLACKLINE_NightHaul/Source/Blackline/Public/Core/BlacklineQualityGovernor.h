// Blackline: Night Haul - Gobernador de calidad adaptativa (resolucion dinamica por tiempo de frame).
// Sin Tick: un timer de baja frecuencia mide frames/segundo y ajusta r.ScreenPercentage con histeresis.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Subsystems/WorldSubsystem.h"
#include "BlacklineQualityGovernor.generated.h"

/** Project Settings > Game > Blackline Quality. */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Blackline Quality"))
class BLACKLINE_API UBlacklineQualitySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
	static const UBlacklineQualitySettings* Get() { return GetDefault<UBlacklineQualitySettings>(); }

	/** Interruptor general de la resolucion adaptativa. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive")
	bool bEnableAdaptiveResolution = true;

	/** FPS objetivo. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "20", ClampMax = "120"))
	float TargetFps = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "40", ClampMax = "100"))
	int32 MinScreenPercentage = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "40", ClampMax = "100"))
	int32 MaxScreenPercentage = 90;

	/** Paso por ajuste (puntos porcentuales). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "1", ClampMax = "20"))
	int32 StepPercent = 5;

	/** Segundos entre mediciones. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "0.25", ClampMax = "5"))
	float SampleIntervalSec = 1.0f;

	/** Mediciones consecutivas con holgura antes de SUBIR resolucion (bajar es inmediato). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Adaptive", meta = (ClampMin = "1", ClampMax = "30"))
	int32 SamplesBeforeRaise = 5;
};

UCLASS()
class BLACKLINE_API UBlacklineQualityGovernor : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;

	/** Porcentaje de pantalla actual aplicado por el gobernador. */
	UFUNCTION(BlueprintPure, Category = "Blackline|Quality")
	int32 GetCurrentScreenPercentage() const { return CurrentPercent; }

	/** Ultimo FPS medido. */
	UFUNCTION(BlueprintPure, Category = "Blackline|Quality")
	float GetMeasuredFps() const { return MeasuredFps; }

private:
	void Sample();
	void Apply(int32 NewPercent);

	FTimerHandle SampleTimer;
	uint64 LastFrameCounter = 0;
	double LastTimeSec = 0.0;
	int32 CurrentPercent = 100;
	int32 HeadroomStreak = 0;
	float MeasuredFps = 0.f;
};
