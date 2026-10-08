#pragma once
#include "core/SceneObject.h"
#include "camera/Camera.h"

namespace shooter {
// Use a small named value to choose a weapon and index its shared specification table.
enum class WeaponType { Pistol, Shotgun, Rifle };
// Range is meters, speed is meters/second, cooldown is seconds, and spread offsets the aim direction.
struct WeaponSpec {
    const char* name;
    float range, speed, cooldown, spread;
    int pellets;
    Vec3 projectileScale;
};
// Look up each weapon's range, speed, firing interval, spread, pellet count, and bullet size.
const WeaponSpec& weaponSpec(WeaponType type);
// Assemble weapon-specific cube parts relative to the player, including recoil and a brief muzzle flash.
std::vector<SceneObject> createWeapon(WeaponType type, const Camera& player, float recoil=0);
// Return the barrel tip in world coordinates so projectiles begin at the visible weapon.
Vec3 weaponMuzzle(WeaponType type, const Camera& player);

}
