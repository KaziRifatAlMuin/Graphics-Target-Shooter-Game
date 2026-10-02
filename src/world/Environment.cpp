#include "world/Environment.h"
#include "gameplay/Weapon.h"
#include "gameplay/Projectile.h"

namespace shooter {
std::vector<SceneObject> createEquipmentDisplay() {
    std::vector<SceneObject> result;
    result.push_back(makeCube("EQUIPMENT_TABLE","Environment","Equipment table",{-9,.55f,-8},
        {12,1.1f,3},{.28f,.32f,.34f}));
    result.push_back(makeCube("EQUIPMENT_SURFACE","Environment","Equipment mat",{-9,1.11f,-8},
        {11.8f,.02f,2.8f},{.16f,.21f,.23f}));
    for (int slot=0;slot<3;++slot) {
        auto label=makeCube("EQUIPMENT_LABEL_"+std::to_string(slot),"Environment","Display identification plate",
            {-13.f+slot*4,1.13f,-6.8f},{1.1f,.025f,.22f},{.82f,.65f,.28f});
        label.parent="EQUIPMENT_TABLE"; result.push_back(label);
    }
    int index=0;
    for (auto type:{WeaponType::Pistol,WeaponType::Shotgun,WeaponType::Rifle}) {
        Camera pose; pose.position={-13.0f+index*4,1.65f,-8}; pose.yaw=-90; pose.pitch=0;
        auto parts=createWeapon(type,pose);
        for (auto& part:parts) {
            part.id="DISPLAY_"+part.id;
            part.notes="Stationary equipment exhibit near spawn; actual rendered weapon assembled by createWeapon";
            result.push_back(part);
        }
        Projectile shape{0,type,pose.position+Vec3{1,-.48f,-.7f},{0,0,-1},0,0};
        auto visual=projectileObject(shape);
        visual.id="DISPLAY_PROJECTILE_"+std::to_string(index++);
        visual.notes="Stationary projectile shape on equipment table; rendered with the same builder/scale as fired projectiles";
        result.push_back(visual);
    }
    return result;
}
std::vector<SceneObject> createCelestialObjects(bool night) {
    Transform t; t.position={25,35,-80}; t.scale=night?Vec3{3,3,3}:Vec3{4,4,4}; t.rotation={0,25,15};
    SceneObject body{night?"MOON_VIS":"SUN_VIS","Environment",night?"Moon visual":"Sun visual",t,
        night?Vec3{.55f,.67f,.90f}:Vec3{1,.82f,.38f},night?"Emissive moon; dim directional fill":"Emissive sun; daytime directional source"};
    body.emission=1;
    body.parent=night?"MOON":"SUN";
    std::vector<SceneObject> objects{body};
    if (night) for (int i=0;i<3;++i) {
        auto patch=makeCube("MOON_PATCH_"+std::to_string(i),"Environment","Moon surface patch",
            {24.2f+i*.7f,34.4f+i*.5f,-78.25f},{.45f,.4f,.1f},{.35f,.43f,.62f},25);
        patch.emission=.6f; patch.parent="MOON"; objects.push_back(patch);
    }
    return objects;
}
}
