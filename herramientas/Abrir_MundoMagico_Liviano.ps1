# Abre Mundo Magico en modo liviano para la Intel Iris Plus.
# Apaga trazado de rayos (y con eso MegaLights), sombras virtuales y Lumen SOLO mientras Unreal este abierto.
# No modifica DefaultEngine.ini ni ninguna configuracion guardada.
$p = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO\MUNDO_M*GICO.uproject').FullName
$cv = @(
  'r.RayTracing.Enable=0',
  'r.Shadow.Virtual.Enable=0',
  'r.DynamicGlobalIlluminationMethod=0',
  'r.ReflectionMethod=2',
  'r.Lumen.DiffuseIndirect.Allow=0',
  'r.Lumen.Reflections.Allow=0',
  'r.VolumetricFog.GridPixelSize=16',
  'r.VolumetricFog.GridSizeZ=64'
) -join ','
$a = @("`"$p`"", '/Game/MundoMagico/Maps/L_Hospital', '-dx11', '-ModelContextProtocolStartServer', "-dpcvars=$cv")
Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' -ArgumentList $a
