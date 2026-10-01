# 3D Target Shooter - Phase 2 of 4

C++17 / OpenGL 3.3 project by **Kazi Rifat Al Muin (2107042)**.

This extends the working Phase 1 project with the seven-level Challenge specified in
[final-project.md](final-project.md), sections 25-26. The original Practice Sandbox remains
available. Full Free Mode, Developer Mode, persistence and final polish remain for Phases 3-4.

## Build and run

Windows requires **64-bit MinGW/GCC** on PATH and an OpenGL 3.3 driver. GLFW/GLAD
are bundled; models, fonts and sounds are procedural. Windows audio uses WinMM.

```powershell
./run.bat
# Or build and launch separately:
./build.bat
./main.exe
```

CMake 3.15+ is supported (verified with CMake 3.31.6 and GCC 13.2):

```powershell
cmake -S . -B build/phase2-clean -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build/phase2-clean --parallel 4
ctest --test-dir build/phase2-clean --output-on-failure
./build/phase2-clean/main.exe
```

Portable CMake is available locally under `build/tools/cmake-3.31.6-windows-x86_64/bin/`;
use that executable path if CMake is absent from PATH. Bundled GLFW requires MinGW,
not MSVC. Linux needs GLFW/OpenGL development packages; playback is Windows-only.
Makefile and VS Code build tasks also work. Keep shaders and project files together.

## Seven-level Challenge

Choose **START CHALLENGE** or press Enter. Destroy all targets, wait for the brief
completion animation, then press Enter/NEXT LEVEL. Level 7 ends with run statistics.
Challenge targets never respawn. Introduction, pause and completion screens freeze time.

| Level | Targets | Rules | NPCs |
| --- | --- | --- | --- |
| 1 | 3 | Position locked; static targets within pistol range | None |
| 2 | 3 | Position locked; farther targets, one moving on X; rifle reaches all | None |
| 3 | 4 | Position locked; varied single-axis X/Y/Z movement; clear rifle lanes | None |
| 4 | 6 | Movement enabled; varied single-axis speeds, two spinners; cover requires repositioning | None |
| 5 | 8 | Retains six; adds spinning X+Z and Y+Z targets | 8 continuously active birds |
| 6 | 10 | Retains eight; adds two faster XYZ spinners | 16 birds near target zones |
| 7 | 12 | Retains ten; adds two still faster XYZ spinners; densest cargo | 4 birds and 3 humans per active zone |

Motion uses shared bounded patterns with varied amplitude, phase, speed, direction and
spin. Birds choose bounded random 3D destinations and flap. Humans walk/pause on the
ground near their target zones and avoid cargo, walls and moving stands. Struck NPCs
are immediately replaced to preserve population. Levels 5-6 redistribute birds to
remaining zones; Level 7 retires NPCs from cleared zones.

The loader validates fixed-player sightlines/range, flood-fills walkable space from spawn
in movable levels, and checks reachable close firing positions. Seeded cargo layouts
avoid target movement regions and leave routes around cover.

## Scoring and controls

Targets have 60 health. Front-center through outer ring damage: **60/30/20/15/12/10**.
Shotgun pellets share a trigger ID and contribute only the strongest pellet damage.
Back/edge contacts stop shots without damage.

- Destroy target: **+100**. Destroy it on its first valid trigger: **+150 total**.
- A later center hit gives normal destruction points. Damage alone gives no points.
- Bird: **-100**. Human: **-200**. Negative scores are valid.
- NPC penalties count once per trigger. Points float; penalties flash red and sound.
- Score, active time, targets destroyed, bullseyes, bird/human hits, shots, target hits
  and cleared levels accumulate through the run.
- Restarting an unfinished level restores its starting score/time/statistics.
  Completed levels cannot be replayed inside that run to farm points.

| Input | Action |
| --- | --- |
| Enter | Start Challenge / resume pause / advance completed level |
| WASD / Shift | Walk / faster movement when allowed; fly in F4 |
| Mouse | Aim in F1; look in F4 |
| Left click / Space | Fire in F1; hold for rifle, new press for pistol/shotgun |
| 1 / 2 / 3 | Pistol 25 m / shotgun 18 m / assault rifle 70 m |
| F1 / F2 / F3 / F4 | Player / elevated / side / free camera |
| Q / E | Free camera down / up |
| Tab | Release/capture pointer for HUD buttons |
| N / M | Day-night / sound |
| R | Restart unfinished Challenge level; reset Practice targets |
| F5 | Save calculations while playing |
| Esc | Pause/resume; back from controls/results; exit main menu |

Free camera cannot move the grounded player or fire. Practice preserves Phase 1 respawn
and damage-plus-destruction scoring. Starting either session resets statistics.
A UI click cannot also fire; release fire after starting/resuming. Focus loss pauses.

All Phase 1 systems remain: cube weapons, distinct crosshairs, meter ranges, visible
swept projectiles, player/cargo/boundary collision, four cameras, enclosed 60 x 100 m
arena, fortified walls, seeded 0.6 m cargo, equipment display, day/night, ambient/diffuse/
Blinn-Phong shading, directional sun/moon, eight point lamps and six spotlights.
**All modeled objects use transformed unit cubes.** Targets use 32 cube slices and
a front-face ring material, not a disk mesh.

## Automatic actual-scene calc.csv

**Every launch regenerates calc.csv in the project root.** It refreshes about once per
active second, on weapon/lighting/session changes, F5 and normal exit. Builds refresh
it too; calc-init.csv is unchanged.

The logger observes the same objects sent to the renderer and maps cube corner
(0.5,0.5,0.5) using **M_model = T * Rz * Ry * Rx * H * S**.
Rows contain Phase 2, Level (0 = practice), object/component, local point, scale, six
shears, XYZ rotations, translation, matrix order, intermediate points, full matrix,
world point, purpose, lighting, observation time and UTC.

Current rows are marked as current snapshots. Bounded history retains actual previous
observations of moving/spinning targets, NPC parts, representative cargo, weapons,
projectiles and environment from visited levels, with original times and explicit
retained-observation labels. **Unvisited levels are not invented.** History starts
fresh each launch; completing a run provides all seven levels' coverage.
Startup includes real equipment-display weapon/projectile models; DISPLAY_ rows are
exhibits, while fired projectiles have separate IDs.

The delivered CSV comes from the seven-level rendering test. Your next launch replaces
it with your own session's observations. It is a snapshot plus documented observations,
not an every-frame recording or saved game. Writes use temporary files and atomic
replacement; close applications that lock the CSV.

```powershell
./main.exe --export-calc
./main.exe --export-calc --test-level 7
./main.exe --test-level 4
./main.exe --smoke-test
./main.exe --challenge-smoke-test --capture build/phase2.ppm
powershell -NoProfile -ExecutionPolicy Bypass -File tests/verify_calc.ps1 -RequirePlayedCoverage -RequireChallengeCoverage
```

Export initializes a real scene without a GPU. Test-level is a development CLI hook,
not the future Developer Mode. --calc PATH overrides output; create parent directories
first. --capture saves PPM previews with either smoke option.

## Architecture

| Files in src/ | Responsibility |
| --- | --- |
| main.cpp, Application.* | Small entry point; platform, input, screen transitions and loop |
| Game.*, Challenge.cpp | Shared simulation, level lifecycle and NPC orchestration |
| levels/LevelBase.*, Level1.* through Level7.* | Interface and seven data-oriented definitions |
| LevelManager.* | Intro/active/complete/finished states and gated progression |
| LevelWorld.*, Cargo.* | World construction, seeded cover and accessibility checks |
| Movement.*, Target.* | Parameterized motion, cube assembly and front-only collision |
| ScoreSystem.* | Cumulative statistics, exact scoring and animated feedback |
| Npc.*, Bird.*, Human.* | Shared random movement, NPC collision and cube animation |
| Weapon.*, Projectile.*, Collision.* | Reused weapons, swept nearest contact and cover collision |
| Transform.h, SceneObject.h, Renderer.*, shaders/ | Shared matrices, cube rendering and materials |
| CsvLogger.* | Current/observed transform export and atomic regeneration |
| Arena.*, Environment.*, Lighting.*, Camera.* | Preserved world, lights and views |
| Interface.*, SoundEvent.h, Sound.* | Functional menus/HUD and synthesized audio |

All new sources are in CMake, Makefile and build.bat. Rendering, movement, scoring and
collision algorithms are shared across levels.

## Validation and limits

CTest runs the retained Phase 1 suite, Challenge rules and independent CSV arithmetic
checks. Tests cover every level, swept target/NPC contacts, actual player-muzzle firing
against live moving/spinning targets, movement locks, NPC population/bounds/avoidance,
cover accessibility, exact/negative scores, no respawn, completion gating, accumulated
time and restart rollback.

The hidden OpenGL test renders all levels, completion/victory screens, menus, cameras,
weapons, lighting and NPC penalties. Its fast progression script injects hit events;
separate CPU tests verify normal muzzle/projectile behavior. It checks visible geometry,
OpenGL errors, day/night brightness and exports visited transforms. Audio initialization
and queueing are checked; subjective quality requires listening.

Free camera passes through geometry. Stands/fragments/avatar do not block shots; humans
avoid stands. Cast shadows, networking, ammunition economy and persistent results are
not included. Phase 3 can reuse LevelManager::completedStats as its completed-level
persistence boundary.
