# CONTRATO DE INTERFACES — BLACKLINE: NIGHT HAUL (UE 5.8.4, C++)

Proyecto independiente de Mundo Mágico. Todo vive en `BLACKLINE_NightHaul/`. No tocar nada fuera.
UE 5.8.4 es posterior a tu conocimiento: verifica en `docs/engine-reference/unreal/` y marca APIs dudosas con `// VERIFY-5.8`.
Módulo: `Blackline` (API macro `BLACKLINE_API`). Código en `Source/Blackline/Public|Private/<Subcarpeta>/`. Docs en `docs/NN_tema.md` (español). Log: `DECLARE_LOG_CATEGORY_EXTERN(LogBlackline, Log, All)` en `Blackline.h`.
Dependencias Build.cs: Core, CoreUObject, Engine, InputCore, EnhancedInput, ChaosVehicles, PhysicsCore, Niagara, AIModule, GameplayTags, UMG.

## Tipos (`Core/BlacklineTypes.h`)
```
UENUM(BlueprintType) enum class ECargoClass : uint8 { Standard, Fragile, Volatile, Cryogenic, Relic, AIServer };
UENUM(BlueprintType) enum class ECheckpointState : uint8 { Idle, Approaching, Inspecting, Cleared, PursuitTriggered };
UENUM(BlueprintType) enum class EInspectionVerdict : uint8 { CleanPass, SecondaryInspection, PursuitAlert };
UENUM(BlueprintType) enum class EDriveMode : uint8 { RWD, FourWheel };
UENUM(BlueprintType) enum class EDashControl : uint8 { Headlights, Wipers, Window, SignalJammer, DiffLock, Ignition };
UENUM(BlueprintType) enum class ECameraPerspective : uint8 { Cockpit, ThirdPerson };
UENUM(BlueprintType) enum class ESurfaceKind : uint8 { Asphalt, WetAsphalt, Gravel, Mud, Forest };
UENUM(BlueprintType) enum class EPursuitState : uint8 { None, Hunting, Searching, Lost };
USTRUCT(BlueprintType) struct FSuspicionSnapshot { float SpeedBeforeStopKmh=0; bool bHeadlightsOn=true; float DamageRatio01=0; float UnshieldedSignature01=0; bool bSignalJammerOn=false; float CargoIntegrity01=1; };
USTRUCT(BlueprintType) struct FManifestLine { TSoftObjectPtr<UCargoItemDataAsset> Item; int32 Quantity=1; };
USTRUCT(BlueprintType) struct FCargoSlot { TObjectPtr<UCargoItemDataAsset> Item; FVector LocalOffsetCm; };
```
Canal de traza para botones del tablero: `ECC_GameTraceChannel1` (nombre "Interact"). Tags de componente de botones: `Dash.Headlights`, `Dash.Wipers`, `Dash.Window`, `Dash.SignalJammer`, `Dash.DiffLock`, `Dash.Ignition`.

## Clases y API pública mínima (no cambiar firmas; se puede añadir)
**UCargoItemDataAsset : UPrimaryDataAsset** (`Data/`): `FName ItemId; FText DisplayName; ECargoClass CargoClass; float MassKg; float Fragility01; float ContrabandSignature01; bool bRequiresShielding; FVector BoxExtentCm; float ValueCredits; TSoftObjectPtr<UStaticMesh> Mesh;` GetPrimaryAssetId → ("CargoItem", ItemId).
**UManifestDataAsset : UPrimaryDataAsset** (`Data/`): `FName ManifestId; FText CompanyName; TArray<FManifestLine> DeclaredCargo; float ForgeryQuality01; FText RouteCode; FText Notes;` `float ComputeDeclaredMassKg() const; float ComputeDeclaredSignature01() const;`
**UCargoSimulationComponent : UActorComponent** (`Cargo/`): tick 20 Hz. `bool AddCargo(UCargoItemDataAsset*, FVector LocalOffsetCm); void ClearCargo(); float GetTotalCargoMassKg() const; FVector GetCargoCenterOfMassLocalCm() const; float GetCargoIntegrity01() const; float GetUnshieldedSignature01() const; float GetLateralG() const; void ApplyImpact(float ImpactSpeedMs, FVector LocalNormal);` Delegados dinámicos multicast: `FOnCargoIntegrityChanged(float Integrity01, float Delta)`, `FOnCargoShifted(float Severity01)`. Aplica `SetCenterOfMass` al cuerpo del dueño (`ABlacklineVanPawn`).
**ACheckpointController : AActor** (`Checkpoint/`): `UBoxComponent* InspectionZone; UStaticMeshComponent* Barrier; USpotLightComponent* InspectionLight;` `ECheckpointState GetState() const; void PresentManifest(UManifestDataAsset*); float GetSuspicion01() const;` Delegados: `FOnCheckpointStateChanged(ECheckpointState Old, ECheckpointState New)`, `FOnInspectionVerdict(EInspectionVerdict Verdict, float Suspicion01)`. Usa `ABlacklineVanPawn::GetSuspicionSnapshot()` y al fallar llama `UPursuitDirectorSubsystem::StartPursuit`.
**UPursuitDirectorSubsystem : UWorldSubsystem** (`Pursuit/`): `static UPursuitDirectorSubsystem* Get(const UObject*); void StartPursuit(AActor* Target, int32 Level); void StopPursuit(); void ReportTargetVisible(bool); void ReportEngineAndLights(bool bEngineOn, bool bLightsOn); EPursuitState GetState() const; float GetHeat01() const;` Delegado `FOnPursuitStateChanged(EPursuitState New)`. Sin Tick: usa FTimerManager.
**ATerrainModifierVolume : AActor** (`Terrain/`): `UBoxComponent* Zone; ESurfaceKind Surface; float FrictionMultiplier; bool bRequires4WD; float StuckSinkDepthCm`. Al overlap llama `ABlacklineVanPawn::PushSurfaceModifier(this)` / `PopSurfaceModifier(this)`.
**UBlacklineWheelFront / UBlacklineWheelRear : UChaosVehicleWheel** (`Vehicle/`): defaults 4x4 (radio, ancho, SteerAngle, AxleType, suspensión, FrictionForceMultiplier).
**ABlacklineVanPawn : AWheeledVehiclePawn** (`Vehicle/`): `UCargoSimulationComponent* Cargo; UCameraComponent* CockpitCamera; USpringArmComponent* ChaseArm; UCameraComponent* ChaseCamera;` `void ToggleCamera(); void SetDriveMode(EDriveMode); EDriveMode GetDriveMode() const; FSuspicionSnapshot GetSuspicionSnapshot() const; void ActivateDashControl(EDashControl); bool IsEngineOn() const; float GetSpeedKmh() const; void PushSurfaceModifier(ATerrainModifierVolume*); void PopSurfaceModifier(ATerrainModifierVolume*); void OpenWindowForInspection(bool);` Input Enhanced: `UInputMappingContext* DefaultIMC; UInputAction* IA_Throttle, IA_Steer, IA_Brake, IA_Handbrake, IA_Look, IA_ToggleCamera, IA_Interact, IA_DriveMode`. Delegado `FOnDashControlChanged(EDashControl, bool bOn)`.

## Reglas
Sin Tick en actores (excepto el componente de carga a 20 Hz); `UPROPERTY` en todo puntero UObject; `BlueprintCallable/ReadWrite` solo donde diseño itere; sin coordenadas mágicas: todo en `EditAnywhere` con defaults razonables. Código completo, sin pseudocódigo.
Informe final del agente: máx. 80 palabras, solo rutas escritas y dudas abiertas.
