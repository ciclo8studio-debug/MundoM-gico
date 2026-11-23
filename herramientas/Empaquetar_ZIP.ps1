# Comprime una build de Build\ en un ZIP (para testers o itch.io) y muestra su peso.
# Uso:
#   .\herramientas\Empaquetar_ZIP.ps1                 -> la build mas nueva sin _dev (Shipping)
#   .\herramientas\Empaquetar_ZIP.ps1 -Version v0.3.87
# Deja el ZIP en Build\zip\MundoMagico_<version>.zip. No borra ni modifica la build.
param([string]$Version = '')
$raiz  = Split-Path -Parent $PSScriptRoot
$build = Join-Path $raiz 'Build'
function Get-Ver($n) { $v = ($n -replace '^v','' -replace '_dev$','').Split('.'); [version]::new([int]$v[0],[int]$v[1],[int]$v[2]) }
if (-not $Version) {
  $d = Get-ChildItem $build -Directory | Where-Object { $_.Name -match '^v\d+\.\d+\.\d+$' } | Sort-Object { Get-Ver $_.Name } -Descending | Select-Object -First 1
  if (-not $d) { Write-Host "No hay builds Shipping en Build\."; exit 1 }
  $Version = $d.Name
}
$origen = Join-Path $build "$Version\Windows"
if (-not (Test-Path (Join-Path $origen 'MundoMagico.exe'))) { Write-Host "No encuentro $origen\MundoMagico.exe"; exit 1 }
$destDir = Join-Path $build 'zip'
New-Item -ItemType Directory -Force $destDir | Out-Null
$zip = Join-Path $destDir "MundoMagico_$Version.zip"
if (Test-Path $zip) { Remove-Item $zip -Force }
Write-Host "Comprimiendo $origen (puede tardar varios minutos)..."
$sw = [Diagnostics.Stopwatch]::StartNew()
Compress-Archive -Path (Join-Path $origen '*') -DestinationPath $zip -CompressionLevel Optimal
$peso = [math]::Round((Get-Item $zip).Length / 1GB, 2)
Write-Host ("Listo: {0}  ({1} GB, {2:N0} s). Meta: cerca de 1,3 GB." -f $zip, $peso, $sw.Elapsed.TotalSeconds)
