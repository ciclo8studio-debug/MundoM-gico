# 13 — UX de cabina (diegética, sin HUD tradicional)

Base: `CONTRATO.md`. Tipos: `EDashControl`, `ECheckpointState`, `EInspectionVerdict`, `FSuspicionSnapshot`. Estado: borrador de diseño, pendiente de validar con playtest.

## 1. Principios
- Toda la información vive en la cabina o en el mundo: sin barras, minimapa ni iconos flotantes.
- Nunca solo color: cada señal combina al menos dos canales (forma/texto, sonido, movimiento).
- Todo se opera solo con teclado o solo con gamepad.
- Sin parpadeo mayor a 3 Hz; no hay destellos de pantalla completa.

## 2. Interacción diegética (mirar botón → resaltado → acción)

### Flujo
1. **Mirar:** `IA_Look` mueve la cámara de cabina. Cada frame no: usar un timer a 20 Hz que lanza una traza por `ECC_GameTraceChannel1` (Interact) desde el centro de `CockpitCamera`, alcance ~120 cm. Sin Tick.
2. **Resaltado:** al acertar un componente con tag `Dash.*`, se activa un contorno (custom depth o emisivo) en el botón. Etiqueta diegética pequeña, grabada en el panel (texto + pictograma), visible siempre en baja intensidad y más nítida al resaltar.
3. **Acción:** `IA_Interact` llama `ABlacklineVanPawn::ActivateDashControl(EDashControl)`.
4. **Confirmación:** `FOnDashControlChanged(control, bOn)` dispara el feedback (ver tabla).
5. **Salir de la mira:** el resaltado se apaga con 150 ms de gracia, para evitar parpadeo en el borde.

### Ayudas de precisión (Fitts)
- Colisión de traza del botón ~30% mayor que su malla visible.
- Magnetismo: si dos botones son candidatos, gana el más cercano al centro de la mira.
- Gamepad: sensibilidad de mirada reducida un 40% mientras hay un botón resaltado.
- Atajo opcional: mantener `IA_Interact` abre una rueda de 6 sectores (uno por `EDashControl`), que se elige con la palanca o las flechas. Es la alternativa para quien no pueda apuntar.
- No se permite interactuar mientras `IA_Look` está en movimiento rápido en conducción a alta velocidad: se sube el alcance de la traza en vez de bloquear.

### Feedback por control
| Control | Visual | Sonido | Háptico | Canal no-color |
|---|---|---|---|---|
| Headlights | haz en el parabrisas, piloto en panel | clic grave | pulso corto | posición del interruptor + texto ON/OFF |
| Wipers | movimiento de escobillas | motor + roce | ninguno | movimiento |
| Window | cristal baja/sube | motor eléctrico | ninguno | movimiento |
| SignalJammer | piloto ámbar + pantallita con texto "JAM" | zumbido bajo continuo | pulso largo | texto + sonido continuo |
| DiffLock | palanca/indicador de posición | clac mecánico | golpe medio | posición física + texto |
| Ignition | tablero se enciende, motor | arranque/apagado | vibración de arranque | sonido + vibración |

Todo estado ON/OFF se distingue por posición física del interruptor y texto, no solo por luz.

## 3. Flujo de retén (`ACheckpointController`)

| Estado | Qué ve y oye el jugador | Qué puede hacer |
|---|---|---|
| Idle | Carretera normal | Conducir |
| Approaching | Luces del retén a lo lejos, cono de `InspectionLight`, barrera cerrada. Radio baja el volumen. Franjas reflectantes y un cartel "RETÉN" (texto + forma) | Decidir: frenar, apagar luces, activar Jammer |
| Inspecting | Furgoneta detenida en `InspectionZone`, el foco ilumina la cabina | Ventanilla y manifiesto |
| Cleared | Barrera sube, foco se apaga, sonido de liberación | Reanudar la marcha |
| PursuitTriggered | Sirenas, luces rojo/azul alternadas a baja frecuencia | Huir |

### Pasos del jugador
1. **Detenerse.** Frenar con `IA_Brake` dentro de `InspectionZone`. Marcas en el suelo (líneas de parada) indican dónde. Si el jugador se detiene fuera, el guardia hace un gesto con la linterna para que avance (no hay castigo inmediato).
2. **Ventanilla.** Con la furgoneta detenida (velocidad < 3 km/h) el guardia llega a la puerta. El jugador mira el botón `Dash.Window` y presiona `IA_Interact`, lo que llama `OpenWindowForInspection(true)`. Si no la abre en ~8 s, el guardia golpea el cristal (sonido + vibración); a los ~15 s sube la sospecha.
3. **Presentar manifiesto.** Con la ventanilla abierta, el manifiesto aparece como objeto físico en el asiento del copiloto o en la visera. Mirarlo lo resalta; `IA_Interact` lo entrega: se llama `PresentManifest(UManifestDataAsset*)`. La mano del conductor lo extiende por la ventanilla (animación de ~1,2 s, no saltable para que sea legible).
4. **Espera.** `Inspecting` dura lo que defina diseño. El jugador no puede mover la furgoneta sin que cuente como huida. Se oye el crujido de la carga y el guardia caminando alrededor.
5. **Veredicto** (`FOnInspectionVerdict`):
   - `CleanPass`: guardia asiente, devuelve el manifiesto, barrera sube. Estado `Cleared`.
   - `SecondaryInspection`: guardia indica con la linterna un carril lateral; el jugador debe seguirlo y se repite desde el paso 3 con más sospecha.
   - `PursuitAlert`: sirena, el guardia retrocede; estado `PursuitTriggered` y `StartPursuit`.
6. **Cerrar ventanilla** tras `Cleared` con el mismo botón (`OpenWindowForInspection(false)`).

### Casos límite
- Jugador abre fuego al acelerar contra la barrera: se trata como huida, `PursuitAlert`.
- Soltar el manifiesto sin ventanilla abierta: la acción no está disponible; el manifiesto no se resalta y su etiqueta indica "ABRIR VENTANILLA" (texto).
- Pausa durante `Inspecting`: los temporizadores del guardia se detienen.
- Cargo en `Cryogenic`/`Volatile` con `bRequiresShielding`: no cambia el flujo, solo la sospecha (ver sección 4).

## 4. Feedback de sospecha sin HUD

La sospecha (`GetSuspicion01()`) se comunica con el comportamiento del guardia y el entorno, nunca con una barra. Insumos: `FSuspicionSnapshot` (velocidad antes de parar, luces, daño, firma sin blindaje, Jammer, integridad de carga).

| Nivel | Guardia (animación) | Sonido | Cabina/mundo |
|---|---|---|---|
| Bajo (0–0.33) | Postura relajada, revisa el papel rápido | Murmullo, pasos tranquilos | Foco estable |
| Medio (0.33–0.66) | Mira la carrocería, se asoma, toma notas | Pausas largas, bolígrafo, respiración, la radio del guardia crepita | Foco apunta a la carga; el barrido se hace más lento |
| Alto (0.66–1) | Mano a la funda o radio, se aleja un paso | Radio con voz, sirena en espera; el latido del conductor sube (sonido, no pantalla) | Sombras más largas; vibración sutil del mando |

- Pistas de causa: la linterna del guardia se detiene en lo que lo alarma (carrocería dañada, faros apagados, Jammer encendido con su zumbido). El jugador aprende qué lo delata.
- Contador diegético opcional (Accesibilidad): puede activarse en Opciones un indicador sutil en el panel (aguja del tablero con texto "ALERTA"), apagado por defecto.
- Subtítulos para todos los sonidos clave: `[radio del guardia crepita]`, `[sirena]`, `[golpe en el cristal]`.
- La transición al nivel alto nunca debe ser repentina: mínimo ~2 s de señal previa antes de `PursuitAlert`, salvo huida.

## 5. Onboarding

Principio: divulgación progresiva en 4 momentos, sin ventanas de tutorial. Todo opcional, desactivable en Opciones.

1. **Arranque (Ignition).** La cabina está oscura. Un resaltado pulsante lento (no parpadeo) en `Dash.Ignition` más una nota física en el volante: "ENCENDER". El jugador aprende la mecánica mirar → resaltar → acción con el primer botón.
2. **Faros y limpiaparabrisas.** Primer tramo bajo lluvia/oscuridad: la visibilidad baja a propósito; el jugador descubre `Headlights` y `Wipers`. Pista: etiqueta del panel brilla más tras 10 s de inactividad.
3. **Ventanilla.** En un retén de práctica (vacío, sin penalización): el guardia espera; si pasan 8 s, la etiqueta de `Dash.Window` se resalta. Se repite el flujo de la sección 3 con sospecha fija en baja.
4. **Sistemas avanzados (Jammer, DiffLock).** Se presentan al encontrar su primer uso real (zona con `bRequires4WD`, patrulla). Una única nota física en el panel la primera vez; después desaparece.

Reglas:
- Cada pista se retira por sí sola tras usarse una vez con éxito.
- Un acierto se refuerza con sonido; un fallo se explica con el mundo (la furgoneta se atasca → se resalta DiffLock), nunca con un cartel de "error".
- Si el jugador ya usa un control, esa pista nunca se muestra.
- Ajustes de dificultad de sospecha ("margen del guardia") separados del tutorial.

## 6. Accesibilidad (checklist)
- [x] Teclado solo / gamepad solo: la rueda de controles y el resaltado por mira cubren ambos.
- [x] Sin depender del color: texto, forma, posición, sonido y movimiento (secciones 2 y 4).
- [x] Sin parpadeo > 3 Hz; luces rojo/azul del retén con alternancia lenta y opción de reducirlas.
- [x] Subtítulos para diálogo del guardia y sonidos clave.
- [ ] Tamaño mínimo de texto de las etiquetas del panel: por definir con art-director (propuesta: legible a 1080p a 60 cm de cabina virtual, con opción de escala 100–150%).
- [ ] Remapeo completo de las 8 acciones de `IA_*`: confirmar con ui-programmer.
- [ ] Niveles de accesibilidad del proyecto: pendiente de `/ux-design accessibility` y accessibility-specialist.

## 7. Decisiones abiertas
1. ¿La rueda de controles entra en el alcance de la demo?
2. ¿Indicador opcional de sospecha en el panel: sí o no?
3. ¿Tiempo máximo antes de que el guardia pida abrir la ventanilla (8 s propuesto)?
4. Estilo visual del resaltado (contorno vs emisivo): decide art-director.
5. Animación de entrega del manifiesto: rig de brazos en primera persona.
