#include "Data/ManifestDataAsset.h"
#include "Data/CargoItemDataAsset.h"

float UManifestDataAsset::ComputeDeclaredMassKg() const
{
	float Total = 0.f;
	for (const FManifestLine& Line : DeclaredCargo)
	{
		if (const UCargoItemDataAsset* Item = Line.Item.LoadSynchronous())
		{
			Total += Item->MassKg * FMath::Max(0, Line.Quantity);
		}
	}
	return Total;
}

float UManifestDataAsset::ComputeDeclaredSignature01() const
{
	float Total = 0.f;
	for (const FManifestLine& Line : DeclaredCargo)
	{
		if (const UCargoItemDataAsset* Item = Line.Item.LoadSynchronous())
		{
			Total += Item->ContrabandSignature01 * FMath::Max(0, Line.Quantity);
		}
	}
	return FMath::Clamp(Total, 0.f, 1.f);
}

FPrimaryAssetId UManifestDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(TEXT("Manifest")), ManifestId);
}
