@echo off
chcp 65001 >nul
title Mundo Magico - Probar una zona
rem Abre la ultima build saltando directo a una zona (-ir=) y con el panel de prueba (-dev).
rem Cierra Unreal antes de usarlo (el PC tiene 8 GB de RAM).

set "RAIZ=%~dp0"
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
echo   MUNDO MAGICO - PROBAR UNA ZONA   (%VER%)
echo ==============================================
echo   1. Hospital  (sala del despertar)
echo   2. Pasillo de Urgencias (antes del susto)
echo   3. Entrada del parque
echo   4. Teatro
echo   5. Noria
echo   6. Bodega
echo   7. Autos chocadores
echo   8. Juego normal (sin saltar)
echo   9. Entrada del parque con PERFIL A en vivo (comparar con la 3)
echo   0. Salir
echo ----------------------------------------------
set /p OP="Elige una opcion: "
set "ZONA="
if "%OP%"=="1" set "ZONA=hospital"
if "%OP%"=="2" set "ZONA=pasillo"
if "%OP%"=="3" set "ZONA=parque"
if "%OP%"=="4" set "ZONA=teatro"
if "%OP%"=="5" set "ZONA=noria"
if "%OP%"=="6" set "ZONA=bodega"
if "%OP%"=="7" set "ZONA=autos"
if "%OP%"=="0" exit /b 0
if "%OP%"=="8" (
  start "" "%EXE%" -dev
  goto menu
)
if "%OP%"=="9" (
  rem Perfil A en vivo: apaga GI dinamica, reflejos Lumen, sombras virtuales y MegaLights solo en esta sesion.
  rem No cambia el proyecto ni recompila shaders. Compara los fps del panel con la opcion 3.
  start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Shadow.Virtual.Enable=0,r.MegaLights.Enable=0,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0
  goto menu
)
if not defined ZONA goto menu
echo Abriendo %VER% en: %ZONA% ...
set "MAPA=L_Parque"
if "%ZONA%"=="hospital" set "MAPA=L_Hospital"
if "%ZONA%"=="pasillo" set "MAPA=L_Hospital"
start "" "%EXE%" %MAPA% -ir=%ZONA% -dev
goto menu
