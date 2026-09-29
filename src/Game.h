#pragma once
#include "Arena.h"
#include "Camera.h"
#include <cstdint>

namespace shooter {
enum class WeaponType { Pistol, Shotgun, Rifle };
struct WeaponSpec {
    const char* name;
    float range, speed, cooldown, damage, spread;
    int pellets;
    Vec3 projectileScale;
};
const WeaponSpec& weaponSpec(WeaponType type);
std::vector<SceneObject> createWeapon(WeaponType type, const Camera& player, float recoil=0);
Vec3 weaponMuzzle(WeaponType type, const Camera& player);

struct Target {
    Vec3 base, position;
    int movement=0;
    float phase=0, yaw=0, health=100, hitTime=0, respawn=0;
};
struct Projectile {
    std::uint64_t id=0;
    WeaponType weapon=WeaponType::Pistol;
    Vec3 position, direction;
    float travelled=0;
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
    float elapsed=0, cooldown=0, recoil=0;
    std::vector<Target> targets;
    std::vector<Projectile> projectiles;
    std::vector<SceneObject> staticObjects;
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
private:
    std::uint64_t nextProjectile=1;
    void updateStep(float dt);
    bool canStand(Vec3 position) const;
};
}
