# @ux-toolsmith · Reunión "Replicar el HTML completo en Unreal" · 18-11-2026
Base: `RECORRIDO_HTML.md`, CHANGELOG 0.3.40–0.3.67, `GUIA_TUNING.md`, `reingenieria_2026-11-18/ux-toolsmith.md`, `Probar.bat`. Solo lectura, sin abrir Unreal. "?" = verificar en el BP antes de construir.

## 1. Matriz de paridad UX / audio (HTML → UE)
Leyenda: ✅ hecho · 🟡 parcial · ❌ falta.
| # | Elemento HTML | UE hoy | Estado | Cómo cerrarlo |
|---|---|---|---|---|
| 1 | Portada: Ken Burns, lluvia, relámpagos, "Haz clic para activar el sonido", música del menú | Portada web (0.3.58–59), `MUS_Menu` | ✅ | Solo falta el tic de UI al navegar (`SC_UI`) |
| 2 | Configuración: Música, Sensibilidad, Calidad, Voz de Alfredo | 8 filas + General (0.3.52) | 🟡 | Faltan **Voces** y **Subtítulos** (B3a), **Modo difícil** e invertir Y. Usar `CfgFila` genérico |
| 3 | Despertar: desenfoque 7 px, parpadeo, "[E] Incorporarte", franjas de cine | Sala del despertar existe | 🟡? | Prompt "Incorporarte" + `DOF` del PPV con timeline 3 s; franjas = 2 `Border` negros en `WBP_HUD` |
| 4 | Objetivo arriba + flecha con metros + pista H/LT + "¿PERDIDA?" a los 180 s | `MostrarObjetivo` / `ActualizarPosObjetivo` (coords a mano) | 🟡 | `DT_Objetivos` (ID, texto, pista, destino) y `ObjetivoActual()` con la **prioridad del §2 del recorrido**, evaluada cada 0,25 s por timer. Pista = mismo widget, tecla H / LT |
| 5 | `showMsg` con fundido | `WBP_Panel` / avisos | ✅ | Absorber en `WBP_Avisos` |
| 6 | Prompt contextual "[E / A] Acción" | A2: [E]/[□] por dispositivo | 🟡 | Texto según el **objetivo mirado** (ver §2), no genérico "Recoger/Usar" |
| 7 | HUD: retrato que palidece, 6 corazones, batería, viñeta roja/pulsante ≤2 | Retrato + corazones + batería | 🟡 | Palidez = parámetro `Desat` del material del retrato; viñeta pulsante con 2 o menos (MID del PPV) |
| 8 | Minimapa circular: norte, flecha, cintas rojas, páginas blancas, objetivo; ampliar ×2,2 al tener el mapa | `T_Mapa3` 660 px en HUD | 🟡 | Marcadores de cintas/páginas/objetivo (pool de 20 `Image`, refresco 0,25 s). Ampliar con 3/M/Back solo si `Flag_Mapa` |
| 9 | Mochila (Tab/I/Y): cintas, pilas, botiquines, mapa, llave, fusible, recuerdos n/62, notas | "CINTAS x/4 · PÁGINAS x/15…" | 🟡 | Añadir llave (sí/usada/no) y **fusible**. Barra 1–4 (linterna, mochila, mapa, botiquín) + "BOTELLAS n [T] · AGACHARSE [C]" |
| 10 | Galería de recuerdos (J/↓, ←/→), "★ NOVELA COMPLETA ★" | `WBP_Pagina` papel (15 páginas) | 🟡 | `WBP_Galeria`: grilla de páginas reunidas, abre `WBP_Pagina.Llenar(i)`. Decidir 15 vs 62 (CTO) |
| 11 | Archivo de **15 notas** (N/←) + "nota añadida [N]" | No hay archivo de notas | ❌ | `DA_Notas` (15 títulos+textos del recorrido) + `WBP_Notas` (lista + lector, reutiliza el papel) + `Flags` en GI |
| 12 | Flashbacks en tinta + **voz TTS por personaje** | Burbujas sobre negro (0.3.26), sin voz | 🟡 | Voz: generar WAV offline (TTS local, un script `componer_voces.py`) → `SC_Voz`, `CON_Voz`; subtítulo siempre |
| 13 | Cabina VIDEO: 12 segmentos 3,8 s, nítido/dañado según página, subtítulos "CINTA n · fecha", saltar con E/A | No existe | ❌ | `WBP_VideoCabina`: imagen + material VHS (scanlines, ruido) con `Dañado` 0/1, texto de fecha, timer 3,8 s; cámara `CAM_Cabina`. Audio: zumbido CRT + `SFX_VHS` |
| 14 | Elección modal E (sala) / Q (quedarse) | No existe | ❌ | `WBP_Eleccion` 2 botones con foco (D-pad), [E]/[A] y [Q]/[B] según dispositivo |
| 15 | Pantalla de final (3 finales por páginas) + estadísticas + Reiniciar / Volver al parque | "FIN DE LA DEMO" fijo (0.3.66, hoy apagado) | 🟡 | `WBP_FinalDemo` (A8b) con 3 textos por umbral, tiempo, cintas, páginas, golpes, botellas, "Deja tu opinión en itch.io" |
| 16 | Muerte "TE ENCONTRÓ · FIN DE LA DEMO" + REINTENTAR | `Revivir` en respawn | 🟡 | Overlay corto con "Recuperaste X/4 cintas" antes de `PuntoRespawn` |
| 17 | Pausa: Volumen, Sensibilidad, Calidad, Subtítulos, Voces, Difícil; música al 25 % | Pausa con filas | 🟡 | `SCM_Pausa` (B3b) + filas faltantes (#2) |
| 18 | Barra de ruido azul/amarilla/roja | No (A5 pendiente) | ❌ | `ProgressBar` en HUD leyendo `RadioRuido` cada 0,2 s |
| 19 | Aliento: "Sin aliento...", jadeos, FOV 70→80 | `TextoAliento` existe | 🟡? | Verificar jadeo + FOV por timeline al correr |
| 20 | Avisos de batería <20 % / <8 %, "La batería aún está casi llena" | Batería con gasto (0.3.64) | 🟡 | 3 pitidos `SC_UI` + parpadeo del icono <15 % |
| 21 | Música MP3 del parque **solo en exteriores**, lluvia ahogada dentro | `MUS_Ambiente_Terror`, reverbs `RV_*` | 🟡 | `SCM_Interior` push/pop en los `RV_*` (música 0, lluvia LPF). Música por zona: §4 |
| 22 | Sustos con audio espacial (silla de ruedas, grito, campana payaso, fantasma, BANG) | Sustos del pasillo con `PlaySoundAtLocation` | 🟡 | Los del parque (payaso ángel, muñeca, teatro, tren) al portarse: `SCM_Susto` + caption con dirección |
| 23 | Enemigos: gruñidos/pasos HRTF, latido por distancia, drone que sube | Latido (0.3.22/0.3.50), `MUS_Persecucion` | 🟡 | Música en capas C0/C1/C2 (B3c) |
| 24 | Tele de la casa con frases cada 5–10 s | ? | ❌ | Mismas voces offline + subtítulo `[TV] ...Isidora... despierta...` |
| 25 | Goteos espaciales (bodega, pabellón, hospital) y pasos por superficie | Pasos por suelo (0.3.37/45) ✅ | 🟡 | `BP_Goteo` con timer 2–6 s, `CON_Ambiente` |
| 26 | Mando completo (LB correr, RB agachar, RT botella, ↑ botiquín, → mapa, ↓ recuerdos, ← notas, LT pista, R3 cámara, Back mapa, Start pausa) + vibración | Interactuar, linterna, botella R2, salto ✕, vibración (0.3.29) | 🟡 | Cerrar en un `IMC_Mando` único; D-pad para mochila/notas/galería/mapa. Botón ✕ ya no confirma fuera del menú |
| 27 | Teclado 1–4, Tab/I, M, R, G, V, J, N, H, C, T, Esc | Parcial | 🟡 | Mismo IMC; G calidad, V cámara |
| 28 | Puzle del teatro: "Correcto (n/5)", "El público se ríe bajito" | ? | ❌ | Solo avisos + 5 tonos de campana distintos por pedestal (feedback sin texto) |

## 2. Corrección: interactuar con lo que se mira (no lo más cercano)
Problema (0.3.66): `UsarPuerta` / `RecogerObjeto` / `InteractuarZonas` toman el **más cercano** a < 2,3 m. El HTML recoge a < 2,4 m, o a < 4,5 m **si lo miras**.
Nueva función `ElegirInteractuable()` en el jugador, llamada **por timer cada 0,1 s** (no Tick) y al pulsar E/□:
1. `SphereOverlapActors` radio 450 cm en la cámara, filtro interfaz `BPI_Interaction` (+ recogibles, puertas, interruptores).
2. Para cada candidato: `Dir = Normalize(Centro − CamPos)`, `Dot = Dir · CamForward`, `Dist`.
   - Válido si `Dist < 240` **o** (`Dist < 450` y `Dot > cos(Interaccion_AnguloMirada)`), con `Interaccion_AnguloMirada` = 20°.
   - `Puntaje = Dot × 2 − Dist / 450`. Gana el mayor (el centrado gana aunque esté 1 m más lejos).
3. Desempate: `LineTrace` cámara→centro con canal Visibility; si choca con otra cosa, se descarta (no recoger a través de paredes).
4. Guardar `Objetivo` (actor). El aviso muestra `"[E] " + NombreAccion(Objetivo)` ("Recoger pilas", "Leer el diario", "Usar la llave"). Sin objetivo, se oculta.
5. E/□ actúa **solo sobre `Objetivo`**: una sola entrada que despacha a `UsarInterruptor` / `RecogerObjeto` / `UsarPuerta` según la clase. Fin de las cadenas paralelas (causa del bug del sonido de puerta 0.3.38 y del interruptor 0.3.x).
6. Resalte opcional: `SetRenderCustomDepth` en el objetivo (contorno) con `UI_ResalteMirada` (apagado por defecto en iGPU).
Interruptor: **`UI_InteraccionMirada`** (activo). Apagado = comportamiento actual "más cercano". Prueba: 3 pilas + 1 página juntas en la sala; mirar cada una y recoger en el orden mirado.

## 3. Accesibilidad y mando (mínimo para itch.io)
- Subtítulos y descripción de sonidos **activos por defecto** (voces de flashback, tele, sustos con dirección).
- Tamaño de texto 1,0 / 1,25 / 1,5; invertir Y; sensibilidad mando separada; reducir destellos (glitch, rayos, cortocircuito, destello blanco de la silla).
- Todo modal navegable con D-pad, foco en el primer botón, B/Esc = volver. Ninguna acción solo con mouse.
- Barra de ruido y parpadeo de batería como pista visual para jugar sin sonido.

## 4. Audio por zona (paridad)
| Zona | Música / cama | Sustos clave |
|---|---|---|
| Menú | `MUS_Menu` | — |
| Pabellón 4 / hospital | Sin música; drone grave + goteos + zumbido de tubos (`SCM_Interior`) | Silla de ruedas, grito, BANG de Urgencias |
| Parque sin energía | Lluvia + viento, sin música | Cortocircuito, silueta del letrero (silencio previo, 0.3.27 ✅) |
| Parque con energía | Música del parque (exterior) + campanas del carrusel 3D | Muñeca, payaso ángel (campana+golpe 3D) |
| Bodega / casa / teatro / tren | Música 0, ambiente propio (naftalina+goteo, tele, público mudo, túnel) | Disfraces, tele, figura, fantasma |
| 4/4 cintas | C0→C2 según tensión (B3c) | Despertar de Willy y León |
| Cabina / final | Zumbido CRT; *stinger* final 8 s | — |
Implementación: `BP_ZonaAudio` (caja con `Musica`, `Mix`, `Reverb`) en vez de coordenadas a mano en `ActualizarZona`.

## 5. Orden por builds (1–2 cambios, cada uno con interruptor en `DA_Tuning`)
Requisito previo: que el parque no cierre la iGPU (lo resuelve tech/perf). Mientras tanto, todo se prueba en el hospital + `Probar.bat` P.
| Build | Cambios | Interruptor | Valida |
|---|---|---|---|
| 0.3.68 | **Interacción por mirada** (§2) + prompt con nombre de acción | `UI_InteraccionMirada` | Sala con objetos juntos, teclado y mando |
| 0.3.69 | `DT_Objetivos` + pista H/LT + "¿PERDIDA?" 180 s + flecha | `UI_Objetivos` | Recorrido hospital→entrada |
| 0.3.70 | Archivo de notas (`DA_Notas`, `WBP_Notas`) + "nota añadida [N]" | `UI_Notas` | 3 notas del hospital/entrada |
| 0.3.71 | Mochila completa (llave, fusible, barra 1–4) + minimapa con marcadores/ampliar | `UI_MochilaHTML`, `UI_MapaMarcadores` | Parque: cintas en rojo |
| 0.3.72 | `WBP_Subtitulos` + captions de sustos + tamaño de texto | `UI_Subtitulos` | Probar sin sonido (opción 4) |
| 0.3.73 | SoundClasses + `SCM_Pausa/Interior/Susto` + `BP_ZonaAudio` | `Audio_MezclaZonas` | Underruns = 0 en log |
| 0.3.74 | Cabina VIDEO (`WBP_VideoCabina`) + elección E/Q | `Cabina_Video` | 4 cintas + fusible por F2/dev |
| 0.3.75 | `WBP_FinalDemo` 3 finales + muerte "TE ENCONTRÓ" | `UI_FinalDemo` | Partida completa |
| 0.3.76 | Voces offline (flashbacks + tele) + galería | `Audio_Voces`, `UI_Galeria` | Subtítulo + voz en `SC_Voz` |
| 0.3.77 | Música en capas + barra de ruido + accesibilidad de cámara | `Audio_MusicaDinamica`, `UI_AccesibilidadCamara` | **Freeze 6-12** |

## 6. Herramientas
- `Probar.bat`: añadir zonas `casa`, `cabina` y `-dev` con atajo "dar 4 cintas + fusible" (como F2 del HTML) para probar cabina y final sin recorrer.
- `Probar_Humo.ps1` por zona con `HUMO OK` (pendiente de la reingeniería) antes de cada build.

## 7. Decisiones del CTO
1. Galería y contador: **15 páginas** en texto (demo) o volver a **62** como el HTML? Recomiendo 15 + umbrales de final 15 / ≥8 / <8.
2. Voces de flashback: TTS offline generado (recomendado) o solo subtítulos para el 10-12.
3. Mantener el cortocircuito del 4.º interruptor (HTML) que se quitó en 0.3.x: recomiendo **sí**, con `Interruptor_Cortocircuito`.
