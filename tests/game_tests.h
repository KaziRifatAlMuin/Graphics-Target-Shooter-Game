#pragma once
#include "gameplay/Game.h"
#include "ui/Interface.h"
#include "audio/Sound.h"
#include "persistence/CsvLogger.h"
#include <fstream>

void gameTests() {
    const auto cargo=generateCargoLayout(2107042), same=generateCargoLayout(2107042), other=generateCargoLayout(17);
    require(!cargo.empty() && cargo.size()==same.size(),"Cargo generation is not deterministic.");
    bool changed=cargo.size()!=other.size();
    for (std::size_t i=0;i<cargo.size();++i) {
        near(cargo[i].transform.position.x,same[i].transform.position.x);
        near(cargo[i].transform.position.z,same[i].transform.position.z);
        require(std::abs(cargo[i].transform.position.x)>12,"Cargo obstructs reserved target corridors.");
        if (i<other.size()) changed|=cargo[i].transform.position.x!=other[i].transform.position.x;
    }
    require(changed,"Cargo seed has no effect.");
    require(cargo.size()>900,"Cargo foundation is too sparse.");
    for (const auto& o:cargo) if (o.component=="Crate") near(o.transform.scale.y,dimensions::crateSize);
    // Walking routes connect every target area, including the far rifle lane.
    Game routes;
    for (float z=-5;z>=-95;z-=1) require(canStandAt({0,1.7f,z},routes.staticObjects),"Central walking route blocked.");
    for (float z:{-15.f,-25.f,-35.f,-45.f,-55.f,-75.f,-85.f})
        for (float x=-21;x<=21;x+=.5f) require(canStandAt({x,1.7f,z},routes.staticObjects),"Cross aisle blocked.");
    Transform box; box.position={0,0,-5}; box.scale={2,2,.3f}; box.rotation.y=45;
    near(intersectCube({0,0,0},{0,0,-1},box,10),5-.15f/std::cos(radians(45)));
    require(!std::isfinite(intersectCube({5,0,0},{0,0,-1},box,10)),"Ray outside box should miss.");
    require(!std::isfinite(intersectCube({0,0,0},{0,0,-1},box,2)),"Ray exceeded allowed range.");

    Game moving;
    const Vec3 initial=moving.targets[0].position;
    moving.update(1);
    require(std::abs(moving.targets[0].position.x-initial.x)>1,"Target did not move horizontally.");
    require(std::abs(moving.targets[1].position.y-moving.targets[1].base.y)>.1f,"Target did not move vertically.");
    require(std::abs(moving.targets[2].yaw)>1,"Target did not rotate.");
    const auto playerStart=moving.player.position;
    moving.movePlayer(1,0,1,false);
    near(moving.player.position.z,playerStart.z-5);
    near(moving.activeCamera().position.z,moving.player.position.z);
    moving.setCamera(4);
    moving.freeCamera.move(1,0,1,1,false);
    near(moving.player.position.z,playerStart.z-5);
    require(moving.freeCamera.position.y>moving.player.position.y,"Free camera is not independent.");
    moving.staticObjects.clear();
    moving.movePlayer(1,0,100,true);
    require(moving.player.position.z>=-96,"Player escaped the closed arena.");
    moving.reset(); moving.staticObjects.clear();
    SceneObject obstacle; obstacle.type="Cargo"; obstacle.transform.position={0,1.5f,-10}; obstacle.transform.scale={3,3,3};
    moving.staticObjects.push_back(obstacle);
    moving.movePlayer(1,0,4,true);
    require(moving.player.position.z>-8.1f,"Player walked through cargo.");

    auto controlled=[]() {
        Game game; game.staticObjects.clear(); game.targets.resize(1);
        game.player.pitch=0;
        game.targets[0].base={0,1.7f,-12}; game.targets[0].position=game.targets[0].base;
        game.targets[0].movement=3; // No animation in deterministic collision fixtures.
        return game;
    };
    auto hit=controlled(); float distance=0;
    require(hit.aimedTarget(distance)==0,"Crosshair ray missed a visible target."); near(distance,7);
    require(hit.fire(),"Pistol did not fire.");
    require(!hit.fire(),"Weapon cooldown was ignored.");
    hit.update(.3f);
    require(hit.hits==1 && hit.targets[0].health==0,"Center hit must break the target in one shot.");
    require(hit.projectiles.empty(),"Projectile survived impact.");
    require(hit.destroyed==1 && hit.targets[0].respawn>0,"Target destruction did not trigger respawn.");
    hit.update(2);
    require(hit.targets[0].health==60 && hit.targets[0].respawn==0,"Target did not reset after hit.");
    auto restarted=controlled(); restarted.fire();
    const auto firstId=restarted.projectiles.front().id;
    restarted.reset(); restarted.fire();
    require(restarted.projectiles.front().id>firstId,"New session reuses an ID retained by CSV history.");

    auto blocked=controlled();
    obstacle.transform.position={0,1.7f,-9}; obstacle.transform.scale={4,4,.2f}; blocked.staticObjects.push_back(obstacle);
    require(blocked.aimedTarget(distance)<0,"Range finder saw through cover.");
    blocked.fire(); blocked.update(.5f);
    require(blocked.hits==0 && blocked.projectiles.empty(),"Projectile passed through cover.");

    auto nearest=controlled();
    auto farther=nearest.targets[0]; farther.base.z=-20; farther.position=farther.base;
    nearest.targets.insert(nearest.targets.begin(),farther);
    nearest.fire(); nearest.update(.3f);
    require(nearest.targets[0].health==60 && nearest.targets[1].health==0,"Collision did not choose the nearest target.");

    auto shotgun=controlled(); shotgun.weapon=WeaponType::Shotgun; shotgun.fire();
    require(shotgun.projectiles.size()==9,"Shotgun pellet count is wrong.");
    require(std::abs(shotgun.projectiles[0].direction.x-shotgun.projectiles[1].direction.x)>.03f,"Shotgun has no spread.");
    shotgun.targets.clear(); shotgun.update(1);
    require(shotgun.projectiles.empty(),"Projectiles exceeded maximum range.");
    auto rifle=controlled(); rifle.weapon=WeaponType::Rifle; rifle.fire(); rifle.update(.12f);
    require(rifle.fire() && rifle.shots==2,"Rifle repeated fire failed.");
    require(weaponSpec(WeaponType::Rifle).range>weaponSpec(WeaponType::Pistol).range,"Weapon ranges are not distinct.");
    require(createWeapon(WeaponType::Shotgun,rifle.player).size()!=createWeapon(WeaponType::Pistol,rifle.player).size(),"Weapon models are not distinct.");

    Game snapshot;
    for (const char* display:{"DISPLAY_PISTOL_BODY","DISPLAY_SHOTGUN_BODY","DISPLAY_RIFLE_BODY","DISPLAY_PROJECTILE_0","DISPLAY_PROJECTILE_1","DISPLAY_PROJECTILE_2"}) {
        const auto initialScene=snapshot.scene();
        require(std::any_of(initialScene.begin(),initialScene.end(),[&](const SceneObject& o) { return o.id==display; }),
                "A required starting CSV representative is not actually rendered in the scene.");
    }
    CsvLogger logger;
    // Only real scene instances are exported: select/fire all weapons to observe them.
    for (auto weapon:{WeaponType::Pistol,WeaponType::Shotgun,WeaponType::Rifle}) {
        snapshot.weapon=weapon; snapshot.cooldown=0;
        require(snapshot.fire(),"Snapshot test could not fire a real projectile.");
        logger.observe(snapshot.calculationObjects(),snapshot.elapsed,snapshot.night);
    }
    std::set<std::string> ids;
    bool target=false,cargoRow=false,pistol=false,shotgunRow=false,rifleRow=false,projectile=false;
    for (const auto& o:snapshot.calculationObjects()) {
        require(ids.insert(o.id).second,"Duplicate calculation object ID.");
        target|=o.type=="Target"; cargoRow|=o.type=="Cargo"; projectile|=o.type=="Projectile";
        pistol|=o.id=="PISTOL_BODY"; shotgunRow|=o.id=="SHOTGUN_BODY"; rifleRow|=o.id=="RIFLE_BODY";
    }
    require(target&&cargoRow&&!pistol&&!shotgunRow&&rifleRow&&projectile,"Snapshot contains fabricated inactive weapon rows.");
    snapshot.projectiles.clear();
    logger.observe(snapshot.calculationObjects(),snapshot.elapsed,snapshot.night);
    const auto csvPath=std::filesystem::temp_directory_path()/"shooter-phase1-test.csv";
    logger.save(csvPath);
    std::ifstream csv(csvPath); const std::string contents((std::istreambuf_iterator<char>(csv)),{}); csv.close();
    require(contents.find("Phase,Object_ID") == 0,"CSV phase metadata missing.");
    for (const char* id:{"PISTOL_BODY","SHOTGUN_BODY","RIFLE_BODY","PROJECTILE_"})
        require(contents.find(id)!=std::string::npos,"Observed weapon/projectile missing from CSV.");
    require(contents.find("retained last actual observation")!=std::string::npos,"Transient observation lost.");
    require(contents.find("Unit Disk")==std::string::npos && contents.find("SAMPLE_PROJECTILE")==std::string::npos,"CSV contains invented geometry.");
    std::filesystem::remove(csvPath);
    require(clickedAction(Screen::Menu,150,420)==Action::Start,"Start button does not activate.");
    require(clickedAction(Screen::Menu,150,575)==Action::Controls,"Controls button does not activate.");
    require(clickedAction(Screen::Paused,150,420)==Action::Resume,"Resume button does not activate.");
    require(clickedAction(Screen::Playing,1040,40)==Action::Menu,"HUD menu button does not activate.");
    require(clickedAction(Screen::Playing,1200,40)==Action::Exit,"HUD exit button does not activate.");
    require(!buildInterface(snapshot,Screen::Controls,0,0,true).empty(),"Controls overlay is empty.");
    require(clickedAction(Screen::Playing,850,740)==Action::DayNight,"Day/night button failed.");
    require(clickedAction(Screen::Menu,1040,650)==Action::Sound,"Menu sound button failed.");

    // Each concentric band requires exactly its index+1 distinct shots, including shotgun triggers.
    for (int ring=0;ring<6;++ring) {
        auto scoring=controlled();
        for (int shot=1;shot<=ring+1;++shot) {
            scoring.applyTargetHit(0,ring,shot);
            require((scoring.destroyed==1)==(shot==ring+1),"Incorrect shots required for a scoring ring.");
        }
    }
    auto pelletDamage=controlled();
    for (int pellet=0;pellet<9;++pellet) pelletDamage.applyTargetHit(0,5,1);
    near(pelletDamage.targets[0].health,50); require(pelletDamage.hits==1,"Pellets counted as separate shots.");
    pelletDamage.applyTargetHit(0,3,1); near(pelletDamage.targets[0].health,45);
    pelletDamage.applyTargetHit(0,5,2); near(pelletDamage.targets[0].health,35);
    require(pelletDamage.hits==2,"Distinct triggers were not counted separately.");

    Target circle; circle.position={0,0,0};
    require(!std::isfinite(intersectTarget({.75f,.75f,5},{0,0,-1},circle,10).distance),"Square corner scored outside circle.");
    auto back=intersectTarget({0,0,-5},{0,0,1},circle,10);
    require(std::isfinite(back.distance) && back.ring==-1,"Back face accepted a scoring hit.");
    auto edge=intersectTarget({5,0,0},{-1,0,0},circle,10);
    require(std::isfinite(edge.distance) && edge.ring==-1,"Target edge accepted a scoring hit.");
    for (float yaw:{0.0f,90.0f,180.0f,270.0f}) for (int ring=0;ring<6;++ring) {
        circle.yaw=yaw;
        const Mat4 rotation=makeRotationY(yaw);
        const auto origin=transformPoint(rotation,{Target::radius*(ring+.5f)/6,0,5,1});
        const auto direction=transformPoint(rotation,{0,0,-1,0});
        const auto contact=intersectTarget({origin.x,origin.y,origin.z},{direction.x,direction.y,direction.z},circle,10);
        require(contact.ring==ring,"Rotated front-face ring scoring is incorrect.");
    }
    auto rearShot=controlled(); rearShot.player.position={0,1.7f,-20}; rearShot.player.yaw=90;
    require(rearShot.aimedTarget(distance)==-1,"Range finder highlighted the unprinted back.");
    rearShot.fire(); rearShot.update(.4f);
    require(rearShot.hits==0 && rearShot.targets[0].health==60 && rearShot.projectiles.empty(),"Back projectile scored or passed through target.");
    Game fast; fast.update(.1f);
    require(fast.targets[0].position.x>.8f,"Target movement is not faster than Phase 4.");
    require(Target::radius<1,"Targets were not made smaller.");
    const auto day=createLighting(false),night=createLighting(true);
    require(day.points.size()==8 && night.spots.size()==6,"Missing point/spot light sources.");
    require(day.points[0].color.x==0 && night.points[0].color.x>0,"Night lamps do not toggle.");
    require(day.ambient.x>night.ambient.x && day.sunColor.x>night.sunColor.x,"Day/night illumination is unchanged.");
    snapshot.night=true;
    bool moon=false,glowing=false;
    for (auto& o:snapshot.scene()) { moon|=o.id=="MOON_VIS"; glowing|=o.component=="Point lamp head"&&o.emission>0; }
    require(moon&&glowing,"Night environment geometry is missing.");
    for (int event=0;event<6;++event) {
        const auto samples=synthesizeSound(static_cast<SoundEvent>(event));
        require(samples.size()>500,"Sound effect has no duration.");
        float energy=0;
        for (float sample:samples) { require(std::isfinite(sample)&&std::abs(sample)<=1,"Invalid sound sample."); energy+=sample*sample; }
        require(energy>1,"Sound effect is silent.");
        require(std::abs(samples.front())<.001f && std::abs(samples.back())<.001f,"Sound envelope has a hard edge.");
    }
    std::cout<<"PASS: all six ring shot counts, pellet grouping, front-only round collision, faster/smaller targets, day/night lights and synthesized audio.\n";
    std::cout<<"PASS: moving targets, cargo seed, player/camera movement, collision, weapons, hits/respawn, range, snapshots and menu buttons.\n";
}
