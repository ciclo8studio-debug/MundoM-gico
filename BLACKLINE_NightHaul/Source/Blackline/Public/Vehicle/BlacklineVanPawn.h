// Blackline: Night Haul - Furgoneta jugable (Chaos Wheeled Vehicle). Sin Tick: solo timers y eventos.
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Core/BlacklineTypes.h"
#include "BlacklineVanPawn.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class ARoadsidePOI;
class UBlacklineProceduralAudio;
class UBlacklineCabinComponent;
class UBlacklineRainComponent;
class USpringArmComponent;
class USpotLightComponent;
class UCargoSimulationComponent;
class UChaosWheeledVehicleMovementComponent;
class UInputMappingContext;
class UInputAction;
class ATerrainModifierVolume;
class APlayerController;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDashControlChanged, EDashControl, Control, bool, bOn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVehicleStuckWarning, ATerrainModifierVolume*, Volume, bool, bStuck);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDriveModeChanged, EDriveMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVanFlipped);

UCLASS()
class BLACKLINE_API ABlacklineVanPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

public:
	ABlacklineVanPawn(const FObjectInitializer& ObjectInitializer);

	// ---- Componentes ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UCargoSimulationComponent> Cargo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UCameraComponent> CockpitCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<USpringArmComponent> ChaseArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UCameraComponent> ChaseCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<USpotLightComponent> HeadlightLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<USpotLightComponent> HeadlightRight;

	/** Audio sintetizado de la cabina: motor segun RPM, lluvia, viento y sirena. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UBlacklineProceduralAudio> ProceduralAudio;

	/** Cabina propia: tablero, volante, collar, pelota y foto de Pancito. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UBlacklineCabinComponent> CabinProps;

	/** Lluvia visible alrededor del furgon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UBlacklineRainComponent> RainFX;

	/** Ultimo acelerador recibido [0,1] (para el audio). */
	UFUNCTION(BlueprintPure, Category = "Blackline|Vehicle")
	float GetThrottleInput01() const { return FMath::Clamp(LastThrottleInput, 0.f, 1.f); }

	/** Carroceria visual (placeholder). La malla se asigna en el BP hijo. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UStaticMeshComponent> BodyVisual;

	/** Neumaticos visuales, anclados a los huesos VisWheel_* del esqueleto. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UStaticMeshComponent> WheelVisualFL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UStaticMeshComponent> WheelVisualFR;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UStaticMeshComponent> WheelVisualRL;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blackline|Components")
	TObjectPtr<UStaticMeshComponent> WheelVisualRR;

	// ---- Input (Enhanced Input) ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputMappingContext> DefaultIMC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	int32 IMCPriority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Throttle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Steer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Brake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Handbrake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Look;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_ToggleCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Interact;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_DriveMode;

	/** L / D-pad arriba: luces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_ToggleHeadlights;

	/** I / D-pad abajo: motor. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_ToggleEngine;

	/** J / D-pad izquierda: inhibidor GPS/EM. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_ToggleJammer;

	/** R / Select: volver a la carretera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Input")
	TObjectPtr<UInputAction> IA_Recover;

	// ---- Tuning ----
	/** Duracion de la transicion cockpit <-> chase (s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Camera", meta = (ClampMin = "0.05"))
	float CameraBlendTimeSec = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Camera")
	ECameraPerspective StartPerspective = ECameraPerspective::ThirdPerson; // placeholder: se ve el furgon hasta tener cabina propia

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Camera")
	float LookSensitivity = 1.0f;

	/** Giro maximo de cabeza en cockpit (grados). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Camera")
	float CockpitMaxYawDeg = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Camera")
	float CockpitMaxPitchDeg = 40.f;

	/** Alcance de la traza hacia el tablero (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Dash", meta = (ClampMin = "10"))
	float InteractRangeCm = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Dash")
	bool bStartEngineOn = true; // true mientras no exista el boton fisico Dash.Ignition en la cabina propia

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Dash")
	bool bStartHeadlightsOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Drive")
	EDriveMode StartDriveMode = EDriveMode::RWD;

	/** Reparto delantero/trasero del diferencial en 4WD suelto (0 = todo atras, 1 = todo delante). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Drive", meta = (ClampMin = "0", ClampMax = "1"))
	float FrontRearSplitUnlocked = 0.4f;

	/** Reparto en 4WD con bloqueo diferencial. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Drive", meta = (ClampMin = "0", ClampMax = "1"))
	float FrontRearSplitLocked = 0.5f;

	/** Factor extra de friccion cuando el terreno exige 4WD y no se tiene (atasco). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Terrain", meta = (ClampMin = "0.05", ClampMax = "1"))
	float StuckFrictionFactor = 0.35f;

	// ---- Sospecha / danos ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Suspicion", meta = (ClampMin = "0.05"))
	float SpeedSampleIntervalSec = 0.25f;

	/** Ventana (s) en la que se busca la velocidad maxima previa a detenerse. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Suspicion", meta = (ClampMin = "0.5"))
	float SpeedHistoryWindowSec = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Damage")
	float MinImpactSpeedMs = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Damage")
	float DamagePerImpactMs = 0.01f;

	/** Impulso lateral (N*s por unidad de severidad) al desplazarse la carga. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Cargo")
	float CargoShiftImpulseScale = 400000.f;

	// ---- Delegados ----
	UPROPERTY(BlueprintAssignable, Category = "Blackline|Events")
	FOnDashControlChanged OnDashControlChanged;

	UPROPERTY(BlueprintAssignable, Category = "Blackline|Events")
	FOnVehicleStuckWarning OnVehicleStuckWarning;

	UPROPERTY(BlueprintAssignable, Category = "Blackline|Events")
	FOnDriveModeChanged OnDriveModeChanged;

	// ---- API publica (contrato) ----
	UFUNCTION(BlueprintCallable, Category = "Blackline|Camera")
	void ToggleCamera();

	UFUNCTION(BlueprintPure, Category = "Blackline|Camera")
	ECameraPerspective GetCameraPerspective() const { return TargetPerspective; }

	UFUNCTION(BlueprintCallable, Category = "Blackline|Drive")
	void SetDriveMode(EDriveMode NewMode);

	UFUNCTION(BlueprintPure, Category = "Blackline|Drive")
	EDriveMode GetDriveMode() const { return DriveMode; }

	UFUNCTION(BlueprintPure, Category = "Blackline|Suspicion")
	FSuspicionSnapshot GetSuspicionSnapshot() const;

	UFUNCTION(BlueprintCallable, Category = "Blackline|Dash")
	void ActivateDashControl(EDashControl Control);

	UFUNCTION(BlueprintPure, Category = "Blackline|Dash")
	bool IsEngineOn() const { return bEngineOn; }

	UFUNCTION(BlueprintPure, Category = "Blackline|Dash")
	bool IsDiffLocked() const { return bDiffLocked; }

	UFUNCTION(BlueprintPure, Category = "Blackline|Vehicle")
	float GetSpeedKmh() const;

	/** RPM actuales del motor (0 si esta apagado). */
	UFUNCTION(BlueprintPure, Category = "Blackline|Vehicle")
	float GetEngineRpm() const;

	/** RPM normalizadas 0..1 respecto al maximo del motor (para MetaSounds/HUD). */
	UFUNCTION(BlueprintPure, Category = "Blackline|Vehicle")
	float GetEngineRpm01() const;

	UFUNCTION(BlueprintPure, Category = "Blackline|Dash")
	bool AreHeadlightsOn() const { return bHeadlightsOn; }

	UFUNCTION(BlueprintPure, Category = "Blackline|Dash")
	bool IsSignalJammerOn() const { return bSignalJammerOn; }

	/** Sigilo 0..1: 1 = muy dificil de detectar (motor apagado, luces apagadas, inhibidor activo, lento). */
	UFUNCTION(BlueprintPure, Category = "Blackline|Suspicion")
	float GetStealth01() const;

	// ---- Combustible, lugares de la ruta y mensajes ----
	UFUNCTION(BlueprintPure, Category = "Blackline|Fuel")
	float GetFuel01() const { return Fuel01; }

	UFUNCTION(BlueprintCallable, Category = "Blackline|Fuel")
	void AddFuel(float Amount01);

	UFUNCTION(BlueprintPure, Category = "Blackline|Suspicion")
	float GetDamage01() const { return DamageRatio01; }

	UFUNCTION(BlueprintCallable, Category = "Blackline|Suspicion")
	void RepairDamage(float Amount01);

	/** El lugar (bencinera/restaurante) cuya zona esta usando el jugador; E / Circulo interactua con el. */
	UFUNCTION(BlueprintCallable, Category = "Blackline|Interact")
	void SetActiveInteractable(ARoadsidePOI* POI);

	UFUNCTION(BlueprintCallable, Category = "Blackline|HUD")
	void ShowHudMessage(const FText& Text, float Seconds = 3.f);

	/** Mensaje actual del HUD (vacio si ya expiro). */
	UFUNCTION(BlueprintPure, Category = "Blackline|HUD")
	FText GetHudMessage() const;

	/** El furgon lleva volcado FlipResetSeconds sin moverse (lo escucha la ruta para devolverlo a la carretera). */
	UPROPERTY(BlueprintAssignable, Category = "Blackline|Events")
	FOnVanFlipped OnVanFlipped;

	/** Segundos volcado antes de recuperarlo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Flip", meta = (ClampMin = "0.5"))
	float FlipResetSeconds = 3.f;

	/** Se considera volcado si el eje Z del furgon apunta menos que esto hacia arriba (1 = derecho, 0 = de lado, -1 = patas arriba). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Flip", meta = (ClampMin = "-1", ClampMax = "1"))
	float FlipUpDotThreshold = 0.25f;

	/** Consumo: fraccion de tanque por km recorrido (1:100 => una zona de 7 km gasta ~55%). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Fuel", meta = (ClampMin = "0"))
	float FuelPerKm = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackline|Fuel", meta = (ClampMin = "0", ClampMax = "1"))
	float StartFuel01 = 0.8f;

	UFUNCTION(BlueprintCallable, Category = "Blackline|Terrain")
	void PushSurfaceModifier(ATerrainModifierVolume* Volume);

	UFUNCTION(BlueprintCallable, Category = "Blackline|Terrain")
	void PopSurfaceModifier(ATerrainModifierVolume* Volume);

	UFUNCTION(BlueprintCallable, Category = "Blackline|Dash")
	void OpenWindowForInspection(bool bOpen);

	/** Hook visual para BP (animar ventanilla, limpiaparabrisas, etc.). */
	UFUNCTION(BlueprintImplementableEvent, Category = "Blackline|Dash")
	void BP_OnDashVisual(EDashControl Control, bool bOn);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

private:
	// Input handlers
	void OnThrottle(const FInputActionValue& Value);
	void OnSteer(const FInputActionValue& Value);
	void OnBrake(const FInputActionValue& Value);
	void OnHandbrakeStarted(const FInputActionValue& Value);
	void OnHandbrakeCompleted(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnToggleCameraInput(const FInputActionValue& Value);
	void OnInteract(const FInputActionValue& Value);
	void OnToggleHeadlightsInput(const FInputActionValue& Value);
	void OnToggleEngineInput(const FInputActionValue& Value);
	void OnToggleJammerInput(const FInputActionValue& Value);
	void OnRecoverInput(const FInputActionValue& Value);
	void OnDriveModeInput(const FInputActionValue& Value);

	void RegisterIMC();

	/** Ultimo valor de acelerador recibido (solo para telemetria en log). */
	float LastThrottleInput = 0.f;
	int32 TelemetryCounter = 0;
	void UnregisterIMC();

	UChaosWheeledVehicleMovementComponent* GetChaosMovement() const;

	// Camara
	void StepCameraBlend();
	void ApplyCockpitLook();

	// Traccion / terreno
	void ApplyDriveConfig();
	void RecomputeSurfaceFriction();
	void CacheBaseWheelFriction();

	// Dash
	void SetHeadlights(bool bOn);
	void SetEngine(bool bOn);
	void NotifyPursuitEngineAndLights() const;
	void BroadcastDash(EDashControl Control, bool bOn);

	// Sospecha y carga
	void SampleSpeed();
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void HandleCargoShifted(float Severity01);
	UFUNCTION()
	void HandleCargoIntegrityChanged(float Integrity01, float Delta);

	// Estado
	EDriveMode DriveMode = EDriveMode::RWD;
	bool bEngineOn = false;
	bool bHeadlightsOn = true;
	bool bWipersOn = false;
	bool bWindowOpen = false;
	bool bSignalJammerOn = false;
	bool bDiffLocked = false;
	bool bHandbrakeHeld = false;
	bool bStuckWarningActive = false;

	float DamageRatio01 = 0.f;
	float Fuel01 = 0.8f;
	float FlippedSeconds = 0.f;
	bool bOutOfFuelWarned = false;
	TWeakObjectPtr<ARoadsidePOI> ActivePOI;
	FText HudMessage;
	double HudMessageExpire = 0.0;
	float CockpitLookYaw = 0.f;
	float CockpitLookPitch = 0.f;

	/** 0 = cockpit, 1 = chase. */
	float CameraBlendAlpha = 0.f;
	ECameraPerspective TargetPerspective = ECameraPerspective::Cockpit;
	FTimerHandle CameraBlendTimer;

	FTimerHandle SpeedSampleTimer;
	TArray<float> SpeedHistoryKmh;
	int32 SpeedHistoryHead = 0;

	TArray<TWeakObjectPtr<ATerrainModifierVolume>> ActiveModifiers;
	TArray<float> BaseWheelFriction;

	TWeakObjectPtr<APlayerController> IMCController;
};
