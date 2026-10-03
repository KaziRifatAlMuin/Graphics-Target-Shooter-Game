For the current textured release, HUD, screenshots and verification, see [README](../README.md) and [release verification](release-verification.md). Build paths in the historical validation section below refer to the earlier lighting-only release and have been cleaned.

# Lighting and NPC implementation guide

This guide describes the final implementation, rather than the original planning prompts.
The README remains the main player/build guide. `docs/scene-reference.md` is generated
from the actual builders and lists the current cube components and level configurations.

## A short demonstration for the viva

1. Start the final executable. Choose Free or Developer. The registration page appears
   before gameplay; no name is filled in on a normal fresh launch. Enter your callsign.
   A name provided explicitly through `--name` is intentional user input, not a default.
2. Choose Level 7 in Developer to inspect the most populated level without recording a
   competitive score. The seven original level definitions are shared by every mode.
3. Press F2 for the arena camera and N for night. Point out the grounded boundary towers,
   their three-panel floodlight banks, start posts and wall brackets.
4. Press F4 to inspect from nearby. Q/E changes altitude. The camera can fly, but the
   player stays grounded and free-camera shooting remains disabled.
5. Observe brighter ground under floodlights and darker gaps. Look at a wall near a red
   or blue side lamp: colored light affects the wall, not merely the lens.
6. Press F6, F7, F8 at the same camera position. Explain why large polygons lose local
   highlights with vertex sampling. Return to F8 for normal play.
7. Press N for day. All artificial light contributions become zero; lenses become gray.
   The one directional sunlight source supplies general illumination. At night there is
   no visible moon or sunlight contribution.
8. Switch back to F1 to shoot. Weapon selection, aim, scoring, camera movement, target
   motion, respawns and level progression use the original gameplay pipeline.

The Developer HUD identifies the active shading mode; shading shortcuts are also listed on Controls. F1-F4 were already camera keys, so their
meaning is preserved. F6/F7/F8 avoid introducing a camera-control regression.

## Files and data flow

- `src/world/Lighting.h`: small point/spot records and the ambient/directional rig.
- `src/world/Lighting.cpp`: eight point definitions, six spotlight definitions and
  cube-built fixtures. This is the single source for fixture/source placement.
- `src/core/SceneObject.h`: existing RGB, specular strength, shininess and emission.
  Ambient and diffuse share the object's RGB instead of adding a redundant material class.
- `src/rendering/Renderer.cpp`: compiles shaders, caches uniform locations, uploads the
  rig, computes normal matrices and renders the existing shared cube geometry.
- `shaders/lighting.glsl`: shared Phong calculation, distance falloff and cone mask.
- `shaders/object.vert`: transforms geometry; computes vertex lighting for Flat/Gouraud.
- `shaders/object.frag`: selects constant/interpolated/per-fragment illumination, applies
  material/pattern, emission and the existing gamma conversion.
- `src/world/Environment.cpp`: returns no celestial geometry at night.
- `src/gameplay/GameScene.cpp`: sets lens emission according to day/night.
- `src/gameplay/SessionController.cpp`: required registration and existing-name checks.
- `src/gameplay/Challenge.cpp`: population initialization and survivor reassignment.

GLSL 3.30 does not have a built-in include directive. The existing shader loader expands
one exact `#include "lighting.glsl"` line before compiling. The same readable equations
are therefore compiled into both shader stages without maintaining two copies. Shader
compile/link failures report errors and stop startup instead of silently drawing black.

## How the light calculations fit together

Everything is calculated in world space. The transformed surface position, normal,
camera position and lamp positions all use the same coordinate system. For a point:
subtract position from lamp position, take its length as d, divide by d to obtain L.
A small minimum distance prevents division by zero at a source.

For the sun, L is constant. For a spotlight, also compare its outward direction with
-L. A dot product is the cosine of their angle. The cosine thresholds are computed
once from degrees on the CPU. Between outer and inner cone boundaries the intensity
smoothly changes from zero to one. Constant + linear + quadratic attenuation then
reduces the contribution with distance. Ambient is added once globally, not once for
every local lamp; otherwise adding lamps would brighten the whole night uniformly.

Diffuse and specular are accumulated separately. With constant object RGB, interpolating
these terms and then multiplying diffuse by RGB gives the same result as interpolating
lit vertex colors. This also keeps target rings legible without requiring a densely
subdivided target mesh. Flat uses one constant lighting sample per triangle, while its
surface pattern can still change across that triangle. Neither pattern nor emission
is evidence of per-fragment lighting in the vertex modes.

The inverse-transpose normal matrix is calculated on the CPU per object and uploaded as
a mat3. Ordinary position transforms are insufficient for normals under nonuniform
scale or shear. The shared cube stores a distinct normal per face, so no averaging
across a sharp box edge occurs. A larger shininess exponent tightens highlights; it
does not make a surface more diffusely illuminated.

## Fixture construction and limitations

The light rig is deliberately bounded to 8 points and 6 spots. Six towers sit close to
the side walls, three on each side, aiming across the arena and slightly downrange.
Each tower has a concrete base, vertical pole, support bars, metal heads and three
small lenses. One mathematical spot represents the bank to keep fragment cost modest.
Lens fronts point along the spotlight direction. Smaller lamps use two grounded posts
and six wall brackets attached to the existing boundary.

The illuminated lens makes the source visible; emission alone cannot light another
object. That requires the source's point or spotlight contribution. In daylight the
scene builder overrides lens color with neutral gray and sets emission to zero. Night uses
the rig's RGB and makes lenses emissive. RGB values can exceed one as a simple intensity
scale before the existing output clamp. This does not introduce an HDR pipeline.

There are no shadow maps or occlusion checks, so a lamp can illuminate through cargo or
a wall. This is an intentional limitation of the requested simple illumination model.
There is no visible volumetric cone, bloom, PBR, ray tracing, radiosity or normal mapping.
The physical cone is visible through the lit surfaces it reaches. The large floor cube
has few vertices: Gouraud can miss interior light pools, whereas Phong evaluates them
at the pixels where they occur. Flat/Gouraud are demonstration modes, not a guarantee
of the same nighttime visibility as the default Phong mode.

## Exactly what happens to living birds and humans

### Damage is not destruction

A nonfatal target hit only changes target health/feedback. Its NPCs keep their current
zone. Reassignment occurs after the target is permanently eliminated in Challenge or
Developer, when the next active NPC update sees `target.eliminated`.

### Reassignment for BOTH species

The code first counts active birds assigned to each undestroyed target, then independently
does the same for humans. For each living NPC whose old target has been eliminated:

1. Collect undestroyed targets with fewer than 10 active NPCs of that species.
2. Use that NPC's seeded pseudorandom generator to choose among the available targets.
3. Update its zone and home. Increase the destination count immediately, so a later NPC
   cannot overfill it during the same update.
4. Reposition the bird to a bounded flying location, or the human to a collision-checked
   ground location using the existing placement builder. Pick a new local destination.
5. Keep identity, living status, movement speed, animation state, human height and prior
   shot-penalty history. This is a relocation of a survivor, not a new or revived NPC.

Relocation is immediate; there is no advanced pathfinding journey between zones. The
random generator is deterministic for a fixed seed and event sequence, which makes the
behavior reproducible for tests and demonstrations.

### Capacity and the last target

The limit is **10 birds AND 10 humans per target**, not 10 combined. Initial loading also
clamps configured per-target counts. Level 7 still begins with its original 4 birds and
3 humans per target (48 birds, 36 humans); the cap mainly matters after redistribution.

Example: ten birds already belong to the last live target and another fifteen survive
elsewhere. Those fifteen remain alive in their previous zones. They continue local
movement while gameplay is active and are not killed or deleted. Their old zone IDs are
retained so reassignment can be retried if capacity becomes available. The target cap
limits assignment, not the number of animals that might be visible near one another.
When all targets are eliminated, the level completion rules freeze normal play; remaining
living NPCs are not silently converted into deaths. The next level creates its own
initial population as before.

### Free Mode and fatal NPC hits

Free target destruction starts a 30-second respawn timer without permanent elimination.
Its NPCs keep that zone, so they do not move to another target just because the target
is temporarily unavailable. Target respawn restores the target only. A bird or human
hit is fatal, applies its existing -100/-200 penalty once per shot, removes its live
collider and begins the original death animation. Dead/falling NPCs never enter the
redistribution candidates and are not revived. Bodies remain visual only. Restarting a
level/session is the existing explicit way to recreate the initial population.

## Registration and persistence

Every mode button opens the same large registration panel, labeled with the chosen mode.
The internal enum retains the old name `ChallengeName` to keep the UI patch small; it now
serves all modes. Text is limited to 24 printable ASCII characters. Surrounding spaces
are trimmed; empty/all-space input cannot launch gameplay. Backspace edits, Enter submits,
and Esc returns to the menu. A previously typed callsign may be reused, but a fresh
normal launch starts empty. Developer registration leads to its seven level cards.

Free/Challenge duplicate warnings use the selected mode's records. They require explicit
confirmation, preserving the prior best-score rules. Noncompetitive modes do not submit
records. No new file format was introduced. Existing `leaderboard.csv`, `calc.csv` and
`calc-init.csv` are not migration targets and do not need replacement.

Builds no longer automatically regenerate the root calculation file. Normal interactive
runs retain the existing live calculation export behavior. Use `--calc PATH` and
`--leaderboard PATH` for isolated testing. The tests for this change use build-directory
files, leaving the user's original root CSV contents intact.

## Validation and performance

Use the existing CMake/CTest suite, plus `main --modes-smoke-test` with isolated CSV paths.
The hidden-window test actually opens OpenGL, compiles/links shaders, renders scene/UI,
reads pixels/depth and exercises weapons, camera views, seven Challenge levels, Developer
cards, the full simulated Free timer and respawn, names and persistence. It is automated
coverage, not a claim that a human played every scenario. The release tests additionally
check blank names for every mode, sun/lamp toggles, boundary spotlight placement, survivor
preservation, permanent death and the separate per-target capacity limits.

The scripted near-face projectile helper isolates scoring by placing living NPCs clear
of its injected ray. Previously Level 7 NPCs disappeared with their targets; survivors
can now obstruct that ray. Dedicated physical NPC-hit tests and the longer player-muzzle
firing scenario keep normal NPC collisions enabled.

Rendering keeps one forward pass and the existing shared cube buffer. Uniform locations
are cached; Phong skips vertex lighting and daylight skips zero-color local lights.
Existing fixed simulation steps, cached aiming and asynchronous calculation writes remain.
Smoothness depends on GPU, scene population and resolution; no fixed FPS is promised.
Use the Developer FPS display for the current machine and prefer the default Phong mode
for normal play. There is no additional heavy rendering framework or dependency.


## Final verification record (2026-10-03)

- Release build: CMake 3.31.6 / MinGW GCC 13.2, `build/lighting-final`, successful.
- CTest: **10/10 passed**, including all seven levels, mode rules, live projectile/NPC
  contacts, registration, survivor caps, persistence and independent CSV transform math.
- Full `--modes-smoke-test`: passed on AMD Radeon(TM) Graphics / OpenGL 3.3, including
  sequential Challenge completion, seven Developer cards, 180 simulated Free seconds,
  30-second target respawn, three weapons and camera/UI checks.
- Final renderer `--smoke-test`: passed; Flat, Gouraud and Phong produced distinct
  night frames with visible geometry and no OpenGL errors.
- Day/night, name-page and shading captures were visually inspected. Reproducible PPM
  captures and locally converted PNGs are under `build/lighting-final/preview-*`.
- Root `main.exe` matches the final CMake executable. Original root CSV SHA-256 values
  are unchanged from the beginning of this task:

```text
calc.csv        971F2A4D6C846FC99FADF54715DDA464DF6FCBB75F4FE9216A32B94CCE36550F
leaderboard.csv 87991FAE71DF3A2D9F4A6622FDC6E86D21E6742C58C03AB47A2E2759E3D868FC
```

The gameplay checks are automated; they are not a measured frame-rate guarantee or a
claim of exhaustive manual playtesting. The calculation export still updates normally
when the user subsequently starts an interactive game without `--calc`.
