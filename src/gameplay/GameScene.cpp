#include "gameplay/Game.h"
#include "world/Environment.h"
#include "gameplay/Effects.h"
namespace shooter {
std::vector<SceneObject> Game::scene(bool includePlayer) const {
    auto objects=staticObjects;
    const auto rig=createLighting(night);
    auto vectorText=[](Vec3 v) { return "("+std::to_string(v.x)+","+std::to_string(v.y)+","+std::to_string(v.z)+")"; };
    std::size_t pointIndex=0,spotIndex=0;
    for (auto& o:objects) {
        if (o.component=="Point lamp head") {
            o.emission=night?1:0;
            o.notes+="; point intensity="+vectorText(rig.points[pointIndex++].color)+"; attenuation=1/(1+0.045*d+0.003*d*d)";
        }
        if (o.component=="Spotlight head") {
            o.emission=night?1:0;
            const auto& light=rig.spots[spotIndex++];
            o.notes+="; spot intensity="+vectorText(light.color)+"; direction="+vectorText(light.direction)+
                "; cone inner/outer=22/34 degrees; attenuation=1/(1+0.025*d+0.002*d*d)";
        }
        if (o.id=="ARENA_FLOOR") o.notes+="; ambient="+vectorText(rig.ambient)+"; sunlight="+vectorText(rig.sunColor);
    }
    auto celestial=createCelestialObjects(night);
    for (auto& o:celestial) o.notes+="; directional intensity="+vectorText(rig.sunColor)+"; direction to light="+vectorText(rig.sunDirection);
    objects.insert(objects.end(),celestial.begin(),celestial.end());
    for (std::size_t i=0; i<targets.size(); ++i) {
        const auto parts=createTargetObjects(targets[i],i);
        objects.insert(objects.end(),parts.begin(),parts.end());
    }
    for (std::size_t i=0;i<birds.size();++i) if (birds[i].active) {
        const auto parts=createBirdObjects(birds[i],i); objects.insert(objects.end(),parts.begin(),parts.end());
    }
    for (std::size_t i=0;i<humans.size();++i) if (humans[i].active) {
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
        auto o=makeCube("TARGET_FRAGMENT_"+std::to_string(p.id),"Hit effect","Break fragment",p.position,{.12f,.12f,.05f},{.95f,.4f,.14f});
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
std::vector<SceneObject> Game::calculationObjects() const {
    auto objects=scene(true);
    for (auto& object:objects) object.notes+="; simulation time = "+std::to_string(elapsed)+" s; mode="+(night?"NIGHT":"DAY");
    return objects;
}
}
