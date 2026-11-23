@echo off
chcp 65001 >nul
title Blackline - Probar (modo liviano 8 GB)
rem Abre la build Development mas nueva en ventana 1280x720 con el contador de FPS.
rem Los ajustes de render van SOLO como argumentos de esta prueba; no tocan el proyecto.
rem Cierra Unreal antes de usarlo (el PC tiene 8 GB de RAM).

set "RAIZ=%~dp0"
set "EXE="
for /f "delims=" %%d in ('dir /b /ad /o-d "%RAIZ%Build\v*-Development" 2^>nul') do (
  if not defined EXE if exist "%RAIZ%Build\%%d\Windows\Blackline.exe" set "EXE=%RAIZ%Build\%%d\Windows\Blackline.exe" & set "VER=%%d"
)
if not defined EXE (
  echo No encontre ninguna build en Build\. Ejecuta scripts\Build.ps1 -Version X.Y.Z
  pause & exit /b 1
)

:menu
cls
echo ==============================================
echo   BLACKLINE - PROBAR   (%VER%)
echo ==============================================
echo   1. Liviano   (720p, sin Lumen, sombras medias)
echo   2. Muy liviano (720p, calidad baja, 60%% resolucion)
echo   3. Como en el proyecto (para comparar)
echo   4. Abrir carpeta de logs
echo   0. Salir
echo ----------------------------------------------
set /p OP="Elige una opcion: "
set "BASE=-dx11 -windowed -ResX=1280 -ResY=720 -log -nosplash"
set "LIVIANO=-dpcvars=r.RayTracing.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Shadow.Virtual.Enable=0"
if "%OP%"=="0" exit /b 0
if "%OP%"=="1" start "" "%EXE%" %BASE% %LIVIANO% -ExecCmds="stat fps,sg.ShadowQuality 1,sg.PostProcessQuality 1,sg.EffectsQuality 1,r.ScreenPercentage 75" & goto menu
if "%OP%"=="2" start "" "%EXE%" %BASE% %LIVIANO% -ExecCmds="stat fps,sg.ViewDistanceQuality 0,sg.ShadowQuality 0,sg.PostProcessQuality 0,sg.EffectsQuality 0,sg.TextureQuality 0,r.ScreenPercentage 60" & goto menu
if "%OP%"=="3" start "" "%EXE%" %BASE% -ExecCmds="stat fps" & goto menu
if "%OP%"=="4" start "" explorer "%RAIZ%Build\%VER%\Windows\Blackline\Saved\Logs" & goto menu
goto menu
