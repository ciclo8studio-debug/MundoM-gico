// Blackline: Night Haul - tipos de la ruta de Chile comprimida (waypoints de ciudades reales).
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ChileRouteTypes.generated.h"

/** Bioma del tramo: define colores y relieve del terreno generado. Coincide con la columna Biome del CSV. */
UENUM(BlueprintType)
enum class EChileBiome : uint8
{
	DesertCoast,
	DesertInterior,
	Semiarid,
	CentralValley,
	HumidSouth,
	Cordillera
};

/** Fila de DT_ChileWaypoints. El nombre de fila ordena el tramo (Z1_01, Z1_02, ...). */
USTRUCT(BlueprintType)
struct BLACKLINE_API FChileWaypointRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	FText Name;

	/** Zona jugable: 1 Norte, 2 Centro, 3 Sur, 4 Cordillera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint", meta = (ClampMin = "1"))
	int32 ZoneId = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float LatDeg = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	float LonDeg = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint", meta = (Units = "m"))
	float ElevationM = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waypoint")
	EChileBiome Biome = EChileBiome::CentralValley;
};
