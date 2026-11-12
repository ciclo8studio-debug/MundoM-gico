# Empaqueta el juego (Shipping Win64) sin abrir el editor.
# Uso: herramientas\Construir_Version.bat  (Unreal cerrado; tarda 1-3 h la primera vez en este PC)
# 1) Sincroniza el proyecto a C:\MM_Build\MundoMagico (ruta sin tildes: UAT falla con "MUNDO_MÁGICO")
# 2) Ejecuta BuildCookRun y deja el juego en MundoMagico_Project\Build\v<version>\
$proy = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO').FullName
$bld  = 'C:\MM_Build\MundoMagico'
$repo = Split-Path $PSScriptRoot
if (Get-Process UnrealEditor -ErrorAction SilentlyContinue) { Write-Host 'Cierra Unreal antes de construir.'; exit 1 }
$ver = (Select-String -Path "$proy\Config\DefaultGame.ini" -Pattern '^ProjectVersion=(.+)$').Matches.Groups[1].Value
if (-not $ver) { $ver = 'sin_version' }
$out = Join-Path $repo "Build\v$ver"
Write-Host "Version $ver -> $out"
New-Item -ItemType Directory -Force $bld | Out-Null
robocopy "$proy\Content" "$bld\Content" /MIR /NFL /NDL /NJH /NP /R:1 /W:1 | Out-Null
robocopy "$proy\Config"  "$bld\Config"  /MIR /NFL /NDL /NJH /NP /R:1 /W:1 | Out-Null
if (Test-Path "$proy\Fuentes") { robocopy "$proy\Fuentes" "$bld\Fuentes" /MIR /NFL /NDL /NJH /NP | Out-Null }
# Copia binaria del .uproject (Set-Content agrega BOM y UAT falla)
$up = Get-ChildItem $proy -Filter 'MUNDO*.uproject' | Select-Object -First 1
[IO.File]::WriteAllBytes("$bld\MundoMagico.uproject", [IO.File]::ReadAllBytes($up.FullName))
$uat = 'C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\RunUAT.bat'
$log = Join-Path $repo "Build_log_v$ver.txt"
& $uat BuildCookRun "-project=$bld\MundoMagico.uproject" -noP4 -platform=Win64 -clientconfig=Shipping -build -cook `
  -map=/Game/MundoMagico/Maps/L_Hospital+/Game/MundoMagico/Maps/L_Parque -stage -pak -prereqs -archive "-archivedirectory=$out" `
  -nocompileeditor -skipbuildeditor "-unrealexe=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  -utf8output -unattended *> $log
if ($LASTEXITCODE -eq 0) { Write-Host "BUILD OK: $out\Windows" } else { Write-Host "BUILD FALLO (codigo $LASTEXITCODE). Revisa $log" }
exit $LASTEXITCODE
