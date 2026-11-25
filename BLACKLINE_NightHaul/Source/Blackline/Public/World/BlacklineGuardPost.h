// Blackline: Night Haul - garita del reten: guardia, caseta y barrera con modelo (formas basicas, sin colision).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlacklineGuardPost.generated.h"

UCLASS()
class BLACKLINE_API ABlacklineGuardPost : public AActor
{
	GENERATED_BODY()

public:
	ABlacklineGuardPost();

	/** Levanta o baja la barrera (visual). */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetBarrierOpen(bool bOpen);

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	bool IsBarrierOpen() const { return bOpen; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> BarrierPivot;

	bool bOpen = false;
};
