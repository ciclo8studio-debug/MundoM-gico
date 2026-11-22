# 16 — Build y CI

Motor: UE 5.8.4 (C++, módulo `Blackline`). Plataforma: Windows (Win64). Configuraciones: Development y Shipping.

## Requisitos
- UE 5.8.4 instalado; ruta en variable `UE_ROOT` (por defecto `C:\Program Files\Epic Games\UE_5.8`).
- Visual Studio 2022 con carga "Desarrollo de juegos con C++" y Windows SDK (versión exacta según `docs/engine-reference/unreal/VERSION.md`; `// VERIFY-5.8`).
- Git LFS (`git lfs install`). Los `.uasset`/`.umap` y arte van por LFS (`.gitattributes`).
- `Blackline.uproject` en la raíz de `BLACKLINE_NightHaul/` (pendiente de crear; el script falla claro si falta).

## Uso (un comando)
```
.\scripts\Build.ps1 -Config Development -Version 0.1.0
.\scripts\Build.ps1 -Config Shipping    -Version 0.1.0
.\scripts\Build.ps1 -Config Development -CompileOnly   # solo UBT, rápido (CI de PR)
.\scripts\Build.ps1 -Config Development -Clean         # limpia Intermediate/Binaries
```
- Compila, hace cook, stage, pak y archiva en `Build\v<versión>-<config>\` (ignorado por git).
- Logs en `Saved\BuildLogs\`.
- Avisa si el motor no es 5.8.4. Termina con código distinto de 0 si algo falla.
- Internamente: `RunUAT.bat BuildCookRun -platform=Win64 -clientconfig=<Config> -build -cook -stage -pak -archive`.
  `-CompileOnly` usa `Build.bat Blackline Win64 <Config>`.

## Diferencias de configuración
| | Development | Shipping |
|---|---|---|
| Consola / `stat` / logs | sí | no |
| Debug info | sí | `-nodebuginfo` |
| Uso | QA, testers internos | demo/release |

`UE_LOG(LogBlackline, ...)` en Shipping solo emite lo que el código deje activo; no depender de logs para lógica.

## CI (propuesta, trunk-based)
- `main` protegida; ramas cortas por PR.
- Cada PR: `Build.ps1 -Config Development -CompileOnly` (gate de compilación).
- Cada merge a `main`: build Development completo + artefacto.
- Tag `v<x.y.z>` en `main`: build Shipping, artefacto zip, retención de los últimos 5 tags.
- Runner: máquina Windows autoalojada con UE 5.8.4 y VS (los runners en la nube no traen UE). Decisión de infraestructura: pendiente de aprobación del technical-director.
- Pruebas automáticas (Automation Tests / QA): por definir con `qa-lead`; se añadirá un paso `-ExecCmds="Automation RunTests Blackline"` cuando existan.

## Pendientes / dudas
- Crear `Blackline.uproject`, target `Blackline.Target.cs` y `BlacklineEditor.Target.cs`.
- Confirmar presets de empaquetado (mapas a cocinar en `DefaultGame.ini`).
- Firma y distribución: sin secretos en el repo; definir dónde viven antes de publicar.
- Flags de 5.8.4 de UAT no verificados contra el motor instalado (`// VERIFY-5.8`).
