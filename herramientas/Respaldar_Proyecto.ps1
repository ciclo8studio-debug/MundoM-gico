# Respalda lo propio del proyecto Unreal (no los packs de Fab) en un zip con fecha.
# Funciona aunque Unreal este abierto (lee los archivos en modo compartido), pero conviene Guardar todo antes.
# Destino: Escritorio\MundoMagico_Respaldos\MM_aaaammdd_hhmm.zip  (conserva los ultimos 10)
Add-Type -AssemblyName System.IO.Compression, System.IO.Compression.FileSystem
$proy = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO').FullName
$dest = Join-Path ([Environment]::GetFolderPath('Desktop')) 'MundoMagico_Respaldos'
New-Item -ItemType Directory -Force $dest | Out-Null
$zip = Join-Path $dest ('MM_' + (Get-Date -Format 'yyyyMMdd_HHmm') + '.zip')
$rel = @('Content\MundoMagico','Content\ThirdPerson','Content\Fuentes','Content\Input','Config','Saved\SaveGames')
$files = foreach ($r in $rel) { $p = Join-Path $proy $r; if (Test-Path $p) { Get-ChildItem $p -Recurse -File } }
$files = @($files) + @(Get-ChildItem $proy -File | Where-Object { $_.Name -like 'MUNDO*.uproject' })
$fs = [IO.File]::Open($zip, 'Create')
$za = New-Object IO.Compression.ZipArchive($fs, [IO.Compression.ZipArchiveMode]::Create)
$n = 0; $fallos = 0
foreach ($f in $files) {
  try {
    $entry = $za.CreateEntry($f.FullName.Substring($proy.Length + 1), [IO.Compression.CompressionLevel]::Fastest)
    $in = [IO.File]::Open($f.FullName, 'Open', 'Read', 'ReadWrite, Delete')
    $out = $entry.Open(); $in.CopyTo($out); $out.Dispose(); $in.Dispose(); $n++
  } catch { $fallos++; Write-Warning "No se pudo leer: $($f.FullName)" }
}
$za.Dispose(); $fs.Dispose()
$mb = [math]::Round((Get-Item $zip).Length / 1MB, 1)
Write-Host "Respaldo creado: $zip ($n archivos, $mb MB, $fallos fallos)"
Get-ChildItem $dest -Filter 'MM_*.zip' | Sort-Object LastWriteTime -Descending | Select-Object -Skip 10 | Remove-Item -Confirm:$false
