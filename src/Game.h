#pragma once
#include "Arena.h"
#include "Camera.h"
#include "Cargo.h"
#include "Collision.h"
#include "Lighting.h"
#include "Weapon.h"
#include "Projectile.h"
#include "Target.h"
#include "SoundEvent.h"
#include "LevelManager.h"
#include "Bird.h"
#include "Human.h"

namespace shooter {
struct Debris { std::uint64_t id; Vec3 position, velocity, rotation; float life; };
class Game : public RunStats {
public:
    Game();
    Camera player, freeCamera;
    WeaponType weapon=WeaponType::Pistol;
    int cameraMode=1, lastRing=-1;
    float feedbackTime=0;
    bool night=false, soundEnabled=true, soundAvailable=true;
    float cooldown=0, recoil=0;
    bool challenge=false;
    LevelManager levels;
    std::vector<Bird> birds;
    std::vector<Human> humans;
    std::vector<ScorePopup> scoreFeedback;
    float dangerTime=0;
    std::vector<Target> targets;
    std::vector<Projectile> projectiles;
    std::vector<SceneObject> staticObjects;
    std::vector<Debris> debris;
    std::vector<SoundEvent> soundEvents;
    void reset();
    void resetTargets();
    void startChallenge(int firstLevel=1);
    bool nextLevel();
    void restartLevel();
    bool gameplayActive() const;
    int remainingTargets() const;
    void applyNpcHit(bool human, std::size_t index, std::uint64_t shotId);
    void movePlayer(float forward, float right, float dt, bool fast);
    void update(float dt);
    bool fire();
    Camera activeCamera() const;
    void setCamera(int mode);
    int aimedTarget(float& distance) const;
    std::vector<SceneObject> scene(bool includePlayer=true) const;
    std::vector<SceneObject> calculationObjects() const;
    void applyTargetHit(std::size_t index, int ring, std::uint64_t shotId);
private:
    std::uint64_t nextProjectile=1;
    std::uint64_t nextShot=1, nextDebris=1;
    void updateStep(float dt);
    bool canStand(Vec3 position) const;
    void loadCurrentLevel();
    void updateNpcs(float dt);
    std::vector<NpcCollider> npcColliders() const;
};
}
