CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm
CORE = src/world/Arena.cpp src/gameplay/GameMode.cpp src/persistence/CsvFile.cpp src/persistence/Leaderboard.cpp src/camera/BirdEyeCamera.cpp src/gameplay/SessionController.cpp src/ui/ModeUI.cpp src/ui/LeaderboardUI.cpp src/camera/Camera.cpp src/gameplay/Game.cpp src/gameplay/GameScene.cpp src/gameplay/Effects.cpp src/ui/Presentation.cpp src/gameplay/Weapon.cpp src/ui/Interface.cpp src/world/Lighting.cpp src/audio/Sound.cpp src/core/Collision.cpp src/persistence/CsvLogger.cpp src/persistence/SnapshotWriter.cpp src/world/Cargo.cpp src/world/Environment.cpp src/gameplay/Target.cpp src/gameplay/Projectile.cpp src/gameplay/Challenge.cpp src/gameplay/Movement.cpp src/gameplay/LevelManager.cpp src/world/LevelWorld.cpp src/gameplay/ScoreSystem.cpp src/gameplay/Npc.cpp src/gameplay/Bird.cpp src/gameplay/Human.cpp src/levels/LevelBase.cpp src/levels/Level1.cpp src/levels/Level2.cpp src/levels/Level3.cpp src/levels/Level4.cpp src/levels/Level5.cpp src/levels/Level6.cpp src/levels/Level7.cpp
SRC = src/main.cpp src/core/Application.cpp src/testing/ModeSmoke.cpp $(CORE) src/rendering/Renderer.cpp src/rendering/TextureCache.cpp src/third_party/glad.c
HEADERS = $(wildcard src/*/*.h)
TARGET = main.exe

.PHONY: all run test clean
all: $(TARGET)
	@echo Final version build complete.

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

core_tests.exe: tests/core_tests.cpp tests/game_tests.h $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/core_tests.cpp $(CORE) -lwinmm -o $@

challenge_tests.exe: tests/challenge_tests.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/challenge_tests.cpp $(CORE) -lwinmm -o $@

mode_tests.exe: tests/mode_tests.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/mode_tests.cpp $(CORE) -lwinmm -o $@

release_tests.exe: tests/release_tests.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/release_tests.cpp $(CORE) -lwinmm -o $@

test: all core_tests.exe challenge_tests.exe mode_tests.exe release_tests.exe
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path .release-work | Out-Null"
	./core_tests.exe
	./challenge_tests.exe .release-work/make-challenge-calc.csv
	./mode_tests.exe .release-work/make-mode-test.csv
	./release_tests.exe .release-work/make-release-test.csv
	./$(TARGET) --modes-smoke-test --calc .release-work/make-smoke-calc.csv --leaderboard .release-work/make-smoke-leaderboard.csv

clean:
	powershell -NoProfile -Command "Remove-Item -LiteralPath 'main.exe','core_tests.exe','challenge_tests.exe','mode_tests.exe','release_tests.exe' -ErrorAction SilentlyContinue"
