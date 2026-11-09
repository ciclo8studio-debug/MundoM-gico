@echo off
rem Empaqueta el juego Shipping Win64 en Build<version>. Requiere Unreal cerrado.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Construir_Version.ps1"
pause
