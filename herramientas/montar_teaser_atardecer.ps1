# Monta el teaser "El ultimo turno" (atardecer -> noche) desde la grabacion del modo -teaser.
# Uso: .\herramientas\montar_teaser_atardecer.ps1 -Crudo .\Teaser\crudo.mkv -Inicio <s>
#   -Inicio: segundo de la grabacion donde empieza la toma 1. Las tomas duran 36 s en total.
# Audio: MUSICA_PARA_UNREAL\Teaser_Vals.wav (43 s, de herramientas\componer_teaser_vals.py).
param([Parameter(Mandatory)][string]$Crudo, [Parameter(Mandatory)][double]$Inicio)
$ErrorActionPreference = 'Stop'
$env:Path = [Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [Environment]::GetEnvironmentVariable('Path','User')
$raiz = Split-Path $PSScriptRoot -Parent
$sal  = Join-Path $raiz 'Teaser'
$f    = { param($n) (Join-Path $raiz "Fuentes_descarga\$n").Replace('\','/').Replace(':','\:') }
$fnt  = & $f 'SpecialElite-Regular.ttf'
$fntB = & $f 'BarlowCondensed-SemiBold.ttf'
$audio = Join-Path $raiz 'MUSICA_PARA_UNREAL\Teaser_Vals.wav'

$cortes = @(0, 6, 12, 17, 21.5, 27, 30.5, 36)
$fades = @()
for ($i = 0; $i -lt $cortes.Count - 1; $i++) {
  $a = $cortes[$i]; $b = $cortes[$i+1]
  if ($i -gt 0) { $fades += "fade=t=in:st=$($a):d=0.4" }
  $fades += "fade=t=out:st=$($b - 0.35):d=0.35"
}
$fades[0] = "fade=t=in:st=0:d=1.2," + $fades[0]
$grade  = 'eq=contrast=1.06:saturation=0.9:gamma=1.1,vignette=PI/4.5,noise=alls=6:allf=t'
$letter = 'drawbox=x=0:y=0:w=iw:h=ih*0.11:color=black:t=fill,drawbox=x=0:y=ih*0.89:w=iw:h=ih*0.11:color=black:t=fill'
function Texto($txt, $a, $b) {
  "drawtext=fontfile='$fnt':text='$txt':fontcolor=0xE8E2D2:fontsize=h/22:x=(w-tw)/2:y=h*0.80:enable='between(t,$a,$b)':alpha='if(lt(t,$($a+0.5)),(t-$a)/0.5,if(gt(t,$($b-0.5)),($b-t)/0.5,1))'"
}
$t1 = Texto 'EL PARQUE CERRÓ HACE AÑOS.' 1.5 5.4
$t2 = Texto 'PERO NADIE LE AVISÓ A ELLA.' 22.3 26.6
$vf = (@("trim=start=$($Inicio):duration=36", 'setpts=PTS-STARTPTS', 'scale=1280:720', 'fps=30', $grade) + $fades + @($letter, $t1, $t2, 'format=yuv420p')) -join ','

$card = "color=c=black:s=1280x720:d=7:r=30," +
  "drawtext=fontfile='$fnt':text='MUNDO MÁGICO':fontcolor=0xE8E2D2:fontsize=96:x=(w-tw)/2:y=h*0.36:alpha='if(lt(t,0.8),0,if(lt(t,2),(t-0.8)/1.2,if(gt(t,6),(7-t),1)))'," +
  "drawbox=x=(iw-560)/2:y=ih*0.36+112:w=560:h=3:color=0xB21C18:t=fill:enable='between(t,1.4,6.6)'," +
  "drawtext=fontfile='$fntB':text='T U R N O   N O C T U R N O':fontcolor=0xB21C18:fontsize=30:x=(w-tw)/2:y=h*0.36+128:alpha='if(lt(t,1.6),0,if(lt(t,2.6),(t-1.6),if(gt(t,6),(7-t),1)))'," +
  "drawtext=fontfile='$fntB':text='PRÓXIMAMENTE':fontcolor=0x8C8C87:fontsize=24:x=(w-tw)/2:y=h*0.72:alpha='if(lt(t,3),0,if(lt(t,4),(t-3),if(gt(t,6),(7-t),1)))',format=yuv420p"

$cuerpo = Join-Path $sal 'cuerpo.mp4'; $titulo = Join-Path $sal 'titulo.mp4'; $mudo = Join-Path $sal 'mudo.mp4'
ffmpeg -y -loglevel error -i $Crudo -vf $vf -an -c:v libx264 -crf 18 -preset medium $cuerpo
ffmpeg -y -loglevel error -f lavfi -i $card -c:v libx264 -crf 18 -preset medium $titulo
$lf = Join-Path $sal 'lista.txt'
"file '$($cuerpo.Replace('\','/'))'`nfile '$($titulo.Replace('\','/'))'" | Set-Content -Encoding ascii $lf
ffmpeg -y -loglevel error -f concat -safe 0 -i $lf -c copy $mudo
$final = Join-Path $sal 'Teaser_ElUltimoTurno.mp4'
ffmpeg -y -loglevel error -i $mudo -i $audio -map 0:v -map 1:a -c:v copy -c:a aac -b:a 192k -shortest $final
Remove-Item $cuerpo, $titulo, $mudo, $lf -ErrorAction SilentlyContinue
Write-Host "TEASER OK: $final"
