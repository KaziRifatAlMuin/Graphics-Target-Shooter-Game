CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -Isrc
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm
CORE = src/Arena.cpp src/Camera.cpp src/Game.cpp src/Weapon.cpp src/Interface.cpp src/Lighting.cpp src/Sound.cpp src/Collision.cpp src/CsvLogger.cpp src/Cargo.cpp src/Environment.cpp src/Target.cpp src/Projectile.cpp src/Challenge.cpp src/Movement.cpp src/LevelManager.cpp src/LevelWorld.cpp src/ScoreSystem.cpp src/Npc.cpp src/Bird.cpp src/Human.cpp src/levels/LevelBase.cpp src/levels/Level1.cpp src/levels/Level2.cpp src/levels/Level3.cpp src/levels/Level4.cpp src/levels/Level5.cpp src/levels/Level6.cpp src/levels/Level7.cpp
SRC = src/main.cpp src/Application.cpp $(CORE) src/Renderer.cpp src/glad.c
HEADERS = $(wildcard src/*.h src/levels/*.h)
TARGET = main.exe

.PHONY: all run test clean
all: $(TARGET)
	./$(TARGET) --export-calc

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

core_tests.exe: tests/core_tests.cpp tests/game_tests.h $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/core_tests.cpp $(CORE) -lwinmm -o $@

challenge_tests.exe: tests/challenge_tests.cpp $(CORE) $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/challenge_tests.cpp $(CORE) -lwinmm -o $@

test: all core_tests.exe challenge_tests.exe
	./core_tests.exe
	./challenge_tests.exe
	./$(TARGET) --challenge-smoke-test

clean:
	powershell -NoProfile -Command "Remove-Item -LiteralPath 'main.exe','core_tests.exe','challenge_tests.exe' -ErrorAction SilentlyContinue"
