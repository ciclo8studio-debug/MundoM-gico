#include "World/TrafficCar.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ATrafficCar::ATrafficCar()
{
	PrimaryActorTick.bCanEverTick = false; // lo mueve la ruta con un timer unico

	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
	SetRootComponent(Body);
	Body->SetBoxExtent(FVector(230.f, 95.f, 70.f));
	// Sin colision fisica: un cuerpo movido a saltos por timer lanzaba al furgon a 300 km/h. El choque lo detecta la ruta.
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Body->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MatFinder.Succeeded())
	{
		BaseMaterial = MatFinder.Object;
	}

	auto MakePart = [&](const TCHAR* Name, const FVector& Pos, const FVector& SizeCm) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* C = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		C->SetupAttachment(Body);
		if (CubeFinder.Succeeded())
		{
			C->SetStaticMesh(CubeFinder.Object);
		}
		C->SetRelativeLocation(Pos);
		C->SetRelativeScale3D(SizeCm / 100.f);
		C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		C->SetCastShadow(false);
		return C;
	};
	Shell = MakePart(TEXT("Shell"), FVector(0, 0, -20), FVector(460, 190, 90));
	Cabin = MakePart(TEXT("Cabin"), FVector(-30, 0, 50), FVector(240, 170, 70));

	HeadLeft = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadLeft"));
	HeadLeft->SetupAttachment(Body);
	HeadLeft->SetRelativeLocation(FVector(235, -65, 0));
	HeadLeft->SetIntensity(6000.f);
	HeadLeft->SetOuterConeAngle(32.f);
	HeadLeft->SetAttenuationRadius(3500.f);
	HeadLeft->SetCastShadows(false);

	HeadRight = CreateDefaultSubobject<USpotLightComponent>(TEXT("HeadRight"));
	HeadRight->SetupAttachment(Body);
	HeadRight->SetRelativeLocation(FVector(235, 65, 0));
	HeadRight->SetIntensity(6000.f);
	HeadRight->SetOuterConeAngle(32.f);
	HeadRight->SetAttenuationRadius(3500.f);
	HeadRight->SetCastShadows(false);

	Tail = CreateDefaultSubobject<UPointLightComponent>(TEXT("Tail"));
	Tail->SetupAttachment(Body);
	Tail->SetRelativeLocation(FVector(-240, 0, 10));
	Tail->SetLightColor(FLinearColor::Red);
	Tail->SetIntensity(800.f);
	Tail->SetAttenuationRadius(500.f);
	Tail->SetCastShadows(false);
}

void ATrafficCar::SetBodyColor(const FLinearColor& Color)
{
	if (!BaseMaterial || !Shell)
	{
		return;
	}
	if (!ShellMaterial)
	{
		ShellMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		Shell->SetMaterial(0, ShellMaterial);
		if (Cabin)
		{
			Cabin->SetMaterial(0, ShellMaterial);
		}
	}
	ShellMaterial->SetVectorParameterValue(TEXT("Color"), Color);
}
