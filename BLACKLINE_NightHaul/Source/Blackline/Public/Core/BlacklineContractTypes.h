// Blackline: Night Haul - tipos de contratos (encargos). DataTable: Content/Data/DT_BlacklineContracts.
// Nota: EBlacklineDriveMode / EBlacklineCheckpointState ya existen como EDriveMode / ECheckpointState
// (Core/BlacklineTypes.h); no se duplican para no romper el codigo ya integrado.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BlacklineContractTypes.generated.h"

/** Clima del encargo. Los nombres deben coincidir con la columna WeatherType del CSV. */
UENUM(BlueprintType)
enum class EBlacklineWeather : uint8
{
	ClearNight,
	ColdNight,
	HumidNight,
	LightRain,
	ModerateRain,
	HeavyRain,
	Storm,
	Windy,
	UrbanFog,
	CoastalFog,
	PortFog,
	DenseFog,
	Freezing,
	Blizzard
};

/** Tipo de terreno predominante del encargo. Coincide con la columna TerrainType del CSV. */
UENUM(BlueprintType)
enum class EBlacklineTerrainType : uint8
{
	Asphalt,
	DirtBypass,
	DirtRoad,
	DeepMud,
	Gravel,
	WetSlope,
	Dockyard,
	MountainRoad,
	IceSnow,
	OldTunnel,
	MixedOffroad
};

/** Fila de DT_BlacklineContracts. El nombre de fila es el ContractID (C01..C25). */
USTRUCT(BlueprintType)
struct BLACKLINE_API FBlacklineContractRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FName ContractID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FText Title;

	/** Arco narrativo 1..5 (un broker por arco). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "1", ClampMax = "5"))
	int32 Arc = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FText BrokerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FText CargoDisplayName;

	/** Historia de cobertura que declara el manifiesto falso. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FText CoverStoryManifest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	int32 Payout = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "0", Units = "kg"))
	float CargoWeightKg = 100.f;

	/** 0 = robusta, 1 = extremadamente fragil. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "0", ClampMax = "1"))
	float Fragility = 0.2f;

	/** Firma EM/termica de la carga [0,1]; sube la sospecha si no hay blindaje/inhibidor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "0", ClampMax = "1"))
	float EMSignature = 0.f;

	/** Nivel de documento falso exigido en retenes (1-3). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "1", ClampMax = "3"))
	int32 RequiredManifestLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	EBlacklineWeather WeatherType = EBlacklineWeather::ClearNight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	EBlacklineTerrainType TerrainType = EBlacklineTerrainType::Asphalt;

	/** Asimetria de la carga: -1 (todo a la izquierda) .. 1 (todo a la derecha). 0 = centrada. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract", meta = (ClampMin = "-1", ClampMax = "1"))
	float LateralBias = 0.f;

	/** Mecanica destacada del encargo (nota de diseno / pista para el HUD de la tablet). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contract")
	FText DesignNote;
};
