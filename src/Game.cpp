#include "Game.h"
#include <algorithm>
#include <limits>
#include <random>

namespace shooter {
namespace {
constexpr float infinity=std::numeric_limits<float>::infinity();
SceneObject cube(std::string id, std::string type, std::string component, Vec3 pos, Vec3 scale,
                 Vec3 color, float yaw=0) {
    Transform t; t.position=pos; t.scale=scale; t.rotation.y=yaw;
    return {id,type,component,t,color,"1 unit = 1 m; live simulation snapshot"};
}
Vec3 asVec3(Vec4 v) { return {v.x,v.y,v.z}; }
float length(Vec3 v) { return std::sqrt(dot(v,v)); }
SceneObject projectileObject(const Projectile& p) {
    auto o=cube("PROJECTILE_"+std::to_string(p.id),"Projectile",weaponSpec(p.weapon).name,
                p.position,weaponSpec(p.weapon).projectileScale,{1,.8f,.22f});
    o.transform.rotation={std::asin(std::clamp(p.direction.y,-1.0f,1.0f))*180/pi,
                          std::atan2(-p.direction.x,-p.direction.z)*180/pi,0};
    return o;
}
}
float intersectCube(Vec3 origin, Vec3 direction, const Transform& box, float maximum) {
    const Mat4 m=composeModelMatrix(box);
    // Invert the affine 3x3 basis. This also handles the wall supports' shears.
    const Vec3 a{m.at(0,0),m.at(1,0),m.at(2,0)}, b{m.at(0,1),m.at(1,1),m.at(2,1)},
               c{m.at(0,2),m.at(1,2),m.at(2,2)};
    const float determinant=dot(a,cross(b,c));
    if (std::abs(determinant)<1e-8f) return infinity;
    auto inverse=[&](Vec3 v) { return Vec3{dot(cross(b,c),v)/determinant,
        dot(cross(c,a),v)/determinant,dot(cross(a,b),v)/determinant}; };
    const Vec3 p=inverse(origin-box.position), d=inverse(direction);
    const float origins[]={p.x,p.y,p.z}, directions[]={d.x,d.y,d.z};
    float enter=0, leave=maximum;
    for (int axis=0; axis<3; ++axis) {
        if (std::abs(directions[axis])<1e-7f) {
            if (origins[axis]<-.5f || origins[axis]>.5f) return infinity;
        } else {
            float first=(-.5f-origins[axis])/directions[axis], last=(.5f-origins[axis])/directions[axis];
            if (first>last) std::swap(first,last);
            enter=std::max(enter,first); leave=std::min(leave,last);
            if (enter>leave) return infinity;
        }
    }
    return enter;
}
std::vector<SceneObject> generateCargoLayout(unsigned seed) {
    std::mt19937 random(seed);
    std::vector<SceneObject> objects;
    for (int stack=0; stack<10; ++stack) {
        // Jittered side zones leave the center lanes and spawn area clear.
        const float x=(stack%2?1.0f:-1.0f)*(18+float(random()%30)/10);
        const float z=-22-float(stack/2)*15-float(random()%30)/10;
        const int levels=1+int(random()%3);
        const float yaw=float(int(random()%31)-15);
        for (int level=0; level<levels; ++level) {
            const std::string id="CARGO_"+std::to_string(stack)+"_"+std::to_string(level);
            auto crate=cube(id,"Cargo","Crate",{x,1.5f+3*level,z},{3,3,3},{.52f,.35f,.18f},yaw);
            crate.notes="Fixed seed "+std::to_string(seed)+"; reproducible side-lane stack";
            objects.push_back(crate);
            // Contrasting bands remain cube instances and share the crate rotation.
            for (int band : {-1,1}) {
                const Vec3 offset=asVec3(transformPoint(makeRotationY(yaw),{band*.9f,0,0,0}));
                objects.push_back(cube(id+"_BAND_"+std::to_string(band),"Cargo","Crate band",
                    crate.transform.position+offset,{.16f,3.04f,3.04f},{.26f,.29f,.28f},yaw));
            }
        }
    }
    return objects;
}
Game::Game() {
    staticObjects=createArena();
    const auto cargo=generateCargoLayout(2107042);
    staticObjects.insert(staticObjects.end(),cargo.begin(),cargo.end());
    reset();
}
void Game::resetTargets() {
    targets.clear(); projectiles.clear();
    const Vec3 positions[]={{0,2.7f,-19},{-8,3,-29},{8,4.4f,-38},{-6,3.5f,-52},
                            {8,5,-65},{0,3.7f,-82}};
    for (int i=0; i<6; ++i) {
        Target t; t.base=positions[i]; t.position=t.base; t.movement=i%3; t.phase=i*.8f;
        targets.push_back(t);
    }
    elapsed=0; cooldown=0; recoil=0;
}
void Game::reset() {
    player.position={0,1.7f,-5}; player.yaw=-90; player.pitch=3.8f;
    freeCamera=Camera{}; cameraMode=1; weapon=WeaponType::Pistol;
    shots=hits=destroyed=0; nextProjectile=1; resetTargets();
}
Transform Game::targetTransform(const Target& t) const {
    Transform transform;
    transform.position=t.position; transform.rotation.y=t.yaw;
    transform.scale={3,3,.32f};
    return transform;
}
bool Game::canStand(Vec3 p) const {
    if (p.x<-26 || p.x>26 || p.z<-96 || p.z>-4) return false;
    // Conservative footprints keep the walking player outside rotated crates/supports.
    for (const auto& o:staticObjects) {
        if (o.type!="Cargo" && o.component!="Sheared stone support") continue;
        const Mat4 m=composeModelMatrix(o.transform);
        const float extentX=.5f*(std::abs(m.at(0,0))+std::abs(m.at(0,1))+std::abs(m.at(0,2)))+.45f;
        const float extentZ=.5f*(std::abs(m.at(2,0))+std::abs(m.at(2,1))+std::abs(m.at(2,2)))+.45f;
        if (std::abs(p.x-o.transform.position.x)<extentX && std::abs(p.z-o.transform.position.z)<extentZ)
            return false;
    }
    return true;
}
void Game::movePlayer(float forward, float right, float dt, bool fast) {
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
        if (targets[i].respawn>0) continue;
        const float hit=intersectCube(player.position,player.forward(),targetTransform(targets[i]),nearest);
        if (hit<nearest) { nearest=hit; result=int(i); }
    }
    distance=result<0?0:length(targets[result].position-player.position);
    return result;
}
bool Game::fire() {
    if (cooldown>0) return false;
    const auto& spec=weaponSpec(weapon);
    const Vec3 muzzle=weaponMuzzle(weapon,player);
    float aimDistance=100;
    for (const auto& object:staticObjects)
        aimDistance=std::min(aimDistance,intersectCube(player.position,player.forward(),object.transform,aimDistance));
    for (const auto& t:targets) if (t.respawn<=0)
        aimDistance=std::min(aimDistance,intersectCube(player.position,player.forward(),targetTransform(t),aimDistance));
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
        projectiles.push_back({nextProjectile++,weapon,muzzle,normalize(direction+right*x+up*y),0});
    }
    ++shots; cooldown=spec.cooldown; recoil=1;
    return true;
}
void Game::update(float dt) {
    // Small simulation steps avoid tunnelling and keep motion stable across frame rates.
    while (dt>0) { const float step=std::min(dt,1.0f/120); updateStep(step); dt-=step; }
}
void Game::updateStep(float dt) {
    elapsed+=dt; cooldown=std::max(0.0f,cooldown-dt); recoil=std::max(0.0f,recoil-dt*7);
    for (auto& t:targets) {
        t.hitTime=std::max(0.0f,t.hitTime-dt);
        if (t.respawn>0) { t.respawn=std::max(0.0f,t.respawn-dt); if (t.respawn==0) t.health=100; }
        t.position=t.base;
        if (t.movement==0) t.position.x+=std::sin(elapsed*.8f+t.phase)*3;
        if (t.movement==1) t.position.y+=std::sin(elapsed*1.1f+t.phase)*1.2f;
        t.yaw=t.movement==2?std::sin(elapsed*.9f+t.phase)*65:0;
    }
    for (auto& p:projectiles) {
        const auto& spec=weaponSpec(p.weapon);
        const float travel=std::min(spec.speed*dt,spec.range-p.travelled);
        float nearest=travel; int hitTarget=-1; bool blocked=false;
        for (const auto& o:staticObjects) {
            const float d=intersectCube(p.position,p.direction,o.transform,nearest);
            if (d<=nearest) { nearest=d; blocked=true; }
        }
        for (std::size_t i=0; i<targets.size(); ++i) if (targets[i].respawn<=0) {
            const float d=intersectCube(p.position,p.direction,targetTransform(targets[i]),nearest);
            if (d<nearest) { nearest=d; hitTarget=int(i); blocked=true; }
        }
        p.position=p.position+p.direction*nearest;
        p.travelled+=nearest;
        if (hitTarget>=0) {
            auto& target=targets[hitTarget]; target.hitTime=.3f; target.health-=spec.damage; ++hits;
            if (target.health<=0) { target.respawn=1.5f; ++destroyed; }
        }
        if (blocked) p.travelled=spec.range;
    }
    projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),[](const Projectile& p) {
        return p.travelled>=weaponSpec(p.weapon).range-.0001f;
    }),projectiles.end());
}
std::vector<SceneObject> Game::scene(bool includePlayer) const {
    auto objects=staticObjects;
    for (std::size_t i=0; i<targets.size(); ++i) {
        const auto& t=targets[i]; const std::string id="TARGET_"+std::to_string(i+1);
        objects.push_back(cube(id+"_BASE","Target","Stand base",{t.position.x,.15f,t.position.z},{2,.3f,1.5f},{.25f,.29f,.32f}));
        const float height=std::max(.3f,t.position.y-1.5f);
        objects.push_back(cube(id+"_STAND","Target","Stand",{t.position.x,height/2,t.position.z},{.24f,height,.24f},{.33f,.37f,.39f}));
        if (t.respawn>0) continue;
        auto plate=cube(id+"_PLATE","Target","Target plate",t.position,{3,3,.32f},
                        t.hitTime>0?Vec3{1,.8f,.15f}:Vec3{.82f,.22f,.17f},t.yaw);
        objects.push_back(plate);
        for (int layer=0; layer<2; ++layer) {
            const Vec3 offset=asVec3(transformPoint(makeRotationY(t.yaw),{0,0,.18f+layer*.045f,0}));
            objects.push_back(cube(id+"_RING_"+std::to_string(layer),"Target","Cube ring layer",
                t.position+offset,{layer==0?2.15f:1.05f,layer==0?2.15f:1.05f,.06f},
                layer==0?Vec3{.93f,.89f,.72f}:Vec3{.85f,.24f,.18f},t.yaw));
        }
    }
    if (includePlayer) {
        objects.push_back(cube("PLAYER_BODY","Player","Shooter body",player.position+Vec3{0,-.8f,0},{.65f,.95f,.4f},{.18f,.40f,.45f},-player.yaw-90));
        objects.push_back(cube("PLAYER_HEAD","Player","Shooter head",player.position+Vec3{0,.05f,0},{.4f,.4f,.4f},{.75f,.62f,.46f}));
        for (int side:{-1,1}) objects.push_back(cube("PLAYER_LEG_"+std::to_string(side),"Player","Shooter leg",
            player.position+Vec3{side*.19f,-1.42f,0},{.22f,.56f,.3f},{.19f,.24f,.28f}));
    }
    const auto gun=createWeapon(weapon,player,recoil);
    objects.insert(objects.end(),gun.begin(),gun.end());
    for (const auto& p:projectiles) objects.push_back(projectileObject(p));
    return objects;
}
std::vector<SceneObject> Game::calculationObjects() const {
    auto objects=scene(true);
    for (auto type:{WeaponType::Pistol,WeaponType::Shotgun,WeaponType::Rifle}) {
        if (type!=weapon) {
            auto gun=createWeapon(type,player);
            for (auto& part:gun) part.notes+="; inactive weapon preview at current player pose";
            objects.insert(objects.end(),gun.begin(),gun.end());
        }
        // Representative projectile geometry even when no live shot exists.
        Projectile example{0,type,weaponMuzzle(type,player),player.forward(),0};
        auto sample=projectileObject(example);
        sample.id="SAMPLE_PROJECTILE_"+std::to_string(static_cast<int>(type));
        sample.notes="Representative muzzle snapshot; not an active projectile";
        objects.push_back(sample);
    }
    for (auto& object:objects) object.notes+="; simulation time = "+std::to_string(elapsed)+" s";
    return objects;
}
}
