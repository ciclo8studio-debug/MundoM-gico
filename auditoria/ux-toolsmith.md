# Auditoría UX / Audio 3D / Herramientas: @ux-toolsmith

**Proyecto:** Mundo Mágico: Turno Nocturno (UE 5.8). **Fecha:** 2026-11-09. **Modo:** solo lectura (`grep -a` sobre .uasset/.umap, logs y Config). No se tocó nada. No se pudo verificar nada jugando (PIE cuelga la GPU), así que todo lo de abajo sale de evidencia estática.

## Resumen
- La UI funciona y es compacta: 4 widgets (`WBP_HUD`, `WBP_Menu`, `WBP_Panel`, `WBP_Pagina`). Casi toda la lógica vive en un único `ThirdPerson/Blueprints/BP_ThirdPersonCharacter.uasset` de **10,1 MB**: HUD, pausa, opciones, candado, galería, mochila, notas, linterna y audio. Es el mayor riesgo de mantenimiento.
- **Audio 3D casi inexistente.** Ninguno de los 20 SoundWave propios (`MundoMagico/Audio/**`) tiene `AttenuationSettings`. El mapa tiene 16 `AmbientSound` y solo **1** `bOverrideAttenuation`. No hay SoundClass, SoundMix, Concurrency ni Submix propios: se usan los del motor (`/Engine/EngineSounds/Master`, `Music`, `Duck_Normal_Attack_MixMod`).
- Las opciones de volumen funcionan por `SetSoundMixClassOverride` sobre Master/Music del motor. Como solo `Musica_Menu` y `Musica_Persecucion` están asignadas a `Music`, el resto cae en Master, y no hay forma de separar SFX, ambiente y voces.
- Legibilidad: 4 glifos (▲ ✕ ↓ ↑) no existen en BarlowCondensed y caen al font *last resort*. En un build empaquetado probablemente se verán como cuadrados.
- Herramientas internas: correctas pero frágiles. 16 de 19 scripts Python tienen rutas absolutas `C:\Users\pansh\...`, y no hay README ni lanzador de cook/package.

## Hallazgos

### 🔴 Críticos
1. **SFX y ambientes sin espacialización.** `grep AttenuationSettings` en los 20 .uasset de `Content/MundoMagico/Audio/**` da 0 resultados (los SoundWave solo traen `SoundClassObject`, `bLooping`). En `Maps/L_Pabellon4.umap` hay `AmbientSound` ×16 contra `bOverrideAttenuation` ×1. Los `SND_MetalTren`, `SND_MetalNoria`, `SND_MetalMontana`, `SND_MetalTorre`, `SND_MetalBodega`, `SND_FeriaCarrusel`, `SND_ChispasTorre` y `SND_VientoParque` apuntan a SoundWaves sin atenuación (`SoulCity/.../Wind_Breeze05`, `SparkLoop01`, `Ambiente_Parque`, etc.), así que probablemente **se oyen 2D en todo el mapa con el mismo volumen**. Solo las cues `MetalPanel_*_Cue` y `Rain_Heavy_Ext_Cue` de SoulCity traen atenuación. En un survival horror esto anula la orientación por sonido.
2. **`BP_SustoPasillo` usa `PlaySound2D`** con `SFX_T_Chillido`, `SFX_T_Carrera`, `SFX_T_Golpe` y `SFX_T_Respiracion`. El susto no tiene posición: el jugador no sabe de dónde viene la amenaza.
3. **Glifos faltantes en la UI.** En `Saved/Logs/*.log`: `Glyph Index 0 with codepoint U+25b2` ×39, `U+2191`/`U+2193`/`U+2715` ×16 cada uno, "getting last resort font data C:\Users\pansh\Desktop\MundoMagico_Project\Fuentes_descarga\BarlowCondensed-SemiBold.ttf". Además, la fuente de origen está **fuera del proyecto**, en el Escritorio. Hay que confirmar que el Font asset tenga los datos embebidos (*Inline*) y no *Lazy load* desde esa ruta.

### 🟠 Importantes
4. **Monolito UI en el personaje.** `BP_ThirdPersonCharacter` (10,15 MB) contiene `CrearHUD`, `AplicarHUD`, `Opciones` ×27, `Candado` ×24, `Codigo` ×15, `Galeria` ×9, `Mochila` ×6, `SetGamePaused`, `SetInputMode` y un `CreateWidget` único. La UI no se puede reutilizar entre personajes, el BP corre riesgo de corrupción y abrirlo es lento (relevante con la RAM actual).
5. **Mezcla sin estructura.** No existen SoundClass/SoundMix propios (solo `SC_WoodenCrate*`, que son cues de un pack). Volumen y VolMusica se aplican vía `SetSoundMixClassOverride` + `PushSoundMixModifier` sobre `/Engine/EngineSounds/Duck_Normal_Attack_MixMod`, o sea, un asset de ducking del motor reutilizado como mix de opciones. No hay ducking real (por ejemplo, bajar el ambiente durante una persecución).
6. **Concurrency.** Solo aparece `ConcurrencySet` (vacío por defecto) en `BP_Enemigo` y `BP_SustoPasillo`. `BP_RuidoAzar` tiene 5 `AudioComponent` sin límite. Con lluvia, truenos y 16 ambientes a la vez el mixer se satura: `MUNDO_MÁGICO_2.log` muestra `Audio Buffer Underrun (starvation)` ×5, con `AudioCallbackBufferFrameSize=1024` y `AudioNumBuffersToEnqueue=1` en `DefaultEngine.ini`.
7. **Linterna inaudible y con assets duplicados.** El BP referencia `FlashLight_System/Sound/Flashlight_On/OFF`, `LightFlicker02` y también `/Game/Horror_Template/Sounds/Sound_Flashlight`, con volumen 0,1 (dato del brief). `Att_FlashLight` existe pero la linterna es del propio jugador: debería ser 2D o tener un radio corto, y oírse con claridad.
8. **Opciones.sav sin versión.** Es un CSV posicional `1.0,1.0,0.9,1.0,1,0` (Brillo, Volumen, Sensibilidad, VolMusica, Calidad, ?) dentro de un GVAS. Si se agrega un campo, las partidas guardadas viejas se leen corridas. Falta un campo de versión y valores por defecto si el parseo falla.
9. **Música y ambientes del mapa.** `Musica_Menu`, `Musica_Persecucion`, `Ambiente_Parque` y `Ambiente_T_Hospital` están colocados como actores en `L_Pabellon4`. Hay que confirmar que la música sea no espacializada y que el ambiente del hospital no suene en el parque (no hay *Audio Volumes* ni reverb por zona detectados).

### 🟢 Correcto / positivo
10. El menú tiene `ScaleBox` y `VerticalBox`, y el BP soporta todas las teclas de mando (`Gamepad_FaceButton_*`, sticks, gatillos, `Special_Left/Right`) además de Enhanced Input (`IA_Move`, `IA_Look`, `IA_MouseLook`).
11. Los textos usan `OutlineSettings`/`ShadowOffset`, lo que da buen contraste sobre escenas oscuras. `AutoWrapText` está activo en HUD y Panel.
12. La pausa ya expone Brillo, Volumen, Sensibilidad, VolMusica y Calidad, que es una base de accesibilidad razonable.
13. El pipeline de audio procedural (`Isidora3D/parque/sonidos_terror.py`, numpy: formantes, reverb, glitch) y el import (`importar_audio.py`) son reproducibles. `Abrir_MundoMagico_Liviano.ps1` aplica cvars con `-dpcvars` sin tocar el ini, lo cual es correcto.
14. Hay 2 paquetes de recursos sin usar: `FreeSurvivalIconsKG` (ninguna referencia fuera de su carpeta) y el plugin RadialWheel (no está en `.uproject` ni en Config). Sirven para la mochila e íconos de prompts.

## Estado de atenuación 3D, SoundClasses y mezcla
| Elemento | Estado |
|---|---|
| Attenuation assets propios | **0** (solo de packs: `Att_FlashLight`, `SA_WoodenCrateAttenuation`, `Door/Item/FootstepsAttenuation` de FPMovement) |
| SoundWaves propios con atenuación | **0 / 20** |
| AmbientSound en mapa con override | **1 / 16** |
| SoundClass propios | **0**. Música → `/Engine/EngineSounds/Music`; resto → Master por defecto |
| SoundMix | `Duck_Normal_Attack_MixMod` del motor, reutilizado para opciones |
| Concurrency | `ConcurrencySet` vacío en 2 BP; sin assets |
| Submix / Reverb / AudioVolume | No detectados |
| Salud del mixer | Underruns ×5 (buffer 1024, 1 buffer encolado) |

## Accesibilidad y legibilidad del HUD
- **Glifos**: reemplazar ▲ ✕ ↓ ↑ por íconos (Image) o agregar una fuente de respaldo (sub-font en el Composite Font con un rango Unicode de flechas y símbolos, por ejemplo Noto Sans Symbols).
- **Fuentes mezcladas**: BarlowCondensed (Regular/SemiBold), SpecialElite e IMFeENrm28P. IMFe (fuente antigua, tipo imprenta del siglo XVII) en `WBP_HUD`/`WBP_Panel` es poco legible en tamaños chicos. Conviene reservarla para títulos y notas y usar Barlow en los prompts del HUD.
- **Escala**: el menú usa `ScaleBox`, pero no hay `SafeZone` en ningún widget y no se detectó config de DPI Curve, lo que es un problema en TV o ultrawide. Falta una opción de tamaño de texto.
- **Faltan**: subtítulos o *closed captions* para sustos (Chillido, Golpe, Carrera), indicador visual de ruido (sordera/hipoacusia), opción de invertir eje Y, remapeo, y un control de volumen separado para SFX/Ambiente/Voz.
- **Prompts**: hoy son texto fijo. Deberían cambiar según el dispositivo activo (teclado o mando) usando los íconos de `FreeSurvivalIconsKG` o un set de mando.

## Recomendaciones Sprint 1 (priorizadas)
1. **[S, 🔴] Crear 3 attenuation assets** (`ATT_Ambiente_Grande` 1500–6000 uu, `ATT_SFX_Medio` 300–2500, `ATT_Susto` 200–1800 con Focus/LPF) y asignarlos a los 16 AmbientSound y a los SoundWave de `Terror/`. Se puede automatizar con un script Python editor-only (`asignar_atenuacion.py`) en `Isidora3D/parque/`.
2. **[S, 🔴] Cambiar el `PlaySound2D` de `BP_SustoPasillo`** por `SpawnSoundAtLocation`/`PlaySoundAtLocation` en la posición del susto.
3. **[S, 🔴] Arreglar los glifos**: sub-font de símbolos en el Composite Font de BarlowCondensed, o reemplazar por Image. Verificar que la fuente esté *Inline*, no apuntando a `Desktop\Fuentes_descarga`.
4. **[M, 🟠] Jerarquía de mezcla**: `SC_Master` → `SC_Musica`, `SC_Ambiente`, `SC_SFX`, `SC_UI`, `SC_Voz`, más `SCM_Opciones` (propio) y `SCM_Persecucion` (que baje el ambiente unos 6 dB). Conectar los sliders de pausa a cada clase y agregar VolSFX.
5. **[S, 🟠] Concurrency**: `CON_Ambiente` (máx. 8, StopQuietest), `CON_Truenos` (máx. 1), `CON_Susto` (máx. 2), y subir `AudioNumBuffersToEnqueue` a 2.
6. **[S, 🟠] Linterna**: un solo set de sonidos (descartar el duplicado de `Horror_Template`), 2D, volumen de 0,5 a 0,7 dentro de `SC_SFX`.
7. **[S, 🟠] Versionar Opciones.sav**: el campo 0 pasa a ser la versión, con defaults si el CSV no tiene la longitud esperada.
8. **[M, 🟠] Sacar la UI del personaje**: mover CrearHUD, AplicarHUD, Opciones, Candado y Galería a un `BP_UIManager` (HUD class o PlayerController). Se puede hacer en etapas, empezando por Opciones.
9. **[S, 🟢] SafeZone** en `WBP_HUD`/`WBP_Menu` y opción de tamaño de texto (multiplicador sobre `Font.Size`).
10. **[S, 🟢] Herramientas**: centralizar las rutas en `herramientas/config_rutas.py` (16 scripts usan rutas absolutas), escribir un README breve de los lanzadores, y crear `Empaquetar_MundoMagico.bat` (RunUAT BuildCookRun) para probar el juego empaquetado sin PIE, lo que podría esquivar el cuelgue de GPU.

## Solicitudes a otros agentes
- **Gameplay / Blueprints**: aprobar y coordinar la extracción de la UI de `BP_ThirdPersonCharacter` (10 MB). Confirmar qué representa el sexto campo de Opciones.sav. Que `BP_Enemigo` emita sonido espacial (pasos y respiración con atenuación) y `ReportNoiseEvent` para la IA.
- **Level design**: posicionar y dimensionar los AmbientSound del parque y el hospital, y agregar AudioVolumes con reverb (interior del hospital vs. exterior con lluvia).
- **Render / Performance**: validar que el modo Baja no reduzca la calidad de audio, y medir los underruns en un build empaquetado.
- **Narrativa**: textos de subtítulos para sustos y notas; decidir si las notas en IMFe necesitan una versión transcrita en una fuente legible.
- **QA**: cuando sea posible probar un build empaquetado, hacer un checklist de glifos, SafeZone en 16:9/21:9, navegación con mando en todos los menús, y persistencia de opciones al pasar de versión.
