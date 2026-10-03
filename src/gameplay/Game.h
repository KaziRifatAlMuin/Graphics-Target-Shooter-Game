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
struct Debris {
    std::uint64_t id;
    Vec3 position, velocity, rotation;
    float life;
    Vec3 color{.95f,.4f,.14f};
    Vec3 scale{.12f,.12f,.05f};
    bool isBlood=false;
};
class Game : public RunStats {
public:
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
    void reset();
    void resetTargets();
    void startChallenge(int firstLevel=1);
    void startMode(GameMode selected,int level=1);
    bool usesLevel() const { return mode!=GameMode::Practice; }
    std::optional<EligibleResult> takeResult();
    bool nextLevel();
    void restartLevel();
    bool gameplayActive() const;
    int remainingTargets() const;
    void applyNpcHit(bool human, std::size_t index, std::uint64_t shotId);
    void spawnBlood(Vec3 position, int count);
    void movePlayer(float forward, float right, float dt, bool fast);
    void update(float dt);
    bool fire();
    Camera activeCamera() const;
    void setCamera(int mode);
    int aimedTarget(float& distance) const;
    // Cached version avoids recomputing ray-vs-all-objects every frame.
    mutable int cachedAimTarget=-1;
    mutable float cachedAimDistance=0;
    mutable Vec3 cachedAimPos{}, cachedAimDir{};
    mutable int cachedAimFrame=-1;
    std::vector<SceneObject> scene(bool includePlayer=true) const;
    std::vector<SceneObject> calculationObjects() const;
    void applyTargetHit(std::size_t index, int ring, std::uint64_t shotId);
private:
    std::uint64_t nextProjectile=1;
    std::uint64_t nextShot=1, nextDebris=1;
    bool challengeFromStart=true;
    std::optional<EligibleResult> pendingResult;
    void updateStep(float dt);
    bool canStand(Vec3 position) const;
    void loadCurrentLevel();
    void updateNpcs(float dt);
    void updateNpcDeaths(float dt);
    std::vector<NpcCollider> npcColliders() const;
};
}
