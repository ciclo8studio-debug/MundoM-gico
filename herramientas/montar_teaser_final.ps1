# Corte final de "El ultimo turno": tramos de la grabacion del atardecer + la toma de la silueta
# del primer teaser + titulo, con Teaser_Vals_corto.wav (39 s).
# Uso: .\herramientas\montar_teaser_final.ps1 -Crudo .\Teaser\crudo.mkv
param([Parameter(Mandatory)][string]$Crudo)
$ErrorActionPreference = 'Stop'
$env:Path = [Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [Environment]::GetEnvironmentVariable('Path','User')
$raiz = Split-Path $PSScriptRoot -Parent
$sal  = Join-Path $raiz 'Teaser'
$f    = { param($n) (Join-Path $raiz "Fuentes_descarga\$n").Replace('\','/').Replace(':','\:') }
$fnt  = & $f 'SpecialElite-Regular.ttf'
$fntB = & $f 'BarlowCondensed-SemiBold.ttf'
$audio = Join-Path $raiz 'MUSICA_PARA_UNREAL\Teaser_Vals_corto.wav'
$viejo = Join-Path $sal 'Teaser_MundoMagico.mp4'

# fuente, inicio, duracion, texto, aplicar etalonaje
$tomas = @(
  @($Crudo, 3.2, 6.0, 'EL PARQUE CERRÓ HACE AÑOS.', $true),
  @($Crudo, 9.2, 6.5, '', $true),
  @($Crudo, 16.0, 6.0, '', $true),
  @($Crudo, 24.0, 6.0, 'PERO NADIE LE AVISÓ A ELLA.', $true),
  @($Crudo, 30.0, 3.3, '', $true),
  @($viejo, 13.0, 4.2, '', $false)
)
$grade  = 'eq=contrast=1.06:saturation=0.9:gamma=1.12,vignette=PI/4.5,noise=alls=6:allf=t'
$letter = 'drawbox=x=0:y=0:w=iw:h=ih*0.11:color=black:t=fill,drawbox=x=0:y=ih*0.89:w=iw:h=ih*0.11:color=black:t=fill'
$lista = @(); $k = 0
foreach ($t in $tomas) {
  $src, $ini, $dur, $txt, $gr = $t
  $vf = 'scale=1280:720,fps=30'
  if ($gr) { $vf += ",$grade,$letter" }
  $fi = if ($k -eq 0) { 1.2 } else { 0.4 }
  $vf += ",fade=t=in:st=0:d=$fi,fade=t=out:st=$($dur-0.35):d=0.35"
  if ($txt) {
    $vf += ",drawtext=fontfile='$fnt':text='$txt':fontcolor=0xE8E2D2:fontsize=h/22:x=(w-tw)/2:y=h*0.80:alpha='if(lt(t,0.8),0,if(lt(t,1.3),(t-0.8)/0.5,if(gt(t,$($dur-0.9)),max(0,($($dur-0.4)-t)/0.5),1)))'"
  }
  $vf += ',format=yuv420p'
  $o = Join-Path $sal "toma$k.mp4"
  ffmpeg -y -loglevel error -ss $ini -t $dur -i $src -vf $vf -an -c:v libx264 -crf 18 -preset medium $o
  $lista += "file '$($o.Replace('\','/'))'"; $k++
}
$card = "color=c=black:s=1280x720:d=7:r=30," +
  "drawtext=fontfile='$fnt':text='MUNDO MÁGICO':fontcolor=0xE8E2D2:fontsize=96:x=(w-tw)/2:y=h*0.36:alpha='if(lt(t,0.8),0,if(lt(t,2),(t-0.8)/1.2,if(gt(t,6),(7-t),1)))'," +
  "drawbox=x=(iw-560)/2:y=ih*0.36+112:w=560:h=3:color=0xB21C18:t=fill:enable='between(t,1.4,6.6)'," +
  "drawtext=fontfile='$fntB':text='T U R N O   N O C T U R N O':fontcolor=0xB21C18:fontsize=30:x=(w-tw)/2:y=h*0.36+128:alpha='if(lt(t,1.6),0,if(lt(t,2.6),(t-1.6),if(gt(t,6),(7-t),1)))'," +
  "drawtext=fontfile='$fntB':text='PRÓXIMAMENTE':fontcolor=0x8C8C87:fontsize=24:x=(w-tw)/2:y=h*0.72:alpha='if(lt(t,3),0,if(lt(t,4),(t-3),if(gt(t,6),(7-t),1)))',format=yuv420p"
$titulo = Join-Path $sal 'titulo.mp4'
ffmpeg -y -loglevel error -f lavfi -i $card -c:v libx264 -crf 18 -preset medium $titulo
$lista += "file '$($titulo.Replace('\','/'))'"
$lf = Join-Path $sal 'lista.txt'; $lista -join "`n" | Set-Content -Encoding ascii $lf
$mudo = Join-Path $sal 'mudo.mp4'
ffmpeg -y -loglevel error -f concat -safe 0 -i $lf -c copy $mudo
$final = Join-Path $sal 'Teaser_ElUltimoTurno.mp4'
ffmpeg -y -loglevel error -i $mudo -i $audio -map 0:v -map 1:a -c:v copy -c:a aac -b:a 192k -shortest $final
Remove-Item (Join-Path $sal 'toma*.mp4'), $titulo, $mudo, $lf -ErrorAction SilentlyContinue
Write-Host "TEASER OK: $final"
