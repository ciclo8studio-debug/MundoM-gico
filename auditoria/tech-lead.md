# Auditoría Tech Lead: arquitectura, datos y rendimiento

**Agente:** @tech-lead · **Fecha:** 2026-11-09 · **Modo:** solo lectura (sin editor ni unreal-mcp)
**Proyecto:** `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` (UE 5.8, solo Blueprints; no hay carpeta `Source/`, confirmado)

## Resumen

- `Content/` pesa **38 GB** y el contenido propio (`MundoMagico/`) solo **0,18 GB**. Cerca del **72 %** es Horror_Hospital (18,4 GB) más UndergroundTunnel (5,1 GB). Desde el mapa y los BP propios se citan apenas 40 assets de Horror_Hospital y **ninguno** de UndergroundTunnel.
- El render del proyecto (Ray Tracing, Lumen, VSM, MegaLights, Substrate, Distance Fields y `DefaultGraphicsPerformance=Maximum`) no es viable en una Intel Iris Plus con DX11. **El modo liviano no evita los cuelgues:** hay 8 `DXGI_ERROR_DEVICE_HUNG` en los logs, y uno ocurrió el 9-nov a las 13:04 con el lanzador liviano.
- **El proyecto Unreal no tiene control de versiones.** Los respaldos son copias manuales del `.umap` y del BP del jugador, y ese BP pesa 10 MB.
- El guardado funciona, pero es frágil: la misma clase `BP_Partida` sirve para la partida y para las opciones, los datos van en strings CSV, no hay número de versión y la lógica vive en el personaje.
- En la raíz del proyecto quedan **4 `.uproject`**, redirects de otro juego y `.ini` duplicados.

## Hallazgos

### 🔴 H1. La configuración de render no corresponde al hardware y los cuelgues siguen con el modo liviano
- `Config/DefaultEngine.ini` `[/Script/Engine.RendererSettings]` tiene activos `r.RayTracing=True`, `r.MegaLights.EnableForProject=True`, `r.RayTracing.RayTracingProxies.ProjectEnabled=True`, `r.DynamicGlobalIlluminationMethod=1` (Lumen), `r.ReflectionMethod=1`, `r.Shadow.Virtual.Enable=1`, `r.Substrate=True`, `r.GenerateMeshDistanceFields=True` y `r.AllowStaticLighting=False`. En `HardwareTargeting` figura `DefaultGraphicsPerformance=Maximum`.
- El RHI es DX11 (`DefaultGraphicsRHI_DX11`, se declara dos veces), así que el Ray Tracing y MegaLights no llegan a correr. El log lo dice: `Ray tracing is disabled. Reason: not supported by current RHI` (`Saved/Logs/MUNDO_MÁGICO.log`). Igual se compilan permutaciones de shaders y se generan datos que no se usan.
- El lanzador liviano (`herramientas/Abrir_MundoMagico_Liviano.ps1`) apaga RT, VSM, Lumen GI y Lumen reflections con `-dpcvars`. Sin embargo, **Substrate y Mesh Distance Fields son ajustes de proyecto que no se pueden cambiar en runtime**: siguen activos y consumen memoria compartida.
- Hay cuelgues registrados en 8 logs (`grep DEVICE_HUNG`). El último es `MUNDO_MÁGICO-backup-2026.11.09-13.04.57.log`, cuya línea de comandos ya incluía `-dpcvars=r.RayTracing.Enable=0,...`. El error sale de `D3D11Query.cpp:384`, que corresponde a un timeout de GPU (TDR).
- La escalabilidad del editor está en Alta: `Saved/Config/WindowsEditor/GameUserSettings.ini` tiene todo `sg.*=2` y `sg.ResolutionQuality=87`. El benchmark guardado da `LastGPUBenchmarkResult=51.7`, que es nivel bajo. La calidad "Baja" del juego no afecta al viewport del editor.
- `[SystemSettings]` tiene `r.Streaming.PoolSize=300` y `r.Streaming.LimitPoolSizeToVRAM=0`. En una GPU integrada con 8 GB de RAM compartida, eso deja que el pool compita con el editor.

### 🔴 H2. El proyecto Unreal no tiene control de versiones y hay señales de guardados fallidos
- En la raíz del proyecto no existe `.git`, `.p4` ni `.svn`. El repo de GitHub solo versiona scripts (`.gitignore` usa una lista blanca con `*`).
- Los respaldos se hacen a mano: `Respaldo_antes_perfilA/`, `Respaldo_antes_rediseno/` (con `BP_ThirdPersonCharacter.uasset`, `L_Pabellon4.umap` y `Opciones.sav.bak`) y `Perfil_A_pendiente_mejora_RAM/L_Pabellon4_PerfilA.umap`.
- `Saved/` contiene **20 archivos `BP_ThirdPersonCharacter*.tmp`** del 3-nov, de entre 4,7 y 9 MB cada uno. Son restos de guardados del BP interrumpidos o fallidos.
- `Content/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.uasset` pesa **10,15 MB** y tiene unas 136 funciones. Es un único punto de fallo: si se corrompe, se pierden a la vez linterna, inventario, guardado, menús y sustos. `Saved/Crashes/` tiene 24 crashes (56 MB).

### 🔴 H3. Content está sobredimensionado y muchos packs casi no se usan
Tamaño total: **38 GB**. La columna "Refs" cuenta los assets distintos de cada pack citados directamente desde `Content/MundoMagico/**` (mapa incluido) y `Content/ThirdPerson/**` (`grep -a "/Game/<Pack>/"`). Es un mínimo: las dependencias transitivas dentro de cada pack (MI → texturas, M) no se cuentan.

| Pack | Peso | Refs directas | Comentario |
|---|---:|---:|---|
| Horror_Hospital | **18,43 GB** | 40 (decals, MIs y meshes) | `Textures/` ocupa 19 GB en disco: 1290 texturas, **328 de más de 20 MB** (4K/8K), por ejemplo `T_Bed_Cloth_Quilt_01_D_N` de 50 MB. Del mapa no sale ninguna referencia directa a `Textures/`. |
| UndergroundTunnel | **5,13 GB** | **0** | Sin uso. Su `Maps/Demonstration.umap` pesa 25 MB. |
| DetectiveOffice | 2,59 GB | 32 | Uso parcial. |
| UnrealBucket | 2,42 GB | 16 | Uso parcial. |
| SoulCity | 1,55 GB | 10 | Trae dos mapas demo de 21 a 22 MB, uno de ellos `_Mobile`. |
| Apocalypse | 1,36 GB | 20 | Uso parcial. |
| GV_FreeShrubsPack | 1,34 GB | 4 | Muy poco uso para su peso. |
| clown_monster | 0,81 GB | 7 | Es el enemigo pendiente (ver memoria "Packs Fab pendientes"). |
| SICKA_MouldingSet | 0,66 GB | 1 | Muy poco uso. |
| FPMovement | 0,40 GB | 23 | Se usa desde el BP del jugador. |
| AnimatedWoodenCrateV1 | 0,39 GB | 2 | Muy poco uso. |
| SciFi_Beast06 | 0,39 GB | **0** | Sin uso. |
| FlashLight_System | 0,37 GB | 4 | Uso parcial. |
| PunkGirl_Bundle | 0,26 GB | **0** | Sin uso. Además tiene `__ExternalActors__`. |
| NY_Subway | 0,19 GB | 7 | Uso parcial. |
| RoninGirl, DynamicFalling, FreePack-RocksStylized, FreeSurvivalIconsKG, Textures, Variant_Platforming | ~0,23 GB en total | **0** | Sin uso directo. |

- **Se podrían sacar de inmediato unos 6,3 GB** (los packs con 0 referencias). Si además se migran los ~40 assets usados de Horror_Hospital a `MundoMagico/` y se archiva el pack, se recuperan **unos 18 GB más**.
- Hay mapas demo de packs que se pueden abrir por error y cargar varios GB: `UndergroundTunnel/Maps/Demonstration`, `SoulCity/Maps/LV_Soul_Slum*`, `Horror_Hospital/Maps/*`, `Maps/L_Overview` (en la raíz de Content) y otros 20 `.umap` de demo.

### 🟠 H4. El sistema de guardado es frágil
- La clase `Content/MundoMagico/Blueprints/BP_Partida.uasset` (SaveGame) guarda estas variables: `Vida, Bateria, Pilas, Botiquines, Botellas, Energia, TieneLinterna/Llave/Fusible, CasaAbierta, FalloEnergia, IntentosEnergia, PedestalPaso, CineDespertar/Parque/Payaso, Susto1..5, Nota*, Paginas, Cintas, CintasVistas, PosControl, RecogidosTxt, SwitchesTxt`.
- **La misma clase guarda también las Opciones.** `Saved/SaveGames/Opciones.sav` es un `BP_Partida_C` con las opciones metidas en `SwitchesTxt` como CSV: `"1.0,1.0,0.9,1.0,1,0"` (brillo, volumen, sensibilidad, etc.). Cualquier cambio en el formato de los switches de la partida rompe las opciones, y al revés.
- `RecogidosTxt` y `SwitchesTxt` son strings serializados a mano en lugar de `Set<Name>` o un struct, y **no hay campo `Version`** para migrar partidas viejas.
- La lógica de guardado vive en el personaje: `GuardarOpciones`, `CargarOpciones`, `CargarCandado`, `AplicarOpciones`, además de 2 `SaveGameToSlot`, 2 `LoadGameFromSlot`, 2 `DeleteGameInSlot` y 2 `DoesSaveGameExist` dentro de `BP_ThirdPersonCharacter`. No hay GameInstance propio que sobreviva a los cambios de mapa.
- Ahora mismo no existe en disco ningún slot de partida. Solo están `Opciones.sav` y `Opciones_antes_baja.sav`. El nombre exacto del slot de partida no se pudo confirmar sin abrir el BP.

### 🟠 H5. La raíz del proyecto y la configuración tienen restos de otros proyectos
- Hay **4 `.uproject`** en la raíz: `MUNDO_MÁGICO` (5.8), `FlashlightHorror` (5.7), `PlaygroundApocalypse` (5.0) y `Sci_Fi_AlienFacility` (5.7). Vienen de packs de Fab. Hacer doble clic en el equivocado abre otra configuración de plugins.
- `DefaultEngine.ini` tiene `+ActiveGameNameRedirects` hacia `/Script/Sci_Fi_AlienFacility`.
- `[/Script/WindowsTargetPlatform.WindowsTargetSettings]` aparece **3 veces**: `D3D11 PCD3D_SM5` está dos veces, `D3D12 SM5+SM6` sobra con DX11, y Vulkan se declara y luego se borra con `!ClearArray`.
- Hay archivos que sobran: `Config/DefaultEngine.ini.bak_basura`, `Config/HoloLens/HoloLensEngine.ini` y la sección `AndroidFileServer` con `bEnablePlugin=True` y un `SecurityToken` en texto plano.
- Plugins de `MUNDO_MÁGICO.uproject`: `ModelContextProtocol` y `AllToolsets` están activos en todos los targets (deberían ser solo de editor). `GameplayStateTree` está activo pero **no se usa** (0 referencias a StateTree en contenido propio).

### 🟠 H6. Estructura de carpetas: el código y los assets propios están repartidos fuera de `MundoMagico/`
- El jugador, el GameMode y el PlayerController están en `/Game/ThirdPerson/Blueprints/` (carpeta de plantilla), y `GlobalDefaultGameMode` apunta allí.
- Hay props sueltos en la raíz: `/Game/Meshes/` (19 SM de calle) y `/Game/Materials/` (8 MI, `MM_MasterSimple`). El mapa los cita 9 veces.
- Quedan restos de plantilla: `Variant_Platforming/` (13 archivos), `LevelPrototyping/` (29 archivos, 2 referencias) y `ThirdPerson/Lvl_ThirdPerson.umap`.
- Hay **`__ExternalActors__` huérfanos**: `AnimatedStorageBox/DemoLevel` (el pack ya no existe en Content), además de `ThirdPerson/Lvl_ThirdPerson`, `Variant_Platforming/Lvl_Platforming` y `PunkGirl_Bundle`, que pertenecen a mapas demo.
- Las únicas carpetas vacías son `Content/Collections` y `Content/Developers/Pancho/Collections`, ambas inofensivas.

### 🟢 H7. La nomenclatura del contenido propio es consistente en general
- `MundoMagico/` usa prefijos correctos en casi todo: `MI_` 96, `T_` 78, `SM_` 66, `BP_` 20, `M_` 19, `WBP_` 4, `SKM_` 3, `NS_` 2 y `ABP_` 1.
- La excepción es el audio: usa `SFX_`, `Ambiente_` y `Musica_` en vez de un prefijo de tipo (`S_`/`SW_`/`SC_`), y tiene subcarpetas sin significado (`Audio/SFX/SFX2`, `SFX3`). Además `Ambiente_T_Hospital` y `SFX_T_*` mezclan convenciones.
- `UI/` mezcla texturas (`T_Portada*`, `T_Mapa2`, `T_Mapa3`) con widgets. Conviene crear `UI/Texturas`.

### 🟢 H8. El mapa es único y manejable, pero monolítico
- `MundoMagico/Maps/L_Pabellon4.umap` pesa 5,2 MB y no usa World Partition ni One File Per Actor. Aunque la partida va del hospital al parque, todo vive en un solo archivo. Por eso cada guardado reescribe todo el nivel y no se puede trabajar en paralelo.

### 🟢 H9. El repo de scripts está bien acotado
- El `.gitignore` en lista blanca deja fuera `Build/` (2,2 GB), el zip de 592 MB, `Intel-Driver-...exe` y los respaldos. Hay 34 archivos versionados.

## Riesgos

1. **Pérdida de trabajo.** Sin VCS, una corrupción del BP de 10 MB o del `.umap` causada por un cuelgue de GPU durante el guardado (ya hay `.tmp` de guardados fallidos) puede costar días.
2. **Los cuelgues seguirán con el modo liviano.** Substrate, Distance Fields, la escalabilidad Alta del editor y las texturas 4K/8K no se tocan en runtime.
3. **Disco y RAM.** 38 GB de Content inflan el Asset Registry, el escaneo inicial y la DDC. En un futuro cook o empaquetado (`Build/` ya pesa 2,2 GB), cualquier referencia accidental arrastra GB de texturas.
4. **Las partidas guardadas pueden romperse en silencio** al cambiar `BP_Partida` (no hay versión) o el CSV de opciones.
5. **Se puede abrir el `.uproject` equivocado** (otra versión del motor u otros plugins) y regenerar config o DDC.

## Recomendaciones para Sprint 1 (priorizadas)

1. **P0: Respaldo y versionado del proyecto Unreal.** Lo ideal es Git + LFS (o Diversion/Perforce) que excluya `DerivedDataCache/`, `Intermediate/`, `Saved/` y `Binaries/`, empezando por `Config/`, `*.uproject` y `Content/MundoMagico` + `Content/ThirdPerson`. Como mínimo, un `.ps1` de respaldo con fecha de esas rutas antes de cada sesión. Después, borrar los 20 `.tmp` de `Saved/` con el editor cerrado.
2. **P0: Bajar la carga de GPU sin tocar la decisión del CTO sobre Perfil A.**
   a) Poner la escalabilidad del editor en Baja (`sg.*=0`, ResolutionQuality ~70) desde el menú Settings → Engine Scalability, y fijar `t.MaxFPS 30` en el editor.
   b) Agregar al lanzador liviano `r.Streaming.PoolSize=200`, `r.Streaming.LimitPoolSizeToVRAM=1`, `r.ScreenPercentage=70` y `r.VolumetricFog=0`.
   c) Volver a evaluar con el CTO **un Perfil A parcial**, porque los cuelgues siguen. La propuesta es un mínimo que solo apaga lo que DX11 no usa: `r.RayTracing=False`, `r.MegaLights.EnableForProject=False` y `DefaultGraphicsPerformance=Scalable`. Así no se toca Lumen ni Substrate y la recompilación de shaders es menor.
3. **P1: Dieta de Content (con el editor cerrado y un respaldo hecho).** Mover fuera de `Content/`, a una carpeta archivo externa, los packs con 0 referencias: UndergroundTunnel, SciFi_Beast06, PunkGirl_Bundle, RoninGirl, DynamicFalling, FreeSurvivalIconsKG, Variant_Platforming y los `__ExternalActors__` huérfanos (~6,3 GB). Antes hay que confirmarlo con el Reference Viewer (@world-builder). Luego planificar la migración de los assets usados de Horror_Hospital, DetectiveOffice, SoulCity, GV_FreeShrubs, SICKA y AnimatedWoodenCrate a `MundoMagico/Props/<Origen>/` y archivar los packs.
4. **P1: Limitar el tamaño de las texturas.** Con un `Config/DefaultDeviceProfiles.ini` (perfil Windows) o con `MaxTextureSize`/LODBias por grupo de texturas, poner un tope de 2048 (1024 en `World`). Es reversible y no cambia la iluminación.
5. **P1: Rehacer el guardado.** Separar `SG_Opciones` de `SG_Partida`, agregar `Version:int`, reemplazar `RecogidosTxt` y `SwitchesTxt` por `Set<Name>`, y mover el guardado y la carga a un `BP_GameInstance_MM` (o un componente `BPC_Guardado`). Al cargar, mantener la compatibilidad leyendo el CSV viejo una sola vez.
6. **P2: Limpiar la raíz y la configuración.** Archivar los 3 `.uproject` ajenos, `DefaultEngine.ini.bak_basura`, `Config/HoloLens/` y los `ActiveGameNameRedirects`. Dejar una sola sección `WindowsTargetSettings` (solo D3D11 SM5 mientras se use DX11). Desactivar `GameplayStateTree` y AndroidFileServer, y marcar `ModelContextProtocol`/`AllToolsets` como solo editor.
7. **P2: Estructura.** Mover `ThirdPerson/Blueprints/*` a `MundoMagico/Blueprints/Jugador/` y `/Game/Meshes` + `/Game/Materials` a `MundoMagico/Props/Calle/`, siempre con *Fix Up Redirectors*. Empezar a dividir el BP del jugador en componentes (`BPC_Linterna`, `BPC_Inventario`, `BPC_Guardado`, `BPC_Sustos`). Coordinarlo con @gameplay-engineer.

## Solicitudes a otros agentes

- **@gameplay-engineer:** confirmar el nombre del slot de partida y en qué momentos se guarda o carga. Proponer cómo dividir `BP_ThirdPersonCharacter` en componentes y qué variables migrar del CSV.
- **@world-builder:** con el Reference Viewer o el Size Map, validar la lista de packs con 0 referencias y la lista de assets de Horror_Hospital, DetectiveOffice y SoulCity que realmente quedan en `L_Pabellon4`, antes de archivar.
- **@qa-publisher:** reproducir el cuelgue con `stat unit`, `stat gpu` y `profilegpu` justo antes del hang (parece ser en PIE y en ciertas zonas). Registrar en qué zona del mapa ocurre, para aislar si viene de la niebla volumétrica, las luces Movable (23) o los materiales Substrate.
- **@ux-toolsmith:** agregar al lanzador liviano los cvars de streaming y resolución, y crear el script de respaldo con fecha (P0).
- **CTO:** decidir sobre el "Perfil A parcial" (RT y MegaLights off, `Scalable`), dado que los cuelgues ocurren aun con el modo liviano.
