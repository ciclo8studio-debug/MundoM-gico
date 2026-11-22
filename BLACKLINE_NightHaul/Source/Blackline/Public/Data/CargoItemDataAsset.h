// Blackline: Night Haul - definicion de un tipo de carga (CONTRATO.md).
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/BlacklineTypes.h"
#include "CargoItemDataAsset.generated.h"

class UStaticMesh;

UCLASS(BlueprintType)
class BLACKLINE_API UCargoItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	ECargoClass CargoClass = ECargoClass::Standard;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo", meta = (ClampMin = "0", Units = "kg"))
	float MassKg = 50.f;

	/** 0 = indestructible, 1 = se rompe con cualquier golpe. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo", meta = (ClampMin = "0", ClampMax = "1"))
	float Fragility01 = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo", meta = (ClampMin = "0", ClampMax = "1"))
	float ContrabandSignature01 = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	bool bRequiresShielding = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo", meta = (Units = "cm"))
	FVector BoxExtentCm = FVector(50.0, 50.0, 50.0);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo", meta = (ClampMin = "0"))
	float ValueCredits = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cargo")
	TSoftObjectPtr<UStaticMesh> Mesh;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
