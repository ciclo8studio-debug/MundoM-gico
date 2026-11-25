// Blackline: Night Haul - ayudante para armar modelos simples con las formas basicas del motor (sin assets propios).
#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace BlacklineShapes
{
	enum class EShape : uint8
	{
		Cube,
		Cylinder,
		Sphere,
		Cone
	};

	inline UStaticMesh* LoadShape(EShape Shape)
	{
		const TCHAR* Path = TEXT("/Engine/BasicShapes/Cube.Cube");
		switch (Shape)
		{
		case EShape::Cylinder: Path = TEXT("/Engine/BasicShapes/Cylinder.Cylinder"); break;
		case EShape::Sphere: Path = TEXT("/Engine/BasicShapes/Sphere.Sphere"); break;
		case EShape::Cone: Path = TEXT("/Engine/BasicShapes/Cone.Cone"); break;
		default: break;
		}
		return LoadObject<UStaticMesh>(nullptr, Path);
	}

	/** Crea un componente con la forma, sin colision ni sombra, hijo de Parent, con color. */
	inline UStaticMeshComponent* Add(AActor* Owner, USceneComponent* Parent, EShape Shape, const FVector& Loc, const FRotator& Rot,
		const FVector& Scale, const FLinearColor& Color, bool bCastShadow = false)
	{
		UStaticMesh* Mesh = Owner ? LoadShape(Shape) : nullptr;
		if (!Mesh)
		{
			return nullptr;
		}
		UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(Owner);
		C->SetStaticMesh(Mesh);
		C->SetMobility(EComponentMobility::Movable);
		C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		C->SetCastShadow(bCastShadow);
		C->SetupAttachment(Parent ? Parent : Owner->GetRootComponent());
		C->RegisterComponent();
		C->SetRelativeLocationAndRotation(Loc, Rot);
		C->SetRelativeScale3D(Scale);
		if (UMaterialInterface* Base = C->GetMaterial(0))
		{
			if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, C))
			{
				MID->SetVectorParameterValue(TEXT("Color"), Color);
				MID->SetVectorParameterValue(TEXT("BaseColor"), Color);
				C->SetMaterial(0, MID);
			}
		}
		return C;
	}
}
