# 10 — Estrategia de QA del slice (BLACKLINE: NIGHT HAUL)

Fuente: `CONTRATO.md`. UE 5.8.4, C++, módulo `Blackline`. APIs de test marcadas `// VERIFY-5.8` (confirmar en `docs/engine-reference/unreal/`).
Pruebas solo con builds empaquetadas o Automation por línea de comandos (PC 8 GB: nunca editor + juego a la vez; sin PIE).

## 1. Estrategia

Pirámide: 70 % lógica pura con Automation Spec (sin mundo), 20 % integración con mundo mínimo (`FAutomationEditorCommonUtils`/mapa de test vacío), 10 % playtest manual con evidencia.

| Tipo de historia | Evidencia | Gate |
|---|---|---|
| Logic (sospecha, carga, persecución, fricción) | Automation Spec en `Source/Blackline/Private/Tests/<Sistema>/` | BLOQUEANTE |
| Integration (furgón+carga, checkpoint+persecución, terreno+furgón) | Spec con mundo de test o playtest documentado | BLOQUEANTE |
| Visual/Feel (manejo, cámara, tablero) | Captura/video + visto bueno en `production/qa/evidence/` | BLOQUEANTE |
| UI (HUD del tablero) | Captura por pantalla | BLOQUEANTE |
| Config/Data (DataAssets) | Smoke check | Aviso |

Orden de ejecución: smoke (build abre, mapa carga, furgón conduce 30 s) → Automation → playtest guiado → regresión.
Un smoke fallido bloquea el traspaso a QA manual.

## 2. Criterios de aceptación por pilar mecánico

Valores en `EditAnywhere`/DataAsset; los umbrales son propuesta inicial, el diseño los confirma (marcados `[CONFIRMAR]`).

### P1 Conducción 4x4 (`ABlacklineVanPawn`, ruedas, `ATerrainModifierVolume`)
- AC1.1 `SetDriveMode(FourWheel)` cambia `GetDriveMode()` y el reparto de torque; `RWD` lo revierte. Idempotente.
- AC1.2 Entrar a un `ATerrainModifierVolume` llama `PushSurfaceModifier`; salir llama `PopSurfaceModifier`; solapes anidados se apilan y desapilan en orden sin fugas (stack vacío al final).
- AC1.3 Superficie con `bRequires4WD` y modo RWD: el furgón se hunde hasta `StuckSinkDepthCm` o pierde tracción medible (velocidad < X km/h con acelerador 100 % durante 3 s `[CONFIRMAR]`); en FourWheel avanza.
- AC1.4 `FrictionMultiplier` se aplica mientras dura el overlap y se restaura al salir (valor base exacto).
- AC1.5 `GetSpeedKmh()` >= 0 y coherente con la velocidad de física (±2 %).
- AC1.6 Cada superficie `ESurfaceKind` produce comportamiento distinto y ordenado: Asphalt > WetAsphalt > Gravel > Mud en agarre.

### P2 Carga y física (`UCargoSimulationComponent`)
- AC2.1 `GetTotalCargoMassKg()` = suma exacta de `MassKg` de ítems; `ClearCargo()` deja 0 y `GetCargoIntegrity01()`=1.
- AC2.2 `GetCargoCenterOfMassLocalCm()` es el promedio ponderado por masa; vacío devuelve `FVector::ZeroVector` sin dividir por cero.
- AC2.3 `ApplyImpact` con velocidad 0 no cambia integridad; integridad siempre en [0,1]; `FOnCargoIntegrityChanged` se emite con `Delta` < 0 y solo si cambia.
- AC2.4 Ítems `Fragile` pierden más integridad que `Standard` ante el mismo impacto (monotonía con `Fragility01`).
- AC2.5 `FOnCargoShifted` se emite cuando `GetLateralG()` supera el umbral `[CONFIRMAR]`; no se emite en reposo.
- AC2.6 `GetUnshieldedSignature01()` solo cuenta ítems con `bRequiresShielding` sin blindaje; resultado en [0,1].
- AC2.7 Tick a 20 Hz (±10 %) y componente sin tick cuando no hay carga `[CONFIRMAR]`.
- AC2.8 `SetCenterOfMass` del dueño se actualiza tras `AddCargo`/`ClearCargo`.
- AC2.9 `AddCargo` rechaza `nullptr` (devuelve false, sin crash) y respeta capacidad/offset.

### P3 Puesto de control (`ACheckpointController`, `UManifestDataAsset`)
- AC3.1 Máquina de estados: Idle → Approaching → Inspecting → Cleared | PursuitTriggered. Transiciones inválidas se rechazan; cada cambio emite `FOnCheckpointStateChanged(Old,New)` una vez.
- AC3.2 `ComputeDeclaredMassKg/Signature01` coinciden con `DeclaredCargo` (cantidad × valor); manifiesto vacío = 0.
- AC3.3 Veredicto determinista dado un `FSuspicionSnapshot` + manifiesto: masa real vs declarada, firma sin blindar, faros encendidos, jammer, daño, velocidad antes de parar. `GetSuspicion01()` en [0,1].
- AC3.4 Monotonía: más desvío de masa, más daño o más firma nunca baja la sospecha; `ForgeryQuality01` alta la baja.
- AC3.5 Umbrales: sospecha < A → `CleanPass`; A..B → `SecondaryInspection`; > B → `PursuitAlert` `[CONFIRMAR A,B]`. En `PursuitAlert` se llama `StartPursuit` exactamente una vez.
- AC3.6 `PresentManifest(nullptr)` no crashea y no genera `CleanPass`.
- AC3.7 Sin Tick; el tiempo de inspección usa timers.

### P4 Persecución (`UPursuitDirectorSubsystem`)
- AC4.1 `StartPursuit` → `Hunting`; `StopPursuit` → `None`; cada cambio emite `FOnPursuitStateChanged`.
- AC4.2 Hunting → Searching al `ReportTargetVisible(false)` sostenido T1; Searching → Lost tras T2 con motor y luces apagados `[CONFIRMAR T1,T2]`; ver de nuevo vuelve a Hunting.
- AC4.3 `ReportEngineAndLights(true,true)` en Searching acelera la detección; `(false,false)` la frena.
- AC4.4 `GetHeat01()` en [0,1], sube cuando es visible, decae cuando no.
- AC4.5 `StartPursuit(nullptr)` es rechazado con log `LogBlackline`; doble `StartPursuit` no duplica timers.
- AC4.6 Sin Tick: solo `FTimerManager`; los timers se limpian en `Deinitialize` (sin callbacks tras destruir el mundo).
- AC4.7 `Get(WorldContext)` devuelve el subsistema del mundo correcto; nullptr con contexto nulo.

### P5 Cabina y tablero (`EDashControl`, cámara, input)
- AC5.1 Cada `EDashControl` tiene botón con tag `Dash.<Nombre>` detectable por traza `ECC_GameTraceChannel1`; `ActivateDashControl` alterna estado y emite `FOnDashControlChanged(control,bOn)`.
- AC5.2 `Ignition` apaga/enciende el motor: `IsEngineOn()` coherente y el acelerador se ignora con motor apagado.
- AC5.3 `SignalJammer` y `Headlights` se reflejan en `FSuspicionSnapshot` (`bSignalJammerOn`, `bHeadlightsOn`).
- AC5.4 `ToggleCamera` alterna Cockpit/ThirdPerson; solo una cámara activa; sin cambio de estado de conducción.
- AC5.5 `OpenWindowForInspection(true/false)` abre/cierra la ventanilla y es idempotente.
- AC5.6 Todas las `IA_*` y `DefaultIMC` asignadas; si falta alguna: log de error, sin crash.
- AC5.7 Sensación (Visual/Feel): giro/aceleración sin sobresteer incontrolable a 60 km/h en asfalto; verificado por playtest con checklist (3 testers, ≥ 2 aprueban).

## 3. Gates de calidad

| Gate | Criterio | Responsable |
|---|---|---|
| G0 Commit | Compila sin errores ni warnings nuevos; Automation `Blackline.*` verde | Programador |
| G1 Smoke (pre-QA) | Build abre, mapa carga, conducir 30 s, 1 checkpoint completo, sin crash | QA Lead |
| G2 Historia Done | Evidencia por tipo (sección 1) presente; 0 S1/S2 abiertos en su sistema | QA Lead |
| G3 Hito / slice | 0 S1, 0 S2; S3 ≤ 5 con workaround documentado; todos los AC de la sección 2 verificados | QA Lead + TD |
| G4 Rendimiento | ≥ 30 FPS 1080p Baja en PC de referencia (iGPU 8 GB), 1 % low ≥ 20; sin hitch > 100 ms en checkpoint; sin fuga de memoria en 30 min `[CONFIRMAR]` | TD |
| G5 Estabilidad | Soak 60 min de conducción + 5 persecuciones sin crash; 0 ensure/check en log | QA |
| G6 Regresión | Suite completa verde antes de cada build candidata | QA |

Con presión de calendario no se omiten gates: se escala a producción.

## 4. Severidades

- **S1 Crítica**: crash, pérdida de progreso, furgón inconducible, persecución/checkpoint que bloquea el avance. Corregir antes de cualquier build.
- **S2 Alta**: pilar roto pero jugable (p. ej. veredicto incorrecto, 4x4 no engancha, carga sin daño). Corregir antes del hito.
- **S3 Media**: degradado con workaround (tablero con un botón difícil de alcanzar, FX faltante).
- **S4 Baja**: cosmético, texto, ruido de log.

Prioridad: P1 este sprint, P2 pronto, P3 backlog, P4 no se arregla. Un bug requiere: build, mapa, pasos, esperado/real, frecuencia, log `LogBlackline`.

## 5. Automation Specs sugeridos por clase

Convención: `BEGIN_DEFINE_SPEC(F<Clase>Spec, "Blackline.<Area>.<Clase>", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)`; ubicar en `Source/Blackline/Private/Tests/`. Flags exactos `// VERIFY-5.8`. Tests de lógica pura construyen objetos con `NewObject` y DataAssets en memoria (`NewObject<UCargoItemDataAsset>`), sin mundo.

### UCargoItemDataAsset (`Blackline.Data.CargoItem`) — Config/Data
- `GetPrimaryAssetId` devuelve tipo `CargoItem` e `ItemId`.
- Validación de defaults: `MassKg > 0`, `Fragility01` y `ContrabandSignature01` en [0,1], `BoxExtentCm` > 0.

### UManifestDataAsset (`Blackline.Data.Manifest`)
- `ComputeDeclaredMassKg`: 0 líneas = 0; 2 líneas con cantidades = suma exacta (tolerancia 0,01).
- `ComputeDeclaredSignature01`: acotada a [0,1]; línea con `Item` nulo/soft ptr sin resolver se ignora sin crash.
- `Quantity` ≤ 0 no aporta masa negativa.

### UCargoSimulationComponent (`Blackline.Cargo.Simulation`)
- Masa total tras `AddCargo` ×3 y `ClearCargo`.
- Centro de masa ponderado (dos ítems 10 kg y 30 kg a ±100 cm → 50 cm hacia el pesado).
- `ApplyImpact(0)` sin cambio; impacto alto baja integridad; piso en 0; delegado con `Delta<0` capturado con un handler de test.
- Fragile pierde más que Standard con igual impacto.
- Firma sin blindar con ítem `bRequiresShielding`.
- `AddCargo(nullptr)` devuelve false.
- Integración (mapa de test): `ABlacklineVanPawn` con carga actualiza `SetCenterOfMass` (comparar `GetCenterOfMass` antes/después `// VERIFY-5.8`).

### ACheckpointController (`Blackline.Checkpoint.Controller`)
- Tabla de verdad de transiciones (válidas e inválidas) con conteo de eventos de `FOnCheckpointStateChanged`.
- Tabla de veredictos con `FSuspicionSnapshot` sintéticos: limpio → `CleanPass`; faros apagados + daño alto + firma alta → `PursuitAlert`.
- Monotonía: barrido de `DamageRatio01` 0..1 en 11 pasos, la sospecha no decrece.
- `PursuitAlert` llama `StartPursuit` una sola vez (observar `FOnPursuitStateChanged`).
- `PresentManifest(nullptr)` seguro.
- Latent: `ADD_LATENT_AUTOMATION_COMMAND` para esperar el timer de inspección.

### UPursuitDirectorSubsystem (`Blackline.Pursuit.Director`)
- Mundo de test (`FTestWorldWrapper` o `UWorld::CreateWorld` `// VERIFY-5.8`): `Get` devuelve instancia; estados None→Hunting→Searching→Lost→None.
- Doble `StartPursuit` no duplica timers; `StopPursuit` los limpia.
- `Heat01` acotado; sube/decae.
- Teardown del mundo sin callbacks colgantes.
- Los tiempos T1/T2 se inyectan por UPROPERTY para no esperar tiempo real (avanzar con `FTimerManager::Tick` manual).

### ATerrainModifierVolume (`Blackline.Terrain.Volume`)
- Mundo de test: spawn volumen + furgón, `SetActorLocation` dentro/fuera, verificar Push/Pop (contador interno expuesto para test o `GetActiveModifierCount()` añadido).
- Overlaps anidados de 2 volúmenes: orden de pila correcto.
- Destruir el volumen con el furgón dentro hace Pop (sin puntero colgante).
- `bRequires4WD` + RWD vs FourWheel: flag de "atascado" esperado.

### UBlacklineWheelFront / UBlacklineWheelRear (`Blackline.Vehicle.Wheels`)
- CDO: `SteerAngle` > 0 en delantera, 0 en trasera; `AxleType` correctos; radio/ancho/suspensión > 0; `FrictionForceMultiplier` > 0.

### ABlacklineVanPawn (`Blackline.Vehicle.Van`)
- `SetDriveMode`/`GetDriveMode` ida y vuelta e idempotencia.
- `ActivateDashControl` por cada `EDashControl` (6): estado alterna y `FOnDashControlChanged` se emite una vez.
- `Ignition` apaga motor: `IsEngineOn()` false.
- `ToggleCamera` dos veces vuelve al inicio; solo una cámara activa.
- `GetSuspicionSnapshot` refleja faros/jammer/integridad de carga.
- `Push/PopSurfaceModifier` con `nullptr` seguros.
- Verificación de referencias de input: todas las `IA_*` y `DefaultIMC` no nulas en el Blueprint hijo (test de datos).
- `OpenWindowForInspection` idempotente.

### Tests transversales
- `Blackline.Rules.NoTick`: recorre CDOs de los actores del módulo y comprueba `PrimaryActorTick.bCanEverTick == false` (excepto `UCargoSimulationComponent` a 20 Hz: `TickInterval ≈ 0.05`).
- `Blackline.Rules.UPropertyPointers`: revisión estática en CI (grep de `UObject*` crudos sin `UPROPERTY`).
- `Blackline.Data.Validate`: carga todos los `PrimaryAsset` de tipo `CargoItem`/`Manifest` y valida rangos.

## 6. Ejecución

```
UnrealEditor-Cmd.exe <Proyecto>.uproject -ExecCmds="Automation RunTests Blackline; Quit" -unattended -nullrhi -log -dx11
```
`// VERIFY-5.8`: confirmar flags y ruta. Cierre de editor antes de abrir el juego; un proceso a la vez.

## 7. Playtests guiados (Visual/Feel e Integración)

Protocolo corto, 3 testers, build empaquetada, FPS `-dev` visible:
1. Conducir ruta con asfalto → mojado → grava → barro con y sin 4x4.
2. Cargar ítems distintos; frenar y girar fuerte; observar daño/desplazamiento.
3. Puesto de control: pasar limpio, pasar con faros apagados y daño, pasar con carga sin blindar.
4. Persecución: escapar apagando motor y luces.
Cuestionario: claridad del tablero (1-5), control del furgón (1-5), tensión del checkpoint (1-5), bugs vistos. Umbral de aceptación: media ≥ 3,5 `[CONFIRMAR]`.

## 8. Riesgos de testabilidad (a corregir antes de implementar)

- Sospecha y veredicto deben ser funciones puras sobre `FSuspicionSnapshot` (estáticas o en el controlador sin mundo) para probar sin spawn.
- Exponer contadores de Push/Pop o `GetActiveModifierCount()` (se puede añadir al contrato).
- Tiempos de persecución e inspección como `EditAnywhere` inyectables.
- Umbrales `[CONFIRMAR]` deben vivir en DataAsset/UPROPERTY, no en código.
- Física de Chaos no es determinista entre máquinas: criterios de P1 con tolerancias y no valores exactos.
