# 20 — Hoja de eventos SFX (BLACKLINE: NIGHT HAUL)

Estado: BORRADOR para revisión de audio-director. No hay guía de nombres ni paleta sónica en el repo; la convención y los valores de mezcla son propuestos y deben ratificarse.
Fuente de disparadores: `CONTRATO.md` (delegados y enums). Sin Tick: todo SFX se dispara por delegado o timer.

## 1. Convenciones
- Nombre de evento: `sfx_<sistema>_<accion>` (minúsculas, guion bajo). Variantes: sufijo `_01..NN`.
- Volúmenes en dBFS de pico relativo al bus (0 dB = referencia de bus). Atenuación: Log salvo nota.
- Prioridad: 0 (más baja) a 100. Voces robadas: la de menor prioridad y más antigua.

## 2. Buses y jerarquía
| Bus | Contenido | Nivel base | Padre |
|---|---|---|---|
| Master | todo | 0 dB | - |
| Bus_Vehicle | motor, suspensión, barro, impactos propios | -3 | Master |
| Bus_Cabin | tablero, ventanilla, carga desplazándose (interior) | -4 | Master |
| Bus_World | barrera, sirenas, voces de puesto, ambiente | -5 | Master |
| Bus_Alert | stingers de veredicto/persecución, UI | -2 | Master |
| Bus_Music | música (fuera de alcance) | -8 | Master |

Reverb sends: Cabin_Small (interior, cabina, RT60 0.25 s), Exterior_Night (RT60 1.2 s, cap sugerido 1.5 s). Cámara Cockpit: Bus_Cabin +0, Bus_World con LPF 6 kHz suave; ThirdPerson: Bus_Cabin -6, Bus_World sin filtro.

## 3. Ducking (sidechain)
| Fuente | Objetivo | Reducción | Ataque / Hold / Release |
|---|---|---|---|
| Bus_Alert (stinger veredicto) | Bus_World, Bus_Vehicle | -4 dB | 20 ms / 600 ms / 900 ms |
| sfx_siren_* (Hunting) | Bus_Music | -6 dB | 100 / sostenido / 1500 ms |
| sfx_impact_* heavy (>=Sev 0.6) | Bus_Cabin, Bus_World | -6 dB | 5 / 250 / 700 ms |
| sfx_window_* durante Inspecting | Bus_Vehicle (motor) | -3 dB | 150 / sostenido / 600 ms |
| Voz de inspector (futuro) | Bus_World ambiente, Bus_Vehicle | -5 dB | 50 / sostenido / 500 ms |

## 4. Eventos

### 4.1 Tablero (disparo: `ABlacklineVanPawn::ActivateDashControl` / `FOnDashControlChanged`)
Espacial: 2D en cabina (adjuntos al pawn), sin atenuación; ThirdPerson -6 dB y LPF 4 kHz.
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Pitch | Prio | Conc. / Cooldown |
|---|---|---|---|---|---|---|---|---|
| sfx_dash_headlights_on / _off | Dash.Headlights bOn true/false | clic de interruptor plástico, off más seco | 0.15 s | -14 | 3 | +-3% | 40 | 1 / 80 ms |
| sfx_dash_wipers_on / _off | Dash.Wipers | clic + arranque motor de limpiaparabrisas; off: parada con rebote | 0.4 s | -14 | 2 | +-2% | 35 | 1 |
| sfx_wipers_loop | Wipers activos | barrido goma sobre vidrio, ciclo ~1.1 s | loop | -22 | 1 (loop) | 0 | 20 | 1; se corta con off (fade 150 ms). Con lluvia: más húmedo |
| sfx_dash_window_toggle | Dash.Window | clic + motor de ventanilla (ver 4.2) | 0.2 s | -14 | 2 | +-3% | 45 | 1 / 100 ms |
| sfx_dash_jammer_on / _off | Dash.SignalJammer | clic pesado + zumbido electrónico que sube (on); cae (off) | 0.5 s | -12 | 2 | +-2% | 55 | 1 |
| sfx_jammer_loop | Jammer activo | zumbido bajo 120 Hz + parpadeo 4-6 kHz, muy discreto | loop | -26 | 1 | 0 | 25 | 1 |
| sfx_dash_difflock_on / _off | Dash.DiffLock / SetDriveMode | palanca metálica, "clunk" de engranaje (0.1 s) | 0.45 s | -10 | 3 | +-3% | 60 | 1 / 300 ms |
| sfx_dash_ignition_start | Ignition, motor apagado->encendido | llave, burro de arranque, 2-3 intentos, encendido | 1.5-2.5 s | -8 | 3 | +-2% | 70 | 1 |
| sfx_dash_ignition_stop | Ignition, encendido->apagado | caída de RPM, tic térmico del escape | 1.2 s | -10 | 2 | +-2% | 65 | 1 |
| sfx_dash_press_fail | Interact sobre control bloqueado | clic hueco sin acción | 0.12 s | -18 | 2 | +-4% | 30 | 1 / 200 ms |
| sfx_dash_hover | trazo Interact (ECC_GameTraceChannel1) entra a botón | tick muy suave opcional | 0.05 s | -28 | 1 | +-5% | 10 | 1 / 150 ms |

Nota: el trazo usa el canal "Interact"; el sonido sale del componente con tag `Dash.<Control>`, para ubicar el emisor.

### 4.2 Ventanilla (`OpenWindowForInspection(bool)`, estado `Inspecting`)
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Notas |
|---|---|---|---|---|---|---|---|
| sfx_window_open | OpenWindowForInspection(true) | motor eléctrico + goma, entra aire exterior | 1.6 s | -12 | 2 | 50 | Al terminar, activar sfx_amb_window_open (loop ambiente exterior, +6 dB a Bus_World en Cockpit, fade 400 ms) |
| sfx_window_close | OpenWindowForInspection(false) | motor + golpe de sello | 1.4 s | -12 | 2 | 50 | Quita el aumento de exterior; ducking de ventanilla off |
| sfx_window_stuck | ventana con daño alto (DamageRatio01 > umbral) | motor forzado, chirrido | 1.0 s | -12 | 2 | 50 | Opcional, requiere acuerdo con diseño |

Reglas: la duración debe encajar con el tiempo de animación; el diseño decide el timing, el audio se escala (pitch de motor +-10%, no timestretch).

### 4.3 Barrera y puesto (`ACheckpointController`, `FOnCheckpointStateChanged`, `FOnInspectionVerdict`)
Espacial 3D, emisor en `Barrier`, atenuación Log, min 500 cm / max 6000 cm, LPF de 20 kHz a 3 kHz según distancia, oclusión activa.
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Conc. |
|---|---|---|---|---|---|---|---|
| sfx_barrier_buzzer | Idle->Approaching | zumbador de advertencia corto, 2 pulsos | 0.8 s | -10 | 2 | 55 | 1 |
| sfx_barrier_raise | Cleared / CleanPass | motor hidráulico, barra sube, tope metálico | 2.2 s | -8 | 3 | 60 | 1 por barrera |
| sfx_barrier_lower | Idle (reset) o PursuitTriggered | barra baja, golpe seco al final | 1.8 s | -8 | 3 | 60 | 1 por barrera |
| sfx_barrier_servo_loop | mientras se mueve | zumbido de servo 90-200 Hz | loop | -20 | 1 | 30 | 1 |
| sfx_barrier_hit_vehicle | barrera golpeada por la van | crujido de madera/plástico + metal | 0.7 s | -4 | 4 | 80 | 2 / 150 ms |
| sfx_checkpoint_light_on | InspectionLight activada | relé + zumbido de foco que sube | 0.6 s | -14 | 2 | 35 | 1 |
| sfx_checkpoint_radio | Inspecting (opcional) | estática de radio policial, murmullo ininteligible | 2-4 s | -22 | 4 | 25 | 1 / 8 s |
| sfx_verdict_cleanpass | CleanPass | stinger corto, resolución grave sutil (alivio) | 1.2 s | -10 | 1 | 85 | 1 (Bus_Alert) |
| sfx_verdict_secondary | SecondaryInspection | stinger de tensión, pulso agudo seco | 1.5 s | -8 | 1 | 88 | 1 (Bus_Alert) |
| sfx_verdict_pursuit | PursuitAlert | golpe + disonancia corta, silbato de alarma | 1.8 s | -6 | 1 | 95 | 1 (Bus_Alert) |
Orden: verdict dispara ducking Bus_Alert antes que barrier_lower/sirenas (retardo sirena +400 ms).

### 4.4 Sirenas (`EPursuitState`, `FOnPursuitStateChanged`)
Espacial: vehículos de persecución, 3D con Doppler (factor 0.8), min 800 cm / max 12000 cm, curva Log-reverse, LPF según distancia (20 kHz a 1.5 kHz), pan por dirección. Bus_World; el estado Hunting además activa capa 2D de "cerco" en Bus_Alert -16 dB.
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Conc. |
|---|---|---|---|---|---|---|---|
| sfx_siren_hunting_loop | Hunting | barrido wail/yelp rápido, ciclo 3-4 s, 650-1500 Hz | loop | -12 | 3 (patrones) | 90 | 3 vehículos simultáneos max; el 4.o hace virtual |
| sfx_siren_searching_loop | Searching | sirena más lenta y distante, ciclo 5-6 s, con silencios | loop | -18 | 2 | 75 | 3 |
| sfx_siren_lost_tail | Lost | sirena que se aleja con eco, fade 3 s | 3.0 s | -20 | 2 | 60 | 2 |
| sfx_siren_onset | None->Hunting | grito corto de sirena (burst) + radio | 1.0 s | -8 | 2 | 92 | 1 |
| sfx_siren_blip | Searching, cada 10-18 s aleatorio (timer) | chirp de alerta puntual | 0.3 s | -16 | 3 | 55 | 1 / 6 s |
Transiciones: crossfade 800 ms entre loops. Cap de reverb de cola Exterior_Night 1.5 s. La cantidad de vehículos la fija diseño según `Level` de `StartPursuit`; el nivel sonoro escala con `GetHeat01()`: -4 dB (heat 0) a 0 dB (heat 1) en Hunting.

### 4.5 Carga desplazándose (`FOnCargoShifted(Severity01)`, `FOnCargoIntegrityChanged`, `GetLateralG`)
Espacial: 3D anclado a la parte trasera del pawn, offset desde `GetCargoCenterOfMassLocalCm`; en Cockpit bus Cabin con LPF 2.5 kHz (sonido de caja amortiguado).
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Conc. / Cooldown |
|---|---|---|---|---|---|---|---|
| sfx_cargo_shift_light | Shifted, Severity < 0.35 | roce de cajas, golpecitos | 0.6 s | -16 | 5 | 45 | 2 / 250 ms |
| sfx_cargo_shift_medium | 0.35 <= Sev < 0.7 | arrastre + golpe de caja, tintineo | 0.9 s | -11 | 5 | 60 | 2 / 300 ms |
| sfx_cargo_shift_heavy | Sev >= 0.7 | desplome de carga, metal crujiente | 1.4 s | -6 | 4 | 75 | 1 / 600 ms |
| sfx_cargo_creak_loop | GetLateralG > umbral (comprobado a 20 Hz del componente) | crujido de correas y tablas, volumen por G | loop | -28 a -14 | 2 | 25 | 1; volumen = f(LateralG), suavizado 200 ms |
| sfx_cargo_fragile_clink | Fragile con integridad en caída (Delta < 0) | cristal/cerámica, tintineo | 0.5 s | -12 | 4 | 55 | 2 |
| sfx_cargo_volatile_hiss | Volatile y Delta < 0 | siseo de presión | 1.0 s | -14 | 2 | 65 | 1 |
| sfx_cargo_cryo_alarm | Cryogenic y Delta < 0 | bip de alarma de temperatura + siseo | 1.2 s | -12 | 2 | 65 | 1 / 5 s |
| sfx_cargo_relic_hum | Relic, integridad < 0.5 (opcional) | resonancia grave rara, sin identificar | 2.0 s | -20 | 2 | 40 | 1 / 10 s |
Los sonidos específicos de clase dependen de `ECargoClass`; el cargamento Standard solo usa shift/creak. El loop de crujido no se dispara por Tick: el componente de carga (20 Hz) ya publica los valores.

### 4.6 Barro y superficies (`ATerrainModifierVolume`, `PushSurfaceModifier`/`PopSurfaceModifier`, `ESurfaceKind`)
Espacial: 3D por rueda (4 emisores), se pueden fusionar en 2 (eje) para presupuesto de voces. Bus_Vehicle.
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Conc. |
|---|---|---|---|---|---|---|---|
| sfx_surface_roll_asphalt | Asphalt, velocidad > 5 km/h | rodadura seca, ruido de banda | loop | -22 (+velocidad) | 1 | 20 | 1 |
| sfx_surface_roll_wet | WetAsphalt | siseo de agua + salpicado | loop | -20 | 1 | 22 | 1 |
| sfx_surface_roll_gravel | Gravel | crujido de grava, granular | loop | -18 | 2 | 24 | 1 |
| sfx_surface_roll_mud | Mud | succión, chapoteo pesado, graves | loop | -14 | 2 | 28 | 1 |
| sfx_surface_roll_forest | Forest | hojas y ramas secas, crujidos | loop | -20 | 2 | 22 | 1 |
| sfx_surface_enter_mud | Push de Mud | golpe sordo, salpicadura al entrar | 0.8 s | -8 | 4 | 55 | 1 / 400 ms |
| sfx_surface_exit_mud | Pop de Mud | desprendimiento, goteo | 0.7 s | -14 | 3 | 40 | 1 |
| sfx_mud_wheelspin | Mud + patinaje (bRequires4WD y modo RWD) | motor al ralentí, barro lanzado, chapoteo rápido | loop | -10 | 2 | 65 | 1 |
| sfx_mud_stuck | Hundimiento >= StuckSinkDepthCm | borboteo, motor ahogado, ruido de succión | loop | -9 | 2 | 70 | 1 |
| sfx_mud_unstuck | Sale del estado stuck | "pop" de succión, alivio | 0.8 s | -8 | 3 | 60 | 1 |
Loops de rodadura: un solo loop por superficie activa, con crossfade 300 ms entre superficies; volumen y pitch por GetSpeedKmh (RTPC: pitch 0.8-1.3, vol -14 a 0 dB relativo). Combinar con el nivel de FrictionMultiplier para el chirrido.
Sin Tick: el volumen se actualiza vía timer (10 Hz) o delegado, no Event Tick.

### 4.7 Impactos (`ApplyImpact(ImpactSpeedMs, LocalNormal)`)
Espacial: 3D en el punto y normal local. Severidad = clamp(ImpactSpeedMs / 15, 0, 1) (propuesto; calibrar con diseño).
| Evento | Disparador | Carácter | Dur. | Nivel | Var. | Prio | Conc. / Cooldown |
|---|---|---|---|---|---|---|---|
| sfx_impact_light | Sev < 0.25 | golpe de chapa, raspón | 0.4 s | -14 | 6 | 50 | 3 / 80 ms |
| sfx_impact_medium | 0.25 <= Sev < 0.6 | golpe de chapa, vidrio de tablero tintinea | 0.8 s | -8 | 6 | 70 | 2 / 150 ms |
| sfx_impact_heavy | Sev >= 0.6 | colisión fuerte, doblez de metal, caída de piezas | 1.5 s | -2 | 5 | 92 | 1 / 400 ms |
| sfx_impact_glass | impacto en parabrisas / ventana con Sev >= 0.5 | vidrio quebrado | 1.0 s | -6 | 3 | 85 | 1 / 1 s |
| sfx_impact_scrape | contacto continuo lateral | rasguño metálico largo | loop | -14 | 2 | 55 | 1 |
| sfx_impact_prop_debris | impacto contra prop destructible | madera, plástico, rebotes | 0.8 s | -10 | 4 | 55 | 2 |
| sfx_impact_tail_cargo | acompaña con ApplyImpact (retardo 80-150 ms) | caja contra caja dentro de la van, por Sev | 0.5 s | -10 | 4 | 60 | 1; dispara sfx_cargo_shift_* si CargoIntegrity cae |
Capas en heavy: transitorio (0-30 ms, 2-5 kHz), cuerpo (200-800 Hz), cola de graves (60-120 Hz). Sidechain a Bus_Cabin/World según 3. Pitch aleatorio +-6%, volumen +-1.5 dB. Round-robin sin repetir la misma variante dos veces seguidas.

## 5. Presupuesto de voces
- Máximo 48 voces totales; Bus_Vehicle 16, Bus_Cabin 8, Bus_World 16, Bus_Alert 4, reserva 4.
- Prioridad de robo: menor prioridad primero; los eventos con prioridad >= 85 nunca se roban.
- Virtualización: loops sin audibilidad pasan a virtual (continúan, sin coste de render).

## 6. Enmascaramiento de frecuencias
- Sirenas 650-1500 Hz: recortar motor en 1 kHz (-2 dB) en Bus_Vehicle durante Hunting.
- Impactos heavy y barro ocupan 60-250 Hz: ducking de motor (4.7) evita enmascarar el motor.
- Tablero y ventanilla en 1-4 kHz: mantener por debajo de -12 dB para no competir con alertas.

## 7. Dudas abiertas
1. Falta guía de nombres y paleta sónica del audio-director: ¿se acepta `sfx_<sistema>_<accion>`?
2. Validar escala de severidad de impacto (ImpactSpeedMs / 15) y umbral de LateralG con diseño.
3. Ventanilla atascada, radio del puesto y relic_hum: ¿se incluyen en la demo?
4. Middleware (FMOD/Wwise) o MetaSounds nativo: afecta RTPC, ducking y presupuesto de voces.
5. Voz de inspector: ¿habrá diálogo? Define el ducking correspondiente.
