# 12 - Blueprints a crear (Blackline: Night Haul)

Estado: **NO aplicado**. Los .uasset se crean a mano en el editor (UE 5.8.4). Las APIs de 5.8 no estan verificadas contra `docs/engine-reference/unreal/` en este documento; revisar antes de depender de ellas.
Fuente: `CONTRATO.md`. Regla: C++ define el marco, Blueprint solo contenido y afinacion. Sin Tick en BPs.

## 1. Lista de assets

Carpetas sugeridas: `Content/Blackline/{Vehicle,Checkpoint,Pursuit,Terrain,Data,Input}`.

| Asset | Padre (C++) | Notas |
|---|---|---|
| `BP_BlacklineVan` | `ABlacklineVanPawn` | Malla, ruedas, camaras, input |
| `BP_WheelFront` / `BP_WheelRear` | `UBlacklineWheelFront/Rear` | Clases de rueda para el Chaos Vehicle |
| `BP_Checkpoint` | `ACheckpointController` | Barrera, luz, zona |
| `BP_PatrolCar` | **Sin clase C++ en el contrato.** Propuesta: `APawn`/`ACharacter` con AIController (AIModule) | Decidir antes de crear (ver 6) |
| `BP_Guard` | **Sin clase C++ en el contrato.** Propuesta: `ACharacter` | Idem |
| `BP_Mud` | `ATerrainModifierVolume` | Surface=Mud |
| `BP_Gravel` | `ATerrainModifierVolume` | Surface=Gravel |
| `BP_WetAsphalt` (opcional) | `ATerrainModifierVolume` | Surface=WetAsphalt |
| `DA_Cargo_*` | `UCargoItemDataAsset` | Ver 2 |
| `DA_Manifest_*` | `UManifestDataAsset` | Ver 2 |
| `IMC_Van` | InputMappingContext | Ver 3 |
| `IA_Throttle, IA_Steer, IA_Brake, IA_Handbrake, IA_Look, IA_ToggleCamera, IA_Interact, IA_DriveMode` | InputAction | Ver 3 |
| `BP_GameMode` / `BP_PlayerController` | GameMode / PC | Default Pawn = `BP_BlacklineVan` |

## 2. DataAssets de ejemplo (Content Browser > Miscellaneous > Data Asset)

`DA_Cargo_Crate_Standard` (CargoClass=Standard, MassKg=120, Fragility01=0.2, ContrabandSignature01=0, bRequiresShielding=false, BoxExtentCm=(50,50,50), ValueCredits=200).
`DA_Cargo_Relic_Idol` (Relic, 40, 0.8, 0.6, true, (30,30,40), 2500).
`DA_Cargo_AIServer` (AIServer, 200, 0.7, 0.9, true, (60,50,90), 5000).
`DA_Cargo_Volatile_Drum` (Volatile, 180, 0.5, 0.4, false, (35,35,50), 900).
Asignar `Mesh` (soft). ItemId unico por asset (clave del PrimaryAssetId).
`DA_Manifest_Clean`: DeclaredCargo = lineas que coinciden con lo cargado, ForgeryQuality01=1.
`DA_Manifest_Forged`: declara Standard pero se carga Relic; ForgeryQuality01=0.6.
Si se usan Primary Assets: Project Settings > Asset Manager > Primary Asset Types: `CargoItem` y `Manifest` (verificar el tipo de Manifest en C++).

## 3. Input (Enhanced Input)

Crear 8 InputActions: Throttle, Steer, Brake (Axis1D, float); Handbrake, ToggleCamera, Interact, DriveMode (Digital bool); Look (Axis2D).
`IMC_Van`: Throttle=W/Gamepad RT, Brake=S/LT, Steer=A(-1)/D(+1) con Negate, stick X; Handbrake=Space; Look=Mouse XY/stick R; ToggleCamera=V; Interact=clic izq o E; DriveMode=T.
En `BP_BlacklineVan` > Class Defaults > Input: asignar `DefaultIMC` y cada `IA_*`.

## 4. Propiedades que se afinan en BP (Class Defaults)

- **BP_BlacklineVan**: Skeletal Mesh + AnimBP de ruedas, Wheel Setups (4 ruedas -> BP_WheelFront/Rear, nombres de hueso), masa, centro de masa, curvas de torque/direccion, DriveMode inicial, offsets de `CockpitCamera` y `ChaseArm` (largo, lag), FOV, IA_*/DefaultIMC. Los componentes de boton del tablero llevan Tags `Dash.*` y colision en canal Interact (`ECC_GameTraceChannel1`).
- **BP_WheelFront/Rear**: radio, ancho, SteerAngle, AxleType, suspension, FrictionForceMultiplier.
- **BP_Checkpoint**: tamano de `InspectionZone`, malla de `Barrier`, color/intensidad/cono de `InspectionLight`, umbrales de sospecha (si se exponen como EditAnywhere), manifiesto por defecto.
- **BP_Mud / BP_Gravel**: Zone (extent), Surface, FrictionMultiplier (Mud ~0.4, Gravel ~0.7), bRequires4WD (Mud=true), StuckSinkDepthCm.
- **BP_PatrolCar / BP_Guard**: velocidades, rutas (referencias a actores en nivel, no coordenadas fijas), conos de vision.

## 5. Eventos a enganchar

Todo en `BeginPlay`, desenlazar en `EndPlay`. Sin Tick.
- `BP_BlacklineVan.Cargo` -> `OnCargoIntegrityChanged(Integrity01, Delta)`: HUD de integridad / sonido de golpe. `OnCargoShifted(Severity01)`: camera shake, sonido de carga.
- `BP_BlacklineVan` -> `OnDashControlChanged(Control, bOn)`: animar palancas, encender luces/limpiaparabrisas/ventana.
- `BP_Checkpoint` -> `OnCheckpointStateChanged(Old, New)`: mover Barrier, encender InspectionLight. `OnInspectionVerdict(Verdict, Suspicion01)`: UI, dialogo del guardia (CleanPass abre, SecondaryInspection retiene, PursuitAlert dispara persecucion).
- `UPursuitDirectorSubsystem.OnPursuitStateChanged(New)`: musica/sirenas, activar `BP_PatrolCar` (Hunting), bajar alerta (Lost). Obtener con `Get(WorldContextObject)`.
- `BP_BlacklineVan` en `OnPossess/BeginPlay`: llamar `ReportEngineAndLights` desde el director cuando cambie `OnDashControlChanged` (Ignition/Headlights).
- Presentar manifiesto: al entrar la van a `InspectionZone` (OnComponentBeginOverlap) llamar `PresentManifest(DA_Manifest_*)`; y `OpenWindowForInspection(true)` en la van.

## 6. Paso a paso en el editor

1. Compilar el modulo `Blackline` (Visual Studio / Live Coding desactivado en PC de 8 GB) y abrir el editor. Confirmar plugins activos: ChaosVehicles, EnhancedInput, Niagara, GameplayTags.
2. Project Settings > Collision > Trace Channels: nuevo canal `Interact` (queda como GameTraceChannel1; verificar que sea el primero) con respuesta Block en los botones del tablero.
3. Crear `IA_*` y `IMC_Van` (seccion 3).
4. Crear `BP_WheelFront` y `BP_WheelRear` (Blueprint Class, padre las ruedas C++).
5. Crear `BP_BlacklineVan` (padre `BlacklineVanPawn`): asignar malla y AnimBP, 4 Wheel Setups, input, camaras. Agregar componentes de botones del tablero con los Tags `Dash.*`.
6. Crear `BP_GameMode`, Default Pawn = `BP_BlacklineVan`; ponerlo en World Settings.
7. Crear `BP_Mud`, `BP_Gravel`: ajustar valores (seccion 4). Colocar en el nivel y escalar `Zone`.
8. Crear `DA_Cargo_*` y `DA_Manifest_*` (seccion 2). Cargar la carga de prueba desde `BeginPlay` de la van con `Cargo->AddCargo(DA, offset)` leyendo offsets de una variable EditAnywhere, no escritos a mano.
9. Crear `BP_Checkpoint`: ajustar zona, barrera, luz; enlazar eventos (seccion 5). Poner en el nivel junto a la ruta.
10. Crear `BP_PatrolCar` y `BP_Guard` tras decidir la clase base (ver nota). Agregar NavMesh y AIController; enganchar `OnPursuitStateChanged`.
11. Compilar cada BP, guardar todo (Ctrl+Shift+S) y probar con build empaquetada (en este PC el PIE no se usa).

Nota: el contrato no define C++ para PatrolCar ni Guard. Decision pendiente del CTO: BP puro sobre `ACharacter`/`APawn` o agregar clases C++ al contrato.

## 7. Errores comunes

- Padre equivocado: crear los BP sobre `Pawn` en vez de `BlacklineVanPawn`; la van no tiene Cargo ni camaras.
- Ruedas sin asignar o sin hueso correcto: la van no se mueve o se hunde. Revisar Wheel Setups y que usen BP_Wheel*.
- Canal Interact mal numerado: los botones no responden. Confirmar que `Interact` es `ECC_GameTraceChannel1`.
- `DefaultIMC` o `IA_*` vacios: no hay input. Asignar en Class Defaults.
- Tags `Dash.*` mal escritos (mayusculas/espacios): el control no se reconoce.
- Zonas de terreno sin `Generate Overlap Events` o con perfil de colision que ignora Pawn: no se aplica el modificador.
- Desenlazar eventos olvidado en `EndPlay`: fugas y llamadas dobles.
- Referencias duras a assets grandes en `DA_Manifest` (usa `TSoftObjectPtr`): cargar con Async Load, no con Cast.
- Casting a `BP_BlacklineVan` desde otros BP: usar la clase C++ `ABlacklineVanPawn` o una interfaz `BPI_*`.
- Poner logica en Tick de BP: prohibido por contrato; usar eventos y timers.
- Valores/coordenadas fijas en grafos: exponer como variable EditAnywhere.
- Mover el centro de masa a mano: lo controla `UCargoSimulationComponent`; no sobrescribirlo en BP.
