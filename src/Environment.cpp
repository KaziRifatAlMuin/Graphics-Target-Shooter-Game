#include "Environment.h"
#include "Weapon.h"
#include "Projectile.h"

namespace shooter {
std::vector<SceneObject> createEquipmentDisplay() {
    std::vector<SceneObject> result;
    result.push_back(makeCube("EQUIPMENT_TABLE","Environment","Equipment table",{-9,.55f,-8},
        {12,1.1f,3},{.28f,.32f,.34f}));
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
    return {body};
}
}
