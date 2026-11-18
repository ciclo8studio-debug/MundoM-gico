# @ux-toolsmith · UX / UI / Audio / Herramientas · 18-11-2026

Solo lectura: CHANGELOG (0.3.30–0.3.60), GUIA_TUNING, ARQUITECTURA_3_NIVELES, BOARD, `herramientas/*.py`, `Content/MundoMagico/{UI,Audio}` (listado de disco) e `index.html` del juego web (grep, sin base64). Nada se jugó.

## 1. Resumen
- La capa UE ya copia lo visible del HTML: portada con luz que sigue al mouse, linterna con los mismos parámetros, HUD con retrato, corazones y mapa (`T_Retrato_Isidora`, `T_Corazon`, `T_Mapa*`), configuración de 8 filas y reverb por zona.
- Lo que falta es lo **invisible** del HTML: subtítulos con interruptor, música que se adapta al estado (pausa, interior, narración), avisos que cambian con el dispositivo, y una mezcla con clases propias para Música, Voz y UI.
- El riesgo para el 10-12 es la **legibilidad del primer minuto**, más que el contenido: qué botón hace qué, dónde está el objetivo y qué se oye.
- **Nota: 6,5/10** (UX 6 · UI 7 · Audio 6,5).

## 2. Hallazgos por agente
**ux-designer**
- En el HTML el aviso de controles se reescribe al conectar un mando (`gamepadconnected` cambia `#hint` de F/E/TAB a X/A/Y/Back). En UE los avisos son texto fijo, así que un tester con mando de PS4 lee "E".
- El HTML siempre muestra un objetivo (`#obj`) y un contador (`#ev` "EVIDENCIAS 0/4"). En UE el objetivo existe en el BP del jugador (14 referencias), pero no hay contador visible de páginas o evidencias.
- El HTML cierra con un **final según las páginas reunidas** (3 textos y resumen de tiempo, páginas y golpes). La demo de UE no tiene pantalla de cierre con resumen, y es el mejor momento para invitar a dejar feedback en itch.io.

**ui-programmer**
- Toda la UI vive en `BP_ThirdPersonCharacter` (≈10 MB): `Opciones`, `Galeria`, `Mochila`, `CfgSel0..7`. Cada fila nueva suma unas 5 funciones (ver 0.3.52). Antes de agregar más filas, conviene una función genérica `CfgFila(i, delta)`.
- `Opciones.sav` sigue siendo un CSV posicional. La fila "General" se cargó con valor por defecto, pero no hay campo de versión.

**ue-umg-specialist**
- `WBP_Menu` tiene un Tick (`MoverLuz`, 0.3.58). Se acepta porque solo vive en el menú, pero no se debe copiar en el HUD: ahí, timers de 0,1 a 0,5 s, como el panel `-dev`.
- No se detectó `SafeZone` en `WBP_HUD`/`WBP_Panel`. En TV o ultrawide se recortan los corazones y el mapa. `T_PortadaHTML`, `T_PortadaJuego`, `T_PortadaNoria` y `T_PortadaPS5` parecen texturas huérfanas de portadas anteriores y pesan en la build.

**accessibility-specialist**
- El HTML tiene **Subtítulos (`oSub`)**, **Voces (`oVoz`)** y **Modo difícil (`oHard`)**. UE tiene Dificultad, pero ni subtítulos ni closed captions de sustos (chillido, golpe, carrera, latido). Para una demo de terror pública es la brecha de accesibilidad más visible.
- Faltan invertir eje Y, tamaño de texto y una señal visual del ruido propio (ya existe en `AC_Ruido`, solo en el panel `-dev`).

**tools-programmer**
- Hay 10 compositores procedurales `componer_*.py` y 8 importadores `importar_*.py` reproducibles, pero sin un índice ni un lanzador único. Falta un `herramientas/LEEME_AUDIO.md` mínimo (script → asset → SoundClass) o un `regenerar_audio.ps1`.
- Duplicados en `Audio/`: `Musica_Menu` y `MUS_Menu`, `Musica_Persecucion` y `MUS_Persecucion`, `Ambiente_Parque` y `AMB_Parque`. Parecen redirectores del renombre de la 0.2.0. Hay que limpiarlos con *Fix Up Redirectors* (con el OK del CTO, porque implica borrar).

**audio-director**
- El HTML **baja la música al 25 % en pausa o fuera de juego** (`musicTick`, rampa de 1,2 s) y la apaga dentro del pabellón. En UE la música es binaria (`MUS_Menu` o `MUS_Persecucion`) y no reacciona a la pausa ni a la tensión del Director de miedo, que ya existe.
- `Audio/Mezcla` tiene `SC_SFX`, `SC_Ambiente`, `SM_Silencio`, `RV_*` y `ATT_Puntual`. La música sigue en `/Engine/.../Music` y faltan `SC_Voz` y `SC_UI`, ya planificados en ARQUITECTURA (E2).

**sound-designer**
- El HTML usa HRTF (`panner HRTF`, refDistance 2,5 y rolloff 1,5) para los enemigos. UE ya imita esto con `ATT_Puntual` (2,5 m + 35 m + LPF), así que esa brecha está cerrada. Falta `Concurrency` para `BP_RuidoAzar` y los truenos, que ya produjeron underruns.
- Ahora hay sonido propio para la página y la linterna (0.3.60). Faltan un "tic" de UI al navegar el menú (el HTML usa `sfx(900,.05)`) y un *stinger* al recoger una página nueva para la galería.

## 3. Brechas HTML → Unreal
| HTML (index.html) | Estado UE | Cómo replicarlo |
|---|---|---|
| Portada con luz al mouse, niebla, lluvia | ✅ 0.3.58–59 | — |
| Linterna: color, cono y foco con L2 | ✅ 0.3.60 | — |
| HUD: retrato, corazones, minimapa, batería | ✅ (texturas y `WBP_HUD`) | Verificar SafeZone |
| `#obj` objetivo + `#ev` contador | 🟡 objetivo sí, contador no | `TextBlock` "PÁGINAS x/N" en `WBP_HUD`, actualizado en el evento de recoger (sin binding) |
| `#hint` cambia según teclado o mando | ❌ | `ActualizarAvisos(bMando)` desde `AnyKey` → `IsGamepadKey`; 2 juegos de texto o íconos (□ ✕ △ L2 / E Espacio F) |
| `showMsg` con fundido | ✅ (`WBP_Panel`) | — |
| Mochila (cintas, pilas, botiquín, mapa, recuerdos) | ✅ básica | Agregar la línea "Recuerdos x/N" como en el HTML |
| Galería de recuerdos (62 páginas + Ken Burns) | ✅ `Galeria`/`WBP_Pagina` | Ken Burns = animación UMG de escala y traslación (sin Tick) |
| Final según páginas + resumen | ❌ | `WBP_Panel` con 3 textos + tiempo/páginas/golpes + "Deja tu opinión en itch.io" |
| Opciones: Subtítulos / Voces | ❌ | Fila 9 "Subtítulos" + `WBP_Subtitulos` (cola de líneas con temporizador) |
| Modo difícil | ✅ Dificultad | — |
| Música adaptable (pausa 25 %, interior 0) | ❌ | `SCM_Pausa` (Música 0,25, rampa 1 s) con `Push` en la pausa; música de tensión con `Director.Tension` a un `AudioComponent` vía `SetFloatParameter` cada 0,5 s (timer existente) |
| Pausa suspende el audio y la voz | 🟡 | `SCM_Pausa` también baja SFX/Ambiente a 0,3; las voces se pausan con `SetPaused` |
| Panner HRTF de enemigos | ✅ `ATT_Puntual` | — |
| Vibración (`dual-rumble`) | ✅ 0.3.29 | Agregar el interruptor "Vibración" en Opciones |
| Voz TTS de Alfredo | ❌ (no aplica en UE) | Usar `SC_Voz` + líneas grabadas o TTS horneado con script Python; subtítulo obligatorio |

## 4. Top 6 mejoras priorizadas (demo del 10-12)
| # | Mejora | Impacto | Esfuerzo | Interruptor DA_Tuning | Responsable |
|---|---|---|---|---|---|
| 1 | Avisos según el dispositivo (mando PS4 o teclado) en el HUD y en las pantallas de ayuda | 5 | S | `UI_AvisosPorDispositivo` | ui-programmer + ue-umg-specialist |
| 2 | `WBP_Subtitulos` + fila "Subtítulos" + captions de sustos ("[chillido a la izquierda]") | 5 | M | `UI_Subtitulos` (por defecto activo) | accessibility-specialist + ui-programmer |
| 3 | Mezcla completa: `SC_Musica`, `SC_Voz`, `SC_UI` propios + `SCM_Pausa` + concurrency (`CON_Ambiente` máx. 8, `CON_Trueno` máx. 1) | 4 | M | `Audio_MezclaPausa`, `Audio_Concurrency` | audio-director |
| 4 | Música de tensión ligada al Director (2 capas, volumen según `Tension`, timer de 0,5 s) | 4 | M | `Audio_MusicaDinamica` | audio-director + sound-designer |
| 5 | Pantalla final de la demo: final según páginas, resumen y llamado a itch.io | 4 | S | `UI_FinalDemo` | ux-designer + ui-programmer |
| 6 | Contador "PÁGINAS x/N" + `SafeZone` en el HUD + "tic" de UI y *stinger* de página | 3 | S | `UI_ContadorPaginas` | ue-umg-specialist + sound-designer |

Todo con timers o eventos existentes: **ningún Event Tick nuevo**. UMG por MCP (`write_graph_dsl` con `-culture=en`), con validación solo en la build empaquetada (`Probar.bat` 1, 2 y 8). Orden sugerido: 1 → 6 → 5 (S, antes de los testers del 26-11) y después 2 → 3 → 4 (antes del freeze del 10-12).

Herramientas (fuera del top, tamaño S): `herramientas/regenerar_audio.ps1` que encadena `componer_*` → `importar_*` y se puede correr con Unreal cerrado; refactor de opciones a `CfgFila(i)` antes de sumar la fila 9.

## 5. Decisiones para el CTO
1. **¿Subtítulos activos por defecto en la demo?** Recomiendo **sí**: la demo es pública y de terror, y muchos juegan sin sonido o con el volumen bajo.
2. **¿Se limpian los assets duplicados y las portadas viejas** (`Musica_*` vs `MUS_*`, `T_Portada*` sin uso)? Implica borrar con *Fix Up Redirectors* y una build de verificación.
3. **¿Íconos de mando de PS4 o texto (□ ✕ △)?** Los íconos de `FreeSurvivalIconsKG` se ven mejor pero cuestan más. Con texto se reutiliza la fuente de respaldo `DroidSansFallback`, que ya está.
