# 19 — Accesibilidad (BLACKLINE: Night Haul)

Base: `CONTRATO.md`, `13_ux_cabina.md`. Objetivo: WCAG 2.1 AA adaptado a juegos. Estado: borrador de requisitos, sin build auditada (todo "NO EVALUADO" hasta tener build). Gate: BLOQUEANTE = falla el objetivo o deja información crítica en un solo canal; ASESOR = mejora.

## 1. Remapeo
Acciones (`ABlacklineVanPawn`): `IA_Throttle, IA_Steer, IA_Brake, IA_Handbrake, IA_Look, IA_ToggleCamera, IA_Interact, IA_DriveMode`.
- **BLOQUEANTE:** las 8 acciones remapeables en teclado, ratón y gamepad (Enhanced Input, `UInputMappingContext` por usuario, persistido en config de jugador). Verificar API de remapeo en `docs/engine-reference/unreal/modules/input.md` (`// VERIFY-5.8`).
- Conflictos: avisar al asignar una tecla repetida y ofrecer intercambiar; nunca asignar en silencio.
- Reset a valores por defecto por acción y global.
- **BLOQUEANTE:** ninguna acción exige pulsaciones simultáneas ni mantener. Todo "mantener" (p. ej. rueda de controles con `IA_Interact`) tiene alternativa de alternar (toggle) y duración ajustable (0,2–1,5 s).
- Ejes: zona muerta, curva y sensibilidad de `IA_Steer`/`IA_Look` ajustables; opción de invertir mirada. Acelerador/freno como eje o botón.
- Conducción asistida opcional: acelerador automático (velocidad objetivo), asistencia de dirección y freno de emergencia. Manos: esquema a una mano (preset) y soporte de mando adaptativo (Xbox Adaptive Controller) mediante el remapeo.
- Glifos de entrada se actualizan según el dispositivo activo (Xbox, PlayStation, Switch, teclado).

## 2. Cámara cockpit: motion sickness
La cabina en primera persona con furgoneta de suspensión Chaos es de riesgo alto. Todo ajuste en `DA_`/config de jugador, sin Tick nuevo.

| Ajuste | Rango / default | Notas |
|---|---|---|
| FOV cockpit | 70–110°, default 90° | Aplica a `CockpitCamera`; `ChaseCamera` igual. Ajuste en vivo con vista previa |
| Sacudida de cámara (bamboleo/baches) | 0–100%, default 60% | Escala el movimiento de cabeza por suspensión, impactos y `ApplyImpact` |
| Sacudida por golpes/choques | 0–100%, default 60% | Independiente del bamboleo; con 0% el impacto se comunica por sonido y háptico |
| Balanceo (roll) de cámara en curvas | 0–100%, default 40% | `GetLateralG()` no debe rotar la cámara con 0% |
| Desenfoque de movimiento y viñeta | Off/On; viñeta dinámica por velocidad opcional | Default: desenfoque off |
| Cabeceo al frenar/acelerar | 0–100% | Parte del bamboleo; ajuste propio |
| Punto de referencia fijo | Off/On | Mira discreta o tablero/volante siempre visibles; cabina siempre estable (el cuerpo del vehículo es el ancla) |
| Vista tercera persona | Alternar por `IA_ToggleCamera` | Alternativa recomendada contra el mareo; recordar última elección |
| Suavizado de giro de `IA_Look` | Off/Bajo/Alto | Sin aceleración angular brusca; sin "snap" de cámara |
| Reducir efectos de lluvia/pantalla | Off/On | Sin destellos de pantalla completa |

Reglas de diseño:
- Sin movimiento de cámara forzado fuera de la acción del jugador, excepto impactos (atenuables a 0%).
- Cambios de cámara (`ToggleCamera`) con fundido corto, no corte brusco ni giro.
- **BLOQUEANTE:** ningún efecto de cámara es obligatorio; todos con opción de reducir/apagar (SC 2.3.3 es AAA, fuera del objetivo, pero se adopta por seguridad del jugador).
- Destellos: ninguno > 3 Hz ni de área grande (SC 2.3.1 Three Flashes or Below Threshold). Luces rojo/azul del retén a baja frecuencia (< 2 Hz) con opción de "reducir": luz fija alternada con baja intensidad.

## 3. Subtítulos y subtítulos descriptivos
- **BLOQUEANTE:** todo diálogo del guardia, radio y voz de narrativa con subtítulo (SC 1.2.2 Captions (Prerecorded)). Activados por defecto.
- Identificación de hablante: "GUARDIA:", "RADIO:", "DESPACHO:" (texto, no solo color).
- Sonidos clave con descripción entre corchetes (SC 1.2.1 Audio-only y 1.2.2): `[radio del guardia crepita]`, `[sirena]`, `[golpe en el cristal]`, `[crujido de la carga]`, `[zumbido del Jammer]`, `[motor se apaga]`, `[latido acelerado]`. Opción "Descripciones de sonido" On/Off.
- Tamaños: Pequeño / Medio / Grande / Muy grande (mín. equivalente a 18 px a 1080p en Medio; hasta 200%). Localización: texto largo en otros idiomas sin cortes (a validar con localization-lead).
- Fondo: ninguno / semitransparente / opaco, con opacidad ajustable. Contraste texto/fondo mínimo 4,5:1 (SC 1.4.3 Contrast (Minimum)).
- Máximo 2 líneas, ~40 caracteres por línea; permanencia suficiente (≥ 1 s por 15 caracteres, mín. 1,5 s).
- Posición: parte inferior, sobre el parabrisas, sin tapar el tablero ni el guardia; opción de subir/bajar.
- Sonidos direccionales (sirena, patrulla en `Pursuit`): indicador visual direccional opcional (flecha/texto "IZQ/DER/ATRÁS", no solo color). **BLOQUEANTE** si la dirección es información de juego sin otro canal.
- Audio: sliders Master, Música, SFX, Diálogo, UI; opción mono; limitador/normalizador de sonidos fuertes (sirenas, impactos).

## 4. Modo daltónico para indicadores
Principio de `13_ux_cabina.md`: nunca solo color. Esta sección lo concreta.

Indicadores afectados y canal no-color existente / requerido:
| Indicador | Color actual | Canal no-color | Estado |
|---|---|---|---|
| Piloto Headlights | verde/ámbar | posición del interruptor + texto ON/OFF | cubierto en diseño |
| Piloto SignalJammer | ámbar | texto "JAM" + zumbido | cubierto en diseño |
| Aguja de alerta (opcional) | rojo/verde | texto "ALERTA" + posición en escala con marcas | verificar: añadir marcas numéricas |
| Luces del retén | rojo/azul | patrón + sirena + cartel "RETÉN" | cubierto |
| Resaltado de botones por mira | contorno/emisivo | forma (contorno grueso) + etiqueta nítida | cubierto; evitar solo cambio de tono |
| Estados del retén y persecución (indicador de panel/radio) | por definir | texto + sonido | pendiente de diseño |

Modos (filtro por post-proceso o paleta alternativa; los indicadores de cabina usan paleta alternativa, no filtro global):
- Protanopia, Deuteranopia, Tritanopia + Desactivado.
- Intensidad 0–100%.
- Paletas probadas con simuladores; evitar pares rojo/verde; preferir azul/naranja y diferencia de luminosidad.
- Alto contraste de etiquetas del panel (opción): texto claro sobre fondo oscuro, 4,5:1 mínimo, 3:1 para iconos y contornos (SC 1.4.11 Non-text Contrast).
- Coordinar con art-director: el modo no debe romper la dirección visual nocturna; lo decide art-director si hay conflicto.
- **BLOQUEANTE:** cualquier indicador nuevo debe declarar su canal secundario (forma, texto, sonido o posición) antes de integrarse (SC 1.4.1 Use of Color).

## 5. Alternativas a la interacción por LineTrace
Hoy: traza por `ECC_GameTraceChannel1` a 20 Hz desde `CockpitCamera`, alcance ~120 cm, y `IA_Interact` (según `13_ux_cabina.md`). Riesgos: exige apuntar con precisión, mirar y pulsar a la vez, y falla con temblor o poca movilidad.

Alternativas (todas activas a la vez; el jugador elige):
1. **Rueda de controles** (6 sectores = `EDashControl`): elección con palanca/flechas/teclas numéricas, sin apuntar. Sube a **requisito** (hoy "decisión abierta" 1, `13_ux_cabina.md`): **BLOQUEANTE** para cumplir SC 2.1.1 Keyboard, porque sin ella la cabina exige apuntar con ratón o palanca.
2. **Atajos directos:** una acción remapeable por control (`Dash.*`), con alternancia (toggle), sin apuntar. Vía `ActivateDashControl(EDashControl)`, que ya es API pública.
3. **Ciclo de foco:** `Siguiente/Anterior control` recorre botones con resaltado, luego `IA_Interact` activa. Orden fijo y coherente con el diseño del panel.
4. **Asistencia de apuntado:** radio de colisión de la traza ajustable (100–200% de la malla visible), magnetismo al más cercano, tiempo de gracia de resaltado 150–600 ms, alcance 120–200 cm.
5. **Activación por permanencia (dwell):** mirar el botón N s lo activa, con tiempo ajustable (0,5–3 s) y barra circular + sonido; desactivado por defecto.
6. **Retén asistido:** modo "retén automático": abrir ventanilla y entregar manifiesto con una sola acción (`IA_Interact` o atajo), manteniendo la animación de ~1,2 s. Opcional.
7. **Comandos de voz:** fuera de alcance de la demo (ASESOR).
- Todas las alternativas llaman a la misma ruta de código (`ActivateDashControl`), sin duplicar lógica. Las traza sin Tick se mantienen.
- La etiqueta diegética del panel muestra el glifo/atajo de la alternativa activa.
- Mientras se usa la rueda, el vehículo sigue en marcha; opción "ralentizar el tiempo al abrir la rueda" (0–100%), compatible con la regla de pausa del retén.

## 6. Ajustes a exponer (menú Accesibilidad)
Todos con vista previa, persistidos y accesibles desde la pantalla de inicio y la pausa. Pausa disponible siempre (un jugador).

**Entrada:** remapeo completo; mantener→alternar; duración de mantener; zona muerta/curva/sensibilidad; invertir mirada; acelerador automático; dirección asistida; frenado de emergencia; esquema a una mano; rueda de controles; atajos por control; ciclo de foco; radio de traza/magnetismo; tiempo de gracia; permanencia (dwell); retén asistido; vibración (intensidad 0–100%).
**Cámara/visual:** FOV; sacudida; golpes; balanceo; desenfoque; viñeta; punto de referencia; vista por defecto (cockpit/tercera persona); reducir luces del retén; reducir efectos de lluvia/pantalla; brillo/gamma; alto contraste; modo daltónico + intensidad; escala de texto de etiquetas (100–200%); resaltado reforzado.
**Audio y subtítulos:** subtítulos On/Off; tamaño (4); fondo y opacidad; hablante; descripciones de sonido; indicador direccional; Master/Música/SFX/Diálogo/UI; mono; limitador de volumen.
**Cognitivo/juego:** pistas de onboarding On/Off y repetir tutorial; indicador opcional de sospecha en panel (decisión abierta 2); margen del guardia (dificultad, separado del tutorial); tiempo máximo antes de pedir ventanilla ajustable (8 s por defecto); velocidad de juego opcional en persecuciones; recordatorio de objetivo accesible desde pausa y radio.

Defaults: subtítulos On, descripciones On, FOV 90°, sacudida 60%, desenfoque Off, daltónico Off. Presets: "Estándar", "Reducir movimiento", "Baja visión", "Una mano".

## 7. Tabla de auditoría (sin build: todo NO EVALUADO salvo diseño)
| Hallazgo | Criterio WCAG | Gravedad | Gate | Recomendación |
|---|---|---|---|---|
| Sin rueda ni atajos por control: cabina exige apuntar | SC 2.1.1 Keyboard | S1 Crítica | BLOQUEANTE | Rueda + atajos (sección 5) en el alcance de la demo |
| Remapeo de 8 `IA_*` sin confirmar | SC 2.1.1 Keyboard | S2 Alta | BLOQUEANTE | Confirmar con ui-programmer; probar en build |
| "Mantener" de la rueda sin alternancia | SC 2.5.1 Pointer Gestures / 2.1.1 | S2 Alta | BLOQUEANTE | Toggle + duración ajustable |
| Dirección de sirena/patrulla solo por sonido | SC 1.1.1 Non-text Content / 1.2.1 | S2 Alta | BLOQUEANTE | Indicador direccional + subtítulo |
| Estados retén/persecución en panel sin canal no-color definido | SC 1.4.1 Use of Color | S2 Alta | BLOQUEANTE | Texto + sonido por estado |
| Tamaño y contraste de etiquetas del panel | SC 1.4.3 / 1.4.11 | S3 Media | NO EVALUADO — sin build ni definición de art-director | Medir a 1080p; opción 100–150% mínimo |
| Subtítulos y tamaños | SC 1.2.2 | S3 Media | NO EVALUADO — sin build | Probar 4 tamaños y fondos |
| Destellos del retén | SC 2.3.1 | S2 Alta | NO EVALUADO — sin build; diseño < 3 Hz | Medir frecuencia y área en build |
| Efectos de movimiento de cámara reducibles | SC 2.3.3 Animation from Interactions (AAA, fuera del objetivo AA) | S3 Media | ASESOR | Adoptar de todos modos (sección 2) |

## 8. Pendientes y decisiones del CTO
1. ¿Rueda de controles y atajos entran en la demo? Se recomienda que sí (bloqueante).
2. Alcance de conducción asistida (acelerador automático, dirección).
3. Escala de texto mínima de etiquetas (con art-director).
4. Plan de pruebas para qa-tester una vez haya build.
5. Requisitos de implementación para ux-designer y ui-programmer.
