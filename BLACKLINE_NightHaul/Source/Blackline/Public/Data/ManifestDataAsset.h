// Blackline: Night Haul - manifiesto declarado al control (CONTRATO.md).
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/BlacklineTypes.h"
#include "ManifestDataAsset.generated.h"

UCLASS(BlueprintType)
class BLACKLINE_API UManifestDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest")
	FName ManifestId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest")
	FText CompanyName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest")
	TArray<FManifestLine> DeclaredCargo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest", meta = (ClampMin = "0", ClampMax = "1"))
	float ForgeryQuality01 = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest")
	FText RouteCode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Manifest", meta = (MultiLine = "true"))
	FText Notes;

	/** Masa total declarada (suma MassKg * Quantity). Carga items de forma sincrona. */
	UFUNCTION(BlueprintCallable, Category = "Manifest")
	float ComputeDeclaredMassKg() const;

	/** Firma de contrabando declarada, saturada a [0,1]. Carga items de forma sincrona. */
	UFUNCTION(BlueprintCallable, Category = "Manifest")
	float ComputeDeclaredSignature01() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
