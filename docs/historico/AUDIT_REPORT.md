# 🧪 AUDIT REPORT — Auditoría Integral Inicial

**Proyecto:** Mundo Mágico: Turno Nocturno (UE 5.8.4) · **Fecha:** 2026-11-09 · **Compilado por:** `@qa-publisher`
**Método:** auditoría de solo lectura (disco, logs, configuración, referencias dentro de `.uasset`). No se modificó el proyecto, no se usó el editor y no se jugó (PIE se cuelga en este hardware).
**Detalle por agente:** [`auditoria/tech-lead.md`](auditoria/tech-lead.md) · [`auditoria/gameplay-engineer.md`](auditoria/gameplay-engineer.md) · [`auditoria/world-builder.md`](auditoria/world-builder.md) · [`auditoria/ux-toolsmith.md`](auditoria/ux-toolsmith.md) · [`auditoria/qa-publisher.md`](auditoria/qa-publisher.md)

---

## 1. Veredicto ejecutivo

El juego tiene **muy buen contenido narrativo y ambiental**: escritura (Alfredo, Rafaela, Isidora, 1998), puzzles con ambientación y el pasillo de Urgencias. Pero tiene **cimientos frágiles**:

1. **No se puede jugar ni probar en el PC actual.** La GPU se cuelga en el render (BasePass).
2. **Toda la lógica está en un solo Blueprint de 10 MB** (`BP_ThirdPersonCharacter`).
3. **La IA no tiene Behavior Tree.**
4. **El audio prácticamente no es 3D.**
5. **El proyecto Unreal no tiene control de versiones.**

El Sprint 1 debe priorizar **estabilidad, capacidad de testeo y seguridad del proyecto** antes de agregar contenido.

---

## 2. Hallazgos críticos (🔴)

| # | Hallazgo | Agente | Evidencia |
|:-:|---|:-:|---|
| 1 | **Cuelgues de GPU (`DXGI_ERROR_DEVICE_HUNG`)**: 6 de 11 sesiones de hoy, siempre en `BasePass > ParallelDraw`. Hay 34 crash reports desde el 02-11. Ocurren también con el lanzador liviano. | qa / tech | `Saved/Logs`, `Saved/Crashes` |
| 2 | **Escena cara para una iGPU**: SkyAtmosphere y VolumetricCloud activos de noche, niebla volumétrica con rejilla 8/128 (unas 8 veces la estándar), unas 70 luces Movable, unos 1.134 árboles y unos 1.417 StaticMeshActors. Substrate y Mesh Distance Fields no se pueden apagar en runtime. | world / tech | `L_Pabellon4.umap`, `DefaultEngine.ini` |
| 3 | **Nadie ha jugado el parque iluminado.** Todo el ritmo de tensión del parque es teórico. | world | — |
| 4 | **El proyecto Unreal no tiene control de versiones.** Hay 20 `.tmp` en `Saved/` por guardados interrumpidos del BP del jugador. | tech | `Saved/` |
| 5 | **BP del jugador monolítico**: 10,1 MB y unas 135 funciones. Mezcla jugador, HUD/menús, guardado, puzzles, cinemáticas y clima. | gameplay / ux / tech | `/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter` |
| 6 | **IA sin Behavior Tree, Blackboard ni AIPerception.** Es una FSM sobre Tick que lee directamente las variables del jugador. | gameplay | `BP_Enemigo` |
| 7 | **El audio no es 3D**: 0 de 20 sonidos propios tienen atenuación, solo 1 de 16 fuentes del mapa la ajusta, y el susto del pasillo usa `PlaySound2D`. | ux | `MundoMagico/Audio`, `BP_SustoPasillo` |
| 8 | **La iluminación va al revés del género**: al volver la energía se encienden unas 95 luces y baja la tensión. Los 4 enemigos están en la avenida central y los laterales quedan sin amenaza. | world | Mapa |
| 9 | **Faltan glifos** (▲ ✕ ↓ ↑) en BarlowCondensed. La fuente de respaldo apunta fuera del proyecto, así que en un build probablemente se verán cuadrados. | ux / qa | Log `LogSlate` (87 avisos) |

## 3. Hallazgos importantes (🟠)

- **Disco:** Content pesa **38 GB**, de los cuales el contenido propio es solo **0,18 GB**.
  - Horror_Hospital pesa 18,4 GB y se usan unos 40 assets suyos.
  - UndergroundTunnel (5,1 GB) no tiene ninguna referencia.
  - Se pueden archivar unos 6,3 GB ya, y unos 18 GB más migrando lo que se usa de Horror_Hospital.
- **Guardado:** `BP_Partida` sirve a la vez para la partida y las opciones (CSV en `SwitchesTxt`), sin versión. La lógica vive en el personaje.
- **Restos de otros proyectos:**
  - 4 `.uproject` en la raíz.
  - Redirects a `Sci_Fi_AlienFacility`.
  - Sección de plataforma repetida 3 veces en `DefaultEngine.ini`.
  - Plugin GameplayStateTree activo sin usarse.
  - `__ExternalActors__` huérfanos.
- **Código:**
  - Nodos de depuración (PrintString, LogEstado, DrawDebug).
  - Coordenadas escritas a mano en el BP del jugador.
  - Índices fijos sobre `GetAllActorsOfClass`.
  - La botella no tiene evento de impacto ni de ruido.
- **NavMesh:** falta el NavMesh para el CrowdManager, y `BP_Giratorio` tiene escala 0.
- **Audio:**
  - No hay SoundClasses, SoundMixes ni límites de voces propios.
  - Los volúmenes se ajustan sobre un modificador "Duck" del motor y no hay control de SFX ni de ambiente.
  - El log registra 5 cortes del mixer por falta de buffer.
- **Diseño:**
  - 62 páginas de notas es demasiado.
  - El glitch pasa del susto a la calma sin descompresión.
  - Se oye lluvia que no se ve.
  - Hay sustos que solo existen como texto (noria, teatro, tren).
  - Hay unos 140 m de retroceso vacío entre la casa y la cabina.
- **Build:** el último Shipping (02-11, 2,2 GB, 0 errores) es anterior a los arreglos de hoy. No hay versión, CHANGELOG, script de build ni tests propios.
- **Localización:** no existe. Los textos están escritos directamente en el BP del jugador.
- **Nomenclatura:** 296 de 309 assets cumplen. Las 13 violaciones son 5 `SM_SM_`, 5 audios sin prefijo de tipo, `SKM_Isidora2/3` y `SM_Terr_Seco`.

## 4. Fortalezas (🟢)

- Los arreglos de hoy están confirmados en el log: ya no aparecen los SetTimer con tiempo 0, ni `Payasos[4]`, ni ningún `Accessed None`.
- La nomenclatura propia es buena y la clase coincide con el prefijo en el 100 % de los casos.
- Hay soporte completo de mando con Enhanced Input. El texto usa contorno, sombra y ScaleBox.
- Los generadores en Python (terreno, árboles, sonidos) se pueden volver a ejecutar, y el repo de scripts está en GitHub.
- La narrativa ambiental y los puzzles son el gran activo del proyecto.

## 5. Nota de QA sobre los cuelgues

El cuelgue de las 13:04 que menciona `@tech-lead` ocurrió con el editor **en reposo, mientras corría el instalador de controladores de Dell/Intel**. Ese instalador escanea y bloquea el hardware de video, así que no sirve como prueba de que el modo liviano falle. En cambio, el cuelgue de las 12:37 sí fue con el modo liviano y en PIE, en el primer cuadro de juego. La conclusión de fondo se mantiene: **el editor y PIE no son una plataforma de testeo fiable en este PC.** La alternativa a validar es un **build empaquetado**, que no carga el editor.

## 6. Recomendaciones consolidadas

Ver el **Plan a Detalle del Sprint 1** en [`BOARD.md`](BOARD.md).
