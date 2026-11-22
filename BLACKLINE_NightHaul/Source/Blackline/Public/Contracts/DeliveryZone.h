// Blackline: Night Haul - zona de entrega: cierra el contrato activo cuando el furgon llega y se detiene.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeliveryZone.generated.h"

class UBoxComponent;
class ABlacklineVanPawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDelivered, FName, ContractID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeliveryBlocked, FText, Reason);

UCLASS()
class BLACKLINE_API ADeliveryZone : public AActor
{
	GENERATED_BODY()

public:
	ADeliveryZone();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Delivery")
	TObjectPtr<UBoxComponent> Zone;

	/** Solo acepta este contrato (NAME_None = cualquiera). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	FName RequiredContractId = NAME_None;

	/** Exige detenerse dentro de la zona. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	bool bRequireStop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery", meta = (ClampMin = "0", Units = "km/h"))
	float StopSpeedKmh = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery", meta = (ClampMin = "0", Units = "s"))
	float StopHoldSeconds = 1.5f;

	/** No acepta la entrega mientras haya persecucion activa (la carga esta "caliente"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery")
	bool bBlockDuringPursuit = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Delivery", meta = (ClampMin = "0.05", Units = "s"))
	float PollIntervalSeconds = 0.25f;

	UPROPERTY(BlueprintAssignable, Category = "Delivery")
	FOnDelivered OnDelivered;

	UPROPERTY(BlueprintAssignable, Category = "Delivery")
	FOnDeliveryBlocked OnDeliveryBlocked;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Poll();
	void StopPolling();

	UPROPERTY(Transient)
	TObjectPtr<ABlacklineVanPawn> Van;

	FTimerHandle PollTimer;
	float StoppedSeconds = 0.f;
	float LastBlockedMessageTime = -100.f;
};
