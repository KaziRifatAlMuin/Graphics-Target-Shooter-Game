#pragma once
#include "Game.h"
#include "Interface.h"

void gameTests() {
    const auto cargo=generateCargoLayout(2107042), same=generateCargoLayout(2107042), other=generateCargoLayout(17);
    require(!cargo.empty() && cargo.size()==same.size(),"Cargo generation is not deterministic.");
    bool changed=cargo.size()!=other.size();
    for (std::size_t i=0;i<cargo.size();++i) {
        near(cargo[i].transform.position.x,same[i].transform.position.x);
        near(cargo[i].transform.position.z,same[i].transform.position.z);
        require(std::abs(cargo[i].transform.position.x)>16,"Cargo obstructs the center lane.");
        if (i<other.size()) changed|=cargo[i].transform.position.x!=other[i].transform.position.x;
    }
    require(changed,"Cargo seed has no effect.");
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
    require(hit.hits==1 && hit.targets[0].health==60,"Swept projectile did not hit the target.");
    require(hit.projectiles.empty(),"Projectile survived impact.");
    hit.fire(); hit.update(.3f); hit.fire(); hit.update(.3f);
    require(hit.destroyed==1 && hit.targets[0].respawn>0,"Target destruction did not trigger respawn.");
    hit.update(2);
    require(hit.targets[0].health==100 && hit.targets[0].respawn==0,"Target did not reset after hit.");

    auto blocked=controlled();
    obstacle.transform.position={0,1.7f,-9}; obstacle.transform.scale={4,4,.2f}; blocked.staticObjects.push_back(obstacle);
    require(blocked.aimedTarget(distance)<0,"Range finder saw through cover.");
    blocked.fire(); blocked.update(.5f);
    require(blocked.hits==0 && blocked.projectiles.empty(),"Projectile passed through cover.");

    auto nearest=controlled();
    auto farther=nearest.targets[0]; farther.base.z=-20; farther.position=farther.base;
    nearest.targets.insert(nearest.targets.begin(),farther);
    nearest.fire(); nearest.update(.3f);
    require(nearest.targets[0].health==100 && nearest.targets[1].health==60,"Collision did not choose the nearest target.");

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
    std::set<std::string> ids;
    bool target=false,cargoRow=false,pistol=false,shotgunRow=false,rifleRow=false,projectile=false;
    for (const auto& o:snapshot.calculationObjects()) {
        require(ids.insert(o.id).second,"Duplicate calculation object ID.");
        target|=o.type=="Target"; cargoRow|=o.type=="Cargo"; projectile|=o.type=="Projectile";
        pistol|=o.id=="PISTOL_BODY"; shotgunRow|=o.id=="SHOTGUN_BODY"; rifleRow|=o.id=="RIFLE_BODY";
    }
    require(target&&cargoRow&&pistol&&shotgunRow&&rifleRow&&projectile,"CSV snapshot is missing a major object category.");
    require(clickedAction(Screen::Menu,150,420)==Action::Start,"Start button does not activate.");
    require(clickedAction(Screen::Menu,150,490)==Action::Controls,"Controls button does not activate.");
    require(clickedAction(Screen::Paused,150,420)==Action::Resume,"Resume button does not activate.");
    require(clickedAction(Screen::Playing,1040,40)==Action::Menu,"HUD menu button does not activate.");
    require(clickedAction(Screen::Playing,1200,40)==Action::Exit,"HUD exit button does not activate.");
    require(!buildInterface(snapshot,Screen::Controls,0,0,true).empty(),"Controls overlay is empty.");
    std::cout<<"PASS: moving targets, cargo seed, player/camera movement, collision, weapons, hits/respawn, range, snapshots and menu buttons.\n";
}
