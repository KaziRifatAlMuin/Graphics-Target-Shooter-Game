#include "Human.h"
#include "Collision.h"
#include <algorithm>
#include <stdexcept>
namespace shooter {
namespace {
bool clearHumanPosition(Vec3 p,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    if (!canStandAt(p,obstacles)) return false;
    for (const auto& t:targets) if (std::abs(p.x-t.position.x)<1.4f && std::abs(p.z-t.position.z)<1.2f) return false;
    return true;
}
void placeHuman(Human& h,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    for (int tries=0;tries<512;++tries) {
        const auto candidate=npcDestination(h,false);
        if (clearHumanPosition(candidate,obstacles,targets)) { h.position=candidate; return; }
    }
    throw std::runtime_error("Human target zone has no safe standing position.");
}
}
Human createHuman(std::size_t zone, Vec3 home, unsigned seed, const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    Human h; h.zone=zone; h.home=home; h.random=seed; h.speed=.7f+npcRandom(h)*.8f;
    h.animation=npcRandom(h)*6;
    placeHuman(h,obstacles,targets);
    h.destination=npcDestination(h,false); return h;
}
void updateHuman(Human& h, float dt, const std::vector<SceneObject>& obstacles, const std::vector<Target>& targets) {
    if (!h.active) return;
    // Moving stands are kinematic obstacles. Resolve overlap before the walk step.
    if (!clearHumanPosition(h.position,obstacles,targets)) {
        bool resolved=false;
        // Prefer a small outward displacement when a moving stand reaches a walker.
        for (const auto& t:targets) {
            for (const Vec3 candidate:std::array<Vec3,4>{{
                {t.position.x-1.41f,0,h.position.z},{t.position.x+1.41f,0,h.position.z},
                {h.position.x,0,t.position.z-1.21f},{h.position.x,0,t.position.z+1.21f}}}) {
                const auto d=candidate-h.home;
                if (!resolved && d.x*d.x+d.z*d.z<=9 && length(candidate-h.position)<.6f && clearHumanPosition(candidate,obstacles,targets)) {
                    h.position=candidate; resolved=true;
                }
            }
        }
        if (!resolved) placeHuman(h,obstacles,targets);
    }
    if (h.pause>0) { h.pause-=dt; return; }
    h.animation+=dt*h.speed*5;
    if (length(h.destination-h.position)<.2f) { h.destination=npcDestination(h,false); h.pause=npcRandom(h)*1.3f; return; }
    const Vec3 velocity=normalize(h.destination-h.position)*h.speed;
    const Vec3 next=h.position+velocity*dt;
    const bool valid=clearHumanPosition(next,obstacles,targets);
    if (!valid) { h.destination=npcDestination(h,false); h.pause=.1f+npcRandom(h)*.3f; }
    else { h.position=next; h.heading=std::atan2(-velocity.x,-velocity.z)*180/pi; }
}
std::vector<SceneObject> createHumanObjects(const Human& h, std::size_t index) {
    if (!h.active) return {};
    const auto id="HUMAN_"+std::to_string(index);
    const Vec3 skin{.65f,.47f,.33f}, trousers{.15f,.20f,.28f};
    const Vec3 shirts[]={{.76f,.48f,.16f},{.21f,.47f,.53f},{.56f,.30f,.28f}};
    const Vec3 shirt=shirts[index%3];
    const float walk=h.pause>0?0:std::sin(h.animation)*22;
    std::vector<SceneObject> parts;
    auto add=[&](std::string name,Vec3 p,Vec3 s,Vec3 c,float rx=0) { parts.push_back(npcPart(h,id,"Human",name,p,s,c,rx)); };
    add("Torso",{0,1.12f,0},{.46f,.58f,.25f},shirt);
    add("Head",{0,1.61f,0},{.29f,.34f,.27f},skin);
    add("Hair",{0,1.78f,.02f},{.31f,.08f,.28f},{.15f,.11f,.08f});
    for (int side:{-1,1}) {
        const auto suffix=std::to_string(side); const float swing=walk*side;
        add("UpperArm"+suffix,{side*.31f,1.21f,0},{.14f,.31f,.16f},shirt,-swing);
        add("Forearm"+suffix,{side*.32f,.93f,-std::sin(radians(swing))*.19f},{.12f,.29f,.13f},skin,-swing);
        add("Thigh"+suffix,{side*.14f,.62f,0},{.18f,.38f,.21f},trousers,swing);
        add("Shin"+suffix,{side*.14f,.26f,std::sin(radians(swing))*.22f},{.15f,.35f,.17f},trousers,-swing*.5f);
        add("Shoe"+suffix,{side*.14f,.075f,-.04f+std::sin(radians(swing))*.22f},{.18f,.12f,.29f},{.10f,.11f,.12f});
    }
    return parts;
}
}
