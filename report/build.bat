@echo off
setlocal
rem Launch the PowerShell report builder from this script's folder and forward user arguments.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0build.ps1" %*
exit /b %errorlevel%
