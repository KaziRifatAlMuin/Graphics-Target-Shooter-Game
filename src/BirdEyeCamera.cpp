#include "BirdEyeCamera.h"
#include "Collision.h"
#include <algorithm>
namespace shooter {
BirdEyeCamera::BirdEyeCamera() { reset(); }
void BirdEyeCamera::reset() {
    overhead.position={0,120,-50}; overhead.yaw=-90; overhead.pitch=-89.9f;
    observation.position={0,1.7f,-5}; observation.yaw=-90; observation.pitch=0; observing=false;
}
void BirdEyeCamera::overview() { observing=false; }
void BirdEyeCamera::pan(float f,float r,float dt) {
    if (observing) return;
    overhead.position.x=std::clamp(overhead.position.x+r*25*dt,-25.f,25.f);
    overhead.position.z=std::clamp(overhead.position.z-f*25*dt,-90.f,-10.f);
}
void BirdEyeCamera::zoom(float steps) {
    if (!observing) overhead.position.y=std::clamp(overhead.position.y-steps*8,65.f,180.f);
}
std::optional<Vec3> BirdEyeCamera::groundPoint(float u,float v,float aspect) const {
    if (u<0 || u>1 || v<0 || v>1 || aspect<=0) return {};
    // Invert the same 60-degree perspective/view mapping used by Renderer.
    const auto forward=overhead.forward(),right=normalize(cross(forward,{0,1,0})),up=cross(right,forward);
    const float half=std::tan(radians(60)*.5f);
    const auto ray=normalize(forward+right*((2*u-1)*aspect*half)+up*((1-2*v)*half));
    if (ray.y>=-.0001f) return {};
    const auto p=overhead.position+ray*(-overhead.position.y/ray.y);
    if (!std::isfinite(p.x) || !std::isfinite(p.z)) return {};
    return Vec3{p.x,0,p.z};
}
bool BirdEyeCamera::validPosition(Vec3 p,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) const {
    if (!canStandAt(p,obstacles)) return false;
    for (const auto& t:targets) if (std::abs(p.x-t.position.x)<1.45f && std::abs(p.z-t.position.z)<1.25f) return false;
    return true;
}
bool BirdEyeCamera::observeAt(float u,float v,float aspect,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    const auto point=groundPoint(u,v,aspect);
    if (!point || !validPosition(*point,obstacles,targets)) return false;
    const auto direction=normalize(*point-overhead.position); const float distance=length(*point-overhead.position);
    for (const auto& o:obstacles) if (o.type!="Arena" && intersectCube(overhead.position,direction,o.transform,distance)<distance) return false;
    observation.position=*point+Vec3{0,1.7f,0}; observation.yaw=overhead.yaw; observation.pitch=0;
    observing=true; return true;
}
void BirdEyeCamera::moveObservation(float f,float r,float dt,bool fast,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    auto forward=observation.forward(); forward.y=0; forward=normalize(forward);
    const auto delta=normalize(forward*f+cross(forward,{0,1,0})*r)*((fast?9.f:5.f)*dt);
    const int count=std::max(1,int(std::ceil(length(delta)/.15f)));
    for (int i=0;i<count;++i) {
        auto next=observation.position; next.x+=delta.x/count;
        if (validPosition(next,obstacles,targets)) observation.position=next;
        next=observation.position; next.z+=delta.z/count;
        if (validPosition(next,obstacles,targets)) observation.position=next;
    }
}
std::vector<SceneObject> BirdEyeCamera::referenceObjects() const {
    // This visible marker is anchored to the real observation camera, not a CSV-only sample.
    auto base=makeCube("OBSERVATION_MARKER","Camera reference","Observation ground marker",
        {observation.position.x,.045f,observation.position.z},{.9f,.08f,.9f},{.2f,.9f,.7f});
    base.notes="Actual observation camera at ("+std::to_string(observation.position.x)+","+
        std::to_string(observation.position.y)+","+std::to_string(observation.position.z)+
        "); yaw="+std::to_string(observation.yaw)+"; pitch="+std::to_string(observation.pitch);
    auto heading=makeCube("OBSERVATION_HEADING","Camera reference","Observation heading",
        base.transform.position+observation.forward()*.4f+Vec3{0,.09f,0},{.12f,.1f,.75f},{1,.7f,.2f},-observation.yaw-90);
    heading.transform.rotation.x=observation.pitch; heading.notes=base.notes;
    return {base,heading};
}
}
