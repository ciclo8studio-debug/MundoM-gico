// Blackline: Night Haul - Checkpoint de inspeccion (maquina de estados, sin Tick).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BlacklineTypes.h"
#include "Core/BlacklineContractTypes.h"
#include "CheckpointController.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpotLightComponent;
class USceneComponent;
class UManifestDataAsset;
class ABlacklineVanPawn;
class UPrimitiveComponent;
struct FHitResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCheckpointStateChanged, ECheckpointState, OldState, ECheckpointState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInspectionVerdict, EInspectionVerdict, Verdict, float, Suspicion01);

/** Pesos de la sospecha. Cada factor va de 0 a 1 y se multiplica por su peso; la suma se recorta a 0..1. */
USTRUCT(BlueprintType)
struct FSuspicionWeights
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float SpeedBeforeStop = 0.10f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float HeadlightsOff = 0.10f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float Damage = 0.15f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float UnshieldedSignature = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float SignalJammer = 0.20f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float CargoIntegrityLoss = 0.10f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float MassMismatch = 0.30f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float SignatureMismatch = 0.30f;
	/** Se aplica completo si el conductor no presenta manifiesto a tiempo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0")) float NoManifest = 0.50f;
};

UCLASS(Blueprintable)
class BLACKLINE_API ACheckpointController : public AActor
{
	GENERATED_BODY()

public:
	ACheckpointController();

	// ---- Componentes (contrato) ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<UBoxComponent> InspectionZone;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<UStaticMeshComponent> Barrier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<USpotLightComponent> InspectionLight;

	/** Punto donde aparece el guardia si GuardClass esta definido y GuardActor no. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	TObjectPtr<USceneComponent> GuardMarker;

	// ---- API (contrato) ----
	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	ECheckpointState GetState() const { return State; }

	/** Solo tiene efecto en estado Inspecting (ventanilla abierta). */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void PresentManifest(UManifestDataAsset* Manifest);

	/** Aplica el contrato activo: exige RequiredManifestLevel y registra el nombre para logs. */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint|Contract")
	void ApplyContract(const FBlacklineContractRow& Row);

	UFUNCTION(BlueprintPure, Category = "Checkpoint|Contract")
	int32 GetRequiredManifestLevel() const { return RequiredManifestLevel; }

	/** Nivel (1-3) que cubre un manifiesto segun su calidad de falsificacion. */
	UFUNCTION(BlueprintPure, Category = "Checkpoint|Contract")
	static int32 ManifestLevelFromQuality(float ForgeryQuality01);

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	float GetSuspicion01() const;

	/** Vuelve a Idle, cierra la barrera y apaga la luz. */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void ResetCheckpoint();

	UPROPERTY(BlueprintAssignable, Category = "Checkpoint")
	FOnCheckpointStateChanged OnCheckpointStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Checkpoint")
	FOnInspectionVerdict OnInspectionVerdict;

	/** Hook de diseno: animar al guardia segun el veredicto. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Checkpoint")
	void OnGuardReact(AActor* Guard, EInspectionVerdict Verdict, float Suspicion01);

	// ---- Guardia ----
	/** Guardia ya colocado en el nivel (Actor o Pawn). Tiene prioridad sobre GuardClass. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Checkpoint|Guard")
	TObjectPtr<AActor> GuardActor;

	/** Si GuardActor es nulo, se crea esta clase (Blueprint) en GuardMarker. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Guard")
	TSubclassOf<AActor> GuardClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Guard")
	bool bGuardFacesVan = true;

	// ---- Deteccion de detencion ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Stop", meta = (ClampMin = "0.02"))
	float PollIntervalSeconds = 0.1f;

	/** Velocidad bajo la cual se considera detenido. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Stop", meta = (ClampMin = "0"))
	float StopSpeedKmh = 2.0f;

	/** Tiempo continuo bajo StopSpeedKmh para contar como detencion completa. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Stop", meta = (ClampMin = "0"))
	float StopHoldSeconds = 1.0f;

	/** Si el van sale de la zona sin detenerse o sin veredicto, se dispara la persecucion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Stop")
	bool bPursuitOnRunThrough = true;

	// ---- Inspeccion ----
	/** Tiempo maximo esperando el manifiesto desde que se abre la ventanilla. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float ManifestTimeoutSeconds = 20.0f;

	/** Pausa entre presentar el manifiesto y el veredicto. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float VerdictDelaySeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float SecondaryInspectionSeconds = 6.0f;

	/** Multiplica los terminos de manifiesto durante la inspeccion secundaria. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "1"))
	float SecondaryScrutinyMultiplier = 1.5f;

	/** Moverse mas rapido que esto durante la inspeccion es fuga: alerta inmediata. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float BreakAwaySpeedKmh = 15.0f;

	/** Moverse mas rapido que esto (pero menos que la fuga) acumula sospecha. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float MoveDuringInspectionKmh = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float MovePenaltyPerSecond = 0.5f;

	/** Sospecha acumulada por segundo mientras el inhibidor de senal esta encendido. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Inspection", meta = (ClampMin = "0"))
	float JammerAccumulationPerSecond = 0.05f;

	// ---- Sospecha ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion")
	FSuspicionWeights Weights;

	/** Nivel de documento exigido por el contrato activo (0 = sin exigencia). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Contract", meta = (ClampMin = "0", ClampMax = "3"))
	int32 RequiredManifestLevel = 0;

	/** Sospecha extra por cada nivel de documento que falta (se multiplica por el escrutinio). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Contract", meta = (ClampMin = "0", ClampMax = "1"))
	float ManifestLevelDeficitPenalty = 0.30f;

	/** Sospecha >= este valor: inspeccion secundaria. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0", ClampMax = "1"))
	float SecondaryThreshold01 = 0.30f;

	/** Sospecha >= este valor: alerta de persecucion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0", ClampMax = "1"))
	float PursuitThreshold01 = 0.65f;

	/** Velocidad de llegada que empieza a sumar sospecha. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0"))
	float SpeedSuspicionStartKmh = 40.0f;

	/** Velocidad de llegada que suma sospecha completa. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "1"))
	float SpeedSuspicionFullKmh = 100.0f;

	/** Diferencia relativa de masa tolerada sin sospecha. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0"))
	float MassToleranceRatio = 0.10f;

	/** Diferencia relativa de masa que cuenta como discrepancia total. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0.01"))
	float MassMismatchFullRatio = 0.50f;

	/** Diferencia de firma (0..1) tolerada sin sospecha. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0", ClampMax = "1"))
	float SignatureToleranceDelta = 0.05f;

	/** Cuanto de la discrepancia oculta una falsificacion perfecta (ForgeryQuality01 = 1). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Suspicion", meta = (ClampMin = "0", ClampMax = "1"))
	float ForgeryMitigation = 0.6f;

	// ---- Persecucion ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Pursuit", meta = (ClampMin = "1"))
	int32 PursuitLevel = 1;

	// ---- Barrera y luz ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Barrier")
	float BarrierOpenPitchDeg = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Barrier", meta = (ClampMin = "0.05"))
	float BarrierMoveSeconds = 1.5f;

	/** Segundos tras salir de la zona (Cleared) antes de volver a Idle. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Barrier", meta = (ClampMin = "0"))
	float ResetDelayAfterLeavingSeconds = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Light")
	FLinearColor LightColorInspecting = FLinearColor(1.0f, 0.85f, 0.4f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Light")
	FLinearColor LightColorCleared = FLinearColor(0.2f, 1.0f, 0.3f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint|Light")
	FLinearColor LightColorAlert = FLinearColor(1.0f, 0.1f, 0.1f);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetState(ECheckpointState NewState);
	void Poll();
	void BeginInspection();
	void ResolveInspection();
	void ApplyVerdict(EInspectionVerdict Verdict);
	float ComputeStaticSuspicion(bool bScrutiny) const;
	void SetLight(bool bOn, const FLinearColor& Color);
	void SetBarrierOpen(bool bOpen);
	void StepBarrier();
	void ClearInspectionTimers();

	ECheckpointState State = ECheckpointState::Idle;

	UPROPERTY(Transient)
	TObjectPtr<ABlacklineVanPawn> ActiveVan;

	UPROPERTY(Transient)
	TObjectPtr<UManifestDataAsset> PresentedManifest;

	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedGuard;

	FTimerHandle PollTimer;
	FTimerHandle VerdictTimer;
	FTimerHandle BarrierTimer;
	FTimerHandle ResetTimer;

	float StopStartTime = -1.0f;
	float InspectionStartTime = 0.0f;
	float ApproachPeakSpeedKmh = 0.0f;
	float CachedSpeedBeforeStopKmh = 0.0f;
	float StaticSuspicion = 0.0f;
	float DynamicSuspicion = 0.0f;
	bool bVerdictScheduled = false;
	bool bSecondaryPhase = false;

	float BarrierAlpha = 0.0f;
	float BarrierTarget = 0.0f;
	FRotator BarrierClosedRotation = FRotator::ZeroRotator;
};
