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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDogWellbeingChanged, float, Wellbeing01);

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

	/** Suma o resta viaticos (no baja de 0). */
	UFUNCTION(BlueprintCallable, Category = "Contracts")
	void AddCredits(int32 Delta);

	// ---- Pancito (el perro de Enzo): lo que esta en juego ----
	/** Bienestar de Pancito [0,1]. Sube con entregas a tiempo; baja con atrasos y cargas perdidas. A 0 es el final malo. */
	UFUNCTION(BlueprintPure, Category = "Contracts|Cofradia")
	float GetDogWellbeing01() const { return DogWellbeing01; }

	UFUNCTION(BlueprintCallable, Category = "Contracts|Cofradia")
	void AddDogWellbeing(float Delta01);

	/** Plazo del encargo activo (segundos). Al vencer, Pancito pierde LatePenalty. */
	UFUNCTION(BlueprintCallable, Category = "Contracts|Cofradia")
	void StartDeadline(float Seconds);

	/** Segundos que quedan (negativo si va atrasado; 0 si no hay plazo). */
	UFUNCTION(BlueprintPure, Category = "Contracts|Cofradia")
	float GetDeadlineRemainingSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Contracts|Cofradia")
	bool HasDeadline() const { return bHasDeadline; }

	UFUNCTION(BlueprintPure, Category = "Contracts|Cofradia")
	bool IsContractLate() const { return bHasDeadline && GetDeadlineRemainingSeconds() < 0.f; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contracts|Cofradia", meta = (ClampMin = "0", ClampMax = "1"))
	float StartDogWellbeing01 = 0.8f;

	/** Bienestar que gana una entrega a tiempo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contracts|Cofradia", meta = (ClampMin = "0", ClampMax = "1"))
	float OnTimeDeliveryBonus = 0.10f;

	/** Bienestar que pierde un contrato fallido (carga destruida). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contracts|Cofradia", meta = (ClampMin = "0", ClampMax = "1"))
	float FailurePenalty = 0.25f;

	/** Bienestar que se pierde al vencer el plazo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contracts|Cofradia", meta = (ClampMin = "0", ClampMax = "1"))
	float LatePenalty = 0.15f;

	UPROPERTY(BlueprintAssignable, Category = "Contracts|Cofradia")
	FOnDogWellbeingChanged OnDogWellbeingChanged;

	UPROPERTY(BlueprintAssignable, Category = "Contracts")
	FOnContractStarted OnContractStarted;

	UPROPERTY(BlueprintAssignable, Category = "Contracts")
	FOnContractFinished OnContractFinished;

private:
	UDataTable* ResolveTable() const;

	void HandleDeadlineExpired();

	float DogWellbeing01 = 0.8f;
	double DeadlineEndTime = 0.0;
	bool bHasDeadline = false;
	FTimerHandle DeadlineTimer;

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
