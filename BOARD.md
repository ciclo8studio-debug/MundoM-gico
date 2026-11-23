# 🎛️ CONTROL BOARD — THRILLER / SURVIVAL HORROR (UE 5.8.4)

> Proyecto: **Mundo Mágico: Turno Nocturno** · Unreal: `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` · Mapa principal: `/Game/MundoMagico/Maps/L_Pabellon4`
> Productor Ejecutivo / CTO: Francisco · Coordinación: Claude
> Comunicación interna solo vía este archivo (sin correos ni notificaciones externas).
> 🗺️ **Plan de trabajo hasta la demo:** [`PLAN_DE_TRABAJO.md`](PLAN_DE_TRABAJO.md). Demo v0.5.0 el **10-12-2026** (plan acelerado, sprints de 1 semana), decisión de hardware el 17-11-2026.

## 🎯 Estado actual (2026-11-11)
- 🧪 **A9 en marcha (23-11):** una sola checklist de humo para la RC1 en [`auditoria/humo/rc1_2026-11-24.md`](auditoria/humo/rc1_2026-11-24.md) (reemplaza v0.3.84 y v0.3.92); `Probar.bat` suma la opción **8 = Casona**. Estado del plan en [`auditoria/estudio_2026-11-23/estado_plan.md`](auditoria/estudio_2026-11-23/estado_plan.md): Sprint A 9/10 construido sin humo formal; falta decidir el G0 del parque y congelar la Casona.
- 📨 **Kit de testers RC1 (23-11):** [`auditoria/kit_testers_2026-11-26.md`](auditoria/kit_testers_2026-11-26.md) (mensaje, 10 preguntas, métricas y fechas; reemplaza al formulario del 13-11). Faltan `[LINK]`, `[CLAVE]` y las teclas de agacharse y correr. Panel actualizado al 23-11: https://claude.ai/artifact/2jHerSAK3SJcb9rZeii2Nd (copia local sin versionar en `auditoria/estudio_2026-11-23/panel.html`).
- 🅱️ **Plan B activado (23-11, v0.3.109):** el G0 del parque no pasó (cuelgue de GPU al viajar parque → Casona, sesión 2). El CTO decidió "Plan B ya": `Demo_SinParque` = verdadero. La demo es hospital → glitch y TV → pantalla FIN. Detalle en `auditoria/humo/g0_parque_2026-11-23.md` y `CHANGELOG.md`. Pendiente de decisión: si el adelanto de la Casona entra a la demo.
- 🎯 **Camino al 10/10 (23-11):** cada departamento tiene 5 criterios con prueba en [`auditoria/estudio_2026-11-23/camino_10_10.md`](auditoria/estudio_2026-11-23/camino_10_10.md). Puntaje hoy (criterios T1, T2 y W5 ajustados al Plan B): tech-lead 6,0 · gameplay 4,0 · world-builder 6,0 · ux-toolsmith 3,0 · qa-publisher 3,0. Builds: 40 borradas (85 a 12 GB); ZIP de la v0.3.87 = 1,21 GB (165 s). Lo que más sube: una prueba de humo jugada con la RC1 y el G0 del parque. Herramientas nuevas: `herramientas/Limpiar_Builds.ps1` (lista 73 GB a liberar, borra solo con `-Borrar` y tu OK) y `herramientas/Empaquetar_ZIP.ps1`.
- 📅 **Nuevo calendario (decisión del CTO, 12-11):** **se cancelan el itch.io restringido del 13-11 y la publicación del 16-11.** Habrá testers privados (3–5) alrededor del **26-11**, con el hospital AAA terminado (cierre de E4). El **lanzamiento público en itch.io es el 10-12**, con la v0.5.0 y el Nivel 1 completo. La v0.3.42 queda como respaldo. Los documentos de itch.io siguen vigentes para esa fecha.
- 🏗️ **Arquitectura en 3 niveles** (System Prompt Maestro del CTO): ver [`ARQUITECTURA_3_NIVELES.md`](ARQUITECTURA_3_NIVELES.md). La secuencia es Hospital → Casona → Parque, con `BP_GI_MundoMagico`, `BPI_Interactuable`, `DA_NovelaData`, IA Sombra con Perception y BT, escondites, puertas con código, superficies sonoras, cordura y cámara 1ª/3ª. ✅ **El CTO aprobó los 3 niveles el 11-11.** ✅ **Demo del 10-12: el Nivel 1 completo.** ✅ **La 1ª persona es el modo principal** y la 3ª queda como opción.
- 🎮 **Reunión "Replicar el HTML completo" (18-11):** el equipo recorrió el HTML ([recorrido](auditoria/html_completo/RECORRIDO_HTML.md)) → [acta](docs/historico/REUNION_2026-11-18_ReplicaHTML.md). El parque vuelve (3 actos: hospital → parque → persecución y expediente). ⚠️ Bloqueo n.º 1 **sigue abierto**: la v0.3.68 (sin sombras) también se cae; D9 y D10 se cayeron según los reportes. Build Development con logs para encontrar la causa raíz. Plan reordenado: hospital y datos primero. Gate G0 el 22-11. ✅ El CTO aprobó las 9 decisiones (18-11). Build v0.3.67 para el diagnóstico P0.
- 🗓️ **Reunión de cierre (18-11, noche):** [acta](docs/historico/REUNION_2026-11-18_Cierre.md). Cierre del parque resuelto en v0.3.69 (H2); v0.3.73 (parque arranca) y v0.3.75 (Acto 3) **sin probar**; FPS del parque en Shipping desconocido. El CTO prueba 5 puntos el 19-11; el producer decide G0 el 20-11 a las 20:00 (parque o Plan B ampliado). rc1 el 25-11, demo el 10-12.
- 🔧 **Reingeniería (18-11):** los 5 super-agentes rediseñaron su área → [`REINGENIERIA_2026-11-18.md`](REINGENIERIA_2026-11-18.md) (detalle en `auditoria/reingenieria_2026-11-18/`). Hoja de ruta: v0.3.62–v0.3.67 → rc1 el 25-11 → Sprint B → v0.5.0 el 10-12. ✅ El CTO aprobó las 8 decisiones (18-11): BP por piezas `Arq_*`, demo sin parque ni payaso, cook sin SM6, canon de Isidora (leucemia + miocardiopatía, 03:33 del 13/03/1998), vida de 4 golpes, clímax de la llave, subtítulos activos y música de 3 capas, gate firme y String Tables.
- 🏢 **Estudio ampliado (18-11):** 31 agentes de Claude Code Game Studios bajo los 5 super-agentes ([organigrama](ESTUDIO_AGENTES.md)). Evaluación completa en [`auditoria/estudio_2026-11-18/`](auditoria/estudio_2026-11-18/) → [plan consolidado](docs/historico/PLAN_ESTUDIO_2026-11-18.md): Sprint A (A1–A10, build de testers el 25-11), Sprint B (Nivel 1, freeze el 1-12), release el 10-12. ✅ El CTO aprobó el Sprint A con las recomendaciones (18-11). Hechos según el CHANGELOG: A1 + A10 (v0.3.61), A6 + A2 (v0.3.62), A3 (v0.3.64), A5 + A7 (v0.3.65), A4 (v0.3.74); la mayoría sin build ni humo. El 20-11 se amplió A6 (v0.3.82: `Calidad_Auto` quita la dependencia del mapa; la detección por GPU se descartó porque el CTO no autorizó Perfil A). A8 revisada el 20-11 y dada por hecha: contador "PÁGINAS x/15" (v0.3.63, se apaga con `Novela_Texto`), pantalla FIN (v0.3.66, solo en `Demo_SinParque`) y "tic" de UI en el menú (`MenuArriba` ya reproduce `SW_Note_Next`). Los interruptores `UI_ContadorPaginas` y `UI_FinalDemo` se crearon igual el 20-11 (v0.3.83, por pedido del CTO), sin probar en build. 🛠️ **v0.3.84 (20-11):** TSR solo en calidad Alta (el perfil de GPU mostró 315 de 363 ms por cuadro en TSR; hospital de 3 a ~30 FPS en Baja). Build v0.3.84 lista (Shipping y `_dev`); panel del estudio actualizado. **A9:** checklist lista en [`auditoria/humo/v0.3.84.md`](auditoria/humo/v0.3.84.md), falta que el CTO la ejecute. Pendiente: stinger al recoger una página y final según páginas con llamado a itch.io (necesita textos del CTO). Error sin reproducir: al salir del pasillo no cambia el mapa. **Logs de `v0.3.84_dev` (21-11):** sin cierres ni errores de GPU; spam de ~46.000 avisos de script en la Casona y el Parque (listas vacías de `Munecas`, `Payasos` y etiquetas en `ActualizarPosObjetivo` y `ActualizarBateria`, desde el tick del jugador) y `SetTimer` con tiempo cero en `LimpiarMensaje`. **B1 arreglado el 21-11 en v0.3.85** (guardas de lista vacía en `ActualizarPosObjetivo`, `ActualizarCerca` y `ParpadeoParque`; compila, **sin build ni prueba**). Quedan `Munecas` y B2 (detalle en la checklist de humo). G0 del parque (20-11, 20:00) sin decisión registrada.
- 🛡️ **Reunión CTO + @tech-lead** ([acta](docs/historico/REUNION_2026-11-11_TechLead.md)): @tech-lead queda autorizado a proponer mejoras AAA. ✅ **Manda `MUNDO_MÁGICO` con itch.io.** `MundoMagico2` queda en pausa. ✅ **Aprobado:** el hospital y el parque pasan a 2 mapas, unidos por la transición glitch → TV (13-11, v0.3.42).
- ✅ **El CTO descartó la refundación.** Se sigue el [plan de mejora rápida](docs/historico/PLAN_MEJORA_RAPIDA.md), que combina lo mejor de las dos reuniones. Fase 1, el kit del CTO, en curso.
- 🔁 *(Archivada)* **Refundación propuesta (11-11, noche):** ver el [acta](docs/historico/REUNION_2026-11-11_Refundacion.md). Los 5 agentes votan la opción **A**: mapa nuevo y núcleo nuevo en `/Game/MM`, dentro del mismo proyecto. La v0.3.40 queda congelada como respaldo. Hay 5 decisiones pendientes del CTO.
- 🧭 **Reunión del plan de mejora (11-11, noche):** ver el [acta](docs/historico/REUNION_2026-11-11_PlanMejora.md). Se congelan las features hasta validar la v0.3.40. Viene un kit de afinación para el CTO (`DA_Tuning`, saltos de prueba y guía), el hospital con `Horror_Hospital` y un Director de miedo. ✅ **itch.io:** el 13-11 se abre restringido para testers y el 16-11 se publica (decisión del CTO). ✅ Después del kit va primero el **hospital con `Horror_Hospital`**. Quedan 2 decisiones: el Perfil A parcial y borrar builds viejas.
- 🆕 **11-11 noche · build v0.3.40 lista** (`Build0.3.40`, 0 errores en el cook):
  - **Qué trae:** hospital AAA (balizas rojas, alarma, cuerpos y papeles de `SM_Notes`), sustos en el teatro y la noria, payaso acosador a distancia, Isidora y latido más notorios, y descompresión tras el glitch.
  - **Mañana (12-11):** el CTO la prueba con [`auditoria/checklist_v0.3.40.md`](auditoria/checklist_v0.3.40.md) y decide qué se publica: **v0.3.40**, o la v0.3.35 si algo falla.
  - **Excepción a la regla de builds:** el CTO pidió esta segunda build del día.
  - **Pendiente:** el slider de SFX, que toca el menú y el guardado, se hace después de validar la build.
- Hubo **reunión de avance** con los 5 super-agentes: ver el [acta](docs/historico/REUNION_2026-11-11.md).
- Del 10 al 11-11 entraron 33 builds de prueba (v0.3.2 a v0.3.34) con mejoras visuales, de audio y de mecánicas. Están en el CHANGELOG, pero **casi nada se ha probado en el `.exe`**.
- **Para el 13-11:** se recomienda publicar **v0.3.1** y corregir el borrador de itch.io, que promete contenido de la v0.4.
- **Escondites:** desde la v0.3.2 los clósets volvieron a `Hospital/Escondites` (`BP_Hiding_Door` arreglado) y están **activos**. Nadie los ha probado.
- **Decisiones pendientes del CTO:** las 5 del acta (qué se publica, playtest, ritmo de builds, prioridad del sprint y autorización de IMAGINARIO).
- ✅ **IMAGINARIO:** el CTO confirmó el 11-11 que ya está la autorización para usar el material. Se puede usar el arte de la novela en la portada y el teaser.
- 🧪 **Playtest de v0.3.34** en curso con [`auditoria/checklist_v0.3.34.md`](auditoria/checklist_v0.3.34.md).
- ℹ️ El 13-11 se abre en **itch.io** solo para testers y el 16-11 pasa a ser público. No es Steam: Steam requiere la cuenta Steamworks, la página de tienda y una revisión de 1 a 2 semanas.

## 🗂️ Estado anterior (2026-11-09, noche)
**Demo corta v0.3.1: lista para publicar.** El CTO jugó 32 minutos seguidos sin cierres.
Validado por el CTO:
- Controles (✕ salta, □ interactúa).
- Entrada al parque.
- Sombra bajo el letrero al llegar.
- 4 interruptores en cualquier orden.
- Luces y cinemática en órbita.
- Audio de terror priorizado y lluvia sutil.

Último ajuste: los 3 clósets-escondite (`ESC_Armario_*`) quedan fuera de la demo porque el jugador se atascaba dentro. Se movieron a `Parque/Demo/Sobrantes`.

### ✅ Mejoras aprobadas por el CTO en la reunión del 2026-11-09 ([acta](docs/historico/REUNION_2026-11-09.md))
- **S2 (desde ya, sin refactors grandes antes del 13-11):**
  - ✅ **Versionado (hecho 2026-11-09):** el proyecto Unreal (contenido propio, unos 200 MB, 403 assets en LFS) está en la rama `unreal` del repo privado. Para subir cambios: `herramientas/Versionar_Unreal.bat`.
  - Guardado con puntos de control.
  - Escondites + contener la respiración.
  - Sonido espacial (reverb por zona y oclusión).
- **S3:**
  - 30 fps en notebooks modestos (según el hardware del 17-11).
  - Música y silencios según la amenaza.
  - Voces y sonidos de época.
- **S3–S4:** Payaso acosador con IA nueva.
- ❌ La identidad chilena de 1998 como eje quedó descartada.

### Lo que queda (en orden)
1. **13-11 · Publicar la demo corta** (CTO): subir `Build/v0.3.1/Windows` comprimido a itch.io con el borrador de [`auditoria/itchio_pagina_demo.md`](auditoria/itchio_pagina_demo.md). Faltan las capturas.
2. **14 al 16-11 · Pulido post-demo**:
   - Arreglar la mecánica de esconderse (`BP_Hiding_Door`: salida del clóset) y devolver los clósets.
   - 20–30 s de descompresión después del glitch.
3. **17-11 · Decisión de hardware (Camino A/B)** → Sprint 2:
   - Separar el guardado (`SG_Opciones`/`SG_Partida`).
   - Jugador y GameMode a `Core`.
   - `AC_Linterna`.
   - Piloto de Behavior Tree.
   - Slider de SFX.
4. **Demo completa (v0.4.x)**: validar en el `.exe` el Carrusel, la Casa y la huida (ya implementados). Volver `DemoCorta=false`.
5. **10-12 · Demo completa v0.5.0** (Sprints 3 a 5 de `PLAN_DE_TRABAJO.md`).

## 🟢 Estado de los 5 Super-Agentes
- 🛡️ `@tech-lead`      | Estado: ✅ Sprint 1 · ⏳ Sprint 2 espera el hardware (17-11)
- 🕹️ `@gameplay-engineer` | Estado: ✅ controles, interruptores, Carrusel y Casa · ⏳ esconderse + BT
- 🏚️ `@world-builder`     | Estado: ✅ entrada, sombra, luces, cinemática · ⏳ descompresión post-glitch
- 🎨 `@ux-toolsmith`     | Estado: ✅ audio de terror, lluvia, fuentes · ⏳ slider de SFX
- 🧪 `@qa-publisher`     | Estado: ✅ v0.3.1 validada por el CTO · ⏳ publicar en itch.io el 13-11

---

## 📋 Tablero de Tareas de la Auditoría Inicial

| Tarea / Diagnóstico | Agente Asignado | Estado | Directorio / Archivo Afectado | Requiere Aprobación CTO? |
| :--- | :---: | :---: | :--- | :---: |
| Auditoría de estructura de carpetas y dependencias | `@tech-lead` | ✅ Completada | [`auditoria/tech-lead.md`](auditoria/tech-lead.md) | ❌ No |
| Diagnóstico de Player Controller y Linterna | `@gameplay-engineer` | ✅ Completada | [`auditoria/gameplay-engineer.md`](auditoria/gameplay-engineer.md) | ❌ No |
| Evaluación de mapa de prueba y tensión visual | `@world-builder` | ✅ Completada | [`auditoria/world-builder.md`](auditoria/world-builder.md) | ❌ No |
| Revisión de HUD y componentes de Audio 3D | `@ux-toolsmith` | ✅ Completada | [`auditoria/ux-toolsmith.md`](auditoria/ux-toolsmith.md) | ❌ No |
| Inspección de warnings de compilación y linters | `@qa-publisher` | ✅ Completada | [`auditoria/qa-publisher.md`](auditoria/qa-publisher.md) · [`docs/historico/AUDIT_REPORT.md`](docs/historico/AUDIT_REPORT.md) | ❌ No |

---

## 🚀 Plan a Detalle del Sprint 1: Cimientos de Tensión e Interacción
> *Propuesto por `@tech-lead` sobre los hallazgos de `docs/historico/AUDIT_REPORT.md`. Estado: ✅ **Aprobado por el CTO (2026-11-09)** · 🟡 En ejecución.*

### 📊 Avance del Sprint 1 (2026-11-09)
| # | Estado | Resultado |
|:-:|:-:|---|
| 1.1 | ✅ | `herramientas/Respaldar_Proyecto.bat`. Primer respaldo creado: `Escritorio\MundoMagico_Respaldos\MM_20261109_1159.zip` (352 archivos, 150 MB). 20 `.tmp` enviados a la Papelera. |
| 1.2 | ✅ | SkyAtmosphere y VolumetricCloud ocultos. Sombras apagadas en 29 luces decorativas (LUZG_, PQ_Luz, FAR_). `r.VolumetricFog.GridPixelSize=16` y `GridSizeZ=64` en `DefaultEngine.ini` (respaldo `.antes_sprint1`). |
| 1.3 | 🟡 | **Build v0.2.0 OK** (Shipping Win64, cook con 0 errores y 0 warnings, 13 min, 3,2 GB) en `Build/v0.2.0/Windows/MundoMagico.exe`. Log: `Build_log_v0.2.0.txt`. **Prueba de humo 1 (CTO):** 8 min sin cierres de GPU. Se encontró un atasco por la camilla en la sala inicial; ya se corrigió y se reconstruyó (0 errores). **Falta la prueba de humo 2.** |
| 1.4 | ✅ | `CHANGELOG.md`. `DefaultGame.ini`: `ProjectVersion=0.2.0`, `ProjectName`, `CompanyName` (respaldo `.antes_sprint1`). |
| 1.5 | ✅ | `ATT_Corto` (2/15 m), `ATT_Medio` (5/35 m) y `ATT_Largo` (15/90 m) en `MundoMagico/Audio/Atenuacion`, con filtro por distancia. Asignados a los 12 `SND_*` y a 7 `SFX`. `BP_SustoPasillo`: 4 `PlaySound2D` → `PlaySoundAtLocation` en la posición de la criatura. |
| 1.6 | ✅ | Respaldo `DroidSansFallback`, la fuente del motor que trae ▲ ✕ ↑ ↓ ← →, en las 4 fuentes del juego. |
| 1.7 | ✅ | Revisión del BP del jugador: el único `PrintString` está en `LogEstado` y no se llama (en Shipping se elimina). Los índices fijos son lecturas de CSV de opciones con formato conocido, no bugs. `BP_ThirdPersonCharacter`, `BP_Enemigo` y `BP_SustoPasillo` compilan con *warnings as errors*: 0 avisos. |
| 1.8 | 🟡 | La botella emite `MakeNoise` (25 m, tag `Botella`) en `SonidoBotella`. `BP_Enemigo` sigue con su lectura actual, que funciona; la escucha por percepción entra con el Behavior Tree (ver 1.14). |
| 1.9 | 🟡 | Luz en 3 zonas: entrada segura (sin cambios), avenida en penumbra (LUZG_X 6→3, LUZG_L 4→2) y laterales oscuros (Carrusel 70→25, Noria 90→30). Oso Willy al lateral oeste (-4700,-10300), León al lateral este (7300,-11300). **Pendiente:** descompresión tras el glitch (necesita playtest). |
| 1.10 | ✅ | Aviso de escala 0: las mallas de la noria (giran con colisión) quedaron sin colisión. El aviso de CrowdManager es inofensivo: aparece porque el NavMesh dinámico aún no se construye al arrancar. |
| 1.11 | ✅ | 7 packs archivados (6.078 MB) en `Documents\Unreal Projects\MUNDO_MÁGICO_PacksSinUsar`: UndergroundTunnel, SciFi_Beast06, PunkGirl_Bundle, RoninGirl, DynamicFalling, FreeSurvivalIconsKG y Variant_Platforming. Se restauran moviéndolos de vuelta con Unreal cerrado. Respaldo previo: `MM_20261109_1219.zip`. |
| 1.12 | ⏸️ | **Postergada a Sprint 2:** separar el guardado toca la persistencia del jugador, y en este PC no se puede validar sin build. Se hará después de probar v0.2.0. |
| 1.13 | ✅ | 10 renombres con redirectores: 5 `SM_SM_*` → `SM_*`; audio `AMB_Hospital`, `AMB_Parque`, `AMB_T_Hospital`, `MUS_Menu`, `MUS_Persecucion`. Los scripts del repo se actualizaron. Pendientes de decisión del CTO: `SKM_Isidora2/3` y `SM_Terr_Seco1/2`. |
| 1.14 | ✅ | [`auditoria/diseno_bt_enemigo.md`](auditoria/diseno_bt_enemigo.md) |

**Objetivo:** dejar el recorrido **Sala → Pasillo → Glitch → Entrada del parque** estable, con tensión audible y probado de punta a punta **en un build empaquetado** (el editor y PIE no sirven para testear en el PC actual).

**Restricción de plataforma:** iGPU Intel Iris Plus con 8 GB. Todo cambio se valida por compilación de BP sin errores, logs limpios y una prueba de humo en el `.exe`.

### P0 — Seguridad y testeabilidad (bloqueantes)
| # | Tarea | Agente | Archivo / Zona | Aprobación CTO |
|:-:|---|:-:|---|:-:|
| 1.1 | Respaldo versionado del proyecto Unreal: script `Respaldar_Proyecto.ps1` (Content/MundoMagico, ThirdPerson, Config y .uproject a un zip con fecha) y limpieza de los `.tmp` de `Saved/` | `@tech-lead` | `herramientas/`, `Saved/` | ✅ Sí |
| 1.2 | Presupuesto de render para la iGPU en el mapa: apagar VolumetricCloud y SkyAtmosphere nocturnos, niebla 16/64, sombras off en luces decorativas (LUZG_, NEON_, FAR_) | `@world-builder` | `L_Pabellon4` | ✅ Sí |
| 1.3 | Build empaquetado **v0.2.0** con los arreglos de hoy y prueba de humo de 10 min en el `.exe` (sin editor) | `@qa-publisher` | `C:\MM_Build` | ✅ Sí (genera `.exe`) |
| 1.4 | Crear `CHANGELOG.md` (v0.1.0 = build del 02-11; v0.2.0 = arreglos del 09-11) y `ProjectVersion` en `DefaultGame.ini` | `@qa-publisher` | raíz, `Config/DefaultGame.ini` | ❌ No |

### P1 — Tensión e interacción
| # | Tarea | Agente | Archivo / Zona | Aprobación CTO |
|:-:|---|:-:|---|:-:|
| 1.5 | Audio 3D: crear `ATT_Corto` / `ATT_Medio` / `ATT_Largo` y asignarlos a los `SND_*` y `SFX_T_*`; pasar `BP_SustoPasillo` de `PlaySound2D` a `PlaySoundAtLocation` | `@ux-toolsmith` | `MundoMagico/Audio`, `BP_SustoPasillo` | ❌ No |
| 1.6 | Fuente de respaldo con ▲ ✕ ↓ ↑ dentro del proyecto (o reemplazar esos símbolos por texto o íconos) | `@ux-toolsmith` | `Content/Fuentes`, `MundoMagico/UI` | ❌ No |
| 1.7 | Higiene del BP del jugador: quitar nodos de depuración e índices fijos, compilar con 0 warnings (con respaldo previo de 1.1) | `@gameplay-engineer` | `BP_ThirdPersonCharacter` | ✅ Sí (toca el BP central) |
| 1.8 | Ruido real: la botella emite `MakeNoise` al impactar y `BP_Enemigo` escucha con `ReportNoiseEvent` o PawnSensing, sin leer variables del jugador | `@gameplay-engineer` | `BP_Enemigo`, botella | ❌ No |
| 1.9 | Lenguaje de luz en 3 zonas (segura, penumbra, oscuridad), 20–30 s de descompresión tras el glitch, mover 2 de los 4 enemigos a los laterales | `@world-builder` | `L_Pabellon4` | ✅ Sí (cambia el diseño) |
| 1.10 | Arreglar NavMesh/CrowdManager y la escala 0 de `BP_Giratorio` | `@world-builder` | `L_Pabellon4` | ❌ No |

### P2 — Deuda técnica (si queda capacidad)
| # | Tarea | Agente | Archivo / Zona | Aprobación CTO |
|:-:|---|:-:|---|:-:|
| 1.11 | Archivar fuera de `Content/` los packs sin referencias (UndergroundTunnel y otros, unos 6,3 GB) | `@tech-lead` | `Content/` | ✅ Sí (mueve assets) |
| 1.12 | Separar `SG_Opciones` de `SG_Partida`, con campo de versión | `@tech-lead` | `MundoMagico/Blueprints` | ✅ Sí |
| 1.13 | Corregir las 13 violaciones de nomenclatura (con fix-up de redirectors) | `@qa-publisher` | `Content/MundoMagico` | ❌ No |
| 1.14 | Diseño (documento, sin migrar aún) del Behavior Tree del enemigo, tomando como base `BT_Ai_Enemy` de Horror_Template | `@gameplay-engineer` | `auditoria/` | ❌ No |

**Definición de terminado:** cada tarea compila sin errores, el log de la sesión no muestra nuevos warnings de script, y el recorrido Sala → Parque se completa en el build v0.2.0 sin cierres.

**Fuera del Sprint 1 (backlog):** dividir el BP del jugador en componentes (Linterna, Interacción, UI); migrar la IA a Behavior Tree; StringTables y localización; migrar los ~40 assets usados de Horror_Hospital (unos 18 GB recuperables); convertir en eventos los sustos que hoy son solo texto.

---

## 🎯 Hito: DEMO CORTA v0.3.0 (13-11-2026)
| Tarea | Agente | Estado |
|---|---|:-:|
| Calidad Baja por defecto en el primer arranque | `@ux-toolsmith` | ✅ |
| Bloqueos de zonas fuera de la demo (6 rejas y conos) | `@world-builder` | ✅ |
| Fin de la demo frente al castillo (`RevisarFinDemo` → pantalla final) | `@gameplay-engineer` | ✅ |
| Checklist de humo | `@qa-publisher` | ✅ [`auditoria/checklist_humo.md`](auditoria/checklist_humo.md) |
| Build v0.3.0 (`Build/v0.3.0/Windows/MundoMagico.exe`, 0 errores y 0 warnings; respaldo `MM_20261109_1407.zip`) | `@qa-publisher` | ✅ |
| **Build candidata v0.3.1** (demo corta, entrada arreglada y modo Baja liviano): `Build/v0.3.1/Windows/MundoMagico.exe`, 0 errores | `@qa-publisher` | ✅ |
| **Prueba de humo del CTO sobre v0.3.1 (11 o 12 de noviembre)** | CTO | ⏳ |
| Correcciones y publicación (13 de noviembre) | todos | ⏳ |

---

## 🟡 Sprint 2: Columna vertebral (avance 2026-11-09)
| Tarea | Agente | Estado |
|---|---|:-:|
| Respaldo fijo de la demo estable: `MundoMagico_Respaldos\DEMO_v0.3.0_estable.zip` | `@tech-lead` | ✅ |
| Mezcla propia: `SC_SFX` y `SC_Ambiente` (hijos de Master) en `MundoMagico/Audio/Mezcla`. 19 sonidos asignados (`MUS_*` → Music). El volumen general ahora afecta también a las clases hijas (`bApplyToChildren`) | `@ux-toolsmith` | ✅ (falta el slider de SFX en el menú, Sprint 4) |
| Diseño del tramo de la demo completa: [`auditoria/diseno_sprint3_demo.md`](auditoria/diseno_sprint3_demo.md) | `@world-builder` | ✅ |
| **Clímax de la Casa y nuevo final** (fusible → apagón → payaso → huida a la entrada → pantalla final). Bodega abierta y final del castillo desactivado. Compila sin avisos. **Sin probar** | `@world-builder` + `@gameplay-engineer` | 🟡 |
| **Evento del Carrusel** (adelantado del Sprint 3): gira solo, silencio y susto de la muñeca. Compila sin avisos. **Sin probar en el `.exe`**, entra en v0.4.0 | `@world-builder` + `@gameplay-engineer` | 🟡 |
| Separar el guardado, mover el jugador y el GameMode a `Core`, `AC_Linterna`, piloto de BT | `@tech-lead` / `@gameplay-engineer` | ⏸️ **Después del 13:** son refactors del BP central y no se meten antes de publicar la demo corta |
| Camino A/B de hardware | `@tech-lead` | ⏳ 17-11 |

---

> **Build v0.4.0** (demo completa: Carrusel, Casa y huida) en `Build/v0.4.0/Windows/MundoMagico.exe`, 0 errores y 0 warnings. Respaldo `MM_20261109_1518.zip`. **Falta la prueba del CTO.**

> Avance adicional: el clímax de la Casa ahora exige estar a menos de 16 m de la casa (corrige que se repitiera al cargar). Borrador de itch.io en [`auditoria/itchio_pagina_demo.md`](auditoria/itchio_pagina_demo.md), sin publicar.

## 📬 Buzón de Solicitudes Internas (Mailbox)
> *Registro exclusivo de requerimientos entre agentes pendientes de aprobación por el CTO.*

- [ ] **@tech-lead → CTO:** reconsiderar un **Perfil A parcial**: solo `Scalable` más apagar RayTracing y MegaLights, que en DX11 ya no funcionan, sin el resto del Perfil A. Motivo: los cuelgues siguen con el modo liviano.
- [ ] **@world-builder → @tech-lead:** aprobar el presupuesto de render (1.2) antes de cambiar la iluminación (1.9).
- [ ] **@gameplay-engineer → @world-builder:** reemplazar las coordenadas escritas a mano en el BP del jugador por actores `BP_Marcador` en el mapa.
- [ ] **@ux-toolsmith → @gameplay-engineer:** sacar la UI (HUD, opciones, candado, galería, mochila) del BP del jugador a widgets propios. Va al backlog.
- [ ] **@ux-toolsmith → @gameplay-engineer:** usar `ReportNoiseEvent` en `BP_Enemigo`. Está cubierto por la tarea 1.8.
- [ ] **@qa-publisher → @tech-lead:** decidir qué packs se cocinan en el build (`DirectoriesToNeverCook` para los que no se usan).
- [ ] **@world-builder → CTO:** conseguir acceso a un equipo que corra PIE para un playtest real del parque iluminado.
