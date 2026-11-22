// Blackline: Night Haul - simulacion de carga (CoM ponderado por masa, fuerza G, impactos).
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/BlacklineTypes.h"
#include "Core/BlacklineContractTypes.h"
#include "CargoSimulationComponent.generated.h"

class UCargoItemDataAsset;
class UPrimitiveComponent;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCargoIntegrityChanged, float, Integrity01, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCargoShifted, float, Severity01);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCargoDestroyed);

UCLASS(ClassGroup = (Blackline), meta = (BlueprintSpawnableComponent))
class BLACKLINE_API UCargoSimulationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCargoSimulationComponent();

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	bool AddCargo(UCargoItemDataAsset* Item, FVector LocalOffsetCm);

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	void ClearCargo();

	/** Reemplaza la carga por la del contrato: masa, fragilidad, firma EM y asimetria lateral. */
	UFUNCTION(BlueprintCallable, Category = "Cargo|Contract")
	bool LoadContract(const FBlacklineContractRow& Row);

	/** Busca la fila RowName en el DataTable y la carga. */
	UFUNCTION(BlueprintCallable, Category = "Cargo|Contract")
	bool LoadContractFromTable(UDataTable* Table, FName RowName);

	UFUNCTION(BlueprintPure, Category = "Cargo|Contract")
	FName GetActiveContractId() const { return ActiveContractId; }

	/** true si la integridad llego a ~0 (carga perdida). */
	UFUNCTION(BlueprintPure, Category = "Cargo")
	bool IsCargoDestroyed() const { return bDestroyedBroadcast; }

	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetTotalCargoMassKg() const;

	/** CoM de la carga en espacio local del dueno (cm), incluye deslizamiento dinamico. */
	UFUNCTION(BlueprintPure, Category = "Cargo")
	FVector GetCargoCenterOfMassLocalCm() const;

	/** Integridad media ponderada por masa, [0,1]. 1 sin carga. */
	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetCargoIntegrity01() const;

	/** Suma de firma de contrabando de items que requieren blindaje, si no hay blindaje. [0,1]. */
	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetUnshieldedSignature01() const;

	/** Fuerza G lateral con signo (+ = hacia la derecha del vehiculo). */
	UFUNCTION(BlueprintPure, Category = "Cargo")
	float GetLateralG() const { return LateralG; }

	UFUNCTION(BlueprintCallable, Category = "Cargo")
	void ApplyImpact(float ImpactSpeedMs, FVector LocalNormal);

	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnCargoIntegrityChanged OnCargoIntegrityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnCargoShifted OnCargoShifted;

	/** Se dispara una vez cuando la integridad cae bajo DestroyedThreshold01. */
	UPROPERTY(BlueprintAssignable, Category = "Cargo")
	FOnCargoDestroyed OnCargoDestroyed;

	// ---- Configuracion (EditAnywhere) ----
	/** Integridad bajo la cual la carga se considera destruida. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Config", meta = (ClampMin = "0", ClampMax = "0.5"))
	float DestroyedThreshold01 = 0.02f;

	/** Posicion longitudinal local de la carga del contrato (cm; negativo = hacia atras). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Contract")
	float ContractCargoXCm = -120.f;

	/** Altura local de la carga del contrato (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Contract")
	float ContractCargoZCm = 60.f;

	/** Desplazamiento lateral maximo (cm) cuando LateralBias = +-1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Contract", meta = (ClampMin = "0"))
	float ContractLateralSpanCm = 45.f;

	/** Umbral de firma EM bajo el cual la carga no exige blindaje. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Contract", meta = (ClampMin = "0", ClampMax = "1"))
	float ContractShieldingSignatureMin = 0.05f;


	/** Hay blindaje instalado: anula la firma sin blindar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Config")
	bool bCargoShielded = false;

	/** Aplicar SetCenterOfMass al cuerpo del dueno. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cargo|Config")
	bool bApplyCenterOfMass = true;

	/** Si true, la masa del vehiculo se lee del cuerpo fisico en BeginPlay. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config")
	bool bReadVehicleMassOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "1", Units = "kg"))
	float VehicleMassKg = 2200.f;

	/** CoM base del vehiculo vacio (local, cm). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config")
	FVector VehicleBaseCenterOfMassLocalCm = FVector::ZeroVector;

	/** Frecuencia del tick (Hz). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "1", ClampMax = "60"))
	float TickRateHz = 20.f;

	/** G lateral a partir de la cual la carga empieza a desplazarse. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0"))
	float ShiftThresholdG = 0.35f;

	/** G lateral donde la severidad llega a 1. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0.01"))
	float ShiftMaxG = 1.2f;

	/** Deslizamiento lateral maximo de la carga (cm). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0", Units = "cm"))
	float MaxSlideCm = 30.f;

	/** Velocidad de interpolacion del deslizamiento (1/s). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0.1"))
	float SlideInterpSpeed = 4.f;

	/** Suavizado de la G lateral (1/s). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0.1"))
	float GSmoothingSpeed = 8.f;

	/** Peso de la aceleracion lateral por derivada de velocidad lateral frente a la centripeta. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0", ClampMax = "1"))
	float LateralVelocityDerivativeWeight = 0.5f;

	/** Dano de integridad/segundo a severidad 1 y Fragility 1. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0"))
	float ShiftDamagePerSecond = 0.08f;

	/** Minimo de severidad para disparar OnCargoShifted. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0", ClampMax = "1"))
	float ShiftEventMinSeverity = 0.1f;

	/** Tiempo minimo entre eventos OnCargoShifted (s). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0"))
	float ShiftEventCooldownSeconds = 0.75f;

	/** Velocidad de impacto bajo la cual no hay dano (m/s). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0", Units = "m/s"))
	float ImpactSpeedThresholdMs = 3.f;

	/** Velocidad de impacto de referencia que causa dano 1 a Fragility 1 (m/s). */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0.1", Units = "m/s"))
	float ImpactReferenceSpeedMs = 25.f;

	/** Cambio minimo de CoM (cm) para reaplicar SetCenterOfMass. */
	UPROPERTY(EditAnywhere, Category = "Cargo|Config", meta = (ClampMin = "0"))
	float CenterOfMassApplyToleranceCm = 0.5f;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(Transient)
	FName ActiveContractId;

	bool bDestroyedBroadcast = false;

	UPROPERTY(Transient)
	TArray<FCargoSlot> Slots;

	/** Integridad por slot, paralela a Slots. */
	TArray<float> SlotIntegrity;

	float LateralG = 0.f;
	float PreviousLateralVelocityCmS = 0.f;
	bool bHasPreviousVelocity = false;
	float CurrentSlideCm = 0.f;
	float LastShiftEventTime = -1000.f;
	FVector LastAppliedCoMNudge = FVector(FLT_MAX);

	void UpdateLateralG(float DeltaTime);
	void UpdateShift(float DeltaTime);
	void ApplyDamageToSlots(float Damage01, float& OutDelta);
	void BroadcastIntegrityIfChanged(float OldIntegrity);
	void ApplyCenterOfMassToOwner();
	UPrimitiveComponent* GetOwnerBody() const;
};
