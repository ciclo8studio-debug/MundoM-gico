# 🗺️ PLAN DE TRABAJO — Demo jugable de *Mundo Mágico: Turno Nocturno*

> **Meta:** una **demo jugable, estable y pulida** lista para publicar (itch.io y, más adelante, Steam Next Fest).
> **Ritmo (acelerado el 2026-11-09):** sprints de **1 semana**. Los agentes trabajan en continuo y el CTO hace **1 prueba de build por semana** (3 a 5 h en total).
> **Fecha objetivo de la demo:** **v0.5.0, 10 de diciembre de 2026** (antes era el 10 de enero; se adelanta un mes).
> **Decisión clave:** **17 de noviembre de 2026**, si llega o no el hardware del partner.

---

## 0. Hito intermedio: DEMO CORTA v0.3.0 (13 de noviembre de 2026) · aprobado por el CTO
Recorrido de unos 15 minutos:
1. Sala del despertar.
2. Pasillo con el susto en audio 3D.
3. Glitch.
4. Entrada del parque.
5. Avenida en penumbra (muñeca y botella).
6. Fin frente al castillo, con la pantalla "Capítulo 1 completado".

Las zonas fuera del recorrido quedan bloqueadas con rejas y conos. Calidad Baja por defecto.
**Requisito:** prueba de humo del CTO con `auditoria/checklist_humo.md` el **11 o 12 de noviembre**. El 13 se corrigen los bugs y se publica el build final.

## 1. Qué es la demo completa (alcance cerrado)

**Recorrido de 20 a 30 minutos:**
1. **Sala del despertar:** tutorial implícito de mover, recoger la linterna, interactuar y abrir la puerta.
2. **Pasillo de Urgencias:** primera tensión, susto de la criatura con sonido 3D y glitch.
3. **Descompresión:** 20 a 30 s de calma en la entrada del parque, con boletería, neones y lluvia.
4. **Avenida en penumbra:** primera amenaza (la muñeca), notas de Alfredo, Rafaela e Isidora, y la botella como distracción.
5. **Dos atracciones jugables:** **Carrusel** (puzzle corto) y **Casa de Alfredo** (exploración más el clímax).
6. **Cierre de la demo:** persecución corta del payaso, pantalla "Continuará…" e invitación a lista de deseos.

**Fuera de la demo, pero sigue en el juego completo:** noria, teatro, castillo, tren, torre, montaña, bodega, el resto de las 62 páginas, el final y los packs archivados (túnel, bestia, RoninGirl, voltereta).

> Principio: **menos zonas, mejor terminadas.** Todo lo que no entra en la demo se oculta o se cierra con bloqueos diegéticos, como rejas, escombros o una atracción cortada.

---

## 2. Cómo trabajamos (rituales del CTO, unas 4 h por semana)

| Momento | Duración | Qué haces | Dónde |
|---|---|---|---|
| **Lunes, apertura del sprint** (cada semana) | 30 min | Leer el plan del sprint y aprobar o rechazar las tareas marcadas "Requiere aprobación CTO" | `BOARD.md` |
| **Mitad de semana, prueba del build** | 60–90 min | Jugar el `.exe` de la semana con la checklist de humo y anotar los bugs | `Build/vX.Y.Z/` · `BOARD.md` → Buzón |
| **Viernes, buzón** | 20 min | Resolver las solicitudes cruzadas entre agentes | `BOARD.md` → Buzón |
| **Domingo de cierre del sprint** | 60 min | Revisar la demo del sprint, aceptar o rechazar y priorizar el siguiente | `BOARD.md` · `CHANGELOG.md` |

**Reglas fijas:**
- Cada sprint termina con un **build empaquetado** (`herramientas/Construir_Version.bat`) que sube la versión menor.
- **Antes de cada cambio grande** se ejecuta `Respaldar_Proyecto.bat`.
- **Se prueba siempre en el `.exe`**, no en el editor, mientras siga el PC actual.
- **Definición de terminado:**
  - Compila con 0 avisos.
  - El cook termina con 0 errores.
  - El recorrido del sprint se completa en el `.exe` sin cierres.
  - El cambio queda anotado en `CHANGELOG.md`.

---

## 3. Decisión del 17 de noviembre (dos caminos)

| | **Camino A: llega el hardware del partner** | **Camino B: seguimos con el PC actual** |
|---|---|---|
| Pruebas | PIE en el editor más un build semanal | Solo builds empaquetados (unos 15 min por build) |
| Gráficos | Se aplica el **Perfil A** (sin RT/MegaLights; Lumen o luz horneada según rendimiento) y se recupera la escala de calidad Alta | Se mantiene el **presupuesto iGPU**: calidad Baja por defecto en la demo, niebla 16/64, sin nubes ni atmósfera |
| Iteración de diseño | Rápida (playtest en vivo) | Más lenta: cada ajuste visual espera al build |
| Riesgo de fecha | Bajo | Medio: se compensa con menos alcance (sin persecución final pulida, si hace falta) |

> El `@tech-lead` deja preparado el cambio de camino para que sea de **un solo paso**: aplicar `Perfil_A_pendiente_mejora_RAM/DefaultEngine_PerfilA.ini` más el lanzador normal.

---

## 4. Sprints

### 🟢 Sprint 1: Cimientos de Tensión e Interacción (9 → 12 de noviembre) · *en cierre*
Ya está hecho: respaldo, render iGPU, audio 3D, fuentes, renombres, packs archivados y build v0.2.0 estable (sin cierres de GPU).
**Para cerrar:**
| Tarea | Agente |
|---|---|
| Prueba de humo 2: salir de la sala con la linterna y llegar al parque | CTO + `@qa-publisher` |
| Descompresión tras el glitch (20–30 s sin amenazas, luz segura, audio que baja) | `@world-builder` |
| **Calidad Baja por defecto** cuando no hay opciones guardadas (primer arranque) | `@ux-toolsmith` |

### 🟡 Sprint 2: Columna vertebral (9 → 19 de noviembre) · *arrancó el 9 de noviembre; lo que depende del hardware espera al 17*
| Tarea | Agente | Aprob. CTO |
|---|---|:-:|
| Aplicar el camino A o B (Perfil A, o consolidar el presupuesto iGPU) | `@tech-lead` | ✅ |
| Separar el guardado: `SG_Opciones` / `SG_Partida`, con campo de versión (tarea postergada del Sprint 1) | `@tech-lead` | ✅ |
| Pasar el jugador y el GameMode de `/ThirdPerson` a `MundoMagico/Blueprints/Core` (`BP_Jugador`, `BP_GM_MundoMagico`) | `@tech-lead` | ✅ |
| **Behavior Tree** del enemigo: `BB_`, `BT_` y `AIC_Enemigo`, piloto con el Oso Willy (`UsarBT=true`) | `@gameplay-engineer` | ✅ |
| Extraer la **linterna** a un componente `AC_Linterna` (batería, parpadeo, pilas) | `@gameplay-engineer` | ✅ |
| Cerrar el **alcance de la demo**: bloqueos diegéticos en las zonas fuera de la demo | `@world-builder` | ✅ |
| `SoundClass`/`SoundMix` propios: Master, Música, SFX y Ambiente, con slider de SFX en opciones | `@ux-toolsmith` | ❌ |
| Checklist de humo escrita (`auditoria/checklist_humo.md`) y build **v0.3.0** | `@qa-publisher` | ❌ |

### 🔵 Sprint 3: El tramo de la demo (20 → 26 de noviembre)
| Tarea | Agente | Aprob. CTO |
|---|---|:-:|
| Sala y pasillo finales: tutorial implícito (prompts contextuales) y susto de la criatura pulido | `@world-builder` | ❌ |
| **Carrusel jugable**: puzzle corto y susto convertido de texto a evento | `@world-builder` + `@gameplay-engineer` | ✅ |
| **Casa de Alfredo**: exploración, notas clave y clímax | `@world-builder` | ✅ |
| Lenguaje de luz final (segura, penumbra, oscuridad) en el tramo de la demo | `@world-builder` | ❌ |
| Botella con percepción real: los enemigos oyen `MakeNoise` (BT) | `@gameplay-engineer` | ❌ |
| Reverb por zona (sala, pasillo, exterior) y ambiente de lluvia coherente con lo que se ve | `@ux-toolsmith` | ❌ |
| Build **v0.4.0** y playtest del CTO de punta a punta | `@qa-publisher` | ❌ |

### 🔵 Sprint 4: Pulido y experiencia (27 de noviembre → 3 de diciembre)
| Tarea | Agente | Aprob. CTO |
|---|---|:-:|
| Persecución del payaso y pantalla de cierre "Continuará…" | `@gameplay-engineer` + `@world-builder` | ✅ |
| Sacar la UI del BP del jugador: HUD, pausa, mochila y notas como widgets propios | `@ux-toolsmith` | ✅ |
| Menú final: calidad autodetectada, tamaño de texto, SafeZone y control completo con mando | `@ux-toolsmith` | ❌ |
| **StringTable ES** para todos los textos (y EN opcional si hay tiempo) | `@qa-publisher` | ❌ |
| Pase de rendimiento en el `.exe`: meta de **30 fps en calidad Baja** en el PC actual | `@tech-lead` | ❌ |

### 🔵 Sprint 5: QA y lanzamiento de la demo (4 → 10 de diciembre)
| Tarea | Agente | Aprob. CTO |
|---|---|:-:|
| *Bug bash* con la checklist completa y corrección de todo lo 🔴 y 🟠 | `@qa-publisher` + todos | ❌ |
| Playtest externo con 3 a 5 personas (formulario corto) y ajustes | CTO + `@qa-publisher` | ✅ |
| Capturas y tráiler de 60 s grabados del `.exe` | `@world-builder` + `@ux-toolsmith` | ❌ |
| Página de itch.io (textos, capturas, requisitos mínimos) | `@qa-publisher` | ✅ (publicar) |
| **Build v0.5.0: DEMO** y entrada final en `CHANGELOG.md` | `@qa-publisher` | ✅ |

---

## 5. Responsabilidad de cada agente durante el plan

| Agente | Foco hasta la demo | Entregable que lo mide |
|---|---|---|
| 🛡️ `@tech-lead` | Camino A/B, guardado robusto, carpetas Core, rendimiento | Build sin cierres y 30 fps en Baja |
| 🕹️ `@gameplay-engineer` | Behavior Tree, componente de linterna, ruido y percepción, persecución | Enemigos que reaccionan a ruido y luz |
| 🏚️ `@world-builder` | Tramo de la demo, sustos como eventos, lenguaje de luz, bloqueos | Recorrido de 20–30 min con ritmo de tensión |
| 🎨 `@ux-toolsmith` | Mezcla de audio, reverb, UI extraída, opciones y accesibilidad | Menú y HUD finales, audio 3D coherente |
| 🧪 `@qa-publisher` | Checklist, builds por sprint, StringTable, itch.io | Build v0.5.0 publicado |

---

## 6. Riesgos y mitigación

| Riesgo | Prob. | Impacto | Mitigación |
|---|:-:|:-:|---|
| No llega el hardware y el PC no aguanta la demo | Media | Alto | Camino B, calidad Baja por defecto y menos alcance |
| Cambios en el BP del jugador (10 MB) rompen el juego | Media | Alto | Respaldo antes de cada sprint, extracción gradual a componentes y build de prueba |
| El BT no se puede validar sin playtest | Media | Medio | Piloto con 1 enemigo (`UsarBT`) y FSM como respaldo |
| Poco tiempo del CTO para probar | Alta | Medio | Checklist corta de 60–90 min y solo una prueba por semana |
| El alcance crece | Alta | Alto | Regla: lo que no está en la sección 1 va a "Juego completo", sin excepciones |

---

## 7. Próximos pasos inmediatos
1. **CTO:** terminar la **prueba de humo 2** con calidad **Baja** (0) en Configuración.
2. **Agentes:** cerrar el Sprint 1 (descompresión y calidad Baja en el primer arranque).
3. **17 de noviembre:** decidir el Camino A o B y abrir el Sprint 2.
