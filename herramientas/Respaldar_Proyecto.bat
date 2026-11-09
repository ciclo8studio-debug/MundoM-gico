@echo off
rem Crea un zip con fecha de lo propio del proyecto (MundoMagico, ThirdPerson, Fuentes, Input, Config, SaveGames).
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0Respaldar_Proyecto.ps1"
pause
