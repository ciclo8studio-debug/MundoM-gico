# Junta logs y reportes de cierre del juego en un ZIP en el Escritorio (para adjuntar a un reporte de bug).
# Solo lee %LOCALAPPDATA%\MundoMagico\Saved\{Logs,Crashes,Config}; no borra nada.
$saved = Join-Path $env:LOCALAPPDATA 'MundoMagico\Saved'
$dirs  = 'Logs','Crashes','Config' | ForEach-Object { Join-Path $saved $_ } | Where-Object { Test-Path $_ }
if (-not $dirs) { Write-Host "No hay logs ni cierres en $saved"; exit 0 }
$ts  = Get-Date -Format 'yyyyMMdd_HHmmss'
$zip = Join-Path ([Environment]::GetFolderPath('Desktop')) "MundoMagico_logs_$ts.zip"
Compress-Archive -Path $dirs -DestinationPath $zip -Force
Write-Host "Listo: $zip"
