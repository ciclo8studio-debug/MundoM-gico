# Propuestas @tech-lead: rendimiento, arquitectura, datos, guardado y build

**Fecha:** 2026-11-09 · **Base:** `auditoria/tech-lead.md` (H1–H9) y `_contexto.md`
**Escala de esfuerzo (1 persona + IA):** S = 1-2 días · M = 3-5 días · L = 6+ días
**Sprints:** S1 = hasta demo corta 13-11 · S2 16-20/11 · S3 23-27/11 · S4 30/11-4/12 · S5 7-10/12 (v0.5.0) · Post = después del 10-12

**TOP 3:** ⭐ P1 (versionado), ⭐ P2 (perfil iGPU), ⭐ P3 (guardado y checkpoints).

---

## ⭐ P1. Versionado real del proyecto Unreal + respaldo automático
- **Qué:** Git + LFS (o Diversion, gratis para 1 persona) sobre `Config/`, `.uproject`, `Content/MundoMagico` y `Content/ThirdPerson`. Mientras tanto, un `.ps1` que respalde con fecha esas rutas al abrir el lanzador liviano.
- **Por qué le importa al jugador:** hoy un cuelgue de GPU durante un guardado (ya hay 20 `.tmp` fallidos) puede corromper el BP de 10 MB o el `.umap` y costar días. Eso pone en riesgo la fecha de la demo y la estabilidad de lo que llega a sus manos. Además, con versionado cada build de itch.io se puede reproducir y revertir.
- **Impacto:** Alto · **Esfuerzo:** S (script: 0,5 días; Git LFS: 1 día).
- **Dependencias y riesgos:** las cuotas de LFS en GitHub (1 GB gratis) no alcanzan para los packs, así que se versiona solo el contenido propio y los packs se tratan como externos. Mitigación: Diversion o un remoto LFS propio.
- **Sprint:** el script, **S1** (antes del 13-11). Git LFS, **S2**. Es requisito de P3 y P4.

## ⭐ P2. Perfil "iGPU": 30 fps estables en notebooks chilenos
- **Qué:**
  1. Perfil A parcial: `r.RayTracing=False`, `MegaLights=False` y `DefaultGraphicsPerformance=Scalable`. Lumen y Substrate no se tocan.
  2. `DefaultDeviceProfiles.ini` con un tope de texturas de 2048 (1024 en el grupo World), `r.Streaming.LimitPoolSizeToVRAM=1` y el pool en 200.
  3. **Autodetección al primer arranque:** se ejecuta `RunHardwareBenchmark` y se elige un preset (Muy baja, Baja o Media).
  4. **TSR o FSR** al 60-70 % con un nitidez mínima, y `t.MaxFPS` configurable (30/60).
  5. Presupuesto por zona: luces Movable con sombra ≤ 4 visibles y niebla volumétrica opcional.
- **Por qué le importa al jugador:** el público objetivo juega en Iris Xe o Vega 8 con 8 GB. Si la demo tartamudea o se cuelga, el susto no funciona y la reseña en itch.io es mala. Un horror fluido a 720p asusta más que uno bonito que se arrastra.
- **Impacto:** Alto · **Esfuerzo:** M (3-4 días, contando una recompilación de shaders y validación en Shipping).
- **Dependencias y riesgos:** requiere la decisión del CTO sobre el Perfil A parcial y el hardware del partner (17-11). La recompilación de shaders tarda horas en el PC de desarrollo, así que conviene dejarla de noche. Validar con `stat unit` y `profilegpu` en las 3 zonas más pesadas (entrada del parque, avenida y castillo).
- **Sprint:** **S2-S3.** Para el 13-11 solo los cvars del lanzador y el tope de texturas, que son reversibles.

## ⭐ P3. Guardado robusto con checkpoints
- **Qué:**
  1. Separar `SG_Opciones` de `SG_Partida` y agregar `Version:int`. Los CSV pasan a `Set<Name>` y structs, con migración del formato viejo al cargar.
  2. Mover la lógica a `BP_GameInstance_MM` (o `BPC_Guardado`), fuera del personaje.
  3. **Escritura segura:** se guarda primero en `Partida_tmp`, se verifica que cargue y recién entonces se copia a `Partida`. Además se mantiene `Partida_bak` con el guardado anterior.
  4. **Autoguardado en checkpoints** marcados con un actor `BP_Checkpoint`: cada interruptor, la llave, el fusible, la entrada a la Casa de Alfredo y el inicio del apagón. Al morir con el payaso se reaparece en el último checkpoint, con el estado del mundo restaurado (puertas, objetos recogidos y enemigos en su posición inicial).
- **Por qué le importa al jugador:** la demo completa dura más de 45 minutos e incluye una persecución. Perder 20 minutos por una muerte o por una partida corrupta hace que el jugador deje el juego. Los checkpoints justos sostienen la tensión sin castigar.
- **Impacto:** Alto · **Esfuerzo:** M (3-4 días).
- **Dependencias y riesgos:** P1 primero. @gameplay-engineer debe confirmar el slot y los puntos de guardado. Riesgo de regresión en "Continuar", por lo que hace falta una prueba con partida vieja, nueva y corrupta.
- **Sprint:** **S2-S3.** Es indispensable antes de la v0.5.0.

## P4. Arquitectura escalable: componentes + Data Assets + enemigo base
- **Qué:**
  - Partir `BP_ThirdPersonCharacter` en `BPC_Linterna`, `BPC_Inventario`, `BPC_Aliento`, `BPC_Interaccion` y `BPC_Sustos`.
  - Crear los Data Assets `DA_Item`, `DA_Nota` (las 62 notas), `DA_Cinta` (las 4 VHS) y `DA_Enemigo` (velocidades, oído, visión, daño).
  - Crear `BP_EnemigoBase` con la FSM común y percepción por `AIPerception` en lugar de Tick, del que heredan el payaso, Willy, el león y la muñeca. Así se prepara la migración al Behavior Tree ya diseñado.
  - Hacer funcionar los escondites `BP_Hiding_Door` como componente reutilizable.
- **Por qué le importa al jugador:** enemigos más coherentes y afinables, con menos bugs al sumar contenido. Las notas y las cintas quedan en datos, de modo que agregar historia o una traducción al inglés no toca código. Además, menos Tick significa más fps en iGPU y CPU modestas.
- **Impacto:** Alto (a mediano plazo) · **Esfuerzo:** L (6-8 días, incremental).
- **Dependencias y riesgos:** P1 es obligatorio. Coordinar con @gameplay-engineer. El riesgo de regresión es alto, así que se hace un componente por PR y se prueba en build.
- **Sprint:** `BPC_Guardado` en S2 (junto con P3) y `DA_Nota` en S3. El resto, **Post**.

## P5. Pipeline de build de un clic + smoke test automático
- **Qué:**
  - Un script `RunUAT BuildCookRun` con `-iterativecooking`, `MapsToCook` explícito (solo `L_Pabellon4` y el menú) y exclusión de las carpetas de packs en `DirectoriesToNeverCook`. Así se baja el tiempo del build (~15 min) y su tamaño.
  - Número de versión y hash visibles en el menú y en el log.
  - Subida a itch.io con **butler** en canales `demo-corta` y `beta`.
  - **Smoke test:** el build se lanza con `-ExecCmds` y un `BP_AutoRecorrido` que teletransporta por los checkpoints. En cada uno se capturan `csvprofiler` y hitches y se buscan `Error` o `DEVICE_HUNG` en el log. El resultado es un reporte OK/FALLA en texto.
- **Por qué le importa al jugador:** cada versión publicada pasó por el mismo recorrido mínimo, así que hay menos builds rotos en itch.io. Las descargas son más chicas, algo que pesa con la conexión de muchos jugadores, y los parches llegan más rápido.
- **Impacto:** Medio-Alto · **Esfuerzo:** M (3 días).
- **Dependencias y riesgos:** se apoya en P7 (dieta de Content) para que el cook no arrastre packs. El auto-recorrido depende de los checkpoints de P3.
- **Sprint:** el script de build y butler en **S1-S2**. El smoke test en **S3-S4**.

## P6. Telemetría de playtest local y opt-in
- **Qué:**
  - Un `BPC_Telemetria` (o una función en el GameInstance) que escribe un CSV o JSON en `Saved/Playtest/` con: hardware (GPU, RAM, resolución, preset), tiempo por tramo y checkpoint, muertes con su causa y posición, uso de pilas, botellas y botiquín, notas leídas, fps promedio y percentil 1 % por zona, y si hubo cierre limpio.
  - Al final de la demo, una pantalla "¿Nos ayudas?" con un botón que abre la carpeta y un enlace a un formulario.
  - Sin backend ni datos personales. En el futuro se puede evaluar GameAnalytics (gratis).
- **Por qué le importa al jugador:** el diseño se ajusta con datos reales y no con suposiciones (dónde se pierde, dónde muere, en qué PC sufre). Los sustos y la dificultad mejoran entre versiones, y el jugador ve que sus reportes sirvieron.
- **Impacto:** Alto · **Esfuerzo:** S-M (2-3 días).
- **Dependencias y riesgos:** hay que cuidar la privacidad: opt-in explícito, aviso en la página de itch y nada identificable. También hay que evitar que la escritura a disco provoque tirones, así que se escribe en buffer y se vuelca en cada checkpoint.
- **Sprint:** **S2-S3**, para que la v0.5.0 llegue a la comunidad ya instrumentada. En la demo corta solo se pide el log y una encuesta.

## P7. Dieta de Content + streaming hospital y parque
- **Qué:**
  - Archivar fuera de `Content/` los packs con 0 referencias (~6,3 GB) y migrar los ~40 assets usados de Horror_Hospital a `MundoMagico/Props/`, lo que libera unos 18 GB más.
  - Separar `L_Pabellon4` en un persistente más los subniveles `SL_Hospital`, `SL_Parque` y `SL_Parque_Vivo`, con carga y descarga durante el glitch. El glitch tapa la transición, así que se aprovecha narrativamente.
  - Activar One File Per Actor.
- **Por qué le importa al jugador:** menos RAM en el parque porque el hospital se descarga, menos tirones y una descarga más chica. En el estudio se gana un editor más estable y guardados del mapa más seguros.
- **Impacto:** Medio-Alto · **Esfuerzo:** M (3-4 días).
- **Dependencias y riesgos:** @world-builder debe validar con el Reference Viewer. Hacerlo con el editor cerrado y P1 hecho. Las referencias rotas al migrar se corrigen con *Fix Up Redirectors*.
- **Sprint:** archivar packs en **S2**. Subniveles en **S3-S4**.

## P8. Preparación para Steam Deck (y consolas a largo plazo)
- **Qué:**
  - Preset "Deck": 1280x800, TSR al 67 % y 30 fps fijos.
  - Glifos dinámicos según el último input (PS, Xbox, Deck o teclado), usando el mando ya completo.
  - UI con texto mínimo legible a 7" y suspensión y reanudación sin perder el estado (de esto se encarga P3).
  - Probar con Proton (DX11 funciona y `-vulkan` como alternativa).
  - Consolas solo después de Steam: requieren dev kits, cumplir certificación (TRC) y probablemente pasar de Blueprints puros a un mínimo de C++.
- **Por qué le importa al jugador:** el horror portátil en la cama con audífonos es un público natural, y el sello "Deck Verified" da visibilidad en Steam. La mayor parte del trabajo coincide con P2.
- **Impacto:** Medio · **Esfuerzo:** M (3 días sobre P2).
- **Dependencias y riesgos:** P2 y P3. Hace falta acceso a un Deck (o pedírselo al partner del 17-11). No bloquea nada de la demo.
- **Sprint:** **Post**. En S3 solo se deja preparado el sistema de glifos dinámicos, si @ux lo pide.

---

## Orden sugerido
| Sprint | Entregables tech |
|---|---|
| S1 (→13-11) | Script de respaldo (P1) · cvars y tope de texturas reversibles (P2) · script de build + butler (P5) |
| S2 | Git LFS (P1) · Perfil A parcial y autodetección (P2) · guardado nuevo y `BPC_Guardado` (P3/P4) · archivar packs (P7) |
| S3 | Checkpoints (P3) · telemetría (P6) · `DA_Nota` (P4) · smoke test (P5) |
| S4-S5 | Subniveles (P7) · validación de rendimiento en el hardware del partner · *freeze* técnico el 7-12 |
| Post | Componentes completos y `BP_EnemigoBase` (P4) · Steam Deck (P8) |
