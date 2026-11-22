<#
.SYNOPSIS
  Compila el editor, regenera el mapa/BPs con Python y empaqueta (Development). Requiere TODOS los editores cerrados.
.EXAMPLE
  .\scripts\Rebuild_And_Package.ps1 -Version 0.1.3
#>
param([Parameter(Mandatory)][string]$Version,
      [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8')
$ErrorActionPreference = 'Stop'
$proj = Split-Path -Parent $PSScriptRoot
$busy = Get-Process UnrealEditor*, Blackline -ErrorAction SilentlyContinue
if ($busy) { throw "Cierra antes: $(($busy | % ProcessName) -join ', ')" }

& "$EngineRoot\Engine\Build\BatchFiles\Build.bat" BlacklineEditor Win64 Development "-Project=$proj\Blackline.uproject" -WaitMutex -NoHotReload
if ($LASTEXITCODE -ne 0) { throw "Fallo la compilacion del editor ($LASTEXITCODE)" }

$log = Join-Path $proj 'Saved\build_map_run.log'
& "$EngineRoot\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "$proj\Blackline.uproject" /Engine/Maps/Entry `
  "-ExecutePythonScript=$proj\Content\Python\blackline_build_map.py" -unattended -nullrhi -nosplash -culture=en -stdout -FullStdOutLogOutput *> $log
if (-not (Select-String -Path $log -Pattern 'BLK: FIN OK' -Quiet)) { throw "El script del mapa fallo. Ver $log" }

& "$proj\scripts\Build.ps1" -Config Development -Version $Version
