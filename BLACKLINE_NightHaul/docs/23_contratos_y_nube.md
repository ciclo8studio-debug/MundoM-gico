# 23 — Contratos (25 encargos) y guía de integración

## 1. Qué se creó (local)
| Archivo | Contenido |
|---|---|
| `Source/Blackline/Public/Core/BlacklineContractTypes.h` | `FBlacklineContractRow` (FTableRowBase), `EBlacklineWeather`, `EBlacklineTerrainType` |
| `Source/Blackline/Public/Contracts/BlacklineContractSubsystem.h` + `Private/Contracts/…cpp` | `UBlacklineContractSubsystem` (GameInstance) y `UBlacklineContractSettings` (Project Settings) |
| `Cargo/CargoSimulationComponent` (ampliado) | `LoadContract`, `LoadContractFromTable`, `OnCargoDestroyed`, `IsCargoDestroyed` |
| `Checkpoint/CheckpointController` (ampliado) | `ApplyContract`, `RequiredManifestLevel`, penalización por nivel de documento faltante |
| `Vehicle/BlacklineVanPawn` (ampliado) | `GetEngineRpm`, `GetEngineRpm01`, `GetStealth01`, `AreHeadlightsOn`, `IsSignalJammerOn` |
| `Content/Data/DT_BlacklineContracts.csv` | 25 contratos, 5 arcos, 5 brokers |
| `Content/Python/blackline_contracts.py` | Valida y regenera el CSV; `import_datatable()` crea el DataTable en el editor |

**Enums pedidos que no se duplicaron:** `EBlacklineDriveMode` y `EBlacklineCheckpointState` ya existen como `EDriveMode` y `ECheckpointState` (Core/BlacklineTypes.h) y están integrados al furgón y al retén. Se agregó `EBlacklineTerrainType` (terreno del encargo); el tipo de superficie de los volúmenes sigue siendo `ESurfaceKind`.

**Campos extra de la fila:** `Arc`, `LateralBias` (asimetría de carga, p. ej. C09 = 0.7) y `DesignNote` (pista para la tablet).

**Cómo calcula el CoM un contrato:** la carga se convierte en un ítem sintético de masa `CargoWeightKg`, ubicado en `X=ContractCargoXCm`, `Y=LateralBias × ContractLateralSpanCm`. El componente ya aplica `SetCenterOfMass`, G lateral y daño por `Fragility`. La firma EM exige blindaje si pasa de `ContractShieldingSignatureMin`; sin blindaje sube la sospecha en el retén.

**Retén:** nivel del manifiesto presentado = `ceil(ForgeryQuality01 × 3)`. Cada nivel que falta frente a `RequiredManifestLevel` suma `ManifestLevelDeficitPenalty` (0.30) × escrutinio.

## 2. Pasos en el editor (UE 5.8.4)
1. Cierra el editor y compila: `Build.bat BlacklineEditor Win64 Development -Project=…\Blackline.uproject` (hay C++ nuevo; sin esto no existe `FBlacklineContractRow`).
2. Importa el DataTable: en la consola Python del editor, `import blackline_contracts as c; c.import_datatable()` → crea `/Game/Data/DT_BlacklineContracts`. (Manual: arrastra el CSV al Content Browser, elige `BlacklineContractRow` como fila.)
3. `Project Settings > Game > Blackline Contracts`: verifica que `ContractTable` apunte a `DT_BlacklineContracts`.
4. Para probar un contrato: en el Level Blueprint o en un widget de la tablet, `Get Game Instance Subsystem (BlacklineContractSubsystem)` → `Start Contract("C01")`. Al llegar a la entrega, `Finish Contract(true)`.
5. HUD/FX: enlaza `OnCargoIntegrityChanged`, `OnCargoDestroyed`, `OnContractStarted`, `OnContractFinished`.
6. Chaos Vehicles, escena nocturna y MetaSounds: ya documentados en `C_chaos_vehicles.md`, `D1_escena_nocturna.md` y `D2_metasounds.md`. El MetaSound usa `GetEngineRpm01()` y `GetStealth01()`.
7. Render para 8–16 GB: aplicar de a poco `Config/DefaultEngine.snippet.ini` (Lumen High, TSR 75%, `r.Streaming.PoolSize` 2048–3072). Con la iGPU actual, medir con `stat unit` tras cada cambio.

## 3. Entorno nube / tareas externas
Todo lo de abajo se integra por assets/DataTable sin tocar el C++; el juego corre con placeholders si faltan.
| Tarea | Dónde | Cómo se integra |
|---|---|---|
| Furgón blindado propio (malla esquelética + cabina + botones del tablero, huesos `PhysWheel_*`) | Blender/DCC o generador 3D en nube | Reemplaza `SKM_Offroad` en `BP_BlacklineVan`; botones con tags `Dash.*` |
| Props de carga por contrato (contenedores, cajas, tubos) | Generador 3D/texturas en nube | Mesh por `ContractID`; se asigna en el BP según `ActiveContractId` |
| Texturas de asfalto mojado, nieve, barro; decals de ruta | Nube o Substance | Parámetros del `MPC_Weather` (D1) |
| Samples de motor diésel, lluvia, retén, sirenas | Generación/bancos de audio externos | Entradas de los MetaSounds (D2) y spec `20_sfx_spec.md` |
| Bakeo de iluminación/Lumen, builds de Shipping, pruebas automáticas | CI en nube (ver `16_build_ci.md`) | `scripts/Build.ps1` en un runner Windows con UE 5.8.4 |
| Redacción de manifiestos y diálogos de los brokers | Modelo de lenguaje | Se vuelcan al CSV (`CoverStoryManifest`, `DesignNote`) y a las string tables |

**Prompt base para assets 3D en nube:** «Furgón blindado mediano 4x4, estilo neo-noir industrial, 40–60k tris, esqueleto con huesos `PhysWheel_FL/FR/BL/BR`, cabina con tablero de 6 botones físicos nombrados Dash.Headlights/Wipers/Window/SignalJammer/DiffLock/Ignition, texturas PBR 2K».

## 4. Notas de ficción
Los nombres de empresas y entidades de `CoverStoryManifest` son inventados; los títulos de encargos y lugares se mantienen como pidió el brief. Revisa con legal antes de publicar si algún nombre se parece a uno real.
