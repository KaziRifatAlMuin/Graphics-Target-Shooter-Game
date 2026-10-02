# 3D Target Shooter - Final Release (Phase 4 of 4)

C++17 / OpenGL 3.3 project by **Kazi Rifat Al Muin (2107042)**.

The completed four-phase project follows [final-project.md](final-project.md). All modes
reuse the seven level definitions, world, entities, collision and renderer. The original
Practice Sandbox remains. Final presentation includes countdown introductions, animated
level completion, seven-level victory, Free time-up, result statistics and ranked tables.

## Guide contents

- [Build and run](#build-and-run), [mode menu and Challenge name entry](#mode-menu)
- [Leaderboard persistence](#persistent-leaderboardcsv), [seven levels](#seven-level-challenge), [controls and scoring](#scoring-and-controls)
- [Transformation CSV](#automatic-actual-scene-calccsv), [architecture](#architecture), [validation](#validation-and-limits)
- [Complete play flow](#complete-play-flow), [object inventory](#world-coordinates-and-object-inventory), [populations and spawns](#initial-population-and-cargo-placement)
- [Weapons](#weapons-aiming-and-collision-details), [exact level layouts](#detailed-target-layouts-and-movement), [lighting/cameras/performance](#lighting-cameras-effects-and-performance)
- [Generated per-component counts, dimensions and target configurations](docs/scene-reference.md)

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

Challenge always opens its own name-entry page before Level 1 (also on Replay and
--mode challenge). Enter 1-24 printable ASCII characters; surrounding spaces are removed.
Blank Challenge names are rejected. Press Enter or CHECK NAME / START. If that exact name
already has a Challenge record, the game shows its rank, best score and levels cleared.
A second explicit USE EXISTING NAME / Enter confirms reuse; REWRITE NAME clears the
field, and Esc/Back returns to the menu. Confirmation never deletes or resets a record.
Only a better eligible result can replace it. A name existing only in Free does not
trigger the Challenge warning. Keys are case-sensitive, although the font is uppercase.

Free uses the editable name on the main menu; a blank menu name becomes Player. This is
a local callsign system, not an account/password system. Leaderboard data is loaded on
entry and rechecked on submission; typing checks the in-memory list without disk reads.

| Mode | Playable behavior | Persistence |
| --- | --- | --- |
| Free | Shared Level 7, 12 advanced targets, 48 birds, 36 humans; 03:00 countdown; targets return 30 seconds after destruction | Only after all 180 active seconds expire |
| Challenge | Sequential Levels 1-7; every target required; cumulative score/time/stats; no target respawn | After each cleared level |
| Developer | Seven clickable cards launch the exact shared levels; debug target IDs/health/motion, NPC counts, player position and FPS | Never submits |
| Bird's-Eye | Shared Level 7 overhead; click open ground to place a 1.7 m observation camera; mouse look and collision-aware walking | Never submits |
| Practice | Preserved Phase 1 sandbox, quick respawns and legacy scoring | Never submits |

Free targets pulse an amber cube warning during the final three seconds before respawn.
Surviving NPCs stay around respawning zones; killed NPCs never respawn in that session.
Clearing all targets does not end Free Mode.
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
locking the file before retrying. Your production records are preserved; test records are isolated under build/.

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
| 5 | 8 | Retains six; adds spinning X+Z and Y+Z targets | 8 initial birds |
| 6 | 10 | Retains eight; adds two faster XYZ spinners | 16 birds near target zones |
| 7 | 12 | Retains ten; adds two still faster XYZ spinners; densest cargo | 4 birds and 3 humans per active zone |

Motion uses shared bounded patterns with varied amplitude, phase, speed, direction and
spin. Birds choose bounded random 3D destinations and flap. Humans walk/pause on the
ground near their target zones and avoid cargo, walls and moving stands. A hit is fatal:
the live collider is removed immediately, the penalty is applied once, a distinct death
sound plays and a short red cube-particle burst appears. Birds tumble under gravity;
humans collapse as connected assemblies over 0.75 seconds. Fallen bodies remain visible
but do not obstruct movement or shots. Grounding uses the actual transformed cube bounds.
There are no replacement birds or humans during that level/session, including after a
Free target respawns. Reloading/restarting explicitly creates a fresh initial population;
advancing Challenge loads the next level's own population. Surviving Level 5-6 birds
redistribute when their zone target is cleared; Level 7 retires surviving NPCs from
cleared zones. Killed NPCs never participate in that redistribution.

This permanent-death rule is the latest requested behavior and supersedes the earlier
specification's continuously replenished NPC populations. Target respawn rules are unchanged.

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
| Enter | Open Challenge name entry / submit name / confirm existing name / resume / advance cleared level |
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
Repeated projectile, blood and break-effect history retains the latest actual observation per
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
Alive, falling and dead NPC assemblies have distinct BIRD_/DYING_BIRD_/DEAD_BIRD_ and
HUMAN_/DYING_HUMAN_/DEAD_HUMAN_ IDs, preceded by the mode/level prefix. Notes record life
state and death time. Blood cubes use BLOOD_ IDs. Short effects are observed every rendered
frame; the expensive full static snapshot is copied only when a disk refresh is due.
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
time and restart rollback. Death/name tests additionally check actual fatal contacts,
connected/grounded falls, no revival after target respawn, bounded effects, death sounds,
blank names, whitespace normalization, mode-specific duplicate warnings, explicit reuse,
rewrite, replay and preservation of an existing best. Shared mode checks cover Free expiry/respawn/warnings, negative
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

## Complete play flow

1. Launch ./main.exe (or ./run.bat to rebuild first). Set sound/day-night from the menu.
2. Challenge: choose CHALLENGE, enter a name, and resolve any duplicate-name confirmation.
   The 3/2/1/GO introduction lasts 1.5 seconds and does not consume score time.
3. Levels 1-3 lock the player's position; use mouse aim and the rifle for farther targets.
   Levels 4-7 allow walking around cover. The barrel must also be clear of nearby cargo.
4. Use the range HUD and weapon-specific crosshair. Only the printed front of a target scores;
   rotating back/edge hits stop the shot. Wait for a front face or move to another angle.
5. Destroy every required target. Avoid birds/humans: every first fatal NPC hit deducts points.
   Dead bodies are visual remains, not additional scoring opportunities or new obstacles.
6. The level-complete animation runs, then Next Level becomes available after one second.
   Score, time and statistics carry forward. Each completed level may improve the leaderboard.
7. After all 46 Challenge targets across seven levels, Victory leads to full statistics and
   the Challenge table. PLAY AGAIN/RESTART RUN returns through name entry and starts at Level 1.
8. In Free, play for 180 active seconds. Targets respawn 30 seconds after destruction,
   including fresh health and bonus eligibility; killed NPCs stay dead. Expiry leads to stats
   and the Free leaderboard. Early quitting does not create an eligible Free result.
9. Developer cards start any shared level without a competitive record. Bird's-Eye offers
   inspection and collision-aware observation walking without shooting or scoring.
10. Esc/focus loss pauses active gameplay. Resume continues the same state; explicit R/reset
    restores the selected level/session and its NPC population. Exiting an incomplete Challenge
    retains only previously eligible completed-level records. F5 requests a fresh transform CSV.

## World coordinates and object inventory

All sizes below are in **meters**, normally X width x Y height x Z depth before rotation.
The floor is Y=0, X is left/right, and forward is negative Z. The arena footprint is
X=-30..30, Z=-100..0; grounded movement is restricted to X=-26..26, Z=-96..-4 with a
0.45 m conservative player margin around blockers. Spawn eye position is (0,1.7,-5),
yaw -90 degrees, pitch 3.8 degrees. Walk/sprint speeds are 5/9 m/s; diagonal input is normalized.
The projection is 60 degrees vertical FOV, near 0.05 m, far 400 m.

The exhaustive, **generated** [component and level reference](docs/scene-reference.md)
lists every starting component category, its exact count in each level, measured scale
ranges, initial center ranges, and all 46 target configurations. It is generated from
shared model builders, so dimensions/counts can be regenerated after editing the game.
calc.csv gives every observed named instance's full transform, material, parent and world point.

| Assembly | Number / components | Dimensions and appearance | Placement / behavior |
| --- | --- | --- | --- |
| Ground | 1 cube | 60 x 0.2 x 100, muted green | Center (0,-0.1,-50); top at Y=0 |
| Aisle paint | 44 cubes | 0.075 x 0.018 x 1.35, yellow-gray | X=+-2.8, Y=0.012, Z=-8,-12,...,-92 |
| Enclosing walls | 4 cubes | North/south 60 x 8 x 1; sides local 100 x 8 x 1 rotated 90 degrees | Z=0/-100 or X=+-30, Y=4; solid gray boundary |
| Masonry relief | 12 cubes | Side courses 0.14 x 0.13 x 99; end courses 59 x 0.13 x 0.14 | Y=2,4,6 at X=+-29.44 or Z=-0.56/-99.44 |
| Battlements | 50 cubes | 2 x 1.5 x 1.5 | Y=8.75; north/south X=-27..27 every 6; sides Z=-6..-90 every 6 |
| Corner towers/caps | 4 + 4 cubes | Towers 3.5 x 9.5 x 3.5; caps 4.2 x 1 x 4.2 | X=+-30, Z=0/-100; Y=4.75/10 |
| Leaning supports | 18 cubes | 1.5 x 6 x 2, XY shear 0.22, rotated and grounded | Side X=+-28, Z=-18,-42,-66,-90; end Z=-2/-98, X=-20,-10,0,10,20 |
| Cargo crate | 4 cubes per assembly | Main cube 0.70 each side; two thin bands and one inventory plate; brown/green/gray | Seeded side clusters and advanced cover screens; solid to player/projectiles |
| Target | 38 cubes alive; 6 stand parts after destruction | Plate diameter 1.6, thickness 0.16; 32 horizontal cube slabs with six printed rings | Centers in target tables below; Y-axis spin; only front scores |
| Target stand | Base + post + 2 braces + 2 stripes | Base 2 x 0.3 x 1.5; post 0.24 x h x 0.24, h=max(0.3,targetY-0.8) | Base Y=0.15; post follows target X/Z; humans avoid stand footprint |
| Bird | 10 cubes each | Body 0.62 x 0.26 x 0.28; head 0.24 x 0.24 x 0.23; wings 0.38 x 0.055 x 0.64 plus tips | Brown/gray feathers, yellow beak, two eyes; random bounded flight and flapping |
| Human | 21 cubes each | Standing height 2.05-2.20; reference body proportions uniformly multiplied by height/1.82 | Three shirt/skin palettes, trousers, shoes, arms/hands, neck/head/hair/eyes/nose; bounded walking |
| Point lamp | 8 fixtures, 6 cubes each | Pole 0.22 x 6.5 x 0.22; head 0.85 x 0.4 x 0.85; cap, footing and 2 frame strips | X=+-23, Z=-12,-38,-64,-90; light at Y=6.7 |
| Spotlight | 6 fixtures, 3 cubes each | Pole 0.16 x 8.8 x 0.16; head 1.1 x 0.4 x 0.65 tilted -28 degrees; footing | X=+-11, Z=-12,-42,-72; light at Y=9 |
| Equipment table | Table, mat, 3 labels | Table 12 x 1.1 x 3; mat 11.8 x 0.02 x 2.8 | Center (-9,0.55,-8); three weapon exhibits and three projectile exhibits |
| Sun / moon | Day 1 cube; night 4 cubes | Sun 4 x 4 x 4; moon 3 x 3 x 3 plus 3 patches; emissive | Main body (25,35,-80), rotated (0,25,15) |
| Player avatar | 4 cubes, hidden in first person | Torso 0.65 x 0.95 x 0.4; head 0.4 each side; legs 0.22 x 0.56 x 0.3 | Follows actual player, shown by external cameras |
| Weapon | Active pistol 11 / shotgun 14 / rifle 15 cubes; optional 1 flash | Metal receiver/barrel, brown grip/stock, guards, sights and details | Camera-relative muzzle offsets: (0.32,-0.22,-1.02) pistol; Z=-1.52 for long guns |
| Projectile | 1 cube each; shotgun 9 pellets per trigger | Sizes/speeds in weapon table below; yellow/emissive | Swept segment collision selects nearest cargo/target/live NPC contact |
| Blood-like effect | 14 cubes per bird hit; 24 per human hit | Red cubes 0.035-0.070; gravity 13.5 m/s^2; lifetime 1.0-1.6 s | Spawn at hit NPC; human burst at 65% body height; brief flattened ground flecks |
| Target fragments | 12 cubes per destroyed target | 0.12 x 0.12 x 0.05, orange; gravity 7; lifetime 0.75 s | Radial burst from actual target center |
| Victory/session confetti | Up to 84 / 28 cubes | 0.24 x 0.14 x 0.32, four colors, emissive | Timed ballistic burst near (0,22,-26), at most 4 seconds |
| Observation references | 2 cubes in Bird's-Eye | Ground marker 0.9 x 0.08 x 0.9; heading 0.12 x 0.1 x 0.75 | At actual selected observation camera; heading follows yaw/pitch |

## Initial population and cargo placement

Counts below are measured from the shipped seed **2107042**. A crate is an assembly,
not one CSV row: its crate, bands and plate produce four rows. Free/Developer 7/Bird's-Eye
share Level 7 geometry. Populations decrease after fatal hits; vector slot counts are not
live NPC counts. Developer HUD shows live counts. Corpses stay until the world is reset.

| Level | Targets | Initial birds | Initial humans | Crate assemblies | Cargo component cubes |
| --- | ---: | ---: | ---: | ---: | ---: |
| 1 | 3 | 0 | 0 | 349 | 1396 |
| 2 | 3 | 0 | 0 | 349 | 1396 |
| 3 | 4 | 0 | 0 | 349 | 1396 |
| 4 | 6 | 0 | 0 | 638 | 2552 |
| 5 | 8 | 8 | 0 | 704 | 2816 |
| 6 | 10 | 16 | 0 | 770 | 3080 |
| 7 | 12 | 48 | 36 | 836 | 3344 |

Side clusters use lanes X=-20,-15,15,20 with seeded X jitter of -0.24..0.24.
Rows are Z=-20-10*r: four rows in Levels 1-3, seven in Levels 4-7 and Practice.
Each cluster has a 3-7-column run, optional two-column L/T extension, 0.735 spacing,
and yaw 0/90/180/270. A column has 1 + random(0..2) + random(0..2) crates: heights 1..5,
with probabilities proportional to 1:2:3:2:1. Crate centers are Y=0.7*(layer+0.5).
Practice contains 572 crates / 2288 cargo component cubes with the shipped seed.

Advanced levels add 2*(level-3) cover screens. Screen g is centered at
X=0.75*target[g].baseX, Z=target[g].baseZ+6, with seven columns spaced 0.705 m.
Outer columns are four crates high, inner columns five. Whole crate assemblies inside
a target's swept X/Z safety region are removed. Fixed-player levels validate rifle
sightlines; movable levels flood-fill navigation and require reachable firing positions.
A failing procedural layout retries up to four deterministic seeds (base + attempt*7919).

Bird i starts in zone i modulo target count, with seed base + i*193 + 47. Its destination
radius is 1.4..3.5 m around the zone base, altitude clamp(baseY-1 + random*3,1.3,7),
X clamped -24..24 and Z -94..-13. Speed is 1.8..4.1 m/s. Humans use seed
base + zone*547 + localIndex*193, speed 0.7..1.5 m/s, Y=0, radius up to 3 m;
55% of newly chosen destinations favor crossing in front of the target. Some initial
positions are directly 2.2 m in front of their current zone target when safe. Obstacles
and moving stands can cause a new waypoint or short pause. Death disables this AI.

## Weapons, aiming, and collision details

| Weapon | Effective range | Projectile speed | Cooldown | Pellets | Projectile cube size | Crosshair |
| --- | ---: | ---: | ---: | ---: | --- | --- |
| Pistol | 25 m | 42 m/s | 0.28 s | 1 | 0.10 x 0.10 x 0.25 | Center dot, four arms with wider gap |
| Shotgun | 18 m | 36 m/s | 0.80 s | 9 | 0.08 x 0.08 x 0.18 | Circular spread reticle |
| Assault rifle | 70 m | 65 m/s | 0.11 s | 1 | 0.13 x 0.13 x 0.30 | Compact dot/four arms |

Shotgun has a center pellet and eight spread pellets (spread parameter 0.075);
rifle spread parameter is 0.009. Pistol/shotgun require a fresh press; rifle supports
held fire. There is no ammunition/reload limit. Each target starts with 60 HP and
front-ring damage is 60/30/20/15/12/10. The strongest pellet from one trigger is used,
not nine independent damage events. One-trigger destruction earns +150 total;
ordinary destruction earns +100. Bird death costs -100; human death costs -200.
The target stand, corpse, avatar and cosmetic particles do not intercept projectiles.
Solid scenery does; both player-to-muzzle clearance and swept travel are checked.
The player is not damaged by NPCs and there is no health/lives game-over condition.

## Detailed target layouts and movement

Positions below are movement centers, not necessarily first rendered positions.
A=(Ax,Ay,Az) is the maximum displacement on each axis. With variation v and speed s:
frequency=(d*s,1.13*s,0.87*d*s), phase=(0.63*v,0.47*v,0.81*v),
where d=-1 for odd v and +1 for even v. Offset= A*sin(time*frequency+phase).
Spin is about local Y, signed by d. All levels share this algorithm. The generated
reference lists exact signed frequencies/phases for every target, including inherited ones.

| Level | Target bases and movement configuration |
| --- | --- |
| 1 | T1 (-5,2.4,-18), T2 (0,2.7,-22), T3 (5,2.4,-18); all static; player locked |
| 2 | T1 (-6,2.5,-26) static; T2 (0,3,-31), A=(2.3,0,0), s=1.1, v=0; T3 (6,2.5,-26) static; player locked |
| 3 | T1 (-8,2.8,-30), A=(2,0,0), s=1.2, v=1; T2 (-3,3.2,-39), A=(0,0.9,0), s=1.5, v=2; T3 (3,2.8,-34), A=(0,0,2.5), s=1.05, v=3; T4 (8,3.4,-45), A=(1.6,0,0), s=1.7, v=4; no spin; player locked |
| 4 | Six configurations in the next table; movement unlocked and cargo requires repositioning |
| 5 | Inherits Level 4; T7 (-8,3.3,-77), A=(1.9,0,1.6), s=1.65, spin=95, v=7; T8 (8,3.2,-78), A=(0,0.85,1.8), s=1.8, spin=110, v=8; eight initial birds |
| 6 | Inherits Level 5; T9 (-8,3.8,-88), A=(2.1,1,1.8), s=2.4, spin=135, v=9; T10 (8,4,-89), A=(1.9,1.1,1.9), s=2.6, spin=145, v=10; 16 initial birds |
| 7 | Inherits Level 6; T11 (0,4,-36), A=(2.2,1.1,1.9), s=3.4, spin=165, v=11; T12 (0,4.5,-72), A=(2.1,1.2,2), s=3.7, spin=180, v=12; four birds/three humans per zone |

| Shared Level 4-7 target | Base | A | s (rad/s base) | Spin magnitude (deg/s) | v |
| --- | --- | --- | ---: | ---: | ---: |
| T1 | (-8,2.4,-26) | (1.5,0,0) | 1.2 | 0 | 1 |
| T2 | (8,2.7,-28) | (0,0.6,0) | 1.4 | 65 | 2 |
| T3 | (-8,2.5,-42) | (0,0,1.6) | 1.15 | 0 | 3 |
| T4 | (8,2.9,-45) | (1.8,0,0) | 1.6 | 80 | 4 |
| T5 | (-8,3,-60) | (0,0.8,0) | 1.8 | 0 | 5 |
| T6 | (8,2.6,-63) | (0,0,1.7) | 1.35 | 0 | 6 |

Practice has seven legacy target bases: (0,2.7,-19), (-8,3,-29), (8,4.4,-38),
(-6,3.5,-52), (8,5,-65), (0,3.7,-82), (5,2.2,-16). It includes horizontal/vertical
movement, rotating plates and one fixed close target. It has no birds/humans, a 1.8 s
target respawn and legacy damage-plus-destruction scoring, separate from competitive modes.

## Lighting, cameras, effects and performance

Day ambient RGB is (0.30,0.32,0.35), night (0.055,0.07,0.10). Directional light points
along normalized (0.45,0.8,0.3), with day color (0.95,0.88,0.73) and night (0.07,0.09,0.16).
Eight point lamps use night color (0.65,0.54,0.36), attenuation 1/(1+0.045*d+0.003*d^2).
Six spots use (1,1.1,1.25), inner/outer cones 22/34 degrees and
1/(1+0.025*d+0.002*d^2). Point/spot contributions are zero by day. Materials use ambient,
diffuse and Blinn-Phong specular; individual RGB, specular strength, shininess and emission
are exported in calc.csv. Gun metal uses specular 0.75/shininess 80, fixtures 0.65/64,
targets 0.45/48, and human parts 0.06 specular. The sky uses a procedural shader.

F2 arena camera: (22,24,18), yaw -108, pitch -24. F3 side: (25,15,-42), yaw -175,
pitch -17. F4 begins at the currently active camera and flies at 12/30 m/s (Shift),
with minimum height 0.3; it does not move the grounded player. Mouse sensitivity is
0.12 degrees/pixel and pitch clamps to +-89 degrees. Overhead starts (0,120,-50),
pitch -89.9; wheel zoom is 8 m/step clamped 65..180, pan speed 25 m/s. Selected
observation height is 1.7 m and its walking collision matches the open arena constraints.

Simulation is stepped at at most 1/120 s; projectile sweeps avoid tunneling. NPC death
bursts are emitted once, not once per simulation step, and the shared particle budget is
250 (oldest effects are recycled). Blood lives at most 1.6 s; corpses are bounded by the
level's initial NPC population and have no active AI/collider. Corpse grounding evaluates
one transformed bound per part. Aim queries reuse a result within a simulation frame,
then invalidate on world updates, level resets and hits. Full CSV copies are periodic;
short-lived render effects are retained separately, and disk writes use the background
SnapshotWriter. Performance depends on resolution/GPU and no fixed FPS is promised.

Sound is synthesized locally: separate pistol/shotgun/rifle fire, target hit/break,
UI click, start/completion/victory/time-up, bird penalty/death, and stronger human
penalty/death. Bird death is a descending chirp (0.42 s); human death is a lower
noise/tone cue (0.70 s). M toggles sound; a missing output device leaves the game playable.
