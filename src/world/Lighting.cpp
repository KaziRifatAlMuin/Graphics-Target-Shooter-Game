#include "world/Lighting.h"

namespace shooter {
LightingRig createLighting(bool night) {
    LightingRig rig;
    rig.ambient=night?Vec3{.012f,.016f,.025f}:Vec3{.30f,.32f,.35f};
    rig.sunDirection=normalize({.45f,.8f,.3f});
    rig.sunColor=night?Vec3{}:Vec3{.95f,.88f,.73f};
    // Two warm lamps near spawn, four boundary lamps, two wall-mounted accents.
    const Vec3 positions[]={{-18,4.7f,-12},{18,4.7f,-12},{-28.8f,5,-38},{28.8f,5,-38},
        {-28.8f,5,-68},{28.8f,5,-68},{-28.8f,3.2f,-89},{28.8f,3.2f,-89}};
    const Vec3 colors[]={{2.2f,1.65f,.9f},{1.2f,2.0f,1.3f},{1.7f,1.6f,1.4f},{1.7f,1.6f,1.4f},
        {1.7f,1.6f,1.4f},{1.7f,1.6f,1.4f},{2.0f,.25f,.15f},{.25f,.65f,2.0f}};
    for(int i=0;i<8;++i) rig.points.push_back({positions[i],night?colors[i]:Vec3{}});
    // Six stadium towers, all at the boundary, aimed down and across the arena.
    for(float z:{-16.f,-48.f,-80.f}) for(float x:{-28.f,28.f})
        rig.spots.push_back({{x,12,z},normalize({-x,-10,-4}),
            night?Vec3{1.8f,1.9f,2.1f}:Vec3{},std::cos(radians(32)),std::cos(radians(53))});
    return rig;
}
std::vector<SceneObject> createLightFixtures() {
    std::vector<SceneObject> objects;
    auto add=[&](std::string id,std::string component,Vec3 p,Vec3 scale,Vec3 color) -> SceneObject& {
        objects.push_back(makeCube(id,"Lighting",component,p,scale,color));
        auto& o=objects.back(); o.specular=.45f; o.shininess=48;
        o.notes="Mounted arena fixture; lens and illumination share the same rig position";
        return o;
    };
    const Vec3 metal{.19f,.23f,.27f};
    const auto rig=createLighting(true);
    int i=0;
    for(const auto& light:rig.points) {
        const auto id="LAMP_"+std::to_string(++i); const auto p=light.position;
        if(i<=2) {
            add(id+"_BASE","Footing",{p.x,.15f,p.z},{.8f,.3f,.8f},metal);
            add(id+"_POLE","Lamp post",{p.x,p.y/2,p.z},{.22f,p.y,.22f},metal);
        } else {
            const float wall=p.x<0?-29.5f:29.5f;
            add(id+"_MOUNT","Wall bracket",{(p.x+wall)/2,p.y-.35f,p.z},{1.3f,.18f,.25f},metal);
            add(id+"_PLATE","Wall mounting plate",{wall,p.y-.35f,p.z},{.2f,.7f,.6f},metal);
        }
        add(id+"_HOUSING","Lamp housing",p+Vec3{0,-.24f,0},{.9f,.18f,.9f},metal);
        const float peak=std::max(light.color.x,std::max(light.color.y,light.color.z));
        add(id+"_LENS","Lens",p,{.72f,.36f,.72f},light.color*(1/peak));
        add(id+"_CAP","Lamp cap",p+Vec3{0,.26f,0},{.95f,.16f,.95f},metal);
    }
    i=0;
    for(const auto& light:rig.spots) {
        const auto id="FLOOD_"+std::to_string(++i); const auto p=light.position;
        add(id+"_BASE","Concrete footing",{p.x,.25f,p.z},{1.3f,.5f,1.3f},{.35f,.37f,.39f});
        add(id+"_POLE","Stadium tower",{p.x,6,p.z},{.38f,12,.38f},metal);
        add(id+"_BAR","Floodlight crossbar",p,{2.9f,.18f,.25f},metal);
        const Vec3 rotation{-std::asin(light.direction.y)*180/pi,std::atan2(light.direction.x,light.direction.z)*180/pi,0};
        for(int panel=-1;panel<=1;++panel) {
            const Vec3 center=p+Vec3{0,0,float(panel)*.95f};
            add(id+"_BRACKET_"+std::to_string(panel),"Head support",(p+center)*.5f,{.18f,.22f,2.1f},metal);
            auto& head=add(id+"_HEAD_"+std::to_string(panel),"Floodlight housing",center,{.85f,.72f,.32f},metal);
            head.transform.rotation=rotation;
            auto& lens=add(id+"_LENS_"+std::to_string(panel),"Lens",center+light.direction*.18f,{.73f,.60f,.035f},{.86f,.93f,1});
            lens.transform.rotation=rotation;
        }
    }
    return objects;
}
}
