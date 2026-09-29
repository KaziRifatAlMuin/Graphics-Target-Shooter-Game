CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32 -lwinmm
CORE = src/Arena.cpp src/Camera.cpp src/Game.cpp src/Weapon.cpp src/Interface.cpp src/Lighting.cpp src/Sound.cpp
SRC = src/main.cpp $(CORE) src/Renderer.cpp src/glad.c
HEADERS = $(wildcard src/*.h)
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

test: all core_tests.exe
	./core_tests.exe
	./$(TARGET) --smoke-test

clean:
	powershell -NoProfile -Command "Remove-Item -LiteralPath 'main.exe','core_tests.exe' -ErrorAction SilentlyContinue"
