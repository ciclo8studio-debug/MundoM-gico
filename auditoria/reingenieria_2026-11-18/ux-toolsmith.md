# @ux-toolsmith · Reingeniería UX / UI / Audio / Herramientas · 18-11-2026
Base: `auditoria/estudio_2026-11-18/ux-toolsmith.md` (diagnóstico, no se repite) y `PLAN_ESTUDIO` (A2, A8, A9, B3). Solo lectura, sin abrir Unreal.

## 1. Visión
- La UI deja de vivir en `BP_ThirdPersonCharacter`: **un gestor (`AC_UI`) y widgets pequeños**, cada uno con una sola tarea y reaccionando a eventos, nunca con bindings ni Tick.
- Los datos que sobreviven al cambio de mapa (opciones, dispositivo, páginas, flags) viven en **`BP_GI_MundoMagico`**, que avisa con *dispatchers*.
- El audio pasa de "dos músicas y volúmenes sueltos" a **una mezcla con clases, concurrencias y estados** (juego, pausa, novela, susto), y la música respira con la tensión del Director.
- Accesibilidad mínima pero real para una demo pública de terror: subtítulos con sustos, tamaño de texto, sensibilidad, invertir Y y destellos.
- Herramientas: **una prueba de humo automática por zona** que deja un informe, para que cada build (1–2 cambios) se valide sin jugar 20 minutos.

## 2. Arquitectura de UI
**Dónde vive cada cosa**
| Pieza | Dónde | Responsabilidad |
|---|---|---|
| `BP_GI_MundoMagico` | GameInstance (ya existe) | `Opciones` (struct `S_Opciones` + `Version`), `bMando`, `PaginasReunidas`, `TiempoPartida`, `Golpes`. Dispatchers `OnDispositivo(bMando)` y `OnOpciones` |
| `AC_UI` | componente nuevo en el jugador (más adelante, en el PlayerController) | Crea los widgets en `BeginPlay`, guarda la **pila de pantallas** (`Push/Pop`), fija `InputMode` y foco, y expone `MostrarAviso(ID)`, `Subtitulo(Texto, Hablante, Dur)` y `AbrirFinal()` |
| `DT_Avisos` | DataTable | `ID → TextoTeclado / TextoMando` (por ejemplo `Interactuar → "E" / "□"`). Un aviso nunca lleva una tecla escrita a mano |
| `DT_OpcionesFilas` | DataTable | Fila → nombre, mínimo, máximo, paso y campo. Alimenta un `CfgFila(i, delta)` genérico (fin de las 5 funciones por fila) |

**Widgets objetivo (capas por ZOrder)**
| Capa | Widget | Notas |
|---|---|---|
| 0 Juego | `WBP_HUD` | Retrato, corazones, mapa, batería, contador "PÁGINAS x/N" y barra de ruido (A5). Dentro de un `SafeZone` |
| 1 Juego | `WBP_Subtitulos` | Cola de 2 líneas con timer y fondo al 60 %. Escala según `Opciones.TamTexto` |
| 1 Juego | `WBP_Avisos` | Aviso contextual ("□ Abrir") y `showMsg` con fundido (absorbe `WBP_Panel`) |
| 2 Modal | `WBP_Pausa` → `WBP_Opciones` | Las Opciones se reutilizan desde el menú principal (un solo widget, hoy duplicado entre `WBP_Menu` y la pausa) |
| 2 Modal | `WBP_Novela` / `WBP_Galeria` / `WBP_Mochila` | Ken Burns como animación UMG |
| 3 Sistema | `WBP_Carga`, `WBP_FinalDemo` | El final: 3 textos según páginas, resumen (tiempo, páginas, golpes) y "Deja tu opinión en itch.io" |
| 9 Dev | `WBP_PanelDev` | Solo con `-dev` (ya existe) |

**Flujo de pantallas**
```
[WBP_Menu] --Jugar/Continuar--> (Juego: HUD + Subtitulos + Avisos)
   |  Opciones <-> [WBP_Opciones] <-- Volver --+
   |                                           |
(Juego) --Start/Esc--> [WBP_Pausa] --Opciones--+   Pausa: SCM_Pausa + SetGamePaused
(Juego) --Tab/Back--> [WBP_Mochila] --Recuerdos--> [WBP_Galeria] --> [WBP_Novela]
(Juego) --página--> [WBP_Novela] (modal corto, vuelve al juego)
(Juego) --salida demo--> [WBP_FinalDemo] --> Menú | Salir
Cualquier modal: B/Esc = AC_UI.Pop() ; pila vacía = InputMode Game + oculta cursor
```
**Entrada y dispositivo**
- Un **AnyKey** en el jugador llama a `IsGamepadKey`. Si cambia `bMando`, el GI dispara `OnDispositivo`; `WBP_Avisos`, `WBP_HUD` y `WBP_Opciones` releen `DT_Avisos`. El evento solo actúa cuando hay un cambio, así que no cuesta nada mientras se juega.
- Mouse: el movimiento del mouse vuelve a "teclado" solo si supera unos 5 px, para evitar parpadeos por el ruido del mouse.
- En los modales, el foco va al primer botón (`SetUserFocus`) y la navegación usa el D-pad o el stick. Los botones usan `IsFocusable`, sin pseudo-selección manual `CfgSel0..7` en los widgets nuevos.
- Regla: **cero property bindings** (cada binding se evalúa en cada frame). Todo se actualiza con `Set...` desde eventos o timers de 0,1 a 0,5 s.

**Migración (patrón "estrangulador")**: los widgets nuevos (`Avisos`, `Subtitulos`, `FinalDemo`) nacen en `AC_UI` desde el día 1. `Pausa` y `Opciones` siguen en el BP del jugador hasta después del 10-12 y luego se mudan una por build. `Opciones.sav` (CSV) migra a `SG_Opciones` v2 con lectura de respaldo del CSV.

## 3. Arquitectura de audio
**SoundClasses** (`/Game/MundoMagico/Audio/Mezcla`): `SC_Master` → `SC_Musica`, `SC_SFX` (→ `SC_Ambiente`, `SC_Pasos`), `SC_Voz`, `SC_UI`. Dejar `/Engine/.../Music`. Sliders: General = Master, Efectos = SFX, Música, Voz (nuevo). `SC_UI` lleva `bIsUISound` para sonar en pausa.

**Mixes** (Push/Pop desde `AC_UI` o el Director; rampas en segundos)
| Mix | Música | SFX | Ambiente | Voz | UI | Cuándo |
|---|---|---|---|---|---|---|
| `SCM_Pausa` | 0,25 (1,2 s) | 0,3 | 0,3 | pausa | 1 | Pausa, Opciones en el juego |
| `SCM_Novela` | 0,5 | 0,6 | 0,6 | 1 | 1 | Leer una página o la galería |
| `SCM_Susto` | 0,4 (0,1 s ↓ / 1,5 s ↑) | 1 | 0,5 | 1 | — | 1,5 s alrededor del susto (*ducking*) |
| `SCM_Interior` | 0 | 1 | 0,7 | 1 | — | Pabellón (los volúmenes `RV_*` existentes hacen el Push) |
| `SM_Silencio` | ya existe | | | | | Transición TV |

**Concurrencias**: `CON_Ambiente` máx. 8 (para el más bajo), `CON_Trueno` 1, `CON_Pasos` 2, `CON_UI` 2, `CON_Voz` 1 (para el más antiguo), `CON_Susto` 1 (rechaza el nuevo), `CON_Enemigo` 3. Esto ataca los underruns de `BP_RuidoAzar` y los truenos.

**Música dinámica (3 capas, mismo tempo y largo, en loop sincronizado)**: un componente `AC_MusicaTension` en `DEV_DirectorMiedo` usa **el timer que ya tiene** el Director (cada 0,5 s), sin Tick.
- C0 *drone*: siempre suena, de 0,6 a 1,0 según la tensión.
- C1 pulso grave: entra con tensión > 35 y sale con < 25 (histéresis).
- C2 persecución: cuando un enemigo persigue o la tensión es > 70. Reemplaza el corte binario a `MUS_Persecucion`.
- Fundidos con `AdjustVolume` de 1,5 s.
- Las capas se generan con `componer_musica_terror.py --capas` (stems de 48 kHz, OGG en streaming).

**Eventos de sonido**
| Evento | Estado | Clase / Conc. | Spec (si falta) | Caption |
|---|---|---|---|---|
| Pasos por superficie, latido, lluvia, truenos, puertas, página, linterna, alarma, TV, reverbs | ✅ | SFX / Pasos / Trueno | — | [trueno] solo si `Subt_Ambiente` |
| UI navegar / confirmar / volver | ❌ | UI / CON_UI | 40–80 ms, senoidal de 900 Hz con clic (como `sfx(900,.05)` del HTML), −18 dBFS, 2D | — |
| *Stinger* de página nueva | ❌ | Música | 2,5 s, campana y cuerda grave, −12 dBFS | [nota musical] |
| Chillido / alerta de la Sombra | ❌ | SFX / Susto | 1,2 s, 3D `ATT_Puntual`, −6 dBFS | [CHILLIDO · izquierda] |
| Escondite: entrar, salir, respiración contenida | ❌ | SFX | 0,4 s madera o tela; loop de 4 s de respiración a −20 dBFS | [respiras despacio] |
| Batería baja de la linterna | ❌ | UI | 3 pitidos de 0,1 s y parpadeo | [linterna: batería baja] |
| Golpe recibido | ❌ | SFX / Susto | 0,5 s de impacto sordo con un zumbido de 2 s (LPF) | [te golpean] |
| Botella que se rompe (A5) | ❌ | SFX / Enemigo | 0,8 s, 3D, alcance de 40 m | [vidrio rompiéndose · atrás] |
| Negatoscopio y cámara de la morgue (B2) | ❌ | Ambiente | Loop de 6 s con zumbido de 50 Hz y *flicker* | [zumbido eléctrico] |
| Final de la demo | ❌ | Música | 8 s, resolución en menor | — |
| Música en capas C0/C1/C2 | ❌ | Música | Loops de 32 s, mismo tempo, −16 LUFS | — |

## 4. Accesibilidad
- **Subtítulos** (activos por defecto): `Subtitulo(Texto, Hablante, Dur)` con Dur = 2 s + 0,06 s por carácter, máximo 2 líneas y el nombre del hablante en color.
  - **Descriptores de sustos** entre corchetes, con la dirección calculada **una vez** al emitir el sonido (producto punto con la cámara → izquierda, derecha, atrás o delante).
  - Interruptores: `Subtítulos` (Sí/No) y `Describir sonidos` (Sí/No).
- **Tamaño de texto**: Normal, Grande o Muy grande (escala 1,0 / 1,25 / 1,5). Afecta Subtítulos, Avisos, Novela y Opciones.
- **Cámara**: sensibilidad separada para mouse y mando (de 0,3 a 2,0, en pasos de 0,1) e **invertir Y**. Se aplica en el `Look` del Enhanced Input con un multiplicador leído del GI.
- **Vibración** (Sí/No) y **reducir destellos** (atenúa truenos, glitch TV y aberración un 70 %).
- **Remapeo mínimo**: con teclado, 4 acciones (Interactuar, Linterna, Agacharse, Correr) mediante Enhanced Input User Settings (`MapPlayerKey`), guardado en `SG_Opciones`. Con mando, solo 2 presets (Estándar y Zurdo). Remapeo completo: después de la demo.
- Señal visual del ruido propio (barra del HUD, A5) como apoyo para quien juega sin sonido.

## 5. Herramientas
- **`Probar.bat` v2**:
  - Zonas nuevas: Sótano, Morgue, Escondites y Casona (`-ir=sotano|morgue|escondites|casona`; requiere entradas en `BP_KitPrueba`).
  - `H` prueba de humo, `L` abre el último log, `B` **borra la partida** (la renombra a `.bak` con fecha, no la elimina) y `S` abre sin sonido (`-nosound`) para probar los subtítulos.
- **`herramientas/Probar_Humo.ps1`**: recorre las zonas una por una (nunca dos procesos a la vez) con `-ir=<zona> -dev -humo`.
  - `BP_KitPrueba` con `-humo`: después de 20 s escribe `HUMO OK <zona> fps=<n> mem=<MB>` en el log y llama a `QuitGame`.
  - El script espera hasta 90 s por proceso, copia el log a `Build\v<ver>\humo\<zona>.log` y busca `HUMO OK`, `Error`, `Ensure`, `Accessed None` y `underrun`.
  - Escribe `humo_v<ver>.txt` (PASA / FALLA por zona), que @qa-publisher adjunta al checklist.
- **Captura de logs**: `Recoger_Logs.ps1` junta `Saved\Logs` (del juego y de `%LOCALAPPDATA%\MundoMagico`), el log de build y `DxDiag` en un ZIP para los testers del 26-11.
- **Audio**:
  - `regenerar_audio.ps1`: encadena `componer_*` → `importar_*`, con Unreal cerrado para componer.
  - `verificar_audio.py`: revisa pico ≤ −1 dBFS, sonoridad aproximada por clase, 48 kHz y **mono para 3D**.
  - Índice `script → asset → SoundClass → caption` como tabla dentro de `GUIA_TUNING.md`, sin crear un archivo nuevo.

## 6. Plan por builds
| Build | Cambio (1–2) | Interruptor `DA_Tuning` | Valida |
|---|---|---|---|
| — (sin Unreal, 19-11) | `Probar.bat` v2, `Probar_Humo.ps1`, `Recoger_Logs.ps1`, `regenerar_audio.ps1` | — | Ejecución sobre v0.3.61 |
| 0.3.62 | **A2** GI `bMando` + `DT_Avisos` + `WBP_Avisos` (nace `AC_UI`) · flag `-humo` | `UI_AvisosPorDispositivo` | Humo + mando PS4 |
| 0.3.63 | **A8a** contador de páginas + `SafeZone` · `SC_UI` + tic de UI + `CON_UI` | `UI_ContadorPaginas` | Humo, zonas 1 y 8 |
| 0.3.64 | **A8b** `WBP_FinalDemo` (+ *stinger*) | `UI_FinalDemo` | Partida completa → **build de testers 25-11** |
| 0.3.65 | Refactor `CfgFila` + `SG_Opciones` v2 (sin cambio visible) | — (con respaldo del CSV) | Opciones viejas cargan |
| 0.3.66 | **B3a** `WBP_Subtitulos` + captions de sustos + tamaño de texto | `UI_Subtitulos` | Opción S (sin sonido) |
| 0.3.67 | **B3b** SoundClasses, `SCM_Pausa`/`Novela`/`Susto`, concurrencias, slider Voz | `Audio_MezclaPausa`, `Audio_Concurrency` | Underruns = 0 en el log |
| 0.3.68 | **B3c** música en 3 capas con `AC_MusicaTension` | `Audio_MusicaDinamica` | Panel `-dev` (tensión) |
| 0.3.69 | Sensibilidad, invertir Y, vibración, reducir destellos, remapeo de 4 teclas | `UI_AccesibilidadCamara` | **Freeze el 6-12** |
| Después del 10-12 | Mudar `Pausa`/`Opciones` a `AC_UI`; mover `AC_UI` al PlayerController; remapeo completo | — | — |

## 7. Decisiones del CTO
1. **¿Subtítulos y descripción de sustos activos por defecto en la demo?** Recomiendo **sí**.
2. **¿La mudanza de `Pausa`/`Opciones` fuera del BP del jugador queda para después del 10-12?** Recomiendo **sí**: antes de la demo, solo nacen en `AC_UI` los widgets nuevos.
3. **¿Música de 3 capas o de 2?** 3 capas cuestan unos 3 streams OGG simultáneos (bajo en RAM). 2 capas (drone + persecución) ahorran una pieza de composición. Recomiendo **3**, con la C1 apagable desde `DA_Tuning`.
