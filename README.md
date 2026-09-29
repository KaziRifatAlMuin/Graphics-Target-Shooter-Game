# 3D Target Shooter - Complete Game

C++17 / OpenGL 3.3 graphics project for Kazi Rifat Al Muin (2107042).
All six implementation phases are complete. The 60 x 100 m arena has solid
walls on every side, fortified corners, cargo, moving targets, lights,
three weapons, a movable shooter, four camera modes, menus, HUD, and sound.

## Build and run

Use **64-bit MinGW/GCC** on PATH and a GPU supporting OpenGL 3.3.
GLFW/GLAD are bundled. Windows audio uses the system WinMM library.
No additional downloads, textures, fonts, or sound files are required.

```powershell
.\run.bat
```

For a build without launching the window:

```powershell
.\build.bat
.\main.exe
```

**Ctrl+Shift+B** in VS Code invokes the same build. Make also works:

```powershell
mingw32-make
mingw32-make run
mingw32-make test
```

Optional CMake build:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\main.exe
ctest --test-dir build --output-on-failure
```

The bundled GLFW binary requires MinGW, not MSVC. CMake is not installed in
this development environment; the batch and Make paths are tested locally.
On other platforms, CMake uses installed GLFW/OpenGL development packages;
the current sound backend is Windows-only and the game reports audio unavailable
elsewhere instead of failing.

## Starting and menus

The opening menu shows the instructions and scoring rules before play.
Click **Start Session**, **View Controls**, or **Exit**. Enter also starts.
Esc opens the pause menu with Resume Session, View Controls, Main Menu, and Exit.
Menus freeze gameplay. Losing window focus also pauses the session.

Day/night and sound buttons work from menus and the HUD. Press Tab during
player/free-camera play to release the pointer for those buttons.
A click used to resume or operate a menu is not also used to fire.

## Controls

| Input | Action |
| --- | --- |
| W / A / S / D | Walk the shooter; fly the camera in F4 mode |
| Mouse | Aim in player view; look in free-camera view |
| Left click / Space | Fire in player view |
| 1 / 2 / 3 | Pistol / shotgun / assault rifle |
| Shift | Move faster |
| F1 | Player camera attached to the shooter |
| F2 | Elevated arena view |
| F3 | Side view |
| F4 | Independent free camera |
| Q / E | Free-camera down / up |
| N | Day/night toggle |
| M | Sound on/off |
| R | Reset targets, fragments, and active projectiles |
| Tab | Release/capture the pointer |
| Esc | Pause/resume; return from controls |
| F5 | Save the current calculation snapshot |

F2/F3 let you observe the walking shooter; F4 moves the camera independently.
Return to F1 to aim and fire. Walking cannot cross the enclosed boundary or
cargo/support/lamp footprints. The free camera can leave the arena for inspection.
Start Session starts fresh scores while preserving current sound/day settings.
R resets targets but preserves score. Ammunition is unlimited for target practice.

## Six-ring targets

Targets are **1.6 m diameter** round disks with six scoring regions.
Only the printed front (+local Z) scores. The back is plain metal.
Back/edge hits stop projectiles without damage, hit markers, or score.
Shots through the empty corners around the circle miss.

| Region, center outward | Color | Damage out of 60 | Same-region shots to break |
| --- | --- | --- | --- |
| 1 - bullseye | Red | 60 | 1 |
| 2 | Yellow | 30 | 2 |
| 3 | Blue | 20 | 3 |
| 4 | White | 15 | 4 |
| 5 | Red | 12 | 5 |
| 6 - outer | White | 10 | 6 |

Mixed hits add their damage. Each distinct trigger counts once per target:
shotgun pellets share a shot ID, so additional pellets can improve that shot's
region damage but cannot add nine separate outer-ring hits.
This rule applies equally to all three weapons.

Horizontal targets move up to 8.4 m/s; vertical targets move up to 2.24 m/s.
Rotating targets turn at 140 degrees/s, exposing their unprinted backs.
A hit flashes the face; a break produces cube fragments, adds a 100-point
bonus, and respawns the target after 1.8 seconds. Ring damage also contributes
to score. The HUD shows hits, cleared targets, bullseyes, current score, aimed
target health, and range in meters.

Swept projectile/cylinder tests use the target's current orientation and the
same ring radii as the shader. Simulation runs in steps no larger than 1/120 s.
The nearest collision wins, so cover and nearer targets shield objects behind them.

## Weapons

| Weapon | Game range | Behavior | Crosshair |
| --- | --- | --- | --- |
| Pistol | 25 m | One projectile per click; 0.28 s cooldown | Dot and four marks |
| Shotgun | 18 m | Nine spreading pellets; 0.8 s cooldown | Circle and spread ticks |
| Assault rifle | 70 m | Hold to fire; 0.11 s cooldown | Compact cross |

Weapons are assembled from transformed cubes, follow the player's aim, and
show recoil/muzzle flash. Visible projectiles originate at their modeled muzzles
and converge toward the center aim ray. They stop at cover or their maximum
travel range. The range readout measures distance from the player to the aimed
target center; observation views label this as PLAYER AIM. Out-of-range aiming
is highlighted. Each projectile has its own movement state.

## Lighting, sky, and audio

The GLSL scene shader computes ambient + diffuse + Blinn-Phong specular light.
Normals use the inverse-transpose model matrix, including for nonuniform scales
and shears. Weapons and lamp metal have stronger specular highlights; walls,
ground, and crates have a rougher response.

- **Day:** blue gradient sky, visible emissive cube sun, directional sunlight,
  higher ambient illumination, and lamps off.
- **Night:** dark gradient sky, emissive cube moon, dim directional fill,
  lower ambient light, eight warm point lamps, and six cool spotlights.
- Point/spot lights attenuate with distance. Spotlights use inner/outer cone
  angles of 22/34 degrees for soft edges. Fixture geometry and shader light
  positions come from the same light definitions.

Sound effects are synthesized PCM: three distinct firing sounds, hit, break,
and UI click. The Windows backend mixes overlapping voices into asynchronous
output buffers, so firing does not block rendering. Muting immediately clears
queued sound and active voices; unmuting does not replay old events. Settings
persist across sessions in the current application run.
If no audio device is available, the HUD says so and gameplay continues.

## Automatically generated calc.csv

`calc-init.csv` remains unchanged as the reference. All **16 columns** are retained.

```text
M = T * Rz * Ry * Rx * H * S
worldPoint = M * localPoint
```

Every launch and provided build regenerates `calc.csv` from code. During play
it refreshes once per simulation second, on weapon changes, target resets,
day/night changes, F5, and exit. The file is a current snapshot, not a growing log.

Rows cover arena geometry, cargo, shooter, weapons, projectiles, targets, ring
boundaries, fragments, lamp poles/heads, spotlights, and the active sun or moon.
The starting snapshot has 267 rows; active projectiles, fragments, muzzle flash,
and respawning targets can change that count.

Each row contains the actual scale, shear, rotations, translation, six intermediate
point mappings, final matrix, world point, material settings, and simulation time.
Light-source notes also record intensity, direction, attenuation, and cone values.

Most geometry remains unit-cube based. The explicit exception is the requested
round target: a unit disk of radius 0.5 and Z thickness 1, tessellated with 96 sides.
Its representative local point is `(0.5,0,0.5)`, on the printed front rim.
Six additional rows per visible target document the printed ring boundaries
analytically; they are labeled as regions of the same disk, not extra objects.
Inactive weapons and representative projectile samples are also explicitly labeled.

Export a reproducible starting snapshot without opening graphics/audio:

```powershell
.\main.exe --export-calc
.\main.exe --export-calc --calc alternate.csv
```

The default file is in the project root even when launched from a build directory.
Custom paths are relative to the working directory. Write failures return an error.

## Verification and previews

`mingw32-make test` runs math/gameplay tests plus a hidden-window GPU smoke test.
Tests cover closed walls, transforms, motion, cargo seeds, collision/cover,
all six shot thresholds, shotgun trigger grouping, rotated front-only scoring,
back/edge rejection, circular misses, respawn, lights, sound waveform validity,
menu buttons, and CSV categories.

The graphics check exercises menus, cameras, weapons, day/night, target front/back,
and sound toggles. It verifies visible geometry with depth/pixel readback, checks
OpenGL errors, and confirms night changes scene brightness. It opens the audio
device and queues silent buffers; it does not play test noises.

```powershell
New-Item -ItemType Directory -Force build
.\main.exe --smoke-test --capture build/final.ppm
```

This saves PPM previews for player view, menus, controls, day arena, night arena,
night player, and target front/back. Run the visible app to check controls and
listen to audio at your system volume. After testing, `--export-calc` restores
the starting snapshot if desired.

## Source layout

- `Transform.h`: shared model stages, point tracing, view/projection math.
- `Arena.*`: fully enclosed arena and calculation export.
- `Camera.*`: camera view, look, and free movement.
- `Game.*`: targets, ring scoring, cargo, player, collision, projectiles,
  fragments, scores, and snapshots.
- `Weapon.cpp`: weapon settings, cube models, muzzle placement.
- `Lighting.*`: shared light rigs, fixtures, sun/moon geometry.
- `Sound.*`: effect synthesis and asynchronous Windows playback/muting.
- `Interface.*`: menus, HUD, scoring instructions, toggles, crosshairs, font.
- `Renderer.*` and `shaders/`: cube/disk rendering, lighting, sky, batched UI.
- `main.cpp`: application flow, input, timing, sound events, snapshot updates.
- `tests/`: transformation and gameplay checks.
