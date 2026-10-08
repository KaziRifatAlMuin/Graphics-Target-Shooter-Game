#include "gameplay/Human.h"
#include "core/Collision.h"
#include <algorithm>
#include <stdexcept>
namespace shooter {
namespace {
// Choose walking destinations near the target, often crossing its firing lane.
Vec3 humanDestination(Human& h) {
    // Cross the near side of the target zone often enough to obscure a firing lane.
    // Other waypoints still cover the full bounded neighborhood, so waiting/repositioning works.
    if (npcRandom(h)<.55f) return {h.home.x+(npcRandom(h)-.5f)*3.6f,0,h.home.z+1.6f+npcRandom(h)*.7f};
    return npcDestination(h,false);
}
// Check both solid-world clearance and the rectangular space occupied by each target stand.
bool clearHumanPosition(Vec3 p,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    if (!canStandAt(p,obstacles)) return false;
    for (const auto& t:targets) if (std::abs(p.x-t.position.x)<1.4f && std::abs(p.z-t.position.z)<1.2f) return false;
    return true;
}
// Try up to 512 repeatable random positions, failing explicitly if no safe placement exists.
void placeHuman(Human& h,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    for (int tries=0;tries<512;++tries) {
        const auto candidate=npcDestination(h,false);
        if (clearHumanPosition(candidate,obstacles,targets)) { h.position=candidate; return; }
    }
    throw std::runtime_error("Human target zone has no safe standing position.");
}
}
// Create a walker with seeded speed/height variation and a collision-free starting position.
Human createHuman(std::size_t zone, Vec3 home, unsigned seed, const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    Human h; h.zone=zone; h.home=home; h.random=seed; h.speed=.7f+npcRandom(h)*.8f;
    h.animation=npcRandom(h)*6;
    h.height=dimensions::humanMinHeight+npcRandom(h)*(dimensions::humanMaxHeight-dimensions::humanMinHeight);
    placeHuman(h,obstacles,targets);
    if (seed%3==0 && zone<targets.size()) {
        const auto candidate=targets[zone].position+Vec3{0,-targets[zone].position.y,2.2f};
        auto offset=candidate-h.home; offset.y=0;
        if (length(offset)<=3 && clearHumanPosition(candidate,obstacles,targets)) h.position=candidate;
    }
    h.destination=humanDestination(h); return h;
}
// Handle falling, resolve overlaps with moving stands, and walk toward bounded waypoints.
void updateHuman(Human& h, float dt, const std::vector<SceneObject>& obstacles, const std::vector<Target>& targets) {
    if (h.dying) {
        h.deathTime+=dt;
        const float progress=std::min(1.f,h.deathTime/.75f);
        // Falling angle=90*progress^2 gives a fall that starts slowly and accelerates.
        h.deathPitch=90*progress*progress;
        if (h.deathTime>=0.75f) {
            h.dying=false;
            h.dead=true;
        }
        return;
    }
    if (h.dead) return;
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
    if (length(h.destination-h.position)<.2f) { h.destination=humanDestination(h); h.pause=npcRandom(h)*1.3f; return; }
    const Vec3 velocity=normalize(h.destination-h.position)*h.speed;
    const Vec3 next=h.position+velocity*dt;
    const bool valid=clearHumanPosition(next,obstacles,targets);
    if (!valid) { h.destination=humanDestination(h); h.pause=.1f+npcRandom(h)*.3f; }
    else { h.position=next; h.heading=std::atan2(-velocity.x,-velocity.z)*180/pi; }
}
// Assemble a proportionally scaled cube person with opposite arm and leg swings.
std::vector<SceneObject> createHumanObjects(const Human& h, std::size_t index) {
    if (!h.active && !h.dying && !h.dead) return {};
    const auto id=(h.dead?"DEAD_HUMAN_":h.dying?"DYING_HUMAN_":"HUMAN_")+std::to_string(index);
    const Vec3 skins[]={{.65f,.47f,.33f},{.84f,.66f,.49f},{.40f,.27f,.19f}};
    const Vec3 skin=skins[(index/3)%3], trousers{.15f,.20f,.28f};
    const Vec3 shirts[]={{.76f,.48f,.16f},{.21f,.47f,.53f},{.56f,.30f,.28f}};
    const Vec3 shirt=shirts[index%3];
    // Walking swing=22*sin(animation) degrees; stop swinging while paused or fallen.
    const float walk=(h.dying || h.dead)?0.0f:(h.pause>0?0:std::sin(h.animation)*22);
    std::vector<SceneObject> parts;
    const float proportion=h.height/1.82f;
    auto add=[&](std::string name,Vec3 p,Vec3 s,Vec3 c,float rx=0) {
        parts.push_back(npcPart(h,id,"Human",name,p*proportion,s*proportion,c,rx));
        parts.back().notes+="; standing height="+std::to_string(h.height)+(h.active?" m; live cube projectile collider":" m; cosmetic fallen body; no collider");
        parts.back().specular=.06f;
    };
    add("Torso",{0,1.12f,0},{.46f,.58f,.25f},shirt);
    add("Head",{0,1.61f,0},{.29f,.34f,.27f},skin);
    add("Hair",{0,1.78f,.02f},{.31f,.08f,.28f},{.15f,.11f,.08f});
    add("Neck",{0,1.43f,0},{.16f,.13f,.17f},skin);
    add("Belt",{0,.845f,0},{.47f,.055f,.26f},{.11f,.13f,.15f});
    add("VestStripe",{0,1.10f,-.135f},{.44f,.075f,.025f},{.84f,.82f,.43f});
    add("Nose",{0,1.60f,-.16f},{.055f,.075f,.07f},skin);
    for (int side:{-1,1}) {
        const auto suffix=std::to_string(side); const float swing=walk*side;
        add("UpperArm"+suffix,{side*.31f,1.21f,0},{.14f,.31f,.16f},shirt,-swing);
        add("Forearm"+suffix,{side*.32f,.93f,-std::sin(radians(swing))*.19f},{.12f,.29f,.13f},skin,-swing);
        add("Hand"+suffix,{side*.32f,.75f,-std::sin(radians(swing))*.22f},{.115f,.10f,.13f},skin);
        add("Eye"+suffix,{side*.072f,1.67f,-.14f},{.035f,.025f,.02f},{.07f,.06f,.05f});
        add("Thigh"+suffix,{side*.14f,.62f,0},{.18f,.38f,.21f},trousers,swing);
        add("Shin"+suffix,{side*.14f,.26f,std::sin(radians(swing))*.22f},{.15f,.35f,.17f},trousers,-swing*.5f);
        add("Shoe"+suffix,{side*.14f,.075f,-.04f+std::sin(radians(swing))*.22f},{.18f,.12f,.29f},{.10f,.11f,.12f});
    }
    if(h.dying || h.dead) groundNpcParts(parts);
    return parts;
}
}
