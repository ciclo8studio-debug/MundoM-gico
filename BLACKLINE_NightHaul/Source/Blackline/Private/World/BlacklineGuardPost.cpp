#include "World/BlacklineGuardPost.h"
#include "Core/BlacklineShapes.h"

ABlacklineGuardPost::ABlacklineGuardPost()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	BarrierPivot = CreateDefaultSubobject<USceneComponent>(TEXT("BarrierPivot"));
	BarrierPivot->SetupAttachment(Root);
	BarrierPivot->SetRelativeLocation(FVector(0.f, 450.f, 110.f));
}

void ABlacklineGuardPost::BeginPlay()
{
	Super::BeginPlay();
	using namespace BlacklineShapes;
	USceneComponent* Root = GetRootComponent();

	// Poste de la barrera (a la orilla derecha) y brazo rojo y blanco que cruza la calzada.
	Add(this, Root, EShape::Cube, FVector(0.f, 450.f, 60.f), FRotator::ZeroRotator, FVector(0.3f, 0.3f, 1.2f), FLinearColor(0.15f, 0.15f, 0.16f));
	Add(this, BarrierPivot, EShape::Cube, FVector(0.f, -350.f, 0.f), FRotator::ZeroRotator, FVector(0.12f, 7.f, 0.12f), FLinearColor(0.75f, 0.05f, 0.05f));
	for (int32 i = 0; i < 3; ++i)
	{
		Add(this, BarrierPivot, EShape::Cube, FVector(0.f, -120.f - 230.f * i, 0.f), FRotator::ZeroRotator, FVector(0.125f, 0.8f, 0.125f), FLinearColor(0.9f, 0.9f, 0.9f));
	}

	// Caseta con ventana iluminada, a un costado.
	Add(this, Root, EShape::Cube, FVector(-250.f, 620.f, 140.f), FRotator::ZeroRotator, FVector(2.4f, 2.4f, 2.8f), FLinearColor(0.2f, 0.22f, 0.2f), true);
	Add(this, Root, EShape::Cube, FVector(-250.f, 497.f, 170.f), FRotator::ZeroRotator, FVector(1.2f, 0.02f, 0.8f), FLinearColor(1.f, 0.75f, 0.3f));

	// Guardia: cuerpo, cabeza y gorra, mirando hacia la calzada.
	Add(this, Root, EShape::Cylinder, FVector(60.f, 520.f, 90.f), FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.9f), FLinearColor(0.08f, 0.14f, 0.1f), true);
	Add(this, Root, EShape::Sphere, FVector(60.f, 520.f, 192.f), FRotator::ZeroRotator, FVector(0.28f), FLinearColor(0.6f, 0.45f, 0.35f));
	Add(this, Root, EShape::Cylinder, FVector(60.f, 520.f, 212.f), FRotator::ZeroRotator, FVector(0.34f, 0.34f, 0.05f), FLinearColor(0.05f, 0.1f, 0.07f));
	Add(this, Root, EShape::Cube, FVector(60.f, 520.f, 148.f), FRotator::ZeroRotator, FVector(0.52f, 0.52f, 0.05f), FLinearColor(0.7f, 0.7f, 0.1f)); // chaleco reflectante
}

void ABlacklineGuardPost::SetBarrierOpen(bool bNewOpen)
{
	bOpen = bNewOpen;
	if (BarrierPivot)
	{
		// Rolar en negativo levanta el brazo (que cuelga hacia -Y).
		BarrierPivot->SetRelativeRotation(FRotator(0.f, 0.f, bOpen ? -80.f : 0.f));
	}
}
