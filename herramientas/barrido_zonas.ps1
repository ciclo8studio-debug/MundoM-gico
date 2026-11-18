# Barrido de FPS por zona del parque, sin intervencion: abre cada zona con -ir=<zona> en la build Development,
# espera, cierra el juego y calcula los FPS desde los contadores de cuadro del log. Escribe auditoria\html_completo\fps_por_zona.txt
# Uso: powershell -ExecutionPolicy Bypass -File herramientas\barrido_zonas.ps1 [-Build v0.3.70_dev] [-Zonas parque,teatro] [-Seg 70]
param([string]$Build = 'v0.3.70_dev', [string[]]$Zonas = @('parque','teatro','noria','bodega','autos'), [int]$Seg = 70)
$raiz = Split-Path $PSScriptRoot
$exe  = Join-Path $raiz "Build\$Build\Windows\MundoMagico.exe"
$log  = Join-Path $raiz "Build\$Build\Windows\MundoMagico\Saved\Logs\MundoMagico.log"
$crash = Join-Path $env:LOCALAPPDATA 'MundoMagico\Saved\Crashes'
$salida = Join-Path $raiz 'auditoria\html_completo\fps_por_zona.txt'
$filas = @("Barrido de FPS · build $Build · $(Get-Date -Format 'yyyy-MM-dd HH:mm')", '')
foreach ($z in $Zonas) {
    Get-Process MundoMagico* -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep 3
    $ini = Get-Date
    Start-Process $exe "L_Parque -ir=$z -dev -log" | Out-Null
    $t = 0; while ($t -lt $Seg) { Start-Sleep 5; $t += 5 }
    $vivo = [bool](Get-Process MundoMagico* -ErrorAction SilentlyContinue)
    Get-Process MundoMagico* -ErrorAction SilentlyContinue | Stop-Process -Force
    Start-Sleep 3
    $cierre = $false
    if (Test-Path $crash) { $cierre = [bool](Get-ChildItem $crash -Directory | Where-Object { $_.LastWriteTime -gt $ini }) }
    $pts = @()
    if (Test-Path $log) {
        Select-String $log -Pattern '^\[(\d{4})\.(\d\d)\.(\d\d)-(\d\d)\.(\d\d)\.(\d\d):(\d{3})\]\[\s*(\d+)\]' | ForEach-Object {
            $m = $_.Matches[0].Groups
            $pts += [pscustomobject]@{ t = [double]$m[4].Value*3600 + [double]$m[5].Value*60 + [double]$m[6].Value + [double]$m[7].Value/1000; fr = [int]$m[8].Value }
        }
    }
    $u = $pts | Sort-Object fr -Unique
    $fps = 'sin datos'
    if ($u.Count -ge 2) {
        $t0 = $u[0].t + 30
        $ev = $u | Where-Object { $_.t -ge $t0 }
        if ($ev.Count -ge 2) { $fps = '{0:N1}' -f (($ev[-1].fr - $ev[0].fr) / ($ev[-1].t - $ev[0].t)) }
    }
    $estado = if ($cierre) { 'CIERRE (GPUCrash)' } elseif ($vivo) { 'estable' } else { 'se cerró solo' }
    $filas += ("{0,-8} FPS {1,6}   {2}" -f $z, $fps, $estado)
}
Get-Process MundoMagico* -ErrorAction SilentlyContinue | Stop-Process -Force
$filas | Set-Content $salida -Encoding utf8
$filas
