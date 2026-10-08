#pragma once
#include "core/SceneObject.h"
#include "gameplay/Target.h"
#include <unordered_set>
namespace shooter {
// Shared bird/human state: home zone, motion, seeded randomness, and alive/falling/dead lifecycle.
struct NpcState {
    std::size_t zone=0;
    Vec3 home, position, destination;
    float speed=1, heading=0, animation=0, pause=0;
    unsigned random=1;
    bool active=true;
    bool dying=false;
    bool dead=false;
    float deathTime=0;
    Vec3 deathVelocity{0,0,0};
    float deathPitch=0;
    std::unordered_set<std::uint64_t> penalizedShots;
};
// Lift the whole assembly until its lowest transformed cube touches the ground.
void groundNpcParts(std::vector<SceneObject>& parts,float ground=.015f);
// Repeatable random sequence: seed=1664525*seed+1013904223 (unsigned wrap), then map selected bits to [0,1].
float npcRandom(NpcState& npc);
// Choose a nearby waypoint: x=home.x+r*cos(angle), z=home.z+r*sin(angle), with bounded flight height.
Vec3 npcDestination(NpcState& npc, bool flying);
// Place a body part with worldOffset=Ry(heading)*Rx(fall)*localOffset, then add the character's position.
SceneObject npcPart(const NpcState& npc, const std::string& id, const std::string& type,
                    const std::string& component, Vec3 offset, Vec3 size, Vec3 color, float rotationX=0);
// Group the transformed cube parts that can be hit on one live character.
struct NpcCollider { bool human=false; std::size_t index=0; std::vector<Transform> parts; };
// Store the nearest hit distance and character index; index -1 means no character was hit.
struct NpcContact { float distance; int collider=-1; };
// Test each character's cube parts and return the nearest ray contact.
NpcContact intersectNpcs(Vec3 origin, Vec3 direction, float maximum, const std::vector<NpcCollider>& colliders);
}
