# 3D Target Shooter --- Complete Project Specification

**Student:** Kazi Rifat Al Muin\
**Roll:** 2107042\
**Platform:** OpenGL / C++\
**Project Type:** Interactive 3D Graphics Project

## 1. Project Overview

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
