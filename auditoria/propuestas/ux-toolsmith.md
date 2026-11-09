# Propuestas @ux-toolsmith: UI/UX, audio 3D y herramientas

**Reunión con el CTO, 2026-11-09.** Parto de algo ya validado: con pasos, enemigos y sustos por encima de la lluvia, el juego da más miedo. Lo que sigue lleva esa idea al resto del juego.
**Sprints de 1 semana:** S1 cierra el 13-11 (demo corta). S2 va del 16 al 20-11, S3 del 23 al 27-11, S4 del 30-11 al 04-12 y S5 del 07 al 10-12 (v0.5.0).
**Marcadas con ⭐: mis 3 TOP.**

---

### 1. ⭐ Mapa sonoro: atenuación, reverb por zona y oclusión
- **Qué siente el jugador:** sabe de dónde viene cada ruido. El pasillo del hospital suena estrecho y con eco de azulejo, y el parque suena abierto y húmedo. Si el payaso está detrás de una pared, se oye apagado, y cuando dobla la esquina se oye claro. Con audífonos se puede activar el binaural (HRTF) para distinguir arriba, abajo y atrás.
- **Impacto:** muy alto. Hoy hay 0 de 20 SoundWaves con atenuación y el susto del pasillo es 2D. Sin espacialización no hay sigilo ni tensión.
- **Esfuerzo:** M.
  - 3 assets de atenuación (Ambiente, SFX y Susto).
  - Script `asignar_atenuacion.py`.
  - 3 o 4 AudioVolumes con Reverb Effect (Pabellón, Pasillo, Parque abierto, interiores Casa/Bodega).
  - Oclusión nativa por trazado (`bEnableOcclusion`) solo en enemigos, para cuidar la CPU.
  - Spatialization con plugin HRTF como opción en el menú.
- **Assets reutilizables:** cues con atenuación de SoulCity (`MetalPanel_*`, `Rain_Heavy_Ext_Cue`), `FootstepsAttenuation` de FPMovement, `Att_FlashLight` y el reverb procedural de `sonidos_terror.py` para pre-renderizar colas.
- **Sprint:** S2. El cambio del susto del pasillo a `PlaySoundAtLocation` puede entrar en S1 si queda un hueco.

### 2. ⭐ Director de amenaza: música dinámica y silencios estratégicos
- **Qué siente el jugador:** el juego "respira" en 4 estados:
  - **Calma:** ambiente casi mudo.
  - **Sospecha:** aparece un drone grave y baja la lluvia.
  - **Caza:** suena `Musica_Persecucion` y suben el latido y los pasos del enemigo.
  - **Post-susto:** 3 segundos de silencio casi total, con zumbido de oído.
  Antes de un susto grande, todo se apaga entre 1,5 y 2 segundos. Ese silencio anuncia el susto mejor que cualquier música.
- **Impacto:** muy alto. Aplica a todo el juego lo que ya validó el CTO (priorizar la amenaza).
- **Esfuerzo:** M.
  - Un `BP_DirectorAudio` (actor o componente, fuera del BP monolítico) lee la distancia y el estado FSM de los enemigos.
  - Ese director empuja SoundMixes: `SCM_Sospecha`, `SCM_Caza` y `SCM_Silencio`.
  - Requiere primero la jerarquía de SoundClass (Ambiente, SFX, Música, Voz, UI).
- **Assets reutilizables:** `Musica_Persecucion`, el latido y el aliento existentes, `SFX_T_Respiracion` y los drones generados con `sonidos_terror.py`.
- **Sprint:** S2 (SoundClasses y estados) y S3 (silencios en los sustos del Carrusel, la Casa y el apagón).

### 3. ⭐ Voces chilenas y sonidos de 1998: radio AM, cassette y VHS
- **Qué siente el jugador:**
  - Una **radio AM** a pilas en la sala del Pabellón da un boletín de una emisora local sobre el incendio de Mundo Mágico, con estática y cambios de dial.
  - **Cassette** en un walkman: Isidora graba notas de voz.
  - Las **4 cintas VHS** tienen audio con *wow & flutter*, pérdida de agudos y voces de Alfredo, Rafaela y Luisa con modismos chilenos reales.
  - El fonógrafo "LAS OCHO" suena a acetato rayado.
- **Impacto:** alto. Es la identidad del juego. Ningún survival horror suena a Chile de 1998, y eso es lo que lo vuelve "el mejor de Chile".
- **Esfuerzo:** M.
  - **Fase 1 (TTS es-CL como placeholder):** guion de unas 25 líneas y efecto de época con `sonidos_terror.py` (paso de banda AM, wow, hiss).
  - **Fase 2:** actores locales. Basta un estudio casero o un actor de doblaje de Santiago por media jornada.
- **Assets reutilizables:** el pipeline numpy (`sonidos_terror.py` y `importar_audio.py`), las 62 notas como fuente de guion, las 4 VHS y props de radio y TV de Horror_Hospital o DetectiveOffice.
- **Sprint:** S3 (TTS y efectos) y S5 o post-v0.5 (actores).

### 4. HUD diegético: linterna, mapa de papel y notas
- **Qué siente el jugador:** casi no hay HUD.
  - La **batería** se lee en la propia linterna: un LED en el mango (verde, ámbar o rojo), un haz que amarillea y parpadea, y un clic seco al cambiar las pilas.
  - El **mapa** es un folleto turístico de Mundo Mágico de 1998 que Isidora despliega en sus manos. Ella misma marca con lápiz lo que ya visitó, y abrirlo no pausa el juego.
  - Las **notas** se muestran en papel arrugado, con una transcripción legible opcional en Barlow.
- **Impacto:** alto en inmersión, y libera pantalla en la iGPU.
- **Esfuerzo:** M (linterna y notas) a L (mapa en mano con animación).
- **Assets reutilizables:** el sistema actual de batería y pilas, `WBP_Pagina`, las fuentes SpecialElite e IMFe (solo para títulos), los papeles y props de DetectiveOffice y Horror_Hospital, y la lista de atracciones del HTML del parque para dibujar el folleto.
- **Sprint:** S3 (LED de batería y notas en papel) y S4 (mapa folleto).

### 5. Menús VHS 1998
- **Qué siente el jugador:**
  - El menú principal es la pantalla azul de un VCR: "► PLAY" para continuar, "REC" para nueva partida, "EJECT" para salir y fecha en pantalla "NOV 09 1998 03:14".
  - Al confirmar hay tracking, ruido de cabezal y el clac mecánico de la cinta.
  - La pausa es "❚❚ PAUSE" con la imagen congelada temblando.
  - Cargar partida se ve como rebobinar la cinta.
- **Impacto:** medio-alto. Es la primera impresión en itch.io y en el tráiler.
- **Esfuerzo:** S-M. Restyle de `WBP_Menu` más un material UI barato de scanlines y ruido (sin post-proceso pesado, apto para la iGPU). Los íconos ► ❚❚ deben ir como Image, no como glifo, por el bug de BarlowCondensed.
- **Assets reutilizables:** `WBP_Menu` actual (ScaleBox ya listo), SFX de las VHS de la propuesta 3 y una fuente monoespaciada tipo OSD (libre, Inline).
- **Sprint:** S2.

### 6. Accesibilidad: subtítulos, tamaño de texto y daltonismo
- **Qué recibe el jugador:**
  - Subtítulos para voces, radio y VHS.
  - *Closed captions* con dirección para sustos, por ejemplo "[chillido, izquierda]" y "[pasos pesados, detrás]".
  - Un indicador visual de ruido para personas con hipoacusia.
  - Tamaño de texto en 3 niveles.
  - Modo daltónico (deuteranopia, protanopia y tritanopia) con `SetColorVisionDeficiencyType` nativo.
  - Glifos de mando arreglados y SafeZone.
- **Impacto:** alto en reseñas y en alcance. Además, los captions reutilizan los datos del director de amenaza.
- **Esfuerzo:** S-M. Una tabla de datos `DT_Subtitulos` (ID, texto, duración, dirección), un widget de captions y opciones nuevas en el Opciones.sav ya versionado.
- **Assets reutilizables:** el `WBP_HUD` con outline y sombra, Barlow, los iconos de FreeSurvivalIconsKG y los sliders de pausa existentes.
- **Sprint:** S3 (subtítulos, tamaño y daltonismo). El arreglo de glifos va en S1 o S2.

### 7. Mando PS4: barra de luz y vibración como sistema nervioso
- **Qué siente el jugador:** el mando "tiene miedo" con Isidora.
  - **Barra de luz:** blanca tenue con la linterna encendida, ámbar parpadeando con la batería baja, roja latiendo al ritmo del corazón en persecución y magenta en el glitch.
  - **Vibración:** latido doble en las manos, pasos del payaso que aumentan con la cercanía, un tirón al activar cada interruptor y el zumbido del generador.
- **Impacto:** medio-alto para quien juega con mando. Es material muy vistoso en el tráiler.
- **Esfuerzo:** M.
  - La vibración se resuelve con Force Feedback Effects nativos.
  - En Windows la barra de luz necesita una vía extra (Input Device Properties o plugin, o Steam Input), así que hay que validarla en el hardware del 17-11.
  - Todo debe tener un interruptor en Opciones.
- **Assets reutilizables:** el mapeo PS4 completo ya existente, el latido y aliento y los estados del director de amenaza (propuesta 2).
- **Sprint:** S4.

### 8. Herramienta: captura de pantallas y tráiler en un clic
- **Qué recibe el CTO:** `Capturar_Trailer.bat` abre el build Shipping con una bandera `-ModoCaptura`, que hace lo siguiente:
  - Oculta el HUD.
  - Recorre de 8 a 10 cámaras marcadas en el mapa (tag `CAM_Trailer`).
  - Toma `HighResShot 2` en cada una.
  - Graba tomas de 10 segundos.
  Después, un script ffmpeg las une con un LUT VHS y las deja listas para itch, Steam o redes. Además hay un modo foto con tecla de desarrollo (cámara libre y pausa).
- **Impacto:** alto en tiempo, porque reemplaza horas de capturas manuales con una iGPU que no corre PIE. Es necesario para la página de itch del 13-11 y para el anuncio de la v0.5.
- **Esfuerzo:** S-M.
- **Assets reutilizables:** los lanzadores existentes (`Abrir_MundoMagico_Liviano.ps1` con `-dpcvars`), la cinemática en órbita y `config_rutas.py` (centralizar las rutas).
- **Sprint:** S2. La versión mínima (solo screenshots) podría estar en S1.

### 9. Herramienta: panel de playtest
- **Qué recibe el CTO:** cada build Shipping escribe `Saved/Playtest/*.csv` con la siguiente información:
  - El tiempo en cada uno de los 8 hitos de la demo.
  - Muertes y la causa.
  - La batería al llegar a cada hito.
  - Los sustos disparados.
  - El FPS mínimo por zona.
  - La posición cada 5 segundos.
  Un script de Python genera un HTML con un mapa de calor sobre la vista aérea del parque, un embudo de abandono y un "¿dónde se perdió el jugador?". También hay un overlay con F10 para notas del tester.
- **Impacto:** alto. Convierte cada partida de un tester (y los feedbacks de itch) en datos sin necesidad de mirar el video entero. Además detecta zonas pesadas para la iGPU.
- **Esfuerzo:** M. Un componente `BP_Telemetria` liviano (fuera del BP monolítico) y el script `reporte_playtest.py`.
- **Assets reutilizables:** el sistema de guardado y continuar, los hitos de la demo, el tooling Python del repo y el render aéreo del parque del HTML.
- **Sprint:** S2 (CSV) y S3 (reporte HTML).

---

## Orden sugerido
**S2:** 1, 2a, 5, 8 y 9a. **S3:** 2b, 3a, 4a, 6 y 9b. **S4:** 4b y 7. **S5:** pulido y actores (3b).
**Dependencia clave:** la jerarquía de SoundClass y SoundMix (auditoría, recomendación 4) es la base de 1, 2, 3 y 6, y debe ir primero en S2.
