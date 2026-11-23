@echo off
chcp 65001 >nul
title Blackline - Probar (perfil liviano)
rem Abre la ultima build de Blackline con el perfil grafico liviano del PC de 8 GB.
rem Cierra Unreal y cualquier otro juego antes de usarlo.
set "RAIZ=%~dp0"
set "EXE="
for /f "delims=" %%d in ('dir /b /ad /o-n "%RAIZ%Build\v*" 2^>nul') do (
  if not defined EXE if exist "%RAIZ%Build\%%d\Windows\Blackline.exe" set "EXE=%RAIZ%Build\%%d\Windows\Blackline.exe" & set "VER=%%d"
)
if not defined EXE ( echo No hay builds en Build\. Ejecuta scripts\Build.ps1 & pause & exit /b 1 )
echo Abriendo %VER% ...
start "" "%EXE%" -dx11 -windowed -ResX=1280 -ResY=720 -dpcvars=r.RayTracing.Enable=0,r.Shadow.Virtual.Enable=0,r.DynamicGlobalIlluminationMethod=0,r.ReflectionMethod=2,r.Lumen.DiffuseIndirect.Allow=0,r.Lumen.Reflections.Allow=0,r.VolumetricFog.GridPixelSize=16,r.VolumetricFog.GridSizeZ=64
