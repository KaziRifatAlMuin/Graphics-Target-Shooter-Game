#pragma once
#include "Arena.h"
#include "Camera.h"
#include <cstdint>
#include <unordered_map>
#include "Lighting.h"

namespace shooter {
enum class WeaponType { Pistol, Shotgun, Rifle };
struct WeaponSpec {
    const char* name;
    float range, speed, cooldown, spread;
    int pellets;
    Vec3 projectileScale;
};
const WeaponSpec& weaponSpec(WeaponType type);
std::vector<SceneObject> createWeapon(WeaponType type, const Camera& player, float recoil=0);
Vec3 weaponMuzzle(WeaponType type, const Camera& player);

struct Target {
    Vec3 base, position;
    int movement=0;
    static constexpr float radius=.8f, thickness=.16f;
    float phase=0, yaw=0, health=60, hitTime=0, respawn=0;
    std::unordered_map<std::uint64_t,int> damageByShot;
};
struct TargetContact { float distance; int ring; }; // ring 0=center ... 5=outer; -1=back/edge.
TargetContact intersectTarget(Vec3 origin, Vec3 direction, const Target& target, float maximum);
int ringDamage(int ring);
enum class SoundEvent { Pistol, Shotgun, Rifle, Hit, Break, Click };
struct Debris { std::uint64_t id; Vec3 position, velocity, rotation; float life; };
struct Projectile {
    std::uint64_t id=0;
    WeaponType weapon=WeaponType::Pistol;
    Vec3 position, direction;
    float travelled=0;
    std::uint64_t shotId=0;
};
// Ray parameter is in world meters, including for rotated/scaled/sheared cubes.
float intersectCube(Vec3 origin, Vec3 direction, const Transform& box, float maximum);
std::vector<SceneObject> generateCargoLayout(unsigned seed);

class Game {
public:
    Game();
    Camera player, freeCamera;
    WeaponType weapon=WeaponType::Pistol;
    int cameraMode=1, shots=0, hits=0, destroyed=0;
    int score=0, bullseyes=0, lastRing=-1;
    float feedbackTime=0;
    bool night=false, soundEnabled=true, soundAvailable=true;
    float elapsed=0, cooldown=0, recoil=0;
    std::vector<Target> targets;
    std::vector<Projectile> projectiles;
    std::vector<SceneObject> staticObjects;
    std::vector<Debris> debris;
    std::vector<SoundEvent> soundEvents;
    void reset();
    void resetTargets();
    void movePlayer(float forward, float right, float dt, bool fast);
    void update(float dt);
    bool fire();
    Camera activeCamera() const;
    void setCamera(int mode);
    int aimedTarget(float& distance) const;
    Transform targetTransform(const Target& target) const;
    std::vector<SceneObject> scene(bool includePlayer=true) const;
    std::vector<SceneObject> calculationObjects() const;
    void applyTargetHit(std::size_t index, int ring, std::uint64_t shotId);
private:
    std::uint64_t nextProjectile=1;
    std::uint64_t nextShot=1, nextDebris=1;
    void updateStep(float dt);
    bool canStand(Vec3 position) const;
};
}
