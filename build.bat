@echo off
setlocal
cd /d "%~dp0"
echo Building 3D Target Shooter - Complete Game...
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/Arena.cpp src/Camera.cpp src/Game.cpp src/Weapon.cpp src/Interface.cpp src/Lighting.cpp src/Sound.cpp src/Renderer.cpp src/glad.c -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm -o main.exe
if errorlevel 1 exit /b 1
main.exe --export-calc
if errorlevel 1 exit /b 1
echo Build complete.
