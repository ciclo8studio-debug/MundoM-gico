@echo off
rem Sube a GitHub (rama unreal, Git LFS) los cambios del proyecto Unreal. Guarda todo en Unreal antes.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Versionar_Unreal.ps1" %*
pause
