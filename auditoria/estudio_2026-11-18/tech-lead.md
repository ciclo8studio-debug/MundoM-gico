# @tech-lead · Estudio 18-11-2026: visión, arquitectura, iGPU, build y alcance

## 1. Resumen
- Los sistemas base ya están en UE (GI con flags guardados, AC_Ruido, BP_AI_Sombra, HideSpot, DoorLocked, Director, linterna con los parámetros del HTML). Pero **el contenido del Nivel 1 todavía no existe**: no hay sótano ni morgue, ni negatoscopio, `DA_NovelaData` ni `L_Casona`.
- El HTML se siente mejor por **cómo guía y responde al jugador**, no por el arte: objetivos con pistas, calidad automática, linterna con inercia, aliento y FOV al correr, y un drone que sigue la tensión. Todo eso se puede pasar a BP sin Tick y casi sin costo de GPU.
- El riesgo principal es el **calendario**. E4 (21–26-11) concentra el hospital completo, y muchas versiones (de la 0.3.45 a la 0.3.57) se marcaron "sin build" y nunca se jugaron en el `.exe`.
- **Salud del proyecto: 6/10.** El núcleo y el pipeline son sólidos; el contenido y la validación van atrasados.

## 2. Hallazgos por agente
**creative-director**
- Hay dos visiones de demo cruzadas. `ARQUITECTURA_3_NIVELES.md` §9 dice "Hospital + teaser Casona", pero el cook (`Construir_Version.ps1`, `-map=L_Hospital+L_Parque`) y el plan del tráiler incluyen el parque. Hay que fijar un solo arco para el tester.
- El HTML tiene una regla de tono que UE no aplica: **el silencio sube a drone según la tensión** (`droneTick`, 48→62 Hz con filtro 160→420 Hz). En UE la tensión del Director solo se ve en el panel `-dev`.
- El look "verdoso clínico" (§4 Nivel 1) solo existe como tinte global de post-proceso (v0.3.36 aprox.). Falta un LUT por mapa.

**technical-director**
- **Conflicto de arquitectura.** La §6 exige "luz horneada (Lightmass)" en el hospital, pero hornear necesita Static Lighting, es decir, un cambio de renderer (Perfil A), que está vetado sin el OK del CTO. Así como está, la meta de §6 no se puede cumplir.
- El BP del jugador sigue siendo monolítico (unos 10 MB) y el plan de §5 de "restarle lógica" no avanzó: `ActualizarZona`, `RevisarSalida` y `RevisarFinDemo` siguen con coordenadas. Cada edición por MCP sobre ese BP es el punto de falla más caro.
- v0.3.58 agregó una función llamada **desde el Tick del menú** (`MoverLuz`), y eso viola la regla de no agregar Event Tick. Es barata, pero hay que pasarla a un timer de 0,033 s para no sentar precedente.

**producer**
- E2 y E3 cerraron sin `BPI_Interactuable`, `AC_Interaccion`, `DA_NovelaData` ni un BT real. E4 hereda esa deuda, más 2.º piso, sótano, negatoscopio y páginas 1–15, en 6 días. **No cabe.**
- La versión se quedó en 0.3.x mientras la hoja de ruta habla de 0.4.x. Las etapas E5–E7 (Casona, parque y clímax) suponen una velocidad que hoy no hay.

**lead-programmer**
- Los sistemas nuevos usan dos interfaces de interacción en paralelo: la `BPI_Interaction` del template y la `BPI_Interactuable` del plan, que no existe. Hay que decidir una sola antes de E4; recomiendo **quedarse con `BPI_Interaction`**.
- Los objetivos están implícitos en el BP del jugador y no existe una función `Objetivo()` como la del HTML (líneas 1915–1940 aprox.). Sin ella, el tester se pierde y reporta "bugs" que en realidad son de diseño.

**unreal-specialist**
- La inercia de la linterna sale gratis: se pone la `SpotLight` bajo un `SpringArm` de 0 cm con `bEnableCameraRotationLag` (velocidad de 8–10). Eso replica el `lagY/lagP` del HTML sin Tick.
- La calidad automática se puede hacer con `GameUserSettings` (`RunHardwareBenchmark` al primer arranque y `SetOverallScalabilityLevel`) más un timer que promedie `GetWorldDeltaSeconds`.

**engine-programmer**
- El HTML apaga las sombras de la linterna en Baja (`TIERS[3].sh=0`) y usa 512 en Media. En UE, la v0.3.55 las forzó siempre activas y la v0.3.58 tuvo que parcharlas por mapa (`Linterna_SombrasParque`). La regla correcta es **por nivel de calidad, no por mapa**.
- `OpenLevel` entre mapas libera memoria. Falta medir el pico de RAM al cargar `L_Hospital` cuando llegue el kit `Horror_Hospital`, que viene en 4K y está limitado por `DefaultDeviceProfiles.ini`.

**performance-analyst**
- No hay serie de FPS por zona: `Probar.bat` muestra los fps, pero nadie los anota. Para el tester del 26-11 hace falta una tabla de 7 zonas × build.
- Presupuesto §6: el parque tiene unos 1.400 actores, contra un límite de menos de 1.200 draw calls. El hospital con el kit nuevo tiene que medirse antes y después (`stat unit` en `-dev`).

**devops-engineer**
- La build v0.3.60 ocupa 1,5 GB, con 1,1 GB de Paks. El ZIP queda justo en el tope de 1,5 GB, y a eso se suman el kit hospital y la Casona. **Falta `DirectoriesToNeverCook`** (pendiente desde la tarea 1.11 y el buzón del BOARD).
- `Build/` guarda 11 builds de unos 1,5 GB cada una (unos 16 GB en un disco de un PC de 8 GB). Conviene dejar solo las 3 últimas más la publicada.
- No hay una puerta de humo automática: `Construir_Version.ps1` devuelve "BUILD OK" aunque el juego no arranque. Hay que agregar un arranque de 60 s con `-ir=hospital` y revisar el log en busca de `Fatal`.

## 3. Brechas HTML → Unreal (mi área)
| Qué tiene el HTML | Estado en UE | Cómo replicarlo en BP |
|---|---|---|
| Calidad automática (`perfTick`: si los fps bajan de 36 durante 2 s, baja un nivel) | ❌ Solo Baja fija y `t.MaxFPS=30` | Timer de 0,25 s en `BP_GestorNivel` que guarda 8 muestras de DeltaSeconds. Si el promedio da menos de 27 fps durante 6 s, se ejecuta `SetOverallScalabilityLevel(-1)` y `ScreenPercentage` 70, y aparece el mensaje "Calidad automática". |
| Sombras de linterna según la calidad (Baja 0, Media 512, Alta 1024) | 🟡 Por mapa (`CalidadExtra`) | En `CalidadExtra`: `CastShadows = Calidad ≥ Media`, sin mirar el mapa. |
| Objetivo con pista (H o gatillo), pista automática a los 180 s, flecha con metros | ❌ | `DA_Objetivos` (Flag requerido, Texto, Tag del destino). La función `ObjetivoActual` recorre los flags del GI. `WBP_Objetivo` muestra el texto y la flecha, y se actualiza con un timer de 0,25 s. |
| Linterna con inercia y balanceo al caminar | ❌ (parámetros de luz ✅ en v0.3.60) | `SpringArm` con lag de rotación + `Timeline` de balanceo que arranca al empezar a moverse. |
| FOV 70→80 al correr, aliento (gasta 24/s, recupera 22/12, cansada hasta 35) con barra | 🟡 El aliento solo cuenta al esconderse | `Timeline` de FOV en Correr/Soltar. Aliento con timer de 0,1 s y barra en el HUD. |
| Drone de tensión (sube de frecuencia con la tensión) | ❌ El Director tiene la tensión, pero no suena | 2 loops (`SND_Drone_A/B`) con `SetVolume`/`SetPitch` en el timer del Director: volumen = tensión², tono de 1 a 1,3. |
| Lluvia que cambia entre interior y exterior | 🟡 | `BP_VolumenZona` con `Interior`: al entrar, `SetVolume` de la lluvia baja a 0,5 con un fundido de 0,7 s. |
| Guardado cada 8 s y al pausar | ✅ Cada 30 s y al pausar | Suficiente. Falta el campo `Version` en `SG_Partida` (§2). |

## 4. Top 6 mejoras priorizadas
| # | Mejora | Impacto | Esfuerzo | Riesgo iGPU | Interruptor en `DA_Tuning` | Responsable |
|---|---|:-:|:-:|:-:|---|---|
| 1 | Sombras de linterna por calidad + calidad automática (con mensaje) | 5 | S | Lo **baja** | `Calidad_Auto`, `Linterna_SombraMinCalidad` (1 = Media) | engine-programmer + performance-analyst |
| 2 | Objetivos y pistas: `DA_Objetivos`, pista automática a los 180 s, flecha | 5 | M | Nulo (UMG) | `Objetivos_Activo`, `Pista_AutoSeg` (180), `Pista_Flecha` | lead-programmer (con ue-umg) |
| 3 | Linterna con inercia + FOV al correr + aliento como en el HTML | 4 | S | Nulo | `Linterna_Inercia` (8), `FOV_Correr` (80), `Aliento_Activo` | unreal-specialist |
| 4 | Drone de tensión ligado a `BP_DirectorMiedo` | 4 | S | Nulo | `Drone_Activo`, `Drone_VolMax` (0,6) | creative-director (con audio) |
| 5 | Release: `DirectoriesToNeverCook`, ZIP medido en cada build, prueba de humo de 60 s en `Construir_Version`, dejar solo 3 builds | 4 | S | — | — (pipeline) | devops-engineer |
| 6 | Teaser de Casona cerrado: 1 vestíbulo + apagón + "Continuará" (sin Sombra anciana ni álbumes) | 4 | M | Medio (mapa nuevo, medir la RAM) | `Casona_Teaser` | producer + creative-director |

**Cronograma propuesto (1–2 features por build):**
- **E4 (19–26-11):**
  - 19-11: build con las mejoras 1 y 3.
  - 20-11: mejora 2.
  - 21–24-11: sótano y morgue con el kit, y el negatoscopio **en planta baja**.
  - 24-11: congelar las features.
  - 25-11: build para testers con la tabla de fps.
- **E5 (27-11 a 4-12):**
  - 27–29-11: arreglos del feedback y mejora 4.
  - 30-11 a 3-12: mejora 6.
- **E6 (5–9-12):** sin features nuevas. RC el 8-12, mejora 5 y ZIP de 1,5 GB o menos.
- **E7 (10-12):** se publica la v0.5.0.

## 5. Decisiones que necesita el CTO
1. **¿El parque entra en la build pública del 10-12?** Recomiendo dejarlo **fuera del cook** de la demo (hospital + teaser de Casona). Así el ZIP baja unos 400 MB, se evita el riesgo de `GPUCrash` en el parque y se cumple la §9. El parque queda para el tráiler y Steam.
2. **Luz horneada (§6):** necesita el Perfil A, que está vetado. Recomiendo **no hornear para la demo**: luces sin sombra, fluorescentes emisivos y sombras de linterna por calidad. El horneado se revisa después del 10-12.
3. **Recorte de E4:** aprobar que el 2.º piso salga del Nivel 1 (el negatoscopio pasa a planta baja o al sótano) y que la Casona sea solo un teaser de 5 minutos. Sin este recorte, el tester del 26-11 recibe un hospital a medias.
