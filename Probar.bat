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
if not defined ZONA goto menu
echo Abriendo %VER% en: %ZONA% ...
start "" "%EXE%" -ir=%ZONA% -dev
goto menu
