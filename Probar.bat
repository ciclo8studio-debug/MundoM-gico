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
echo   PARQUE (solo pruebas internas)
echo   1. Entrada   2. Teatro   3. Noria   4. Bodega   5. Autos chocadores
echo   9. Entrada con PERFIL A en vivo     0. Volver
set /p PQ="Elige: "
set "ZONA="
if "%PQ%"=="1" set "ZONA=parque"
if "%PQ%"=="2" set "ZONA=teatro"
if "%PQ%"=="3" set "ZONA=noria"
if "%PQ%"=="4" set "ZONA=bodega"
if "%PQ%"=="5" set "ZONA=autos"
if "%PQ%"=="9" (
  start "" "%EXE%" L_Parque -ir=parque -dev -dpcvars=r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Shadow.Virtual.Enable=0,r.MegaLights.Enable=0,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0
  goto menu
)
if not defined ZONA goto menu
start "" "%EXE%" L_Parque -ir=%ZONA% -dev
goto menu
