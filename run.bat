@echo off
setlocal
cd /d "%~dp0"
rem Build the game and required assets before launching it.
call build.bat
if errorlevel 1 exit /b 1
rem Forward all command-line arguments to the game executable.
main.exe %*
exit /b %errorlevel%
