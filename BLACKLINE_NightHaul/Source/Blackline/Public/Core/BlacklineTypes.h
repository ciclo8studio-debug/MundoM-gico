// Blackline: Night Haul - tipos compartidos (CONTRATO.md).
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "UObject/SoftObjectPtr.h"
#include "BlacklineTypes.generated.h"

class UCargoItemDataAsset;

UENUM(BlueprintType)
enum class ECargoClass : uint8 { Standard, Fragile, Volatile, Cryogenic, Relic, AIServer };

UENUM(BlueprintType)
enum class ECheckpointState : uint8 { Idle, Approaching, Inspecting, Cleared, PursuitTriggered };

UENUM(BlueprintType)
enum class EInspectionVerdict : uint8 { CleanPass, SecondaryInspection, PursuitAlert };

UENUM(BlueprintType)
enum class EDriveMode : uint8 { RWD, FourWheel };

UENUM(BlueprintType)
enum class EDashControl : uint8 { Headlights, Wipers, Window, SignalJammer, DiffLock, Ignition };

UENUM(BlueprintType)
enum class ECameraPerspective : uint8 { Cockpit, ThirdPerson };

UENUM(BlueprintType)
enum class ESurfaceKind : uint8 { Asphalt, WetAsphalt, Gravel, Mud, Forest };

UENUM(BlueprintType)
enum class EPursuitState : uint8 { None, Hunting, Searching, Lost };

USTRUCT(BlueprintType)
struct FSuspicionSnapshot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	float SpeedBeforeStopKmh = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	bool bHeadlightsOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	float DamageRatio01 = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	float UnshieldedSignature01 = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	bool bSignalJammerOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspicion")
	float CargoIntegrity01 = 1.f;
};

USTRUCT(BlueprintType)
struct FManifestLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manifest")
	TSoftObjectPtr<UCargoItemDataAsset> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manifest", meta = (ClampMin = "0"))
	int32 Quantity = 1;
};

USTRUCT(BlueprintType)
struct FCargoSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo")
	TObjectPtr<UCargoItemDataAsset> Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo")
	FVector LocalOffsetCm = FVector::ZeroVector;
};
