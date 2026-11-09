# Mueve fuera de Content/ los packs de Fab sin referencias (reversible: basta moverlos de vuelta).
# REQUISITO: Unreal cerrado y respaldo hecho (Respaldar_Proyecto.bat).
# Destino: Documents\Unreal Projects\MUNDO_MAGICO_PacksSinUsar\<Pack>
$proy = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO').FullName
$dest = Join-Path (Split-Path $proy) ((Split-Path $proy -Leaf) + '_PacksSinUsar')
$packs = @('UndergroundTunnel','SciFi_Beast06','PunkGirl_Bundle','RoninGirl','DynamicFalling','FreeSurvivalIconsKG','Variant_Platforming')
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) { Write-Host 'Cierra Unreal antes de archivar.'; exit 1 }
New-Item -ItemType Directory -Force $dest | Out-Null
$total = 0
foreach ($p in $packs) {
  $src = Join-Path $proy "Content\$p"
  if (-not (Test-Path $src)) { Write-Host "No esta: $p"; continue }
  $mb = [int]((Get-ChildItem $src -Recurse -File | Measure-Object Length -Sum).Sum / 1MB); $total += $mb
  Move-Item $src (Join-Path $dest $p) -Force
  Write-Host "Archivado: $p ($mb MB)"
}
# Actores externos huerfanos de packs archivados
foreach ($p in $packs) { $e = Join-Path $proy "Content\__ExternalActors__\$p"; if (Test-Path $e) { Move-Item $e (Join-Path $dest "__ExternalActors__$p") -Force } }
Write-Host "Total archivado: $total MB en $dest"
Write-Host 'Para restaurar un pack: moverlo de vuelta a Content\ con Unreal cerrado.'
