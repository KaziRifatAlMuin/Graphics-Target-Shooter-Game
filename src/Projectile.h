#pragma once
#include "Weapon.h"
#include <cstdint>
#include "Target.h"
#include <functional>

namespace shooter {
struct Projectile {
    std::uint64_t id=0;
    WeaponType weapon=WeaponType::Pistol;
    Vec3 position, direction;
    float travelled=0;
    std::uint64_t shotId=0;
};
SceneObject projectileObject(const Projectile& projectile);
using TargetHitCallback=std::function<void(std::size_t,int,std::uint64_t)>;
void updateProjectiles(std::vector<Projectile>& projectiles, const std::vector<Target>& targets,
                       const std::vector<SceneObject>& obstacles, float dt, const TargetHitCallback& hit);
}
