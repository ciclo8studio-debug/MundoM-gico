// Blackline: Night Haul - HUD principal. Sin Tick: delegados + timer de baja frecuencia.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/BlacklineTypes.h"
#include "BlacklineHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class ABlacklineVanPawn;
class ACheckpointController;
class UCargoSimulationComponent;
class UPursuitDirectorSubsystem;

UCLASS(Abstract, Blueprintable)
class BLACKLINE_API UBlacklineHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Intervalo (s) del sondeo de velocidad/heat/modo (no hay delegado para ellos). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|HUD", meta = (ClampMin = "0.02"))
	float PollIntervalSeconds = 0.1f;

	/** Integridad bajo este umbral se considera crítica (evento BP). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|HUD", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CargoCriticalThreshold01 = 0.35f;

	/** Re-vincula con la furgoneta actual (por si cambia la posesión). */
	UFUNCTION(BlueprintCallable, Category = "Blackline|HUD")
	void RebindToVan();

	/** Eventos opcionales para animación en BP. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Blackline|HUD")
	void BP_OnCargoCritical(float Integrity01);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blackline|HUD")
	void BP_OnCheckpointStateChanged(ECheckpointState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blackline|HUD")
	void BP_OnPursuitStateChanged(EPursuitState NewState);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// --- Widgets opcionales ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeedText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DriveModeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CargoIntegrityText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> CargoIntegrityBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CheckpointStateText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HeatText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HeatBar;

private:
	UFUNCTION()
	void HandleCargoIntegrityChanged(float Integrity01, float Delta);

	UFUNCTION()
	void HandleCheckpointStateChanged(ECheckpointState OldState, ECheckpointState NewState);

	UFUNCTION()
	void HandlePursuitStateChanged(EPursuitState NewState);

	void Poll();
	void BindVan(ABlacklineVanPawn* Van);
	void UnbindAll();
	void BindCheckpoints();
	void BindPursuit();

	void RefreshSpeed(float SpeedKmh);
	void RefreshDriveMode(EDriveMode Mode);
	void RefreshCargo(float Integrity01);
	void RefreshCheckpoint(ECheckpointState State);
	void RefreshHeat(float Heat01);

	TWeakObjectPtr<ABlacklineVanPawn> BoundVan;
	TWeakObjectPtr<UCargoSimulationComponent> BoundCargo;
	TWeakObjectPtr<UPursuitDirectorSubsystem> BoundPursuit;
	TArray<TWeakObjectPtr<ACheckpointController>> BoundCheckpoints;

	FTimerHandle PollTimer;

	// Cache para no reescribir texto sin cambios.
	int32 LastSpeedInt = -1;
	float LastHeat01 = -1.f;
	bool bHasDriveMode = false;
	EDriveMode LastDriveMode = EDriveMode::RWD;
	bool bCriticalLatched = false;
};
