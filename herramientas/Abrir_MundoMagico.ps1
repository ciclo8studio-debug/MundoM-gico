$p = (Get-Item 'C:\Users\pansh\Documents\Unreal Projects\MUNDO_M*GICO\MUNDO_M*GICO.uproject').FullName
$a = @("`"$p`"", '/Game/MundoMagico/Maps/L_Hospital', '-dx11', '-ModelContextProtocolStartServer')
Start-Process 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' -ArgumentList $a
