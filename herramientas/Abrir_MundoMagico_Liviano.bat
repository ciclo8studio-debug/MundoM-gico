@echo off
rem Abre Mundo Magico en modo liviano (sin trazado de rayos, MegaLights, sombras virtuales ni Lumen).
rem No cambia la configuracion guardada del proyecto: al cerrar Unreal todo vuelve a como estaba.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Abrir_MundoMagico_Liviano.ps1"
