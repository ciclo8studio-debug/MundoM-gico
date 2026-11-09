# 🎛️ CONTROL BOARD — THRILLER / SURVIVAL HORROR (UE 5.8.4)

> Proyecto: **Mundo Mágico: Turno Nocturno** · Unreal: `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` · Mapa principal: `/Game/MundoMagico/Maps/L_Pabellon4`
> Productor Ejecutivo / CTO: Francisco · Coordinación: Claude
> Comunicación interna solo vía este archivo (sin correos ni notificaciones externas).
> 🗺️ **Plan de trabajo hasta la demo:** [`PLAN_DE_TRABAJO.md`](PLAN_DE_TRABAJO.md). Demo v0.5.0 el **10-12-2026** (plan acelerado, sprints de 1 semana), decisión de hardware el 17-11-2026.

## 🟢 Estado de los 5 Super-Agentes
- 🛡️ `@tech-lead`      | Estado: ✅ Sprint 1 completado (1.1, 1.11 · 1.12 postergada a Sprint 2)
- 🕹️ `@gameplay-engineer` | Estado: ✅ Sprint 1 completado (1.7, 1.8 parcial, 1.14)
- 🏚️ `@world-builder`     | Estado: 🟡 Sprint 1 · falta la descompresión tras el glitch (requiere playtest)
- 🎨 `@ux-toolsmith`     | Estado: ✅ Sprint 1 completado (1.5, 1.6)
- 🧪 `@qa-publisher`     | Estado: 🟡 Sprint 1 · build v0.2.0 OK · esperando la prueba de humo del CTO en el `.exe`

---

## 📋 Tablero de Tareas de la Auditoría Inicial

| Tarea / Diagnóstico | Agente Asignado | Estado | Directorio / Archivo Afectado | Requiere Aprobación CTO? |
| :--- | :---: | :---: | :--- | :---: |
| Auditoría de estructura de carpetas y dependencias | `@tech-lead` | ✅ Completada | [`auditoria/tech-lead.md`](auditoria/tech-lead.md) | ❌ No |
| Diagnóstico de Player Controller y Linterna | `@gameplay-engineer` | ✅ Completada | [`auditoria/gameplay-engineer.md`](auditoria/gameplay-engineer.md) | ❌ No |
| Evaluación de mapa de prueba y tensión visual | `@world-builder` | ✅ Completada | [`auditoria/world-builder.md`](auditoria/world-builder.md) | ❌ No |
| Revisión de HUD y componentes de Audio 3D | `@ux-toolsmith` | ✅ Completada | [`auditoria/ux-toolsmith.md`](auditoria/ux-toolsmith.md) | ❌ No |
| Inspección de warnings de compilación y linters | `@qa-publisher` | ✅ Completada | [`auditoria/qa-publisher.md`](auditoria/qa-publisher.md) · [`AUDIT_REPORT.md`](AUDIT_REPORT.md) | ❌ No |

---

## 🚀 Plan a Detalle del Sprint 1: Cimientos de Tensión e Interacción
> *Propuesto por `@tech-lead` sobre los hallazgos de `AUDIT_REPORT.md`. Estado: ✅ **Aprobado por el CTO (2026-11-09)** · 🟡 En ejecución.*

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
| **Prueba de humo del CTO (11 o 12 de noviembre)** | CTO | ⏳ |
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

## 📬 Buzón de Solicitudes Internas (Mailbox)
> *Registro exclusivo de requerimientos entre agentes pendientes de aprobación por el CTO.*

- [ ] **@tech-lead → CTO:** reconsiderar un **Perfil A parcial**: solo `Scalable` más apagar RayTracing y MegaLights, que en DX11 ya no funcionan, sin el resto del Perfil A. Motivo: los cuelgues siguen con el modo liviano.
- [ ] **@world-builder → @tech-lead:** aprobar el presupuesto de render (1.2) antes de cambiar la iluminación (1.9).
- [ ] **@gameplay-engineer → @world-builder:** reemplazar las coordenadas escritas a mano en el BP del jugador por actores `BP_Marcador` en el mapa.
- [ ] **@ux-toolsmith → @gameplay-engineer:** sacar la UI (HUD, opciones, candado, galería, mochila) del BP del jugador a widgets propios. Va al backlog.
- [ ] **@ux-toolsmith → @gameplay-engineer:** usar `ReportNoiseEvent` en `BP_Enemigo`. Está cubierto por la tarea 1.8.
- [ ] **@qa-publisher → @tech-lead:** decidir qué packs se cocinan en el build (`DirectoriesToNeverCook` para los que no se usan).
- [ ] **@world-builder → CTO:** conseguir acceso a un equipo que corra PIE para un playtest real del parque iluminado.
