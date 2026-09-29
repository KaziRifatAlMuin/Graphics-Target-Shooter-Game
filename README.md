# 3D Target Shooter - Phases 3 and 4

A runnable C++17 / OpenGL 3.3 graphics project for Kazi Rifat Al Muin (2107042).
One world unit is one meter; +Y is up and -Z points into the arena.

The arena remains enclosed by solid walls on all four sides. Phases 1 and 2
provide the shared cube mesh, shaders, camera math, fortified walls, and
scale/shear/rotation/translation pipeline.

Phases 3 and 4 add:

- Six targets that move horizontally, move vertically, or rotate. Hits flash
  yellow, reduce health, and briefly remove a cleared target before it respawns.
- Reproducible cargo stacks from seed 2107042. Side placement leaves the center
  lanes open. Cargo blocks the walking player and projectiles.
- A walking shooter, an attached player camera, arena and side views, and an
  independent free camera. The shooter is visible in the observation views.
- Three distinct cube-based weapons, weapon-specific crosshairs, visible moving
  projectiles, hit markers, range readout, and hit/cleared counters.
- A startup menu with instructions, Start Session, View Controls, and Exit.
  The pause menu offers Resume Session, View Controls, Main Menu, and Exit.

The scene uses flat face colors. Full lighting and day/night remain for
Phases 5 and 6.

## Build and run

Use **64-bit MinGW/GCC** on PATH and a GPU supporting OpenGL 3.3.
GLFW and GLAD are bundled; no new packages or assets are needed.

From PowerShell in the project directory:

```powershell
.\run.bat
```

Or build only and launch the executable:

```powershell
.\build.bat
.\main.exe
```

**Ctrl+Shift+B** in VS Code builds the project. Make is also supported:

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

CMake uses the bundled MinGW GLFW binary on Windows; MSVC is not supported by
that binary. On Linux it uses installed GLFW/OpenGL development packages.
CMake is not installed in the development environment; the MinGW batch and
Make builds are the locally verified paths.

## Starting and controls

Read the on-screen instructions or open **View Controls**, then click
**Start Session** (or press Enter). The mouse is captured for aiming.
Esc pauses gameplay and releases it. Losing window focus also pauses.

| Input | Action |
| --- | --- |
| W / A / S / D | Walk the shooter; fly the camera in F4 mode |
| Mouse | Aim in player view; look around in free-camera view |
| Left click / Space | Fire in player view |
| 1 / 2 / 3 | Pistol / shotgun / assault rifle |
| Shift | Walk or fly faster |
| F1 | Player camera, attached to the shooter |
| F2 | Elevated arena view |
| F3 | Side view |
| F4 | Independent free camera |
| Q / E | Free-camera down / up |
| R | Reset targets and remove current projectiles |
| Tab | Release/capture the mouse to click HUD buttons |
| Esc | Pause/resume; return from controls |
| F5 | Save a current calculation snapshot |

HUD buttons provide weapon selection, Menu, and Exit. In player/free-camera
view, press Tab first to use the pointer. Observation views keep it free.
F2/F3 still allow WASD to move the shooter; F4 leaves the shooter stationary
while the camera moves. Switch to F1 to aim and fire.

Menus freeze targets, projectiles, and player movement. Resume preserves the
session; Start Session from the main menu starts a new one. Target reset
preserves the current score. Exit closes the application.

## Weapons and targets

| Weapon | Game range | Behavior | Crosshair |
| --- | --- | --- | --- |
| Pistol | 25 m | One projectile per click, 0.28 s cooldown | Dot with four marks |
| Shotgun | 18 m | Nine pellets with angular spread, 0.8 s cooldown | Wide circle with ticks |
| Assault rifle | 70 m | Hold fire for repeated shots, 0.11 s cooldown | Compact cross |

Ammunition is unlimited for this graphics demonstration. Projectiles originate
at the modeled muzzle and converge toward the center aim ray. The HUD shows
the distance to the center of the visible target under that ray, in meters;
the readout turns amber when the target is beyond the selected weapon's range.
The crosshair changes color over a visible target. A yellow marker confirms
a hit. Hit counts include individual shotgun pellet impacts.

Swept segment/cube tests prevent fast projectiles from passing through thin
targets between frames. The nearest collision wins, so walls and cargo shield
targets behind them. Target plates use a simple rotated cube collision volume.
Small fixed simulation steps keep motion stable. Walking is bounded by the
enclosed arena and conservative crate/support footprints; the free camera can
fly outside the arena for inspection.

## Automatic calc.csv

`calc-init.csv` is preserved as the reference. The 16-column schema is unchanged.
The program calculates each row from the same cube transforms used by rendering:

```text
M = T * Rz * Ry * Rx * H * S
worldPoint = M * localPoint
```

`Matrix_or_Operation` contains the point after S, H, Rx, Ry, Rz, and T, then
the complete model matrix. Angles are degrees. Matrices have column-major
storage; CSV matrices are printed as rows for readability.

The provided builds and every launch regenerate `calc.csv`. During play it
refreshes once per simulation second, on weapon changes/target reset, on F5,
and at exit. Each row includes simulation time. Menus pause the simulation.
The CSV is a current snapshot rather than an ever-growing frame log.

Snapshots include the arena, cargo, visible targets and stands, shooter,
current weapon, and active projectiles. Inactive weapon models and one
representative projectile for each weapon are also exported with explicit
notes, so all implemented weapon categories remain documented even before
firing. A destroyed target's plate returns to the CSV when it respawns.

Export a deterministic starting snapshot without a graphics window:

```powershell
.\main.exe --export-calc
.\main.exe --export-calc --calc alternate.csv
```

The default output is the project root even when launched from a build
directory. An explicit output path is relative to the current directory.
Write failures return a nonzero exit code.

## Validation

```powershell
mingw32-make test
```

The tests cover transformation stages, closed wall coverage, target movement,
seeded cargo, independent camera/player motion, walking collision, rotated
cube ray tests, projectile impacts and cover, nearest-hit selection, cooldowns,
shotgun spread, projectile expiry, target respawn, CSV categories, and menu
button hit areas.

The hidden-window GPU smoke test exercises menu, controls, start, pause/resume,
all cameras and all weapons. It compiles the actual shaders and checks rendered
scene and UI pixels for OpenGL errors.

To save actual rendered previews:

```powershell
New-Item -ItemType Directory -Force build
.\main.exe --smoke-test --capture build/phase4.ppm
```

This saves player, menu, controls, and arena previews as PPM images. Inspect the
visible app for interactive mouse feel and window resizing. Regenerate the
starting snapshot after a smoke test if desired using `--export-calc`.

## Source layout

- `src/Transform.h`: shared transformation stages and view/projection math.
- `src/Arena.*`: enclosed arena geometry and calculation export.
- `src/Camera.*`: view, look, and free movement.
- `src/Game.*`: target animation, seeded cargo, player movement, collision,
  projectiles, scoring, and snapshot assembly.
- `src/Weapon.cpp`: weapon settings, cube models, and muzzle placement.
- `src/Interface.*`: menu/HUD layouts, buttons, crosshairs, and built-in font.
- `src/Renderer.*`: shared cube mesh and batched UI rendering.
- `src/main.cpp`: application states, input, timing, snapshot updates, and smoke test.
- `shaders/`: scene and UI GLSL.
- `tests/`: deterministic math and gameplay checks.
