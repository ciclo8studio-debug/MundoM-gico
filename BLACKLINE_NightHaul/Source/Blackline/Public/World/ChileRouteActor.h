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
class ARoadsidePOI;
class ATrafficCar;
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

	/** Punto de la ruta a una distancia (cm) con desplazamiento lateral (+ = derecha). Espacio mundo. */
	UFUNCTION(BlueprintCallable, Category = "Chile")
	void GetPointAtDistance(float DistanceCm, float LateralCm, FVector& OutPos, FRotator& OutRot) const;

	UFUNCTION(BlueprintPure, Category = "Chile")
	float GetRouteLengthCm() const { return RouteLengthCm; }

	/** Avance del jugador por la ruta [0,1]. */
	UFUNCTION(BlueprintPure, Category = "Chile")
	float GetProgress01(const FVector& WorldPos) const;

	UFUNCTION(BlueprintPure, Category = "Chile")
	FText GetZoneName() const;

	/** Puntos de la ruta (cada ~32 m) para el minimapa. */
	const TArray<FVector>& GetMapPoints() const { return MapPoints; }
	const TArray<TObjectPtr<ARoadsidePOI>>& GetPOIs() const { return POIs; }
	void GetTrafficLocations(TArray<FVector>& Out) const;

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

	/** Bencinera y restaurante junto a la ruta. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras")
	bool bSpawnRoadsidePlaces = true;

	/** Posicion de la bencinera y del restaurante a lo largo de la ruta [0,1]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (ClampMin = "0", ClampMax = "1"))
	float GasStationAt01 = 0.30f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (ClampMin = "0", ClampMax = "1"))
	float RestaurantAt01 = 0.65f;

	/** Autos que vienen en sentido contrario. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras")
	bool bSpawnOncomingTraffic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (ClampMin = "0", ClampMax = "30"))
	int32 TrafficCount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (Units = "km/h"))
	float TrafficMinSpeedKmh = 55.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (Units = "km/h"))
	float TrafficMaxSpeedKmh = 95.f;

	/** Los autos reaparecen entre estas distancias por delante del jugador (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (Units = "cm"))
	float TrafficSpawnAheadMinCm = 60000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (Units = "cm"))
	float TrafficSpawnAheadMaxCm = 220000.f;

	/** Un auto que quedo atras por mas de esto se recicla (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (Units = "cm"))
	float TrafficDespawnBehindCm = 40000.f;

	/** Inicia solo el contrato de la zona al empezar (se puede forzar con -contract=C09). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras")
	bool bAutoStartContract = true;

	/** Plazo del encargo: la ruta recorrida a esta velocidad media (km/h) mas un margen. Vencido, Chuño pierde bienestar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (ClampMin = "10", Units = "km/h"))
	float DeadlineAverageSpeedKmh = 55.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras", meta = (ClampMin = "1"))
	float DeadlineMargin = 1.25f;

	/** Contrato de cada zona: 1 Norte C15 (fosil), 2 Centro C01, 3 Sur C18 (cuesta con lluvia), 4 Cordillera C21 (paso). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras")
	TMap<int32, FName> ZoneContracts;

	/** Minimapa y datos de manejo en pantalla. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chile|Extras")
	bool bCreateMinimap = true;

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
	float RoadBedDropCm = 5.f;

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
	void SpawnRoadsidePlaces();
	void SpawnTraffic();
	void UpdateTraffic();
	void RespawnCarAhead(ATrafficCar* Car);
	void CreateMinimap();
	void StartZoneContract();

	UFUNCTION()
	void HandleDogWellbeingChanged(float Wellbeing01);

	int32 ActiveArc = 1;
	bool bEndingShown = false;

	UFUNCTION()
	void HandleContractFinished(FName ContractID);

	int32 CreditsAtContractStart = 0;
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

	UPROPERTY(Transient)
	TArray<TObjectPtr<ARoadsidePOI>> POIs;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ATrafficCar>> TrafficCars;

	TArray<FVector> MapPoints;
	FTimerHandle TrafficTimer;
	float VanDistanceCm = 0.f;
	int32 TrafficTick = 0;

	FTimerHandle TeleportTimer;
	int32 TeleportTries = 0;

	FTransform StartLocal = FTransform::Identity;
	FTransform EndLocal = FTransform::Identity;
	float RouteLengthCm = 0.f;
	bool bBuilt = false;
};
