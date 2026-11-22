# A. Arquitectura — BLACKLINE: NIGHT HAUL (UE 5.8.4, C++)

Contrato de referencia: `CONTRATO.md`. Config de render: `Config/DefaultEngine.snippet.ini`.
APIs dudosas de 5.8: marcar `// VERIFY-5.8`.

## 1. `Blackline.uproject`

```json
{
  "FileVersion": 3,
  "EngineAssociation": "5.8",
  "Category": "Games",
  "Description": "Blackline: Night Haul",
  "Modules": [
    {
      "Name": "Blackline",
      "Type": "Runtime",
      "LoadingPhase": "Default",
      "AdditionalDependencies": [ "Engine", "CoreUObject" ]
    }
  ],
  "Plugins": [
    { "Name": "ChaosVehiclesPlugin", "Enabled": true },
    { "Name": "EnhancedInput",       "Enabled": true },
    { "Name": "Niagara",             "Enabled": true },
    { "Name": "GameplayTagsEditor",  "Enabled": true },
    { "Name": "ModelContextProtocol","Enabled": false },
    { "Name": "RayTracing",          "Enabled": false },
    { "Name": "WorldPartition",      "Enabled": true },
    { "Name": "PCG",                 "Enabled": true },
    { "Name": "MegaLights",          "Enabled": false }
  ],
  "TargetPlatforms": [ "Windows" ]
}
```
Notas:
- `ChaosVehiclesPlugin` es el nombre del plugin; el modulo C++ se llama `ChaosVehicles` (verificar nombres con `.uplugin` en 5.8; VERIFY-5.8).
- `EnhancedInput` y Common UI estan unificados en 5.8: no agregar `CommonUI` (se usa UMG simple para HUD).
- `MegaLights` es una funcion del renderer (cvar `r.MegaLights.EnableForProject`), no un plugin: la entrada se deja `false` solo por si 5.8.4 lo expusiera como plugin (borrar si el editor avisa).
- `RayTracing` desactivado: Lumen por software.
- `PCG` habilitado para poblar carretera/arboles/escombros en editor (no corre en runtime del juego).
- C++20 minimo en 5.8: `Blackline.Target.cs` y `BlacklineEditor.Target.cs` usan `CppStandard = CppStandardVersion.Cpp20`.

## 2. Arbol exacto `Source/`

```
Source/
  Blackline.Target.cs
  BlacklineEditor.Target.cs
  Blackline/
    Blackline.Build.cs
    Blackline.cpp                    (IMPLEMENT_PRIMARY_GAME_MODULE, LogBlackline)
    Public/
      Blackline.h                    (DECLARE_LOG_CATEGORY_EXTERN(LogBlackline,...))
      Core/
        BlacklineTypes.h             (enums y structs del contrato)
        BlacklineGameMode.h
        BlacklineGameInstance.h
        BlacklineCollision.h         (#define BL_TRACE_INTERACT ECC_GameTraceChannel1)
      Data/
        CargoItemDataAsset.h
        ManifestDataAsset.h
      Cargo/
        CargoSimulationComponent.h
      Checkpoint/
        CheckpointController.h
      Pursuit/
        PursuitDirectorSubsystem.h
        PursuitPawn.h                (patrulla AI)
        PursuitAIController.h
      Terrain/
        TerrainModifierVolume.h
      Vehicle/
        BlacklineVanPawn.h
        BlacklineWheelFront.h
        BlacklineWheelRear.h
        DashButtonComponent.h        (boton tablero, tag Dash.*)
      UI/
        BlacklineHUD.h               (UMG: velocimetro, manifiesto, sospecha)
        ManifestWidget.h
    Private/
      Core/
        BlacklineGameMode.cpp
        BlacklineGameInstance.cpp
      Data/
        CargoItemDataAsset.cpp
        ManifestDataAsset.cpp
      Cargo/
        CargoSimulationComponent.cpp
      Checkpoint/
        CheckpointController.cpp
      Pursuit/
        PursuitDirectorSubsystem.cpp
        PursuitPawn.cpp
        PursuitAIController.cpp
      Terrain/
        TerrainModifierVolume.cpp
      Vehicle/
        BlacklineVanPawn.cpp
        BlacklineWheelFront.cpp
        BlacklineWheelRear.cpp
        DashButtonComponent.cpp
      UI/
        BlacklineHUD.cpp
        ManifestWidget.cpp
```

## 3. `Blackline.Build.cs`

```csharp
using UnrealBuildTool;

public class Blackline : ModuleRules
{
    public Blackline(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp20;   // minimo en 5.8
        IWYUSupport = IWYUSupport.Full;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "ChaosVehicles", "PhysicsCore", "Niagara", "AIModule",
            "GameplayTags", "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate", "SlateCore"
        });

        PublicIncludePaths.AddRange(new string[] { ModuleDirectory + "/Public" });
        PrivateIncludePaths.AddRange(new string[] { ModuleDirectory + "/Private" });
    }
}
```
`Blackline.Target.cs`: `Type = TargetType.Game; DefaultBuildSettings = BuildSettingsVersion.Latest; IncludeOrderVersion = EngineIncludeOrderVersion.Latest; ExtraModuleNames.Add("Blackline");` y lo mismo con `TargetType.Editor`.

## 4. Arbol exacto `Content/`

```
Content/Blackline/
  Maps/
    L_NightRoute            (World Partition, ruta principal)
    L_Garage                (menu / carga de manifiesto)
    L_Checkpoint_Test       (prueba de retenes)
  Vehicle/
    BP_BlacklineVan         (hijo de ABlacklineVanPawn)
    BP_WheelFront           (hijo de UBlacklineWheelFront)
    BP_WheelRear            (hijo de UBlacklineWheelRear)
    SK_Van, PHYS_Van, ABP_Van
    Materials/  Textures/
  Cargo/
    DA_Cargo_<Item>         (UCargoItemDataAsset; uno por item)
    SM_Crate_*              (mallas de caja)
  Manifests/
    DA_Manifest_<Id>        (UManifestDataAsset)
  Checkpoint/
    BP_CheckpointController (hijo de ACheckpointController)
    SM_Barrier, SM_Booth
  Pursuit/
    BP_PursuitPawn, BP_PursuitAI
    BT_Pursuit, BB_Pursuit, EQS_FindSearchPoint
  Terrain/
    BP_TerrainMud, BP_TerrainGravel, BP_TerrainWet
    PM_Asphalt, PM_WetAsphalt, PM_Gravel, PM_Mud, PM_Forest   (Physical Materials)
  Input/
    IMC_Default
    IA_Throttle, IA_Steer, IA_Brake, IA_Handbrake, IA_Look,
    IA_ToggleCamera, IA_Interact, IA_DriveMode
  UI/
    WBP_HUD, WBP_Manifest, WBP_Dashboard
  FX/
    NS_Rain, NS_Dust, NS_Sparks, NS_Exhaust
  Audio/
    MS_Engine, SC_Rain, SC_Checkpoint
  Environment/
    PCG/  Foliage/  Meshes/  Materials/
  Lighting/
    BP_NightSky, PP_NightGrade
Content/Collections/
```
Regla: cada DataAsset en su carpeta; `Config/DefaultGame.ini` registra `+PrimaryAssetTypesToScan` para `CargoItem` (`/Game/Blackline/Cargo`) y `Manifest` (`/Game/Blackline/Manifests`).

## 5. Arbol `Config/`

```
Config/
  DefaultEngine.ini             (base + contenido de DefaultEngine.snippet.ini)
  DefaultEngine.snippet.ini     (este bloque)
  DefaultGame.ini               (PrimaryAssetTypesToScan, ProjectVersion)
  DefaultInput.ini              (EnhancedInput como clase por defecto)
  DefaultGameplayTags.ini       (Dash.Headlights, Dash.Wipers, Dash.Window,
                                 Dash.SignalJammer, Dash.DiffLock, Dash.Ignition)
```

## 6. Diagrama C++ vs Blueprint

```
 C++ (modulo Blackline)                         BLUEPRINT (Content/Blackline)
 ------------------------------------           ------------------------------------
 ABlacklineVanPawn  (fisica, input, ---hereda-> BP_BlacklineVan (mallas, sonidos,
   camaras, tablero, modos 4x4)                   valores de tuning, IMC/IA asignados)
 UBlacklineWheelFront/Rear ---------hereda-----> BP_WheelFront/Rear (radio, suspension)
 UCargoSimulationComponent (20 Hz) ----usa-----> DA_Cargo_* (datos)
 ACheckpointController (maquina de      <-hereda-- BP_CheckpointController (malla, luz,
   estados, sospecha)                              textos)
 UPursuitDirectorSubsystem (timers) --------> BP_PursuitPawn + BT/BB (comportamiento AI)
 ATerrainModifierVolume ------------hereda----> BP_TerrainMud/Gravel/Wet (valores)
 UManifestDataAsset / CargoItemDataAsset -----> DA_Manifest_* / DA_Cargo_* (contenido)
 Delegados multicast (Dyn.) -------eventos-----> Widgets UMG (WBP_HUD) y FX (Niagara)

 Flujo de datos:
 Input (IA_*) -> VanPawn -> Cargo.AddCargo / GetSuspicionSnapshot
        VanPawn <- TerrainModifierVolume (Push/PopSurfaceModifier)
 Checkpoint.PresentManifest(DA_Manifest) -> lee VanPawn::GetSuspicionSnapshot
        -> OnInspectionVerdict -> HUD (BP)
        -> si PursuitAlert: PursuitDirector.StartPursuit -> PursuitPawn (BT)
 PursuitDirector.OnPursuitStateChanged -> musica/FX/HUD (BP)
```

## 7. Que va en C++ y por que (rendimiento)

| Sistema | En C++ | Motivo de rendimiento |
|---|---|---|
| Vehiculo (Pawn + ruedas) | Si | Chaos Vehicles corre a 60 Hz con sub-stepping; las sobrescrituras de suspension, tracccion 4x4 y friccion por superficie en cada paso son mas baratas nativas (BP en bucle fisico cuesta ~10x). |
| Simulacion de carga | Si | Calculo de centro de masa y G lateral con N cajas a 20 Hz; evita arrays de BP lentos y la sobrecarga de la VM. Un solo componente, sin Tick por caja. |
| Sospecha / retenes | Si | Logica de estados determinista y barata; evita Event Tick (regla del contrato). |
| PursuitDirector | Si (WorldSubsystem + FTimerManager) | Sin Tick; consultas de AI por temporizador. |
| TerrainModifier | Si (overlap) | Push/Pop en pila O(1); aplicado a ruedas sin lanzar trazados por cuadro. |
| Input Enhanced | Si (binding) | Bindings nativos; sin nodos de input en BP. |
| UI/HUD | Parcial: logica en C++, layout en UMG BP | Se actualiza por delegados (evento), no por bindings por cuadro. |
| Tuning de curvas, mallas, sonidos, FX, mapas | Blueprint / editor | Datos; iterables sin recompilar. |
| Comportamiento AI de patrulla | BT/BB en BP + C++ solo en `AIController` | Arbol visual para iterar; acciones pesadas (percepcion, EQS) por servicios a baja frecuencia. |

Reglas de rendimiento transversales:
- Presupuesto 16.6 ms por cuadro: Game <= 5 ms, Render <= 4 ms, GPU <= 14 ms (a 810p internos con TSR 75%).
- Sin Event Tick: temporizadores y delegados. Unica excepcion: `UCargoSimulationComponent` a 20 Hz (`PrimaryComponentTick.TickInterval = 0.05f`).
- `UPROPERTY` en todo puntero UObject; `TObjectPtr` en cabeceras; sin `FindObject`/`GetAllActorsOfClass` por cuadro.
- Nanite solo en estaticos pesados; vehiculo y ruedas son esqueleticos (sin Nanite).
- Presupuestos de memoria (8 GB RAM): texturas 2560 MB (pool), VSM 128 MB (2048 paginas), Nanite streaming 256 MB.
- Perfilar con `stat unit`, `stat gpu`, `stat Niagara`, y Unreal Insights antes de cada hito.
