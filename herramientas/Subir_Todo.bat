@echo off
rem Sube a GitHub los commits pendientes de scripts y del proyecto Unreal, en un solo paso.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Subir_Todo.ps1"
pause
