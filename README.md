# 3D Target Shooter - Phase 1 of 4

C++17 / OpenGL 3.3 graphics project by **Kazi Rifat Al Muin (2107042)**.

This checkpoint implements the **Graphics Foundation + Playable Core** in
[final-project.md](final-project.md), sections 25-26. It extends the existing
project. Challenge levels, NPCs, competitive scoring, modes and leaderboards
belong to later phases; this version opens a working training sandbox.

## Build and run

Windows requires a **64-bit MinGW/GCC** compiler on PATH and an OpenGL 3.3 driver.
GLFW, GLAD and their headers are bundled. Models, UI font and sounds are procedural;
there are no external asset downloads. Windows sound uses WinMM.

```powershell
.\run.bat
```

`run.bat` builds, regenerates calculations and launches. Alternatively:

```powershell
.\build.bat
.\main.exe
```

CMake 3.15+ is supported and verified with CMake 3.31.6 / GCC 13.2:

```powershell
cmake -S . -B build/phase1-clean -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build/phase1-clean --parallel 4
ctest --test-dir build/phase1-clean --output-on-failure
.\build\phase1-clean\main.exe
```

Portable CMake downloaded during this checkpoint is under
`build/tools/cmake-3.31.6-windows-x86_64/bin/`; use that full executable path if
CMake is absent from PATH. It is a local build tool, not a game dependency.
The bundled GLFW archive requires MinGW, not MSVC. Linux CMake builds need installed
GLFW/OpenGL development packages; audio playback is currently Windows-only.

The existing `mingw32-make`, `mingw32-make run`, `mingw32-make test` and VS Code
build task also remain supported. Keep the project and shaders together.

## Playable features

- Enclosed 60 x 100 m arena, 8 m fortified walls, towers, battlements and sheared supports.
- 28 reproducible cargo clusters with 572 crates for the default seed. Each crate
  is 0.6 m (configurable human height / 3). Runs contain 3-7 columns, L/T formations
  add branches, and stacks vary from 1-5 crates with a preference for middle heights.
  Central corridors and cross aisles remain navigable.
- Seven sandbox targets: six existing horizontal/vertical/rotating targets plus
  one nearby static practice target. Every target is built from 32 thin cubes,
  approximating a 1.6 m round plate. The same cube slices drive collision.
- Pistol (25 m), shotgun (18 m, nine spreading pellets), assault rifle (70 m,
  repeated fire), with distinct cube models, recoil, muzzle flashes and visible projectiles.
- Pistol dot/short marks, shotgun spread circle/ticks, rifle compact cross;
  target distance and effective weapon range in meters.
- Grounded player movement with cargo/boundary collision; nearest swept projectile
  contact prevents shots passing through cover. A muzzle inside/beyond nearby cover
  cannot fire through it. Free camera flies independently for inspection.
- Player, elevated, side and free cameras; mouse look, menus, pause/resume, controls,
  target reset and clickable weapon/settings buttons.
- Ambient, diffuse and Blinn-Phong specular shading; directional sun/moon, eight
  point lamps and six soft-edged spotlights; day/night sky and emissive fixtures.
- Preserved synthesized firing, hit, break and UI audio, with mute and device status.

All 3D models use the shared centered unit cube. The later cube-only specification
supersedes the historical disk exception in `project.md`. Target rings are a front-face
material on the cube assembly; no cylinder/sphere/disk mesh is used. Screen-space HUD
and sky are shader/UI geometry, not modeled world objects.

## Controls

| Input | Action |
| --- | --- |
| Enter | Start from menu / resume paused session |
| WASD | Walk in F1-F3; fly in F4 |
| Mouse | Aim in F1; look in F4 with pointer captured |
| Shift | Faster movement |
| Left click / Space | Fire in F1; hold for rifle, new press for pistol/shotgun |
| 1 / 2 / 3 | Pistol / shotgun / assault rifle |
| F1 / F2 / F3 / F4 | Player / elevated / side / free camera |
| Q / E | Free camera down / up |
| Tab | Release / capture pointer for HUD buttons |
| N / M | Day-night / sound toggle, including menus |
| R | Reset targets and temporary effects; preserve score/player/weapon |
| F5 | Save current calculation snapshot |
| Esc | Pause/resume; back from controls; exit from main menu |

Menus and focus loss pause gameplay. Tab alone does not pause. A UI click is consumed;
release the fire button before firing after starting/resuming. Start Session resets the
player, weapon and practice statistics. Shooting is available only in F1.

Targets have 60 health and six front-only regions. From center outward the damage is
60/30/20/15/12/10, requiring 1/2/3/4/5/6 distinct triggers. Shotgun pellets share a shot ID
and contribute only the best region damage to a target. Back/edge contacts stop shots
without damage. Broken targets emit cube fragments and return after 1.8 seconds.
The existing practice score (damage plus 100 on destruction) is preserved; the later
Challenge/Free scoring rules are not yet active. Ammunition is unlimited.

## Automatic, actual-scene calc.csv

**Every normal launch overwrites `calc.csv` with a fresh scene snapshot.** It also updates
about once per second of active play, on weapon/lighting/reset/menu changes, on F5 and
on normal exit. Builds regenerate it too. Default output is the project root even when
launched from a CMake build directory. `calc-init.csv` remains untouched.

The logger receives the **same SceneObject instances passed to the renderer**. It
records one actual local cube corner `(0.5,0.5,0.5)` per instance, applying exactly:

```text
M_model = T * Rz * Ry * Rx * H * S
world_point = M_model * local_point
```

Columns include Phase, Object_ID, Object_Type, Component, Primitive, Local_Point,
Scale (X/Y/Z), Shear (XY/XZ/YX/YZ/ZX/ZY), Rotation_X/Y/Z_deg, Translation (X/Y/Z),
Matrix_Order, Matrix_or_Operation (intermediate points and full matrix), Result_World_Point,
Lighting_or_Use, Notes, Parent_or_Group, Purpose and Generated_UTC. Phase is **1**.
Notes record simulation time and day/night state. Values use six decimal places.

Current objects are marked `current scene snapshot`. For short-lived projectiles,
fragments, previously selected weapons and hidden target slices, the logger retains
bounded **last actual observations from this run**, explicitly marked as no longer active
with their original observation time. These are not current positions or fabricated
examples. History starts empty on every process launch. All three weapons and projectile
shapes are rendered on a cube-built equipment table near spawn, so startup exports cover
every implemented category. `DISPLAY_` rows identify these stationary exhibits; firing
adds separate moving projectile records. The supplied final CSV includes all three
weapons and fired projectile types from the tested run. Ground, walls, cargo,
targets/stands, lamps and environment are present
immediately. CSV is a snapshot/reference, not an every-frame recording or saved game.

Writes go to a temporary file and replace the completed CSV, preventing partial truncation.
Close spreadsheet applications that lock it; write failures produce an explicit error.
Manual CSV edits are overwritten. Edit scene code for lasting changes.

```powershell
.\main.exe --export-calc                 # Initial scene and equipment display, no GPU required
.\main.exe --export-calc --calc other.csv
.\main.exe --smoke-test --capture build/phase1.ppm
powershell -NoProfile -ExecutionPolicy Bypass -File tests/verify_calc.ps1 -RequirePlayedCoverage
```

`--calc PATH` overrides output; create parent directories first. `--capture` requires
`--smoke-test` and produces PPM views. `--help` describes arguments without starting a run.

## Modules and later phases

| Files in src/ | Responsibility |
| --- | --- |
| main.cpp, Application.h/.cpp | Five-line entry point; platform lifecycle, input, screen transitions, loop, smoke checks |
| Game.h/.cpp | Shared sandbox state and gameplay orchestration |
| Transform.h, SceneObject.h | Matrix pipeline, point tracing, common cube/material description |
| Renderer.h/.cpp, shaders/ | Shared cube buffer, materials, lighting upload, sky and UI |
| Collision.h/.cpp | Reusable affine cube ray tests and conservative player footprints |
| CsvLogger.h/.cpp | Current/observed scene records, transform export, atomic replacement |
| Arena.h/.cpp, Cargo.h/.cpp | Fortifications, configurable seeded cargo and reserved routes |
| Environment.h/.cpp, Lighting.h/.cpp | Sun/moon geometry and day/night light rigs/fixtures |
| Camera.h/.cpp | Movement, look and view matrix |
| Weapon.h/.cpp | Weapon specs, cube assemblies and muzzle placement |
| Projectile.h/.cpp | Projectile data, swept movement/collision and visuals; hit callback |
| Target.h/.cpp | Sandbox configuration, reusable movement, cube assembly and exact slice hits |
| Interface.h/.cpp | Working menus, HUD, crosshairs and built-in font |
| SoundEvent.h, Sound.h/.cpp | Shared sound events, synthesis and playback |

Later level configurations can supply targets and cargo without duplicating rendering,
weapons, collision or logging. No placeholder mode buttons, NPC systems, leaderboard
files or seven-level progression are introduced in Phase 1.

## Validation and current limits

CTest covers transform order and all shears, camera math, enclosed walls, grounded
supports, deterministic cargo, navigation paths, player/cover collision, weapon cooldowns,
spread/range, nearest hits, rotated front/back/edge hits, all six ring thresholds,
shotgun shot grouping, respawn, lighting, audio waveform validity and menus.
The CSV test independently recomputes every exported world point with scalar arithmetic.
The GPU smoke test exercises menu/controls/start/pause/resume, all weapons/cameras,
day/night brightness, front/back target views, audio toggles and OpenGL pixel/depth checks.

Free camera can pass through geometry for inspection; player collision is conservative.
Target stands, fragments and avatar do not block shots. There are no cast shadows,
networking, ammunition economy or persistent results. Actual audible quality requires
listening on the user's device; automatic audio checks verify initialization/queueing.
