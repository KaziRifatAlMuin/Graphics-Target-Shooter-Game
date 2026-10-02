#pragma once
#include "core/SceneObject.h"
#include "gameplay/Target.h"
#include <unordered_set>
namespace shooter {
struct NpcState {
    std::size_t zone=0;
    Vec3 home, position, destination;
    float speed=1, heading=0, animation=0, pause=0;
    unsigned random=1;
    bool active=true;
    std::unordered_set<std::uint64_t> penalizedShots;
};
float npcRandom(NpcState& npc);
Vec3 npcDestination(NpcState& npc, bool flying);
SceneObject npcPart(const NpcState& npc, const std::string& id, const std::string& type,
                    const std::string& component, Vec3 offset, Vec3 size, Vec3 color, float rotationX=0);
struct NpcCollider { bool human=false; std::size_t index=0; std::vector<Transform> parts; };
struct NpcContact { float distance; int collider=-1; };
NpcContact intersectNpcs(Vec3 origin, Vec3 direction, float maximum, const std::vector<NpcCollider>& colliders);
}
