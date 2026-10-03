@echo off
setlocal
cd /d "%~dp0"
echo Building final version of 3D Target Shooter...
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp src/core/Application.cpp src/testing/ModeSmoke.cpp src/world/Arena.cpp src/gameplay/GameMode.cpp src/persistence/CsvFile.cpp src/persistence/Leaderboard.cpp src/camera/BirdEyeCamera.cpp src/gameplay/SessionController.cpp src/ui/ModeUI.cpp src/ui/LeaderboardUI.cpp src/camera/Camera.cpp src/gameplay/Game.cpp src/gameplay/GameScene.cpp src/gameplay/Effects.cpp src/ui/Presentation.cpp src/gameplay/Weapon.cpp src/ui/Interface.cpp src/world/Lighting.cpp src/audio/Sound.cpp src/core/Collision.cpp src/persistence/CsvLogger.cpp src/persistence/SnapshotWriter.cpp src/world/Cargo.cpp src/world/Environment.cpp src/gameplay/Target.cpp src/gameplay/Projectile.cpp src/gameplay/Challenge.cpp src/gameplay/Movement.cpp src/gameplay/LevelManager.cpp src/world/LevelWorld.cpp src/gameplay/ScoreSystem.cpp src/gameplay/Npc.cpp src/gameplay/Bird.cpp src/gameplay/Human.cpp src/levels/LevelBase.cpp src/levels/Level1.cpp src/levels/Level2.cpp src/levels/Level3.cpp src/levels/Level4.cpp src/levels/Level5.cpp src/levels/Level6.cpp src/levels/Level7.cpp src/rendering/Renderer.cpp src/third_party/glad.c -Iinclude -Isrc -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm -o main.exe
if errorlevel 1 exit /b 1
echo Final version build complete.
