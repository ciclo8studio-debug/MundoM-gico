// Blackline: Night Haul - Rueda delantera (directriz, motriz 4x4, freno)
#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "BlacklineWheelFront.generated.h"

/**
 * Rueda delantera del furgon 4x4: directriz, con traccion y freno de servicio, sin freno de mano.
 * Asignar en el WheelSetups del movement component del pawn (ver docs/C_chaos_vehicles.md).
 * Valores ajustables tambien en el BP derivado.
 */
UCLASS(BlueprintType, Blueprintable)
class BLACKLINE_API UBlacklineWheelFront : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:
	UBlacklineWheelFront();
};
