# Empaqueta una build para itch.io: Build\v<Version>\Windows -> Publicar\MundoMagico_Demo_v<Version>_Win64.zip
# Uso: .\herramientas\Empaquetar_Demo.ps1 -Version 0.3.35
# Usa tar (incluido en Windows 10/11), que soporta ZIP64 para archivos de mas de 4 GB (el .pak).
param([Parameter(Mandatory)][string]$Version)
$ErrorActionPreference = 'Stop'
$raiz = Split-Path $PSScriptRoot -Parent
$src  = Join-Path $raiz "Build\v$Version\Windows"
if (-not (Test-Path (Join-Path $src 'MundoMagico.exe'))) { throw "No existe la build: $src" }
$dst = Join-Path $raiz 'Publicar'
New-Item -ItemType Directory -Force $dst | Out-Null
$zip = Join-Path $dst "MundoMagico_Demo_v$($Version)_Win64.zip"
if (Test-Path $zip) { Remove-Item $zip }
Push-Location $src
tar -a -c -f $zip --exclude=*.pdb *   # el .pdb (simbolos de depuracion, ~175 MB) no va en la demo
Pop-Location
$mb = [math]::Round((Get-Item $zip).Length / 1MB)
Write-Host "ZIP OK: $zip ($mb MB)"
if ($mb -gt 1000) { Write-Host "Pesa mas de 1 GB: subelo con butler (itch.io no acepta archivos tan grandes desde el navegador)." }
