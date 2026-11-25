#include "World/RoadsidePOI.h"
#include "Blackline.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Vehicle/BlacklineVanPawn.h"

#define LOCTEXT_NAMESPACE "BlacklinePOI"

ARoadsidePOI::ARoadsidePOI()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	Zone->SetMobility(EComponentMobility::Static);
	Zone->SetupAttachment(Root);
	Zone->SetBoxExtent(FVector(1300.f, 1300.f, 300.f));
	Zone->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::NoCollision); // la deteccion es por consulta de caja, sin eventos de solape

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Cube = CubeFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		BaseMaterial = MatFinder.Object;
	}
}

void ARoadsidePOI::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(PollTimer, this, &ARoadsidePOI::Poll, FMath::Max(0.05f, PollIntervalSeconds), true);
	}
}

void ARoadsidePOI::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PollTimer);
	}
	if (ABlacklineVanPawn* Van = TrackedVan.Get())
	{
		Van->SetActiveInteractable(nullptr);
	}
	Super::EndPlay(EndPlayReason);
}

FLinearColor ARoadsidePOI::GetMapColor() const
{
	return Kind == EPoiKind::GasStation ? FLinearColor(1.f, 0.85f, 0.1f, 1.f) : FLinearColor(1.f, 0.45f, 0.1f, 1.f);
}

void ARoadsidePOI::Poll()
{
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!Van)
	{
		return;
	}

	const bool bInside = Zone->Bounds.GetBox().IsInside(Van->GetActorLocation());
	if (bInside == bVanInside)
	{
		return;
	}
	bVanInside = bInside;

	if (bInside)
	{
		TrackedVan = Van;
		Van->SetActiveInteractable(this);
		Van->ShowHudMessage(Kind == EPoiKind::GasStation
			? LOCTEXT("PromptGas", "BENCINERA - detente y presiona E (Circulo) para cargar combustible")
			: LOCTEXT("PromptFood", "RESTAURANTE - detente y presiona E (Circulo) para descansar"), 5.f);
	}
	else
	{
		if (TrackedVan.Get() == Van)
		{
			Van->SetActiveInteractable(nullptr);
		}
		TrackedVan.Reset();
	}
}

void ARoadsidePOI::TryInteract(ABlacklineVanPawn* Van)
{
	const UWorld* World = GetWorld();
	if (!Van || !World)
	{
		return;
	}
	if (Van->GetSpeedKmh() > MaxInteractSpeedKmh)
	{
		Van->ShowHudMessage(LOCTEXT("TooFast", "Detente por completo para usar este lugar"), 3.f);
		return;
	}
	if (World->GetTimeSeconds() - LastInteractTime < InteractCooldownSeconds)
	{
		return;
	}
	LastInteractTime = World->GetTimeSeconds();

	if (Kind == EPoiKind::GasStation)
	{
		Van->AddFuel(1.f);
		Van->RepairDamage(0.15f);
		Van->ShowHudMessage(LOCTEXT("Refueled", "Tanque lleno. El mecanico revisa el chasis (-15% de dano)."), 4.f);
	}
	else
	{
		Van->ShowHudMessage(LOCTEXT("Rested", "Pides lo de siempre y guardas la mitad, por costumbre. El asiento del copiloto sigue vacio. El dueño dice que hay un reten mas adelante."), 5.f);
	}
	BP_OnInteracted(Van);
}

UStaticMeshComponent* ARoadsidePOI::AddBox(const FVector& PosCm, const FVector& SizeCm, const FLinearColor& Color, bool bCollide)
{
	UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(this);
	C->SetStaticMesh(Cube);
	C->SetMobility(EComponentMobility::Static);
	C->SetupAttachment(GetRootComponent());
	C->SetRelativeLocation(PosCm);
	C->SetRelativeScale3D(SizeCm / 100.f);
	if (BaseMaterial)
	{
		UMaterialInstanceDynamic* M = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		M->SetVectorParameterValue(TEXT("Color"), Color);
		C->SetMaterial(0, M);
	}
	C->SetCollisionProfileName(bCollide ? TEXT("BlockAll") : TEXT("NoCollision"));
	C->SetCastShadow(false);
	C->RegisterComponent();
	return C;
}

void ARoadsidePOI::AddLight(const FVector& PosCm, const FLinearColor& Color, float Intensity, float RadiusCm)
{
	UPointLightComponent* L = NewObject<UPointLightComponent>(this);
	L->SetMobility(EComponentMobility::Movable);
	L->SetupAttachment(GetRootComponent());
	L->SetRelativeLocation(PosCm);
	L->SetLightColor(Color);
	L->SetIntensity(Intensity);
	L->SetAttenuationRadius(RadiusCm);
	L->SetCastShadows(false);
	L->RegisterComponent();
}

void ARoadsidePOI::AddSign(const FVector& PosCm, const FString& Text, const FColor& Color, float WorldSize)
{
	UTextRenderComponent* T = NewObject<UTextRenderComponent>(this);
	T->SetMobility(EComponentMobility::Movable);
	T->SetupAttachment(GetRootComponent());
	T->SetRelativeLocation(PosCm);
	T->SetRelativeRotation(FRotator(0.f, -90.f, 0.f)); // el frente del texto mira hacia la ruta (-Y local)
	T->SetText(FText::FromString(Text));
	T->SetWorldSize(WorldSize);
	T->SetTextRenderColor(Color);
	T->SetHorizontalAlignment(EHTA_Center);
	T->SetVerticalAlignment(EVRTA_TextCenter);
	T->RegisterComponent();
}

void ARoadsidePOI::BuildFor(EPoiKind InKind)
{
	Kind = InKind;
	if (!Cube)
	{
		UE_LOG(LogBlackline, Warning, TEXT("RoadsidePOI: sin cubo base."));
		return;
	}

	// Convencion local: X a lo largo de la ruta, Y hacia afuera (la ruta queda en -Y), Z arriba; el origen es la explanada.
	AddBox(FVector(0, 0, 5), FVector(2600, 2600, 10), FLinearColor(0.10f, 0.10f, 0.11f), false); // explanada

	if (Kind == EPoiKind::GasStation)
	{
		AddBox(FVector(0, -200, 520), FVector(1700, 1100, 40), FLinearColor(0.55f, 0.08f, 0.06f), true);   // techo del surtidor
		for (const FVector2D& P : {FVector2D(-700, -650), FVector2D(700, -650), FVector2D(-700, 250), FVector2D(700, 250)})
		{
			AddBox(FVector(P.X, P.Y, 260), FVector(45, 45, 520), FLinearColor(0.35f, 0.35f, 0.37f), true); // pilares
		}
		AddBox(FVector(-320, -200, 60), FVector(90, 220, 120), FLinearColor(0.85f, 0.75f, 0.10f), true);    // surtidores
		AddBox(FVector(320, -200, 60), FVector(90, 220, 120), FLinearColor(0.85f, 0.75f, 0.10f), true);
		AddBox(FVector(0, 950, 200), FVector(1500, 650, 400), FLinearColor(0.50f, 0.46f, 0.38f), true);     // tienda
		AddBox(FVector(1150, -1050, 450), FVector(50, 50, 900), FLinearColor(0.30f, 0.30f, 0.32f), true);   // poste del letrero
		AddBox(FVector(1150, -1050, 980), FVector(90, 520, 180), FLinearColor(0.75f, 0.06f, 0.05f), true);  // letrero
		AddSign(FVector(1150, -1100, 980), TEXT("BENCINERA"), FColor(255, 240, 200), 70.f);
		AddLight(FVector(-420, -200, 470), FLinearColor(1.f, 0.88f, 0.65f), 9000.f, 2600.f);
		AddLight(FVector(420, -200, 470), FLinearColor(1.f, 0.88f, 0.65f), 9000.f, 2600.f);
		AddLight(FVector(1150, -1100, 1000), FLinearColor(1.f, 0.2f, 0.12f), 7000.f, 3500.f);
	}
	else
	{
		AddBox(FVector(0, 900, 270), FVector(2100, 950, 540), FLinearColor(0.45f, 0.26f, 0.18f), true);    // edificio
		AddBox(FVector(0, 900, 560), FVector(2300, 1150, 40), FLinearColor(0.16f, 0.10f, 0.08f), true);    // techo
		AddBox(FVector(-700, 380, 120), FVector(250, 150, 120), FLinearColor(0.55f, 0.40f, 0.25f), true);   // mesas
		AddBox(FVector(0, 380, 120), FVector(250, 150, 120), FLinearColor(0.55f, 0.40f, 0.25f), true);
		AddBox(FVector(700, 380, 120), FVector(250, 150, 120), FLinearColor(0.55f, 0.40f, 0.25f), true);
		AddBox(FVector(1200, -1000, 400), FVector(50, 50, 800), FLinearColor(0.30f, 0.30f, 0.32f), true);   // poste del letrero
		AddBox(FVector(1200, -1000, 860), FVector(90, 600, 180), FLinearColor(0.80f, 0.40f, 0.05f), true);  // letrero
		AddSign(FVector(1200, -1050, 860), TEXT("RESTAURANTE"), FColor(255, 245, 210), 60.f);
		AddLight(FVector(-500, 250, 300), FLinearColor(1.f, 0.78f, 0.45f), 12000.f, 3000.f);                // ventanales calidos
		AddLight(FVector(500, 250, 300), FLinearColor(1.f, 0.78f, 0.45f), 12000.f, 3000.f);
		AddLight(FVector(1200, -1050, 880), FLinearColor(1.f, 0.55f, 0.15f), 7000.f, 3500.f);
	}
}

#undef LOCTEXT_NAMESPACE
