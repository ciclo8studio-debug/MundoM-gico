@echo off
chcp 65001 >nul
title Mundo Magico - Probar (Ciclo Studio Game)
rem Abre la ultima build saltando a una zona (-ir=) con el panel de prueba (-dev: fps, posicion, bateria, ruido, tension).
rem Cierra Unreal antes de usarlo (el PC tiene 8 GB de RAM).

set "RAIZ=%~dp0"
set "SAVED=%LOCALAPPDATA%\MundoMagico\Saved"
set "EXE="
for /f "delims=" %%d in ('dir /b /ad /o-d "%RAIZ%Build\v*" 2^>nul') do (
  if not defined EXE if exist "%RAIZ%Build\%%d\Windows\MundoMagico.exe" set "EXE=%RAIZ%Build\%%d\Windows\MundoMagico.exe" & set "VER=%%d"
)
if not defined EXE (
  echo No encontre ninguna build en Build\. Primero ejecuta herramientas\Construir_Version.bat
  pause & exit /b 1
)

:menu
cls
echo ==============================================
echo   MUNDO MAGICO - PROBAR   (%VER%)
echo ==============================================
echo   HOSPITAL (demo)
echo   1. Sala del despertar
echo   2. Pasillo de Urgencias (Sombra y escondites)
echo   3. Juego normal desde el menu
echo   4. Juego normal SIN SONIDO
echo ----------------------------------------------
echo   HERRAMIENTAS
echo   5. Borrar partida (la guarda como .bak, no la elimina)
echo   6. Abrir carpeta de logs y cierres
echo   7. Recoger logs y cierres en un ZIP (para reportar)
echo ----------------------------------------------
echo   PARQUE (fuera de la demo publica)
echo   P. Zonas del parque
echo   0. Salir
echo ----------------------------------------------
set /p OP="Elige una opcion: "
if "%OP%"=="0" exit /b 0
if "%OP%"=="1" start "" "%EXE%" L_Hospital -ir=hospital -dev & goto menu
if "%OP%"=="2" start "" "%EXE%" L_Hospital -ir=pasillo -dev & goto menu
if "%OP%"=="3" start "" "%EXE%" -dev & goto menu
if "%OP%"=="4" start "" "%EXE%" -dev -nosound & goto menu
if "%OP%"=="5" goto borrar
if "%OP%"=="6" (
  if not exist "%SAVED%" mkdir "%SAVED%"
  start "" explorer "%SAVED%"
  goto menu
)
if "%OP%"=="7" (
  powershell -NoProfile -ExecutionPolicy Bypass -File "%RAIZ%herramientas\Recoger_Logs.ps1"
  pause
  goto menu
)
if /i "%OP%"=="P" goto parque
goto menu

:borrar
set "SG=%SAVED%\SaveGames"
if not exist "%SG%\*.sav" (
  echo No hay partida guardada.
  pause & goto menu
)
for /f %%t in ('powershell -NoProfile -Command "Get-Date -Format yyyyMMdd_HHmmss"') do set "TS=%%t"
mkdir "%SG%\respaldo_%TS%" 2>nul
move /y "%SG%\*.sav" "%SG%\respaldo_%TS%\" >nul
echo Partida movida a SaveGames\respaldo_%TS%  (para recuperarla, devuelve los .sav a SaveGames).
pause
goto menu

:parque
cls
echo   PARQUE (pruebas internas)
echo   1. Entrada   2. Teatro   3. Noria   4. Bodega   5. Autos chocadores
echo   --- DIAGNOSTICO DEL CIERRE (anota si se cae y los fps, 3 min cada una) ---
echo   D1. Parque directo (sin pasar por el hospital)
echo   D2. Parque directo + Perfil A en vivo
echo   D3. D2 + resolucion 50%%, sin niebla volumetrica, distancia de vista 0.5
echo   D4. D2 + pool de texturas 256 MB
echo   D5. Ruta normal SIN salto: juego normal con modo liviano (camina hasta el glitch)
echo   D6. D2 + sombras 256, sin anisotropico, luces a 30%% de distancia
echo   D7. Parque directo con el MODO LIVIANO del editor (RayTracing apagado)
echo   D8. D7 + sin particulas GPU, sin nubes volumetricas, sin AO de campos de distancia
echo   D9. D7 + SIN SOMBRAS dinamicas + cielo sin captura en tiempo real
echo   D10. D7 + SIN SOMBRAS dinamicas (solo eso)
echo   0. Volver
set /p PQ="Elige: "
set "ZONA="
set "PA=r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Shadow.Virtual.Enable=0,r.MegaLights.Enable=0,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0"
if "%PQ%"=="1" set "ZONA=parque"
if "%PQ%"=="2" set "ZONA=teatro"
if "%PQ%"=="3" set "ZONA=noria"
if "%PQ%"=="4" set "ZONA=bodega"
if "%PQ%"=="5" set "ZONA=autos"
if /i "%PQ%"=="D1" start "" "%EXE%" L_Parque -ir=parque -dev & goto menu
if /i "%PQ%"=="D2" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=%PA% & goto menu
if /i "%PQ%"=="D3" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=%PA%,r.ScreenPercentage=50,r.VolumetricFog=0,r.ViewDistanceScale=0.5 & goto menu
if /i "%PQ%"=="D4" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=%PA%,r.Streaming.PoolSize=256,r.Streaming.LimitPoolSizeToVRAM=1 & goto menu
if /i "%PQ%"=="D5" start "" "%EXE%" -dev -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64 & goto menu
if /i "%PQ%"=="D6" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=%PA%,r.MaxAnisotropy=0,r.Shadow.MaxResolution=256,r.LightMaxDrawDistanceScale=0.3 & goto menu
if /i "%PQ%"=="D7" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64 & goto menu
if /i "%PQ%"=="D8" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64,FX.AllowGPUParticles=0,r.VolumetricCloud=0,r.DistanceFieldAO=0,r.AOGlobalDistanceField=0 & goto menu
if /i "%PQ%"=="D9" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64,r.ShadowQuality=0,r.SkyLight.RealTimeReflectionCapture=0,r.VolumetricCloud=0 & goto menu
if /i "%PQ%"=="D10" start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64,r.ShadowQuality=0 & goto menu
if not defined ZONA goto menu
start "" "%EXE%" L_Parque -ir=%ZONA% -dev
goto menu
