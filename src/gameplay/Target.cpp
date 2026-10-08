#include "gameplay/Target.h"
#include "core/Collision.h"
#include <algorithm>
#include <limits>

namespace shooter {
namespace {
constexpr int sliceCount=32;
// Thin cube slabs form a stepped circular silhouette. The same slabs drive hits.
// Approximate a circular plate with 32 horizontal cube slices, reused for rendering and hit detection.
const std::array<Transform,sliceCount>& slices() {
    static const auto shapes=[] {
        std::array<Transform,sliceCount> result{};
        const float height=2*Target::radius/sliceCount;
        for (int i=0;i<sliceCount;++i) {
            const float y=-Target::radius+(i+.5f)*height;
            // Circle equation x^2+y^2=r^2 gives slice width=2*sqrt(r^2-y^2).
            const float width=2*std::sqrt(Target::radius*Target::radius-y*y);
            result[i].position={0,y,0};
            result[i].scale={width,height,Target::thickness};
        }
        return result;
    }();
    return shapes;
}
}
// Ring damage=60/(ring+1), so inner-to-outer rings need 1-6 separate hits against 60 health.
int ringDamage(int ring) { return ring>=0 && ring<6?60/(ring+1):0; }

// Undo target translation/yaw, test its slices, and score only entry through the printed front face.
TargetContact intersectTarget(Vec3 origin, Vec3 direction, const Target& target, float maximum) {
    const auto inverse=makeRotationY(-target.yaw);
    const auto offset=origin-target.position;
    const Vec3 p=asVec3(transformPoint(inverse,{offset.x,offset.y,offset.z,1}));
    const Vec3 d=asVec3(transformPoint(inverse,{direction.x,direction.y,direction.z,0}));
    TargetContact result{std::numeric_limits<float>::infinity(),-1};
    // Cheap broad phase before exact cube-slice intersections.
    if (std::abs(p.x)>Target::radius+maximum || std::abs(p.y)>Target::radius+maximum) return result;
    for (const auto& slice:slices()) {
        const float distance=intersectCube(p,d,slice,maximum);
        if (distance>=result.distance) continue;
        const Vec3 hit=p+d*distance;
        const bool front=d.z<0 && p.z>=Target::thickness/2 &&
            std::abs(hit.z-Target::thickness/2)<.0001f;
        // Hit radius=sqrt(x*x+y*y); ring=min(5,floor(6*radius/targetRadius)).
        const float radial=std::sqrt(hit.x*hit.x+hit.y*hit.y);
        result={distance,front?std::min(5,int(radial/Target::radius*6)):-1};
    }
    return result;
}
// Create seven practice targets with fixed starting positions and varied motion.
std::vector<Target> createSandboxTargets() {
    const Vec3 positions[]={{0,2.7f,-19},{-8,3,-29},{8,4.4f,-38},{-6,3.5f,-52},
                            {8,5,-65},{0,3.7f,-82},{5,2.2f,-16}};
    std::vector<Target> targets;
    for (int i=0;i<7;++i) {
        Target t; t.base=t.position=positions[i]; t.movement=i==6?3:i%3; t.phase=i*.8f;
        targets.push_back(t);
    }
    return targets;
}
// Advance hit/respawn timers and evaluate position or spin from elapsed simulation time.
void updateTarget(Target& t, float elapsed, float dt) {
    if (t.eliminated) return;
    t.hitTime=std::max(0.0f,t.hitTime-dt);
    if (t.respawn>0) {
        t.respawn=std::max(0.0f,t.respawn-dt);
        if (t.respawn==0) { t.health=60; t.damageByShot.clear(); }
    }
    t.position=t.base;
    // Sine motion stays within +/-amplitude; spin wraps at 360 degrees using the remainder.
    if (t.movement==0) t.position.x+=std::sin(elapsed*2.1f+t.phase)*4;
    if (t.movement==1) t.position.y+=std::sin(elapsed*2.8f+t.phase)*.8f;
    if (t.movement==2) t.yaw=std::fmod(elapsed*140+t.phase*30,360.0f);
    if (t.movement==4) {
        t.position=t.base+movementOffset(t.motion,elapsed);
        t.yaw=std::fmod(elapsed*t.motion.spinSpeed,360.0f);
    }
}
// Build the stand, supports, and plate slices, or show a warning while the target respawns.
std::vector<SceneObject> createTargetObjects(const Target& t, std::size_t index) {
    std::vector<SceneObject> objects;
    const auto id="TARGET_"+std::to_string(index+1);
    objects.push_back(makeCube(id+"_BASE","Target","Stand base",{t.position.x,.15f,t.position.z},{2,.3f,1.5f},{.25f,.29f,.32f}));
    const float height=std::max(.3f,t.position.y-Target::radius);
    objects.push_back(makeCube(id+"_STAND","Target","Stand",{t.position.x,height/2,t.position.z},{.24f,height,.24f},{.33f,.37f,.39f}));
    for (int side:{-1,1}) {
        auto brace=makeCube(id+"_BRACE_"+std::to_string(side),"Target","Stand brace",
            {t.position.x+side*.28f,height*.29f+.2f,t.position.z},{.10f,height*.56f,.12f},{.18f,.22f,.25f});
        brace.transform.rotation.z=side*18.f; objects.push_back(brace);
        objects.push_back(makeCube(id+"_FOOT_STRIPE_"+std::to_string(side),"Target","Base warning stripe",
            {t.position.x+side*.7f,.307f,t.position.z},{.16f,.012f,1.36f},{.91f,.62f,.16f}));
    }
    if (t.respawn>0 || t.eliminated) {
        if (!t.eliminated && t.respawn<=3 && t.movement==4) {
            const float pulse=.5f+.5f*std::sin(t.respawn*12);
            auto beacon=makeCube(id+"_RESPAWN","Target","Respawn warning",t.position,
                {.15f+pulse*.15f,.15f+pulse*.15f,.15f+pulse*.15f},{1,.7f,.15f},t.yaw);
            beacon.emission=1; beacon.notes="Inactive target; respawns in "+std::to_string(t.respawn)+" seconds; warning is not hittable";
            objects.push_back(beacon);
        }
        for (auto& object:objects) object.parent=id;
        return objects;
    }
    int i=0;
    for (const auto& slice:slices()) {
        auto part=makeCube(id+"_SLICE_"+std::to_string(i++),"Target","Cube-built six-ring plate",
            t.position+slice.position,slice.scale,{.8f,.8f,.8f},t.yaw);
        part.targetPattern=true; part.specular=.45f; part.shininess=48; part.flash=t.hitTime/.25f;
        // Map every slice into the same full-plate coordinates so ring colors join across slice boundaries.
        part.patternScale={slice.scale.x/(2*Target::radius),slice.scale.y/(2*Target::radius),1};
        part.patternOffset={0,slice.position.y/(2*Target::radius),0};
        part.notes="32 transformed cube slices; printed front +Z; health="+std::to_string(t.health)+"/60; exact slice collider";
        if (t.movement==4) part.notes+="; configured bounded sinusoidal X/Y/Z movement; spin degrees/sec="+std::to_string(t.motion.spinSpeed);
        objects.push_back(part);
    }
    for (auto& object:objects) object.parent=id;
    return objects;
}
}
