// Blackline: Night Haul - patrulla interceptora (placeholder jugable). El subsistema de persecucion la spawnea
// y la guia con AAIController::MoveToActor / MoveToLocation (requiere NavMesh en el mapa).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlacklinePatrolCar.generated.h"

class UStaticMeshComponent;
class USpotLightComponent;
class UPointLightComponent;

UCLASS()
class BLACKLINE_API ABlacklinePatrolCar : public ACharacter
{
	GENERATED_BODY()

public:
	ABlacklinePatrolCar();

	/** Enciende/apaga sirena y luz. */
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	void SetSirenActive(bool bActive);

	UFUNCTION(BlueprintPure, Category = "Patrol")
	bool IsSirenActive() const { return bSirenActive; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	TObjectPtr<USpotLightComponent> Headlight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	TObjectPtr<UPointLightComponent> SirenRed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Patrol")
	TObjectPtr<UPointLightComponent> SirenBlue;

	/** Velocidad maxima de persecucion (cm/s). 2200 = ~79 km/h. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol|Tuning", meta = (ClampMin = "100"))
	float MaxSpeedCmPerSec = 2200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol|Tuning", meta = (ClampMin = "100"))
	float AccelerationCmPerSec2 = 1400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol|Tuning", meta = (ClampMin = "10"))
	float TurnRateDegPerSec = 160.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol|Siren", meta = (ClampMin = "0.05"))
	float SirenFlashIntervalSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol|Siren")
	bool bSirenActive = true;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void FlashSiren();
	void ApplySirenState();

	FTimerHandle SirenTimer;
	bool bRedPhase = true;
};
