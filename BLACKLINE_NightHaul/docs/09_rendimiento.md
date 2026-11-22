# 09 - Rendimiento (BLACKLINE: Night Haul, UE 5.8.4)

Estado: presupuestos propuestos, sin medir. Los cambia solo el technical-director. Plataforma de referencia: PC 8 GB RAM (mínimo) y 16 GB (objetivo). Tarjeta de referencia por confirmar con el CTO.

## 1. Presupuesto de frame (60 FPS = 16.6 ms)

Los hilos corren en paralelo; cada uno debe caber en 16.6 ms, y se deja margen para picos.

| Hilo / categoría | Presupuesto | Notas |
|---|---|---|
| Game thread total | 8.0 ms | |
| - Física (Chaos Vehicles, 1 furgoneta + tráfico) | 2.5 ms | sustepping fijo |
| - Cargo (20 Hz, amortizado) | 0.2 ms | tope 0.5 ms en el tick |
| - IA / Pursuit (timers, sin Tick) | 1.0 ms | |
| - Checkpoint, Terrain, UI/UMG | 1.0 ms | |
| - Niagara CPU | 0.8 ms | |
| - Resto (audio, input, engine) | 2.5 ms | |
| Render thread | 6.0 ms | draw calls < 3000 |
| GPU | 14.0 ms | margen de 2.6 ms |
| Frame total (máx. de los 3) | 16.6 ms | |

Percentil 1% bajo (peor 1%) <= 22 ms. Ningún frame > 33 ms durante la conducción normal.

## 2. Memoria

| Categoría | Presupuesto 8 GB | Presupuesto 16 GB |
|---|---|---|
| Proceso total (working set) | <= 4.5 GB | <= 8 GB |
| Texturas (streaming pool) | 1.5 GB | 3 GB |
| Mallas / Nanite | 0.8 GB | 1.5 GB |
| Audio | 0.3 GB | 0.5 GB |
| Física / Chaos | 0.2 GB | 0.3 GB |
| Game state, DataAssets, UI | 0.3 GB | 0.5 GB |

VRAM: <= 5 GB en 6 GB, <= 3 GB en 4 GB. `r.Streaming.PoolSize` fijo por perfil de calidad (ej. 1500 / 3000 MB). Sin memoria creciente tras 30 min de conducción (tolerancia +100 MB).

## 3. Comandos stat e Insights

En consola (build Development o Test, nunca PIE para medir):
- `stat unit` / `stat unitgraph`: Game, Draw, GPU, RHIT.
- `stat fps`, `stat game`, `stat scenerendering`, `stat gpu`, `stat memory`, `stat llm`, `stat llmfull`, `stat streaming`, `stat niagara`, `stat physics`, `stat chaos` (VERIFY-5.8), `stat ai`.
- `ProfileGPU` (Ctrl+Shift+, en editor), `r.ScreenPercentage`, `r.Streaming.PoolSize`, `r.Nanite.Visualize` (VERIFY-5.8), `r.Lumen.*` según perfil.
- `memreport -full`, `obj list class=Texture2D`, `obj list class=StaticMesh`.
- Insights: lanzar con `-trace=cpu,gpu,frame,log,bookmark,loadtime,memtag -statnamedevents`; abrir Unreal Insights, usar Timing, Memory y Asset Loading. Marcar fases con `TRACE_BOOKMARK`.
- Scopes propios: `DECLARE_CYCLE_STAT` / `TRACE_CPUPROFILER_EVENT_SCOPE` en `UCargoSimulationComponent::TickComponent`, `ACheckpointController` (cálculo de sospecha) y `UPursuitDirectorSubsystem` (handlers de timer).
- Los nombres de CVars y stats de 5.8 se verifican en `docs/engine-reference/unreal/` antes de usarlos.

## 4. Comprobaciones por clase C++ del contrato

| Clase | Comprobar | Límite |
|---|---|---|
| UCargoItemDataAsset / UManifestDataAsset | Carga con `TSoftObjectPtr<UStaticMesh>`: no cargar de forma síncrona en juego; precargar por AssetManager (PrimaryAssetType `CargoItem`). `ComputeDeclared*` recorre pocas líneas: sin allocs. | 0 hitches de carga > 5 ms |
| UCargoSimulationComponent | Tick 20 Hz real (`PrimaryComponentTick.TickInterval = 0.05`). Coste por tick y por slot. `SetCenterOfMass` solo cuando cambia (umbral), no cada tick. Delegados solo al cruzar umbral. Sin allocs en el tick (arrays reservados). | <= 0.3 ms medio, <= 0.5 ms pico; <= 64 slots |
| ACheckpointController | Sin Tick (verificar `bCanEverTick=false`). Overlap solo con el canal del pawn; `InspectionLight` sin sombras dinámicas o con distancia corta. `PresentManifest` calcula una vez. | 0 ms en reposo; luz: 1 sombra a la vez |
| UPursuitDirectorSubsystem | Solo FTimerManager, sin Tick. Frecuencia de timer >= 0.25 s. Consultas de visibilidad (traza) acotadas por número de perseguidores. | <= 8 perseguidores IA activos; <= 16 trazas por tick de timer |
| ATerrainModifierVolume | Sin Tick; Push/Pop solo en overlap; sin trazas por frame. Volúmenes superpuestos: pila acotada. | 0 ms en reposo; <= 4 modificadores simultáneos por vehículo |
| UBlacklineWheelFront / Rear | Coste de suspensión y fricción por rueda; materiales físicos por superficie en cantidad pequeña. | Física total <= 2.5 ms |
| ABlacklineVanPawn | Sin Tick propio (usa el de Chaos). Entrada Enhanced por eventos. Trazas de interacción del tablero (canal `ECC_GameTraceChannel1`) solo al pulsar IA_Interact, nunca por frame. Alternar cámara: sin recrear componentes; la cámara inactiva se desactiva (`SetActive(false)`). Spring arm: sin trazas de colisión excesivas. | <= 1 traza de interacción por pulsación; <= 1 ms de lógica propia |

## 5. Plan de profiling

1. Línea base (semana 1): build Test vacía con furgoneta sobre asfalto. Capturar `stat unit`, Insights 60 s y `memreport -full`. Guardar en `docs/perf/` (fecha y build).
2. Escenarios obligatorios: (a) conducción en asfalto húmedo con lluvia y faros, (b) bosque/barro con 4x4, (c) checkpoint con inspección y luz, (d) persecución con el máximo de IA, (e) carga máxima de la furgoneta (64 slots), (f) cambio de cámara y tablero.
3. Cada escenario: 3 pasadas de 60 s; reportar media, p95, p99 y peor frame.
4. Memoria: sesión de 30 min con `stat llm`; comparar al inicio y al final.
5. Carga: tiempo de carga por nivel con Insights `loadtime`.
6. Regresión: repetir el escenario (a) y (d) en cada merge a main; comparar contra la línea base y avisar si hay +10% en cualquier hilo.
7. Optimizar solo tras medir: el cuello de botella se asigna al dueño (render al technical-artist; física y subsistemas al engine-programmer).
8. Todo cambio de renderer (Lumen, Nanite, VSM) requiere el OK del CTO.

## 6. Límites duros (se rompen = bloquea el merge)

- Frame > 16.6 ms de media en cualquier escenario obligatorio en la máquina de referencia.
- Cualquier frame > 50 ms fuera de pantallas de carga.
- Game thread > 8 ms, Render > 6 ms o GPU > 14 ms de media.
- Memoria > presupuesto del perfil, o crecimiento > 100 MB en 30 min.
- Actor o subsistema del contrato con Tick no autorizado (solo `UCargoSimulationComponent` a 20 Hz).
- Carga síncrona de assets durante el juego.
- Draw calls > 3000 o triángulos visibles > 5 M en el peor caso.
- Contagio de física: más de 64 cuerpos de carga activos o más de 8 vehículos Chaos simulados.
- Hitch de streaming > 100 ms.

Perfiles de calidad (Baja / Media / Alta) se definen tras la línea base; los límites son los mismos para todos, solo cambian los presupuestos de memoria y resolución.

## 7. Dudas abiertas

- GPU y CPU exactas de referencia, y si 8 GB es mínimo o solo objetivo.
- Número de vehículos de tráfico y de perseguidores en el diseño final.
- Si habrá Lumen/Nanite/VSM (afecta el presupuesto de GPU).
- Nombres de stats/CVars de Chaos y Nanite en 5.8 (VERIFY-5.8).
