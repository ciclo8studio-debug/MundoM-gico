// Blackline: Night Haul - genera una zona de la ruta de Chile comprimida: ruta (spline) desde waypoints reales,
// calzada con lineas, y terreno procedural alrededor (colinas por bioma, plano junto al camino).
// Todo en coordenadas locales del actor: cada zona nace cerca del origen (sin problemas de precision lejos del 0,0).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/ChileRouteTypes.h"
#include "ChileRouteActor.generated.h"

class USplineComponent;
class UProceduralMeshComponent;
class UInstancedStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UDataTable;

/** Estilo del terreno por bioma. */
USTRUCT(BlueprintType)
struct FChileBiomeStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome")
	FLinearColor Color = FLinearColor(0.2f, 0.2f, 0.2f, 1.f);

	/** Altura maxima de las colinas (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome", meta = (ClampMin = "0"))
	float HillAmplitudeCm = 5000.f;

	/** Pendiente adicional al alejarse del camino (cm de altura por cm de distancia). Valles encajonados = alto. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Biome", meta = (ClampMin = "0"))
	float RampSlope = 0.05f;
};

UCLASS()
class BLACKLINE_API AChileRouteActor : public AActor
{
	GENERATED_BODY()

public:
	AChileRouteActor();

	/** Genera la zona ZoneId (borra lo anterior). Tambien en el editor (boton del Details). */
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Chile")
	void BuildZone();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Chile")
	void ClearZone();

	/** Inicio de la ruta en el carril derecho, orientado a lo largo del camino (espacio mundo). */
	UFUNCTION(BlueprintPure, Category = "Chile")
	FTransform GetStartTransform() const;

	/** Final de la ruta (espacio mundo). */
	UFUNCTION(BlueprintPure, Category = "Chile")
	FTransform GetEndTransform() const;

	UFUNCTION(BlueprintPure, Category = "Chile")
	float GetRouteLengthKm() const { return RouteLengthCm / 100000.f; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chile")
	TObjectPtr<USplineComponent> Route;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chile")
	TObjectPtr<UProceduralMeshComponent> RoadMesh;

	// ---- Datos ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	TSoftObjectPtr<UDataTable> WaypointTable;

	/** Zona a generar: 1 Norte, 2 Centro, 3 Sur, 4 Cordillera. La linea de comandos -zone=N la sobreescribe. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data", meta = (ClampMin = "1"))
	int32 ZoneId = 2;

	/** Compresion por zona (100 = 1:100). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	TMap<int32, float> ZoneCompression;

	/** Material con Vertex Color en Base Color (M_VertexColor). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	TSoftObjectPtr<UMaterialInterface> RouteMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	bool bBuildOnBeginPlay = true;

	/** Lleva al jugador al inicio de la ruta al terminar de generar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	bool bTeleportPlayerToStart = false;

	/** Crea la zona de entrega al final de la ruta. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Data")
	bool bSpawnDeliveryZoneAtEnd = true;

	// ---- Forma ----
	/** Escala vertical respecto a la altura real (la horizontal va comprimida; 0.08 = 3000 m -> 240 m). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (ClampMin = "0"))
	float ElevationScale = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (ClampMin = "200", Units = "cm"))
	float RoadSampleSpacingCm = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (Units = "cm"))
	float LaneWidthCm = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (Units = "cm"))
	float ShoulderWidthCm = 250.f;

	/** Terreno generado hasta esta distancia del camino. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (ClampMin = "5000", Units = "cm"))
	float TerrainHalfWidthCm = 150000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (ClampMin = "1000", Units = "cm"))
	float TerrainCellCm = 3000.f;

	/** Radio plano junto al camino (cm); el terreno sube suavemente hasta RampRadiusCm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (Units = "cm"))
	float FlatRadiusCm = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (Units = "cm"))
	float RampRadiusCm = 25000.f;

	/** Cuanto queda el terreno bajo la calzada (cm) para que no se solapen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape", meta = (ClampMin = "0", Units = "cm"))
	float RoadBedDropCm = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape")
	int32 NoiseSeed = 1337;

	/** Estilo por bioma, indice = EChileBiome. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Shape")
	TArray<FChileBiomeStyle> BiomeStyles;

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

private:
	struct FRouteSample
	{
		FVector Pos = FVector::ZeroVector;
		FVector Dir = FVector::ForwardVector;
		FLinearColor Color = FLinearColor::Gray;
		float Amplitude = 0.f;
		float Ramp = 0.f;
	};

	void DefaultBiomeStyles();
	void BuildRoadMesh(const TArray<FRouteSample>& Samples);
	void BuildTerrain(const TArray<FRouteSample>& Samples);
	void TeleportPlayerToStart();
	void MovePlayerStartToStart();
	bool bBuiltInInit = false;
	float Fbm(float X, float Y) const;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UProceduralMeshComponent>> TerrainChunks;

	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedDelivery;

	/** Losas de colision invisibles (cubos del motor) bajo la calzada: suelo firme garantizado para el furgon. */
	UPROPERTY(Transient)
	TObjectPtr<UInstancedStaticMeshComponent> RoadCollisionISM;

	UPROPERTY()
	TObjectPtr<UStaticMesh> CollisionCube;

	void BuildRoadCollisionSlabs(const TArray<FRouteSample>& Samples);

	FTimerHandle TeleportTimer;
	int32 TeleportTries = 0;

	FTransform StartLocal = FTransform::Identity;
	FTransform EndLocal = FTransform::Identity;
	float RouteLengthCm = 0.f;
	bool bBuilt = false;
};
