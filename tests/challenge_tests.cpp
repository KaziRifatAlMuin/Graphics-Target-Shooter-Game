#include "gameplay/Game.h"
#include "persistence/CsvLogger.h"
#include "world/LevelWorld.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
using namespace shooter;
// Stop this test with its diagnostic message if an expected rule fails.
void check(bool condition,const char* message) { if (!condition) throw std::runtime_error(message); }
// Count nonzero motion amplitudes to distinguish one-, two-, and three-axis targets.
int axes(const Target& t) { return (t.motion.amplitude.x>0)+(t.motion.amplitude.y>0)+(t.motion.amplitude.z>0); }
// Advance beyond the intro and verify that gameplay actually becomes active.
void activate(Game& game) { game.update(1.51f); check(game.gameplayActive(),"Intro failed to activate level."); }
// Construct a front-facing shot fixture to exercise projectile contact with the chosen target.
void shootTarget(Game& game,std::size_t index,std::uint64_t shot) {
    auto& t=game.targets[index];
    // This helper isolates target scoring/progression. Survivors now redistribute,
    // so keep this short injected ray clear; NPC contacts are tested separately
    // below, and the player-muzzle scenario retains the full live population.
    for(auto& b:game.birds) if(b.active) b.position=b.destination={-22,6,-92};
    for(auto& h:game.humans) if(h.active) h.position=h.destination={22,0,-92};
    // Actual swept projectile from the front; the normal collision/damage pipeline applies.
    const auto normal=asVec3(transformPoint(makeRotationY(t.yaw),{0,0,1,0}));
    game.projectiles.push_back({shot,WeaponType::Pistol,t.position+normal*.3f,normal*-1,0,shot});
    game.update(.008f);
}
// Validate all seven level rules, target motion, character penalties, and progression using isolated CSV output.
int main(int argc,char** argv) {
    try {
        const int counts[]={3,3,4,6,8,10,12};
        std::size_t lastCargo=0;
        for (int n=1;n<=7;++n) {
            const auto c=levelConfiguration(n);
            check(int(c.targets.size())==counts[n-1],"Wrong target count.");
            check(c.playerMovement==(n>=4),"Wrong player movement rule.");
            if (n==1) for (const auto& t:c.targets) check(axes(t)==0,"Level 1 must be static.");
            if (n==2) check(axes(c.targets[0])==0 && axes(c.targets[2])==0 && c.targets[1].motion.amplitude.x>0,"Level 2 X motion rule.");
            if (n==3 || n==4) for (const auto& t:c.targets) check(axes(t)==1,"Single-axis movement rule.");
            if (n>=5) {
                check(axes(c.targets[6])==2 && axes(c.targets[7])==2,"Compound-axis movement missing.");
                int spinners=0; for (int i=0;i<6;++i) spinners+=c.targets[i].motion.spinSpeed!=0;
                check(spinners>=2 && c.targets[6].motion.spinSpeed!=0 && c.targets[7].motion.spinSpeed!=0,"Level 5 spin rules.");
            }
            if (n>=6) for (int i=8;i<10;++i) check(axes(c.targets[i])==3 && std::abs(c.targets[i].motion.frequency.x)>2 && c.targets[i].motion.spinSpeed!=0,"Level 6 advanced target rule.");
            if (n==7) for (int i=10;i<12;++i) check(axes(c.targets[i])==3 && std::abs(c.targets[i].motion.frequency.x)>3 && c.targets[i].motion.spinSpeed!=0,"Level 7 advanced target rule.");
            const auto cargo=generateChallengeCargo(c);
            if (n>=4) check(cargo.size()>lastCargo,"Cargo must increase through movable levels.");
            lastCargo=cargo.size();
        }
        Game game; CsvLogger logger;
        logger.observe(game.scene(),game.elapsed,game.night);
        game.startChallenge();
        std::uint64_t shot=1000;
        int expected=0,destroyed=0;
        double elapsed=0;
        for (int n=1;n<=7;++n) {
            check(game.levels.config.number==n,"Sequential progression skipped a level.");
            check(!game.nextLevel() && !game.fire(),"Intro permits firing or skipping.");
            check(game.score==expected && game.destroyed==destroyed && game.elapsed>=elapsed,"Stats lost across levels.");
            activate(game);
            const auto position=game.player.position;
            game.movePlayer(1,0,.1f,false);
            check((length(game.player.position-position)>.1f)==(n>=4),"Player movement lock failed.");
            check(!game.nextLevel(),"Incomplete level advanced.");
            if (n>=4) {
                bool obstructed=false;
                for (const auto& t:game.targets) obstructed|=!clearSightline({0,1.7f,-5},t.position,game.staticObjects);
                check(obstructed,"Cargo does not require repositioning.");
            }
            const std::size_t birds=n==5?8:n==6?16:n==7?48:0;
            check(game.birds.size()==birds && game.humans.size()==(n==7?36u:0u),"NPC population mismatch.");
            if (birds) {
                auto before=game.birds[0].position;
                game.update(.3f);
                check(length(game.birds[0].position-before)>.05f,"Bird did not move.");
                // Real projectile contacts with bird body and human torso.
                const auto& bird=game.birds[0];
                auto parts=createBirdObjects(bird,0);
                const auto p=parts.front().transform.position;
                game.projectiles.push_back({++shot,WeaponType::Pistol,p+Vec3{0,0,.3f},{0,0,-1},0,shot});
                game.update(.01f);
                expected-=100;
                check(game.score==expected && game.dangerTime>0,"Physical bird penalty failed.");
                game.applyNpcHit(false,0,shot);
                check(game.score==expected,"Same trigger double-counted a bird penalty.");
            }
            if (n==7) {
                const auto parts=createHumanObjects(game.humans[0],0); const auto p=parts.front().transform.position;
                game.projectiles.push_back({++shot,WeaponType::Pistol,p+Vec3{0,0,.25f},{0,0,-1},0,shot});
                game.update(.01f); expected-=200;
                check(game.score==expected && game.humanHits==1,"Physical human penalty failed.");
            }
            game.update(.25f);
            logger.observe(game.scene(),game.elapsed,game.night);
            for (std::size_t i=0;i<game.targets.size();++i) {
                shootTarget(game,i,++shot);
                if(!game.targets[i].eliminated) throw std::runtime_error("Swept projectile failed at level "+std::to_string(n)+" target "+std::to_string(i)+" score "+std::to_string(game.score));
                expected+=150; ++destroyed;
                check(game.score==expected,"Bullseye must award exactly 150 total.");
                if (i+1<game.targets.size()) check(game.levels.stage==LevelStage::Active,"Level completed with a target remaining.");
            }
            check(game.levelsCleared==n && !game.nextLevel(),"Completion gate / cleared count failed.");
            elapsed=game.elapsed;
            game.update(2.2f);
            check(game.elapsed==elapsed && game.remainingTargets()==0 && !game.fire(),"Completed level clock or respawn/fire failed.");
            game.restartLevel(); check(game.remainingTargets()==0,"Completed level restart permits score farming.");
            if (n<7) check(game.nextLevel(),"Completed level cannot advance.");
        }
        check(game.destroyed==46 && game.bullseyes==46 && game.score==6400 && game.levels.stage==LevelStage::Finished,"Final accumulated statistics wrong.");
        logger.save(argc>1?argv[1]:"challenge-calc.csv");
        game.startChallenge(); activate(game);
        game.applyTargetHit(0,5,++shot); check(game.score==0,"Damage alone awarded Challenge points.");
        game.applyTargetHit(0,0,++shot); check(game.score==100 && game.bullseyes==0,"Later center hit wrongly received first-shot bonus.");
        game.applyTargetHit(1,5,++shot); game.applyTargetHit(1,0,shot);
        check(game.score==250 && game.bullseyes==1,"Shotgun strongest-pellet upgrade failed.");
        game.restartLevel(); check(game.score==0 && game.elapsed==0 && game.destroyed==0 && game.shots==0,"Restart did not roll back incomplete progress.");
        activate(game); for (std::size_t i=0;i<3;++i) shootTarget(game,i,++shot);
        game.update(1.1f); check(game.nextLevel(),"Cannot reach level 2 for restart check.");
        const auto committed=game.levels.completedStats;
        activate(game); game.applyTargetHit(0,0,++shot); game.update(.1f); game.restartLevel();
        check(game.score==committed.score && game.elapsed==committed.elapsed && game.levelsCleared==1 && game.destroyed==3,"Restart erased prior levels or kept partial stats.");
        game.startChallenge(7); activate(game);
        game.applyNpcHit(false,0,++shot); game.applyNpcHit(true,0,++shot);
        check(game.score==-300 && game.destroyed==0,"Negative score / NPC separation failed.");
        for (int frame=0;frame<900;++frame) {
            game.update(1.0f/60);
            for (const auto& b:game.birds) if (b.active) {
                const auto d=b.position-b.home;
                check(std::abs(d.x)<=3.51f && std::abs(d.z)<=3.51f && b.position.y>=1.2f && b.position.y<=7.1f,"Bird escaped bounded 3D zone.");
            }
            for (const auto& h:game.humans) if (h.active) {
                const auto d=h.position-h.home;
                check(std::abs(d.x)<=3.01f && std::abs(d.z)<=3.01f && h.position.y==0,"Human escaped ground zone.");
                check(canStandAt(h.position,game.staticObjects),"Human entered cargo/boundary.");
                for (const auto& t:game.targets) check(std::abs(h.position.x-t.position.x)>=1.4f || std::abs(h.position.z-t.position.z)>=1.2f,"Human overlapped a moving target stand.");
            }
        }
        game.reset(); check(game.mode==GameMode::Practice && game.score==0 && game.birds.empty() && game.targets.size()==7,"Practice reset failed.");
        // Fire from the actual player muzzle with live motion and normal weapon timing.
        // Fixed levels use the original spawn; movable levels use validated close firing lanes.
        for (int level=1;level<=7;++level) {
            game.startChallenge(level); activate(game); game.weapon=WeaponType::Rifle;
            for (std::size_t i=0;i<game.targets.size();++i) {
                if (level>=4) {
                    const auto base=game.targets[i].base; bool found=false;
                    for (float dz:{4.f,3.f,5.f,6.f}) for (float dx:{0.f,-3.f,3.f}) {
                        const Vec3 p{base.x+dx,1.7f,base.z+dz};
                        if (!found && canStandAt(p,game.staticObjects) && clearSightline(p,game.targets[i].position,game.staticObjects)) { game.player.position=p; found=true; }
                    }
                    check(found,"No close player firing lane.");
                }
                for (int frame=0;frame<900 && !game.targets[i].eliminated;++frame) {
                    const auto& target=game.targets[i]; float travel=length(target.position-game.player.position)/65;
                    Vec3 aim=target.position;
                    for (int predict=0;predict<3;++predict) {
                        aim=target.base+movementOffset(target.motion,game.levels.levelTime+travel);
                        travel=std::max(0.f,(length(aim-game.player.position)-1.4f)/65);
                    }
                    const auto direction=normalize(aim-game.player.position);
                    game.player.yaw=std::atan2(direction.z,direction.x)*180/pi;
                    game.player.pitch=std::asin(direction.y)*180/pi;
                    game.fire(); game.update(1.f/120);
                }
                if (!game.targets[i].eliminated) throw std::runtime_error("Player-muzzle firing failed at level "+std::to_string(level)+", target "+std::to_string(i+1));
            }
            check(game.remainingTargets()==0,"Live player firing failed to complete level.");
        }
        std::cout<<"PASS: levels 1-7, reachability, movement, actual target/NPC projectile contacts, exact scores, completion gates, accumulated time, restart rollback, NPC bounds and CSV.\n";
        return 0;
    } catch (const std::exception& error) { std::cerr<<"FAIL: "<<error.what()<<'\n'; return 1; }
}
