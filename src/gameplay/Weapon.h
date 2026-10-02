#pragma once
#include "core/SceneObject.h"
#include "camera/Camera.h"

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

}
