# Reunión 25-11 — @ux-toolsmith (UX, UI, audio, herramientas)

Solo Blackline. Base: `24_historia_secuestro.md`, `13_ux_cabina.md`, `D2_metasounds.md`, `INVENTARIO_FAB.md`.

## 1. Cómo se SIENTE Pancito (sin mostrarlo herido)
Regla: Pancito se siente por **ausencia, sonido y texto**. Nunca un icono de perro herido, nunca sangre ni gemido de dolor.
- **Prueba de vida por radio:** al cerrar un arco la radio se corta, entra estática y una voz de La Cofradía habla. De fondo, respiración y un ladrido corto de quiltro. Va con subtítulo `[ladrido de Pancito]` y pista de lugar. Pantalla propia mínima (fondo negro, texto, 6 a 8 s, saltable con la tecla de interactuar).
- **Asiento vacío:** el copiloto se ve vacío y bien iluminado, con la pelota bajo el asiento. Cuando el Bienestar baja de 40%, el asiento queda más en sombra (no es un cartel, es luz). Cuando sube, entra un poco de luz cálida.
- **Collar en el espejo:** se balancea con las curvas (físico simple, sin Tick: timeline o animación en loop). Un tintineo de chapita suave que sube con el bache. Si Bienestar < 25%, el collar se queda quieto. Respuesta a la pregunta 4 del doc 24: **sí**, la cabina lleva asiento vacío y collar.
- **Tablet de La Cofradía:** mensajes cortos, tono cortés y frío del Patrón. Llegan con vibración del mando y un tono de aviso, no con ventana emergente. Sin color rojo como único canal: lleva texto «PLAZO» o «ADVERTENCIA».
- **Plazo que se siente:** entre 50% y 80% del plazo, la radio baja el volumen y el motor sube un poco en el mix. En el último 10% el collar golpea más y hay un latido grave sutil (sonido, sin parpadeo > 3 Hz).

**HUD actual** (panel ENCARGO con barra de Pancito, plazo, minimapa): choca con el principio «sin HUD» de `13_ux_cabina`. Propuesta: mantener, pero marcar como **modo ayuda** (opción), y llevar la versión por defecto a la tablet y al espejo (Pancito como número en la tablet y como collar). Minimapa y panel quedan para la demo.
- La barra de Pancito pasa a **segmentos con texto** («Pancito 80%»), nunca solo color.
- Plazo: texto grande con `mm:ss`, sin parpadeo.

**Controles**
| Acción | Teclado | Mando PS4 |
|---|---|---|
| Mirar / apuntar botón | Ratón | Palanca derecha |
| Interactuar | E | Cuadrado |
| Rueda de controles (mantener) | E mantenido | Cuadrado mantenido |
| Acelerar / frenar | W / S | R2 / L2 |
| Girar | A / D | Palanca izquierda |
| Tablet / mensajes | Tab | Touchpad |
- Remapeo completo de `IA_*` y escala de texto 100 a 150% siguen pendientes (checklist 13, sección 6).

## 2. Packs Fab para Blackline
| Pack | Veredicto | Uso en Blackline |
|---|---|---|
| Free Thunder Sounds (15 MB, 8 assets) | **Sí** | Truenos lejanos y cercanos en tormenta del sur (acto 4); alimenta `RainIntensity` con truenos aleatorios cada 20 a 60 s. |
| First-Person Movement Template (417 MB, 900 assets, ~180 sonidos) | **Quizá, solo sonidos** | Extraer puertas, pasos, golpes y objetos para bencinera, restaurante y cabina (puerta, guantera). No importar el personaje ni el template. Revisar licencia y duplicados antes. |
| FREE15 Utility Icons (FREE15Su, 1 MB) | **Sí** | Iconos de la rueda de controles y del panel (faros, limpiaparabrisas, ventanilla). Texto igual al lado. |
| RadialWheel | **Sí, si está** | No aparece en el inventario con ese nombre; confirmar. Para la rueda de 6 sectores (`EDashControl`). Si no, UMG propio sin Tick. |
| Flashlight (FLashLig V2 y V4, 384 MB c/u) | **Quizá** | Linterna del guardia en el retén (el foco que se detiene en lo que lo alarma). Usar solo V4 (duplicado). Descartar el personaje en tercera persona. |
| ChatAvatar Plugin | **No** | Chat con IA externa: riesgo de red, privacidad y tono impredecible. La tablet usa textos escritos. |
| FREE Horror (papeles) | **Quizá** | Texturas de papel para manifiesto y notas en la cabina. |
| Resto (Horror Hospital, Fog Area, Whisper, etc.) | **No** (o fuera de alcance) | Niebla de Fog Area puede servir en la cordillera: evaluar aparte. |

## 3. Toque chileno en voz y radio
- **Radio de la cabina** con dial: 3 emisoras ficticias.
  - AM «Radio Ruta Norte»: locutor de tarde, avisos de tráfico, saludos a camioneros.
  - FM «FM Cordillera»: música y avisos de clima.
  - Cumbia/cueca: cueca brava, cumbia chilena, música de camionero (originales o libres de derechos; no usar temas con copyright).
- Locutores: calmos, cercanos, con «oiga», «compadre», «bacán», «al tiro», «cachai» **con medida** (1 expresión por mensaje, sin caricatura). La banda habla formal y cortés (usted), lo que da más miedo.
- Subtítulos en **español de Chile**: «bencinera», «fonda», «carabineros ficticios» (nombre propio inventado, no la institución real), «viáticos». Tuteo del Patrón evitado; usa «usted».
- Sirenas: tono local de patrulla, con voz de radio policial ficticia («Móvil 14, conteste»).
- Mezcla: la radio baja en retén (ya en `13`), sube con el plazo corriendo.

**Audio que falta** (de `D2`, los Wave Assets no existen aún)
- Motor diésel: loops idle/low/high, start y stop, escape.
- Lluvia en techo metálico: cama, gotas duras, ventanilla abierta; escobilla.
- Quiltro: ladridos cortos, respiración, jadeo lejano, chapita del collar.
- Sirenas y radio policial ficticias; golpe en el cristal; radio del guardia.
- Locutores AM/FM, jingles ficticios, 4 a 6 pistas de música chilena libre.
- Voz del Patrón y de los 5 lugartenientes (texto + voz).

## 4. Cinco entregables priorizados
1. **Prueba de vida** con audio: pantalla mínima + radio + subtítulo (desbloquea el pendiente del doc 24).
2. **Collar y asiento vacío** en cabina, con luz y balanceo, y la opción de HUD ayuda.
3. **Wave Assets de motor y lluvia** (D2) para poder enchufar los MetaSounds ya diseñados.
4. **Radio con 3 emisoras** y locutores chilenos (guion de 20 frases + música libre).
5. **Rueda de controles** con iconos del pack gratuito y remapeo teclado/PS4.

Decisión para el CTO: ¿el HUD actual pasa a «modo ayuda» opcional (propuesta) o se queda fijo para la demo?
