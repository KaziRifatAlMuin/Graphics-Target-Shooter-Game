#include "gameplay/Game.h"
#include "gameplay/Effects.h"
#include "persistence/SnapshotWriter.h"
#include "persistence/CsvFile.h"
#include "audio/Sound.h"
#include "ui/Presentation.h"
#include "gameplay/SessionController.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
using namespace shooter;
namespace fs=std::filesystem;
namespace {
// Throw a readable failure when an expected release behavior is not satisfied.
void check(bool value,const char* message) { if (!value) throw std::runtime_error(message); }
// Read an entire test artifact so its saved contents can be compared.
std::string read(const fs::path& path) { std::ifstream f(path); return {(std::istreambuf_iterator<char>(f)),{}}; }
// Check human size, safe walking positions, and behavior around target stands.
void humans() {
    Game game; game.startMode(GameMode::Developer,7); game.update(1.51f);
    int obstructions=0;
    for (std::size_t i=0;i<game.humans.size();++i) {
        const auto& h=game.humans[i];
        check(h.height>=dimensions::humanMinHeight && h.height<=dimensions::humanMaxHeight,"Human height outside reference bounds.");
        float top=0; NpcCollider collider{true,i,{}};
        for (const auto& part:createHumanObjects(h,i)) {
            check(part.parent=="HUMAN_"+std::to_string(i),"Human component lost its assembly parent.");
            collider.parts.push_back(part.transform);
            for (float x:{-.5f,.5f}) for (float y:{-.5f,.5f}) for (float z:{-.5f,.5f})
                top=std::max(top,transformPoint(composeModelMatrix(part.transform),{x,y,z,1}).y);
        }
        check(std::abs(top-h.height)<.001f,"Rendered human height disagrees with its definition.");
        // Look from a reachable near-side firing lane at the lower portion of the real zone target.
        const auto& target=game.targets[h.zone];
        for (float dy:{-.65f,-.4f,0.f}) {
            Vec3 aim=target.position+Vec3{0,dy,0},eye{h.position.x,dimensions::eyeHeight,h.position.z+5};
            auto direction=normalize(aim-eye);
            const auto contact=intersectTarget(eye,direction,target,20);
            if (contact.ring>=0 && intersectNpcs(eye,direction,contact.distance,{collider}).collider>=0) ++obstructions;
        }
    }
    check(obstructions>0,"Level 7 humans never obscure any actual target firing lane.");
    // A ray at adult head height now strikes the human before the target behind it.
    Human human; human.position={0,0,-10}; human.home=human.position; human.height=dimensions::humanHeight;
    NpcCollider collider{true,0,{}};
    for (const auto& part:createHumanObjects(human,0)) collider.parts.push_back(part.transform);
    Target target; target.position=target.base={0,1.9f,-14}; target.movement=3;
    std::vector<Projectile> shots{{1,WeaponType::Rifle,{0,1.9f,-5},{0,0,-1},0,41}};
    int targets=0,people=0;
    updateProjectiles(shots,{target},{},.1f,[&](std::size_t,int,std::uint64_t) { ++targets; },{collider},
        [&](bool isHuman,std::size_t,std::uint64_t) { if(isHuman) ++people; });
    check(shots.empty() && people==1 && targets==0,"Visible human did not intercept the projectile.");
    game.applyNpcHit(true,0,99); game.applyNpcHit(true,0,99);
    check(game.score==-200 && game.humanHits==1 && game.dangerHuman && game.dangerTime>1,"Human penalty duplicated or feedback too weak.");
    check(game.soundEvents.back()==SoundEvent::HumanPenalty,"Human penalty reused weaker bird cue.");
    std::cout<<"PASS: proportional adult humans, actual Level 7 target obstruction, projectile interception and single stronger penalty.\n";
}
// Verify crate dimensions and stacked layouts against the shared human-size reference.
void cargo() {
    const auto parts=generateCargoLayout(2107042);
    std::set<std::string> crates;
    std::map<std::string,int> heights;
    for (const auto& p:parts) if (p.component=="Crate") {
        check(std::abs(p.transform.scale.y-dimensions::humanHeight/3)<.0001f,"Crate/human reference ratio changed.");
        crates.insert(p.id); ++heights[p.id.substr(0,p.id.rfind('_'))];
    }
    for (const auto& p:parts) check(crates.count(p.parent)!=0,"Cargo detail lacks its real parent crate.");
    int low=0,middle=0,high=0;
    for (const auto& h:heights) { check(h.second>=1 && h.second<=5,"Stack out of bounds."); if(h.second==1) ++low; else if(h.second==5) ++high; else ++middle; }
    check(low && high && middle>low+high,"Stack distribution lost middle-height tendency.");
}
// Verify current and retained scene observations plus asynchronous snapshot writing.
void snapshots(const fs::path& path) {
    Game game; CsvLogger logger;
    game.startMode(GameMode::Developer,1); auto first=game.scene(); logger.observe(first,0,false);
    game.startMode(GameMode::Developer,7); auto last=game.scene(); logger.observe(last,2,false);
    auto snapshot=logger.snapshot(); std::set<std::string> ids;
    for (const auto& p:snapshot) check(ids.insert(p.id).second,"Duplicate snapshot ID.");
    check(snapshot.size()==first.size()+last.size(),"A visited static or dynamic component was discarded.");
    for (const auto& p:snapshot) check(p.retainedObservation==(p.level==1),"Current/history metadata incorrect.");
    {
        SnapshotWriter writer(path);
        writer.submit(snapshot);
        auto final=makeCube("LATEST","Test","Latest exact transform",{1,2,3},{2,3,4},{.1f,.2f,.3f});
        final.transform.shear[0]=.2f; final.transform.rotation={10,20,30}; final.observedTime=42;
        writer.submit({final}); writer.flush();
        const auto rows=parseCsv(read(path));
        check(rows.size()==2 && rows[1][1]=="LATEST" && rows[1].size()==29 && rows[1].back()=="42.000000","Worker did not flush the latest complete snapshot/schema.");
    }
    {
        SnapshotWriter writer(path); writer.submit({makeCube("SHUTDOWN","Test","Flush on shutdown",{},{1,1,1},{1,1,1})});
    }
    check(read(path).find("SHUTDOWN")!=std::string::npos,"Writer shutdown dropped pending scene.");
    bool failed=false;
    try { SnapshotWriter writer(path/"missing"/"calc.csv"); writer.submit({}); writer.flush(); }
    catch(const std::exception&) { failed=true; }
    check(failed,"CSV write failure was silently reported as success.");
    std::cout<<"PASS: every component retained across visited levels, complete schema, latest snapshot, shutdown flush and write errors.\n";
}
// Check intro/result timing and generated presentation geometry and sound cues.
void presentation() {
    for (int i=0;i<int(SoundEvent::Count);++i) {
        const auto samples=synthesizeSound(SoundEvent(i)); float energy=0;
        for (auto value:samples) { check(std::isfinite(value)&&std::abs(value)<=1,"Invalid audio sample."); energy+=value*value; }
        check(energy>1,"Silent presentation cue.");
    }
    check(synthesizeSound(SoundEvent::HumanPenalty).size()>synthesizeSound(SoundEvent::Penalty).size(),"Human cue is not distinct/longer.");
    check(celebrationObjects(.5f,true).size()>celebrationObjects(.5f,false).size(),"Final victory lacks stronger celebration.");
    Game game; game.startChallenge(); game.update(.5f);
    ui::Painter p; UiState state;
    check(drawPresentation(p,game,Screen::Playing,state) && game.elapsed==0,"Intro animation missing or consuming play time.");
    game.update(1.1f);
    for (std::size_t i=0;i<game.targets.size();++i) game.applyTargetHit(i,0,100+i);
    game.update(.02f); const auto time=game.elapsed; game.update(.4f);
    check(drawPresentation(p,game,Screen::LevelComplete,state) && game.elapsed==time,"Completion animation consumes play time.");
    game.update(1);
    check(!drawPresentation(p,game,Screen::LevelComplete,state),"Completion animation never reveals results.");
}
// Check fatal character hits, falling poses, retained bodies, and single-shot penalties.
void deaths() {
    Game g; g.startMode(GameMode::Free); g.update(1.51f);
    const auto living=createHumanObjects(g.humans[0],0);
    const float spacing=length(living[0].transform.position-living[1].transform.position);
    g.applyNpcHit(true,0,1); g.applyNpcHit(false,0,2);
    check(g.score==-300 && !g.humans[0].active && !g.birds[0].active,"Fatal hit did not remove live collider.");
    check(g.humans[0].dying && g.birds[0].dying && g.debris.size()==38,"Death burst is absent or unbounded.");
    check(std::count(g.soundEvents.begin(),g.soundEvents.end(),SoundEvent::HumanDie)==1 &&
          std::count(g.soundEvents.begin(),g.soundEvents.end(),SoundEvent::BirdDie)==1,"Death cues absent.");
    g.applyNpcHit(true,0,999); g.applyNpcHit(false,0,999);
    check(g.score==-300 && g.humanHits==1 && g.birdHits==1,"Dead NPC awarded another penalty.");
    CsvLogger logger; logger.observe(g.scene(),g.elapsed,false);
    for(int i=0;i<60;++i) {
        g.update(1.f/60);
        const auto body=createHumanObjects(g.humans[0],0);
        check(std::abs(length(body[0].transform.position-body[1].transform.position)-spacing)<.001f,"Falling human parts disconnect.");
        for(const auto& part:body) for(float x:{-.5f,.5f}) for(float y:{-.5f,.5f}) for(float z:{-.5f,.5f})
            check(transformPoint(composeModelMatrix(part.transform),{x,y,z,1}).y>=.014f,"Human sinks through floor.");
    }
    g.update(2); check(g.humans[0].dead && g.birds[0].dead && g.debris.empty(),"Fall/blood never settle/expire.");
    for(const auto& part:createBirdObjects(g.birds[0],0)) for(float x:{-.5f,.5f}) for(float y:{-.5f,.5f}) for(float z:{-.5f,.5f}) {
        const float height=transformPoint(composeModelMatrix(part.transform),{x,y,z,1}).y;
        check(height>=.014f && height<.9f,"Fallen bird should rest on the ground with folded wings.");
    }
    for(std::size_t i=0;i<g.targets.size();++i) g.applyTargetHit(i,0,100+i);
    g.update(31);
    check(g.targets[0].health==60 && g.humans[0].dead && g.birds[0].dead,"Target respawn resurrected NPCs.");
    logger.observe(g.scene(),g.elapsed,false); bool blood=false,corpse=false;
    for(const auto& o:logger.snapshot()) { blood|=o.type=="Blood"; corpse|=o.id.find("DEAD_HUMAN_")!=std::string::npos; }
    check(blood&&corpse,"Blood/corpse actual transforms missing from CSV snapshots.");
    for(std::size_t i=0;i<g.humans.size();++i) g.applyNpcHit(true,i,1000+i);
    for(std::size_t i=0;i<g.birds.size();++i) g.applyNpcHit(false,i,2000+i);
    check(g.debris.size()<=250,"Simultaneous deaths exceed particle budget.");
    g.update(3); check(g.debris.empty(),"Particles leak after a mass hit.");
    g.restartLevel(); check(g.birds[0].active && !g.birds[0].dead && g.humans[0].active && g.score==0,"Explicit session restart failed to reset NPCs.");
    g.startMode(GameMode::Developer,7); g.update(1.51f); g.applyNpcHit(true,0,1); g.applyNpcHit(false,0,2);
    for(std::size_t i=0;i<g.targets.size();++i) g.applyTargetHit(i,0,10+i);
    g.update(.01f); auto elapsed=g.elapsed; g.update(3);
    check(g.elapsed==elapsed && g.birds[0].dead && g.humans[0].dead,"Results screen freezes falls or advances score clock.");
    std::cout<<"PASS: fatal NPC hits, coherent grounded falls, death audio, permanent absence, bounded expiring blood and cosmetic result updates.\n";
}
// Exercise name entry and duplicate-name confirmation against isolated leaderboard records.
void names(const fs::path& path) {
    auto file=path.string()+".names.csv"; fs::remove(file);
    Leaderboard board(file); board.load(); RunStats best; best.score=900; best.levelsCleared=2; best.elapsed=25;
    board.submit({"Ace",GameMode::Challenge,best});
    best.elapsed=180; best.levelsCleared=0;
    check(board.submit({"FreeOnly",GameMode::Free,best}),"Free name fixture was not eligible.");
    const auto preserved=read(file); Game game; SessionController session(game,board);
    session.ui.name="  Ace  "; session.action(Action::Start); session.update(10);
    check(session.ui.screen==Screen::ChallengeName && session.ui.nameExistsWarning && session.ui.existingRank==1 && game.elapsed==0,"Existing normalized Challenge name not warned before play.");
    session.action(Action::ConfirmChallenge);
    check(session.ui.screen==Screen::ChallengeName && session.ui.duplicateConfirmation && read(file)==preserved,"Duplicate name bypassed explicit confirmation or changed board.");
    session.action(Action::RewriteName); session.action(Action::ConfirmChallenge);
    check(session.ui.screen==Screen::ChallengeName && !session.ui.message.empty(),"Blank Challenge name starts gameplay.");
    for(char c:std::string("FreeOnly")) session.type(c);
    check(!session.ui.nameExistsWarning,"Free record collided with Challenge name.");
    session.action(Action::ConfirmChallenge);
    check(session.ui.screen==Screen::Playing && game.playerName=="FreeOnly","New name failed to launch Level 1.");
    session.action(Action::Replay); session.action(Action::RewriteName);
    for(char c:std::string("Ace")) session.type(c);
    session.action(Action::ConfirmChallenge); check(session.ui.duplicateConfirmation,"Replay skipped name confirmation.");
    session.action(Action::UseExistingName); session.update(1.51f);
    for(std::size_t i=0;i<game.targets.size();++i) game.applyTargetHit(i,0,10+i);
    session.update(.02f); check(read(file)==preserved,"Inferior eligible run overwrote existing best.");
    Leaderboard restarted(file); restarted.load();
    check(restarted.records().size()==2 && restarted.sorted(GameMode::Challenge)[0].stats.score==900,"Confirmation broke mode key/restart persistence.");
    std::cout<<"PASS: required name entry, trimmed same-mode duplicate warning, separate confirmation/rewrite, blank rejection and best-record preservation.\n";
}
// Verify day/night source settings, surviving-character redistribution, and required player names.
void lightingAndSurvivors(const fs::path& path) {
    const auto day=createLighting(false),night=createLighting(true);
    check(day.sunColor.x>0 && length(night.sunColor)==0,"Sun must switch off at night.");
    for(const auto& light:day.points) check(length(light.color)==0,"Day point lamp is on.");
    for(const auto& light:day.spots) check(length(light.color)==0,"Day floodlight is on.");
    for(const auto& light:night.spots) check(light.direction.y<0 && std::abs(light.position.x)>=28,"Floodlight is not a downward boundary tower.");
    Game g; g.startMode(GameMode::Developer,7); g.update(1.51f);
    g.applyNpcHit(false,0,900); g.applyNpcHit(true,0,901);
    for(std::size_t t=0;t+1<g.targets.size();++t) {
        g.applyTargetHit(t,0,1000+t); g.update(.01f);
        std::vector<int> birds(g.targets.size()),humans(g.targets.size());
        int aliveBirds=0,aliveHumans=0;
        for(const auto& b:g.birds) if(b.active) { ++aliveBirds; ++birds[b.zone]; }
        for(const auto& h:g.humans) if(h.active) { ++aliveHumans; ++humans[h.zone]; }
        check(aliveBirds==47 && aliveHumans==35,"Clearing a target removed surviving NPCs.");
        for(std::size_t z=0;z<g.targets.size();++z) if(!g.targets[z].eliminated)
            check(birds[z]<=10 && humans[z]<=10,"Live target exceeded NPC capacity.");
        check(!g.birds[0].active && !g.humans[0].active,"Redistribution revived a killed NPC.");
    }
    Leaderboard board(path.string()+".required-name.csv");
    Game blank; SessionController session(blank,board);
    check(session.ui.name.empty(),"Default player name still exists.");
    for(auto mode:{Action::Free,Action::Start,Action::Developer,Action::BirdsEye,Action::Practice}) {
        session.action(mode); session.action(Action::ConfirmChallenge);
        check(session.ui.screen==Screen::ChallengeName && session.ui.editingName,"Blank name bypassed registration.");
        session.type(' '); session.action(Action::ConfirmChallenge);
        check(session.ui.screen==Screen::ChallengeName,"Whitespace name bypassed registration.");
        session.action(Action::RewriteName);
    }
    session.type('A'); session.action(Action::ConfirmChallenge);
    check(session.ui.screen==Screen::Playing && blank.playerName=="A","Typed name did not launch requested mode.");
    std::cout<<"PASS: day/night lights, boundary floodlights, survivor reassignment/caps, required names in every mode.\n";
}
}
// Run release regression scenarios and return failure if any assertion throws.
int main(int argc,char** argv) {
    try { check(argc==2,"Expected isolated CSV test path."); humans(); cargo(); snapshots(argv[1]); presentation(); deaths(); names(argv[1]); lightingAndSurvivors(argv[1]);
        std::cout<<"PASS: reference-sized cargo, final presentation clocks and all audio cues.\n"; return 0;
    } catch(const std::exception& e) { std::cerr<<"FAIL: "<<e.what()<<'\n'; return 1; }
}
