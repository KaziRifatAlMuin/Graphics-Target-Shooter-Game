#include "Lighting.h"

namespace shooter {
LightingRig createLighting(bool night) {
    LightingRig rig;
    rig.ambient=night?Vec3{.055f,.07f,.10f}:Vec3{.30f,.32f,.35f};
    rig.sunDirection=normalize({.45f,.8f,.3f});
    rig.sunColor=night?Vec3{.07f,.09f,.16f}:Vec3{.95f,.88f,.73f};
    for (float z:{-12.0f,-38.0f,-64.0f,-90.0f}) for (float x:{-23.0f,23.0f})
        rig.points.push_back({{x,6.7f,z},night?Vec3{.65f,.54f,.36f}:Vec3{}});
    for (float z:{-12.0f,-42.0f,-72.0f}) for (float x:{-11.0f,11.0f})
        rig.spots.push_back({{x,9,z},normalize({x<0?.22f:-.22f,-.55f,-1}),
            night?Vec3{1.0f,1.1f,1.25f}:Vec3{},std::cos(radians(22)),std::cos(radians(34))});
    return rig;
}
std::vector<SceneObject> createLightFixtures() {
    std::vector<SceneObject> objects;
    auto add=[&](std::string id,std::string component,Vec3 position,Vec3 scale,Vec3 color) {
        Transform t; t.position=position; t.scale=scale;
        SceneObject o{id,"Lighting",component,t,color,"Receives Blinn-Phong light; fixture position shared with light rig"};
        o.specular=.65f; o.shininess=64; objects.push_back(o);
    };
    const auto rig=createLighting(false);
    int i=0;
    for (auto& light:rig.points) {
        const auto id=std::to_string(++i); const Vec3 p=light.position;
        add("LAMP_POLE_"+id,"Lamp pole",{p.x,3.25f,p.z},{.22f,6.5f,.22f},{.20f,.24f,.29f});
        add("LAMP_HEAD_"+id,"Point lamp head",p,{.85f,.4f,.85f},{1,.82f,.45f});
        add("LAMP_CAP_"+id,"Lamp cap",p+Vec3{0,.3f,0},{1,.18f,1},{.18f,.22f,.28f});
    }
    i=0;
    for (auto& light:rig.spots) {
        const auto id=std::to_string(++i); const Vec3 p=light.position;
        add("SPOT_POLE_"+id,"Spotlight pole",{p.x,4.4f,p.z},{.16f,8.8f,.16f},{.22f,.28f,.34f});
        add("SPOT_HEAD_"+id,"Spotlight head",p,{1.1f,.4f,.65f},{.66f,.82f,1});
        objects.back().transform.rotation.x=-28;
    }
    return objects;
}
std::vector<SceneObject> createCelestialObjects(bool night) {
    Transform t; t.position={25,35,-80}; t.scale=night?Vec3{3,3,3}:Vec3{4,4,4}; t.rotation={0,25,15};
    SceneObject body{night?"MOON_VIS":"SUN_VIS","Environment",night?"Moon visual":"Sun visual",t,
        night?Vec3{.55f,.67f,.90f}:Vec3{1,.82f,.38f},night?"Emissive moon; dim directional fill":"Emissive sun; daytime directional source"};
    body.emission=1;
    return {body};
}
}
