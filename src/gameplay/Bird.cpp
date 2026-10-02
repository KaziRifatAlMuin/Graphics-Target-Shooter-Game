#include "gameplay/Bird.h"
#include "core/Collision.h"
#include <algorithm>
namespace shooter {
Bird createBird(std::size_t zone, Vec3 home, unsigned seed) {
    Bird b; b.zone=zone; b.home=home; b.random=seed;
    b.speed=1.8f+npcRandom(b)*2.3f; b.animation=npcRandom(b)*6;
    b.position=npcDestination(b,true); b.destination=npcDestination(b,true);
    return b;
}
void updateBird(Bird& b, float dt, const std::vector<SceneObject>& obstacles) {
    if (!b.active) return;
    b.animation+=dt*(8+b.speed);
    const Vec3 delta=b.destination-b.position;
    if (length(delta)<.2f) b.destination=npcDestination(b,true);
    const Vec3 velocity=normalize(b.destination-b.position)*b.speed;
    const Vec3 next=b.position+velocity*dt;
    bool blocked=false;
    // A short look-ahead steers birds away from cargo and wall fixtures.
    for (const auto& o:obstacles) {
        if (length(o.transform.position-b.position)>5) continue;
        if (std::isfinite(intersectCube(b.position,normalize(velocity),o.transform,b.speed*dt+.5f))) { blocked=true; break; }
    }
    if (blocked) b.destination=npcDestination(b,true);
    else { b.position=next; b.heading=std::atan2(-velocity.z,velocity.x)*180/pi; }
}
std::vector<SceneObject> createBirdObjects(const Bird& b, std::size_t index) {
    if (!b.active) return {};
    const auto id="BIRD_"+std::to_string(index);
    const Vec3 feather=index%2?Vec3{.28f,.30f,.34f}:Vec3{.48f,.36f,.23f};
    std::vector<SceneObject> parts;
    auto add=[&](const char* name,Vec3 p,Vec3 s,Vec3 c,float rx=0) { parts.push_back(npcPart(b,id,"Bird",name,p,s,c,rx)); };
    const float bob=.045f*std::sin(b.animation*.5f);
    add("Body",{0,bob,0},{.62f,.26f,.28f},feather);
    add("Head",{.32f,.12f+bob,0},{.24f,.24f,.23f},{.68f,.64f,.52f});
    add("Beak",{.49f,.09f+bob,0},{.17f,.07f,.10f},{.9f,.62f,.13f});
    add("Tail",{-.4f,bob,0},{.3f,.07f,.24f},feather);
    for (int side:{-1,1}) {
        const float flap=std::sin(b.animation)*35*side;
        const float a=radians(flap);
        add(side<0?"WingLeft":"WingRight",{-.03f,bob-std::sin(a)*side*.35f,std::cos(a)*side*.35f},
            {.38f,.055f,.64f},feather,flap);
        add(side<0?"WingTipLeft":"WingTipRight",{-.09f,bob-std::sin(a)*side*.66f,std::cos(a)*side*.66f},
            {.28f,.04f,.25f},{.16f,.18f,.20f},flap);
    }
    add("Eye",{.42f,.18f+bob,.12f},{.035f,.045f,.025f},{.03f,.03f,.03f});
    add("OtherEye",{.42f,.18f+bob,-.12f},{.035f,.045f,.025f},{.03f,.03f,.03f});
    return parts;
}
}
