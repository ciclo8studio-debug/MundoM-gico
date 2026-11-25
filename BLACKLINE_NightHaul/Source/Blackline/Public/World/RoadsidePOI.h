// Blackline: Night Haul - lugares junto a la ruta (bencinera, restaurante). Se arman con cubos del motor, luces y letreros;
// el jugador se detiene en la explanada y usa E / Circulo para interactuar.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadsidePOI.generated.h"

class UBoxComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UMaterialInterface;
class ABlacklineVanPawn;

UENUM(BlueprintType)
enum class EPoiKind : uint8
{
	GasStation,
	Restaurant
};

UCLASS()
class BLACKLINE_API ARoadsidePOI : public AActor
{
	GENERATED_BODY()

public:
	ARoadsidePOI();

	/** Construye la geometria, luces y letrero del tipo de lugar. Llamar una vez tras el spawn. */
	UFUNCTION(BlueprintCallable, Category = "POI")
	void BuildFor(EPoiKind InKind);

	/** Interaccion (la llama el furgon con E / Circulo). Exige ir casi detenido. */
	UFUNCTION(BlueprintCallable, Category = "POI")
	void TryInteract(ABlacklineVanPawn* Van);

	UFUNCTION(BlueprintPure, Category = "POI")
	EPoiKind GetKind() const { return Kind; }

	/** Color del marcador en el minimapa. */
	UFUNCTION(BlueprintPure, Category = "POI")
	FLinearColor GetMapColor() const;

	/** Para efectos o sonidos en un Blueprint hijo. */
	UFUNCTION(BlueprintImplementableEvent, Category = "POI")
	void BP_OnInteracted(ABlacklineVanPawn* Van);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "POI")
	TObjectPtr<UBoxComponent> Zone;

	/** Velocidad maxima para poder interactuar (km/h). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POI", meta = (ClampMin = "0"))
	float MaxInteractSpeedKmh = 6.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POI", meta = (ClampMin = "0"))
	float InteractCooldownSeconds = 2.f;

	/** Frecuencia con la que se revisa si el furgon esta en la explanada. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POI", meta = (ClampMin = "0.05"))
	float PollIntervalSeconds = 0.25f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void Poll();
	UStaticMeshComponent* AddBox(const FVector& PosCm, const FVector& SizeCm, const FLinearColor& Color, bool bCollide);
	void AddLight(const FVector& PosCm, const FLinearColor& Color, float Intensity, float RadiusCm);
	void AddSign(const FVector& PosCm, const FString& Text, const FColor& Color, float WorldSize);

	UPROPERTY()
	TObjectPtr<UStaticMesh> Cube;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(Transient)
	TWeakObjectPtr<ABlacklineVanPawn> TrackedVan;

	EPoiKind Kind = EPoiKind::GasStation;
	FTimerHandle PollTimer;
	double LastInteractTime = -100.0;
	bool bVanInside = false;
};
