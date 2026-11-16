# Sube a GitHub, en un solo paso, los commits pendientes de los dos repositorios:
#   1) scripts  (esta carpeta)             2) proyecto Unreal (rama "unreal", con Git LFS)
# No crea commits: solo sube lo que ya esta guardado. Cierra Unreal y el juego antes.
# Uso: doble clic en herramientas\Subir_Todo.bat
$scripts = Split-Path $PSScriptRoot -Parent
$unreal  = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO').FullName
$fallos  = 0

function Subir($nombre, $ruta, [switch]$Lfs) {
    Write-Host ""
    Write-Host "=== $nombre ===" -ForegroundColor Cyan
    $rama = git -C $ruta rev-parse --abbrev-ref HEAD
    if ($Lfs) {
        # Primero los binarios: si GitHub no los tiene, rechaza los commits (error GH008).
        Write-Host "Subiendo binarios (LFS)..."
        git -C $ruta lfs push --all origin | Out-Host
        if ($LASTEXITCODE -ne 0) { Write-Host "FALLO la subida de binarios. Copia el mensaje de arriba y pasaselo a Claude." -ForegroundColor Red; return 1 }
    }
    Write-Host "Subiendo commits de la rama $rama..."
    git -C $ruta push origin $rama | Out-Host
    if ($LASTEXITCODE -ne 0) { Write-Host "FALLO el push. Copia el mensaje de arriba y pasaselo a Claude." -ForegroundColor Red; return 1 }
    # Comprobacion: el commit local debe ser el mismo que quedo en GitHub.
    $local  = git -C $ruta rev-parse HEAD
    $remoto = (git -C $ruta ls-remote origin "refs/heads/$rama") -split "\s+" | Select-Object -First 1
    if ($local -eq $remoto) { Write-Host "OK: $nombre al dia en GitHub ($($local.Substring(0,7)))" -ForegroundColor Green; return 0 }
    Write-Host "OJO: GitHub no quedo igual que tu PC. Avisale a Claude." -ForegroundColor Yellow; return 1
}

if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) { Write-Host 'Cierra Unreal antes de subir.' -ForegroundColor Yellow; exit 1 }
$fallos += Subir 'Scripts' $scripts
$fallos += Subir 'Proyecto Unreal' $unreal -Lfs
Write-Host ""
if ($fallos -eq 0) { Write-Host 'TODO SUBIDO.' -ForegroundColor Green } else { Write-Host "Hubo $fallos repositorio(s) con problemas." -ForegroundColor Red }
exit $fallos
