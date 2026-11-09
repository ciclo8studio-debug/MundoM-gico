# Auditoría QA / Publisher — Mundo Mágico: Turno Nocturno

Fecha: 2026-11-09 · Agente: qa-publisher · Modo: SOLO LECTURA (no se modificó el proyecto)
Proyecto: `C:\Users\pansh\Documents\Unreal Projects\MUNDO_MÁGICO` (UE 5.8) · Repo: `C:\Users\pansh\Desktop\MundoMagico_Project`

## 1. Resumen

- **Estabilidad**: 10 crash reports hoy en `Saved/Crashes` (34 desde 11-02). 6 de las 11 sesiones de hoy terminaron con `DXGI_ERROR_DEVICE_HUNG` (0x887A0006), siempre en `BasePass > ParallelDraw` (render, no lógica de juego). Es el riesgo n.º 1.
- **Blueprint**: el ruido de LogScript de hoy (1.016 avisos en la sesión de 04:29) venía de los bugs ya corregidos (SetTimer con 0 s, índice 4 de `Payasos`). En la sesión actual **ya no aparecen**: quedan 15 avisos benignos de `GetObjectProperties` (MCP leyendo `ExtendedBounds`). No hay `Accessed None` en ningún log guardado.
- **Nombres**: el contenido propio (`Content/MundoMagico`, 309 assets) está en buen estado: el 96 % cumple. Hay 13 violaciones menores (doble prefijo `SM_SM_`, audio sin prefijo de tipo, versiones sueltas de SKM).
- **Localización**: no existe (ni `Content/Localization`, ni StringTables, ni cultures en config). Todos los textos del juego están hardcodeados en `BP_ThirdPersonCharacter`.
- **Build**: el último Shipping Win64 salió bien el 2026-11-02 (0 errores, 2,2 GB, desde la copia `C:\MM_Build`). Tiene 7 días y es anterior a todos los arreglos de hoy. `DefaultGame.ini` no tiene ninguna configuración de packaging/versión, y no hay script de build versionado ni CHANGELOG.

## 2. Top warnings / errores (11 sesiones del 2026-11-09: `MUNDO_MÁGICO.log` + 10 backups)

| Categoría | Conteo | Ejemplo | Severidad |
|---|---|---|---|
| LogScript – SetTimer con tiempo <= 0 (MostrarSusto 318, MostrarFinal 189, MostrarPagina 9, OcultarPagina 9) | 525 | `BP_ThirdPersonCharacter_C_0 MostrarSusto SetTimer passed a negative or zero time` | Alta → **corregido** (0 en la sesión actual) |
| LogScript – "Script Msg called by" (traza que acompaña a lo anterior) | 542 | `called by: BP_ThirdPersonCharacter_C ... L_Pabellon4` | Info (duplicado) |
| LogScript – array fuera de rango | 17 | `Attempted to access index 4 from array 'Payasos' of length 1` | Alta → **corregido** |
| LogScript – GetObjectProperties (MCP) | 15 | `SM_Rock_01 ... could not be read: ExtendedBounds` | Baja (herramienta, no juego) |
| LogRHI / LogD3D11RHI – GPU colgada | 6 sesiones (24 RHI + 7 D3D11 + 6 "exiting") | `CreateQuery failed with DXGI_ERROR_DEVICE_REMOVED DXGI_ERROR_DEVICE_HUNG`; Breadcrumb `BasePass > ParallelDraw` | **Crítica** |
| LogAutomationTest – "Condition failed" | 14 por sesión (154 en total) | en el frame 0 del arranque, tras `AttemptToFindUninitializedScriptStructMembers` | Baja (autocheck del motor/plugins; no son tests del juego) |
| LogSlate – glifo inexistente en BarlowCondensed | 87 (U+0025 `%` 39, U+2191 flecha arriba 16, U+2193 flecha abajo 16, U+2715 cruz 16) | `Could not find Glyph ... codepoint U+2715, getting last resort font` | Media (UI muestra fuente de reserva) |
| LogFileManager – MoveFile / delete bloqueado (Error 32) | 20 W + 2 E | `MI_Follaje.uasset`, `MI_Cerro.uasset`, `SM_Arbol_A/B.uasset`, `GameUserSettings*.tmp` | Media (riesgo de asset sin guardar; antivirus/OneDrive/segundo proceso) |
| LogEOSSDK / LogEOSShared – analytics | 45 | `EventCache either took too long to flush` | Baja |
| LogHttp – timeout | 19 | `generate_204 timed out` | Baja (sin red / editor colgado) |
| LogModelContextProtocol | 16 E + 11 W | `Call to unknown method "server/discover"` | Baja (solo editor) |
| LogAudioMixer(Wasapi) | 15 | `Timeout waiting for h/w` (consecuencia del cuelgue de la GPU) | Baja |
| LogCrowdFollowing | 4 | `Unable to find RecastNavMesh instance` | Media (IA de enemigos sin NavMesh en algún mapa) |
| LogPhysics | 4 | `BP_Giratorio_C.Malla has a component set to zero` (escala 0) | Media |
| LogConsoleManager | 3 | `r.ScreenPercentage SetByScalability ignored (SetByConsole)` | Baja (lo fuerza el modo liviano) |
| LogHttpListener / LogToolsetRegistry | 2 | `unable to bind` (puerto del MCP ocupado) | Baja |

Sin hallazgos: `Accessed None` (0), LoadErrors, missing assets o "Failed to load" de assets del juego, y redirectors en los logs.

## 3. Linter de nombres (`Content/MundoMagico`, 309 assets; clase verificada en la cabecera del uasset)

Cumplen: BP_ (20), WBP_ (4), SM_ (66), M_ (19), MI_ (96), T_ (78), NS_ (2), ABP_ (1), SKM_ (3), L_ (1), SFX_ (14). No hay ningún caso de prefijo que no coincida con la clase real. No hay espacios ni tildes en los nombres.

| # | Violación | Ruta | Sugerencia |
|---|---|---|---|
| 1-5 | Doble prefijo `SM_SM_` | `MundoMagico/Meshes/SM_SM_Carrusel_Gira`, `SM_SM_Carrusel_Luces`, `SM_SM_MaquetaAuto`, `SM_SM_Noria_Gira`, `SM_SM_Noria_Luces` | `SM_Carrusel_Gira`, etc. (renombrar en el editor y Fix Up Redirectors) |
| 6-9 | SoundWave sin prefijo de tipo | `MundoMagico/Audio/Ambiente_Hospital`, `Ambiente_Parque`, `Musica_Menu`, `Musica_Persecucion` | `AMB_`/`MUS_` (coherente con `SFX_`) o `SW_` |
| 10 | Igual que 6-9 | `MundoMagico/Audio/Terror/Ambiente_T_Hospital` | `AMB_T_Hospital` |
| 11-12 | Versiones sueltas (posible contenido muerto que se cocina) | `MundoMagico/Isidora/SKM_Isidora2`, `SKM_Isidora3` | Confirmar cuál se usa y eliminar o mover a `_Old` |
| 13 | Nombre poco claro | `MundoMagico/Arboles/SM_Terr_Seco1/2` | `SM_Terreno_Seco_01/02` |

Problemas estructurales (fuera de MundoMagico):
- El jugador, el GameMode y el PlayerController del juego viven en la carpeta de la plantilla: `/Game/ThirdPerson/Blueprints/BP_ThirdPerson*` (es el `GlobalDefaultGameMode`). Recomendado moverlos a `MundoMagico/Blueprints` con nombres propios (`BP_Jugador`, `BP_GM_MundoMagico`).
- Hay más de 25 carpetas de packs de terceros/Fab y de plantillas en `Content/` (Horror_Hospital, NY_Subway, SoulCity, Variant_Platforming, Developers, etc.) y 4 `.uproject` en la raíz (`FlashlightHorror`, `PlaygroundApocalypse`, `Sci_Fi_AlienFacility` además del principal), que confunden el lanzador/UAT.
- Mezcla de idioma: carpetas y assets en español con prefijos en inglés. Aceptable, pero conviene fijarlo en una guía de estilo.

## 4. Localización

- No existe `Content/Localization`, ni `Config/Localization`, ni StringTables (0 assets `StringTable`), ni `CulturesToStage` o `InternationalizationPreset` en la config.
- Los textos de diálogo, notas y avisos están hardcodeados en `BP_ThirdPersonCharacter` (hay decenas de cadenas de juego, p. ej. "No tienes pilas de repuesto. Busca en el parque."). Los 4 WBP casi no contienen texto propio: lo reciben desde el personaje.
- Si esas cadenas son `FText` en nodos, el Localization Dashboard las puede recolectar. Si son `String` convertidos a Text, **no**. Hay que verificarlo en el BP.
- La fuente de UI (BarlowCondensed) no tiene `%`, flechas U+2191/U+2193 ni cruz U+2715 (87 avisos). Sí cubre las tildes y la ñ (no hay avisos de esos caracteres).
- Recomendación mínima: crear `ST_Textos` (StringTable, namespace `MundoMagico`) y migrar los textos del personaje. Configurar las cultures `es` (nativa) y `en`. Añadir una fuente de respaldo con símbolos o reemplazar los glifos por iconos `T_`.

## 5. Estado de build / packaging

- Último build: `Build_log.txt` (2026-11-02 23:46). `BuildCookRun -platform=Win64 -clientconfig=Shipping -cook -map=/Game/MundoMagico/Maps/L_Pabellon4 -stage -pak -prereqs -archive`, desde la copia `C:\MM_Build\MundoMagico\MundoMagico.uproject` (ruta sin tilde). Resultado: **BUILD SUCCESSFUL**, 0 errores y 0 warnings, 24 min. Salida en `Desktop\MundoMagico_Project\Build\Windows` (2,2 GB, 3.541 archivos UFS).
- El build **está desactualizado**: no incluye los arreglos de hoy (Payasos, WidgetPagina, SetTimer).
- `Config/DefaultGame.ini` solo tiene `ProjectID` y AssetManager. Faltan `ProjectName`, `CompanyName`, `ProjectVersion`, `CopyrightNotice` y la sección `[/Script/UnrealEd.ProjectPackagingSettings]` (`BuildConfiguration`, `bUseIoStore`, `bCompressed`, `ForDistribution`, `MapsToCook`, `DirectoriesToNeverCook`, `CulturesToStage`, `IncludePrerequisites`).
- DefaultEngine: `GameDefaultMap` y `EditorStartupMap` = `L_Pabellon4`. GameMode = plantilla ThirdPerson.
- Hay un `DefaultEngine.ini.bak_basura` dentro de `Config/`. Moverlo fuera para que nadie lo edite por error.
- No hay script de build versionado en el repo (`herramientas/` solo tiene lanzadores del editor). El paso "copiar a C:\MM_Build y ejecutar UAT" no está documentado como script.
- No hay `CHANGELOG.md` en el repo.
- No hay tests del juego (no hay Functional Tests ni Automation specs propios).

## 6. Riesgos

1. **Crítico**: los cuelgues de GPU (TDR) en BasePass con la Intel Iris Plus y 8 GB. Si pasa en el editor, pasará en el .exe de los testers con equipos parecidos. Todavía no se sabe si TdrDelay=10 lo resuelve (la sesión 14.10 y la actual no colgaron).
2. **Alto**: un build de 7 días con bugs ya corregidos podría llegar a testers. No hay versión en el ejecutable ni CHANGELOG para saber qué build tiene cada uno.
3. **Alto**: se cocinan packs completos sin usar (no hay `DirectoriesToNeverCook`): build pesado y cook lento con 8 GB de RAM.
4. **Medio**: escrituras bloqueadas en `MI_Follaje`, `MI_Cerro` y `SM_Arbol_A/B` (Error 32). Puede haber assets sin guardar o corruptos.
5. **Medio**: no hay NavMesh en algún mapa (CrowdManager) y `BP_Giratorio` tiene escala 0: enemigos inmóviles y colisión rota.
6. **Medio**: localización inexistente. Migrar después cuesta más cuantos más textos se sumen.
7. **Bajo**: el repo no versiona los `Config/*.ini` del proyecto UE (están en otra ruta). La config de packaging no tiene respaldo.

## 7. Recomendaciones Sprint 1 (priorizadas)

1. **P0** – Prueba de humo de estabilidad: 3 partidas de 15 min en el .exe con calidad Baja. Registrar en `auditoria/` si hay `DEVICE_HUNG`. Si persiste, que rendering aplique un perfil de bajo coste (ver solicitudes).
2. **P0** – Nuevo build Shipping con los arreglos de hoy. Antes, añadir `ProjectVersion=0.2.0` en `DefaultGame.ini` y crear `CHANGELOG.md` en el repo (formato Keep a Changelog), con la entrada 0.2.0: índice de Payasos, WidgetPagina None y SetTimer 0.
3. **P1** – Versionar `herramientas/Build_MundoMagico.ps1` (copia a `C:\MM_Build` + `RunUAT BuildCookRun` + validación de `ExitCode` + log con fecha).
4. **P1** – `ProjectPackagingSettings`: `MapsToCook=L_Pabellon4`, `DirectoriesToNeverCook` para los packs sin usar (tras confirmar referencias con Reference Viewer/Size Map), `bUseIoStore=True`, `ForDistribution=True` en Shipping.
5. **P1** – Arreglar `BP_Giratorio` (escala 0) y añadir/reconstruir el NavMeshBoundsVolume del mapa donde salta CrowdFollowing.
6. **P2** – Renombrados (13 violaciones) + Fix Up Redirectors en `/Game/MundoMagico`. Mover los BP del personaje y del GameMode fuera de `/ThirdPerson`.
7. **P2** – Localización base: StringTable `ST_Textos`, migrar los textos del personaje y usar una fuente con fallback de símbolos.
8. **P3** – Checklist de QA manual por build (arranque, menú, linterna, páginas, susto, final, salir) y 1 Functional Test de arranque del mapa.
9. **P3** – Excluir `Content/` y `Saved/` del antivirus/OneDrive para evitar los Error 32. Retirar los `.uproject` ajenos y `DefaultEngine.ini.bak_basura`.

## 8. Solicitudes a otros agentes

- **Rendering / tech-art**: analizar los dumps de `Saved/Crashes/UECC-*` de hoy (BasePass/ParallelDraw). Proponer un perfil "Iris Plus" (sin Lumen/Nanite, `r.ScreenPercentage` bajo, VSM off) que respete la memoria "Perfil A pendiente RAM".
- **Gameplay / Blueprints**: confirmar que los SetTimer de `MostrarPagina`/`OcultarPagina` también se corrigieron (aparecían en los logs). Revisar la escala de `BP_Giratorio.Malla` y el NavMesh para `BP_Enemigo`. Decidir si los textos pasan a FText/StringTable.
- **UI / UX**: reemplazar `%`, las flechas y la cruz en los WBP, o añadir una fuente de fallback.
- **Art / Content**: confirmar cuál de los `SKM_Isidora*` es el bueno. Listar los packs que realmente usa `L_Pabellon4` para `DirectoriesToNeverCook`.
- **Producción / lead**: aprobar el esquema de versión (SemVer 0.x) y el CHANGELOG como fuente de verdad de cada build entregado.
