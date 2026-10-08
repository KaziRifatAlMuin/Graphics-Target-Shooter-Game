#include "gameplay/Game.h"
#include "world/Environment.h"
#include "gameplay/Effects.h"
namespace shooter {
// Combine static scenery and current animated objects into the cube list used for this frame.
std::vector<SceneObject> Game::scene(bool includePlayer) const {
    // Reserve capacity to avoid repeated reallocations. Static objects go first as a
    // block copy, then dynamic objects are appended. This is much cheaper than the
    // previous approach of copying all statics and then modifying every one.
    std::vector<SceneObject> objects;
    const std::size_t estimatedDynamic=targets.size()*38+projectiles.size()+debris.size()+20+birds.size()*8+humans.size()*12;
    objects.reserve(staticObjects.size()+estimatedDynamic);

    // Copy static objects. The lighting note annotations are deferred to calculationObjects()
    // where they are actually needed, avoiding expensive string operations every frame.
    objects=staticObjects;

    // Update lamp emission state based on day/night without building annotation strings.
    for (auto& o:objects) {
        if (o.type=="Lighting" && o.component=="Lens") {
            o.emission=night?1:0;
            if(!night) o.color={.22f,.25f,.28f};
        }
    }

    auto celestial=createCelestialObjects(night);
    objects.insert(objects.end(),celestial.begin(),celestial.end());
    for (std::size_t i=0; i<targets.size(); ++i) {
        const auto parts=createTargetObjects(targets[i],i);
        objects.insert(objects.end(),parts.begin(),parts.end());
    }
    for (std::size_t i=0;i<birds.size();++i) if (birds[i].active || birds[i].dying || birds[i].dead) {
        const auto parts=createBirdObjects(birds[i],i); objects.insert(objects.end(),parts.begin(),parts.end());
    }
    for (std::size_t i=0;i<humans.size();++i) if (humans[i].active || humans[i].dying || humans[i].dead) {
        const auto parts=createHumanObjects(humans[i],i); objects.insert(objects.end(),parts.begin(),parts.end());
    }
    if (includePlayer) {
        objects.push_back(makeCube("PLAYER_BODY","Player","Shooter body",player.position+Vec3{0,-.8f,0},{.65f,.95f,.4f},{.18f,.40f,.45f},-player.yaw-90));
        objects.push_back(makeCube("PLAYER_HEAD","Player","Shooter head",player.position+Vec3{0,.05f,0},{.4f,.4f,.4f},{.75f,.62f,.46f}));
        for (int side:{-1,1}) objects.push_back(makeCube("PLAYER_LEG_"+std::to_string(side),"Player","Shooter leg",
            player.position+Vec3{side*.19f,-1.42f,0},{.22f,.56f,.3f},{.19f,.24f,.28f}));
    }
    const auto gun=createWeapon(weapon,player,recoil);
    objects.insert(objects.end(),gun.begin(),gun.end());
    for (const auto& p:projectiles) objects.push_back(projectileObject(p));
    for (const auto& p:debris) {
        auto o=makeCube(p.isBlood?("BLOOD_"+std::to_string(p.id)):("TARGET_FRAGMENT_"+std::to_string(p.id)),
                        p.isBlood?"Blood":"Hit effect",
                        p.isBlood?"Blood droplet":"Break fragment",
                        p.position,p.scale,p.color);
        o.transform.rotation=p.rotation; objects.push_back(o);
    }
    if (usesLevel() && levels.stage==LevelStage::Finished && mode!=GameMode::BirdsEye) {
        const auto effect=celebrationObjects(levels.transition,mode==GameMode::Challenge);
        objects.insert(objects.end(),effect.begin(),effect.end());
    }
    if (mode==GameMode::BirdsEye) {
        const auto reference=birdEye.referenceObjects(); objects.insert(objects.end(),reference.begin(),reference.end());
    }
    for (auto& object:objects) { object.mode=modeName(mode); object.observedTime=elapsed; }
    if (usesLevel()) for (auto& object:objects) {
        object.level=levels.config.number;
        const std::string prefix=mode==GameMode::Challenge?"":std::string(modeName(mode))+"_";
        const auto group=prefix+"L"+std::to_string(object.level)+"_";
        object.id=group+object.id;
        object.parent=group+(object.parent.empty()?object.type:object.parent);
        object.notes+="; mode="+object.mode+"; level="+std::to_string(object.level)+"; level time="+std::to_string(levels.levelTime);
    }
    return objects;
}
// Add explanatory lighting metadata to the current scene for the calculation CSV.
std::vector<SceneObject> Game::calculationObjects() const {
    auto objects=scene(true);
    // Full lighting annotation strings are only needed for CSV calculation exports,
    // not for real-time rendering. This keeps them out of the hot path.
    const auto rig=createLighting(night);
    auto vectorText=[](Vec3 v) { return "("+std::to_string(v.x)+","+std::to_string(v.y)+","+std::to_string(v.z)+")"; };
    std::size_t pointIndex=0,spotIndex=0;
    for (auto& o:objects) {
        o.notes+="; simulation time = "+std::to_string(elapsed)+" s; mode="+(night?"NIGHT":"DAY");
        if (o.component=="Lens" && o.id.find("LAMP_")!=std::string::npos) {
            o.notes+="; point intensity="+vectorText(rig.points[pointIndex++].color)+"; attenuation=1/(1+0.09*d+0.032*d*d)";
        }
        if (o.component=="Lens" && o.id.find("FLOOD_")!=std::string::npos) {
            const auto& light=rig.spots[spotIndex++/3];
            o.notes+="; spot intensity="+vectorText(light.color)+"; direction="+vectorText(light.direction)+
                "; cone inner/outer=32/53 degrees; attenuation=1/(1+0.025*d+0.002*d*d)";
        }
        if (o.id=="ARENA_FLOOR") o.notes+="; ambient="+vectorText(rig.ambient)+"; sunlight="+vectorText(rig.sunColor);
    }
    for (auto& o:objects) {
        if (o.type=="Environment" && (o.component=="Sun visual" || o.component=="Moon visual"))
            o.notes+="; directional intensity="+vectorText(rig.sunColor)+"; direction to light="+vectorText(rig.sunDirection);
    }
    return objects;
}
}
