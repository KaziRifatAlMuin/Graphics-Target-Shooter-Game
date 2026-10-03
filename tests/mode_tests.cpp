#include "gameplay/SessionController.h"
#include "ui/Interface.h"
#include "persistence/CsvLogger.h"
#include "persistence/CsvFile.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace shooter;
namespace fs=std::filesystem;
namespace {
void check(bool value,const char* message) { if (!value) throw std::runtime_error(message); }
std::string read(const fs::path& p) { std::ifstream in(p,std::ios::binary); return {(std::istreambuf_iterator<char>(in)),{}}; }
void activate(Game& game) { game.update(1.51f); check(game.gameplayActive(),"Intro did not activate."); }
void clear(Game& game) { for (std::size_t i=0;i<game.targets.size();++i) game.applyTargetHit(i,0,100+i); }
EligibleResult result(std::string name,GameMode mode,int score,double seconds) {
    RunStats stats; stats.score=score; stats.elapsed=seconds; stats.levelsCleared=mode==GameMode::Challenge?1:0;
    stats.destroyed=3; stats.bullseyes=2; return {name,mode,stats};
}
void storage(const fs::path& path) {
    // Only explicitly named test artifacts in the configured build directory are reset.
    fs::remove(path); Leaderboard board(path); board.load();
    check(board.records().empty() && read(path).find("Name,Mode,BestScore")==0,"Missing-file creation failed.");
    auto a=result("Ada, \"Ace\"",GameMode::Challenge,-100,10.125);
    check(board.submit(a),"Negative first score rejected.");
    const auto original=read(path);
    a.stats.score=-200; check(!board.submit(a) && read(path)==original,"Worse result changed the file.");
    a.stats.score=-100; a.stats.elapsed=11; check(!board.submit(a),"Slower score tie won.");
    a.stats.elapsed=9.123456789; a.stats.birdHits=7; a.stats.humanHits=2; a.stats.bullseyes=1;
    check(board.submit(a),"Faster score tie rejected.");
    Leaderboard restart(path); restart.load();
    const auto saved=restart.records().front();
    check(saved.name==a.name && saved.stats.elapsed==a.stats.elapsed && saved.stats.birdHits==7 && saved.stats.bullseyes==1 && !saved.lastUpdated.empty(),"CSV escaping/coherent stats/precision failed.");
    check(!restart.submit(a),"Exact tie rewrote personal best.");
    check(restart.submit(result(a.name,GameMode::Free,-800,180)),"Same name in different mode collided.");
    check(!restart.submit(result("early",GameMode::Free,999,179.9)),"Incomplete Free result accepted.");
    auto incomplete=result("early",GameMode::Challenge,999,10); incomplete.stats.levelsCleared=0;
    check(!restart.submit(incomplete),"Incomplete Challenge result accepted.");
    for (auto mode:{GameMode::Developer,GameMode::BirdsEye,GameMode::Practice}) check(!restart.submit(result("debug",mode,9999,180)),"Noncompetitive result accepted.");
    restart.submit(result("Alpha",GameMode::Challenge,300,20));
    restart.submit(result("Beta",GameMode::Challenge,300,15));
    restart.submit(result("Gamma",GameMode::Challenge,400,40));
    auto sorted=restart.sorted(GameMode::Challenge);
    check(sorted[0].name=="Gamma" && sorted[1].name=="Beta" && sorted[2].name=="Alpha" && restart.rank("Beta",GameMode::Challenge)==2,"Score/time sort and rank failed.");
    auto content=read(path);
    content+="Beta,Challenge,350,2,6,4,0,1,16,old\ninvalid,row\n";
    writeAtomicText(path,content); restart.load();
    check(restart.records().size()==5 && !restart.warning().empty(),"Duplicate/invalid-row loading failed.");
    auto improved=result("Beta",GameMode::Challenge,350,15); improved.stats.levelsCleared=3; improved.stats.destroyed=10;
    check(restart.submit(improved),"Duplicate-key replacement failed."); restart.load();
    check(restart.records().size()==5 && parseCsv(read(path)).size()==6,"Duplicate key survived rewrite.");
    const auto corrupt=path.string()+".corrupt"; writeAtomicText(corrupt,"unrecognized,header\nKEEP ME\n");
    bool refused=false; try { Leaderboard bad(corrupt); bad.submit(a); } catch (const std::exception&) { refused=true; }
    check(refused && read(corrupt)=="unrecognized,header\nKEEP ME\n","Corrupt file was overwritten.");
    std::cout<<"PASS: create/read/write, negative scores, score/time ties, coherent records, quoted names, mode keys, deduplication, invalid-row preservation and rank.\n";
}
void freeMode(CsvLogger& logger) {
    Game game; game.startMode(GameMode::Free); activate(game);
    check(game.targets.size()==12 && game.birds.size()==48 && game.humans.size()==36,"Free must reuse Level 7 populations.");
    game.applyNpcHit(false,0,1); game.applyNpcHit(true,0,2);
    check(game.score==-300 && game.birdHits==1 && game.humanHits==1,"Free negative penalties failed.");
    clear(game);
    check(game.destroyed==12 && game.bullseyes==12 && game.score==1500,"Free target scoring failed.");
    game.update(.01f);
    check(game.gameplayActive() && game.levelsCleared==0 && !game.takeResult(),"Clearing Free targets finished the session early.");
    game.update(27.1f); logger.observe(game.scene(),game.elapsed,false);
    bool warning=false; for (const auto& o:game.scene()) warning|=o.component=="Respawn warning";
    check(warning,"No animated pre-respawn warning.");
    game.update(2.7f); check(game.targets[0].respawn>0,"Target respawned before 30 seconds.");
    game.update(.3f);
    check(game.targets[0].health==60 && game.targets[0].damageByShot.empty() && game.remainingTargets()==12,"30-second respawn did not restore fresh targets.");
    check(game.birds.size()==48 && game.humans.size()==36,"Free NPC population disappeared with targets.");
    game.applyTargetHit(0,1,900); game.applyTargetHit(0,1,901);
    check(game.score==1600 && game.bullseyes==12,"Ordinary two-shot kill incorrectly awarded bonus.");
    game.update(151);
    check(game.levels.stage==LevelStage::Finished && game.freeRemaining==0 && game.elapsed==180,"Free countdown did not clamp to exactly 180.");
    auto completed=game.takeResult(); check(completed && completed->stats.score==1600 && completed->mode==GameMode::Free,"Free completion did not emit final stats.");
    check(!game.takeResult() && !game.fire(),"Free result duplicated or firing continued.");
    game.applyNpcHit(false,0,999); game.applyTargetHit(1,0,1000); game.update(20);
    check(game.score==1600 && game.elapsed==180,"Expired session continued scoring/time.");
    game.restartLevel(); check(game.score==0 && game.destroyed==0 && game.freeRemaining==180 && game.levels.stage==LevelStage::Intro,"Free reset failed.");
    logger.observe(game.scene(),game.elapsed,false);
    std::cout<<"PASS: full Free countdown, no early completion, 30-second respawn/warning, fresh scoring, penalties, NPC retention, expiry freeze and reset.\n";
}
void camera(CsvLogger& logger) {
    Game game; game.startMode(GameMode::BirdsEye);
    check(game.targets.size()==12 && game.birdEye.overhead.position.y==120 && game.birdEye.overhead.pitch<-89,"Not a true overhead Level 7 view.");
    const float aspect=1.6f;
    bool chosen=false;
    for (float z=-10;z>=-90;z-=10) for (float x=-20;x<=20;x+=5) {
        Vec3 ground{x,0,z};
        const auto clip=transformPoint(makePerspective(60,aspect,.05f,400)*game.birdEye.overhead.getViewMatrix(),{x,0,z,1});
        float u=(clip.x/clip.w+1)*.5f,v=(1-clip.y/clip.w)*.5f;
        const auto picked=game.birdEye.groundPoint(u,v,aspect);
        check(picked && length(*picked-ground)<.002f,"Picking disagrees with renderer view/projection.");
        if (!chosen && game.birdEye.observeAt(u,v,aspect,game.staticObjects,game.targets)) {
            check(length(game.activeCamera().position-(ground+Vec3{0,1.7f,0}))<.002f,"Observer was not placed at clicked location."); chosen=true;
        }
    }
    check(chosen,"Could not find a valid observation position.");
    const auto before=game.activeCamera(); game.birdEye.observation.look(100,-50);
    check(game.activeCamera().yaw!=before.yaw && game.activeCamera().pitch!=before.pitch,"Observation mouse look failed.");
    for (const auto& o:game.staticObjects) if (o.type=="Cargo") check(!game.birdEye.validPosition(o.transform.position,game.staticObjects,game.targets),"Observer can enter cargo.");
    check(!game.birdEye.validPosition({40,0,-50},game.staticObjects,game.targets),"Observer can leave walls.");
    check(!game.birdEye.observeAt(-1,.5f,aspect,game.staticObjects,game.targets),"Invalid screen pick accepted.");
    game.birdEye.moveObservation(1,0,100,true,game.staticObjects,game.targets);
    check(game.birdEye.validPosition(game.birdEye.observation.position,game.staticObjects,game.targets),"Observation movement crossed blocking geometry.");
    game.applyTargetHit(0,0,1); game.applyNpcHit(false,0,2);
    check(!game.fire() && game.score==0 && !game.takeResult(),"Bird's-Eye can score/shoot/save.");
    logger.observe(game.scene(),game.elapsed,false);
    game.birdEye.overview(); game.birdEye.pan(1,1,.1f); game.birdEye.zoom(1);
    check(!game.birdEye.observing && game.activeCamera().position.y==112 && game.targets.size()==12,"Return/pan/zoom changed world state.");
    logger.observe(game.scene(),game.elapsed,false);
    std::cout<<"PASS: overhead view, independent project/unproject checks, safe observation placement/movement, yaw/pitch, return/pan/zoom, noncompetitive inspection.\n";
}
void sharedWorld() {
    Game reference; reference.startChallenge(7);
    for (auto mode:{GameMode::Free,GameMode::Developer,GameMode::BirdsEye}) {
        Game game; game.startMode(mode,7);
        check(game.staticObjects.size()==reference.staticObjects.size(),"Mode replaced the shared arena.");
        for (std::size_t i=0;i<game.staticObjects.size();++i) {
            const auto& a=game.staticObjects[i]; const auto& b=reference.staticObjects[i];
            check(a.id==b.id && composeModelMatrix(a.transform).data==composeModelMatrix(b.transform).data,"Mode changed shared Level 7 geometry.");
        }
        for (std::size_t i=0;i<game.targets.size();++i) {
            const auto& a=game.targets[i]; const auto& b=reference.targets[i];
            check(length(a.position-b.position)==0 && length(a.motion.amplitude-b.motion.amplitude)==0 &&
                length(a.motion.frequency-b.motion.frequency)==0 && a.motion.spinSpeed==b.motion.spinSpeed,"Mode changed shared target positions/patterns.");
        }
    }
    std::cout<<"PASS: Free, Developer Level 7 and Bird's-Eye use identical arena/cargo transforms and target configurations.\n";
}
void failedSave(const fs::path& path) {
    fs::remove(path); Leaderboard board(path); board.load(); Game game; SessionController session(game,board);
    auto temporary=path; temporary+=".tmp";
    fs::create_directory(temporary); // Deliberately prevent an atomic-save temporary file from opening.
    session.ui.name="Save Test"; session.action(Action::Start); session.action(Action::ConfirmChallenge); session.update(1.51f); clear(game); session.update(.01f);
    check(session.ui.screen==Screen::LevelComplete && session.ui.saveFailed && board.records().empty(),"Save failure lost playable result/error state.");
    fs::remove(temporary); session.action(Action::RetrySave);
    check(!session.ui.saveFailed && session.ui.personalBest && board.records().size()==1 && board.records()[0].stats.score==450,"Retry failed to persist retained completed stats.");
    session.action(Action::RetrySave); check(board.records().size()==1,"Retry duplicated a completed result.");
    std::cout<<"PASS: failed atomic save preserves results and supports retry without duplicate records.\n";
}
void sessions(const fs::path& path,CsvLogger& logger) {
    writeAtomicText(path,"Name,Mode,BestScore,LevelsCleared,TargetsDestroyed,Bullseyes,BirdKills,HumanKills,BestTimeSeconds,LastUpdated\n");
    Game game; Leaderboard board(path); SessionController session(game,board);
    session.ui.name="  Test Pilot  "; session.acceptName(); check(session.ui.name=="Test Pilot","Name trimming failed.");
    session.action(Action::Start); session.action(Action::ConfirmChallenge); session.update(1.51f); session.update(.2f);
    logger.observe(game.scene(),game.elapsed,false);
    const double elapsed=game.elapsed; session.action(Action::Menu); session.update(100);
    check(game.elapsed==elapsed && board.records().empty(),"Pause advanced time or submitted incomplete run.");
    session.action(Action::Resume); clear(game); session.update(.01f);
    check(session.ui.screen==Screen::LevelComplete && board.records().size()==1 && board.records()[0].stats.levelsCleared==1,"Cleared Challenge level not saved.");
    const auto committed=read(path); session.action(Action::NextLevel);
    check(game.levels.config.number==1,"Next bypassed completion transition.");
    session.update(1.1f); session.action(Action::NextLevel); session.update(1.51f);
    game.applyTargetHit(0,0,500); session.update(.1f); session.action(Action::Menu); session.action(Action::Menu);
    check(read(path)==committed,"Incomplete second level replaced completed snapshot.");
    session.action(Action::Free); session.action(Action::ConfirmChallenge); session.update(1.51f); session.update(10); session.action(Action::Menu); session.update(200);
    check(game.freeRemaining>169 && board.sorted(GameMode::Free).empty(),"Paused/incomplete Free was persisted.");
    session.action(Action::Resume); session.update(171);
    check(session.ui.screen==Screen::FreeComplete && board.sorted(GameMode::Free).size()==1,"Full Free did not show and save result.");
    const auto competitive=read(path);
    const int counts[]={3,3,4,6,8,10,12};
    for (int level=1;level<=7;++level) {
        session.action(Action::Developer); session.action(Action::ConfirmChallenge);
        const auto buttons=screenButtons(Screen::Developer,game.mode);
        const auto& card=buttons.at(level-1);
        session.action(clickedAction(Screen::Developer,card.x+20,card.y+20,game.mode));
        check(game.mode==GameMode::Developer && game.targets.size()==std::size_t(counts[level-1]) && game.levels.config.number==level,"Developer card did not load shared level.");
        session.update(1.51f);
        const auto p=game.player.position; game.movePlayer(1,0,.1f,false);
        check((length(game.player.position-p)>.1f)==(level>=4),"Developer movement rule differs from Challenge.");
        logger.observe(game.scene(),game.elapsed,false); clear(game); session.update(.01f);
        check(session.ui.screen==Screen::LevelComplete && !game.nextLevel() && !game.takeResult(),"Developer progressed or emitted competitive result.");
        session.action(Action::Replay); check(game.score==0 && game.remainingTargets()==counts[level-1],"Developer reset did not restore exact level.");
    }
    check(read(path)==competitive,"Developer updated leaderboard.");
    game.startChallenge(7); activate(game); clear(game); game.update(.01f); check(!game.takeResult(),"Direct level skipping emitted competitive result.");
    for (int i=0;i<15;++i) board.submit(result("Scroll "+std::to_string(i),GameMode::Challenge,1000+i,20));
    session.action(Action::Menu); session.action(Action::Leaderboard); session.action(Action::BoardChallenge);
    session.action(Action::ScrollDown); check(session.ui.firstRow==7,"Leaderboard page down failed.");
    session.scroll(999); check(session.ui.firstRow==int(session.ui.rows.size())-leaderboardVisibleRows,"Scroll did not clamp at last row.");
    session.scroll(-999); check(session.ui.firstRow==0,"Scroll did not clamp at first row.");
    check(!buildInterface(game,session.ui.screen,0,0,true,&session.ui).empty(),"Leaderboard UI empty.");
    std::cout<<"PASS: completion-only saves, pause, partial Challenge preservation, completed Free, all seven Developer cards/movement/reset, menu flow and leaderboard scrolling.\n";
}
}
int main(int argc,char** argv) {
    try {
        check(argc>=2,"Pass a build-directory fixture path."); const fs::path path=argv[1];
        if (argc==3 && std::string(argv[2])=="--persist-write") {
            fs::remove(path); Leaderboard b(path); b.load(); b.submit(result("Restart, \"Pilot\"",GameMode::Free,-200,180));
            b.submit(result("Restart, \"Pilot\"",GameMode::Challenge,450,1.23456789)); return 0;
        }
        if (argc==3 && std::string(argv[2])=="--persist-read") {
            Leaderboard b(path); b.load(); check(b.records().size()==2 && b.rank("Restart, \"Pilot\"",GameMode::Free)==1,"Restart lost logical keys.");
            check(b.sorted(GameMode::Challenge)[0].stats.elapsed==1.23456789 && b.sorted(GameMode::Free)[0].stats.score==-200,"Restart changed stats.");
            check(!b.submit(result("Restart, \"Pilot\"",GameMode::Challenge,450,2)),"Restart lost best-score comparison.");
            std::cout<<"PASS: a separate process reloaded both modes, exact stats, quoted name and comparison behavior.\n"; return 0;
        }
        CsvLogger logger; storage(path); sharedWorld(); freeMode(logger); sessions(path.string()+".sessions",logger); camera(logger); failedSave(path.string()+".retry");
        logger.save(path.string()+".calc.csv");
        return 0;
    } catch (const std::exception& e) { std::cerr<<"FAIL: "<<e.what()<<'\n'; return 1; }
}
