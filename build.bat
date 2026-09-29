@echo off
setlocal
cd /d "%~dp0"
echo Building 3D Target Shooter - Phases 3 and 4...
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/Arena.cpp src/Camera.cpp src/Game.cpp src/Weapon.cpp src/Interface.cpp src/Renderer.cpp src/glad.c -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 -o main.exe
if errorlevel 1 exit /b 1
main.exe --export-calc
if errorlevel 1 exit /b 1
echo Build complete.
