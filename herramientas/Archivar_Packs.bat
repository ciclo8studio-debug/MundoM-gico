@echo off
rem Mueve fuera de Content los packs de Fab sin uso (reversible). Requiere Unreal cerrado.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Archivar_Packs.ps1"
pause
