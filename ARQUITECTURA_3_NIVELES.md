# Arquitectura en 3 niveles: Hospital → Casona → Parque

**Origen:** el "System Prompt Maestro" que entregó el CTO el 11-11.
**Responsables:** @tech-lead es el arquitecto principal. Implementan @gameplay-engineer, @world-builder, @ux-toolsmith y @qa-publisher.
**Proyecto:** `MUNDO_MÁGICO`, el que el CTO decidió que manda el 11-11.

---

## 0. Ajustes del documento maestro a la realidad del proyecto

| El documento maestro pide | Realidad del proyecto | Cómo se resuelve |
|---|---|---|
| C++ o Blueprints | Solo Blueprints. Claude los edita por MCP con un DSL de texto. | **100 % Blueprints.** Los diagramas de abajo son flujos BP. |
| 3 niveles | El 11-11 se aprobó separar en **2 mapas** (hospital y parque). | **Se amplía a 3:** `L_Hospital`, `L_Casona` y `L_Parque`. Los 2 mapas aprobados son la primera etapa de este plan. |
| Steam | El 13-11 itch.io restringido; el 16-11 itch.io público. | Las fechas de itch.io siguen en pie. Steam queda para la v1.0 y necesita Steamworks y revisión. |
| Cámara híbrida 1ª/3ª | Hoy es 3ª persona al hombro (`CambiarCamara` y `CamaraDinamica`, brazo de 190 cm). | Se agrega 1ª persona como opción (sección 3.A). |
| "Monstruo con linterna" | Hay un payaso (`clown_monster`), un Whisper y `BP_Enemigo`, que hoy solo actúa en la persecución. | El **enfermero/sombra con linterna** sale de `Horror_Template` + `Whisper`. El payaso queda para el parque. |
| 62 páginas | Las 62 existen (`REC_Pagina*`, `T_Pag_*`). | Se reparten: 1–15 en el hospital, 16–40 en la casona y 41–62 en el parque. |
| Hardware | 8 GB de RAM, iGPU de 128 MB, solo se prueba con builds empaquetadas. | Un mapa por nivel con `OpenLevel`, **sin World Partition**. Luz horneada y presupuesto fijo por mapa (sección 6). |

---

## 1. Estructura de carpetas

```
/Game/MundoMagico/
  Core/        BP_GI_MundoMagico (GameInstance), BP_GM_MundoMagico, SG_Partida, SG_Opciones
  Datos/       DA_Tuning (ya existe), DA_NovelaData, BP_PaginaDato (struct)
  Interfaces/  BPI_Interactuable, BPI_Escondite, BPI_Ruido
  Componentes/ AC_Linterna, AC_Ruido, AC_Cordura, AC_Interaccion
  Mundo/       BP_TransicionNivel, BP_VolumenZona, BP_HideSpot, BP_DoorLocked, BP_PaginaNovela
  IA/          BP_AI_Sombra (Character), AIC_Sombra, BT_Sombra, BB_Sombra, BTT_*, BP_RutaPatrulla (Spline)
  Miedo/       BP_DirectorMiedo
  UI/          WBP_Novela, WBP_Transicion (TV/VHS), WBP_Codigo, WBP_Subtitulos
Maps/          L_Hospital, L_Casona, L_Parque, L_Menu
```

**Reglas:**
- Ningún Blueprint nuevo usa `Event Tick`; se trabaja con timers y overlaps.
- No hay coordenadas fijas: todo usa volúmenes y etiquetas.
- Todo valor afinable vive en `DA_Tuning`, y cada feature tiene su `Activar_X`.

---

## 2. Persistencia entre niveles: `BP_GI_MundoMagico` (GameInstance)

| Variable | Tipo | Para qué |
|---|---|---|
| `PaginasRecogidas` | Array de Int (o 62 bools) | Inventario de la Novela |
| `Flags` | Array de Name | Eventos narrativos, por ejemplo `LlaveMorgue`, `ApagonCasona`, `SalidaHospital` |
| `Bateria`, `Pilas`, `Botellas` | Float / Int / Int | Estado de la protagonista |
| `NivelActual`, `PuntoEntrada` | Name | A qué mapa vuelve **Continuar** y en qué punto se aparece |
| `Cordura` | Float | Se arrastra al parque |

**Flujo de guardado:**
```
Recoger / Checkpoint ─► GI.ActualizarEstado ─► SG_Partida ← GI (Save Game to Slot "Partida")
Continuar (menú)     ─► Load Game from Slot ─► GI ← SG_Partida ─► Open Level (GI.NivelActual)
BeginPlay del mapa   ─► GI.PuntoEntrada ─► buscar actor con etiqueta "Entrada_<PuntoEntrada>" ─► colocar al jugador
```
`SG_Partida` lleva un campo `Version:int`. Si una partida es de una versión vieja, se descarta con un aviso.

---

## 3. Sistemas

### A. Cámara híbrida 1ª/3ª persona
```
Input "CambiarVista" ─► EnPrimera = !EnPrimera
  ─► Timeline TL_Camara (0,35 s, curva EaseInOut)
       Update: SpringArm.TargetArmLength = Lerp(190, 0, Alpha)
               SpringArm.SocketOffset    = Lerp((0,60,55), (0,0,0), Alpha)
  ─► Si EnPrimera:  Mesh.SetOwnerNoSee(Cabeza) · bUseControllerRotationYaw = true
                    SpringArm.bDoCollisionTest = false · Cámara al socket "head"
     Si no:         lo inverso · CharacterMovement.bOrientRotationToMovement = true
```
- **Sin tirones en espacios chicos:** en tercera persona se usa `ProbeSize` de 12 y `bEnableCameraLag` con valor 10. Al pasar a primera, el `Timeline` arranca desde la posición real del brazo y no desde 190.
- **Ocultar la cabeza:** Isidora es un mesh único sobre el esqueleto del mannequin. Se oculta con `HideBoneByName("head")` en primera y se vuelve a mostrar en tercera.

### B. IA "monstruo con linterna" (`BP_AI_Sombra`)
**Percepción** (`AIPerceptionComponent` en `AIC_Sombra`):
- **Vista:** 18° de cono a cada lado (es el haz), 2200 cm de alcance y 2600 cm para perder al jugador. Una `SpotLight` real en la mano hace visible el haz.
- **Oído:** radio de 1500 cm. El jugador emite ruido con `ReportNoiseEvent`, con un volumen que depende de la superficie (sección D).

**Behavior Tree** (`BT_Sombra`, Blackboard: `Estado`, `Objetivo`, `UltimoRuido`, `IndicePatrulla`):
```
Root (Selector)
├─ [Estado == Persecución] Secuencia
│     ├─ Decorador: ¿el jugador sigue visible o se vio hace menos de 4 s?
│     ├─ BTT_MoverA(Objetivo, velocidad de persecución)
│     └─ BTT_Atrapar (a menos de 120 cm) ─► golpe ─► Respawn en el checkpoint
├─ [Estado == Investigar] Secuencia
│     ├─ BTT_MoverA(UltimoRuido, velocidad de alerta)
│     ├─ BTT_MirarAlrededor (3 s, gira el haz)
│     └─ BTT_SetEstado(Patrulla)
└─ Patrulla (por defecto)
      ├─ BTT_SiguientePuntoSpline(BP_RutaPatrulla)
      ├─ BTT_MoverA(punto, velocidad de patrulla)
      └─ Wait 1–3 s
```
**Pasa a Persecución** cuando el haz toca la cápsula del jugador: un `LineTrace` desde la linterna confirma que no hay nada en medio y el jugador no está escondido (`BPI_Escondite.EstaEscondido == false`).
**Pasa a Investigar** cuando escucha un ruido con fuerza mayor o igual al umbral.
Todas las velocidades, ángulos, radios y tiempos salen de `DA_Tuning`.

### C. La Novela (`BPI_Interactuable` + `DA_NovelaData`)
```
Jugador [□ / E] ─► AC_Interaccion: SphereTrace 180 cm ─► ¿implementa BPI_Interactuable?
   ─► Interactuar(Jugador)
BP_PaginaNovela.Interactuar:
   ─► GI.PaginasRecogidas.AddUnique(ID_Pagina)
   ─► DA_NovelaData[ID] ─► Texto, Voz (SoundBase), FlagQueDesbloquea (Name), Secuencia (LevelSequence)
   ─► WBP_Novela.Mostrar(Texto) en fuente manuscrita + papel crujiendo + voz
   ─► Si FlagQueDesbloquea != None ─► GI.Flags.Add ─► Broadcast "FlagCambiado"
   ─► Si hay Secuencia ─► Play Level Sequence
BP_DoorLocked y los eventos escuchan "FlagCambiado" y se abren o se activan solos.
```
`DA_NovelaData` guarda 62 filas: `ID`, `Nivel`, `Titulo`, `Texto`, `Voz`, `Flag`, `Secuencia`. Los textos se toman de IMAGINARIO, que está autorizado.

### D. Superficies sonoras (`AC_Ruido`)
- Cada material físico (`PM_Madera`, `PM_Alfombra`, `PM_Baldosa`, `PM_Tierra`, `PM_Metal`, `PM_Charco`) tiene en `DA_Tuning` un **multiplicador de ruido**:

  | Superficie | Multiplicador |
  |---|---|
  | Madera | 1,6 |
  | Alfombra | 0,3 |
  | Baldosa | 1,0 |
  | Tierra | 0,6 |
  | Metal | 1,8 |
  | Charco | 1,4 |

- **Flujo:** en cada paso (`AnimNotify` o el timer de `Paso`, que ya existe) se hace un trace hacia abajo para leer `PhysMaterial`. Ruido = base (caminar 1, correr 2,5, agachado 0,3) × multiplicador de la superficie. Con eso se llama a `ReportNoiseEvent(Loudness, MaxRange = 1500 × Loudness)` y se reproduce el sonido de paso.
- Hoy los pasos eligen el sonido por nombre de mesh (v0.3.37). Hay que **migrar a Physical Materials**: así cada superficie cambia el sonido y además el ruido que oye la IA.

### E. Escondites (`BP_HideSpot`) y puerta con código (`BP_DoorLocked`)
```
BP_HideSpot (camas, armarios, arbustos)
  Interactuar ─► desactivar la colisión de la cápsula ─► Cámara a PuntoVista (Arrow)
               ─► GI/Jugador.Escondido = true ─► AC_Ruido.Multiplicador = 0
               ─► "Aguantar respiración" opcional (AC_Cordura, 6 s según DA_Tuning)
  Salir        ─► teletransporte a PuntoSalida + 100 cm en Z (el bug de v0.3.2) ─► restaurar
BP_DoorLocked (Modo: Llave | Código | Flag)
  Interactuar ─► Llave: ¿GI.Flags contiene NombreLlave?
              ─► Código: WBP_Codigo (teclado 0–9, se navega con mando) ─► ¿coincide con CodigoCorrecto?
              ─► Flag:  escucha "FlagCambiado"
              ─► Abrir: Timeline de rotación + sonido; con 3 errores, ruido fuerte (atrae a la IA)
```
`BP_Hiding_Door`, que ya existe, se convierte en una clase hija de `BP_HideSpot`.

### F. Cordura y neblina (parque)
`AC_Cordura` funciona con un timer de 0,25 s:
- Fuera de un `BP_VolumenZona` con `Sendero = true` y sin luz cerca, la cordura baja **0,8 por segundo**. En el sendero o cerca de un farol sube **1,5 por segundo**.
- Efecto: material de post-proceso con desaturación = 1 − cordura y viñeta, y el latido (`BP_LatidoMiedo`) sube de tono.
- Si la cordura llega a 0, aparece una alucinación (Whisper) y el `BP_DirectorMiedo` dispara un susto.

### G. Transiciones entre niveles (`BP_TransicionNivel` + `WBP_Transicion`)
```
Overlap / evento ─► GI.Guardar(NivelDestino, PuntoEntrada) ─► WBP_Transicion.Salida(Estilo)
   Estilo TV:  la imagen se encoge a una línea y luego a un punto ("tuk" + pitido) ─► negro y silencio 1,5 s
   ─► Open Level by Name(NivelDestino)
BeginPlay del mapa nuevo ─► WBP_Transicion.Entrada(Estilo)
   Estilo VHS: tracking + "▶ PLAY 03:17" + clac del mecanismo ─► el parque se sintoniza (aberración) ─► 20–30 s de calma
```
- Hospital → Casona: **glitch + TV**.
- Casona → Parque: **ático + VHS**.

### H. `BP_DirectorMiedo`, uno por mapa
- Tensión de 0 a 100 y enfriamiento global, que sale de `DA_Tuning`.
- Ningún susto (scripts, Isidora, payaso, apagón) se dispara solo: cada uno pide turno con `PedirTurno(Tipo, Intensidad)`.
- No repite el mismo tipo dos veces seguidas: rota entre audio, visual, físico y enemigo.

---

## 4. Los 3 niveles

### 🏛️ Nivel 1 · `L_Hospital` (despertar clínico) · 25–30 min
| | |
|---|---|
| **Planta baja** | Sala de inicio (tutorial de movimiento y linterna), pasillos en L, recepción. Parte de la sala y el pasillo que existen hoy (`P4_*`, `HX_*`, `HOS_*`). |
| **2.º piso** | Pabellón quirúrgico y Rayos X: **puzle del negatoscopio**, donde las radiografías muestran los dígitos de un `BP_DoorLocked` de código. Aquí patrulla la primera Sombra. |
| **Sótano / morgue** | Presión alta y luz baja. Ahí está la **llave de paso al exterior** (flag `LlaveMorgue`). Pasillo de urgencias con su susto, balizas y alarma (v0.3.40). |
| **Kit** | `Horror_Hospital`: 26 muros, techos, 5 puertas, camas, biombos, refrigeradores. Más `LampsPack`, `TA_Corpses` y `SM_Notes`. |
| **Mecánicas nuevas** | Sigilo pasivo y cobertura, cono de la linterna enemiga, escondites en camas y armarios. |
| **Páginas** | 1–15 |
| **Look** | **Verdoso clínico** (LUT verde y cian), fluorescentes parpadeantes, suelo mojado. |
| **Salida** | Llave de la morgue → puerta exterior → **glitch + TV** → `L_Casona`. |

### 🏡 Nivel 2 · `L_Casona` (terror íntimo) · 20–25 min
| | |
|---|---|
| **Planta baja** | Vestíbulo con escalera central, comedor y biblioteca: **puzle de álbumes familiares** (ordenar 4 fotos por año, que da el código de la planta alta). |
| **Planta alta** | Pasillo estrecho con **alfombra**, que es silenciosa, contra la **madera** del resto, que cruje. Dormitorios con roperos para esconderse y baño principal. |
| **Ático** | Escape vertical al parque. |
| **Kit** | `UnrealBucket` (sillones, sillas, araña, consola, jarrones: ya instalado y sin usar), `SICKA_MouldingSet` (molduras), `DetectiveOffice` (escritorio y papeles), `AnimatedWoodenCrate` (ático). Las piezas de la "Casa de Alfredo" del parque se mudan aquí. |
| **Enemigo** | Una Sombra anciana con farol: vista corta y **oído hiperdesarrollado**, con radio ×2. |
| **Evento** | **Apagón** a mitad del nivel (flag `ApagonCasona`): se pierde la luz ambiente y solo queda la linterna (o una vela, que dura poco pero no hace ruido). |
| **Páginas** | 16–40 |
| **Look** | **Ámbar sucio**, con lámparas de tungsteno que mueren. Después del apagón, solo azul de luna. |

### 🌳 Nivel 3 · `L_Parque` (clímax) · 20–25 min
| | |
|---|---|
| **Umbral** | Se sale del ático a la neblina, a la entrada actual (`ENT_*`). |
| **Laberinto** | Senderos de tierra, **faroles de sodio** (la mitad apagados), columpios y carrusel oxidados, vegetación densa (`GV_FreeShrubs`). Las primitivas se reemplazan por `SoulCity` y `Apocalypse`. |
| **Gazebo central** | Una estructura nueva en el centro con el **atril de la Novela**. Se arma con `SICKA_MouldingSet` (columnas) y un techo de `UnrealBucket` o de Fab. |
| **Mecánicas** | Sigilo abierto (los arbustos cortan la línea de visión), **cordura y neblina** (3.F), y el **payaso** como amenaza real: avanza cuando no lo miras. |
| **Asedio final** | Se colocan las páginas 41–62 en el atril. Convergen la Sombra del hospital, la de la casona y el payaso. La **Novela arde** (linterna cargada: destello que los aturde), lo que disipa la pesadilla y da paso a la secuencia final. |
| **Look** | **Sodio y azul**: dos temperaturas, niebla densa, rojo solo para el peligro. |

---

## 5. Migración desde `L_Pabellon4`

1. **Respaldo:** etiqueta git `v0.3.41-monolito`. `L_Pabellon4` queda congelado.
2. **`L_Hospital`:** duplicar el monolito y borrar todo lo que esté en y < −3600, es decir, el parque. Las coordenadas de la sala y el pasillo no cambian, así que las funciones del jugador siguen funcionando mientras se migran a volúmenes. Después se agregan el **2.º piso y el sótano**, construidos con el kit de `Horror_Hospital`.
3. **`L_Parque`:** duplicar el monolito y borrar el hospital. Se agrega el gazebo.
4. **`L_Casona`:** es un mapa **nuevo**, con blockout gris primero y después el kit.
5. **Jugador:** en vez de reescribirlo, se le **resta** lógica. Las funciones con coordenadas (`ActualizarZona`, `RevisarSalida`, `Vigilar`, `RevisarCines`, `RevisarFinDemo`) se reemplazan, una por una, por `BP_VolumenZona` y `BP_TransicionNivel`. El guardado pasa al GameInstance.
6. **Páginas:** reenumerar las `REC_Pagina*` según el rango de cada nivel y crear `DA_NovelaData`.
7. **Cook:** los 4 mapas en la lista (más `L_Menu`), `DirectoriesToNeverCook` para los packs que no se usan y `DirectoriesToAlwaysCook=/Game/MundoMagico/Datos`.

---

## 6. Presupuesto de rendimiento por mapa (iGPU)

| | Objetivo |
|---|---|
| fps | 30 estables en calidad Baja; ≥20 en el 1 % más bajo |
| Luces dinámicas visibles | ≤3 con sombra (la linterna del jugador + 2 de enemigos); ≤10 sin sombra |
| Iluminación | Horneada (Lightmass) en el hospital y la casona; el parque, estático con Sky Light bajo |
| Texturas | 1024 como máximo (LOD bias en `Horror_Hospital`, que viene en 4K) |
| Draw calls | <1 200 |
| RAM del proceso | <3 GB |
| ZIP de la demo | ≤1,5 GB |

---

## 7. Hoja de ruta

> **Actualización del 12-11 (CTO):** no se publica el 13 ni el 16-11. Habrá testers privados alrededor del **26-11**, al cierre de E4, y la publicación en itch.io es el **10-12**.


Una build al día y 1–2 features por build. Cada una con su interruptor en `DA_Tuning` y su línea en la checklist.

| Etapa | Días | Contenido | Build |
|---|---|---|---|
| **E0 · Base** | 12–13-11 | Línea base de la v0.3.40. Kit (v0.3.41). itch.io restringido. | v0.3.41 |
| **E1 · Dos mapas** | 13–14-11 | `L_Hospital` + `L_Parque`, `BP_GI_MundoMagico`, `BP_TransicionNivel` con TV y VHS, Continuar por mapa. | v0.3.42 → candidata al 16-11 |
| **E2 · Núcleo de sistemas** | 15–18-11 | `BPI_Interactuable`, `AC_Interaccion`, `BP_PaginaNovela` + `DA_NovelaData`, `BP_HideSpot`, `BP_DoorLocked`, `AC_Ruido` con Physical Materials, `BP_DirectorMiedo`. | 0.4.0-a/b |
| **E3 · IA** | 18–21-11 | `BP_AI_Sombra` con Perception + BT (patrulla, investigar, persecución, atrapar), rutas spline, y el payaso como amenaza real en el parque. | 0.4.1 |
| **E4 · Hospital completo** | 21–26-11 | Vestido con `Horror_Hospital`, 2.º piso (negatoscopio), sótano y morgue, look verdoso, páginas 1–15. | 0.4.2 (puerta de calidad con testers) |
| **E5 · Casona** | 27-11 a 4-12 | Blockout → kit `UnrealBucket` + `SICKA`, superficies, apagón, álbumes, Sombra anciana, páginas 16–40. | 0.4.5 |
| **E6 · Parque y clímax** | 5–9-12 | Luz de sodio en 3 niveles, primitivas reemplazadas, gazebo, cordura, asedio final. | 0.4.9 |
| **E7 · Demo** | 10-12 | **v0.5.0.** Opción recomendada: la demo pública es el **Nivel 1 completo + el inicio de la Casona** (teaser). El juego completo es la meta de Steam. | 0.5.0 |

**Cámara 1ª/3ª:** entra en E2 como opción del menú, con su interruptor.
**Mezcla de audio y subtítulos:** en E2, con `WBP_Subtitulos` y `SC_Voz`, y el volumen de SFX en el menú.

---

## 8. Riesgos

| Riesgo | Mitigación |
|---|---|
| El alcance es demasiado para el 10-12 | La demo pública es el Nivel 1. La casona y el parque llegan en E5–E6 y pueden recortarse. |
| El MCP falla con Blueprints grandes | Blueprints chicos (componentes), como se acordó. El BP del jugador solo **pierde** lógica, no gana. |
| `OpenLevel` y la memoria con 8 GB | Un solo mapa cargado a la vez, que justamente baja la memoria. Se mide en cada build. |
| La IA no se puede probar en el PIE | `Probar.bat -ir=` en cada mapa y overlay `-dev` con el estado de la IA. |
| Se repiten los sustos | Todo pasa por el `BP_DirectorMiedo`. |

## 9. Decisiones del CTO
1. ✅ **3 niveles (Hospital → Casona → Parque): aprobados por el CTO el 11-11.**
2. ✅ **Demo pública del 10-12: el Nivel 1 (Hospital) completo, más un adelanto de la Casona. Lo decidió el CTO el 11-11.** La Casona y el Parque completos quedan para el juego completo (Steam).
3. ✅ **La 1ª persona es el MODO PRINCIPAL; la 3ª queda como opción del menú. Lo decidió el CTO el 11-11.** Entra en E2 con un interruptor en `DA_Tuning` (`Camara_PrimeraPorDefecto`). Las builds de itch.io del 13 y el 16-11 (v0.3.41 y v0.3.42) siguen en 3ª persona, igual que su página. La página de itch.io se actualiza a "primera persona" cuando salga la build con la cámara nueva.
