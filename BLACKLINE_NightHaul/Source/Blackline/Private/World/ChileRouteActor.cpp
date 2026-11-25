#include "World/ChileRouteActor.h"
#include "Blackline.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "PhysicsEngine/BodySetup.h"
#include "Contracts/BlacklineContractSubsystem.h"
#include "Contracts/DeliveryZone.h"
#include "Engine/GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "UI/BlacklineMinimapWidget.h"
#include "World/RoadsidePOI.h"
#include "World/TrafficCar.h"
#include "Vehicle/BlacklineVanPawn.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "ProceduralMeshComponent.h"
#include "TimerManager.h"

namespace ChileRoutePrivate
{
	constexpr double MetersPerDegree = 111320.0;
	constexpr float HashCellCm = 10000.f;

	float SmoothStep01(float T)
	{
		T = FMath::Clamp(T, 0.f, 1.f);
		return T * T * (3.f - 2.f * T);
	}

	/** Franja de la calzada: desde A hasta B (cm, lateral derecho positivo). */
	struct FStrip
	{
		float A;
		float B;
		FLinearColor Color;
		float ZOffset;
	};
}

namespace ChileRoutePrivate
{
	/** Colision explicita y completa: terreno y calzada son estaticos y bloquean todo. */
	void ApplySolidCollision(UProceduralMeshComponent* C)
	{
		C->bUseComplexAsSimpleCollision = true;
		C->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		C->SetCollisionObjectType(ECC_WorldStatic);
		C->SetCollisionResponseToAllChannels(ECR_Block);
		C->SetMobility(EComponentMobility::Static); // movible+sin simular = cuerpo cinematico: la malla no frena al furgon
	}
}

AChileRouteActor::AChileRouteActor()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	Route = CreateDefaultSubobject<USplineComponent>(TEXT("Route"));
	Route->SetupAttachment(Root);
	Route->SetMobility(EComponentMobility::Static);

	RoadMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RoadMesh"));
	RoadMesh->SetupAttachment(Root);
	RoadMesh->bUseAsyncCooking = false; // el furgon no debe caer a traves de una malla aun sin cocinar
	RoadMesh->SetCollisionProfileName(TEXT("BlockAll"));
	ChileRoutePrivate::ApplySolidCollision(RoadMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		CollisionCube = CubeFinder.Object;
	}

	WaypointTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/Data/DT_ChileWaypoints.DT_ChileWaypoints")));
	RouteMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Blackline/Materials/M_VertexColor.M_VertexColor")));

	ZoneCompression.Add(1, 100.f);
	ZoneCompression.Add(2, 100.f);
	ZoneCompression.Add(3, 100.f);
	ZoneCompression.Add(4, 25.f);

	ZoneContracts.Add(1, FName(TEXT("C15")));
	ZoneContracts.Add(2, FName(TEXT("C01")));
	ZoneContracts.Add(3, FName(TEXT("C18")));
	ZoneContracts.Add(4, FName(TEXT("C21")));

	DefaultBiomeStyles();
}

void AChileRouteActor::DefaultBiomeStyles()
{
	auto Make = [](float R, float G, float B, float Amp, float Ramp)
	{
		FChileBiomeStyle S;
		S.Color = FLinearColor(R, G, B, 1.f);
		S.HillAmplitudeCm = Amp;
		S.RampSlope = Ramp;
		return S;
	};
	BiomeStyles.Reset();
	BiomeStyles.Add(Make(0.34f, 0.25f, 0.15f, 6000.f, 0.05f));  // DesertCoast
	BiomeStyles.Add(Make(0.42f, 0.30f, 0.17f, 9000.f, 0.08f));  // DesertInterior
	BiomeStyles.Add(Make(0.25f, 0.22f, 0.12f, 7000.f, 0.06f));  // Semiarid
	BiomeStyles.Add(Make(0.10f, 0.17f, 0.06f, 3000.f, 0.03f));  // CentralValley
	BiomeStyles.Add(Make(0.04f, 0.12f, 0.05f, 5000.f, 0.04f));  // HumidSouth
	BiomeStyles.Add(Make(0.30f, 0.28f, 0.26f, 20000.f, 0.35f)); // Cordillera
}

void AChileRouteActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Se genera ANTES de que el GameMode haga aparecer al jugador: el PlayerStart ya estara en el inicio de la ruta
	// y el furgon nace ahi, sin teletransportes (que dejaban al vehiculo cayendo a traves del suelo).
	UWorld* World = GetWorld();
	if (bBuildOnBeginPlay && World && World->IsGameWorld())
	{
		FParse::Value(FCommandLine::Get(), TEXT("zone="), ZoneId);
		BuildZone();
		bBuiltInInit = bBuilt;
	}
}

void AChileRouteActor::BeginPlay()
{
	Super::BeginPlay();
	if (bBuildOnBeginPlay && !bBuiltInInit)
	{
		FParse::Value(FCommandLine::Get(), TEXT("zone="), ZoneId);
		BuildZone();
	}
	if (bBuilt && GetWorld() && GetWorld()->IsGameWorld())
	{
		if (bSpawnRoadsidePlaces)
		{
			SpawnRoadsidePlaces();
		}
		if (bSpawnOncomingTraffic)
		{
			SpawnTraffic();
		}
		if (bCreateMinimap)
		{
			CreateMinimap();
		}
		if (bAutoStartContract)
		{
			StartZoneContract();
		}
	}
}

FText AChileRouteActor::GetZoneName() const
{
	switch (ZoneId)
	{
	case 1: return NSLOCTEXT("Blackline", "Zone1", "Norte: Arica - Copiapo");
	case 2: return NSLOCTEXT("Blackline", "Zone2", "Centro: Vallenar - Rancagua");
	case 3: return NSLOCTEXT("Blackline", "Zone3", "Sur: Talca - Puerto Montt");
	case 4: return NSLOCTEXT("Blackline", "Zone4", "Cordillera: Santiago - Paso");
	default: return FText::Format(NSLOCTEXT("Blackline", "ZoneN", "Zona {0}"), FText::AsNumber(ZoneId));
	}
}

void AChileRouteActor::GetPointAtDistance(float DistanceCm, float LateralCm, FVector& OutPos, FRotator& OutRot) const
{
	const float D = FMath::Clamp(DistanceCm, 0.f, RouteLengthCm);
	FVector Pos = Route->GetLocationAtDistanceAlongSpline(D, ESplineCoordinateSpace::World);
	FVector Dir = Route->GetDirectionAtDistanceAlongSpline(D, ESplineCoordinateSpace::World);
	Dir.Z = 0.f;
	Dir = Dir.GetSafeNormal();
	if (Dir.IsNearlyZero())
	{
		Dir = FVector::ForwardVector;
	}
	const FVector Right(-Dir.Y, Dir.X, 0.f);
	OutPos = Pos + Right * LateralCm;
	OutRot = Dir.Rotation();
}

float AChileRouteActor::GetProgress01(const FVector& WorldPos) const
{
	if (!Route || RouteLengthCm <= 1.f)
	{
		return 0.f;
	}
	const float Key = Route->FindInputKeyClosestToWorldLocation(WorldPos);
	return FMath::Clamp(Route->GetDistanceAlongSplineAtSplineInputKey(Key) / RouteLengthCm, 0.f, 1.f);
}

void AChileRouteActor::GetTrafficLocations(TArray<FVector>& Out) const
{
	Out.Reset();
	for (const ATrafficCar* Car : TrafficCars)
	{
		if (Car)
		{
			Out.Add(Car->GetActorLocation());
		}
	}
}

void AChileRouteActor::SpawnRoadsidePlaces()
{
	UWorld* World = GetWorld();
	const float Lateral = 2000.f; // centro de la explanada: borde interior a 7 m del eje (pasada la berma)
	const TArray<TPair<float, EPoiKind>> Places = {{GasStationAt01, EPoiKind::GasStation}, {RestaurantAt01, EPoiKind::Restaurant}};
	for (const TPair<float, EPoiKind>& P : Places)
	{
		FVector Pos;
		FRotator Rot;
		GetPointAtDistance(RouteLengthCm * P.Key, Lateral, Pos, Rot);
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ARoadsidePOI* POI = World->SpawnActor<ARoadsidePOI>(ARoadsidePOI::StaticClass(), Pos - FVector(0, 0, RoadBedDropCm), FRotator(0.f, Rot.Yaw, 0.f), Params);
		if (POI)
		{
			POI->BuildFor(P.Value);
			POIs.Add(POI);
		}
	}
	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: %d lugares junto a la ruta."), POIs.Num());
}

void AChileRouteActor::SpawnTraffic()
{
	UWorld* World = GetWorld();
	static const FLinearColor Colors[] = {
		FLinearColor(0.7f, 0.05f, 0.05f), FLinearColor(0.1f, 0.2f, 0.6f), FLinearColor(0.75f, 0.75f, 0.78f),
		FLinearColor(0.08f, 0.08f, 0.09f), FLinearColor(0.85f, 0.7f, 0.1f), FLinearColor(0.1f, 0.4f, 0.15f)};

	for (int32 i = 0; i < TrafficCount; ++i)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ATrafficCar* Car = World->SpawnActor<ATrafficCar>(ATrafficCar::StaticClass(), GetActorLocation(), FRotator::ZeroRotator, Params);
		if (!Car)
		{
			continue;
		}
		Car->SetBodyColor(Colors[FMath::RandRange(0, UE_ARRAY_COUNT(Colors) - 1)]);
		RespawnCarAhead(Car);
		TrafficCars.Add(Car);
	}
	World->GetTimerManager().SetTimer(TrafficTimer, this, &AChileRouteActor::UpdateTraffic, 0.05f, true);
	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: %d autos en sentido contrario."), TrafficCars.Num());
}

void AChileRouteActor::RespawnCarAhead(ATrafficCar* Car)
{
	const float Max = FMath::Max(TrafficSpawnAheadMinCm + 1.f, TrafficSpawnAheadMaxCm);
	float D = VanDistanceCm + FMath::FRandRange(TrafficSpawnAheadMinCm, Max);
	D = FMath::Min(D, FMath::Max(0.f, RouteLengthCm - 1500.f));
	Car->RouteDistanceCm = D;
	Car->SpeedCmPerSec = FMath::FRandRange(TrafficMinSpeedKmh, FMath::Max(TrafficMinSpeedKmh, TrafficMaxSpeedKmh)) * 27.78f;
}

void AChileRouteActor::UpdateTraffic()
{
	const float Dt = 0.05f;
	if (++TrafficTick % 5 == 0)
	{
		if (const APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
		{
			const float Key = Route->FindInputKeyClosestToWorldLocation(Pawn->GetActorLocation());
			VanDistanceCm = Route->GetDistanceAlongSplineAtSplineInputKey(Key);
		}
	}

	for (ATrafficCar* Car : TrafficCars)
	{
		if (!Car)
		{
			continue;
		}
		Car->RouteDistanceCm -= Car->SpeedCmPerSec * Dt;
		if (Car->RouteDistanceCm < 500.f || Car->RouteDistanceCm < VanDistanceCm - TrafficDespawnBehindCm)
		{
			RespawnCarAhead(Car);
		}
		FVector Pos;
		FRotator Rot;
		GetPointAtDistance(Car->RouteDistanceCm, -LaneWidthCm * 0.5f, Pos, Rot);
		Rot.Yaw += 180.f; // viene hacia el jugador
		Car->SetActorLocationAndRotation(Pos + FVector(0, 0, 60.f), Rot, false, nullptr, ETeleportType::None);
	}
}

void AChileRouteActor::StartZoneContract()
{
	FName Id = ZoneContracts.FindRef(ZoneId);
	FString Cmd;
	if (FParse::Value(FCommandLine::Get(), TEXT("contract="), Cmd) && !Cmd.IsEmpty())
	{
		Id = FName(*Cmd);
	}
	UGameInstance* GI = GetGameInstance();
	UBlacklineContractSubsystem* Contracts = GI ? GI->GetSubsystem<UBlacklineContractSubsystem>() : nullptr;
	if (Id.IsNone() || !Contracts)
	{
		return;
	}

	CreditsAtContractStart = Contracts->GetCredits();
	bEndingShown = false;
	if (Contracts->StartContract(Id))
	{
		Contracts->OnContractFinished.AddUniqueDynamic(this, &AChileRouteActor::HandleContractFinished);
		Contracts->OnDogWellbeingChanged.AddUniqueDynamic(this, &AChileRouteActor::HandleDogWellbeingChanged);

		// Plazo: el largo de la ruta a velocidad media, con margen.
		const float Seconds = (RouteLengthCm / 100.f) / (DeadlineAverageSpeedKmh / 3.6f) * DeadlineMargin;
		Contracts->StartDeadline(Seconds);

		FBlacklineContractRow Row;
		if (Contracts->GetContract(Id, Row))
		{
			ActiveArc = Row.Arc;
			if (ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(UGameplayStatics::GetPlayerPawn(this, 0)))
			{
				Van->ShowHudMessage(FText::Format(NSLOCTEXT("Blackline", "ContractStarted", "{0}: {1}"), Row.BrokerName, Row.Briefing), 12.f);
			}
		}
	}
}

void AChileRouteActor::HandleDogWellbeingChanged(float Wellbeing01)
{
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Van || bEndingShown)
	{
		return;
	}
	if (Wellbeing01 <= 0.f)
	{
		bEndingShown = true;
		Van->ShowHudMessage(NSLOCTEXT("Blackline", "EndBad", "FIN: no llegaste a tiempo y La Cofradia corto la comunicacion. Chuño ya no volvera a la cabina."), 20.f);
	}
	else if (Wellbeing01 < 0.3f)
	{
		Van->ShowHudMessage(NSLOCTEXT("Blackline", "DogLow", "Chuño esta mal. La Cofradia te lo hace saber. Entrega a tiempo."), 6.f);
	}
}

void AChileRouteActor::HandleContractFinished(FName ContractID)
{
	UGameInstance* GI = GetGameInstance();
	const UBlacklineContractSubsystem* Contracts = GI ? GI->GetSubsystem<UBlacklineContractSubsystem>() : nullptr;
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Contracts || !Van || bEndingShown)
	{
		return;
	}

	const int32 Earned = Contracts->GetCredits() - CreditsAtContractStart;
	const float Dog = Contracts->GetDogWellbeing01();
	if (Earned <= 0)
	{
		Van->ShowHudMessage(NSLOCTEXT("Blackline", "ContractFailed", "ENCARGO FALLIDO: la carga se perdio. La Cofradia no perdona."), 10.f);
		return;
	}

	FText Proof;
	switch (ActiveArc)
	{
	case 1: Proof = NSLOCTEXT("Blackline", "Proof1", "PRUEBA DE VIDA: ladridos por la radio. Es Chuño. El Ingeniero corta la llamada."); break;
	case 2: Proof = NSLOCTEXT("Blackline", "Proof2", "PRUEBA DE VIDA: una foto de Chuño, la oreja caida, mirando a camara. La Anticuaria sonrie en el mensaje."); break;
	case 3: Proof = NSLOCTEXT("Blackline", "Proof3", "PRUEBA DE VIDA: Chuño duerme sobre una manta. Detras se oye viento de desierto: es una pista."); break;
	case 4: Proof = NSLOCTEXT("Blackline", "Proof4", "PRUEBA DE VIDA: un aullido lejano y lluvia en un techo. El Capataz: \"Falta poco, Salinas\"."); break;
	default:
		if (Dog >= 0.5f)
		{
			bEndingShown = true;
			Van->ShowHudMessage(NSLOCTEXT("Blackline", "EndGood", "RESCATE: el Arriero abre una puerta. Chuño corre a la cabina y se sube al asiento del copiloto. FIN."), 20.f);
			return;
		}
		bEndingShown = true;
		Van->ShowHudMessage(NSLOCTEXT("Blackline", "EndCost", "Llegas tarde: Chuño esta debil, pero vivo, y vuelve contigo. Final con costo. FIN."), 20.f);
		return;
	}
	Van->ShowHudMessage(FText::Format(NSLOCTEXT("Blackline", "DeliveredStory", "{0}  (+{1} viaticos, Chuño {2}%)"),
		Proof, FText::AsNumber(Earned), FText::AsNumber(FMath::RoundToInt(Dog * 100.f))), 12.f);
}

void AChileRouteActor::CreateMinimap()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}
	if (UBlacklineMinimapWidget* W = CreateWidget<UBlacklineMinimapWidget>(PC))
	{
		W->SetRoute(this);
		W->AddToViewport(50);
	}
}

void AChileRouteActor::MovePlayerStartToStart()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	const FTransform Start = GetStartTransform();
	APlayerStart* Target = nullptr;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		Target = *It;
		break;
	}
	if (!Target)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Target = World->SpawnActor<APlayerStart>(Start.GetLocation(), Start.Rotator(), Params);
	}
	else
	{
		Target->SetActorLocationAndRotation(Start.GetLocation(), Start.Rotator());
	}
	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: PlayerStart movido al inicio de la zona %d (%s)."), ZoneId, *Start.GetLocation().ToString());
}

float AChileRouteActor::Fbm(float X, float Y) const
{
	float Sum = 0.f;
	float Amp = 1.f;
	float Freq = 1.f / 60000.f;
	float Norm = 0.f;
	const float Off = static_cast<float>(NoiseSeed) * 7.919f;
	for (int32 Octave = 0; Octave < 4; ++Octave)
	{
		Sum += Amp * FMath::PerlinNoise2D(FVector2D(X * Freq + Off, Y * Freq - Off));
		Norm += Amp;
		Amp *= 0.5f;
		Freq *= 2.f;
	}
	return Norm > 0.f ? Sum / Norm : 0.f;
}

void AChileRouteActor::ClearZone()
{
	for (UProceduralMeshComponent* C : TerrainChunks)
	{
		if (C)
		{
			C->DestroyComponent();
		}
	}
	TerrainChunks.Reset();
	if (RoadMesh)
	{
		RoadMesh->ClearAllMeshSections();
	}
	if (Route)
	{
		Route->ClearSplinePoints(true);
	}
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(TrafficTimer);
	}
	for (ATrafficCar* Car : TrafficCars)
	{
		if (Car)
		{
			Car->Destroy();
		}
	}
	TrafficCars.Reset();
	for (ARoadsidePOI* P : POIs)
	{
		if (P)
		{
			P->Destroy();
		}
	}
	POIs.Reset();
	MapPoints.Reset();
	if (RoadCollisionISM)
	{
		RoadCollisionISM->DestroyComponent();
		RoadCollisionISM = nullptr;
	}
	if (SpawnedDelivery)
	{
		SpawnedDelivery->Destroy();
		SpawnedDelivery = nullptr;
	}
	bBuilt = false;
}

FTransform AChileRouteActor::GetStartTransform() const
{
	return StartLocal * GetActorTransform();
}

FTransform AChileRouteActor::GetEndTransform() const
{
	return EndLocal * GetActorTransform();
}

void AChileRouteActor::BuildZone()
{
	using namespace ChileRoutePrivate;
	const double T0 = FPlatformTime::Seconds();
	ClearZone();

	UDataTable* Table = WaypointTable.LoadSynchronous();
	if (!Table)
	{
		UE_LOG(LogBlackline, Warning, TEXT("ChileRoute: no encuentro la tabla de waypoints."));
		return;
	}

	// --- Waypoints de la zona, ordenados por nombre de fila (Z2_01, Z2_02, ...).
	TArray<TPair<FName, FChileWaypointRow>> Points;
	Table->ForeachRow<FChileWaypointRow>(TEXT("ChileRoute"), [&](const FName& Key, const FChileWaypointRow& Row)
	{
		if (Row.ZoneId == ZoneId)
		{
			Points.Emplace(Key, Row);
		}
	});
	Points.Sort([](const TPair<FName, FChileWaypointRow>& A, const TPair<FName, FChileWaypointRow>& B)
	{
		return A.Key.LexicalLess(B.Key);
	});
	if (Points.Num() < 2)
	{
		UE_LOG(LogBlackline, Warning, TEXT("ChileRoute: la zona %d tiene %d waypoints (minimo 2)."), ZoneId, Points.Num());
		return;
	}

	// --- Proyeccion equirectangular comprimida. X = hacia el sur, Y = hacia el este, Z = altura * escala vertical.
	const float Compression = FMath::Max(1.f, ZoneCompression.FindRef(ZoneId) > 0.f ? ZoneCompression.FindRef(ZoneId) : 100.f);
	const double Lat0 = Points[0].Value.LatDeg;
	const double Lon0 = Points[0].Value.LonDeg;
	TArray<FVector> Local;
	for (const TPair<FName, FChileWaypointRow>& P : Points)
	{
		const FChileWaypointRow& R = P.Value;
		const double X = (Lat0 - R.LatDeg) * MetersPerDegree * 100.0 / Compression;
		const double Y = (R.LonDeg - Lon0) * MetersPerDegree * FMath::Cos(FMath::DegreesToRadians((Lat0 + R.LatDeg) * 0.5)) * 100.0 / Compression;
		Local.Add(FVector(X, Y, R.ElevationM * 100.0 * ElevationScale));
	}

	// --- Spline suave por los waypoints.
	Route->ClearSplinePoints(false);
	for (int32 i = 0; i < Local.Num(); ++i)
	{
		Route->AddSplinePoint(Local[i], ESplineCoordinateSpace::Local, false);
		Route->SetSplinePointType(i, ESplinePointType::Curve, false);
	}
	Route->UpdateSpline();
	RouteLengthCm = Route->GetSplineLength();

	// --- Muestras a lo largo de la ruta con atributos de bioma interpolados.
	TArray<FRouteSample> Samples;
	const int32 Count = FMath::Max(2, FMath::CeilToInt(RouteLengthCm / RoadSampleSpacingCm) + 1);
	for (int32 i = 0; i < Count; ++i)
	{
		const float Dist = FMath::Min(RouteLengthCm, static_cast<float>(i) * RoadSampleSpacingCm);
		FRouteSample S;
		S.Pos = Route->GetLocationAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::Local);
		FVector Dir = Route->GetDirectionAtDistanceAlongSpline(Dist, ESplineCoordinateSpace::Local);
		Dir.Z = 0.f;
		S.Dir = Dir.GetSafeNormal();
		if (S.Dir.IsNearlyZero())
		{
			S.Dir = FVector::ForwardVector;
		}

		const float Key = Route->GetInputKeyValueAtDistanceAlongSpline(Dist);
		const int32 Idx = FMath::Clamp(FMath::FloorToInt(Key), 0, Points.Num() - 2);
		const float Alpha = FMath::Clamp(Key - static_cast<float>(Idx), 0.f, 1.f);
		const FChileBiomeStyle& A = BiomeStyles[FMath::Clamp(static_cast<int32>(Points[Idx].Value.Biome), 0, BiomeStyles.Num() - 1)];
		const FChileBiomeStyle& B = BiomeStyles[FMath::Clamp(static_cast<int32>(Points[Idx + 1].Value.Biome), 0, BiomeStyles.Num() - 1)];
		S.Color = FMath::Lerp(A.Color, B.Color, Alpha);
		S.Amplitude = FMath::Lerp(A.HillAmplitudeCm, B.HillAmplitudeCm, Alpha);
		S.Ramp = FMath::Lerp(A.RampSlope, B.RampSlope, Alpha);
		Samples.Add(S);
	}

	MapPoints.Reset();
	for (int32 i = 0; i < Samples.Num(); i += 4)
	{
		MapPoints.Add(GetActorTransform().TransformPosition(Samples[i].Pos));
	}
	MapPoints.Add(GetActorTransform().TransformPosition(Samples.Last().Pos));

	const FVector Right0(-Samples[0].Dir.Y, Samples[0].Dir.X, 0.f);
	StartLocal = FTransform(Samples[0].Dir.Rotation(), Samples[0].Pos + Right0 * (LaneWidthCm * 0.5f) + FVector(0, 0, 150.f));
	const FRouteSample& Last = Samples.Last();
	const FVector RightN(-Last.Dir.Y, Last.Dir.X, 0.f);
	EndLocal = FTransform(Last.Dir.Rotation(), Last.Pos + RightN * (LaneWidthCm * 0.5f));

	BuildRoadMesh(Samples);
	BuildRoadCollisionSlabs(Samples);
	BuildTerrain(Samples);
	bBuilt = true;

	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: zona %d (%d waypoints, 1:%.0f): %.1f km de ruta, %d chunks de terreno, %.0f ms."),
		ZoneId, Points.Num(), Compression, RouteLengthCm / 100000.f, TerrainChunks.Num(), (FPlatformTime::Seconds() - T0) * 1000.0);

	if (bSpawnDeliveryZoneAtEnd && GetWorld() && GetWorld()->IsGameWorld())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const FTransform End = GetEndTransform();
		SpawnedDelivery = GetWorld()->SpawnActor<ADeliveryZone>(ADeliveryZone::StaticClass(), End.GetLocation() + FVector(0, 0, 150.f), End.Rotator(), Params);
	}

	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		MovePlayerStartToStart();
	}

	if (bTeleportPlayerToStart && GetWorld() && GetWorld()->IsGameWorld())
	{
		TeleportTries = 0;
		TeleportPlayerToStart();
	}
}

void AChileRouteActor::TeleportPlayerToStart()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Pawn)
	{
		if (++TeleportTries < 20)
		{
			GetWorldTimerManager().SetTimer(TeleportTimer, this, &AChileRouteActor::TeleportPlayerToStart, 0.25f, false);
		}
		return;
	}
	const FTransform Start = GetStartTransform();

	// Diagnostico: ¿hay colision bajo el inicio? (traza hacia abajo + mallas de colision cocinadas)
	{
		FHitResult Hit;
		const FVector From = Start.GetLocation() + FVector(0, 0, 500.f);
		const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, From, From - FVector(0, 0, 20000.f), ECC_WorldStatic);
		const UBodySetup* RoadBody = RoadMesh ? RoadMesh->GetBodySetup() : nullptr;
		UE_LOG(LogBlackline, Log, TEXT("ChileRoute diag: traza=%d en %s (%s) | calzada trimesh=%d fisica=%d | chunks=%d"),
			bHit ? 1 : 0, bHit ? *Hit.ImpactPoint.ToString() : TEXT("-"),
			bHit && Hit.GetComponent() ? *Hit.GetComponent()->GetName() : TEXT("-"),
			RoadBody ? RoadBody->TriMeshGeometries.Num() : -1,
			RoadMesh && RoadMesh->IsPhysicsStateCreated() ? 1 : 0, TerrainChunks.Num());
	}

	Pawn->TeleportTo(Start.GetLocation(), Start.Rotator(), false, true);
	if (UPrimitiveComponent* Body = Cast<UPrimitiveComponent>(Pawn->GetRootComponent()))
	{
		if (Body->IsSimulatingPhysics())
		{
			Body->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Body->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: jugador en el inicio de la zona %d (%s)."), ZoneId, *Start.GetLocation().ToString());
}

void AChileRouteActor::BuildRoadMesh(const TArray<FRouteSample>& Samples)
{
	using namespace ChileRoutePrivate;

	const float L = LaneWidthCm;
	const float S = LaneWidthCm + ShoulderWidthCm;
	const FLinearColor Asphalt(0.025f, 0.025f, 0.03f, 1.f);
	const FLinearColor Shoulder(0.16f, 0.14f, 0.11f, 1.f);
	const FLinearColor Yellow(0.9f, 0.65f, 0.0f, 1.f);
	const FLinearColor White(0.85f, 0.85f, 0.85f, 1.f);
	const TArray<FStrip> Strips = {
		{-S, -L, Shoulder, 0.f},
		{L, S, Shoulder, 0.f},
		{-L, L, Asphalt, 1.f},
		{-18.f, -6.f, Yellow, 3.f},
		{6.f, 18.f, Yellow, 3.f},
		{-L + 10.f, -L + 22.f, White, 3.f},
		{L - 22.f, L - 10.f, White, 3.f},
	};

	TArray<FVector> V;
	TArray<int32> Tri;
	TArray<FVector> N;
	TArray<FVector2D> UV;
	TArray<FLinearColor> C;
	TArray<FProcMeshTangent> Tan;

	for (int32 i = 0; i + 1 < Samples.Num(); ++i)
	{
		const FRouteSample& P0 = Samples[i];
		const FRouteSample& P1 = Samples[i + 1];
		const FVector R0(-P0.Dir.Y, P0.Dir.X, 0.f);
		const FVector R1(-P1.Dir.Y, P1.Dir.X, 0.f);
		for (const FStrip& Strip : Strips)
		{
			const int32 Base = V.Num();
			V.Add(P0.Pos + R0 * Strip.A + FVector(0, 0, Strip.ZOffset));
			V.Add(P0.Pos + R0 * Strip.B + FVector(0, 0, Strip.ZOffset));
			V.Add(P1.Pos + R1 * Strip.B + FVector(0, 0, Strip.ZOffset));
			V.Add(P1.Pos + R1 * Strip.A + FVector(0, 0, Strip.ZOffset));
			for (int32 k = 0; k < 4; ++k)
			{
				N.Add(FVector::UpVector);
				UV.Add(FVector2D(0, 0));
				C.Add(Strip.Color);
			}
			Tri.Append({Base, Base + 3, Base + 2, Base, Base + 2, Base + 1});
		}
	}

	ChileRoutePrivate::ApplySolidCollision(RoadMesh);
	RoadMesh->CreateMeshSection_LinearColor(0, V, Tri, N, UV, C, Tan, true);
	if (UMaterialInterface* Mat = RouteMaterial.LoadSynchronous())
	{
		RoadMesh->SetMaterial(0, Mat);
	}
}

void AChileRouteActor::BuildRoadCollisionSlabs(const TArray<FRouteSample>& Samples)
{
	if (!CollisionCube || Samples.Num() < 2)
	{
		UE_LOG(LogBlackline, Warning, TEXT("ChileRoute: sin cubo de colision; la ruta depende solo de la colision procedural."));
		return;
	}

	RoadCollisionISM = NewObject<UInstancedStaticMeshComponent>(this);
	RoadCollisionISM->SetStaticMesh(CollisionCube);
	RoadCollisionISM->SetMobility(EComponentMobility::Static);
	RoadCollisionISM->SetupAttachment(GetRootComponent());
	RoadCollisionISM->SetCollisionProfileName(TEXT("BlockAll"));
	RoadCollisionISM->SetCastShadow(false);
	RoadCollisionISM->SetHiddenInGame(true);
	RoadCollisionISM->RegisterComponent();

	const float RoadWidth = 2.f * (LaneWidthCm + ShoulderWidthCm);
	const float WideWidth = 2.f * FlatRadiusCm;
	const float Thick = 100.f;
	TArray<FTransform> Instances;
	for (int32 i = 0; i + 1 < Samples.Num(); ++i)
	{
		const FVector& A = Samples[i].Pos;
		const FVector& B = Samples[i + 1].Pos;
		const FVector Delta = B - A;
		const float Len = static_cast<float>(Delta.Size()) + 40.f; // solape entre losas
		const FQuat Rot = Delta.GetSafeNormal().ToOrientationQuat();
		const FVector Mid = (A + B) * 0.5;
		// Losa de la calzada y bermas: la cara superior queda a la altura de la calzada.
		Instances.Add(FTransform(Rot, Mid - Rot.GetUpVector() * (Thick * 0.5f), FVector(Len / 100.f, RoadWidth / 100.f, Thick / 100.f)));
		// Losa ancha a la altura del terreno plano (RoadBedDrop mas abajo).
		Instances.Add(FTransform(Rot, Mid - Rot.GetUpVector() * (Thick * 0.5f + RoadBedDropCm), FVector(Len / 100.f, WideWidth / 100.f, Thick / 100.f)));
	}
	RoadCollisionISM->AddInstances(Instances, false, false);
	UE_LOG(LogBlackline, Log, TEXT("ChileRoute: %d losas de colision bajo la calzada."), Instances.Num());
}

void AChileRouteActor::BuildTerrain(const TArray<FRouteSample>& Samples)
{
	using namespace ChileRoutePrivate;

	// --- Indice espacial de muestras para hallar el camino mas cercano.
	TMap<FIntPoint, TArray<int32>> Hash;
	FVector2D Min(TNumericLimits<double>::Max(), TNumericLimits<double>::Max());
	FVector2D Max(-TNumericLimits<double>::Max(), -TNumericLimits<double>::Max());
	for (int32 i = 0; i < Samples.Num(); ++i)
	{
		const FVector& P = Samples[i].Pos;
		Hash.FindOrAdd(FIntPoint(FMath::FloorToInt(P.X / HashCellCm), FMath::FloorToInt(P.Y / HashCellCm))).Add(i);
		Min.X = FMath::Min(Min.X, P.X);
		Min.Y = FMath::Min(Min.Y, P.Y);
		Max.X = FMath::Max(Max.X, P.X);
		Max.Y = FMath::Max(Max.Y, P.Y);
	}
	const int32 MaxRing = FMath::CeilToInt(TerrainHalfWidthCm / HashCellCm) + 1;

	auto FindNearest = [&](double X, double Y, int32& OutIdx, float& OutDist) -> bool
	{
		const int32 CX = FMath::FloorToInt(X / HashCellCm);
		const int32 CY = FMath::FloorToInt(Y / HashCellCm);
		float Best = TNumericLimits<float>::Max();
		int32 BestIdx = INDEX_NONE;
		for (int32 R = 0; R <= MaxRing; ++R)
		{
			for (int32 DX = -R; DX <= R; ++DX)
			{
				for (int32 DY = -R; DY <= R; ++DY)
				{
					if (FMath::Max(FMath::Abs(DX), FMath::Abs(DY)) != R)
					{
						continue;
					}
					if (const TArray<int32>* Cell = Hash.Find(FIntPoint(CX + DX, CY + DY)))
					{
						for (int32 Idx : *Cell)
						{
							const float D = static_cast<float>(FVector2D::Distance(FVector2D(X, Y), FVector2D(Samples[Idx].Pos.X, Samples[Idx].Pos.Y)));
							if (D < Best)
							{
								Best = D;
								BestIdx = Idx;
							}
						}
					}
				}
			}
			if (BestIdx != INDEX_NONE && Best <= static_cast<float>(R) * HashCellCm)
			{
				break;
			}
		}
		OutIdx = BestIdx;
		OutDist = Best;
		return BestIdx != INDEX_NONE && Best <= TerrainHalfWidthCm;
	};

	// --- Rejilla global de alturas.
	const float Cell = TerrainCellCm;
	const double OriginX = FMath::FloorToDouble((Min.X - TerrainHalfWidthCm) / Cell) * Cell;
	const double OriginY = FMath::FloorToDouble((Min.Y - TerrainHalfWidthCm) / Cell) * Cell;
	const int32 NX = FMath::CeilToInt((Max.X + TerrainHalfWidthCm - OriginX) / Cell) + 1;
	const int32 NY = FMath::CeilToInt((Max.Y + TerrainHalfWidthCm - OriginY) / Cell) + 1;

	TArray<float> H;
	TArray<uint8> Valid;
	TArray<FLinearColor> Col;
	H.Init(0.f, NX * NY);
	Valid.Init(0, NX * NY);
	Col.Init(FLinearColor::Black, NX * NY);

	for (int32 j = 0; j < NY; ++j)
	{
		for (int32 i = 0; i < NX; ++i)
		{
			const double X = OriginX + i * Cell;
			const double Y = OriginY + j * Cell;
			int32 Idx;
			float D;
			const int32 K = j * NX + i;
			if (!FindNearest(X, Y, Idx, D))
			{
				continue;
			}
			const FRouteSample& S = Samples[Idx];
			const float T = SmoothStep01((D - FlatRadiusCm) / FMath::Max(1.f, RampRadiusCm - FlatRadiusCm));
			const float Noise = Fbm(static_cast<float>(X), static_cast<float>(Y));
			const float Hill = Noise * S.Amplitude + S.Ramp * FMath::Max(0.f, D - FlatRadiusCm);
			H[K] = static_cast<float>(S.Pos.Z) - RoadBedDropCm + T * Hill;
			Valid[K] = 1;
			const float Tint = 0.85f + 0.15f * Fbm(static_cast<float>(X) * 3.f, static_cast<float>(Y) * 3.f);
			Col[K] = FLinearColor(S.Color.R * Tint, S.Color.G * Tint, S.Color.B * Tint, 1.f);
		}
	}

	// --- Chunks de 64x64 celdas con colision.
	constexpr int32 ChunkCells = 64;
	UMaterialInterface* Mat = RouteMaterial.LoadSynchronous();
	for (int32 CY0 = 0; CY0 < NY - 1; CY0 += ChunkCells)
	{
		for (int32 CX0 = 0; CX0 < NX - 1; CX0 += ChunkCells)
		{
			const int32 I1 = FMath::Min(CX0 + ChunkCells, NX - 1);
			const int32 J1 = FMath::Min(CY0 + ChunkCells, NY - 1);
			const int32 WI = I1 - CX0 + 1;
			const int32 WJ = J1 - CY0 + 1;

			TArray<FVector> V;
			TArray<FVector> N;
			TArray<FVector2D> UV;
			TArray<FLinearColor> C;
			TArray<int32> Tri;
			V.Reserve(WI * WJ);

			for (int32 j = CY0; j <= J1; ++j)
			{
				for (int32 i = CX0; i <= I1; ++i)
				{
					const int32 K = j * NX + i;
					V.Add(FVector(OriginX + i * Cell, OriginY + j * Cell, H[K]));
					const float Hx0 = H[j * NX + FMath::Max(i - 1, 0)];
					const float Hx1 = H[j * NX + FMath::Min(i + 1, NX - 1)];
					const float Hy0 = H[FMath::Max(j - 1, 0) * NX + i];
					const float Hy1 = H[FMath::Min(j + 1, NY - 1) * NX + i];
					N.Add(FVector(-(Hx1 - Hx0) / (2.f * Cell), -(Hy1 - Hy0) / (2.f * Cell), 1.f).GetSafeNormal());
					UV.Add(FVector2D(0, 0));
					C.Add(Col[K]);
				}
			}

			for (int32 j = 0; j < WJ - 1; ++j)
			{
				for (int32 i = 0; i < WI - 1; ++i)
				{
					const int32 GI = CX0 + i;
					const int32 GJ = CY0 + j;
					if (!(Valid[GJ * NX + GI] && Valid[GJ * NX + GI + 1] && Valid[(GJ + 1) * NX + GI] && Valid[(GJ + 1) * NX + GI + 1]))
					{
						continue;
					}
					const int32 A = j * WI + i;
					const int32 B = A + 1;
					const int32 Cc = A + WI;
					const int32 D = Cc + 1;
					Tri.Append({A, Cc, D, A, D, B});
				}
			}
			if (Tri.Num() == 0)
			{
				continue;
			}

			UProceduralMeshComponent* Chunk = NewObject<UProceduralMeshComponent>(this);
			Chunk->bUseAsyncCooking = false;
			Chunk->SetMobility(EComponentMobility::Static);
			Chunk->SetupAttachment(GetRootComponent());
			Chunk->RegisterComponent();
			Chunk->SetCollisionProfileName(TEXT("BlockAll"));
			ChileRoutePrivate::ApplySolidCollision(Chunk);
			TArray<FProcMeshTangent> Tan;
			Chunk->CreateMeshSection_LinearColor(0, V, Tri, N, UV, C, Tan, true);
			if (Mat)
			{
				Chunk->SetMaterial(0, Mat);
			}
			TerrainChunks.Add(Chunk);
		}
	}
}
