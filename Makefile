CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32
SRC = src/main.cpp src/Arena.cpp src/Camera.cpp src/Renderer.cpp src/glad.c
HEADERS = $(wildcard src/*.h)
TARGET = main.exe

.PHONY: all run test clean
all: $(TARGET)
	./$(TARGET) --export-calc

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

run: all
	./$(TARGET)

core_tests.exe: tests/core_tests.cpp src/Arena.cpp src/Camera.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -Isrc tests/core_tests.cpp src/Arena.cpp src/Camera.cpp -o $@

test: all core_tests.exe
	./core_tests.exe
	./$(TARGET) --smoke-test

clean:
	powershell -NoProfile -Command "Remove-Item -LiteralPath 'main.exe','core_tests.exe' -ErrorAction SilentlyContinue"
