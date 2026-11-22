// Blackline: Night Haul - Ajustes de persecucion (Project Settings > Game > Blackline Pursuit).
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PursuitSettings.generated.h"

class APawn;

/** Configuracion por nivel de persecucion (indice 0 = nivel 1). */
USTRUCT(BlueprintType)
struct FPursuitLevelConfig
{
	GENERATED_BODY()

	/** Pawn de patrulla (Blueprint con AIController). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit")
	TSoftClassPtr<APawn> PatrolClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "0"))
	int32 PatrolCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "0", ClampMax = "1"))
	float InitialHeat01 = 1.0f;

	/** Alcance de vision en Hunting (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "0"))
	float SightRangeCm = 4000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "1", ClampMax = "180"))
	float SightHalfAngleDeg = 45.0f;

	/** Alcance del cono de busqueda en Searching (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "0"))
	float SearchRangeCm = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "1", ClampMax = "180"))
	float SearchHalfAngleDeg = 70.0f;

	/** Esfera de oido: motor encendido dentro de este radio impide enfriar y revela la posicion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Pursuit", meta = (ClampMin = "0"))
	float HearingRangeCm = 1500.0f;
};

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Blackline Pursuit"))
class BLACKLINE_API UPursuitSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPursuitSettings();

	virtual FName GetCategoryName() const override { return TEXT("Game"); }

	static const UPursuitSettings* Get() { return GetDefault<UPursuitSettings>(); }

	/** Niveles de persecucion; StartPursuit(Level) usa Levels[Level-1] (recortado al ultimo). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Levels")
	TArray<FPursuitLevelConfig> Levels;

	// ---- Calor ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0.05"))
	float HeatUpdateIntervalSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0"))
	float HeatGainPerSecondVisible = 0.35f;

	/** Enfriamiento base sin linea de vista. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0"))
	float BaseCoolPerSecond = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "1"))
	float EngineOffCoolMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "1"))
	float LightsOffCoolMultiplier = 2.0f;

	/** Con luces encendidas el alcance de vision de las patrullas se multiplica. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "1"))
	float LightsOnSightMultiplier = 1.5f;

	/** Segundos sin ver al objetivo antes de pasar de Hunting a Searching. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0"))
	float LoseSightGraceSeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0", ClampMax = "1"))
	float LostHeatThreshold01 = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Heat", meta = (ClampMin = "0"))
	float LostCleanupDelaySeconds = 5.0f;

	// ---- Spawn ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Spawn", meta = (ClampMin = "0"))
	float SpawnDistanceMinCm = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Spawn", meta = (ClampMin = "0"))
	float SpawnDistanceMaxCm = 9000.0f;

	/** Intentos por patrulla de hallar un punto fuera de la linea de vista del objetivo. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Spawn", meta = (ClampMin = "1"))
	int32 SpawnAttempts = 8;

	/** Mitad de la altura de la traza vertical para hallar el suelo. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Spawn", meta = (ClampMin = "100"))
	float SpawnGroundTraceHalfHeightCm = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Spawn", meta = (ClampMin = "0"))
	float SpawnLiftCm = 100.0f;

	// ---- Ordenes a patrullas ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Orders", meta = (ClampMin = "0.1"))
	float RepathIntervalSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Orders", meta = (ClampMin = "0"))
	float SearchRadiusCm = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Orders", meta = (ClampMin = "0"))
	float AcceptanceRadiusCm = 300.0f;

	/** Canal de traza para linea de vista. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Config, Category = "Orders")
	TEnumAsByte<ECollisionChannel> SightTraceChannel = ECC_Visibility;
};
