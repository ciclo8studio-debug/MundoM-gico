// Blackline: Night Haul - subsistema de contratos (tablet de la cabina).
// Carga DT_BlacklineContracts, guarda el contrato activo y lo propaga al furgon y a los retenes.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DeveloperSettings.h"
#include "Core/BlacklineContractTypes.h"
#include "BlacklineContractSubsystem.generated.h"

class UDataTable;

/** Ajustes de proyecto (Project Settings > Game > Blackline Contracts). */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Blackline Contracts"))
class BLACKLINE_API UBlacklineContractSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UBlacklineContractSettings();

	/** DataTable con filas FBlacklineContractRow. */
	UPROPERTY(Config, EditAnywhere, Category = "Contracts", meta = (AllowedClasses = "/Script/Engine.DataTable"))
	TSoftObjectPtr<UDataTable> ContractTable;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContractStarted, FName, ContractID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnContractFinished, FName, ContractID);

UCLASS()
class BLACKLINE_API UBlacklineContractSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Todos los contratos (ordenados por ContractID). */
	UFUNCTION(BlueprintCallable, Category = "Contracts")
	TArray<FBlacklineContractRow> GetAllContracts() const;

	/** Contratos de un arco (1-5), para la pantalla del broker. */
	UFUNCTION(BlueprintCallable, Category = "Contracts")
	TArray<FBlacklineContractRow> GetContractsByArc(int32 Arc) const;

	UFUNCTION(BlueprintCallable, Category = "Contracts")
	bool GetContract(FName ContractID, FBlacklineContractRow& OutRow) const;

	/** Activa el contrato: carga la carga del furgon y exige el documento en todos los retenes del mundo. */
	UFUNCTION(BlueprintCallable, Category = "Contracts")
	bool StartContract(FName ContractID);

	/** Cierra el contrato activo; bSuccess=false si la carga fue destruida o fallo la entrega. */
	UFUNCTION(BlueprintCallable, Category = "Contracts")
	void FinishContract(bool bSuccess);

	UFUNCTION(BlueprintPure, Category = "Contracts")
	bool HasActiveContract() const { return !ActiveContractId.IsNone(); }

	UFUNCTION(BlueprintPure, Category = "Contracts")
	FName GetActiveContractId() const { return ActiveContractId; }

	/** Creditos acumulados por contratos completados. */
	UFUNCTION(BlueprintPure, Category = "Contracts")
	int32 GetCredits() const { return Credits; }

	UPROPERTY(BlueprintAssignable, Category = "Contracts")
	FOnContractStarted OnContractStarted;

	UPROPERTY(BlueprintAssignable, Category = "Contracts")
	FOnContractFinished OnContractFinished;

private:
	UDataTable* ResolveTable() const;

	/** Handler: la carga se destruyo -> el contrato falla. */
	UFUNCTION()
	void HandleCargoDestroyed();

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CachedTable;

	UPROPERTY(Transient)
	FName ActiveContractId;

	UPROPERTY(Transient)
	int32 ActivePayout = 0;

	UPROPERTY(Transient)
	int32 Credits = 0;

	UPROPERTY(Transient)
	TWeakObjectPtr<class UCargoSimulationComponent> BoundCargo;
};
