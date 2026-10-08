#include "gameplay/Game.h"
#include <algorithm>
#include <limits>
#include "world/Environment.h"

namespace shooter {
namespace { constexpr float infinity=std::numeric_limits<float>::infinity(); }
// Build the practice arena, cargo, fixtures, and equipment, then initialize the session.
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
// Restore practice targets and clear temporary effects, or restart the current level.
void Game::resetTargets() {
    cachedAimFrame=-1;
    if (usesLevel()) { restartLevel(); return; }
    targets.clear(); projectiles.clear(); debris.clear(); soundEvents.clear(); feedbackTime=0; lastRing=-1;
    targets=createSandboxTargets();
    elapsed=0; cooldown=0; recoil=0;
}
// Return to practice defaults and reset run statistics while preserving unique entity ID counters.
void Game::reset() {
    if (usesLevel()) {
        mode=GameMode::Practice; staticObjects=createArena();
        for (const auto& group:{generateCargoLayout(2107042),createLightFixtures(),createEquipmentDisplay()})
            staticObjects.insert(staticObjects.end(),group.begin(),group.end());
    }
    static_cast<RunStats&>(*this)=RunStats{}; birds.clear(); humans.clear(); scoreFeedback.clear(); dangerTime=0;
    pendingResult.reset(); freeRemaining=freeSessionSeconds;
    player.position={0,1.7f,-5}; player.yaw=-90; player.pitch=3.8f;
    freeCamera=Camera{}; cameraMode=1; weapon=WeaponType::Pistol;
    // Entity IDs stay unique for the process lifetime, including restarted sessions.
    // This keeps retained CSV observations distinct from new shots/fragments.
    shots=hits=destroyed=score=bullseyes=0; resetTargets();
}
// Delegate walking clearance to the shared world collision test.
bool Game::canStand(Vec3 p) const { return canStandAt(p,staticObjects); }
// Normalize input, split movement into <=0.15 m steps, and check X/Z separately to slide along walls.
void Game::movePlayer(float forward, float right, float dt, bool fast) {
    if (mode==GameMode::BirdsEye || (usesLevel() && (!gameplayActive() || !levels.config.playerMovement))) return;
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
// Select player, overview, side, or free view; entering free view starts from the current camera.
void Game::setCamera(int mode) {
    if (this->mode==GameMode::BirdsEye) { if (mode==2) birdEye.overview(); return; }
    if (mode==4 && cameraMode!=4) freeCamera=activeCamera();
    cameraMode=mode;
}
// Return the camera pose selected by the current mode and view setting.
Camera Game::activeCamera() const {
    if (mode==GameMode::BirdsEye) return birdEye.activeCamera();
    if (cameraMode==1) return player;
    if (cameraMode==4) return freeCamera;
    Camera camera;
    if (cameraMode==2) return camera;
    camera.position={25,15,-42}; camera.yaw=-175; camera.pitch=-17;
    return camera;
}
// Find the nearest unobstructed target under the player's aiming ray and reuse unchanged queries.
int Game::aimedTarget(float& distance) const {
    // Return cached result if player aim hasn't changed since last query.
    const Vec3 fwd=player.forward();
    const Vec3 dp=player.position-cachedAimPos;
    const Vec3 dd={fwd.x-cachedAimDir.x,fwd.y-cachedAimDir.y,fwd.z-cachedAimDir.z};
    if (cachedAimFrame>=0 && dot(dp,dp)<1e-8f && dot(dd,dd)<1e-8f) {
        distance=cachedAimDistance;
        return cachedAimTarget;
    }
    float nearest=150;
    int result=-1;
    for (const auto& o:staticObjects) nearest=std::min(nearest,intersectCube(player.position,fwd,o.transform,nearest));
    for (std::size_t i=0; i<targets.size(); ++i) {
        if (targets[i].respawn>0 || targets[i].eliminated) continue;
        const auto hit=intersectTarget(player.position,fwd,targets[i],nearest);
        if (hit.distance<nearest) { nearest=hit.distance; result=hit.ring>=0?int(i):-1; }
    }
    if (intersectNpcs(player.position,fwd,nearest,npcColliders()).distance<nearest) result=-1;
    distance=result<0?0:length(targets[result].position-player.position);
    // Cache result.
    cachedAimTarget=result; cachedAimDistance=distance;
    cachedAimPos=player.position; cachedAimDir=fwd; cachedAimFrame=0;
    return result;
}
// Aim from the muzzle toward the crosshair's contact point, then spawn pellets if firing is allowed.
bool Game::fire() {
    if (mode==GameMode::BirdsEye || cooldown>0 || !gameplayActive()) return false;
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
            // Place eight outer shotgun pellets on a circle: x=spread*cos(angle), y=spread*sin(angle).
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
// Split elapsed time into steps no larger than 1/120 second to stabilize the simulation.
void Game::update(float dt) {
    cachedAimFrame=-1;
    // Small simulation steps avoid tunnelling and keep motion stable across frame rates.
    while (dt>0) { const float step=std::min(dt,1.0f/120); updateStep(step); dt-=step; }
}
// Create a capped set of seeded particles with varied velocity, color, size, and lifetime.
void Game::spawnBlood(Vec3 position, int count) {
    constexpr std::size_t limit=250;
    count=std::clamp(count,0,int(limit));
    if (debris.size()+count>limit) debris.erase(debris.begin(),debris.begin()+(debris.size()+count-limit));
    static unsigned seed = 133742;
    for (int i=0; i<count; ++i) {
        seed = seed * 1664525u + 1013904223u;
        float rx = float((seed >> 8) & 255) / 127.5f - 1.0f;
        seed = seed * 1664525u + 1013904223u;
        float ry = float((seed >> 8) & 255) / 255.0f;
        seed = seed * 1664525u + 1013904223u;
        float rz = float((seed >> 8) & 255) / 127.5f - 1.0f;
        seed = seed * 1664525u + 1013904223u;
        float rl = 1.0f + float((seed >> 8) & 255) / 255.0f * .6f;

        Vec3 vel{ rx * 3.4f, 0.8f + ry * 3.2f, rz * 3.4f };
        float size = 0.035f + float((seed >> 8) & 63) / 63.0f * 0.035f;
        Vec3 bloodColor = (i % 3 == 0) ? Vec3{.68f, .02f, .02f} : (i % 3 == 1) ? Vec3{.88f, .06f, .05f} : Vec3{.52f, .01f, .01f};
        debris.push_back({nextDebris++, position, vel, {float(rx*180), float(ry*180), float(rz*180)}, rl, bloodColor, {size, size, size}, true});
    }
}
// Advance effects, timers, targets, characters, and projectiles for one small simulation step.
void Game::updateStep(float dt) {
    for (auto& piece:debris) {
        piece.life-=dt;
        if (piece.isBlood) {
            if (piece.position.y > 0.03f) {
                piece.velocity.y -= 13.5f * dt;
                piece.position = piece.position + piece.velocity * dt;
                piece.rotation = piece.rotation + Vec3{80, 160, 60} * dt;
            } else {
                piece.position.y = 0.02f;
                piece.velocity = {0, 0, 0};
                piece.rotation = {0,piece.rotation.y,0};
                piece.scale.y = 0.012f;
                piece.scale.x = piece.scale.z = std::min(0.28f, piece.scale.x * 1.35f);
            }
        } else {
            piece.velocity.y-=7*dt;
            piece.position=piece.position+piece.velocity*dt;
            piece.rotation=piece.rotation+Vec3{120,80,50}*dt;
        }
    }
    if (debris.size() > 250) {
        debris.erase(debris.begin(), debris.begin() + (debris.size() - 250));
    }
    debris.erase(std::remove_if(debris.begin(),debris.end(),[](const Debris& p) { return p.life<=0; }),debris.end());
    ScoreSystem::update(scoreFeedback,dt); dangerTime=std::max(0.0f,dangerTime-dt);
    if (usesLevel()) updateNpcDeaths(dt); // Cosmetic falls finish even after the final target/time-up.
    if (usesLevel() && !gameplayActive()) { levels.updateTransition(dt); return; }
    if (mode==GameMode::Free) dt=static_cast<float>(std::min(double(dt),freeRemaining));
    elapsed+=dt; if (usesLevel()) levels.levelTime+=dt;
    cooldown=std::max(0.0f,cooldown-dt); recoil=std::max(0.0f,recoil-dt*7);
    feedbackTime=std::max(0.0f,feedbackTime-dt);
    for (auto& t:targets) updateTarget(t,usesLevel()?levels.levelTime:float(elapsed),dt);
    if (usesLevel()) updateNpcs(dt);
    updateProjectiles(projectiles,targets,staticObjects,dt,
        [this](std::size_t index,int ring,std::uint64_t shot) { applyTargetHit(index,ring,shot); },
        projectiles.empty()?std::vector<NpcCollider>{}:npcColliders(),
        [this](bool human,std::size_t index,std::uint64_t shot) { applyNpcHit(human,index,shot); });
    if ((mode==GameMode::Challenge || mode==GameMode::Developer) && levels.finishIfComplete(targets,*this)) {
        projectiles.clear();
        soundEvents.push_back(levels.config.number==7?SoundEvent::Victory:SoundEvent::Complete);
        if (mode==GameMode::Challenge && challengeFromStart) pendingResult=EligibleResult{playerName,mode,*this};
    }
    if (mode==GameMode::Free) {
        freeRemaining=std::max(0.0,freeRemaining-dt);
        if (freeRemaining<1e-6) {
            soundEvents.push_back(SoundEvent::TimeUp);
            freeRemaining=0; elapsed=freeSessionSeconds; levels.stage=LevelStage::Finished; levels.transition=0;
            projectiles.clear(); levels.completedStats=*this; pendingResult=EligibleResult{playerName,mode,*this};
        }
    }

}
// Apply shot-specific ring damage and trigger score, destruction effects, or respawn when health reaches zero.
void Game::applyTargetHit(std::size_t index,int ring,std::uint64_t shotId) {
    auto& t=targets.at(index);
    if (mode==GameMode::BirdsEye || !gameplayActive() || t.eliminated || t.respawn>0 || ring<0 || ring>5) return;
    const int damage=ringDamage(ring);
    int& previous=t.damageByShot[shotId];
    if (damage<=previous) return;
    if (previous==0) { ++hits; soundEvents.push_back(SoundEvent::Hit); }
    // A shotgun trigger counts once: later pellets may upgrade to a better ring,
    // but their damage is never summed as if they were separate shots.
    t.health-=damage-previous; if (!usesLevel()) score+=damage-previous; previous=damage;
    t.hitTime=.25f; lastRing=ring; feedbackTime=.9f;
    if (t.health<=0) {
        t.health=0;
        if (usesLevel()) {
            if (mode==GameMode::Free) t.respawn=freeRespawnSeconds; else t.eliminated=true;
            ScoreSystem::target(*this,scoreFeedback,t.damageByShot.size()==1);
        }
        else { t.respawn=1.8f; ++destroyed; score+=100; if (ring==0) ++bullseyes; }
        soundEvents.push_back(SoundEvent::Break);
        for (int i=0;i<12;++i) {
            const float angle=i*2*pi/12;
            debris.push_back({nextDebris++,t.position,{std::cos(angle)*2.4f,2+std::sin(angle)*2,1.5f},
                              {0,float(i)*30,0},.75f});
        }
        if(debris.size()>250) debris.erase(debris.begin(),debris.begin()+(debris.size()-250));
    }
}
}
