#include "Terrain/TerrainModifierVolume.h"
#include "Components/BoxComponent.h"
#include "Vehicle/BlacklineVanPawn.h"
#include "Blackline.h"

ATerrainModifierVolume::ATerrainModifierVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Zone = CreateDefaultSubobject<UBoxComponent>(TEXT("Zone"));
	SetRootComponent(Zone);
	Zone->SetBoxExtent(FVector(500.f, 500.f, 100.f));
	Zone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Zone->SetCollisionObjectType(ECC_WorldDynamic);
	Zone->SetCollisionResponseToAllChannels(ECR_Ignore);
	Zone->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Overlap);
	Zone->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Zone->SetGenerateOverlapEvents(true);
}

void ATerrainModifierVolume::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Zone->OnComponentBeginOverlap.AddDynamic(this, &ATerrainModifierVolume::HandleBeginOverlap);
	Zone->OnComponentEndOverlap.AddDynamic(this, &ATerrainModifierVolume::HandleEndOverlap);
}

void ATerrainModifierVolume::HandleBeginOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*,
	int32, bool, const FHitResult&)
{
	if (ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(OtherActor))
	{
		// Varios componentes del furgon pueden solapar: AddUnique evita Push duplicado.
		if (!OverlappingVans.Contains(Van))
		{
			OverlappingVans.Add(Van);
			Van->PushSurfaceModifier(this);
			UE_LOG(LogBlackline, Verbose, TEXT("%s: Push en %s"), *GetName(), *Van->GetName());
		}
	}
}

void ATerrainModifierVolume::HandleEndOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32)
{
	ABlacklineVanPawn* Van = Cast<ABlacklineVanPawn>(OtherActor);
	if (!Van || !OverlappingVans.Contains(Van))
	{
		return;
	}
	// Con varios componentes: solo salimos si ya no hay ningun overlap real con este actor.
	if (Zone->IsOverlappingActor(Van))
	{
		return;
	}
	OverlappingVans.Remove(Van);
	Van->PopSurfaceModifier(this);
	UE_LOG(LogBlackline, Verbose, TEXT("%s: Pop en %s"), *GetName(), *Van->GetName());
}

void ATerrainModifierVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (ABlacklineVanPawn* Van : OverlappingVans)
	{
		if (IsValid(Van))
		{
			Van->PopSurfaceModifier(this);
		}
	}
	OverlappingVans.Reset();
	Super::EndPlay(EndPlayReason);
}
