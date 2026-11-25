<#
.SYNOPSIS
  Build Shipping (sin consola) y ZIP para testers. Requiere editores y juego cerrados; el mapa ya debe estar generado
  (corre antes Rebuild_And_Package.ps1 con la misma version).
.EXAMPLE
  .\scripts\Empaquetar_Shipping.ps1 -Version 0.1.27
#>
param([Parameter(Mandatory)][string]$Version)
$ErrorActionPreference = 'Stop'
$proj = Split-Path -Parent $PSScriptRoot
$busy = Get-Process UnrealEditor*, Blackline -ErrorAction SilentlyContinue
if ($busy) { throw "Cierra antes: $(($busy | % ProcessName) -join ', ')" }

& "$proj\scripts\Build.ps1" -Config Shipping -Version $Version
$dir = Join-Path $proj "Build\v$Version-Shipping"
if (-not (Test-Path $dir)) { throw "No existe $dir" }
$zip = Join-Path $proj "Build\Blackline_v${Version}_Shipping.zip"
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path "$dir\*" -DestinationPath $zip -CompressionLevel Optimal
"ZIP: $zip ({0:N2} GB)" -f ((Get-Item $zip).Length / 1GB)
