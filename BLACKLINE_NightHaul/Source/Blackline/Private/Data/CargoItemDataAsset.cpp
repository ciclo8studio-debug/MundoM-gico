#include "Data/CargoItemDataAsset.h"

FPrimaryAssetId UCargoItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(FPrimaryAssetType(TEXT("CargoItem")), ItemId);
}
