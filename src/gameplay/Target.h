#pragma once
#include "core/SceneObject.h"
#include "gameplay/Movement.h"
#include <cstdint>
#include <unordered_map>

namespace shooter {
// Store a plate's rest/current positions, motion, health, respawn timer, and strongest damage per shot.
struct Target {
    Vec3 base, position;
    int movement=0;
    MovementPattern motion;
    bool eliminated=false;
    static constexpr float radius=.8f, thickness=.16f;
    float phase=0, yaw=0, health=60, hitTime=0, respawn=0;
    std::unordered_map<std::uint64_t,int> damageByShot;
};
// Store ray distance and ring index; a negative ring means a non-scoring back/edge contact.
struct TargetContact { float distance; int ring; }; // ring 0=center ... 5=outer; -1=back/edge.
// Undo target translation/yaw, test its slices, and score only entry through the printed front face.
TargetContact intersectTarget(Vec3 origin, Vec3 direction, const Target& target, float maximum);
// Ring damage=60/(ring+1), so inner-to-outer rings need 1-6 separate hits against 60 health.
int ringDamage(int ring);
// Create seven practice targets with fixed starting positions and varied motion.
std::vector<Target> createSandboxTargets();
// Advance hit/respawn timers and evaluate position or spin from elapsed simulation time.
void updateTarget(Target& target, float elapsed, float dt);
// Build the stand, supports, and plate slices, or show a warning while the target respawns.
std::vector<SceneObject> createTargetObjects(const Target& target, std::size_t index);
}
