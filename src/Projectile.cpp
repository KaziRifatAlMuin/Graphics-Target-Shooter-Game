#include "Projectile.h"
#include <algorithm>
#include "Collision.h"

namespace shooter {
void updateProjectiles(std::vector<Projectile>& projectiles, const std::vector<Target>& targets,
                       const std::vector<SceneObject>& obstacles, float dt, const TargetHitCallback& hit) {
    for (auto& p:projectiles) {
        const auto& spec=weaponSpec(p.weapon);
        const float travel=std::min(spec.speed*dt,spec.range-p.travelled);
        float nearest=travel; int hitTarget=-1,hitRing=-1; bool blocked=false;
        for (const auto& o:obstacles) {
            const float d=intersectCube(p.position,p.direction,o.transform,nearest);
            if (d<=nearest) { nearest=d; blocked=true; }
        }
        for (std::size_t i=0;i<targets.size();++i) if (targets[i].respawn<=0) {
            const auto contact=intersectTarget(p.position,p.direction,targets[i],nearest);
            if (contact.distance<nearest) { nearest=contact.distance; hitTarget=int(i); hitRing=contact.ring; blocked=true; }
        }
        p.position=p.position+p.direction*nearest;
        p.travelled+=nearest;
        if (hitTarget>=0 && hitRing>=0) hit(std::size_t(hitTarget),hitRing,p.shotId);
        if (blocked) p.travelled=spec.range;
    }
    projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),[](const Projectile& p) {
        return p.travelled>=weaponSpec(p.weapon).range-.0001f;
    }),projectiles.end());
}
SceneObject projectileObject(const Projectile& p) {
    auto o=makeCube("PROJECTILE_"+std::to_string(p.id),"Projectile",weaponSpec(p.weapon).name,
                p.position,weaponSpec(p.weapon).projectileScale,{1,.8f,.22f});
    o.transform.rotation={std::asin(std::clamp(p.direction.y,-1.0f,1.0f))*180/pi,
                          std::atan2(-p.direction.x,-p.direction.z)*180/pi,0};
    o.emission=.6f; o.specular=.8f;
    return o;
}
}
