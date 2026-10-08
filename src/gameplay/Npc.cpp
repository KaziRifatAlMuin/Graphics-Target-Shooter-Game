#include "gameplay/Npc.h"
#include "core/Collision.h"
#include <algorithm>
#include <limits>
namespace shooter {
// Repeatable random sequence: seed=1664525*seed+1013904223 (unsigned wrap), then map selected bits to [0,1].
float npcRandom(NpcState& n) {
    n.random=n.random*1664525u+1013904223u;
    return float((n.random>>8)&65535)/65535;
}
// Choose a nearby waypoint: x=home.x+r*cos(angle), z=home.z+r*sin(angle), with bounded flight height.
Vec3 npcDestination(NpcState& n, bool flying) {
    const float radius=flying?3.5f:3.0f;
    const float angle=npcRandom(n)*2*pi, distance=1.4f+npcRandom(n)*(radius-1.4f);
    return {std::clamp(n.home.x+std::cos(angle)*distance,-24.0f,24.0f),
        flying?std::clamp(n.home.y-1.0f+npcRandom(n)*3.0f,1.3f,7.0f):0,
        std::clamp(n.home.z+std::sin(angle)*distance,-94.0f,-13.0f)};
}
// Place a body part with worldOffset=Ry(heading)*Rx(fall)*localOffset, then add the character's position.
SceneObject npcPart(const NpcState& n, const std::string& id, const std::string& type,
                   const std::string& component, Vec3 offset, Vec3 size, Vec3 color, float rx) {
    const float fall=(n.dying || n.dead)?n.deathPitch:0;
    const auto p=asVec3(transformPoint(makeRotationY(n.heading)*makeRotationX(fall),{offset.x,offset.y,offset.z,0}));
    auto part=makeCube(id+"_"+component,type,component,n.position+p,size,color,n.heading);
    part.transform.rotation.x=rx+fall; part.parent=id;
    part.notes="Bounded target-zone NPC; zone="+std::to_string(n.zone+1)+"; animated cube assembly";
    part.notes+="; life="+std::string(n.dead?"dead":n.dying?"falling":"alive")+"; death time="+std::to_string(n.deathTime);
    return part;
}
// Lift the whole assembly until its lowest transformed cube touches the ground.
void groundNpcParts(std::vector<SceneObject>& parts,float ground) {
    float lowest=ground;
    for (const auto& part:parts) {
        const auto model=composeModelMatrix(part.transform);
        // Lowest transformed cube corner, computed analytically from the Y row.
        const float extent=.5f*(std::abs(model.at(1,0))+std::abs(model.at(1,1))+std::abs(model.at(1,2)));
        lowest=std::min(lowest,part.transform.position.y-extent);
    }
    for (auto& part:parts) part.transform.position.y+=ground-lowest;
}
// Test each character's cube parts and return the nearest ray contact.
NpcContact intersectNpcs(Vec3 o, Vec3 d, float maximum, const std::vector<NpcCollider>& colliders) {
    NpcContact result{std::numeric_limits<float>::infinity(),-1};
    for (std::size_t i=0;i<colliders.size();++i) for (const auto& part:colliders[i].parts) {
        const float contact=intersectCube(o,d,part,std::min(maximum,result.distance));
        if (contact<result.distance) result={contact,int(i)};
    }
    return result;
}
}
