# 3D Target Shooter --- Complete Project Specification

**Student:** Kazi Rifat Al Muin\
**Roll:** 2107042\
**Platform:** OpenGL / C++\
**Project Type:** Interactive 3D Graphics Project

## 1. Project Overview

### Final implemented gameplay requirements

All six phases are implemented. The final arena is fully enclosed on all four
sides. Targets are circular disks (the explicit exception to cube-based modeling),
1.6 m in diameter, with six concentric scoring regions printed only on the front.
From center outward, repeated hits require 1, 2, 3, 4, 5, and 6 distinct shots to
break a fresh target. Mixed hits accumulate damage against 60 health; the region
damage values are 60, 30, 20, 15, 12, and 10. Shotgun pellets share a shot ID, so
one trigger contributes at most the best region damage to a target. Back and
edge hits stop projectiles without scoring. Targets move faster, rotating targets
turn through 360 degrees, and broken targets emit fragments before respawning.

Day/night lighting includes ambient, diffuse, and Blinn-Phong specular response,
a directional sun/moon fill, eight point lamps, and six soft-edged spotlights.
N toggles day/night; M toggles synthesized game sound. Both also have menu/HUD
buttons. Instructions, scoring rules, controls, pause/resume, and exit are available
in the interface. `calc.csv` regenerates on every run and records current disk,
ring, lighting, and scene transforms; `calc-init.csv` remains unchanged.

This project extends the original **3D Target Shooter** proposal while
keeping its central idea unchanged: a simple OpenGL shooting arena with
movable/rotating targets, visible projectiles, lighting, multiple camera
views, and interactive controls. The original proposal already defines
an arena, player gun, targets, target stands, bullets, arena objects,
target movement, shooting animation, hit effects,
ambient/diffuse/specular lighting, multiple views, camera movement, and
keyboard/mouse interaction.

The extended version keeps the implementation intentionally
understandable for an academic computer-graphics project, but adds a
more complete arena, three visually and behaviorally distinct game
weapons, day/night modes, weapon-specific target pointers, range
visualization, random cargo-box placement, a fortified boundary wall,
sky/sun, arena lamps, three fixed camera presets plus a movable camera,
and a detailed transformation log in `calc.csv`.

> **Scope note:** Weapon behavior is designed as a fictional game
> simulation. The project focuses on computer-graphics modeling,
> transformations, animation, lighting, camera systems, collision/hit
> visualization, and UI rather than real-world weapon construction.

------------------------------------------------------------------------

## 2. Main Objectives

1.  Build a complete interactive 3D target-shooting arena using OpenGL.
2.  Construct every modeled object from a **unit cube as the fundamental
    primitive** wherever practical.
3.  Demonstrate the major 3D transformations repeatedly and visibly:
    -   Translation
    -   Scaling
    -   Rotation
    -   Shearing
4.  Keep object construction modular using separate functions for
    creation, transformation, drawing, animation, and movement.
5.  Implement three distinct game weapons:
    -   Pistol
    -   Shotgun
    -   Assault Rifle
6.  Implement targets at different positions, heights, and ranges.
7.  Display range/distance in **meters**.
8.  Implement different target-pointer/crosshair styles for each weapon.
9.  Provide three fixed camera angles and a free/movable camera.
10. Implement day and night environments.
11. Demonstrate Ambient, Diffuse, and Specular illumination.
12. Use multiple practical light-source types, including directional,
    point, and spot lighting.
13. Store representative point creation and 3D transformation
    calculations in `calc.csv`.
14. Keep the code simple enough to explain during a lab demonstration or
    viva.

------------------------------------------------------------------------

## 3. Core Graphics Rule: Start from a Cube

The main modeling rule is:

> **Start with a unit cube and obtain the required object by
> transforming and combining cube instances.**

A base cube can be defined in local coordinates, for example from
`(-0.5, -0.5, -0.5)` to `(0.5, 0.5, 0.5)`.

Every drawable object should pass through a common transformation
pipeline:

`Local Point -> Scale -> Shear -> Rotate -> Translate -> World Point -> View -> Projection -> Screen`

Even when an object does not visually require a strong shear or
rotation, its object function should still call the transformation
pipeline. Identity/zero-value transforms may be used when appropriate so
that the same educational pipeline is applied consistently.

Recommended model-matrix convention:

`M_model = T × Rz × Ry × Rx × H × S`

For a local homogeneous point:

`p_world = M_model × p_local`

where:

-   `S` = scaling matrix
-   `H` = shearing matrix
-   `Rx, Ry, Rz` = rotation matrices
-   `T` = translation matrix

The exact multiplication order must remain consistent throughout the
project and in `calc.csv`.

------------------------------------------------------------------------

## 4. Required Transformation Functions

Use small reusable functions such as:

-   `Mat4 makeTranslation(float tx, float ty, float tz)`
-   `Mat4 makeScale(float sx, float sy, float sz)`
-   `Mat4 makeRotationX(float angle)`
-   `Mat4 makeRotationY(float angle)`
-   `Mat4 makeRotationZ(float angle)`
-   `Mat4 makeShear(float xy, float xz, float yx, float yz, float zx, float zy)`
-   `Mat4 composeModelMatrix(...)`
-   `Vec4 transformPoint(const Mat4& M, const Vec4& p)`

For implementation simplicity, GLM may be used for matrix/vector
operations, while the mathematical form of representative
transformations is documented in `calc.csv`.

------------------------------------------------------------------------

## 5. Suggested Project Structure

``` text
3D_Target_Shooter/
├── CMakeLists.txt
├── README.md
├── project.md
├── calc.csv
├── shaders/
│   ├── object.vert
│   ├── object.frag
│   ├── lamp.vert
│   └── lamp.frag
├── src/
│   ├── main.cpp
│   ├── Camera.cpp
│   ├── Camera.h
│   ├── Transform.cpp
│   ├── Transform.h
│   ├── Lighting.cpp
│   ├── Lighting.h
│   ├── Arena.cpp
│   ├── Arena.h
│   ├── Weapon.cpp
│   ├── Weapon.h
│   ├── Target.cpp
│   ├── Target.h
│   ├── Projectile.cpp
│   ├── Projectile.h
│   ├── Cargo.cpp
│   ├── Cargo.h
│   ├── Environment.cpp
│   └── Environment.h
└── assets/
    └── textures/
```

A smaller submission may merge files, but the functions should remain
logically separated.

------------------------------------------------------------------------

## 6. Coordinate System and Arena Scale

Use a simple world convention:

-   `+X` = right
-   `+Y` = up
-   `-Z` = forward into the arena
-   Ground level = `Y = 0`
-   **1 world unit = 1 meter**

Suggested fixed arena:

-   Width: **60 m**
-   Length: **100 m**
-   Main wall height: **8 m**

These are game-world dimensions chosen for clear visualization and easy
distance calculation.

Distance to an aimed target:

`distance = length(targetPosition - playerPosition)`

The HUD displays:

`RANGE: 32.4 m`

The same world-unit-to-meter convention must be used everywhere.

------------------------------------------------------------------------

## 7. Arena Design

### 7.1 Ground

Create the ground from a unit cube:

-   Scale it into a large, thin rectangular slab.
-   Translate it slightly below `Y = 0`.
-   Keep shear and rotation functions in the transformation pipeline.
-   Apply a simple ground material.

Suggested object function:

`drawArenaFloor()`

### 7.2 Great-Wall Boundary

The arena boundary should look like a simplified fortified/great wall
rather than four plain thin walls.

Build it from repeated transformed cubes:

-   Main wall segments
-   Thick corner sections
-   Raised top blocks/battlements
-   Continuous boundary on all four sides, with no gate or entrance opening
-   Optional watch-platform shapes

Suggested functions:

-   `drawBoundaryWall()`
-   `drawWallSegment(position, scale, rotation, shear)`
-   `drawBattlement(...)`

The wall must visually establish the fixed playable boundary.

### 7.3 Cargo Boxes

Cargo boxes are cube-based objects distributed at predetermined
pseudo-random positions.

Requirements:

-   Several stacks with different heights
-   Some single crates
-   Some two- or three-level stacks
-   Random-looking but reproducible placement by using a fixed random
    seed
-   Do not block every target
-   Keep safe open paths for camera/player movement

Suggested functions:

-   `generateCargoLayout(seed)`
-   `drawCargoBox(transform)`
-   `drawCargoStack(...)`

------------------------------------------------------------------------

## 8. Three Weapon Types

All weapons are **stylized game models assembled from transformed
cubes**. The goal is visual distinction and gameplay feedback, not
replication of internal real-world mechanisms.

### 8.1 Pistol

**Visual character** - Compact body - Short barrel block - Grip - Small
front sight - Small projectile visualization

**Game behavior** - Small projectile visual - Shorter effective game
range - Moderate single-shot damage - Low visual spread - Fast and
simple aiming

**Crosshair** - Small center dot with four short marks

**Suggested game range** - About **25 m**

**Functions** - `createPistol()` - `drawPistol()` - `firePistol()` -
`updatePistolProjectile()`

### 8.2 Shotgun

**Visual character** - Wider body - Longer/wider front section - Stock -
Fore-end - Larger muzzle area

**Game behavior** - Short effective game range - Multiple visual
pellets/rays per shot - Spread increases with distance - High
close-range aggregate game damage - Rapid effectiveness reduction
outside the short-range zone

**Crosshair** - Large open circle with spread markers

**Suggested game range** - About **18 m**

**Functions** - `createShotgun()` - `drawShotgun()` - `fireShotgun()` -
`updateShotgunPellets()`

### 8.3 Assault Rifle

**Visual character** - Longer body - Long barrel section - Stock -
Magazine-like stylized block - Front/rear sight blocks

**Game behavior** - Large projectile visual relative to the pistol -
Highest effective game range - Good game damage - Low-to-medium visual
spread - Supports repeated firing animation

**Crosshair** - Compact cross/chevron-style pointer

**Suggested game range** - About **70 m**

**Functions** - `createAssaultRifle()` - `drawAssaultRifle()` -
`fireAssaultRifle()` - `updateRifleProjectile()`

### 8.4 Weapon Switching

Recommended controls:

-   `1` = Pistol
-   `2` = Shotgun
-   `3` = Assault Rifle

When the weapon changes, also change:

-   Visible weapon model
-   Crosshair
-   Displayed effective range
-   Projectile visualization
-   Spread behavior
-   HUD weapon name

Suggested functions:

-   `switchWeapon(WeaponType type)`
-   `drawCurrentWeapon()`
-   `drawCurrentCrosshair()`

------------------------------------------------------------------------

## 9. Projectile and Hit Visualization

The original concept uses bullets/projectiles that travel from the gun
toward targets. Preserve that visible animation.

A simple implementation:

1.  Create projectile at muzzle/world start point.
2.  Store normalized direction.
3.  Update its position with time.
4.  Test against a simple target bounding volume.
5.  Remove projectile after hit, maximum range, or lifetime.
6.  Trigger a visual hit effect.

For the shotgun, create several **game pellet trajectories** with small
angular offsets to form a visible spread.

Hit effects may include:

-   Target color change
-   Short backward translation
-   Small rotation
-   Temporary scale pulse
-   Temporary disappearance and reset

Suggested functions:

-   `spawnProjectile()`
-   `updateProjectiles(deltaTime)`
-   `checkTargetHit()`
-   `applyHitEffect()`

------------------------------------------------------------------------

## 10. Targets

The final gameplay requirements above supersede the original cube-ring suggestion:
use smaller round targets with six front-only scoring regions and faster movement.

Targets should preserve the proposal's idea of multiple targets at
different distances and heights.

Use cube-based approximations for:

-   Target plate
-   Colored/contrasting ring layers
-   Central hit zone
-   Stand
-   Base

Targets may:

-   Move horizontally
-   Move vertically
-   Rotate around Y
-   Reset after a hit

Functions:

-   `createTarget()`
-   `drawTarget()`
-   `updateTargetMovement()`
-   `rotateTarget()`
-   `resetTarget()`

The target's model matrix should also use Scale + Shear + Rotation +
Translation.

------------------------------------------------------------------------

## 11. Target Pointer / Crosshair System

The target pointer must visibly change with the selected weapon.

### Pistol Pointer

-   Small central dot
-   Four short lines
-   Minimal screen obstruction

### Shotgun Pointer

-   Larger circle
-   Multiple outward ticks
-   Circle can expand slightly to visualize spread

### Assault Rifle Pointer

-   Compact cross or chevron
-   Narrower than shotgun
-   Suitable for longer-range aiming

HUD should show:

-   Weapon name
-   Current target distance
-   Effective game range
-   Day/Night state
-   Camera mode

Example:

``` text
WEAPON: Assault Rifle
TARGET: 46.8 m
EFFECTIVE RANGE: 70 m
MODE: DAY
CAMERA: PLAYER
```

------------------------------------------------------------------------

## 12. Camera System

Provide at least **three distinct fixed camera angles**, plus a
movable/free camera.

### Camera 1 --- Player / First-Person

Positioned near the weapon and aimed forward.

### Camera 2 --- Elevated Arena View

A high oblique view showing the player zone, targets, walls, and cargo.

### Camera 3 --- Side / Tactical View

A side angle useful for observing projectile motion, target movement,
and range.

### Free Camera

Use keyboard and mouse:

-   `W/S` = forward/backward
-   `A/D` = left/right
-   `Q/E` = down/up
-   Mouse = yaw/pitch
-   Optional Shift = faster movement

Functions:

-   `setCameraPreset(int id)`
-   `updateFreeCamera(deltaTime)`
-   `processMouseMovement(...)`
-   `getViewMatrix()`

------------------------------------------------------------------------

## 13. Day and Night Modes

### Day Mode

Environment: - Bright sky - Visible sun - Directional sunlight -
Moderate ambient illumination - Arena lamps may remain off - Strong but
controlled diffuse response - Specular highlights on suitable surfaces

### Night Mode

Environment: - Dark sky - Sun hidden or replaced by a simple moon-like
visual if desired - Low ambient level - Arena point lights enabled -
Spotlights can illuminate target lanes - Weapon/metal-like surfaces show
visible specular response - Target areas remain readable

Recommended key:

-   `N` = toggle Day/Night

Functions:

-   `setDayMode()`
-   `setNightMode()`
-   `toggleDayNight()`
-   `drawSky()`
-   `drawSun()`
-   `drawArenaLights()`

------------------------------------------------------------------------

## 14. Lighting and Shading

The original proposal explicitly requires ambient, diffuse, and specular
lighting and directional, point, and spot light sources. The expanded
project should demonstrate all of them clearly.

### 14.1 Ambient Illumination

Ambient illumination approximates background light reaching objects
indirectly.

Conceptual form:

`I_ambient = k_a × I_a`

Use it on **all rendered 3D objects** so that surfaces facing away from
direct light are not completely black.

**Where used** - Ground - Great wall - Cargo boxes - Targets - Weapons -
Target stands - General arena geometry

**Day:** medium ambient level.\
**Night:** low ambient level.

### 14.2 Diffuse Reflection

Diffuse reflection depends on the angle between the surface normal and
the light direction.

Conceptual form:

`I_diffuse = k_d × I_L × max(dot(N, L), 0)`

**Where it is most visible** - Wall faces - Ground - Cargo boxes -
Target plates - Weapon body faces

It should be the main source of shape readability.

### 14.3 Specular Reflection

Specular reflection creates highlights depending on light direction,
surface normal, viewer direction, and material shininess.

A common Phong-style form:

`I_specular = k_s × I_L × max(dot(R, V), 0)^n`

**Where it should be emphasized** - Weapon surfaces - Target
center/rim - Arena lamps - Selected smoother decorative surfaces

Keep rough wall/ground materials at low specular strength.

### 14.4 Directional Light

Use the **sun** as the main directional light during day mode.

Characteristics: - Same direction for the whole arena - No positional
attenuation - Main source of daytime diffuse/specular illumination

### 14.5 Point Lights

Use point lights for arena lamps at night.

Place them at: - Corners - Side walls - Target-lane areas

Use attenuation so illumination decreases with distance.

### 14.6 Spotlights

Use spotlights for focused night lighting.

Possible positions: - Above selected target lanes - Near the player
area - On wall-mounted fixtures

Spotlight parameters: - Position - Direction - Inner cutoff - Outer
cutoff - Attenuation

### 14.7 Shading Method

Use a simple **Phong or Blinn-Phong lighting model** in GLSL.

Recommended fragment calculation:

`Final = Ambient + Diffuse + Specular`

This is simple, visually effective, and directly demonstrates the three
required illumination components.

------------------------------------------------------------------------

## 15. Object-by-Object Transformation Plan

  -----------------------------------------------------------------------------------------------------
  Object       Base         Scale             Shear             Rotation    Translation   Animation
               Primitive                                                                  
  ------------ ------------ ----------------- ----------------- ----------- ------------- -------------
  Ground       Cube         Very wide/thin    Identity or       Usually 0°  Arena center  Static
                                              subtle                                      

  Wall segment Cube         Long/tall/thick   Identity/subtle   Per side    Boundary      Static

  Battlement   Cube         Small block       Identity          Per wall    Wall top      Static

  Cargo box    Cube         Box dimensions    Small/identity    Random Y    Random stack  Static

  Pistol body  Cube         Compact           Small/identity    Aim         Player        Weapon aim
                                                                pitch/yaw                 

  Shotgun      Cube         Long/wide         Small/identity    Aim         Player        Weapon aim
  parts                                                         pitch/yaw                 

  Rifle parts  Cube         Long/slim         Small/identity    Aim         Player        Weapon aim
                                                                pitch/yaw                 

  Target plate Cube         Thin plate        Identity          Y rotation  Target point  Move/rotate

  Target stand Cube         Tall/thin         Identity          0°          Under target  Follows
                                                                                          target

  Projectile   Cube         Very small        Identity          Align       Moving        Forward
                                                                direction   position      motion

  Lamp pole    Cube         Tall/thin         Identity          0°          Arena sides   Static

  Lamp head    Cube         Small             Optional          Aim         Pole top      Static
                                                                direction                 

  Sun visual   Cube-based   Large             Identity          Scene orbit Sky position  Optional
               assembly                                         optional                  
  -----------------------------------------------------------------------------------------------------

Every object creation function should call the shared transformation
routine.

------------------------------------------------------------------------

## 16. Point Mapping and `calc.csv`

`calc.csv` is the project's calculation/reference sheet. It stores
representative calculations for object construction and transformation.

Each row records:

-   Object
-   Component
-   Local point
-   Scale
-   Shear
-   Rotation
-   Translation
-   Matrix order
-   Operation description
-   Resulting world point
-   Purpose

The file is not intended to list every GPU vertex for every frame.
Instead, it provides enough representative mappings to demonstrate
mathematically how each major object is generated from the base cube and
transformed into world space.

### Required Matrix Definitions

#### Translation

``` text
T =
[1 0 0 tx]
[0 1 0 ty]
[0 0 1 tz]
[0 0 0  1]
```

#### Scaling

``` text
S =
[sx  0  0 0]
[ 0 sy  0 0]
[ 0  0 sz 0]
[ 0  0  0 1]
```

#### Rotation about Y

``` text
Ry =
[ cosθ 0 sinθ 0]
[    0 1    0 0]
[-sinθ 0 cosθ 0]
[    0 0    0 1]
```

#### Example Shear

``` text
H =
[1 hxy hxz 0]
[hyx 1 hyz 0]
[hzx hzy 1 0]
[0   0   0  1]
```

The project should print/debug matrices when useful, but `calc.csv`
serves as the permanent human-readable calculation record.

------------------------------------------------------------------------

## 17. Simple Object Architecture

Suggested classes/structures:

``` text
Transform
    position
    rotation
    scale
    shear

RenderableObject
    Transform transform
    Material material
    draw()

Weapon
    type
    visualRange
    damageValue
    spreadValue
    fire()
    draw()
    drawCrosshair()

Target
    position
    movementType
    movementSpeed
    hitState
    update()
    draw()

Projectile
    position
    direction
    speed
    maxDistance
    update()
    draw()

Camera
    position
    yaw
    pitch
    mode
    update()
```

Keep gameplay logic small. The graphics concepts are the priority.

------------------------------------------------------------------------

## 18. Recommended Major Functions

``` cpp
// Geometry / transforms
drawUnitCube();
composeModelMatrix();
applyTransform();
drawTransformedCube();

// Arena
createArena();
drawArena();
drawArenaFloor();
drawBoundaryWall();
drawCargoObjects();

// Weapons
createPistol();
createShotgun();
createAssaultRifle();
drawCurrentWeapon();
switchWeapon();
fireCurrentWeapon();

// Projectile
spawnProjectile();
updateProjectiles();
drawProjectiles();

// Target
createTargets();
updateTargets();
drawTargets();
checkTargetHit();

// Camera
setCameraPreset();
updateFreeCamera();
getViewMatrix();

// Lighting
setupDayLighting();
setupNightLighting();
uploadLightsToShader();
drawLightSources();

// Environment
drawSky();
drawSun();
toggleDayNight();

// UI
drawCrosshair();
drawHUD();
calculateTargetRange();
```

------------------------------------------------------------------------

## 19. Controls

  Input                Action
  -------------------- ------------------
  W/A/S/D              Move free camera
  Q/E                  Camera down/up
  Mouse                Look/aim
  Left Mouse / Space   Fire
  1                    Pistol
  2                    Shotgun
  3                    Assault Rifle
  F1                   Player camera
  F2                   Elevated camera
  F3                   Side camera
  F4                   Free camera
  N                    Day/Night toggle
  M                    Sound on/off toggle
  R                    Reset targets
  Esc                  Pause / resume menu
  Tab                  Release / capture mouse for HUD buttons
  F5                   Save current calculation snapshot

The application starts on a menu with brief instructions, Start Session,
View Controls, and Exit buttons. The pause menu provides Resume, View
Controls, Main Menu, and Exit. Gameplay pauses while menus are open.
Player view uses WASD to walk and mouse movement to aim; free-camera mode
uses WASD/Q/E to fly independently. Firing is enabled in player view.

------------------------------------------------------------------------

## 20. Gameplay / Visualization Flow

1.  Start in the fixed arena.
2.  Load cube geometry and shaders.
3.  Generate walls, cargo stacks, lights, targets, and weapon.
4.  Set day lighting by default.
5.  Player selects a weapon.
6.  Weapon-specific crosshair and effective range appear.
7.  Camera can use presets or free movement.
8.  Targets move/rotate.
9.  Player aims and fires.
10. Projectile visualization travels toward the scene.
11. Range is continuously displayed in meters.
12. Collision test determines hit.
13. Target performs a simple visual hit reaction.
14. Night mode activates arena lights and changes global illumination.
15. All major object types follow the common 3D transformation pipeline.

------------------------------------------------------------------------

## 21. Implementation Priorities

### Phase 1 --- Basic Scene

-   OpenGL window
-   Shader
-   Unit cube
-   Camera
-   Floor
-   Boundary wall

### Phase 2 --- Transformation Framework

-   Translation
-   Scaling
-   Rotation
-   Shearing
-   Common model-matrix function
-   Initial `calc.csv`

### Phase 3 --- Targets and Cargo

-   Target creation
-   Target movement/rotation
-   Cargo randomization
-   Range calculation

### Phase 4 --- Weapons

-   Three cube-based weapon models
-   Weapon switching
-   Three crosshairs
-   Projectile visualization
-   Shotgun spread visualization

### Phase 5 --- Lighting

-   Ambient
-   Diffuse
-   Specular
-   Directional sun
-   Point arena lamps
-   Spot target lights

### Phase 6 --- Day/Night and Final UI

-   Sky
-   Sun
-   Day/night toggle
-   HUD
-   Camera presets
-   Final `calc.csv` verification

------------------------------------------------------------------------

## 22. What Should Be Demonstrated in the Final Presentation

The demonstration should clearly show:

1.  Arena dimensions and boundary.
2.  Cube-based object construction.
3.  Translation.
4.  Scaling.
5.  Shearing.
6.  Rotation.
7.  Matrix composition.
8.  Three weapon models.
9.  Weapon switching.
10. Three different crosshairs.
11. Different visual range behavior.
12. Range in meters.
13. Target movement and rotation.
14. Projectile animation.
15. Shotgun spread visualization.
16. Hit effect.
17. Cargo-box stacks.
18. Three fixed cameras.
19. Movable/free camera.
20. Day mode.
21. Night mode.
22. Sun/directional light.
23. Arena point lights.
24. Spotlights.
25. Ambient illumination.
26. Diffuse reflection.
27. Specular reflection.
28. `calc.csv` point/matrix calculations.

------------------------------------------------------------------------

## 23. Master Prompt for Generating the Complete Project

``` text
Create a complete but academically simple C++ OpenGL project named "3D Target Shooter" based on the following specification.

The original concept is a 3D shooting arena with movable/rotating targets, visible projectiles, hit effects, lighting, multiple camera views, camera movement, and keyboard/mouse interaction.

CORE GRAPHICS REQUIREMENT:
Use a unit cube as the fundamental modeling primitive wherever practical. Every major object must be constructed by combining transformed cubes. Create reusable transformation functions for translation, scaling, shearing, rotation X/Y/Z, and model-matrix composition. Every object creation/drawing routine must use the common transformation pipeline, even when a particular transform is identity/zero. Use a consistent matrix order such as:
M_model = T * Rz * Ry * Rx * H * S.
Keep separate functions/classes for object creation, drawing, animation, movement, lighting, camera control, weapons, targets, projectiles, environment, and UI.

ARENA:
Create a fixed 60 m × 100 m arena where 1 OpenGL world unit represents 1 meter. Surround it with a thick fortified/great-wall-style boundary made from repeated cubes and battlement blocks. Add pseudo-random but reproducible cargo-box stacks around the arena using a fixed random seed. Keep the scene readable and do not block all target lanes. Show a visible sky and a sun.

WEAPONS:
Implement three stylized fictional game weapons built from transformed cubes:
1. Pistol — compact model, small projectile visualization, shorter effective game range around 25 m, moderate single-shot game damage, low spread.
2. Shotgun — wider/longer model, short effective game range around 18 m, several visual pellet trajectories that spread outward, high aggregate close-range game damage.
3. Assault Rifle — longer model, larger projectile visualization than the pistol, highest effective game range around 70 m, good game damage, low-to-medium spread, repeated-fire animation if simple to implement.
These are game-behavior parameters only; do not model real-world internal weapon mechanisms.

Allow switching with keys 1, 2, and 3. Change the weapon model, HUD information, projectile visualization, and target pointer when switching.

CROSSHAIRS:
Pistol = small dot with four short marks.
Shotgun = large open circular spread pointer.
Assault Rifle = compact cross/chevron pointer.
Show current target distance and effective game range in meters.

TARGETS:
Create several targets at different positions, heights, and distances. Targets should be built from cube-based thin plates/rings/stands. Some move horizontally, some vertically, and some rotate. Add simple hit reactions such as color change, short translation, rotation, scale pulse, or temporary disappearance/reset.

PROJECTILES:
Create visible game projectiles. Spawn them at the weapon, move them forward using delta time, remove them after a hit or maximum game range, and use simple bounding-volume collision. For shotgun, create multiple visual pellet trajectories with small angular offsets to demonstrate spread.

CAMERA:
Implement:
F1 = player/first-person camera,
F2 = elevated arena camera,
F3 = side/tactical camera,
F4 = free camera.
Free camera controls: W/A/S/D movement, Q/E vertical movement, mouse yaw/pitch. Keep the camera implementation simple and explainable.

DAY/NIGHT:
Add two modes toggled with N.
Day: bright sky, visible sun, directional sunlight, medium ambient light, arena lamps off.
Night: dark sky, low ambient light, arena point lights on, target-lane spotlights on.
The environment must remain clearly visible in both modes.

LIGHTING:
Use a simple Phong or Blinn-Phong shader.
Explicitly calculate and demonstrate:
1. Ambient Illumination: Ia * ka — used on all scene objects.
2. Diffuse Reflection: kd * IL * max(dot(N,L),0) — strongly visible on walls, ground, cargo, targets, and weapon faces.
3. Specular Reflection: ks * IL * max(dot(R,V),0)^n or equivalent Blinn-Phong form — emphasized on weapons, target rims/centers, and selected smooth surfaces.
Use:
- Directional light for the daytime sun.
- Point lights for night arena lamps.
- Spotlights for selected target lanes/player area.
Use lower specular strength on rough walls and ground.

CODE ORGANIZATION:
Create clear functions such as drawUnitCube, composeModelMatrix, drawTransformedCube, createArena, drawBoundaryWall, generateCargoLayout, createPistol, createShotgun, createAssaultRifle, switchWeapon, fireCurrentWeapon, spawnProjectile, updateProjectiles, createTargets, updateTargets, checkTargetHit, setCameraPreset, updateFreeCamera, setupDayLighting, setupNightLighting, drawSky, drawSun, drawCrosshair, drawHUD, and calculateTargetRange.

Keep implementation simple. Prefer understandable code over complex engines or advanced physics.

CALCULATION FILE:
Create a calc.csv file that documents representative point creation and transformation calculations for every major object category. Columns should include:
Object_ID, Object_Type, Component, Primitive, Local_Point, Scale, Shear, Rotation_X_deg, Rotation_Y_deg, Rotation_Z_deg, Translation, Matrix_Order, Matrix_or_Operation, Result_World_Point, Lighting_or_Use, Notes.
Include representative rows for ground, wall, battlement, cargo box, pistol, shotgun, assault rifle, target plate, target stand, projectile, lamp pole, lamp head, and sun visual. Show actual example point mappings from the unit cube through scaling/shearing/rotation/translation. Keep the same transform convention in code and CSV.

DELIVERABLES:
- Complete compilable C++ OpenGL source.
- CMakeLists.txt.
- Vertex and fragment shaders.
- project.md.
- calc.csv.
- README.md with build/run instructions and controls.
- Clean folder structure.
- Comments explaining transformations, camera, lighting, target movement, projectile update, collision, day/night mode, and weapon switching.

The final result should look like a complete 3D graphics course project, but the implementation should remain simple enough for a student to understand, present, and explain line by line.
```

------------------------------------------------------------------------

## 24. Final Project Identity

**Title:** 3D Target Shooter\
**Main academic focus:** 3D modeling from primitive geometry, point
mapping, matrix transformations, interactive animation, camera
transformation, illumination, shading, and real-time OpenGL rendering.\
**Extended visual focus:** three game weapons, weapon-specific aiming
UI, range visualization, moving targets, fortified arena, randomized
cargo objects, day/night environment, and multiple light sources.

---

# 25. FINAL GAMEPLAY EXPANSION — MODES, LEVELS, SCORING, NPCs, LEADERBOARD

This section supersedes conflicting gameplay rules above. Existing graphics, cube-modeling, transformations, lighting, weapons, camera, arena, collision, day/night, sound, and `calc.csv` requirements remain unless explicitly changed below.

## 25.1 Global Modeling and Visual Rule

- Use the **unit cube as the only 3D modeling primitive** for all newly added gameplay objects, including targets, birds, humans, cargo, UI-world decorations, effects, weapons, walls, and environmental props.
- Circular/rounded-looking objects must be visually approximated from many transformed cubes rather than introducing sphere/cylinder primitives.
- Every object must still use the common transformation pipeline: Scale -> Shear -> Rotate -> Translate.
- Keep separate creation, drawing, update/movement, collision, and animation functions.
- Objects should look as close to realistic as practical using cube assemblies, good proportions, believable materials, lighting, shadows/highlights, and suitable colors.
- Add visual variety: avoid making all cargo, targets, birds, humans, movement patterns, or decorative objects look identical.

## 25.2 Main Game Modes

The main menu must contain four clearly separated mode cards/buttons:

1. **Free Mode**
2. **Challenge Mode**
3. **Developer Mode**
4. **Bird's-Eye View**

Use polished animated transitions when entering/leaving a mode. Menus should use smooth fades/slides/scales, animated headings, mode cards, hover feedback, and clear back navigation.

### 25.2.1 Free Mode

Free Mode uses the **Level 7 arena/target layout and difficulty setup** as its base.

Rules:

- Session duration: **3 minutes (180 seconds)**.
- A large but non-obstructive countdown timer must remain visible on the HUD.
- Use the same 12 target positions/setup family as Level 7.
- When a target is destroyed, it respawns after **30 seconds**.
- Respawn should have a visible warning/spawn animation before the target becomes active.
- Birds/humans and Level 7-style movement remain active.
- Player may continue scoring until the timer reaches zero.
- Negative scores are allowed.
- When time expires, stop gameplay input, play a polished end-of-session animation, then show statistics and the Free Mode leaderboard.
- Free Mode leaderboard records are separate from Challenge Mode records.

### 25.2.2 Challenge Mode

Challenge Mode contains **7 sequential levels**.

- Start at Level 1.
- The player can proceed to the next level **only after completing the current level's rule**.
- Targets **do not respawn** during Challenge Mode levels.
- Score and elapsed time accumulate across completed levels.
- After each completed level, update persistent leaderboard/progress data.
- Show a short animated level-complete sequence between levels.
- The next level begins only after the completion summary/transition.
- Level 7 completion finishes the full Challenge run.
- After all 7 levels are completed, show a strong final victory animation, overall statistics, the player's rank, and a scrollable Challenge Mode leaderboard.

### 25.2.3 Developer Mode

Developer Mode is a testing/debug mode.

- Show **7 clickable level cards**.
- Clicking a card immediately loads that level configuration without requiring previous levels.
- Developer Mode should expose useful debug information such as current level, target IDs, target state, NPC counts, player position, target movement type, and FPS if simple to implement.
- Allow reset/reload of the selected level.
- Keep normal weapon switching, camera, lighting, collision, and gameplay systems available.
- Developer Mode results do **not** need to affect competitive leaderboards unless explicitly enabled with a debug option; default is no leaderboard update.

### 25.2.4 Bird's-Eye View

Bird's-Eye View loads the arena using the **Level 7 positions/layout**.

- Start with a high top-down/oblique camera that shows the arena.
- The player can pan/zoom/orbit as appropriate.
- Clicking a valid point on the arena ground moves/teleports the observation camera to that world position.
- After moving to the clicked point, mouse movement must allow looking around from that position using yaw/pitch.
- The camera must respect arena boundaries and should not be placed inside cargo/walls.
- Provide an easy key/button to return to the overhead view.
- Bird's-Eye View is primarily for inspecting the complete arena, target placement, cargo density, NPC movement, and level design.

## 25.3 Arena and Dense Cargo System

The arena keeps the existing fixed dimensions and fortified boundary, but cargo density is greatly increased.

General rules:

- There should be **many cargo stacks**, enough that the player cannot see all targets from one location.
- The player must move around the arena in movement-enabled levels to obtain clear lines of sight.
- Cargo must never permanently/full-body block a required target from every reasonable player route/view.
- Always preserve navigable paths between important areas.
- Player collision: player **cannot pass through cargo**.
- Projectile collision: bullets/pellets **cannot pass through cargo**; cargo blocks shots.
- Cargo layout should be pseudo-random but reproducible from a seed for a given session/level when needed.
- Cargo quantity, orientation, grouping, and stack height should have strong visual variety.

### Cargo dimensions and stacking

- A single cargo box should be approximately **one-third of normal human height**.
- Use a configurable human reference height in world units; derive crate size from it.
- Stack height should vary roughly from **1 to 5 boxes**, with a normal-distribution-like tendency toward middle values rather than equal probability for every height.
- Most cargo groups should form horizontal or vertical runs of roughly **3 to 7 boxes**.
- Some formations can extend in both directions simultaneously, creating L, T, cross, corner, block, shelf, or compact warehouse-like arrangements.
- Randomly rotate suitable stacks by 90-degree increments and occasionally use small safe visual offsets/shears while keeping collision predictable.
- Use realistic cargo colors/material variation while maintaining readability under day and night lighting.

Suggested generation functions:

```cpp
generateDenseCargoLayout(level, seed);
generateCargoCluster();
generateCargoRun();
generateCargoStack();
validateCargoAgainstTargets();
validateNavigationPaths();
checkCargoPlayerCollision();
checkCargoProjectileCollision();
```

## 25.4 Challenge Levels

### Level 1 — Basic Static Shooting

- Player movement: **disabled**.
- Targets: **3**.
- All 3 targets are static.
- All targets must be inside the effective range of at least one available weapon and clearly shootable from the fixed player position.
- Cargo must not block the required shooting lanes.
- Completion rule: destroy all 3 targets.

### Level 2 — First Moving Target

- Player movement: **disabled**.
- Targets: **3**.
- Targets are positioned slightly farther away than Level 1 but remain targetable by a suitable weapon.
- Two targets may remain static.
- **1 target moves along the X axis**.
- Completion rule: destroy all 3 targets.

### Level 3 — Multi-Axis Introduction

- Player movement: **disabled**.
- Targets: **4**.
- Every target moves.
- Each target receives a movement pattern using one of the X, Y, or Z axes, with varied speed/amplitude/direction.
- All targets must remain within a usable weapon range and must not be blocked by cargo from the fixed player location.
- Completion rule: destroy all 4 targets.

### Level 4 — Player Movement

- Player movement: **enabled**.
- Targets: **6**.
- Targets use varied X/Y/Z movement patterns.
- Some targets rotate/spin while moving.
- Movement speeds must vary between targets.
- Dense cargo begins to matter strongly; some targets require player repositioning for a clean line of sight.
- Completion rule: destroy all 6 targets.

### Level 5 — Birds and Compound Movement

- Player movement: enabled.
- Targets: **8**.
- All targets move using randomized X/Y/Z-based movement.
- **2 targets** use compound two-axis movement: one may use X+Z and another Y+Z (or equivalent varied assignment) and both also spin.
- Of the remaining 6 targets, at least **2 additional targets spin** while moving.
- Maintain varied speeds, amplitudes, phases, directions, and spin rates.
- **6-8 birds** should be active in the arena at all times for this level.
- Birds move randomly in **all three axes (X/Y/Z)** but remain within a bounded neighborhood of their associated/nearby target area.
- Birds must have varied flight patterns; do not use one repeated loop for all birds.
- Shooting a bird: **-100 points**.
- A bird hit triggers a noticeable **red danger/penalty animation** plus a penalty sound.
- Point gains/deductions must appear as animated floating HUD/world feedback, e.g. `+100`, `+150 BULLSEYE`, `-100 BIRD`.
- Completion rule: destroy all 8 targets. Bird hits do not prevent completion.

### Level 6 — Faster Three-Axis Targets

- Targets: **10**.
- The first 8 targets retain Level 5-style movement variety.
- Add **2 new targets** that:
  - move simultaneously along all three axes,
  - spin while moving,
  - move faster than the earlier target classes.
- Bird density increases compared with Level 5.
- Maintain birds around target areas; additionally ensure **at least 3 birds are actively flying near target zones** at all times.
- Bird penalty remains **-100**.
- Completion rule: destroy all 10 targets.

### Level 7 — Final Arena

- Targets: **12**.
- Preserve the Level 6 target behaviors.
- Add **2 final advanced targets** with all-axis movement, spinning, and a noticeably higher movement speed than the Level 6 advanced targets.
- These should feel very fast but remain visually trackable and technically hittable.
- The final arena uses the densest cargo arrangement while preserving fair routes and target accessibility.
- Birds and humans become major penalty obstacles.
- Maintain approximately **4 birds and 3 humans moving around/near each active target zone**, subject to performance-aware pooling/LOD if necessary while preserving the intended visual density.
- Birds: random movement in X/Y/Z, bounded around target zones.
- Humans: random movement primarily on the ground plane using **X and Z** world movement (horizontal ground movement; Y remains ground-height except small animation offsets). They must stay near their associated target zone and never wander far away.
- Use varied speeds, headings, pause durations, route radii, and avoidance offsets so NPC movement does not look synchronized.
- Shooting bird: **-100 points**.
- Shooting human: **-200 points**.
- Negative total score is allowed.
- Completion rule: destroy all 12 targets.
- After completion: freeze gameplay, play a polished all-levels-complete/victory animation, then show overall statistics, rank, and the scrollable leaderboard.

## 25.5 Target Movement Variety

Movement must not be identical between targets. Use parameterized patterns such as:

- X oscillation
- Y oscillation
- Z oscillation
- X+Z ellipse/figure-eight-like path
- Y+Z compound motion
- X+Y compound motion
- X+Y+Z bounded compound motion
- Spin + single-axis motion
- Spin + two-axis motion
- Spin + three-axis motion

Each target can vary:

- speed
- amplitude
- phase
- direction
- spin speed
- spin direction
- movement bounds
- pause/turn timing where appropriate

Movement must remain inside valid arena/target-zone bounds and avoid impossible clipping through walls/cargo.

## 25.6 Bird System

Birds must be stylized but believable **cube-only assemblies**.

Suggested cube components:

- body
- head
- beak
- left/right wings
- tail

Animation:

- wing flap via rotation
- slight body bob
- random bounded X/Y/Z travel
- random heading changes
- variable speed and altitude
- target-zone attraction/bounds so birds never fly too far away

Bird collision must be separate from target collision. Bird hits never count as target destruction.

## 25.7 Human NPC System

Humans must also be constructed only from transformed cubes.

Suggested parts:

- torso
- head
- upper/lower arms
- upper/lower legs
- simple clothing blocks/details

Animation:

- walking cycle using limb rotations
- random ground-plane direction changes
- varied speed
- short pauses/turns
- bounded roaming around target zones
- simple obstacle avoidance so humans do not walk through cargo, walls, or target stands

Humans are penalty NPCs only; they are not enemies and should never attack the player.

## 25.8 Scoring Rules

Base target score:

- Normal target destruction: **+100**.
- Bullseye / one-shot target destruction: **+150 total** instead of +100, i.e. a **+50 bullseye bonus**.
- Bird hit/kill: **-100**.
- Human hit/kill: **-200**.
- Score may become negative.

For leaderboard/stat tracking, a **bullseye** means the target is destroyed by the player's first valid shot/trigger against that fresh target. Shotgun pellets belonging to one trigger share the same shot ID, consistent with the existing damage rules.

Every score event should create immediate animated feedback:

```text
+100 TARGET
+150 BULLSEYE
-100 BIRD
-200 HUMAN
```

Positive feedback should feel rewarding; penalties should use red danger styling and an alert sound.

## 25.9 Challenge Progress and Accumulation

Track during a Challenge run:

- cumulative score
- cumulative elapsed time
- levels cleared
- targets destroyed
- bullseyes
- bird kills/hits
- human kills/hits
- shots fired
- accuracy if desired

Challenge Mode target count through all levels is cumulative, but each level's targets are fresh level instances.

Leaderboard/progress persistence is updated **only after a Challenge level is successfully completed**. If the player quits/fails before completing the current level, do not commit that incomplete level's new competitive progress.

## 25.10 Leaderboard System

Create and maintain:

```text
leaderboard.csv
```

Required columns:

```text
Name,Mode,BestScore,LevelsCleared,TargetsDestroyed,Bullseyes,BirdKills,HumanKills,BestTimeSeconds,LastUpdated
```

### Primary key

The logical primary key is:

```text
(Name, Mode)
```

Therefore one player can have separate best records for Free Mode and Challenge Mode.

### Update rules

- If `(Name, Mode)` does not exist, create a row.
- If it already exists, compare the new eligible result with the stored best.
- **Higher score is better.**
- If scores are equal, **lower time is better**.
- Replace/update the best competitive record only when the new result ranks better by that rule.
- Keep the statistics associated with that best record together so the row represents one coherent best run/progress snapshot.
- Challenge Mode writes only after completing a level; the stored row can therefore represent the player's best completed progress snapshot.
- Free Mode writes after the 3-minute session ends.

### Sorting

Display leaderboard rows sorted by:

1. BestScore descending
2. BestTimeSeconds ascending

For Challenge Mode UI, also display LevelsCleared prominently so progress is obvious even though the requested competitive sort remains score first, then time.

### Leaderboard display

After the end of each eligible game/session:

- show player's current result
- show whether a personal best was achieved
- show calculated leaderboard rank
- show a scrollable leaderboard table
- show columns for Name, Score, Levels, Targets, Bullseyes, Birds, Humans, Time
- visually highlight the current player's row
- provide Replay / Next Level (when applicable) / Main Menu buttons

Suggested functions:

```cpp
loadLeaderboardCSV();
saveLeaderboardCSV();
findLeaderboardRecord(name, mode);
isBetterRecord(newRun, oldRun);
updateLeaderboardAfterLevel();
updateLeaderboardAfterFreeMode();
sortLeaderboard();
calculatePlayerRank();
drawScrollableLeaderboard();
```

## 25.11 Start, Level-Complete, and End Animations

Improve game presentation with polished but simple OpenGL/ImGui-friendly animation.

### Game/mode start

- fade from menu
- animated mode title
- level number/title for Challenge Mode
- short `3 - 2 - 1 - GO!` sequence
- camera intro sweep if simple enough
- HUD slides/fades into place

### Level complete

- freeze target scoring momentarily
- success flash/banner
- show level score/time summary
- animate progress from current level to next level
- save eligible leaderboard/progress record
- load next level after confirmation/countdown

### Final Challenge completion

- strong victory banner/animation
- celebratory particles made from tiny cubes
- animated summary panels
- total score/time/statistics
- rank reveal
- scrollable leaderboard

### Free Mode ending

- countdown reaches 0
- `TIME!` animation
- freeze scoring
- show session stats
- update Free Mode leaderboard
- show rank and scrollable leaderboard

## 25.12 HUD and Interface Update

The HUD should adapt to mode and level.

Always useful:

- player name
- mode
- current level
- score
- target count remaining
- weapon
- effective range
- aimed distance
- crosshair
- day/night

Challenge Mode:

- cumulative time
- level time
- levels cleared
- current completion rule

Free Mode:

- prominent `03:00 -> 00:00` countdown
- active targets
- respawn timers when useful

Penalty/bonus events:

- animated score popup
- icon/text label
- sound cue
- brief color pulse

Menus should be cleaner and more game-like, with consistent typography, cards, hover states, button animations, and readable statistics.

## 25.13 Collision and Fairness Rules

- Player cannot pass through cargo or arena walls.
- Projectiles cannot pass through cargo or walls.
- NPCs should avoid cargo/walls using simple bounding-box checks and direction changes.
- Required targets must always have at least one achievable line-of-sight opportunity.
- Moving targets should stay within designed bounds.
- Targets/NPCs must not become permanently trapped inside cargo.
- Cargo generation must validate target accessibility after placement.
- When a random cargo layout fails validation, regenerate/reposition the conflicting cluster.

## 25.14 Suggested New Data Structures

```cpp
enum class GameMode {
    FREE_MODE,
    CHALLENGE_MODE,
    DEVELOPER_MODE,
    BIRDS_EYE_MODE
};

struct RunStats {
    std::string playerName;
    GameMode mode;
    int score = 0;
    int levelsCleared = 0;
    int targetsDestroyed = 0;
    int bullseyes = 0;
    int birdKills = 0;
    int humanKills = 0;
    int shotsFired = 0;
    float elapsedSeconds = 0.0f;
};

struct LeaderboardRecord {
    std::string name;
    std::string mode;
    int bestScore;
    int levelsCleared;
    int targetsDestroyed;
    int bullseyes;
    int birdKills;
    int humanKills;
    float bestTimeSeconds;
    std::string lastUpdated;
};

struct BirdNPC {
    Transform transform;
    Vec3 homeTargetPosition;
    Vec3 velocity;
    float roamRadius;
    float flapPhase;
    bool active;
};

struct HumanNPC {
    Transform transform;
    Vec3 homeTargetPosition;
    Vec3 moveDirection;
    float roamRadius;
    float walkPhase;
    bool active;
};
```

## 25.15 Suggested New Functions

```cpp
// Modes
startFreeMode();
startChallengeMode();
startDeveloperMode(int selectedLevel);
startBirdsEyeMode();

// Levels
loadLevel(int level);
configureLevelTargets(int level);
checkLevelCompletion();
completeCurrentLevel();
advanceChallengeLevel();

// Timers / respawn
updateFreeModeCountdown();
scheduleTargetRespawn(targetId, 30.0f);
updateTargetRespawns();

// NPCs
spawnBirdsForLevel();
updateBirds();
drawBirds();
spawnHumansForLevel();
updateHumans();
drawHumans();

// Scoring
awardTargetScore();
awardBullseye();
applyBirdPenalty();
applyHumanPenalty();
spawnScorePopup();

// Cargo
regenerateValidatedCargo();
validateTargetVisibility();
validatePlayerPaths();

// Leaderboard
loadLeaderboardCSV();
updateLeaderboard();
saveLeaderboardCSV();
drawLeaderboardScreen();

// Bird's-eye
enterBirdsEyeOverview();
raycastMouseToArenaGround();
moveObservationCameraToPoint();
returnToBirdsEyeOverview();

// Presentation
playModeIntroAnimation();
playLevelStartAnimation();
playLevelCompleteAnimation();
playFinalVictoryAnimation();
playFreeModeEndAnimation();
```

## 25.16 Updated Project Files

The final project structure must now include at minimum:

```text
3D_Target_Shooter/
├── CMakeLists.txt
├── README.md
├── project.md
├── final-project.md
├── calc.csv
├── calc-init.csv
├── leaderboard.csv
├── shaders/
├── src/
└── assets/
```

`leaderboard.csv` should be created automatically with headers if missing.

Initial header:

```csv
Name,Mode,BestScore,LevelsCleared,TargetsDestroyed,Bullseyes,BirdKills,HumanKills,BestTimeSeconds,LastUpdated
```

## 25.17 Final Acceptance Checklist

The finished project is acceptable only when it demonstrates all of the following together:

- Free Mode with 3-minute countdown.
- Level 7-style target/NPC setup in Free Mode.
- 30-second destroyed-target respawn in Free Mode.
- Seven sequential Challenge levels.
- Level completion required before progression.
- No Challenge target respawn.
- Developer Mode with clickable Level 1-7 cards.
- Bird's-Eye View with overhead inspection and click-to-relocate observation camera.
- Dense randomized cargo that forces movement without making targets impossible.
- Player/cargo collision.
- Projectile/cargo collision.
- Varied 1-5-height cargo stacks and 3-7-box horizontal/vertical formations.
- All new 3D objects modeled only from transformed cubes.
- Varied target movement across X/Y/Z and compound axes.
- Spinning targets at required levels.
- Birds with bounded random 3D movement.
- Humans with bounded random ground movement near targets.
- Bird penalty -100.
- Human penalty -200.
- Target score +100.
- One-shot/bullseye target score +150 total.
- Negative scores supported.
- Animated score gains and penalties.
- Strong game-start, level-complete, Free Mode end, and final victory animations.
- Persistent `leaderboard.csv` using `(Name, Mode)` as the logical primary key.
- Leaderboard updates only on eligible completed progress/session events.
- Best record chosen by higher score, then lower time.
- Leaderboard tracks score, levels cleared, targets destroyed, bullseyes, bird kills, human kills, and time.
- Rank and scrollable leaderboard shown after game/session endings.
- Existing three weapons, weapon-specific crosshairs, meter ranges, lighting, day/night, cameras, sound, and transformation logging remain integrated.
- Visuals use believable proportions, realistic-looking colors/materials, and strong variety while preserving an academically understandable implementation.

## 25.18 Final Implementation Principle

The project should feel like a complete small 3D target-shooter game while still clearly demonstrating the computer-graphics course concepts. Prefer modular, readable C++ and deterministic/simple gameplay systems over complex engines. Visual richness should come from repeated transformed cube assemblies, lighting, animation, movement variety, dense scene composition, UI polish, and carefully parameterized gameplay rather than difficult external modeling or physics systems.

---

# 26. Four-Phase Development Master Plan

The complete project must be implemented in **exactly four major development phases**. The phases are cumulative: every phase starts from the complete working result of the previous phase and extends it without breaking already implemented features.

## 26.1 Non-Negotiable Rule: Every Phase Must Be Playable

At the end of **each phase**, the project must:

- Configure and compile successfully with the project's documented CMake workflow.
- Launch into a usable menu/game screen.
- Provide a playable/testable gameplay loop appropriate to the features implemented so far.
- Preserve all previously completed functionality.
- Have working camera and input controls required by that phase.
- Have visible collision/hit feedback where applicable.
- Generate/update the required transformation CSV automatically from the implemented 3D scene.
- Include updated build/run instructions.
- Never leave the repository in a partially implemented or intentionally non-compiling state.

A phase is **not complete** merely because source files have been created. It is complete only when the application can be built, launched, and the phase features can be demonstrated interactively.

## 26.2 Mandatory Modular, Reusable, and Scalable Architecture

Implementation must favor small reusable systems instead of putting gameplay in `main.cpp` or duplicating code between levels.

Recommended organization:

```text
3D_Target_Shooter/
├── CMakeLists.txt
├── README.md
├── final-project.md
├── calc.csv
├── leaderboard.csv
├── shaders/
├── src/
│   ├── core/
│   │   ├── Application.cpp/.h
│   │   ├── GameState.cpp/.h
│   │   ├── Transform.cpp/.h
│   │   ├── Renderer.cpp/.h
│   │   ├── Collision.cpp/.h
│   │   └── CsvLogger.cpp/.h
│   ├── camera/
│   │   ├── Camera.cpp/.h
│   │   └── BirdEyeCamera.cpp/.h
│   ├── world/
│   │   ├── Arena.cpp/.h
│   │   ├── Cargo.cpp/.h
│   │   ├── Environment.cpp/.h
│   │   └── Lighting.cpp/.h
│   ├── gameplay/
│   │   ├── Weapon.cpp/.h
│   │   ├── Projectile.cpp/.h
│   │   ├── Target.cpp/.h
│   │   ├── Bird.cpp/.h
│   │   ├── Human.cpp/.h
│   │   ├── ScoreSystem.cpp/.h
│   │   ├── Leaderboard.cpp/.h
│   │   ├── GameMode.cpp/.h
│   │   └── LevelManager.cpp/.h
│   ├── levels/
│   │   ├── LevelBase.cpp/.h
│   │   ├── Level1.cpp/.h
│   │   ├── Level2.cpp/.h
│   │   ├── Level3.cpp/.h
│   │   ├── Level4.cpp/.h
│   │   ├── Level5.cpp/.h
│   │   ├── Level6.cpp/.h
│   │   └── Level7.cpp/.h
│   ├── ui/
│   │   ├── Menu.cpp/.h
│   │   ├── HUD.cpp/.h
│   │   ├── Animations.cpp/.h
│   │   └── LeaderboardUI.cpp/.h
│   └── main.cpp
└── assets/
```

The exact file names may be adjusted when necessary, but the architecture must preserve these principles:

1. `main.cpp` only initializes the application and starts the main loop.
2. Common target, projectile, weapon, scoring, collision, movement, rendering, transformation, camera, cargo, NPC, CSV, UI, and lighting behavior must be reusable.
3. Each level should preferably have its **own level file/class** (`Level1` through `Level7`).
4. Level files configure rules and entities; they must **not copy the rendering engine or common gameplay code**.
5. Use a shared `LevelBase`/level interface for lifecycle operations such as `load()`, `start()`, `update()`, `draw()`, `isComplete()`, `getStats()`, and `unload()`.
6. Use configuration/data structures for target movement patterns, speed, spin, NPC counts, spawn bounds, and completion rules whenever possible.
7. Challenge Mode, Developer Mode, and Free Mode must reuse the same Level 7/level systems rather than creating duplicate versions of the arena.
8. Bird's-Eye View must reuse the same world state and rendering pipeline.
9. New features should be addable without rewriting unrelated systems.
10. Avoid giant classes/functions and duplicated constants. Put tunable values in named constants/configuration structures.

## 26.3 Mandatory Transformation CSV in Every Phase

Every phase must automatically create or regenerate `calc.csv` when the game runs. The CSV is not an optional final-phase documentation file; it is a required output throughout development.

The logger must use the **actual transformation values used by the running scene**, not unrelated hand-written example values.

At minimum, use columns such as:

```text
Phase,Object_ID,Object_Type,Component,Primitive,Local_Point,
Scale_X,Scale_Y,Scale_Z,
Shear_XY,Shear_XZ,Shear_YX,Shear_YZ,Shear_ZX,Shear_ZY,
Rotation_X_deg,Rotation_Y_deg,Rotation_Z_deg,
Translation_X,Translation_Y,Translation_Z,
Matrix_Order,Result_World_Point,Parent_or_Group,Purpose,Notes
```

The transformation convention remains consistent with the project:

`M_model = T * Rz * Ry * Rx * H * S`

and:

`p_world = M_model * p_local`

Rules:

- Every major 3D object category implemented in the current phase must have representative transformation records.
- Every object is still constructed from the required cube primitive/assemblies defined by this specification.
- Scale, shear, rotation, and translation pass through the common transformation system.
- Identity transforms are valid when an operation is intentionally zero/neutral.
- The CSV must identify which phase generated the row.
- By Phase 4 it must cover the complete arena, walls, cargo, weapons, projectiles, targets, stands, lamps, environment objects, birds, humans, UI-related 3D scene elements where applicable, and representative Level 1-7 objects.
- Keep an optional `calc-init.csv` only if the existing project already depends on it; `calc.csv` remains the current generated transformation record.

## 26.4 Phase 1 — Graphics Foundation + Playable Core

Goal: establish a stable playable graphics foundation before advanced game modes.

Implement/integrate:

- Existing OpenGL/CMake project foundation.
- Unit-cube primitive and common transformation pipeline.
- Translation, scaling, shearing, and X/Y/Z rotation helpers.
- Reusable rendering/material system.
- Fixed arena, ground, fortified boundary, sky/environment.
- Dense but initially manageable cargo system with collision-ready data.
- Player camera plus required camera presets/free camera.
- Three cube-built game weapons and weapon switching.
- Weapon-specific crosshairs and range HUD in meters.
- Visible projectiles and basic projectile collision.
- Basic static/moving targets sufficient for a playable sandbox.
- Day/night and ambient/diffuse/specular lighting.
- Directional, point, and spot lights.
- Basic sound hooks if already available.
- Automatic Phase 1 `calc.csv` generation.

**Phase 1 playable checkpoint:** the player can enter the arena, move/look as permitted, switch all three weapons, aim, shoot targets, see projectile/hit feedback, use cameras, observe lighting, collide with arena/cargo, and inspect a generated transformation CSV.

## 26.5 Phase 2 — Complete Seven-Level Challenge Gameplay

Goal: implement the complete Level 1-7 progression using reusable level infrastructure.

Implement:

- `LevelBase`/common level interface.
- Separate Level 1 through Level 7 implementation/configuration files.
- `LevelManager` for loading, resetting, completing, and transitioning levels.
- Exact target counts, player movement restrictions, target movement axes, spinning, speed variation, birds, humans, penalties, and completion rules specified earlier in this file.
- Reusable movement-pattern system instead of level-specific duplicated movement code.
- Dense cargo generation appropriate to later levels while preserving navigable paths and preventing full permanent target obstruction.
- Target destruction and no target respawn in Challenge Mode.
- Score accumulation across Challenge levels.
- +100 target score and +50 additional one-shot/bullseye bonus, giving +150 total for a one-shot target.
- Bird -100 and human -200 penalties.
- Negative scores.
- Animated point gain/loss feedback.
- Level completion state and accumulated elapsed time.
- Start/level-complete transitions.
- Automatic expanded Phase 2 `calc.csv` generation including representative objects from all seven levels.

**Phase 2 playable checkpoint:** Challenge Mode can be started at Level 1 and played sequentially through Level 7, with progression allowed only after each level's rule is satisfied. Every level must be independently testable during development and the full seven-level sequence must work.

## 26.6 Phase 3 — Game Modes, Bird's-Eye View, Persistence and Leaderboard

Goal: turn the level system into the complete multi-mode game.

Implement:

- Main mode-selection interface.
- Challenge Mode using the Phase 2 seven-level sequence.
- Free Mode using the Level 7 arena/entity positions and a 3-minute countdown.
- Free Mode target respawn 30 seconds after destruction.
- Developer Mode with clickable Level 1-7 cards and direct level launch.
- Bird's-Eye View of the Level 7 arena.
- Click a valid arena point in Bird's-Eye View to relocate an observation camera there, then use mouse look from that point.
- Correct player/cargo and projectile/cargo collision in all relevant modes.
- `leaderboard.csv` persistence.
- `(Name, Mode)` logical primary key.
- Update a player's row only when the new eligible record is better: higher score first; for equal score, lower time.
- Track Best Score, Levels Cleared, Targets Destroyed, Bullseyes, Bird Kills, Human Kills, and Time.
- Update persistent Challenge progress only after completing a level as specified by the gameplay rules.
- Free Mode/session update behavior according to the completed-session rules in this specification.
- Rank calculation.
- Scrollable leaderboard screen.
- Leaderboard display after appropriate game/session endings.
- Automatic Phase 3 `calc.csv` regeneration.

**Phase 3 playable checkpoint:** the main menu can launch Free, Challenge, and Developer modes; Bird's-Eye inspection works; leaderboard persistence survives restarting the program; all modes remain playable without breaking the Phase 1/2 gameplay systems.

## 26.7 Phase 4 — Final Polish, Animation, Realistic Cube-Based Visuals and Release

Goal: produce the complete submission-ready version without sacrificing code clarity.

Implement/refine:

- Strong animated game start sequence.
- Animated level-start and level-complete feedback.
- High-quality Challenge victory/all-levels-complete sequence.
- Free Mode ending sequence.
- Final statistics summary.
- Rank presentation and scrollable leaderboard after final results.
- Red danger feedback and sound for bird/human penalties as specified.
- Clear +100/+150/-100/-200 animated score feedback.
- Improved target destruction effects.
- Improved weapon, target, cargo, wall, bird, human, arena, lamp, and environment appearance while remaining cube-only.
- Realistic-looking proportions and believable materials/colors using transformed cube assemblies rather than imported 3D models.
- Greater cargo variety and density without making targets impossible to reach.
- Natural-looking random variation in cargo stacks, targets, birds, and humans.
- Final collision, camera, UI, audio, day/night, lighting, performance, and reset-state testing.
- Final modularity/refactoring pass to remove duplicated level/game-mode code.
- Final `calc.csv` generated from the complete scene.
- Final `leaderboard.csv` handling.
- Updated README with exact controls, build steps, game modes, scoring, level rules, architecture summary, and CSV descriptions.
- Clean build from a fresh build directory.

**Phase 4 playable checkpoint:** this is the final complete game. All modes, all seven levels, all cameras, all scoring rules, NPC penalties, cargo collision, leaderboard persistence, animations, lighting, sounds, CSV logging, and UI must work together in a clean build.

---

# 27. Master Prompt for the Complete Four-Phase Project

The following is the **master prompt** to use when an AI coding agent is given both this `final-project.md` and the current/full project folder.

```text
You are extending an existing C++ OpenGL project named "3D Target Shooter".

AUTHORITATIVE REFERENCES:
1. Read the complete attached/provided `final-project.md` before changing code. It is the authoritative functional and graphics specification.
2. Inspect the COMPLETE existing project folder before implementation: source files, headers, CMakeLists.txt, shaders, assets, CSV files, README, existing controls, dependencies, and build scripts.
3. Preserve working existing features unless `final-project.md` explicitly supersedes them.
4. Do not invent a second unrelated project. Extend/refactor the supplied project.

DEVELOPMENT STRATEGY:
Implement the specification in exactly FOUR cumulative major phases:
Phase 1 = Graphics Foundation + Playable Core.
Phase 2 = Complete Seven-Level Challenge Gameplay.
Phase 3 = Free/Challenge/Developer Modes + Bird's-Eye View + Leaderboard Persistence.
Phase 4 = Final Polish + Animations + Visual Refinement + Release Validation.

NON-NEGOTIABLE PLAYABILITY:
At the end of EVERY phase, configure/build the project and leave a runnable, playable application. Never finish a phase with intentionally broken compilation, placeholder-only gameplay, unresolved linker errors, or a main menu that cannot enter gameplay. Test the features added in that phase and regression-test important previous features.

ARCHITECTURE:
Use modular, reusable, scalable C++ code. `main.cpp` must remain small. Separate rendering, transforms, collision, arena, cargo, camera, lighting, weapons, projectiles, targets, birds, humans, scoring, game modes, leaderboard, UI, and CSV logging into logical modules/classes. Prefer separate Level1-Level7 files/classes sharing a common LevelBase/interface and reusable systems. Level files should configure entities/rules; they must not duplicate the engine, target renderer, movement algorithms, scoring logic, or collision code. Free Mode and Developer Mode must reuse the same level/world systems.

GRAPHICS RULE:
Follow the cube-only modeling requirement in `final-project.md`. Build required 3D objects from transformed cubes/assemblies. Every major object must use the common transformation pipeline with scaling, shearing, rotation, and translation. Use the project's consistent model matrix order:
M_model = T * Rz * Ry * Rx * H * S.
Use identity/zero transforms when an operation is not visually needed.

TRANSFORMATION CSV — REQUIRED IN ALL FOUR PHASES:
Every playable phase must automatically generate/regenerate `calc.csv` from representative transformations actually used by that phase's running scene. Do not postpone CSV generation until the final phase and do not fill it with unrelated manual examples. Record object/component, cube primitive/local point, scale, shear, rotations, translation, matrix order, resulting world point, grouping/purpose, and phase. As more object systems are implemented, expand the CSV coverage. Phase 4 must cover representative transformations for the complete game.

GAMEPLAY:
Implement the exact Free Mode, Challenge Mode, Developer Mode, Bird's-Eye View, Level 1-7 rules, target counts and motion patterns, cargo behavior, birds/humans, scoring, bullseye bonus, penalties, countdown/respawn behavior, accumulated statistics, completion rules, animations, leaderboard fields, update rules, and sorting rules defined in `final-project.md`. Treat those details as requirements rather than suggestions.

PERSISTENCE:
Maintain `leaderboard.csv` using (Name, Mode) as the logical primary key. Preserve the best eligible result according to the specification: score has priority and lower time breaks equal-score ties. Do not corrupt existing records when saving.

IMPLEMENTATION QUALITY:
Prefer deterministic and understandable algorithms suitable for a university graphics project. Avoid unnecessary engines/frameworks, overly advanced physics, or code that is difficult to explain. At the same time, make the final interface, lighting, animations, cube-built models, colors/materials, cargo composition, target motion, birds, humans, and overall arena visually polished.

FOR EACH PHASE:
1. State which existing files/systems you inspected.
2. Give a short implementation plan.
3. Implement the phase completely in the supplied project.
4. Add/refactor modules instead of duplicating code.
5. Update CMakeLists.txt for all new source files.
6. Generate/update `calc.csv` support for that phase.
7. Update README/control documentation when behavior changes.
8. Configure and compile a clean build.
9. Fix compilation/link/runtime problems you encounter.
10. Launch/test the playable checkpoint when the environment permits.
11. Report exactly what is playable, what files changed, controls, CSV output, and any genuine environment limitation.
12. Do not begin the next phase until the current phase is in a stable playable state.

Do not remove features merely to make compilation easier. Do not replace complex requirements with text-only placeholders. Reuse existing working implementation where appropriate and refactor carefully when architecture needs improvement.

The final Phase 4 output must be a complete, clean, playable, submission-ready project implementing `final-project.md` while remaining modular and understandable enough to explain in a graphics-course demonstration and viva.
```

---

# 28. Exact Prompt — Phase 1

Use the following prompt with **this `final-project.md` and the complete current project folder attached/provided as references**.

```text
PHASE 1 OF 4 — GRAPHICS FOUNDATION + PLAYABLE CORE

Read the entire provided `final-project.md` first, then inspect the COMPLETE existing 3D Target Shooter project. Treat both as references, with `final-project.md` as the authoritative specification when requirements differ.

Work ONLY on Phase 1 in this run, but design the architecture so Phases 2-4 can be added cleanly later.

Before coding, inspect the current CMakeLists.txt, source/header structure, shaders, controls, arena, camera, targets, weapons, projectiles, lighting, CSV logic, assets, and build scripts. Reuse working code where sensible instead of rebuilding everything blindly.

PHASE 1 REQUIRED IMPLEMENTATION:
- Establish/refactor a modular reusable scalable architecture.
- Keep `main.cpp` small.
- Create reusable modules for application/game state, Transform, Renderer, Collision, CsvLogger, Camera, Arena, Cargo, Environment, Lighting, Weapon, Projectile, Target, HUD/Menu, or equivalent clean separation.
- Preserve the unit-cube fundamental primitive rule.
- Every major object must use the shared Scale + Shear + Rotation + Translation pipeline with M_model = T * Rz * Ry * Rx * H * S.
- Build/preserve the fixed arena, ground, fortified boundary, environment, dense cargo foundations, and collision data.
- Implement/preserve the three cube-built weapons: pistol, shotgun, assault rifle.
- Implement weapon switching, distinct crosshairs, effective range display in meters, visible projectile behavior, target hits, and basic playable target behavior.
- Implement/preserve player camera, camera presets, free camera, mouse look, and appropriate movement.
- Implement player/cargo and projectile/cargo collision foundations.
- Implement/preserve day/night rendering and ambient, diffuse, specular, directional, point, and spot lighting.
- Keep the application visually usable and interactive.

MANDATORY CSV:
The Phase 1 game MUST automatically create/regenerate `calc.csv` using transformation values from the actual running scene. Include representative rows for every major object category currently implemented. Record phase, object/component, cube local point, scale, shear, X/Y/Z rotation, translation, matrix order, resulting world point, purpose, and notes. Do not use a fake unrelated CSV. Keep the transform order consistent with the code.

PLAYABLE CHECKPOINT:
At the end of this phase I must be able to build and launch the game, enter a playable arena, move/look as permitted, switch all three weapons, see the three crosshair styles/ranges, shoot visible projectiles, hit targets, collide with cargo/boundaries, change/use cameras, and observe the lighting/day-night systems. `calc.csv` must be generated.

Do NOT implement Phase 2-4 gameplay in this run unless a small interface/stub is structurally necessary. Do not leave broken placeholder menu actions.

DELIVERY PROCESS:
1. Explain briefly what you found in the existing project.
2. Give a concise Phase 1 file/module plan.
3. Implement directly in the supplied project.
4. Update CMakeLists.txt for every new source file.
5. Build from a clean/configured build directory.
6. Fix all compilation and linker errors.
7. Run/test the Phase 1 checkpoint when the environment allows.
8. Update README with current build/run instructions and controls.
9. Report changed/created files and exact playable features.

Do not merely provide code snippets or instructions. Produce the actual updated project files. Preserve working existing features and leave the repository in a stable playable state ready for Phase 2.
```

---

# 29. Exact Prompt — Phase 2

Use this only after Phase 1 has produced a stable playable project. Provide **this `final-project.md` plus the complete Phase 1 project** as references.

```text
PHASE 2 OF 4 — COMPLETE SEVEN-LEVEL CHALLENGE GAMEPLAY

Read the entire provided `final-project.md` and inspect the COMPLETE current project produced by Phase 1. Phase 1 is already a working baseline. Preserve it and extend it; do not create a replacement project.

Work ONLY on Phase 2 in this run.

ARCHITECTURE REQUIREMENT:
Implement a reusable level framework. Prefer:
- LevelBase/interface
- LevelManager
- separate Level1.cpp/.h through Level7.cpp/.h
- reusable target movement-pattern configuration
- reusable spawn/entity configuration
- shared ScoreSystem
- shared Target/Bird/Human behavior
- shared collision/rendering/animation functions

Each level file should primarily describe/configure its level. Do NOT copy target rendering, collision, scoring, weapon logic, or movement algorithms seven times.

IMPLEMENT THE EXACT CHALLENGE RULES FROM `final-project.md`:
- Level 1: stationary player; 3 static targets in weapon range.
- Level 2: stationary player; 3 targets, slightly farther but targetable; 1 target moves on X.
- Level 3: stationary player; 4 targets moving with the required X/Y/Z random-axis behavior; all remain weapon-accessible and are not made impossible by cargo.
- Level 4: player movement enabled; 6 targets; X/Y/Z movement variety; some targets rotate while moving; movement speeds vary.
- Level 5: 8 targets with the exact compound-axis/spinning rules from the specification; 6-8 birds continuously present as required; bird hit = -100.
- Level 6: 10 targets with previous behavior plus the two faster all-three-axis spinning targets; increased birds including the required birds near targets; bird hit = -100.
- Level 7: 12 targets with the exact faster advanced target behavior; required birds and humans around targets; bird = -100 and human = -200; birds use bounded random 3D movement and humans use bounded random ground movement near targets.

Challenge Mode rules:
- Player proceeds to the next level only after completing the current level.
- Level completion requires all targets destroyed unless `final-project.md` explicitly specifies otherwise.
- Targets do NOT respawn in Challenge Mode.
- Target = +100.
- One-shot/bullseye target = +150 total (+100 normal +50 bonus).
- Bird = -100.
- Human = -200.
- Negative total scores are valid.
- Score and elapsed time accumulate through Challenge progression.
- Track target destroyed, bullseye, bird kill, human kill, score, elapsed time, and levels cleared.
- Show animated point additions/deductions.
- Preserve cargo/player/projectile collision and all Phase 1 systems.
- Cargo must become dense enough to require movement in movable levels but must never permanently make a required target impossible.

NPC/OBJECT RULE:
Birds, humans, targets, cargo, and all other modeled 3D objects must continue to follow the cube-only transformed-assembly requirement. Use believable proportions/colors while keeping the implementation understandable.

MANDATORY CSV:
Continue automatic `calc.csv` generation. Expand it for Phase 2 and include representative transformations from Level 1-7 objects, moving/spinning targets, cargo, birds, humans, and existing scene categories. Values must correspond to the actual transformation system. Mark rows as Phase 2 where appropriate.

PLAYABLE CHECKPOINT:
The project must build and run. I must be able to start Challenge Mode at Level 1 and sequentially complete Levels 1-7. Every level must enforce its movement/player/NPC/target rules. Progression must not occur before the level completion condition. Score and time must accumulate correctly. Existing weapons, cameras, lighting, collisions, HUD, and CSV generation must still work.

Do not implement the full Free Mode/Developer Mode/leaderboard persistence/final polish yet except for architecture hooks genuinely needed by Phase 2.

DELIVERY PROCESS:
1. Inspect and summarize the current Phase 1 architecture before modifying it.
2. Plan the reusable level system.
3. Implement actual Level1-Level7 files and shared systems.
4. Update CMakeLists.txt.
5. Build cleanly and fix errors.
6. Test each level independently where practical, then test sequential Challenge progression.
7. Verify scoring, penalties, bullseyes, completion, accumulated time, reset behavior, collision, and CSV output.
8. Update README.
9. Report changed files and the exact playable Phase 2 behavior.

Do not return only examples/snippets. Modify the full supplied project and leave it stable, playable, and ready for Phase 3.
```

---

# 30. Exact Prompt — Phase 3

Use this with **this `final-project.md` and the complete working Phase 2 project**.

```text
PHASE 3 OF 4 — GAME MODES + BIRD'S-EYE VIEW + LEADERBOARD PERSISTENCE

Read the complete provided `final-project.md`, then inspect the COMPLETE working Phase 2 project. Preserve and reuse the existing arena, Level1-Level7, target/NPC systems, weapons, scoring, transforms, collision, rendering, cameras, lighting, UI, and CSV logger.

Work ONLY on Phase 3 in this run.

DO NOT duplicate Level 7 or create separate hard-coded copies of the arena for different modes. Game modes must reuse the shared level/world/entity systems.

IMPLEMENT:

1. MAIN MODE MENU
- Free Mode
- Challenge Mode
- Developer Mode
- Bird's-Eye View entry where appropriate
- Existing controls/settings/help/exit functionality should remain coherent.

2. CHALLENGE MODE
- Use the complete sequential Level 1-7 implementation from Phase 2.
- Preserve accumulated score/time/stats and exact completion behavior.

3. FREE MODE
- Use the Level 7 target positions/world setup and corresponding advanced arena behavior defined in `final-project.md`.
- 3-minute countdown visible on HUD.
- Destroyed targets respawn after 30 seconds.
- Session remains playable for the full countdown.
- Apply scoring/bullseye/NPC penalty behavior required by the specification.
- End cleanly when time reaches zero and produce session stats.

4. DEVELOPER MODE
- Show clickable cards/buttons for Level 1 through Level 7.
- Clicking a level launches that exact shared level implementation directly.
- Provide a clean way to return to Developer Mode/menu.
- Do not duplicate level code.

5. BIRD'S-EYE VIEW
- Display the Level 7 arena from a true overhead/elevated inspection camera.
- Allow clicking a valid arena point to relocate an observation camera to that world location.
- From the selected point, allow mouse look/yaw/pitch so the user can inspect surroundings.
- Reuse the same world and rendering state.
- Prevent invalid placement inside blocking cargo/walls where necessary.

6. LEADERBOARD.CSV
Create/maintain persistent `leaderboard.csv`.
Logical primary key = (Name, Mode).
Track at least:
Name, Mode, BestScore, LevelsCleared, TargetsDestroyed, Bullseyes, BirdKills, HumanKills, Time.
Follow the exact update eligibility rules in `final-project.md`.
When comparing eligible records for the same (Name, Mode):
- higher score is better;
- if score is equal, lower time is better.
Do not create duplicate rows for the same logical key when an update is intended.
Preserve valid existing leaderboard data across restarts.

7. LEADERBOARD UI
- Calculate/show rank.
- Provide a scrollable leaderboard.
- Sort primarily by score descending and then time ascending as specified.
- Show the relevant leaderboard after eligible session/game endings.

8. REGRESSION
- Preserve all Phase 1 and Phase 2 features.
- Player and projectiles cannot pass through cargo.
- Dense cargo remains navigable and cannot permanently invalidate target completion.
- Weapons/crosshairs/range, cameras, day/night, lighting, NPC movement, scoring, and Challenge progression remain functional.

MANDATORY TRANSFORMATION CSV:
`calc.csv` must still regenerate automatically from actual scene transformations. Expand/verify Phase 3 coverage for all current modes and Bird's-Eye-related world/camera reference objects where appropriate. Do not replace the existing transformation logger with hand-written static rows.

PLAYABLE CHECKPOINT:
The project must compile and launch. From the menu I must be able to play Challenge Mode, Free Mode, and any selected Developer Mode level. Free Mode must run for 3 minutes with 30-second target respawn. Bird's-Eye View and click-to-observe must work. `leaderboard.csv` must persist and update correctly. `calc.csv` must still generate. Restarting the application must preserve leaderboard records.

DELIVERY PROCESS:
1. Inspect the full Phase 2 project first.
2. Explain the reuse strategy for levels/game modes.
3. Implement the actual project changes.
4. Update CMakeLists.txt.
5. Build and fix all compile/link errors.
6. Test all three game modes, Bird's-Eye View, leaderboard read/write/update/sort, and restart persistence.
7. Regression-test Challenge progression and core gameplay.
8. Update README with modes, controls, CSV schemas, and persistence behavior.
9. Report exact files changed and tested behavior.

Do not provide only snippets. Return/produce the complete updated project in a stable playable state ready for final Phase 4.
```

---

# 31. Exact Prompt — Phase 4

Use this with **this `final-project.md` and the complete working Phase 3 project**.

```text
PHASE 4 OF 4 — FINAL POLISH + ANIMATION + VISUAL REFINEMENT + RELEASE

Read the ENTIRE provided `final-project.md` and inspect the COMPLETE working Phase 3 project before changing anything. This is the final cumulative phase. Preserve every correct feature from Phases 1-3 and finish the project to submission quality.

Do not rewrite the project unnecessarily. First identify reusable systems and remaining gaps against `final-project.md`, then implement/refactor only what is needed.

FINAL REQUIRED WORK:

1. ANIMATED PRESENTATION
- Add a polished animated game-start sequence.
- Add clear animated level-start feedback.
- Add satisfying level-complete transitions.
- After completing Level 7, show a strong all-levels-complete/victory animation.
- Add Free Mode end animation.
- Then show final statistics, player rank, and scrollable leaderboard.
- Animations must not break game state or input reset.

2. SCORE/HAZARD FEEDBACK
- +100 normal target feedback.
- +150 total one-shot/bullseye feedback.
- -100 bird feedback with red danger animation and sound.
- -200 human feedback with stronger appropriate danger/penalty feedback and sound.
- Negative score must display correctly.
- Ensure duplicate collision events do not award/deduct repeatedly for one resolved hit.

3. VISUAL QUALITY WHILE REMAINING CUBE-ONLY
- Improve weapons, targets, target stands, fortified wall, cargo, birds, humans, lamps, arena, and environmental elements.
- ALL required modeled objects must remain built from transformed cubes/assemblies; do not solve visual quality by importing finished 3D models.
- Use believable colors/materials, proportions, lighting response, and small cube-based details.
- Keep each weapon visually distinct.
- Birds and humans must be recognizable from cube assemblies.
- Keep variety in target/NPC motion.

4. CARGO FINALIZATION
- Make the arena contain many cargo stacks so all targets are not visible from one point in advanced gameplay.
- Use the exact size/stacking intent from `final-project.md`: individual cargo around one-third human height, varied 1-5 stack heights, common horizontal/vertical groups of roughly 3-7 boxes, and mixed-direction formations.
- Use controlled random variation/normal-distribution-style height tendency where specified.
- Player and bullets cannot pass through cargo.
- Never permanently fully block a required target or all routes to it.
- Keep deterministic/reproducible generation when useful for debugging and level consistency.

5. MOVEMENT FINALIZATION
- Verify exact Level 1-7 target counts and movement patterns.
- Verify compound-axis/spinning/fast targets.
- Birds move randomly in bounded 3D regions and stay near associated targets.
- Humans move randomly on the ground plane near associated targets.
- Provide meaningful variation rather than identical movement clones.

6. UI/UX
- Polish menus, mode cards, Developer level cards, HUD, countdown, score, range, weapon indicator, level indicator, stats, rank, leaderboard, pause/help, and transition screens.
- Keep controls readable and consistent.
- Ensure mouse capture/release and menu transitions are reliable.

7. AUDIO/LIGHTING/CAMERA
- Finalize required sound feedback.
- Verify day/night, ambient/diffuse/specular, directional/point/spot lighting.
- Verify player/preset/free/Bird's-Eye/observation cameras.
- Avoid camera placement through blocking geometry where practical.

8. MODULARITY/SCALABILITY REFACTOR
Perform a final architecture review:
- `main.cpp` stays small.
- Level1-Level7 remain separate but reuse LevelBase/common systems.
- Remove duplicated level movement/scoring/collision/rendering code.
- Use shared configuration structures/functions.
- Keep GameMode, LevelManager, ScoreSystem, Leaderboard, CsvLogger, collision, camera, rendering, NPC, weapon, and UI responsibilities separated.
- Break up giant functions/classes if they prevent clear explanation or future extension.
- Do not over-engineer; this must remain understandable for a university graphics viva.

9. LEADERBOARD FINAL VERIFICATION
- `leaderboard.csv` persists.
- (Name, Mode) logical primary key behavior is correct.
- Best eligible record logic = higher score, then lower time.
- Required fields are stored correctly.
- Challenge updates only at valid completed-level events according to the specification.
- Rankings/sorting display score first and lower time second.
- Scrollable leaderboard works after endings.

10. FINAL TRANSFORMATION CSV
The completed application MUST automatically generate the final `calc.csv` from representative actual transformations used by the complete project.
Verify coverage for:
- ground/arena;
- walls/battlements;
- cargo and stacks;
- pistol/shotgun/assault-rifle components;
- projectiles;
- targets/stands and representative movement/spin states;
- lamps/environment objects;
- birds;
- humans;
- representative Level 1-7 objects;
- other major cube-built scene categories.
Use the common M_model = T * Rz * Ry * Rx * H * S convention and actual transformation values. Include phase/object/component/local point/scale/shear/rotations/translation/result/purpose metadata. The CSV must be useful for demonstrating 3D transformations in the course presentation.

11. RELEASE VALIDATION
- Delete/use a fresh build directory as appropriate and perform a clean configure/build.
- Fix every compilation/linker error.
- Run the application when the environment permits.
- Test menu -> each mode -> gameplay -> ending -> leaderboard -> return/restart flows.
- Test all seven levels.
- Test all weapons and crosshairs.
- Test collision with cargo/walls/targets/NPCs.
- Test Free Mode countdown and 30-second target respawn.
- Test Developer direct-level launch.
- Test Bird's-Eye click-to-observe.
- Test day/night, cameras, sound, score animation, victory animation, CSV generation, and leaderboard persistence.

FINAL DOCUMENTATION:
Update README.md with:
- project overview;
- dependency/build instructions;
- exact run steps;
- controls;
- game modes;
- Level 1-7 summary;
- scoring and penalties;
- Free Mode timer/respawn;
- Developer Mode;
- Bird's-Eye View;
- camera controls;
- lighting/day-night;
- `calc.csv` explanation;
- `leaderboard.csv` schema/update logic;
- architecture/module summary.

FINAL PLAYABLE CHECKPOINT:
This must be the complete submission-ready game described by `final-project.md`. All major requirements must work together in one clean executable. Do not leave TODO-only features or knowingly broken menu paths. If the execution environment genuinely prevents a specific runtime test, still complete the code/build work possible and report that limitation precisely rather than claiming it was tested.

Do not respond with snippets only. Modify and deliver the complete project. At the end, give a concise implementation report listing created/changed files, build result, tested flows, generated CSV files, controls, and any genuine remaining limitation.
```

---

# 32. How to Use the Four Prompts

For best results, each phase should receive two references:

1. **This complete `final-project.md`** — authoritative specification.
2. **The complete latest project folder/ZIP** — authoritative implementation baseline.

Run the prompts in order:

`Phase 1 -> verify playable -> Phase 2 -> verify playable -> Phase 3 -> verify playable -> Phase 4 -> final clean verification`

Never start a later phase from an older project copy. The input to each phase must be the complete successfully built output of the previous phase.
