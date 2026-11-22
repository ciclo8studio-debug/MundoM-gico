// Blackline: Night Haul - Rueda trasera (motriz, freno de mano)
#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "BlacklineWheelRear.generated.h"

/**
 * Rueda trasera del furgon 4x4: no directriz, motriz, con freno de servicio y de mano.
 * Asignar en el WheelSetups del movement component del pawn (ver docs/C_chaos_vehicles.md).
 */
UCLASS(BlueprintType, Blueprintable)
class BLACKLINE_API UBlacklineWheelRear : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:
	UBlacklineWheelRear();
};
