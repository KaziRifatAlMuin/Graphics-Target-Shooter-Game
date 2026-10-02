#pragma once
#include "core/SceneObject.h"
#include "gameplay/Movement.h"
#include <cstdint>
#include <unordered_map>

namespace shooter {
struct Target {
    Vec3 base, position;
    int movement=0;
    MovementPattern motion;
    bool eliminated=false;
    static constexpr float radius=.8f, thickness=.16f;
    float phase=0, yaw=0, health=60, hitTime=0, respawn=0;
    std::unordered_map<std::uint64_t,int> damageByShot;
};
struct TargetContact { float distance; int ring; }; // ring 0=center ... 5=outer; -1=back/edge.
TargetContact intersectTarget(Vec3 origin, Vec3 direction, const Target& target, float maximum);
int ringDamage(int ring);
std::vector<Target> createSandboxTargets();
void updateTarget(Target& target, float elapsed, float dt);
std::vector<SceneObject> createTargetObjects(const Target& target, std::size_t index);
}
