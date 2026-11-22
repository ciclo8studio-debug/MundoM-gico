// Blackline: Night Haul - Director de persecucion (WorldSubsystem sin Tick; usa FTimerManager).
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/BlacklineTypes.h"
#include "Pursuit/PursuitSettings.h"
#include "PursuitDirectorSubsystem.generated.h"

class APawn;
struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPursuitStateChanged, EPursuitState, NewState);

UCLASS()
class BLACKLINE_API UPursuitDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Devuelve el subsistema del mundo del objeto de contexto (o nullptr). */
	static UPursuitDirectorSubsystem* Get(const UObject* WorldContextObject);

	/** Inicia (o reinicia) una persecucion contra Target. Level >= 1. */
	UFUNCTION(BlueprintCallable, Category = "Pursuit")
	void StartPursuit(AActor* Target, int32 Level);

	/** Cancela la persecucion y destruye las patrullas. */
	UFUNCTION(BlueprintCallable, Category = "Pursuit")
	void StopPursuit();

	/** Aviso externo (guardia, percepcion) de que el objetivo esta a la vista. */
	UFUNCTION(BlueprintCallable, Category = "Pursuit")
	void ReportTargetVisible(bool bVisible);

	/** Estado del vehiculo; motor y luces apagados enfrian mas rapido el calor. */
	UFUNCTION(BlueprintCallable, Category = "Pursuit")
	void ReportEngineAndLights(bool bEngineOn, bool bLightsOn);

	UFUNCTION(BlueprintPure, Category = "Pursuit")
	EPursuitState GetState() const { return State; }

	UFUNCTION(BlueprintPure, Category = "Pursuit")
	float GetHeat01() const { return Heat01; }

	UPROPERTY(BlueprintAssignable, Category = "Pursuit")
	FOnPursuitStateChanged OnPursuitStateChanged;

	// UWorldSubsystem
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	virtual void Deinitialize() override;

private:
	void SetState(EPursuitState NewState);
	void HeatTick();
	void EnterLost();
	void CleanupRuntime();
	void OnPatrolClassLoaded(int32 InSerial);
	void SpawnPatrols();
	bool FindSpawnLocation(const FVector& TargetLocation, const FVector& TargetForward, FVector& OutLocation) const;
	bool PatrolSeesTarget(const APawn* Patrol, bool& bOutHeard) const;
	void IssueOrders(float Now);

	EPursuitState State = EPursuitState::None;
	float Heat01 = 0.0f;
	bool bReportedVisible = false;
	bool bEngineOn = true;
	bool bLightsOn = true;
	float LastSeenTime = 0.0f;
	float LastOrderTime = -1000.0f;
	int32 Serial = 0;

	FVector LastKnownTargetLocation = FVector::ZeroVector;
	FPursuitLevelConfig ActiveConfig;

	UPROPERTY(Transient)
	TObjectPtr<AActor> Target;

	UPROPERTY(Transient)
	TArray<TObjectPtr<APawn>> Patrols;

	FTimerHandle HeatTimer;
	FTimerHandle LostCleanupTimer;
	TSharedPtr<FStreamableHandle> PatrolLoadHandle;
};
