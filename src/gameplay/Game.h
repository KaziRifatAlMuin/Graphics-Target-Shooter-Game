#pragma once
#include "world/Arena.h"
#include "camera/Camera.h"
#include "world/Cargo.h"
#include "core/Collision.h"
#include "world/Lighting.h"
#include "gameplay/Weapon.h"
#include "gameplay/Projectile.h"
#include "gameplay/Target.h"
#include "audio/SoundEvent.h"
#include "gameplay/LevelManager.h"
#include "gameplay/Bird.h"
#include "gameplay/Human.h"
#include "gameplay/GameMode.h"
#include "camera/BirdEyeCamera.h"
#include <optional>

namespace shooter {
// One temporary particle carries position, velocity (meters/second), rotation, and remaining life (seconds).
struct Debris {
    std::uint64_t id;
    Vec3 position, velocity, rotation;
    float life;
    Vec3 color{.95f,.4f,.14f};
    Vec3 scale{.12f,.12f,.05f};
    bool isBlood=false;
};
// Own the live simulation and run statistics; rendering reads its scene objects each frame.
class Game : public RunStats {
public:
    // Build the practice arena, cargo, fixtures, and equipment, then initialize the session.
    Game();
    Camera player, freeCamera;
    WeaponType weapon=WeaponType::Pistol;
    int cameraMode=1, lastRing=-1;
    float feedbackTime=0;
    bool night=false, soundEnabled=true, soundAvailable=true;
    float cooldown=0, recoil=0;
    GameMode mode=GameMode::Practice;
    std::string playerName;
    BirdEyeCamera birdEye;
    double freeRemaining=freeSessionSeconds;
    LevelManager levels;
    std::vector<Bird> birds;
    std::vector<Human> humans;
    std::vector<ScorePopup> scoreFeedback;
    float dangerTime=0;
    bool dangerHuman=false;
    std::vector<Target> targets;
    std::vector<Projectile> projectiles;
    std::vector<SceneObject> staticObjects;
    std::vector<Debris> debris;
    std::vector<SoundEvent> soundEvents;
    // Return to practice defaults and reset run statistics while preserving unique entity ID counters.
    void reset();
    // Restore practice targets and clear temporary effects, or restart the current level.
    void resetTargets();
    // Start Challenge through the shared mode initialization path.
    void startChallenge(int firstLevel=1);
    // Reset run state and choose the requested level; Free and Bird's-Eye use the final arena.
    void startMode(GameMode selected,int level=1);
    bool usesLevel() const { return mode!=GameMode::Practice; }
    // Return a completed result once, then clear it to prevent duplicate submissions.
    std::optional<EligibleResult> takeResult();
    // Advance only an eligible completed Challenge level and then rebuild its scene.
    bool nextLevel();
    // Restart the mode or restore statistics from the last completed Challenge level.
    void restartLevel();
    // Allow play in practice or while a level is in its active stage.
    bool gameplayActive() const;
    // Count targets that are neither eliminated nor waiting to respawn.
    int remainingTargets() const;
    // Penalize each shot ID at most once per character, then start its death sound, particles, and animation.
    void applyNpcHit(bool human, std::size_t index, std::uint64_t shotId);
    // Create a capped set of seeded particles with varied velocity, color, size, and lifetime.
    void spawnBlood(Vec3 position, int count);
    // Normalize input, split movement into <=0.15 m steps, and check X/Z separately to slide along walls.
    void movePlayer(float forward, float right, float dt, bool fast);
    // Split elapsed time into steps no larger than 1/120 second to stabilize the simulation.
    void update(float dt);
    // Aim from the muzzle toward the crosshair's contact point, then spawn pellets if firing is allowed.
    bool fire();
    // Return the camera pose selected by the current mode and view setting.
    Camera activeCamera() const;
    // Select player, overview, side, or free view; entering free view starts from the current camera.
    void setCamera(int mode);
    // Find the nearest unobstructed target under the player's aiming ray and reuse unchanged queries.
    int aimedTarget(float& distance) const;
    // Cached version avoids recomputing ray-vs-all-objects every frame.
    mutable int cachedAimTarget=-1;
    mutable float cachedAimDistance=0;
    mutable Vec3 cachedAimPos{}, cachedAimDir{};
    mutable int cachedAimFrame=-1;
    // Combine static scenery and current animated objects into the cube list used for this frame.
    std::vector<SceneObject> scene(bool includePlayer=true) const;
    // Add explanatory lighting metadata to the current scene for the calculation CSV.
    std::vector<SceneObject> calculationObjects() const;
    // Apply shot-specific ring damage and trigger score, destruction effects, or respawn when health reaches zero.
    void applyTargetHit(std::size_t index, int ring, std::uint64_t shotId);
private:
    std::uint64_t nextProjectile=1;
    std::uint64_t nextShot=1, nextDebris=1;
    bool challengeFromStart=true;
    std::optional<EligibleResult> pendingResult;
    // Advance effects, timers, targets, characters, and projectiles for one small simulation step.
    void updateStep(float dt);
    // Delegate walking clearance to the shared world collision test.
    bool canStand(Vec3 position) const;
    // Rebuild level scenery, targets, player state, and seeded character populations.
    void loadCurrentLevel();
    // Redistribute surviving characters from eliminated target zones, then advance their motion.
    void updateNpcs(float dt);
    // Keep already-started death animations moving even when normal gameplay is no longer active.
    void updateNpcDeaths(float dt);
    // Use the same live character cube transforms for visible bodies and bullet collisions.
    std::vector<NpcCollider> npcColliders() const;
};
}
