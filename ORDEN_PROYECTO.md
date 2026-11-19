# ORDEN_PROYECTO (auditoría solo lectura, 19-11-2026)

Alcance: proyecto Unreal + repo de scripts. Nada se movió ni borró. Referencias medidas con grep binario de `/Game/<pack>/` desde `MundoMagico`, `ThirdPerson`, `Input`, `Meshes`, `Materials`, `Textures`.

## (a) Estado actual

| Zona | Hallazgo |
|---|---|
| `Content/` | 36 carpetas; 189 MB propios (`MundoMagico`) frente a ~33 GB de packs Fab. Horror_Hospital 19 GB, DetectiveOffice 2,6 GB, UnrealBucket 2,5 GB, SoulCity 1,6 GB, Apocalypse y GV_FreeShrubsPack 1,4 GB c/u |
| Packs en uso | Casi todos los mapas y BPs referencian alguno. **Sin referencias desde lo propio: `HeartBeats` (158 MB) y `AutoFootsteps` (5 MB)**; `Collections`, `Developers`, `Maps/L_Overview` (Content raíz) son restos |
| Mapas del juego | `MundoMagico/Maps`: L_Pabellon4 (5,6 MB, congelado), L_Hospital (1,2 MB, **GameDefaultMap y EditorStartupMap**), L_Parque (4,6 MB), L_HospitalGrande (0,25 MB) y L_Casona (0,16 MB). **L_Casona ya existe** (la auditoría de mapas de hoy dice que no: está desactualizada) |
| Mapas ajenos | 24 `.umap` de packs (demos, Overview) dentro de `Content`. No se cocinan porque `-map=` lista solo 4, pero ensucian el Content Browser |
| Cook | `Construir_Version.ps1`: L_Hospital + L_Parque + L_HospitalGrande + L_Casona. L_Pabellon4 queda fuera, **aunque `L_Hospital` y otros BP aún dependen de sus assets**. `DirectoriesToAlwaysCook` solo tiene `/Game/MundoMagico/Datos` |
| Duplicados | `BP_ThirdPersonCharacter` en `ThirdPerson` (el del juego, `GlobalDefaultGameMode` apunta a `/Game/ThirdPerson/...`), `FlashLight_System` y `Horror_Template` (este último con carpeta mal escrita `Blueprinnts`). El BP del juego referencia FlashLight_System, UnrealBucket, Essential_Foosteps y Thunder_Sounds. Audio duplicado en `MundoMagico/Audio`: `Ambiente_Parque`/`AMB_Parque`, `Ambiente_T_Hospital`/`AMB_T_Hospital`, `Musica_Menu`/`MUS_Menu`, `Musica_Persecucion`/`MUS_Persecucion`, `MUS_Ambiente_Terror` |
| `Config/` | `DefaultEngine.ini.antes_sprint1`, `DefaultGame.ini.antes_sprint1`, `DefaultEngine.ini.bak_basura` (ignorados por git, pero sueltos). `HoloLens/` y `DefaultNiagara.ini` innecesarios para Win64. `GlobalDefaultGameMode` usa `ThirdPerson`, nombre heredado de plantilla |
| Raíz del proyecto | `AnimatedWoodenCrate.png`, `UB_FreePack.png` sueltos (no versionados). `Saved` 274 MB (.tmp de BP_Bosque, autosaves), `Intermediate` 163 MB: no versionados (lista blanca en `.gitignore`), bien. `Fuentes/` en la raíz (49 MB) duplica a `Content/Fuentes` |
| Repo de scripts | Raíz con ~80 `Build_log_*.txt`, `Build/` (49 GB, ignorado), exe de Intel, zip de 2026-11-01, `confirgutacion.ini` y `.txt` (typo), `import_fuentes.*`, 10 docs `REUNION_*`/`PLAN_*`, 3 respaldos y carpetas de SFX/música sueltas |
| Prefijos | Mayoría correcta (BP_, WBP_, MI_, ATT_, RV_). Inconsistentes: audio (`Ambiente_`/`AMB_`, `Musica_`/`MUS_`, `FFE_`), `IES_Linterna`. Sin carpetas vacías dentro de `MundoMagico` |

## (b) Estructura objetivo

```
Content/
  MundoMagico/          # todo lo propio
    Maps/               # L_Pabellon4 -> Maps/_Archivo/, resto igual
    Core, Blueprints, IA, Mundo, Parque, Isidora, UI, Audio, FX, Luces, Datos, Paginas, Materials, Meshes, Arboles, Componentes
    Input/ Fuentes/ Textures/   # mover desde la raíz de Content
    Player/             # BP_ThirdPersonCharacter + GameMode (renombrar a BP_Jugador, BP_ModoJuego)
  _Packs/               # (EXTERNO al cook) Fab agrupados, sin tocar contenido
  _Packs_Archivados/    # fuera de Content/ (ya existe Archivar_Packs.ps1)
Config/                 # solo ini activos; backups en ../Respaldos
repo scripts/
  docs/ (reuniones, planes, auditoria) | herramientas/ | logs/ (ignorado) | archivo/ (zip, exe, respaldos)
```

## (c) Acciones

| # | Acción | Clase | Tamaño / evidencia |
|---|---|---|---|
| 1 | Mover `Build_log_*.txt`, `import_fuentes.*`, `confirgutacion.*` a `logs/` y agregar `logs/` al `.gitignore` | SEGURO | ~80 archivos, trivial |
| 2 | Mover `REUNION_*`, `PLAN_*`, `AUDIT_REPORT.md` a `docs/historico/` (actualizar enlaces de `CONTEXTO_ESTUDIO.md`) | SEGURO | solo docs |
| 3 | Corregir `auditoria/mapas_2026-11-19.md`: L_Casona sí existe; agregar L_HospitalGrande | SEGURO | evidencia: `ls Maps` |
| 4 | Mover `Config/*.antes_*`, `*.bak_basura` a `../Respaldos_Config` | SEGURO | ya ignorados por git |
| 5 | Mover PNG sueltos de raíz del proyecto y el zip/exe de la raíz del repo a `archivo/` | SEGURO | ~150 KB; exe y zip fuera de git |
| 6 | Agregar `DirectoriesToAlwaysCook` solo si hace falta; **no** tocar `-map=` hasta decidir L_Pabellon4 | SEGURO | `DefaultGame.ini` |
| 7 | Renombrar audio duplicado: unificar `AMB_*`, `MUS_*`; mover `Ambiente_*`/`Musica_*` (verificar referencias antes) | REQUIERE EDITOR | usar Fix Up Redirectors; **no** borrar hasta confirmar 0 refs |
| 8 | Mover `Content/Input`, `Fuentes`, `Textures`, `Materials`, `Meshes` bajo `MundoMagico/` y renombrar `ThirdPerson` a `MundoMagico/Player` (ajustar `GlobalDefaultGameMode`) | REQUIERE EDITOR | muchos BPs apuntan ahí; riesgo medio |
| 9 | Mover packs a `_Packs/` | REQUIERE EDITOR | **no recomendado antes del 10-12**: rompe rutas en 4 mapas; mejor dejarlo |
| 10 | Archivar `L_Pabellon4` a `Maps/_Archivo` y sacar del cook | REQUIERE EDITOR | 5,6 MB; congelado, no está en `-map=` |
| 11 | Limpiar los 5 actores del parque en L_Hospital y 12 actores en el origen de L_Parque (según auditoría de mapas) | REQUIERE EDITOR | ver `auditoria/mapas_2026-11-19.md` |
| 12 | Borrar `Content/HeartBeats` | DESTRUCTIVO | 158 MB; 0 refs en propios. Falta verificar refs entre packs (p. ej. desde FlashLight/Horror_Template) |
| 13 | Borrar `Content/AutoFootsteps` | DESTRUCTIVO | 5 MB; 0 refs en propios; mismo caveat |
| 14 | Borrar los 24 `.umap` de demos de packs | DESTRUCTIVO | no se cocinan; ahorro de espacio mínimo. Opcional |
| 15 | Borrar `Fuentes/` de la raíz del proyecto, `Saved/*.tmp` y `Intermediate` | DESTRUCTIVO | 49 MB / 274 MB / 163 MB; confirmar que `Content/Fuentes` es el definitivo |
| 16 | Borrar `Build/` antiguas (conservar la última y rc) | DESTRUCTIVO | 49 GB en total; borrar solo versiones previas |
| 17 | Borrar `Content/Horror_Template` y `FlashLight_System/Maps` | DESTRUCTIVO | **NO**: el juego los referencia (`BP_AI_Sombra`, `BP_PayasoPersecucion`, `L_Hospital`, BP del jugador) |

## (d) Orden de ejecución (1 commit cada uno, repo de scripts salvo indicación)

1. Acciones 1 y 5: logs y archivos sueltos fuera de la raíz (`.gitignore`).
2. Acción 2: docs históricos a `docs/historico/` + actualizar `CONTEXTO_ESTUDIO.md`.
3. Acción 3: corregir la auditoría de mapas.
4. Acción 4 (fuera de git): respaldar y mover backups de `Config`.
5. Decisión CTO: L_Pabellon4 (acciones 10 y 11). En la **sesión de editor del día**: limpiar actores y archivar el mapa, `save_assets []`, 1 commit en el repo `unreal`.
6. Acciones 12, 13, 15 solo con confirmación explícita del CTO y tras grep de refs entre packs.
7. **Post 10-12:** acciones 7, 8, 9 y 14 (reorganización profunda con redirectors).

## (e) Riesgos para el 10-12 (rc1 25-11)

| Riesgo | Mitigación |
|---|---|
| Mover assets rompe referencias y deja pantallas negras en la build | Nada de mover carpetas de assets antes del rc1; solo acciones SEGURO y limpieza de actores |
| Cambiar `GlobalDefaultGameMode` / `ThirdPerson` rompe el jugador | Dejar para después del 10-12 |
| Sacar L_Pabellon4 del Content con dependencias vivas en L_Hospital | Verificar con Reference Viewer antes de archivar |
| Borrar un pack "sin uso" que usa otro pack (HeartBeats/AutoFootsteps) | Grep cruzado entre packs + un build Development de verificación |
| Más de una sesión de editor para el orden (8 GB RAM) | Agrupar las acciones del editor en una sola sesión, la del build del día |
| `Probar.bat` cae al mapa por defecto en Shipping | Ya corregido con ruta completa; **verificar en el siguiente build** |


## Ejecutado el 19-11 (solo acciones SEGURO)
- [x] 1 · `Build_log_*`, `import_fuentes.*`, `confirgutacion.*` -> `logs/` (ignorado). `Construir_Version.ps1` escribe ahora en `logs/`.
- [x] 2 · `REUNION_*`, `PLAN_ESTUDIO`, `PLAN_MEJORA_RAPIDA`, `AUDIT_REPORT` -> `docs/historico/`, con enlaces actualizados.
- [x] 3 · `auditoria/mapas_2026-11-19.md` al día (L_HospitalGrande y L_Casona existen).
- [x] 4 · Backups de `Config` y 2 PNG sueltos -> `Documents/Unreal Projects/Respaldos_Config` (se movieron, no se borraron).
- [x] 5 · zip y exe sueltos -> `archivo/` (ignorado).
- [ ] 6 · sin acción (no se toca `-map=` hasta decidir `L_Pabellon4`).
- Pendiente (REQUIERE EDITOR / DESTRUCTIVO): después del 10-12 o con OK expreso.
