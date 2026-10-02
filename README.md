# 3D Target Shooter - Final Release (Phase 4 of 4)

C++17 / OpenGL 3.3 project by **Kazi Rifat Al Muin (2107042)**.

The completed four-phase project follows [final-project.md](final-project.md). All modes
reuse the seven level definitions, world, entities, collision and renderer. The original
Practice Sandbox remains. Final presentation includes countdown introductions, animated
level completion, seven-level victory, Free time-up, result statistics and ranked tables.

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
cmake -S . -B build/phase4-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build/phase4-release --parallel 4
ctest --test-dir build/phase4-release --output-on-failure
./build/phase4-release/main.exe
```

Portable CMake is available locally under `build/tools/cmake-3.31.6-windows-x86_64/bin/`;
use that executable path if CMake is absent from PATH. Bundled GLFW requires MinGW,
not MSVC. Linux needs GLFW/OpenGL development packages; playback is Windows-only.
Makefile and VS Code build tasks also work. Keep shaders and project files together.

## Mode menu

Click the name field before starting; enter up to 24 printable ASCII characters, use
Backspace to edit and Enter to accept. Blank names become Player. Names are case-sensitive
leaderboard keys, although the procedural font displays uppercase.

| Mode | Playable behavior | Persistence |
| --- | --- | --- |
| Free | Shared Level 7, 12 advanced targets, 48 birds, 36 humans; 03:00 countdown; targets return 30 seconds after destruction | Only after all 180 active seconds expire |
| Challenge | Sequential Levels 1-7; every target required; cumulative score/time/stats; no target respawn | After each cleared level |
| Developer | Seven clickable cards launch the exact shared levels; debug target IDs/health/motion, NPC counts, player position and FPS | Never submits |
| Bird's-Eye | Shared Level 7 overhead; click open ground to place a 1.7 m observation camera; mouse look and collision-aware walking | Never submits |
| Practice | Preserved Phase 1 sandbox, quick respawns and legacy scoring | Never submits |

Free targets pulse an amber cube warning during the final three seconds before respawn.
NPCs stay present around respawning zones. Clearing all targets does not end Free Mode.
At zero, firing/scoring freeze; a 1.6-second TIME animation leads into session statistics
and the Free leaderboard.
Pause, focus loss and introductions do not consume active time.

Developer R reloads the selected level. Esc pauses; SELECT LEVEL returns to its cards.
Completed Developer levels offer Reload, Select Level and Main Menu. In Bird's-Eye,
WASD pans and the wheel zooms. Click visible open ground outside cargo/walls/stands to
observe. Mouse controls yaw/pitch, WASD walks, Shift speeds up, and B/F2 returns overhead.
A cube ground marker and heading show the observation point. Shooting is disabled.

## Persistent leaderboard.csv

The project-root file is created with a header if missing. Builds and launches preserve
existing valid records. Schema:

```csv
Name,Mode,BestScore,LevelsCleared,TargetsDestroyed,Bullseyes,BirdKills,HumanKills,BestTimeSeconds,LastUpdated
```

The key is **(Name, Mode)**, with Mode equal to Free or Challenge. Higher score wins;
equal scores use lower active time. Negative scores are valid. Statistics are replaced
together with the winning snapshot, never mixed between runs. LastUpdated is UTC.
Times use round-trip precision. Commas and quotes in names are CSV-escaped. Duplicate
keys are read as their best record and coalesced on the next accepted save. Invalid
data rows are skipped with a visible warning; valid rows are preserved. Unrecognized
headers or malformed CSV are preserved with an error shown in the UI.

Challenge writes **only after completing a level**. Quitting/restarting an incomplete
level cannot replace its saved completed snapshot. Direct Developer launches do not
qualify as Challenge progress. Free writes **only after the full three-minute session**.
Developer, Bird's-Eye and Practice never create competitive records.

Writes use a checked temporary file and atomic replacement. Failed saves retain the
result in memory for RETRY SAVE during that application session. Close any program
locking the file before retrying. The delivered production leaderboard contains only
the header; test records are isolated under build/.

Results show current statistics, personal-best status, saved best rank and the relevant
mode table. Rows sort by score descending, then time ascending; names break exact ties
deterministically. The current player is highlighted. Scroll with the wheel, Page Up/Down
or UP/DOWN buttons. The menu table has Free/Challenge filters. Replay, Next Level where
applicable, and Main Menu remain available.

## Seven-level Challenge

Choose **START CHALLENGE** or press Enter. Destroy all targets, wait for the brief
completion animation, then press Enter/NEXT LEVEL. Level 7 plays a 2.4-second VICTORY
presentation with cube confetti and a musical cue before revealing run statistics.
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

Humans have varied **2.05-2.20 m** standing heights (2.10 m reference). Their whole
assemblies scale proportionally, including heads, bodies, arms, legs, hands and shoes.
The projectile colliders use those exact animated cube transforms. Some walkers start
on, or cross, the near side of target zones and can obscure lower targets: wait for a
clear shot or reposition. They remain bounded, avoid obstacles and do not permanently
prevent target completion. A human hit gives a longer, stronger red warning and a
distinct low sound; multiple pellets from one trigger cannot duplicate the penalty.

Cargo uses **0.70 m** crates (reference human height / 3), stacks of 1-5 crates with
a middle-height tendency, 3-7-crate runs and L/T formations. Every crate owns its bands
and inventory plate; filtering near target movement corridors removes whole assemblies.
Advanced cover screens require moving around their ends. Crate dimensions and human
height limits are defined together in src/world/Dimensions.h.

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
| Mouse | Aim in F1; look in F4 or the Bird's-Eye observation camera |
| Left click / Space | Fire in F1; hold for rifle, new press for pistol/shotgun |
| 1 / 2 / 3 | Pistol 25 m / shotgun 18 m / assault rifle 70 m |
| F1 / F2 / F3 / F4 | Player / elevated / side / free camera |
| Q / E | Free camera down / up |
| Tab | Release/capture pointer for HUD buttons |
| N / M | Day-night / sound |
| R | Restart unfinished Challenge level, Free session or selected Developer level; reset Practice targets |
| B / F2 in Bird's-Eye | Return to overhead inspection |
| Wheel / Page Up / Page Down | Scroll leaderboard; wheel zooms overhead |
| F5 | Save calculations while playing |
| Esc | Pause/resume; back from controls/results; exit main menu |

Free camera cannot move the grounded player or fire. Practice preserves Phase 1 respawn
and damage-plus-destruction scoring. Starting a new session resets its statistics.
A UI click cannot also fire; release fire after starting/resuming. Focus loss pauses.

All Phase 1 systems remain: cube weapons, distinct crosshairs, meter ranges, visible
swept projectiles, player/cargo/boundary collision, four cameras, enclosed 60 x 100 m
arena, fortified walls, seeded 0.70 m cargo, equipment display, day/night, ambient/diffuse/
Blinn-Phong shading, directional sun/moon, eight point lamps and six spotlights.
**All modeled objects use transformed unit cubes.** Targets use 32 cube slices and
a front-face ring material, not a disk mesh. Weapons include trigger guards, muzzle
insets and grip/slide details; birds have animated wings, tips and eyes; stands have
braces and warning stripes. Masonry relief, aisle markings, lamp frames/footings and
equipment labels complete the environment. UI fades and hover pulses use alpha blending.
Hit fragments keep animating after the last target is destroyed.

## Automatic actual-scene calc.csv

**Every launch regenerates calc.csv in the project root.** It refreshes about once per
active second, on weapon/lighting/session changes, F5 and normal exit. Builds refresh
it too; calc-init.csv is unchanged.

The logger observes the same objects sent to the renderer and maps cube corner
(0.5,0.5,0.5) using **M_model = T * Rz * Ry * Rx * H * S**.
Rows contain Phase 4, Mode, Level (0 = practice), object/component, local point, scale, six
shears, XYZ rotations, translation, matrix order, intermediate points, full matrix,
world point, purpose, lighting, observation time and UTC. Explicit material fields store
color, specular strength, shininess, emission and the target-pattern flag. Parent/group
IDs associate parts with their human, bird, target, weapon or crate assembly.

The 29-column schema is:

```csv
Phase,Object_ID,Object_Type,Component,Primitive,Local_Point,Scale,Shear,Rotation_X_deg,Rotation_Y_deg,Rotation_Z_deg,Translation,Matrix_Order,Matrix_or_Operation,Result_World_Point,Lighting_or_Use,Notes,Parent_or_Group,Purpose,Generated_UTC,Level,Mode,Color_RGB,Specular,Shininess,Emission,Target_Pattern,Snapshot_State,Observed_Time_seconds
```

Every current 3D cube instance is included. History retains **every named component**
of visited levels/modes, including all cargo details, walls, lamps, targets, NPCs and
weapons. Snapshot_State distinguishes Current from Last observed, with original times.
Repeated projectile and break-effect history retains the latest actual observation per
component/mode/level to avoid unlimited growth; all currently present instances are
always included. **Unvisited levels are not invented.** History starts
fresh each launch; completing a run provides all seven levels' coverage.
Startup includes real equipment-display weapon/projectile models; DISPLAY_ rows are
exhibits, while fired projectiles have separate IDs.

The delivered CSV comes from the all-mode rendering test, including Challenge/Developer
Levels 1-7, Free respawns and Bird's-Eye inspection. Your next launch replaces
it with your own session's observations. It is a snapshot plus documented observations,
not an every-frame recording or saved game. Bird's-Eye includes rendered observation
marker/heading transforms with real camera position/yaw/pitch notes. Writes use temporary files and atomic
replacement. An owned copy of the latest snapshot is written by a background worker;
pending writes coalesce, and normal exit flushes the final snapshot. This keeps large
all-level exports from blocking rendering. Close applications that lock the CSV.
HUD text, menus and sound samples are not modeled 3D objects; leaderboard.csv stores
competitive statistics separately.

```powershell
./main.exe --export-calc
./main.exe --export-calc --test-level 7
./main.exe --test-level 4
./main.exe --smoke-test
./main.exe --modes-smoke-test --capture build/phase4.ppm --leaderboard build/test-leaderboard.csv
powershell -NoProfile -ExecutionPolicy Bypass -File tests/verify_calc.ps1 -RequirePlayedCoverage -RequireChallengeCoverage -RequireModeCoverage
```

Export initializes a real scene without a GPU. --test-level N opens Developer Mode directly.
--mode free|challenge|birds-eye starts that mode; --name NAME supplies its player name.
--calc PATH and --leaderboard PATH override the CSV destinations; create parent directories
first. --capture saves PPM previews with any smoke option. Smoke tests default to
build/smoke-leaderboard.csv so they do not populate the production leaderboard.

## Architecture

The source tree is grouped by responsibility. Include paths are relative to src/:

```text
src/
  main.cpp             five-line entry point
  core/                Application, Transform, SceneObject, Collision
  rendering/           Renderer and GPU resource ownership
  camera/              Camera and BirdEyeCamera
  world/               Arena, Cargo, Dimensions, Environment, Lighting, LevelWorld
  gameplay/            Game, GameScene, GameMode, SessionController, Challenge,
                       LevelManager, Movement, Target, Weapon, Projectile,
                       Npc, Bird, Human, ScoreSystem, Effects
  levels/              LevelBase and Level1 through Level7 configurations
  ui/                  Interface, ModeUI, LeaderboardUI, Presentation,
                       UiPainter, UiState
  persistence/         CsvFile, CsvLogger, SnapshotWriter, Leaderboard
  audio/               Sound and SoundEvent
  testing/             ModeSmoke (explicit GPU automation only)
  third_party/         bundled GLAD implementation
shaders/               cube materials and alpha-blended UI shaders
include/ and lib/      bundled GLFW/GLAD headers and MinGW GLFW library
tests/                 CPU gameplay, release, persistence and CSV math checks
```

GameScene assembles actual renderable objects. Shared model builders feed both rendering
and collision; Transform.h owns the common S/H/Rx/Ry/Rz/T math. Level files configure
shared algorithms rather than duplicating gameplay. CsvLogger observes rendered objects;
SnapshotWriter owns disk work. CsvFile.cpp is now src/persistence/CsvFile.cpp.
All sources are registered in CMakeLists.txt, Makefile and build.bat; VS Code resolves
headers through src/ and include/. No external model, font or sound assets are required.

## Validation and limits

CTest runs ten checks: final human obstruction/model proportions, cargo reference sizes,
all-component snapshot retention/worker flushing/presentation/audio; retained Phase 1
gameplay, Challenge rules, shared modes/storage,
separate write/read processes for restart persistence, export, and three independent CSV
arithmetic/coverage checks. Tests cover every level, swept target/NPC contacts, actual player-muzzle firing
against live moving/spinning targets, movement locks, NPC population/bounds/avoidance,
cover accessibility, exact/negative scores, no respawn, completion gating, accumulated
time and restart rollback. Phase 3 checks cover Free expiry/respawn/warnings, negative
penalties, fresh health/bonus eligibility, pause/reset, every Developer card, independent
projection/unprojection, observation collision, completed-only submission, coherent best
records, escaping, duplicate keys, malformed-file preservation, sorting and scrolling.

The hidden OpenGL --modes-smoke-test renders every level/mode, completion/victory/Free
results, Developer cards/debug HUD, leaderboard, overhead and selected observation views.
It advances all 180 seconds of Free simulation faster than real time and checks the
30-second respawn boundary. Its fast progression script injects hit events;
separate CPU tests verify normal muzzle/projectile behavior. It checks visible geometry,
OpenGL errors, day/night brightness and exports visited transforms. Audio initialization
and queueing are checked; subjective quality requires listening.

The F4 free-fly camera passes through geometry; the Bird's-Eye observation camera stays
outside cargo and walls. Stands/fragments/avatar do not block shots; humans avoid stands.
Cast shadows, networking and an ammunition economy are outside this release.

## Final release changes

The formerly flat source directory is organized into the folders above. New reusable
modules are world/Dimensions.h, gameplay/GameScene.cpp, gameplay/Effects.*,
ui/Presentation.* and persistence/SnapshotWriter.*. Human/cargo/world/weapon/target/bird
models, UI, audio, scene logging and application integration were refined. Build files,
VS Code include paths, shaders/ui.*, tests and this README match the final layout.
tests/release_tests.cpp covers the final changes. main.exe and calc.csv are regenerated
from the updated project; valid leaderboard records are preserved across builds/runs.
