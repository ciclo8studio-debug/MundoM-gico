# 11 — Casos de prueba · BLACKLINE: Night Haul (vertical slice)

**Estado:** borrador para revisión del CTO · **Fecha:** 2026-11-21 · **Motor:** UE 5.8.4 (C++, módulo `Blackline`)
**Fuentes:** `CONTRATO.md`, `docs/00_vision_pilares.md`, `docs/01_plan_riesgos.md`

## 0. Convenciones

- **Ubicación de tests automatizados:** `Source/Blackline/Private/Tests/`, nombres `Blackline.<Sistema>.<Escenario>` (UE Automation).
- **Tipo y gate por caso:**
  - **Logic** (fórmulas, estados, eventos): test automatizado, **BLOCKING**.
  - **Integration** (varios sistemas en mapa de prueba): test o playtest documentado, **BLOCKING**.
  - **Visual/Feel** (cámara, audio, sensación): captura o video retenido en `production/qa/evidence/` + nota de aprobación, **BLOCKING**.
  - **Config/Data** (tuning): smoke check, **ADVISORY**.
- **Valores numéricos:** el `CONTRATO.md` no fija números. Todo valor marcado *(propuesto)* es un default inicial para `EditAnywhere` / `DA_Tuning` y debe confirmarse antes de congelar el caso. Si el tuning cambia un valor, se cambia la tabla, no el criterio. Los criterios de banda (p. ej. "veredicto CleanPass") no dependen del número exacto.
- **Tolerancias:** distancias ±1 cm; masas ±0.5 kg; tiempos ±0.2 s; flotantes ±0.01 salvo que se indique.

### Valores de referencia (propuestos)

| Parámetro | Valor | Nota |
|---|---|---|
| Masa vacía de la van | 2400 kg | |
| CoM base van (local) | (0, 0, 40) cm | X adelante, Y derecha, Z arriba |
| Masa Standard / Fragile / Volatile | 800 / 400 / 300 kg | |
| Masa Cryogenic / Relic / AIServer | 500 / 150 / 120 kg | |
| Fragility01: Standard / Fragile / Volatile / Relic / AIServer / Cryogenic | 0.2 / 0.9 / 0.6 / 0.7 / 0.8 / 0.5 | |
| Umbral de daño por impacto | 4 m/s | Pérdida = Fragility01 × clamp((v − 4) / 12, 0, 1) |
| Umbral de desplazamiento lateral | 0.55 g | Severidad = clamp((G − 0.55) / 0.45, 0, 1); G = v² / R / 9.81 |
| Velocidad máxima para entrar a Inspecting | 5 km/h (1.39 m/s) | |
| Veredictos por sospecha | < 0.35 CleanPass · 0.35–0.69 SecondaryInspection · ≥ 0.70 PursuitAlert | |
| Persecución: Hunting → Searching | 8 s sin visibilidad | |
| Persecución: Searching → Lost | 20 s sin visibilidad | Con motor **y** luces apagados: 10 s (factor 0.5) |
| Fricción por superficie (FrictionMultiplier) | Asfalto 1.0 · Mojado 0.7 · Grava 0.6 · Bosque 0.5 · Barro 0.35 | |
| Hundimiento en barro (StuckSinkDepthCm) | 25 cm | |
| Blend de cámara cabina ↔ tercera persona | 0.25 s | |
| Longitud de traza Interact | 150 cm | |
| Frecuencia del componente de carga | 20 Hz (intervalo 0.05 s) | Única excepción a "sin Tick" |

---

## 1. Carga y centro de gravedad (`UCargoSimulationComponent`, `UCargoItemDataAsset`)

**Tipo:** Logic · **Gate:** BLOCKING · **Evidencia:** test automatizado

### CP-C01 — Furgón vacío
**Precondición:** van con `UCargoSimulationComponent` sin cargas (`ClearCargo()` ejecutado).
**Pasos:**
  1. Leer `GetTotalCargoMassKg()`, `GetCargoCenterOfMassLocalCm()`, `GetCargoIntegrity01()`, `GetUnshieldedSignature01()`.
  2. Leer `GetLateralG()` con la van quieta.
**Resultado esperado:** masa 0 kg; CoM de carga (0, 0, 0) cm; integridad 1.00; firma sin blindaje 0.00; G lateral 0.00.
**Criterio de aprobación:** los cinco valores coinciden exactamente; ningún NaN.

### CP-C02 — Una carga Standard
**Precondición:** van vacía; item Standard (800 kg, firma 0.0, sin blindaje requerido).
**Pasos:**
  1. `AddCargo(Standard, (0, 0, 50))` → debe devolver `true`.
  2. Leer masa, CoM de carga, firma sin blindaje.
**Resultado esperado:** masa 800 kg; CoM de carga (0, 0, 50); firma sin blindaje 0.00.
**Criterio de aprobación:** masa 800 ±0.5; CoM ±1 cm en cada eje.

### CP-C03 — Dos cargas, CoM ponderado
**Precondición:** van vacía.
**Pasos:**
  1. `AddCargo(Standard 800 kg, (0, 0, 50))`.
  2. `AddCargo(Fragile 400 kg, (0, 100, 50))`.
  3. Leer CoM de carga y masa total.
  4. Calcular el CoM del cuerpo esperado: van 2400 kg en (0, 0, 40) + carga 1200 kg en el CoM de carga.
**Resultado esperado:** masa 1200 kg; CoM de carga (0, 33.3, 50); CoM del cuerpo (0, 11.1, 43.3) cm.
**Criterio de aprobación:** Y de carga 33.3 ±0.5; CoM del cuerpo dentro de ±0.5 cm del valor calculado. Si el CoM de Chaos no coincide con la referencia local de la van, se marca `// VERIFY-5.8` y se abre duda.

### CP-C04 — Masa cero y offset cero (borde)
**Precondición:** van vacía; item de prueba con `MassKg = 0`.
**Pasos:**
  1. `AddCargo(item masa 0, (0, 0, 0))`.
  2. Leer CoM de carga y masa.
**Resultado esperado:** sin división entre cero; CoM (0, 0, 0); masa 0 kg; la llamada no crashea.
**Criterio de aprobación:** `isfinite` en todos los componentes del CoM; sin crash ni warning de NaN en el log `LogBlackline`.

### CP-C05 — Carga máxima de prueba
**Precondición:** van vacía; 10 items Cryogenic (500 kg c/u) con offsets distintos dentro de ±100 cm en Y y ±50 cm en X.
**Pasos:**
  1. Agregar los 10 items.
  2. Leer masa total y CoM de carga.
  3. Ejecutar `ApplyImpact(0, (0,0,1))` para confirmar que no altera la masa.
**Resultado esperado:** masa 5000 kg; CoM dentro de la caja envolvente de los offsets; sin NaN; sin pérdida de integridad.
**Criterio de aprobación:** masa 5000 ±0.5; cada componente del CoM dentro del rango de offsets ±1 cm.

### CP-C06 — Impacto bajo el umbral
**Precondición:** una carga Fragile (400 kg) cargada; integridad 1.00.
**Pasos:**
  1. Suscribirse a `OnCargoIntegrityChanged`.
  2. `ApplyImpact(3.0, (1,0,0))`.
  3. Leer integridad.
**Resultado esperado:** integridad 1.00; el delegado no se dispara.
**Criterio de aprobación:** contador de disparos = 0 e integridad = 1.00 ±0.01.

### CP-C07 — Impacto sobre el umbral (Fragile vs Standard)
**Precondición:** Fragile 400 kg y, en otra prueba, Standard 800 kg, ambos con integridad 1.00.
**Pasos:**
  1. `ApplyImpact(10.0, (1,0,0))` con Fragile cargada.
  2. Leer integridad y el `Delta` del delegado.
  3. Repetir con Standard.
**Resultado esperado:** Fragile → Δ = −0.45, integridad 0.55; Standard → Δ = −0.10, integridad 0.90. El delegado se dispara una vez por impacto con el Δ correspondiente.
**Criterio de aprobación:** Δ dentro de ±0.01 de los valores; un solo disparo por impacto.

### CP-C08 — Desplazamiento lateral: curva suave vs. curva cerrada
**Precondición:** van cargada con Fragile 400 kg en (0, 100, 50); mapa de prueba con circunferencia de radio 50 m.
**Pasos:**
  1. Conducir a 40 km/h (11.1 m/s) en el círculo. Registrar G y disparos de `OnCargoShifted`.
  2. Conducir a 80 km/h (22.2 m/s) en el mismo círculo. Registrar G y disparos.
**Resultado esperado:** a 40 km/h, G ≈ 0.25 g y no hay desplazamiento. A 80 km/h, G ≈ 1.00 g y hay al menos un desplazamiento con severidad ≥ 0.9.
**Criterio de aprobación:** 40 km/h → 0 disparos; 80 km/h → ≥ 1 disparo con severidad ≥ 0.9 (ambas medidas en log).

---

## 2. Retén (`ACheckpointController`) — 5 estados

**Tipo:** Integration · **Gate:** BLOCKING · **Evidencia:** test o playtest documentado en mapa de prueba con un checkpoint y una van

### CP-R01 — Estado inicial
**Precondición:** checkpoint recién colocado en el mapa; sin van en zona.
**Pasos:**
  1. Leer `GetState()` y `GetSuspicion01()`.
**Resultado esperado:** `Idle`; sospecha 0.00.
**Criterio de aprobación:** estado = `Idle` y sospecha = 0.00 exactos.

### CP-R02 — Idle → Approaching
**Precondición:** checkpoint en `Idle`; van a 40 km/h acercándose desde 30 m antes de la barrera.
**Pasos:**
  1. Conducir hacia la barrera sin frenar.
  2. Al entrar en la zona de aproximación, leer estado y registrar disparos de `OnCheckpointStateChanged`.
**Resultado esperado:** estado `Approaching`; un único disparo `(Idle, Approaching)`.
**Criterio de aprobación:** un disparo con los parámetros exactos. Si la zona de aproximación no existe en el contrato, ver duda D-01.

### CP-R03 — Approaching → Inspecting, solo con velocidad ≤ 5 km/h
**Precondición:** checkpoint en `Approaching`; van en `InspectionZone`.
**Pasos:**
  1. Mantener 20 km/h dentro de la zona durante 3 s. Leer estado.
  2. Frenar hasta 4 km/h. Esperar 1 s. Leer estado.
**Resultado esperado:** en el paso 1 el estado sigue en `Approaching`; en el paso 2 pasa a `Inspecting`.
**Criterio de aprobación:** paso 1 = `Approaching`; paso 2 = `Inspecting` dentro de 1.5 s desde el frenado.

### CP-R04 — Inspecting → Cleared con veredicto CleanPass
**Precondición:** checkpoint en `Inspecting`; van con luces encendidas, sin daño, integridad 1.00, firma sin blindaje 0.00; manifiesto que coincide con la carga (masa ±2 %, firma ≤ 0.2).
**Pasos:**
  1. Llamar `PresentManifest(manifiesto)`.
  2. Esperar el veredicto. Registrar `OnInspectionVerdict` y `GetSuspicion01()`.
**Resultado esperado:** veredicto `CleanPass`; sospecha < 0.35; estado `Cleared`.
**Criterio de aprobación:** los tres valores cumplen la banda; el veredicto se emite una sola vez.

### CP-R05 — Inspecting con SecondaryInspection
**Precondición:** checkpoint en `Inspecting`; van con luces apagadas y firma sin blindaje 0.5 (dentro de la banda media).
**Pasos:**
  1. `PresentManifest(manifiesto)`.
  2. Registrar veredicto, sospecha y estado tras 5 s.
**Resultado esperado:** veredicto `SecondaryInspection`; sospecha entre 0.35 y 0.69; estado **distinto** de `Cleared` y de `PursuitTriggered` (el estado exacto está en duda D-02).
**Criterio de aprobación:** veredicto y banda de sospecha cumplidos; el estado no es `Cleared` ni `PursuitTriggered`.

### CP-R06 — Inspecting → PursuitTriggered con PursuitAlert
**Precondición:** checkpoint en `Inspecting`; van con luces apagadas, firma sin blindaje 0.9, bSignalJammerOn = false, daño 0.6, manifiesto con firma declarada 0.1.
**Pasos:**
  1. `PresentManifest(manifiesto)`.
  2. Registrar veredicto, sospecha, estado y llamadas a `UPursuitDirectorSubsystem::StartPursuit`.
**Resultado esperado:** veredicto `PursuitAlert`; sospecha ≥ 0.70; estado `PursuitTriggered`; `StartPursuit` llamado una vez con la van como target y nivel 1 (propuesto).
**Criterio de aprobación:** los cuatro puntos se cumplen; `StartPursuit` recibe el target correcto.

### CP-R07 — Cleared → Idle y sin doble inspección
**Precondición:** checkpoint en `Cleared` (caso CP-R04); van sale de la zona de inspección.
**Pasos:**
  1. Alejarse 15 m de la zona y esperar 5 s. Leer estado.
  2. Volver a entrar a 4 km/h.
**Resultado esperado:** tras salir, estado `Idle`; al volver a entrar, no hay un segundo veredicto para el mismo paso (el estado vuelve a `Approaching`, no a `Inspecting`, hasta confirmar la regla de re-inspección, duda D-03).
**Criterio de aprobación:** paso 1 = `Idle`; paso 2 sin veredicto nuevo dentro de 10 s.

### CP-R08 — Ciclo completo, transiciones exactas
**Precondición:** checkpoint en `Idle`; van con caso CP-R04 (limpia).
**Pasos:**
  1. Recorrer: aproximación, frenado, `PresentManifest`, salida de zona.
  2. Registrar **todas** las emisiones de `OnCheckpointStateChanged` en orden.
**Resultado esperado:** exactamente cuatro transiciones: `(Idle→Approaching)`, `(Approaching→Inspecting)`, `(Inspecting→Cleared)`, `(Cleared→Idle)`.
**Criterio de aprobación:** lista de transiciones igual a la esperada, sin duplicados ni transiciones intermedias.

---

## 3. Sospecha (`FSuspicionSnapshot`) — apoyo al retén

**Tipo:** Logic · **Gate:** BLOCKING · **Evidencia:** test automatizado sobre `GetSuspicionSnapshot()`

### CP-S01 — Snapshot limpio
**Precondición:** van con luces encendidas, daño 0, firma sin blindaje 0.0, integridad 1.0, jammer apagado, velocidad al frenar 4 km/h.
**Pasos:**
  1. Leer `GetSuspicionSnapshot()`.
**Resultado esperado:** `SpeedBeforeStopKmh` = 4; `bHeadlightsOn` = true; `DamageRatio01` = 0; `UnshieldedSignature01` = 0; `CargoIntegrity01` = 1.
**Criterio de aprobación:** cada campo coincide con el estado real de la van (sin valores de caché viejos).

### CP-S02 — Cada factor sube la sospecha de forma independiente
**Precondición:** partir de CP-S01 y medir sospecha base.
**Pasos:**
  1. Apagar luces → medir.
  2. Volver a luces encendidas; subir daño a 0.5 → medir.
  3. Repetir con firma sin blindaje 0.5 y con integridad de carga 0.5.
**Resultado esperado:** cada paso aumenta la sospecha respecto del anterior (monotonía); ningún factor la reduce.
**Criterio de aprobación:** cuatro valores estrictamente crecientes respecto de la base; log de factores muestra el aporte de cada uno.

---

## 4. Persecución (`UPursuitDirectorSubsystem`)

**Tipo:** Integration · **Gate:** BLOCKING · **Evidencia:** test automatizado con timers simulados + playtest de escape

### CP-P01 — Inicio de persecución
**Precondición:** sin persecución (`GetState() == None`).
**Pasos:**
  1. `StartPursuit(van, 1)`.
  2. Leer estado y calor; registrar `OnPursuitStateChanged`.
**Resultado esperado:** estado `Hunting`; calor > 0.00 y ≤ 1.00; un disparo `Hunting`.
**Criterio de aprobación:** los tres puntos se cumplen.

### CP-P02 — Hunting → Searching a los 8 s
**Precondición:** persecución en `Hunting`; `ReportTargetVisible(true)` al inicio.
**Pasos:**
  1. `ReportTargetVisible(false)` en t = 0.
  2. Leer estado en t = 7.8 s y en t = 8.2 s.
**Resultado esperado:** 7.8 s → `Hunting`; 8.2 s → `Searching`.
**Criterio de aprobación:** la transición ocurre en 8.0 s ±0.2 s.

### CP-P03 — Searching → Lost a los 20 s (o 10 s con motor y luces apagados)
**Precondición:** persecución en `Searching`; objetivo no visible.
**Pasos:**
  1. Caso A: `ReportEngineAndLights(true, true)` durante 25 s. Medir el tiempo hasta `Lost`.
  2. Caso B: `ReportEngineAndLights(false, false)`. Medir el tiempo hasta `Lost`.
**Resultado esperado:** caso A → `Lost` a 20 s ±0.2 s desde entrar a `Searching`; caso B → `Lost` a 10 s ±0.2 s.
**Criterio de aprobación:** ambos tiempos dentro de tolerancia; caso A no llega a `Lost` antes de 20 s.

### CP-P04 — Reacquisición cancela el escape
**Precondición:** persecución en `Searching` desde hace 10 s.
**Pasos:**
  1. `ReportTargetVisible(true)`.
  2. Esperar 15 s con visibilidad en true. Leer estado.
**Resultado esperado:** estado `Hunting` tras la reacquisición; sin `Lost` en la ventana.
**Criterio de aprobación:** estado = `Hunting` al final; no hay disparo `Lost`; los timers previos no disparan transición.

### CP-P05 — Sin Tick en la persecución
**Precondición:** `UPursuitDirectorSubsystem` activo.
**Pasos:**
  1. Revisar que el subsistema no declara Tick (`bCanEverTick == false` o equivalente).
  2. Iniciar persecución y medir `stat game`/conteo de ticks durante 10 s.
**Resultado esperado:** el subsistema no tiene Tick; las transiciones usan `FTimerManager`.
**Criterio de aprobación:** revisión de código sin `Tick`; no aparecen ticks propios en el perfil.

### CP-P06 — StopPursuit limpia el estado
**Precondición:** persecución en `Searching` con timers activos.
**Pasos:**
  1. `StopPursuit()`.
  2. Esperar 25 s y leer estado. Registrar disparos posteriores.
**Resultado esperado:** estado `None`; ningún disparo de `OnPursuitStateChanged` tras `StopPursuit`.
**Criterio de aprobación:** estado = `None`; cero disparos en 25 s.

---

## 5. Terreno: barro, 2WD vs 4WD (`ATerrainModifierVolume`, `ABlacklineVanPawn`)

**Tipo:** Integration · **Gate:** BLOCKING · **Evidencia:** test de distancia en mapa de prueba con barro; playtest documentado

### CP-T01 — Asfalto seco, línea base
**Precondición:** van vacía en RWD; asfalto seco, sin volumen de barro.
**Pasos:**
  1. Arrancar desde parado con acelerador a fondo durante 10 s.
  2. Medir distancia recorrida.
**Resultado esperado:** distancia ≥ 60 m (propuesto); sin hundimiento.
**Criterio de aprobación:** distancia ≥ 60 m y sumidero = 0 cm.

### CP-T02 — Barro en RWD queda atascado
**Precondición:** van vacía en RWD; entrada a volumen `Mud` (multiplicador 0.35, `bRequires4WD = true`).
**Pasos:**
  1. Entrar al barro a 15 km/h y acelerar a fondo durante 10 s.
  2. Medir distancia y hundimiento de las ruedas traseras.
**Resultado esperado:** distancia ≤ 2 m; hundimiento ≥ 25 cm; velocidad < 3 km/h a los 5 s.
**Criterio de aprobación:** los tres valores cumplen; el furgón se considera atascado.

### CP-T03 — Barro en 4WD con DiffLock cruza
**Precondición:** van vacía; volumen `Mud` idéntico a CP-T02.
**Pasos:**
  1. Activar tracción 4WD (`SetDriveMode(FourWheel)`) y DiffLock desde el tablero (ver CP-D02).
  2. Entrar al barro a 15 km/h y acelerar a fondo durante 10 s.
  3. Medir distancia y hundimiento.
**Resultado esperado:** distancia ≥ 20 m; hundimiento < 10 cm; el furgón sale del barro.
**Criterio de aprobación:** distancia ≥ 20 m y hundimiento < 10 cm.

### CP-T04 — 4WD sin DiffLock (ruta de duda)
**Precondición:** igual a CP-T03, pero con DiffLock apagado.
**Pasos:**
  1. Repetir la prueba de CP-T03 sin DiffLock.
  2. Medir distancia.
**Resultado esperado:** distancia menor que en CP-T03 en al menos 20 %; el resultado de pasar o no depende de si DiffLock es obligatorio (duda D-04).
**Criterio de aprobación:** la distancia es menor que CP-T03 en ≥ 20 %; el comportamiento se documenta junto con D-04.

### CP-T05 — Cambio de modo de tracción dentro del barro
**Precondición:** van en RWD dentro de `Mud`, velocidad 0.
**Pasos:**
  1. `SetDriveMode(FourWheel)`. Leer `GetDriveMode()`.
  2. Acelerar 5 s. Medir distancia.
  3. `SetDriveMode(RWD)`. Leer `GetDriveMode()`.
**Resultado esperado:** modo `FourWheel` después del paso 1, `RWD` después del paso 3; sin crash; sin pérdida de referencia a los volúmenes.
**Criterio de aprobación:** ambos modos correctos; log sin errores.

### CP-T06 — Multiplicador por rueda al entrar y salir
**Precondición:** van en asfalto; `FrictionForceMultiplier` base de cada rueda leído (`base`).
**Pasos:**
  1. `PushSurfaceModifier(volumen Mud)`. Leer `FrictionForceMultiplier` de las 4 ruedas.
  2. `PopSurfaceModifier(volumen Mud)`. Leer de nuevo.
**Resultado esperado:** tras Push, cada rueda = 0.35 × base; tras Pop, cada rueda = base.
**Criterio de aprobación:** cada rueda dentro de ±0.01 del valor esperado en ambos pasos.

### CP-T07 — Solapamiento de volúmenes (barro dentro de grava)
**Precondición:** volumen `Gravel` (0.6) con volumen `Mud` (0.35) dentro.
**Pasos:**
  1. Entrar a Gravel. Entrar a Mud.
  2. Salir de Mud. Leer multiplicador.
  3. Salir de Gravel. Leer multiplicador.
  4. Llamar `PopSurfaceModifier` de un volumen que nunca se hizo Push.
**Resultado esperado:** en paso 2, multiplicador = 0.6 (grava); en paso 3, = base; el paso 4 no crashea y deja un aviso en `LogBlackline`.
**Criterio de aprobación:** valores exactos en los pasos 2 y 3; paso 4 sin crash y con aviso en log.

---

## 6. Cámara (`ABlacklineVanPawn`)

**Tipo:** Visual/Feel · **Gate:** BLOCKING · **Evidencia:** video o captura retenida en `production/qa/evidence/` + nota de aprobación del CTO

### CP-K01 — Alternar cámaras sin estado roto
**Precondición:** van en cabina.
**Pasos:**
  1. `ToggleCamera()` 10 veces, esperando 1 s entre cada una.
  2. Tras cada toggle, leer cámara activa.
**Resultado esperado:** la cámara activa alterna entre `CockpitCamera` y `ChaseCamera` en cada paso; sin crash; el estado de conducción no cambia.
**Criterio de aprobación:** 10 de 10 toggles con la cámara correcta; velocidad y dirección no se modifican por el toggle.

### CP-K02 — Transición sin corte brusco
**Precondición:** van a 40 km/h en línea recta; grabación de pantalla a 60 FPS.
**Pasos:**
  1. Ejecutar `ToggleCamera()`.
  2. Revisar la grabación cuadro a cuadro durante el blend de 0.25 s.
**Resultado esperado:** el movimiento de cámara es continuo; no hay salto de posición mayor que el desplazamiento de la van más 10 cm en un cuadro.
**Criterio de aprobación:** ningún salto que exceda ese límite; el CTO aprueba la nota de sensación.

### CP-K03 — Sin mareo en curva
**Precondición:** tercera persona; van a 60 km/h en curva de 50 m (≈0.57 g).
**Pasos:**
  1. Mantener la curva 15 s.
  2. Medir inclinación (roll) de la cámara y retardo del brazo de cámara.
**Resultado esperado:** roll ≤ 3°; retardo del brazo ≤ 0.15 s; sin sacudidas visibles en la grabación.
**Criterio de aprobación:** roll ≤ 3° y retardo ≤ 0.15 s medidos; nota de aprobación del CTO sobre la sensación.

---

## 7. Tablero (`ABlacklineVanPawn::ActivateDashControl`, traza Interact)

**Tipo:** Integration · **Gate:** BLOCKING · **Evidencia:** test de traza en mapa de prueba + playtest en cabina

### CP-D01 — Traza a los seis botones
**Precondición:** cabina con los seis botones con tags `Dash.Headlights`, `Dash.Wipers`, `Dash.Window`, `Dash.SignalJammer`, `Dash.DiffLock`, `Dash.Ignition`; canal `ECC_GameTraceChannel1` configurado (riesgo 7 del plan).
**Pasos:**
  1. Desde la posición de cabina, apuntar a cada botón a 100 cm. Trazar en canal Interact.
  2. Repetir a 200 cm.
**Resultado esperado:** a 100 cm, los seis botones son golpeados y tienen su tag; a 200 cm no hay golpe.
**Criterio de aprobación:** 6 de 6 a 100 cm con tag correcto; 0 de 6 a 200 cm.

### CP-D02 — Cada control alterna su estado
**Precondición:** van con todos los controles apagados.
**Pasos:**
  1. Para cada uno de los seis controles: `ActivateDashControl(X)` dos veces.
  2. Registrar cada disparo de `FOnDashControlChanged`.
**Resultado esperado:** doce disparos en total, dos por control, con `bOn` true y luego false.
**Criterio de aprobación:** 12 disparos, orden correcto por control, sin disparos extra.

### CP-D03 — Encendido activa la conducción
**Precondición:** motor apagado (`IsEngineOn() == false`).
**Pasos:**
  1. Acelerar a fondo 5 s. Medir velocidad.
  2. `ActivateDashControl(Ignition)`. Leer `IsEngineOn()`.
  3. Acelerar a fondo 5 s. Medir velocidad.
**Resultado esperado:** paso 1 = 0 km/h; paso 2 = true; paso 3 > 10 km/h.
**Criterio de aprobación:** velocidad 0 km/h con motor apagado; > 10 km/h con motor encendido.

### CP-D04 — Ventanilla para inspección
**Precondición:** checkpoint en `Inspecting`; ventanilla cerrada.
**Pasos:**
  1. Activar `Dash.Window` (o `OpenWindowForInspection(true)`).
  2. Esperar el veredicto (CP-R04).
**Resultado esperado:** la ventanilla queda abierta y la acción registra el estado; el snapshot refleja la ventanilla solo si el contrato la incluye (duda D-05).
**Criterio de aprobación:** estado de ventanilla correcto en 100 % de las pruebas; el alcance en la sospecha queda documentado con D-05.

---

## 8. Audio

**Tipo:** Visual/Feel (sensación) · **Gate:** BLOCKING · **Evidencia:** grabación de audio + nota del CTO. El contrato no define API de audio: los casos dependen de assets y de la duda D-06.

### CP-A01 — Motor sube de tono con la velocidad
**Precondición:** van en asfalto; grabación de audio.
**Pasos:**
  1. Acelerar de 0 a 100 km/h en 20 s, sin cambios bruscos de carga.
  2. Medir el tono (pitch) cada 5 km/h.
**Resultado esperado:** el tono sube de forma monótona; no hay saltos mayores que 1 semitono en 100 ms.
**Criterio de aprobación:** monotonía verificada en la grabación; salto máximo ≤ 1 semitono.

### CP-A02 — Superficie cambia el sonido
**Precondición:** van en asfalto a 20 km/h, luego en grava y luego en barro.
**Pasos:**
  1. Rodar 5 s en asfalto. Registrar la capa de sonido.
  2. Entrar a grava. Registrar la capa.
  3. Entrar a barro. Registrar la capa.
**Resultado esperado:** cada superficie tiene una capa distinta; la capa de barro aparece en ≤ 200 ms tras el overlap.
**Criterio de aprobación:** tres capas distintas; latencia de cambio ≤ 200 ms medida en la grabación.

### CP-A03 — Apagado del motor
**Precondición:** motor encendido a ralentí.
**Pasos:**
  1. `ActivateDashControl(Ignition)` para apagar el motor.
  2. Medir el tiempo hasta silencio del loop de motor.
**Resultado esperado:** el loop de motor baja de volumen y queda en silencio en ≤ 1.0 s.
**Criterio de aprobación:** silencio en ≤ 1.0 s (±0.1 s).

---

## 9. Rendimiento y estabilidad

**Tipo:** Integration · **Gate:** BLOCKING para F-01 y F-02 · **Evidencia:** captura de `stat unit` / `stat fps` y CSV de frame time en `production/qa/evidence/`

**Objetivo:** 60 FPS estables (pedido del CTO). Ver duda D-07: el pilar de `00_vision_pilares.md` dice 30 FPS.

### CP-F01 — 60 FPS en ruta normal
**Precondición:** build **empaquetada** en el equipo de referencia (D-08); ruta de 10 min sin persecución; van cargada con Standard 800 kg; luces encendidas.
**Pasos:**
  1. Iniciar la ruta y registrar frame time durante 10 min.
  2. Calcular FPS promedio, 1 % low y p99 de frame time.
**Resultado esperado:** promedio ≥ 60 FPS; 1 % low ≥ 50 FPS; p99 de frame time ≤ 20 ms; frames > 33.3 ms ≤ 1 %.
**Criterio de aprobación:** los cuatro valores cumplen. Si no, se abre bug con el perfil adjunto.

### CP-F02 — 60 FPS en el peor caso
**Precondición:** igual a CP-F01, con persecución en `Hunting`, lluvia (Niagara) activa y carga Fragile + Volatile.
**Pasos:**
  1. Provocar persecución en el primer retén.
  2. Mantener persecución 5 min con lluvia.
  3. Registrar frame time.
**Resultado esperado:** mismos umbrales que CP-F01.
**Criterio de aprobación:** mismos cuatro valores; el nivel de LOD de Niagara queda registrado.

### CP-F03 — Sin crash en 30 min
**Precondición:** build empaquetada; contrato completo (carga, retén, persecución, barro).
**Pasos:**
  1. Recorrer un contrato completo de punta a punta, repetido, durante 30 min.
  2. Revisar `LogBlackline` y el log de crash.
**Resultado esperado:** cero crashes; cero errores `Fatal` o `Error` de `LogBlackline` no esperados.
**Criterio de aprobación:** 0 crashes y 0 errores no esperados en 30 min.

---

## 10. Dudas abiertas (requieren ruling antes de congelar los casos)

| ID | Duda | Casos afectados | Quién decide |
|---|---|---|---|
| D-01 | La zona de aproximación no está en el contrato. ¿Es un BoxComponent extra o una distancia a la barrera? | CP-R02, CP-R03 | technical-director |
| D-02 | ¿Qué estado tras `SecondaryInspection`? ¿Vuelve a `Inspecting` o pasa a un estado nuevo? | CP-R05 | game-designer |
| D-03 | ¿Se puede re-inspeccionar la misma van tras `Cleared`? | CP-R07 | game-designer |
| D-04 | ¿DiffLock es obligatorio para cruzar barro en 4WD o solo mejora la tracción? | CP-T03, CP-T04 | game-designer |
| D-05 | La ventanilla no está en `FSuspicionSnapshot`. ¿Debe afectar la sospecha? | CP-D04 | game-designer |
| D-06 | El contrato no define API de audio. ¿Quién crea los assets y en qué clase viven los casos? | Sección 8 | audio / technical-director |
| D-07 | Objetivo de FPS: 60 (pedido del CTO) frente a 30 (pilar de `00_vision_pilares.md`). Se requiere ruling. | Sección 9 | creative-director / CTO |
| D-08 | Equipo de referencia para medir FPS no está definido. El portátil de 8 GB con iGPU tuvo 2-3 FPS en builds de 20-11 (límite de hardware), así que 60 FPS allí es poco probable. | Sección 9 | CTO |
| D-09 | ChaosVehicles figura como **Experimental** en `docs/engine-reference/unreal/PLUGINS.md`. Confirmar la API de 5.8.4 de `SetCenterOfMass` y de `FrictionForceMultiplier` (`// VERIFY-5.8`). | CP-C03, CP-T06 | technical-director |
| D-10 | ¿Lost pasa a None solo, o solo con `StopPursuit`? | CP-P03, CP-P06 | game-designer |

---

## 11. Resumen de cobertura

| Sección | Casos | Tipo principal |
|---|---|---|
| 1. Carga y CoM | CP-C01 a CP-C08 (8) | Logic |
| 2. Retén, 5 estados | CP-R01 a CP-R08 (8) | Integration |
| 3. Sospecha | CP-S01, CP-S02 (2) | Logic |
| 4. Persecución | CP-P01 a CP-P06 (6) | Integration |
| 5. Terreno 2WD/4WD | CP-T01 a CP-T07 (7) | Integration |
| 6. Cámara | CP-K01 a CP-K03 (3) | Visual/Feel |
| 7. Tablero | CP-D01 a CP-D04 (4) | Integration |
| 8. Audio | CP-A01 a CP-A03 (3) | Visual/Feel |
| 9. Rendimiento | CP-F01 a CP-F03 (3) | Integration |
| **Total** | **44** | |

Cobertura de pilares: carga (sección 1), retén con 5 estados (sección 2) y los tres veredictos, persecución con `Hunting → Searching → Lost` (sección 4), barro con 2WD y 4WD (sección 5), tablero con 6 controles (sección 7), cámara (sección 6) y audio (sección 8).
