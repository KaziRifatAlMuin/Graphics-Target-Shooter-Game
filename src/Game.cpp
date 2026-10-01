#include "Game.h"
#include <algorithm>
#include <limits>
#include "Environment.h"

namespace shooter {
namespace { constexpr float infinity=std::numeric_limits<float>::infinity(); }
Game::Game() {
    staticObjects=createArena();
    const auto cargo=generateCargoLayout(2107042);
    staticObjects.insert(staticObjects.end(),cargo.begin(),cargo.end());
    const auto fixtures=createLightFixtures();
    staticObjects.insert(staticObjects.end(),fixtures.begin(),fixtures.end());
    const auto equipment=createEquipmentDisplay();
    staticObjects.insert(staticObjects.end(),equipment.begin(),equipment.end());
    reset();
}
void Game::resetTargets() {
    if (challenge) { restartLevel(); return; }
    targets.clear(); projectiles.clear(); debris.clear(); soundEvents.clear(); feedbackTime=0; lastRing=-1;
    targets=createSandboxTargets();
    elapsed=0; cooldown=0; recoil=0;
}
void Game::reset() {
    if (challenge) {
        challenge=false; staticObjects=createArena();
        for (const auto& group:{generateCargoLayout(2107042),createLightFixtures(),createEquipmentDisplay()})
            staticObjects.insert(staticObjects.end(),group.begin(),group.end());
    }
    static_cast<RunStats&>(*this)=RunStats{}; birds.clear(); humans.clear(); scoreFeedback.clear(); dangerTime=0;
    player.position={0,1.7f,-5}; player.yaw=-90; player.pitch=3.8f;
    freeCamera=Camera{}; cameraMode=1; weapon=WeaponType::Pistol;
    // Entity IDs stay unique for the process lifetime, including restarted sessions.
    // This keeps retained CSV observations distinct from new shots/fragments.
    shots=hits=destroyed=score=bullseyes=0; resetTargets();
}
bool Game::canStand(Vec3 p) const { return canStandAt(p,staticObjects); }
void Game::movePlayer(float forward, float right, float dt, bool fast) {
    if (challenge && (!gameplayActive() || !levels.config.playerMovement)) return;
    Vec3 f=player.forward(); f.y=0; f=normalize(f);
    const Vec3 movement=normalize(f*forward+cross(f,{0,1,0})*right)*((fast?9.0f:5.0f)*dt);
    const int steps=std::max(1,int(std::ceil(length(movement)/.15f)));
    for (int i=0; i<steps; ++i) {
        Vec3 next=player.position; next.x+=movement.x/steps;
        if (canStand(next)) player.position=next;
        next=player.position; next.z+=movement.z/steps;
        if (canStand(next)) player.position=next;
    }
}
void Game::setCamera(int mode) {
    if (mode==4 && cameraMode!=4) freeCamera=activeCamera();
    cameraMode=mode;
}
Camera Game::activeCamera() const {
    if (cameraMode==1) return player;
    if (cameraMode==4) return freeCamera;
    Camera camera;
    if (cameraMode==2) return camera;
    camera.position={25,15,-42}; camera.yaw=-175; camera.pitch=-17;
    return camera;
}
int Game::aimedTarget(float& distance) const {
    float nearest=150;
    int result=-1;
    for (const auto& o:staticObjects) nearest=std::min(nearest,intersectCube(player.position,player.forward(),o.transform,nearest));
    for (std::size_t i=0; i<targets.size(); ++i) {
        if (targets[i].respawn>0 || targets[i].eliminated) continue;
        const auto hit=intersectTarget(player.position,player.forward(),targets[i],nearest);
        if (hit.distance<nearest) { nearest=hit.distance; result=hit.ring>=0?int(i):-1; }
    }
    if (intersectNpcs(player.position,player.forward(),nearest,npcColliders()).distance<nearest) result=-1;
    distance=result<0?0:length(targets[result].position-player.position);
    return result;
}
bool Game::fire() {
    if (cooldown>0 || !gameplayActive()) return false;
    const auto& spec=weaponSpec(weapon);
    const Vec3 muzzle=weaponMuzzle(weapon,player);
    float aimDistance=100;
    for (const auto& object:staticObjects)
        aimDistance=std::min(aimDistance,intersectCube(player.position,player.forward(),object.transform,aimDistance));
    for (const auto& t:targets) if (t.respawn<=0 && !t.eliminated)
        aimDistance=std::min(aimDistance,intersectTarget(player.position,player.forward(),t,aimDistance).distance);
    aimDistance=std::min(aimDistance,intersectNpcs(player.position,player.forward(),aimDistance,npcColliders()).distance);
    const Vec3 direction=normalize(player.position+player.forward()*aimDistance-muzzle);
    const Vec3 right=normalize(cross(direction,{0,1,0})), up=normalize(cross(right,direction));
    // Reject a muzzle beyond nearby cover instead of shooting through the cover.
    const Vec3 toMuzzle=muzzle-player.position;
    for (const auto& object:staticObjects)
        if (intersectCube(player.position,normalize(toMuzzle),object.transform,length(toMuzzle))<infinity) return false;
    for (int i=0; i<spec.pellets; ++i) {
        float x=0,y=0;
        if (weapon==WeaponType::Shotgun && i>0) {
            const float angle=2*pi*(i-1)/8;
            x=std::cos(angle)*spec.spread; y=std::sin(angle)*spec.spread;
        } else if (weapon==WeaponType::Rifle) {
            x=std::sin(float(shots)*2.4f)*spec.spread; y=std::cos(float(shots)*1.7f)*spec.spread;
        }
        projectiles.push_back({nextProjectile++,weapon,muzzle,normalize(direction+right*x+up*y),0,nextShot});
    }
    ++shots; ++nextShot; cooldown=spec.cooldown; recoil=1;
    soundEvents.push_back(weapon==WeaponType::Pistol?SoundEvent::Pistol:weapon==WeaponType::Shotgun?SoundEvent::Shotgun:SoundEvent::Rifle);
    return true;
}
void Game::update(float dt) {
    // Small simulation steps avoid tunnelling and keep motion stable across frame rates.
    while (dt>0) { const float step=std::min(dt,1.0f/120); updateStep(step); dt-=step; }
}
void Game::updateStep(float dt) {
    ScoreSystem::update(scoreFeedback,dt); dangerTime=std::max(0.0f,dangerTime-dt);
    if (challenge && !gameplayActive()) { levels.updateTransition(dt); return; }
    elapsed+=dt; if (challenge) levels.levelTime+=dt;
    cooldown=std::max(0.0f,cooldown-dt); recoil=std::max(0.0f,recoil-dt*7);
    feedbackTime=std::max(0.0f,feedbackTime-dt);
    for (auto& t:targets) updateTarget(t,challenge?levels.levelTime:elapsed,dt);
    if (challenge) updateNpcs(dt);
    updateProjectiles(projectiles,targets,staticObjects,dt,
        [this](std::size_t index,int ring,std::uint64_t shot) { applyTargetHit(index,ring,shot); },
        projectiles.empty()?std::vector<NpcCollider>{}:npcColliders(),
        [this](bool human,std::size_t index,std::uint64_t shot) { applyNpcHit(human,index,shot); });
    if (challenge && levels.finishIfComplete(targets,*this)) projectiles.clear();
    for (auto& piece:debris) {
        piece.life-=dt; piece.velocity.y-=7*dt;
        piece.position=piece.position+piece.velocity*dt;
        piece.rotation=piece.rotation+Vec3{120,80,50}*dt;
    }
    debris.erase(std::remove_if(debris.begin(),debris.end(),[](const Debris& p) { return p.life<=0; }),debris.end());
}
void Game::applyTargetHit(std::size_t index,int ring,std::uint64_t shotId) {
    auto& t=targets.at(index);
    if (!gameplayActive() || t.eliminated || t.respawn>0 || ring<0 || ring>5) return;
    const int damage=ringDamage(ring);
    int& previous=t.damageByShot[shotId];
    if (damage<=previous) return;
    if (previous==0) { ++hits; soundEvents.push_back(SoundEvent::Hit); }
    // A shotgun trigger counts once: later pellets may upgrade to a better ring,
    // but their damage is never summed as if they were separate shots.
    t.health-=damage-previous; if (!challenge) score+=damage-previous; previous=damage;
    t.hitTime=.25f; lastRing=ring; feedbackTime=.9f;
    if (t.health<=0) {
        t.health=0;
        if (challenge) { t.eliminated=true; ScoreSystem::target(*this,scoreFeedback,t.damageByShot.size()==1); }
        else { t.respawn=1.8f; ++destroyed; score+=100; if (ring==0) ++bullseyes; }
        soundEvents.push_back(SoundEvent::Break);
        for (int i=0;i<12;++i) {
            const float angle=i*2*pi/12;
            debris.push_back({nextDebris++,t.position,{std::cos(angle)*2.4f,2+std::sin(angle)*2,1.5f},
                              {0,float(i)*30,0},.75f});
        }
    }
}
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
    if (challenge) for (auto& object:objects) {
        object.level=levels.config.number;
        object.id="L"+std::to_string(object.level)+"_"+object.id;
        object.notes+="; challenge level "+std::to_string(object.level)+"; level time="+std::to_string(levels.levelTime);
    }
    return objects;
}
std::vector<SceneObject> Game::calculationObjects() const {
    auto objects=scene(true);
    for (auto& object:objects) object.notes+="; simulation time = "+std::to_string(elapsed)+" s; mode="+(night?"NIGHT":"DAY");
    return objects;
}
}
