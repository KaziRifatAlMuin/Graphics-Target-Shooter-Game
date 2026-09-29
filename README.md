# 3D Target Shooter - Phase 1

A runnable C++17 / OpenGL 3.3 basic scene for Kazi Rifat Al Muin (2107042),
following [project.md](project.md). One world unit is one meter; +Y is up
and -Z points into the arena.

Implemented: an OpenGL window, GLSL shaders, a shared unit-cube mesh,
a free camera, a 60 x 100 m floor, 8 m boundary walls, battlements,
corner towers, and a 10 m entrance. The initial elevated view shows the
arena. Fixed face colors make the cubes readable; gameplay, targets,
weapons, full lighting, day/night, HUD, and camera presets belong to later phases.

## Build and run (Windows)

Use a **64-bit MinGW/GCC** compiler on PATH. GLFW and GLAD are already
included. No additional packages are needed. The GPU must support OpenGL 3.3.

From PowerShell in the project directory:

```powershell
.\run.bat
```

This builds, regenerates `calc.csv`, and launches the scene. To build only:

```powershell
.\build.bat
.\main.exe
```

VS Code: **Ctrl+Shift+B** invokes the same build script. Alternatively:

```powershell
mingw32-make
mingw32-make run
```

With CMake installed:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\main.exe
ctest --test-dir build --output-on-failure
```

The default CMake build also regenerates the calculation file. The bundled
GLFW binary is for MinGW, not MSVC. On Linux, CMake uses installed GLFW 3.3+
and OpenGL development packages; the Windows batch/Make scripts use bundled libraries.

## Controls

| Input | Action |
| --- | --- |
| W / S | Fly forward / backward |
| A / D | Fly left / right |
| Q / E | Move down / up |
| Hold right mouse + move | Look around |
| Shift | Fly faster |
| Home | Reset the camera to the starting view |
| Esc | Exit |

Movement is frame-time based. Diagonal motion is normalized. The camera can
fly above/outside the walls to inspect the arena; it stays above ground.
Resizing updates the viewport and perspective aspect ratio. Release right
mouse to use the pointer normally.

## Generated calculations

`calc-init.csv` remains the original reference. **Every build through the
provided scripts/default CMake target, and every application launch,
overwrites `calc.csv` with values calculated from the code.** It is a static
scene record, not a per-frame log.

The exporter preserves all 16 reference columns. It writes one representative
local corner `(0.5,0.5,0.5)` for **every currently rendered cube instance**,
including its actual model matrix and resulting world point. Later phases
will add rows as their objects enter the scene. The initial examples for
future weapons/targets are not copied into the Phase 1 output.

Drawing and export both call `composeModelMatrix` with:

```text
M = T * Rz * Ry * Rx * H * S
worldPoint = M * localPoint
```

Angles are degrees; matrices use column-major storage and column vectors.
The CSV prints matrix rows for readability. Identity shear/rotation still
pass through the full pipeline. This small shared math foundation is included
now so the first runnable phase already produces accurate calculations.

Regenerate without opening a graphics window:

```powershell
.\main.exe --export-calc
.\main.exe --export-calc --calc alternate.csv
```

The default file is in the project root even when launched from a build
directory. An explicit `--calc` path is relative to the current directory.
A write failure is reported and returns a nonzero exit code.

## Validation

```powershell
mingw32-make test
```

This checks known mathematical results for transform order, all six shear
coefficients, view/projection matrices, camera motion, arena bounds, and
unique object IDs. It then creates a hidden OpenGL window, compiles/links the
real shaders, draws three frames, reads pixels back to verify visible floor
and walls, checks for GL errors, and exits.

The graphics check can also be run directly:

```powershell
.\main.exe --smoke-test
.\main.exe --smoke-test --capture phase1.ppm
```

The optional PPM image is the actual rendered frame. Run the visible app to
inspect movement, mouse look, and resizing interactively.

## Source layout

- `src/main.cpp`: startup, resource paths, input loop, window, and smoke check.
- `src/Transform.h`: vectors, matrices, model/view/projection calculations.
- `src/Camera.*`: free camera movement and view.
- `src/Arena.*`: cube instances and CSV generation.
- `src/Renderer.*`: shader loading, shared cube mesh, scene drawing.
- `shaders/object.vert`, `shaders/object.frag`: Phase 1 GLSL.
- `tests/core_tests.cpp`: deterministic math and scene checks.

Edit geometry in `createArena()`, then rebuild: rendering and calculation
output use the same data. Each later phase can extend this runnable baseline.
