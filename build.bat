@echo off
setlocal
cd /d "%~dp0"
echo Building 3D Target Shooter - Phase 3...
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/Application.cpp src/ModeSmoke.cpp src/Arena.cpp src/GameMode.cpp src/CsvFile.cpp src/Leaderboard.cpp src/BirdEyeCamera.cpp src/SessionController.cpp src/ModeUI.cpp src/LeaderboardUI.cpp src/Camera.cpp src/Game.cpp src/Weapon.cpp src/Interface.cpp src/Lighting.cpp src/Sound.cpp src/Collision.cpp src/CsvLogger.cpp src/Cargo.cpp src/Environment.cpp src/Target.cpp src/Projectile.cpp src/Challenge.cpp src/Movement.cpp src/LevelManager.cpp src/LevelWorld.cpp src/ScoreSystem.cpp src/Npc.cpp src/Bird.cpp src/Human.cpp src/levels/LevelBase.cpp src/levels/Level1.cpp src/levels/Level2.cpp src/levels/Level3.cpp src/levels/Level4.cpp src/levels/Level5.cpp src/levels/Level6.cpp src/levels/Level7.cpp src/Renderer.cpp src/glad.c -Iinclude -Isrc -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm -o main.exe
if errorlevel 1 exit /b 1
main.exe --export-calc
if errorlevel 1 exit /b 1
echo Build complete.
