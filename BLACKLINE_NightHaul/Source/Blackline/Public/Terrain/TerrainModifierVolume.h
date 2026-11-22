// Blackline: Night Haul - Volumen que modifica la superficie (barro, grava, etc.)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BlacklineTypes.h"
#include "TerrainModifierVolume.generated.h"

class UBoxComponent;
class ABlacklineVanPawn;
class UPrimitiveComponent;

/**
 * Volumen de terreno. Al entrar el furgon llama PushSurfaceModifier(this); al salir, PopSurfaceModifier(this).
 * El furgon decide como aplicar FrictionMultiplier / hundimiento (ver docs/C_chaos_vehicles.md).
 * Sin Tick. No thread-safe: solo hilo de juego.
 *
 * Uso: arrastrar al nivel, escalar Zone, Surface=Mud, FrictionMultiplier=0.45, bRequires4WD=true.
 */
UCLASS(BlueprintType, Blueprintable)
class BLACKLINE_API ATerrainModifierVolume : public AActor
{
	GENERATED_BODY()

public:
	ATerrainModifierVolume();

	/** Zona de overlap. Escalar en el editor. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain")
	TObjectPtr<UBoxComponent> Zone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	ESurfaceKind Surface = ESurfaceKind::Mud;

	/** Multiplicador de friccion de neumatico en esta zona (1 = sin cambio). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain", meta = (ClampMin = "0.05", ClampMax = "2.0"))
	float FrictionMultiplier = 0.45f;

	/** Si true, sin traccion 4x4 / bloqueo el furgon queda atascado (lo evalua el pawn). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain")
	bool bRequires4WD = true;

	/** Profundidad de hundimiento (cm) usada para atascar el vehiculo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain", meta = (ClampMin = "0.0", ClampMax = "100.0", Units = "cm"))
	float StuckSinkDepthCm = 15.f;

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Furgones actualmente dentro (para limpiar al destruir el volumen). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<ABlacklineVanPawn>> OverlappingVans;
};
