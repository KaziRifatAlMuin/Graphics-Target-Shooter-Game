# 3D Target Shooter — Complete Game

A C++17 / OpenGL 3.3 graphics project by **Kazi Rifat Al Muin (2107042)**.

Practice shooting in a fully enclosed 60 × 100 m arena with six moving circular targets, three weapons, a movable shooter, four camera modes, menus, procedural audio, and day/night lighting. Scene calculations are generated automatically from the same transformations used by the game.

All six implementation phases are included in the current application. See [project.md](project.md) for the original specification.

## Contents

1. [Quick start and building](#quick-start-and-building)
2. [Features](#features)
3. [Gameplay and session flow](#gameplay-and-session-flow)
4. [Complete controls](#complete-controls)
5. [Menus and HUD](#menus-and-hud)
6. [Health, rings, and scoring](#health-rings-and-scoring)
7. [Target motion and lifecycle](#target-motion-and-lifecycle)
8. [Weapons and projectiles](#weapons-and-projectiles)
9. [Player and cameras](#player-and-cameras)
10. [Arena and object catalog](#arena-and-object-catalog)
11. [Object creation and transformations](#object-creation-and-transformations)
12. [Collision and simulation](#collision-and-simulation)
13. [Lighting and day-night system](#lighting-and-day-night-system)
14. [Sound system](#sound-system)
15. [Automatic CSV generation](#automatic-csv-generation)
16. [Architecture and source layout](#architecture-and-source-layout)
17. [Testing and previews](#testing-and-previews)
18. [Customization](#customization)
19. [Troubleshooting](#troubleshooting)
20. [Current limitations](#current-limitations)
21. [Presentation walkthrough](#presentation-walkthrough)

## Quick start and building

### Requirements

| Item | Requirement |
| --- | --- |
| Primary platform | Windows with 64-bit MinGW/GCC on PATH |
| Language | C++17; C support for the GLAD loader |
| Graphics | OpenGL 3.3 core-capable GPU and driver |
| Graphics libraries | GLFW and GLAD, bundled for the Windows build |
| Audio | Windows WinMM and an available output device |
| Optional tools | GNU Make or CMake 3.15+ |

The models, ring graphics, UI font, and audio are generated in code. No external texture, model, font, or sound downloads are needed.

### Build and launch

Open a terminal in the project directory:

~~~powershell
.\run.bat
~~~

This builds the executable, generates calculations, and starts the game after a successful build. To build and launch separately:

~~~powershell
.\build.bat
.\main.exe
~~~

The batch build uses C++17, optimization, compiler warnings, and the GLFW, OpenGL, GDI, and WinMM libraries. In VS Code, **Ctrl+Shift+B** invokes the same build.

Keep the executable and project resource layout together so the application can locate its shaders.

### Make

~~~powershell
mingw32-make
mingw32-make run
mingw32-make test
mingw32-make clean
~~~

The default build generates calculations even when compilation is already up to date. The test target runs core checks and a graphics smoke test.

### CMake

~~~powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\main.exe
ctest --test-dir build --output-on-failure
~~~

The bundled GLFW binary requires 64-bit MinGW rather than MSVC. CMake includes automatic calculation generation. CTest runs the core and export checks; the GPU smoke test is invoked separately.

The batch and Make workflows were tested during implementation. CMake configuration is supplied but was not locally verified because CMake was unavailable. On other platforms, CMake searches for installed GLFW/OpenGL development packages; audio playback currently uses a Windows-only backend.

### Command-line options

| Option | Purpose |
| --- | --- |
| --help | Display command-line help |
| --export-calc | Generate a fresh starting CSV and exit without graphics/audio initialization |
| --calc PATH | Select a different CSV output path |
| --smoke-test | Run the scripted hidden-window graphics check |
| --capture PATH | Save PPM previews during a smoke test |

~~~powershell
.\main.exe --help
.\main.exe --export-calc
.\main.exe --export-calc --calc alternate.csv
.\main.exe --smoke-test
~~~

Export-only and smoke-test modes cannot be combined. Capture requires smoke-test mode.

## Features

| Area | Implemented behavior |
| --- | --- |
| Arena | Floor, four closed walls, battlements, towers, caps, sheared supports, cargo |
| Mathematics | Custom vectors/matrices, translation, scale, three-axis rotation, six shear components |
| Player | Ground movement, collision, faster movement, mouse aiming |
| Cameras | Player, elevated, side, and independent free camera |
| Weapons | Pistol, shotgun, rifle, recoil, muzzle flash, visible projectiles |
| Targets | Six small moving disks, six printed rings, front-only damage |
| Gameplay | Target health, scoring, bullseyes, hit feedback, fragments, automatic respawn |
| Interface | Main menu, controls screen, pause menu, HUD, clickable buttons |
| Lighting | Day/night, sky gradient, directional light, eight point lights, six spotlights |
| Sound | Generated firing, hit, break, and click effects; on/off toggle |
| Calculations | Automatic starting and live CSV snapshots |
| Verification | Math/gameplay tests, export checks, graphics smoke test, preview capture |

## Gameplay and session flow

### Objective and first session

This is an endless shooting-range practice game. Destroy targets to earn points. Center hits deal the most damage; outer hits require more shots.

1. Open **View Controls** from the main menu.
2. Select **Start Session**, or press Enter.
3. Move with W/A/S/D and aim using the mouse.
4. Fire with left click or Space.
5. Switch to the rifle for distant targets.
6. Watch rotating targets and fire when their printed front is visible.
7. Use Esc to pause, inspect controls, or return to the menu.

The pistol is selected initially. Ammunition is unlimited. There are no enemy attacks, player-health points, reloads, timed victory conditions, or game-over screen.

### Pausing and resetting

| Action | State behavior |
| --- | --- |
| Esc during play | Pause and show the pause menu |
| Resume Session / Enter while paused | Continue the current session |
| R during play | Reset targets and elapsed simulation time; clear projectiles, fragments, recoil, cooldown, and feedback |
| Score after R | Preserved |
| Player, camera, and weapon after R | Preserved |
| Main Menu | Return to the opening menu |
| Start Session | Reset player, cameras, weapon, targets, and score counters |
| Window loses focus | Pause gameplay |

Menus freeze the simulation. Releasing the pointer using Tab does **not** pause target movement.

Day/night and sound selections survive a new session within the same application run. Preferences and scores are not saved across application restarts.

## Complete controls

### Movement and cameras

| Input | Action and context |
| --- | --- |
| W / A / S / D | Move the shooter in F1/F2/F3; move the free camera in F4 |
| Left or right Shift | Faster movement |
| Mouse movement | Aim in F1; look around in F4, while captured |
| Q / E | Free-camera down / up in F4 |
| F1 | Player view; aiming and shooting |
| F2 | Fixed elevated overview |
| F3 | Fixed side view |
| F4 | Independent free camera |
| Tab | Release or recapture the pointer |

F2/F3 allow observing the moving shooter but do not provide mouse aiming. F4 leaves the shooter in place while moving the camera.

### Combat and settings

| Input | Action |
| --- | --- |
| 1 / 2 / 3 | Pistol / shotgun / assault rifle |
| Left click / Space | Fire in F1 with the mouse captured |
| Hold fire with rifle | Repeated fire, limited by cooldown |
| New press with pistol or shotgun | One shot when ready |
| R | Reset targets and temporary gameplay effects |
| F5 | Write the current calculation snapshot during play |
| N | Toggle day/night |
| M | Toggle sound on/off |

Weapon selection, target reset, and F5 apply during gameplay. N and M also work from menus.

### Menus

| Input | Action |
| --- | --- |
| Left click with visible pointer | Activate a button |
| Enter at main menu | Start a new session |
| Enter while paused | Resume |
| Esc while playing | Pause |
| Esc while paused | Resume |
| Esc on controls screen | Return to the originating menu |
| Esc at main menu | Exit |

A click used to start, resume, or operate the UI does not also shoot. Release the button before firing after a UI action. Right mouse button, mouse wheel, and Home have no assigned gameplay action.

## Menus and HUD

The main menu contains Start Session, View Controls, Exit, and the mode toggles. The pause menu adds Resume Session and Main Menu. The controls screen has Back and mode toggles. During play, the HUD includes weapon buttons, Menu, Exit, day/night, and sound controls.

| HUD element | Meaning |
| --- | --- |
| Weapon and camera | Current selections |
| Score | Damage contributions plus destruction bonuses |
| Hits | Distinct damaging shot-target pairs |
| Cleared targets | Number of destructions |
| Bullseyes | Destructions caused by a center hit |
| Target health | Remaining health out of 60 for the aimed target in F1 |
| TARGET range | Distance from player position to the eligible target center |
| PLAYER AIM | In observer views, aim information still belongs to the shooter |
| Weapon range | Maximum projectile travel distance |
| Day/night | Current lighting mode |
| Sound | Enabled, disabled, or NO AUDIO DEVICE |

A dash in the target range means no unobstructed printed target front is under the player's aim. Range feedback becomes amber when the target-center distance exceeds the weapon range.

The crosshair turns teal over an eligible front face. A yellow marker briefly confirms damage. Ring feedback lasts approximately 0.9 seconds.

Player-to-center distance differs slightly from the projectile's actual muzzle-to-impact travel distance.

## Health, rings, and scoring

### Target health

Health belongs to targets; the player does not have a health bar.

Each target starts at **60 health**, with a **0.8 m radius**, **1.6 m diameter**, and **0.16 m thickness**. Its six rings have equal radial widths.

| Ring, center outward | Color | Outer radius | Damage | Same-ring shots to break a fresh target |
| --- | --- | --- | --- | --- |
| 1 — Bullseye | Red | 0.1333 m | 60 | 1 |
| 2 | Yellow | 0.2667 m | 30 | 2 |
| 3 | Blue | 0.4000 m | 20 | 3 |
| 4 | White | 0.5333 m | 15 | 4 |
| 5 | Red | 0.6667 m | 12 | 5 |
| 6 — Outer | White | 0.8000 m | 10 | 6 |

Dark separators are visual outlines, not extra health regions. The hit calculation uses the distance r from the disk center:

~~~text
ringIndex = min(5, int((r / targetRadius) * 6))
damage = 60 / (ringIndex + 1)
~~~

The internal ring index starts at zero. Mixed hits accumulate: an outer hit deals 10, a second-ring hit deals 30, and a third-ring hit deals the remaining 20.

### Shotgun grouping

Each trigger pull receives a unique shot ID. All nine shotgun pellets share it. For each target, only the strongest ring contribution from that trigger is credited.

~~~text
additional damage = max(0, new ring damage - previous best damage)
~~~

If an outer pellet first contributes 10 and a center pellet later contacts the same target, the later pellet contributes 50 more, for a total of 60. Nine outer-ring pellets from one trigger still contribute only 10 to that target.

Separate targets can each count a hit from the same trigger. Pellets cannot keep damaging a target after it breaks.

### Front-only damage

The printed face is local positive Z and must be approached from its front.

- Printed front: apply radial ring damage.
- Plain back: stop the projectile without damage, score, or hit feedback.
- Thin circular edge: stop the projectile without damage or score.
- Empty corners outside the disk: miss the target.

The valid front rotates with the plate. Wait for a rotating target to face you or move to its printed side.

### Scoring rules

Each newly credited damage contribution adds the same amount to the score. Destruction adds **100 points**. A center hit that causes destruction increments the bullseye counter.

Hits count distinct damaging shot-target pairs, not every pellet contact. Damage score is not capped to remaining health:

| Sequence | Score |
| --- | --- |
| One center hit on a fresh target | 60 + 100 = 160 |
| Six separate outer hits | 6 × 10 + 100 = 160 |
| Outer hit, then a separate center hit | 10 + 60 + 100 = 170 |

Health is clamped to zero when the target breaks.

## Target motion and lifecycle

### Target arrangement

| Target | Base position (X, Y, Z), meters | Motion |
| --- | --- | --- |
| 1 | (0, 2.7, -19) | Horizontal |
| 2 | (-8, 3, -29) | Vertical |
| 3 | (8, 4.4, -38) | Rotation |
| 4 | (-6, 3.5, -52) | Horizontal |
| 5 | (8, 5, -65) | Vertical |
| 6 | (0, 3.7, -82) | Rotation |

Phase is 0.8 times the zero-based target index. Time is measured in simulation seconds:

~~~text
Horizontal: X = baseX + sin(2.1 × time + phase) × 4
Vertical:   Y = baseY + sin(2.8 × time + phase) × 0.8
Rotation:   yaw = (140 × time + 30 × phase) modulo 360 degrees
~~~

Peak horizontal speed is 8.4 m/s, peak vertical speed is 2.24 m/s, and rotation speed is 140 degrees/s.

### Lifecycle

1. A valid hit reduces health and flashes the face for about 0.25 seconds.
2. At zero health, the plate disappears and 12 fragments spawn.
3. Fragments rotate and fall, disappearing after about 0.75 seconds.
4. After about 1.8 seconds, the target returns with full health and cleared shot history.

The stand and base remain during the respawn delay. Motion continues while the plate is hidden, so it reappears at its current animated position.

## Weapons and projectiles

### Weapon specifications

| Weapon | Range | Speed | Cooldown | Projectiles per trigger | Spread parameter | Fire mode |
| --- | --- | --- | --- | --- | --- | --- |
| Pistol | 25 m | 42 m/s | 0.28 s | 1 | 0 | One per press |
| Shotgun | 18 m | 36 m/s | 0.80 s | 9 | 0.075 | One per press |
| Assault rifle | 70 m | 65 m/s | 0.11 s | 1 | 0.009 | Hold to repeat |

Spread is a direction-vector offset before normalization, not an angle in degrees. The shotgun uses a center pellet plus eight circularly distributed offsets. Rifle spread varies deterministically with shot count.

Cooldown is shared across weapon switches, so switching does not bypass the remaining delay.

### Model construction

| Weapon | Cube components |
| --- | --- |
| Pistol | Body, grip, barrel, sight |
| Shotgun | Body, barrel, stock, grip, fore-end, front sight, rear sight |
| Rifle | Body, barrel, stock, grip, fore-end, front sight, rear sight, magazine |

Parts use different scales, local offsets, rotations, and selected shear values. The assembled model follows player position and aim. Firing applies recoil and a brief muzzle flash.

Projectile scales are (0.10, 0.10, 0.25), (0.08, 0.08, 0.18), and (0.13, 0.13, 0.30) for pistol, shotgun, and rifle respectively.

### Firing pipeline

1. Validate trigger state, F1 view, pointer capture, and cooldown.
2. Cast the player's aim ray to find an aim point.
3. Compute the selected weapon's modeled muzzle position.
4. Aim from the muzzle toward the aim point.
5. Check cover between player and muzzle.
6. Create projectiles with the trigger's shot ID.
7. Apply cooldown/recoil and queue a firing sound.
8. Move projectiles and resolve the nearest collision.

Projectiles move in straight lines and expire on collision or at maximum range. There is no bullet gravity, penetration, or ricochet.

## Player and cameras

The player starts at eye position **(0, 1.7, -5)** with yaw -90 degrees and pitch 3.8 degrees.

| Property | Value |
| --- | --- |
| Walking speed | 5 m/s |
| Fast walking | 9 m/s |
| Player X bounds | -26 to 26 |
| Player Z bounds | -96 to -4 |
| Free-camera speed | 12 m/s |
| Fast free-camera speed | 30 m/s |
| Minimum free-camera height | 0.3 m |
| Mouse sensitivity | 0.12 degrees per input unit |
| Pitch limits | -89 to +89 degrees |
| Perspective field of view | 60 degrees |
| Near / far planes | 0.05 / 400 |

Diagonal movement is normalized. Shooter movement is horizontal and constrained by the arena and obstacle bounds. The observer-view avatar has a cube body, head, and two legs; it is omitted in F1.

F2 starts around (22, 24, 18), and F3 around (25, 15, -42). Entering F4 copies the current camera before allowing independent movement. The free camera can pass through geometry for inspection. Projection aspect ratio follows window resizing.

## Arena and object catalog

One world unit represents approximately one meter. Positive X points right, positive Y points up, and negative Z extends downrange.

### Arena geometry

| Component | Count | Dimensions / construction |
| --- | --- | --- |
| Floor | 1 | 60 × 0.2 × 100, with top at Y = 0 |
| Boundary walls | 4 | 8 m high, 1 m thick |
| Battlements | 50 | Repeated blocks, typically 2 × 1.5 × 1.5 |
| Corner towers | 4 | 3.5 × 9.5 × 3.5 |
| Tower caps | 4 | 4.2 × 1 × 4.2 |
| Supports | 18 | 1.5 × 6 × 2 with shear and rotation |
| **Arena total** | **81** | |

The floor extends from X = -30 to 30 and Z = -100 to 0. All four sides are closed without doorways or boundary openings. Long side walls use rotated cube geometry.

Support grounding checks all eight transformed corners and places the lowest corner at Y = 0.

### Cargo

Ten stacks are generated with fixed seed **2107042**, using one to three levels. The current layout has 23 crates and 46 reinforcing bands.

Each crate is a 3 m cube with two narrow bands. Small yaw and position variations create variety. Stacks occupy the sides while preserving the central lane. The fixed seed makes initialization reproducible.

### Targets

Each target assembly has one plate, a narrow stand, and a 2 × 0.3 × 1.5 base. The stand follows the plate position and adjusts its height.

The plate is a 96-segment cylinder. A shader colors six radial bands on the front; the back and edge remain plain dark metal.

### Light fixtures

Eight point-light assemblies contain poles, heads, and caps. Six spotlight assemblies contain poles and heads, giving **36 fixture objects**.

Point lights sit at X = ±23 and Z = -12, -38, -64, -90, at height 6.7. Spotlights sit at X = ±11 and Z = -12, -42, -72, at height 9.

The emissive sun/moon cube is at (25, 35, -80), with scale 4 by day and 3 by night.

### Temporary objects

Projectiles, muzzle flashes, and fragments appear as needed. Each destruction produces 12 small cube fragments, with outward velocity, rotation, 7 m/s² downward acceleration, and a short lifetime.

## Object creation and transformations

### Shared geometry and data

Scene objects contain an identifier, category, component, primitive, transform, color, and material properties. Rendering and calculation export use this shared scene description.

Most objects begin as a centered unit cube from -0.5 to +0.5, rendered using 36 triangle vertices and face normals. Targets use a cylinder of radius 0.5 and local Z depth -0.5 to +0.5.

The UI uses separate screen-space geometry and a code-defined font.

### Model matrix

[Transform.h](src/Transform.h) implements vectors and matrices without GLM. Storage is column-major and points are column vectors:

~~~text
M = T × Rz × Ry × Rx × H × S
worldPoint = M × localPoint
~~~

The point receives **scale → shear → X rotation → Y rotation → Z rotation → translation**. Rotation values are in degrees. The six shear components form:

~~~text
H = | 1    xy   xz   0 |
    | yx   1    yz   0 |
    | zx   zy   1    0 |
    | 0    0    0    1 |
~~~

Operation order matters. Rendering and CSV tracing use the same stages.

### Worked example

For the floor, use local point (0.5, 0.5, 0.5), scale (60, 0.2, 100), and translation (0, -0.1, -50), with no shear or rotation:

~~~text
Local point:       (0.5, 0.5, 0.5)
After scale:       (30, 0.1, 50)
After translation: (30, 0, 0)
~~~

The result is a corner on the floor's upper surface.

Normals use the inverse-transpose of the model matrix's 3 × 3 linear part, preserving correct lighting under nonuniform scale and shear.

The six target rings are regions of one mesh, not six physical disks. Their additional CSV rows are analytical samples.

## Collision and simulation

Frame simulation delta is capped at 0.05 seconds. Gameplay subdivides updates into steps no larger than 1/120 second. This is bounded substepping rather than a fixed-step accumulator.

Player movement uses increments no larger than 0.15 m and resolves X/Z separately for sliding along obstacles. Cargo, supports, and light fixtures use conservative transformed bounds with a 0.45 m player margin.

Projectile ray tests transform cube geometry into local space and use a unit-cube slab intersection. This handles rotation, scale, and shear. Targets use a circular cylinder test in their local orientation.

Each projectile tests its traveled segment and resolves the nearest contact, so intervening cover blocks targets. Static scene geometry blocks projectiles; target stands/bases, the avatar, and fragments do not.

The target collider is an ideal mathematical circle; the rendered perimeter approximates it with 96 segments.

## Lighting and day-night system

Press N or use the button to change modes during play or from menus.

| Setting | Day | Night |
| --- | --- | --- |
| Ambient RGB | (0.30, 0.32, 0.35) | (0.055, 0.07, 0.10) |
| Directional RGB | (0.95, 0.88, 0.73) | (0.07, 0.09, 0.16) |
| Point lamps | Off | Eight warm lamps |
| Spotlights | Off | Six cool spotlights |
| Sky | Day gradient | Night gradient |
| Celestial object | Sun | Moon |

The directional-light direction is normalized (0.45, 0.8, 0.3). Point-light RGB is (0.65, 0.54, 0.36), and spotlight RGB is (1.0, 1.1, 1.25).

Distance attenuation is:

~~~text
Point: 1 / (1 + 0.045d + 0.003d²)
Spot:  1 / (1 + 0.025d + 0.002d²)
~~~

Spotlights face downward and downrange toward the central area. Their 22-degree inner and 34-degree outer cones blend smoothly.

Shading combines ambient, diffuse, and Blinn-Phong specular terms:

| Material | Specular strength | Shininess |
| --- | --- | --- |
| Default scene | 0.12 | 24 |
| Targets | 0.45 | 48 |
| Fixtures | 0.65 | 64 |
| Weapon metal | 0.75 | 80 |

Emissive surfaces retain their own color contribution. Output uses a gamma-style power of 1/2.2 and clamps to display range. Visible emissive objects do not independently illuminate surroundings; light comes from the defined rig. There are no shadow maps or global illumination.

## Sound system

[Sound.cpp](src/Sound.cpp) synthesizes effects without audio assets:

| Event | Approximate duration |
| --- | --- |
| Pistol | 0.16 s |
| Shotgun | 0.28 s |
| Rifle | 0.11 s |
| Target hit | 0.13 s |
| Target break | 0.34 s |
| UI click | 0.05 s |

Effects combine tones, changing frequencies, deterministic noise, and amplitude envelopes. Windows playback uses 22,050 Hz mono, 16-bit PCM, three 512-sample buffers, and up to 16 mixed voices.

Sound starts enabled. M or the button toggles it. Muting clears voices and queued playback, preventing old effects from replaying after unmuting.

Unavailable audio does not stop gameplay; the HUD displays **NO AUDIO DEVICE**. The non-Windows backend currently reports audio as unavailable.

## Automatic CSV generation

[calc.csv](calc.csv) is generated from the current code and scene transformations. [calc-init.csv](calc-init.csv) remains the reference file.

### Generation triggers

- Normal startup, before graphics/audio initialization.
- The provided standard build/export workflows.
- Approximately every second of active simulation.
- Weapon selection, target reset, day/night changes, and relevant menu actions.
- F5 during gameplay.
- Normal exit.

Help-only mode does not generate a snapshot. Each export replaces the file; it is neither an append-only log nor a saved game.

### Columns

| Column | Meaning |
| --- | --- |
| Object_ID | Object or sample identifier |
| Object_Type | Scene category |
| Component | Assembly component |
| Primitive | Geometry/sample type |
| Local_Point | Input point in local coordinates |
| Scale | X/Y/Z scaling |
| Shear | Six shear values |
| Rotation_X_deg | X rotation |
| Rotation_Y_deg | Y rotation |
| Rotation_Z_deg | Z rotation |
| Translation | World position offset |
| Matrix_Order | Composition order |
| Matrix_or_Operation | Intermediate stages and matrix |
| Result_World_Point | Final transformed point |
| Lighting_or_Use | Material/light or usage context |
| Notes | Additional state and explanation |

The 16-column structure follows the reference. Values use six decimal places and locale-independent formatting.

### Sample interpretation

Cube rows normally trace (0.5, 0.5, 0.5). Target plates use (0.5, 0, 0.5), a point on the unit cylinder's printed front rim.

Ring rows describe each radial region's outer boundary and damage. They are regions of the same disk, not extra drawn objects. Inactive weapons and representative projectile samples are explicitly identified as calculation previews.

The starting export has **267 data rows**:

| Category | Rows |
| --- | --- |
| Arena | 81 |
| Cargo | 69 |
| Lighting fixtures | 36 |
| Sun/moon | 1 |
| Target assemblies | 18 |
| Player avatar | 4 |
| All three weapon models | 19 |
| Ring samples | 36 |
| Additional calculation samples | 3 |
| **Total** | **267** |

Live snapshots may differ because plates can be broken and temporary effects can exist. Notes identify simulation time, lighting mode, and relevant material/light parameters.

### Export and file handling

~~~powershell
.\main.exe --export-calc
.\main.exe --export-calc --calc alternate.csv
~~~

The default path is the project root, even when launched from a build directory. Custom paths are relative to the working directory. Create parent directories for nested output paths. Write failures are reported as errors.

Manual changes to calc.csv are overwritten. Edit scene code for lasting changes. The export traces representative points, not every mesh vertex or every rendered frame.

## Architecture and source layout

~~~text
Graphics-Project/
|-- project.md          Original specification
|-- README.md           Player and implementation guide
|-- calc-init.csv       Calculation reference
|-- calc.csv            Generated snapshot
|-- build.bat           Windows build and export
|-- run.bat             Build and launch
|-- Makefile            Build/run/test targets
|-- CMakeLists.txt      CMake configuration
|-- .vscode/            Editor configuration
|-- include/            Bundled headers
|-- lib/                Bundled GLFW library
|-- shaders/            Object, sky, and UI shaders
|-- src/                Application implementation
|-- tests/              Automated checks
~~~

| Source | Responsibility |
| --- | --- |
| [main.cpp](src/main.cpp) | Initialization, input, screens, timing, audio events, export scheduling |
| [Transform.h](src/Transform.h) | Vector/matrix math, transformation tracing, view/projection |
| [Arena.cpp](src/Arena.cpp) | Arena construction and calculation export |
| [Camera.cpp](src/Camera.cpp) | View direction, look, camera movement |
| [Game.h](src/Game.h) | Gameplay types and shared interfaces |
| [Game.cpp](src/Game.cpp) | Targets, cargo, player, collisions, scores, effects, snapshots |
| [Weapon.cpp](src/Weapon.cpp) | Weapon specifications, assemblies, muzzle positions |
| [Lighting.cpp](src/Lighting.cpp) | Light definitions, fixtures, mode configuration |
| [Sound.cpp](src/Sound.cpp) | Effect synthesis, playback, muting |
| [Interface.cpp](src/Interface.cpp) | Menus, controls, HUD, crosshairs, font |
| [Renderer.cpp](src/Renderer.cpp) | Meshes, shader uniforms, scene/UI rendering |
| [object.vert](shaders/object.vert) | Vertex and normal transformation |
| [object.frag](shaders/object.frag) | Lighting, materials, printed target rings |
| [core_tests.cpp](tests/core_tests.cpp) | Math and scene checks |
| [game_tests.h](tests/game_tests.h) | Gameplay checks |

### Runtime flow

1. Parse options and find resources.
2. Construct the scene and export the initial calculations.
3. Exit if export-only mode was requested.
4. Initialize graphics and audio.
5. Poll input and process menu/gameplay actions.
6. Update active gameplay, audio events, and scheduled exports.
7. Render sky, world, weapon/effects, and interface.
8. Present the frame and repeat.
9. Export the final snapshot and release resources on normal exit.

## Testing and previews

~~~powershell
mingw32-make test
~~~

Core tests cover transformations, shear, camera math, closed boundaries, grounded supports, deterministic cargo, movement/cover collision, ring thresholds, shotgun grouping, rotated front-only hits, back/edge rejection, circular misses, cooldown, respawn, lighting, waveform validity, UI behavior, and CSV categories.

The hidden-window GPU test exercises menus, cameras, weapons, lighting modes, target front/back views, and sound controls. It checks OpenGL errors, depth/pixel readback, and day/night brightness differences.

Audio checks open the output device and queue silent buffers. Listen in the visible application to verify audible quality and system volume.

### Save previews

~~~powershell
New-Item -ItemType Directory -Force build
.\main.exe --smoke-test --capture build/final.ppm
~~~

This creates PPM views of the player, menu, controls, arena, night arena, night player, and target front/back. Open them in a compatible image viewer.

Testing can leave a live CSV snapshot. Run export-only mode afterward if a deterministic starting snapshot is desired.

## Customization

| Change | Main source | Keep consistent |
| --- | --- | --- |
| Arena size and geometry | [Arena.cpp](src/Arena.cpp) | Player bounds, enclosure, grounding |
| Cargo and target motion | [Game.cpp](src/Game.cpp) | Clear lanes, collision, playability |
| Target size/health | [Game.h](src/Game.h), [Game.cpp](src/Game.cpp) | Rendering, hit tests, damage table |
| Ring appearance | [object.frag](shaders/object.frag) | CPU radial hit regions |
| Weapon properties/model | [Weapon.cpp](src/Weapon.cpp) | HUD, muzzle position, projectile behavior |
| Camera movement | [Camera.cpp](src/Camera.cpp) | Input capture and view behavior |
| Lighting | [Lighting.cpp](src/Lighting.cpp) | Fixtures and actual source positions |
| Sound | [Sound.cpp](src/Sound.cpp) | Volume, clipping, envelope, muting |
| Interface | [Interface.cpp](src/Interface.cpp) | Actual controls and displayed instructions |
| Input and session flow | [main.cpp](src/main.cpp) | Menu state and README controls |
| CSV format | [Arena.cpp](src/Arena.cpp) | Shared transforms and expected columns |

Rebuild after source changes. The normal build regenerates calculations from the updated implementation.

## Troubleshooting

| Symptom | Action |
| --- | --- |
| g++ not found | Put the 64-bit MinGW compiler directory on PATH |
| GLFW link errors | Use a matching 64-bit MinGW toolchain |
| OpenGL startup failure | Check OpenGL 3.3 support and GPU driver |
| Missing shaders | Preserve the resource layout and use the root launch script |
| Pointer is hidden | Press Tab to release it |
| Cannot fire | Resume, select F1, capture the pointer, release/repress fire, and allow cooldown |
| Target takes no damage | Check printed front, weapon range, and intervening cover |
| Shotgun damage seems low | Pellets share one strongest-ring contribution per target |
| Far target is unaffected | Move closer or use the rifle |
| R preserves score | Expected; Start Session resets scores |
| No audio | Check M, system volume, output device, and HUD status |
| CSV write fails | Check directory existence, permissions, and file locking by another program |
| CSV row count changes | Live effects and broken plates change the snapshot |
| CSV edits disappear | Change source code; the file is regenerated |
| Preview capture fails | Use --capture with --smoke-test and create its parent directory |

## Current limitations

- Local single-player practice; no networking or persistent saves.
- No player health, enemies, ammo economy, reload, jump, crouch, or timed matches.
- Shooting is restricted to player view.
- Projectiles do not fall, bounce, or penetrate.
- Free camera passes through geometry.
- Player collision uses conservative bounds.
- Target stands/bases, avatar, and fragments do not block shots.
- No cast shadows or global illumination.
- Audio playback is Windows-only.
- CSV is a representative calculation snapshot, not a frame recording.
- Under severe frame delays, the simulation delta cap can make game time progress more slowly than wall-clock time.

## Presentation walkthrough

1. Build and show automatic CSV generation.
2. Read the controls screen and start a session.
3. Demonstrate shooter movement and aiming.
4. Show all four closed walls using the overview camera.
5. Inspect supports, cargo, and towers with the free camera.
6. Compare the three weapons.
7. Explain the six rings and demonstrate center versus outer damage.
8. Show a back-face hit causing no damage.
9. Break a target and observe fragments and respawn.
10. Toggle night mode and inspect lamps and spotlights.
11. Toggle sound off and on.
12. Compare target reset with starting a new session.
13. Export a snapshot and trace an object's scale, shear, rotations, and translation.
