# Monta el teaser a partir de la grabacion cruda del modo -teaser del juego (MundoMagico.exe -teaser).
# Uso: .\herramientas\montar_teaser.ps1 -Crudo .\Teaser\crudo.mkv
# Edita $tomas para elegir los tramos (segundos de la grabacion) y los textos de cada uno.
param([Parameter(Mandatory)][string]$Crudo)
$ErrorActionPreference = 'Stop'
$env:Path = [Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [Environment]::GetEnvironmentVariable('Path','User')
$raiz = Split-Path $PSScriptRoot -Parent
$sal  = Join-Path $raiz 'Teaser'
New-Item -ItemType Directory -Force $sal | Out-Null
$f    = { param($n) (Join-Path $raiz "Fuentes_descarga\$n").Replace('\','/').Replace(':','\:') }
$fnt  = & $f 'SpecialElite-Regular.ttf'
$fntB = & $f 'BarlowCondensed-SemiBold.ttf'
$mus  = Join-Path $raiz 'MUSICA_PARA_UNREAL\Musica_Terror_Menu.wav'
$lat  = Join-Path $raiz 'MUSICA_PARA_UNREAL\SFX_Latido.wav'

# inicio, duracion, texto
$tomas = @(
  @(1.0, 4.5, 'EL PARQUE CERRÓ HACE AÑOS.'),
  @(8.0, 3.8, ''),
  @(25.0, 4.5, 'ALGUIEN SIGUE DE TURNO.'),
  @(11.8, 5.0, '')
)
$grade  = 'eq=contrast=1.06:saturation=0.8:gamma=1.22,vignette=PI/4.5,noise=alls=7:allf=t'
$letter = 'drawbox=x=0:y=0:w=iw:h=ih*0.11:color=black:t=fill,drawbox=x=0:y=ih*0.89:w=iw:h=ih*0.11:color=black:t=fill'
$lista = @(); $k = 0; $total = 0.0
foreach ($t in $tomas) {
  $ini, $dur, $txt = $t
  $vf = "scale=1280:720,fps=30,$grade,fade=t=in:st=0:d=0.5,fade=t=out:st=$($dur-0.5):d=0.5,$letter"
  if ($txt) {
    $vf += ",drawtext=fontfile='$fnt':text='$txt':fontcolor=0xE8E2D2:fontsize=h/22:x=(w-tw)/2:y=h*0.80:alpha='if(lt(t,0.8),0,if(lt(t,1.3),(t-0.8)/0.5,if(gt(t,$($dur-0.9)),max(0,($($dur-0.4)-t)/0.5),1)))'"
  }
  $vf += ',format=yuv420p'
  $o = Join-Path $sal "toma$k.mp4"
  ffmpeg -y -loglevel error -ss $ini -t $dur -i $Crudo -vf $vf -an -c:v libx264 -crf 18 -preset medium $o
  $lista += "file '$($o.Replace('\','/'))'"; $k++; $total += $dur
}

# Tarjeta final (7 s)
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

# Audio: musica de terror; el latido entra en la ultima toma (la silueta) y corta en seco al titulo
$ultima = $tomas[-1][1]; $ent = $total - $ultima - 1.5
$ms = [int]($ent * 1000); $largo = $total + 7
$af = "[1:a]atrim=0:$largo,afade=t=in:st=0:d=2,afade=t=out:st=$($largo-3):d=3,volume=1.6[m];" +
      "[2:a]atrim=0:$(($ultima + 1.5) * 1.3),asetrate=44100*1.3,aresample=44100,adelay=$ms|$ms,afade=t=in:st=$($ent):d=1.5,volume=1.2[h];" +
      "[m][h]amix=inputs=2:duration=first:normalize=0,alimiter=limit=0.9[a]"
$final = Join-Path $sal 'Teaser_MundoMagico.mp4'
ffmpeg -y -loglevel error -i $mudo -i $mus -stream_loop -1 -i $lat -filter_complex $af -map 0:v -map '[a]' -c:v copy -c:a aac -b:a 192k -shortest $final
Remove-Item (Join-Path $sal 'toma*.mp4'), $titulo, $mudo, $lf -ErrorAction SilentlyContinue
Write-Host "TEASER OK: $final ($([math]::Round($largo,1)) s)"
