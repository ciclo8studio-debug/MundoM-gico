// Blackline: Night Haul - auto de trafico (sentido contrario). Solo geometria y luces: AChileRouteActor lo mueve por la ruta.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrafficCar.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpotLightComponent;
class UPointLightComponent;
class UStaticMesh;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class BLACKLINE_API ATrafficCar : public AActor
{
	GENERATED_BODY()

public:
	ATrafficCar();

	/** Color de carroceria. */
	UFUNCTION(BlueprintCallable, Category = "Traffic")
	void SetBodyColor(const FLinearColor& Color);

	/** Distancia a lo largo de la ruta (cm) y velocidad (cm/s); los gestiona AChileRouteActor. */
	float RouteDistanceCm = 0.f;
	float SpeedCmPerSec = 2000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Traffic")
	TObjectPtr<UBoxComponent> Body;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Shell;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cabin;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent> HeadLeft;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpotLightComponent> HeadRight;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> Tail;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> ShellMaterial;
};
