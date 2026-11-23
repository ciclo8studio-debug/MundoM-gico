# Lista (y, solo si lo pides, borra) las builds viejas de Build\ para recuperar disco.
# Por defecto NO borra nada: muestra que conservaria y que borraria, con el peso de cada carpeta.
# Uso:
#   .\herramientas\Limpiar_Builds.ps1                      -> solo la lista
#   .\herramientas\Limpiar_Builds.ps1 -Borrar              -> borra lo que quedo en la lista, pidiendo confirmacion
#   .\herramientas\Limpiar_Builds.ps1 -ConservarShipping 3 -ConservarDev 2 -Extra v0.3.1
param(
  [int]$ConservarShipping = 3,   # builds sin _dev mas nuevas
  [int]$ConservarDev      = 2,   # builds _dev mas nuevas
  [string[]]$Extra        = @(), # carpetas que se conservan ademas (por ejemplo v0.3.1, la primera demo validada)
  [switch]$Borrar
)
$raiz  = Split-Path -Parent $PSScriptRoot
$build = Join-Path $raiz 'Build'
function Get-Ver($n) { $v = ($n -replace '^v','' -replace '_dev$','').Split('.'); [version]::new([int]$v[0],[int]$v[1],[int]$v[2]) }
$todas = Get-ChildItem $build -Directory | Where-Object { $_.Name -match '^v\d+\.\d+\.\d+(_dev)?$' } |
         Sort-Object { Get-Ver $_.Name } -Descending
$ship = $todas | Where-Object { $_.Name -notlike '*_dev' }
$dev  = $todas | Where-Object { $_.Name -like '*_dev' }
$guardar = @()
$guardar += $ship | Select-Object -First $ConservarShipping
$guardar += $dev  | Select-Object -First $ConservarDev
$guardar += $todas | Where-Object { $Extra -contains $_.Name }
$guardarNombres = $guardar.Name | Sort-Object -Unique
$lista = $todas | Where-Object { $guardarNombres -notcontains $_.Name }
function Tam($d) { [math]::Round((Get-ChildItem $d.FullName -Recurse -File -ErrorAction SilentlyContinue | Measure-Object Length -Sum).Sum / 1GB, 2) }
Write-Host "`nSe CONSERVAN:" -ForegroundColor Green
$guardar | Sort-Object { Get-Ver $_.Name } -Descending -Unique | ForEach-Object { "  {0,-16} {1,6} GB" -f $_.Name, (Tam $_) }
Write-Host "`nSe BORRARIAN:" -ForegroundColor Yellow
$total = 0
$lista | ForEach-Object { $t = Tam $_; $total += $t; "  {0,-16} {1,6} GB" -f $_.Name, $t }
Write-Host ("`nTotal a liberar: {0:N1} GB en {1} carpetas." -f $total, $lista.Count)
if (-not $Borrar) { Write-Host "Modo lista: no se borro nada. Agrega -Borrar para borrar."; exit 0 }
$r = Read-Host "Escribe BORRAR para eliminar estas $($lista.Count) carpetas (no hay vuelta atras)"
if ($r -ceq 'BORRAR') { $lista | ForEach-Object { Remove-Item $_.FullName -Recurse -Force; "borrada $($_.Name)" } } else { Write-Host "Cancelado." }
