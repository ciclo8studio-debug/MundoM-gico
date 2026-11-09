# Guarda una version del proyecto Unreal en GitHub (rama "unreal", con Git LFS).
# Uso: herramientas\Versionar_Unreal.bat  [mensaje opcional]   (conviene Guardar todo en Unreal antes)
param([string]$Mensaje = '')
$proy = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO').FullName
Set-Location $proy
if (-not $Mensaje) { $Mensaje = 'Cambios ' + (Get-Date -Format 'yyyy-MM-dd HH:mm') }
git add -A
$cambios = git diff --cached --name-only
if (-not $cambios) { Write-Host 'No hay cambios para versionar.'; exit 0 }
Write-Host ("Archivos cambiados: " + @($cambios).Count)
git commit -q -m $Mensaje
git push -q origin unreal
if ($LASTEXITCODE -eq 0) { Write-Host "Version subida a GitHub (rama unreal): $Mensaje" } else { Write-Host 'Fallo el push; revisa la conexion.' }
