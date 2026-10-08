#include "testing/ModeSmoke.h"
#include "ui/Interface.h"
#include <iostream>
#include <stdexcept>
namespace shooter {
namespace {
void require(bool condition,const char* message) { if (!condition) throw std::runtime_error(message); }
void click(SessionController& session,Game& game,Action action) {
    for (const auto& b:screenButtons(session.ui.screen,game.mode)) if (b.action==action) {
        session.action(clickedAction(session.ui.screen,b.x+b.w/2,b.y+b.h/2,game.mode));
        if(action==Action::Free || action==Action::Developer || action==Action::BirdsEye) { session.action(Action::ConfirmChallenge); if(session.ui.duplicateConfirmation) session.action(Action::UseExistingName); } return;
    }
    throw std::runtime_error("Smoke: requested button is absent.");
}
}
// Drive scripted session actions at specific frames to exercise Free, Developer, and Bird's-Eye modes.
float modeSmokeStep(int frame,SessionController& session,Game& game,Leaderboard& board) {
    constexpr int counts[]={3,3,4,6,8,10,12};
    if (frame==0) { session.action(Action::Menu); click(session,game,Action::Developer); }
    if (frame>=1 && frame<=700) {
        const int step=(frame-1)%100,level=(frame-1)/100+1;
        if (step==0) {
            if (level>1) { session.action(Action::Developer); session.action(Action::ConfirmChallenge); }
            click(session,game,Action(int(Action::Level1)+level-1));
            require(game.mode==GameMode::Developer && game.targets.size()==std::size_t(counts[level-1]),"Developer card loaded wrong level.");
        }
        if (step==94) {
            require(game.gameplayActive(),"Developer intro did not finish.");
            game.setCamera(2);
            for (std::size_t i=0;i<game.targets.size();++i) game.applyTargetHit(i,0,50000+level*100+i);
        }
    }
    if (frame==701) { session.action(Action::Menu); click(session,game,Action::Free); }
    if (frame==792) {
        game.setCamera(4);
        game.freeCamera.position=game.humans.front().position+Vec3{0,1.4f,4};
        game.freeCamera.yaw=-90; game.freeCamera.pitch=-5;
    }
    if (frame==793) game.setCamera(1);
    if (frame==794) {
        require(game.gameplayActive(),"Free intro did not finish.");
        for (std::size_t i=0;i<game.targets.size();++i) game.applyTargetHit(i,0,60000+i);
        game.applyNpcHit(false,0,70000); game.applyNpcHit(true,0,70001);
        require(game.score==1500,"Free target/penalty scoring failed.");
        game.setCamera(4); game.freeCamera.position=game.humans[0].position+Vec3{3,3,6};
        game.freeCamera.yaw=-116.565f; game.freeCamera.pitch=-16;
        return .1f;
    }
    if (frame==797) game.setCamera(1);
    if (frame==822) require(game.targets[0].respawn>0 && !game.targets[0].eliminated,"Free target respawned early.");
    if (frame==825) {
        require(game.targets[0].health==60 && game.targets[0].respawn<=0,"Free target did not respawn after 30 seconds.");
        require(game.birds.size()==48 && game.humans.size()==36,"Free NPC zones were removed.");
        require(game.birds[0].dead && game.humans[0].dead && !game.birds[0].active && !game.humans[0].active,"Dead NPC respawned with a target.");
        game.applyTargetHit(0,0,80000);
    }
    if (frame>=794 && frame<976) return 1; // Full 180 seconds of shared simulation, accelerated for automation.
    if (frame==977) {
        require(session.ui.screen==Screen::FreeComplete && game.elapsed==180 && game.freeRemaining==0,"Free did not finish at 180 seconds.");
        require(board.rank(game.playerName,GameMode::Free)>0,"Completed Free result was not persisted.");
    }
    if (frame==980) { session.action(Action::Menu); click(session,game,Action::BirdsEye); }
    if (frame==982) {
        bool picked=false;
        for (float v=.4f;v<.7f && !picked;v+=.02f) for(float u=.4f;u<.6f && !picked;u+=.02f)
            picked=game.birdEye.observeAt(u,v,1.6f,game.staticObjects,game.targets);
        require(picked && game.activeCamera().position.y==1.7f,"Bird's-Eye click-to-observe failed.");
        game.birdEye.observation.look(100,-35);
        require(!game.fire(),"Observation mode must not shoot.");
    }
    if (frame==987) { session.action(Action::Overview); game.birdEye.pan(1,1,.1f); game.birdEye.zoom(1); }
    if (frame==990) { session.action(Action::Menu); session.action(Action::Menu); click(session,game,Action::Leaderboard); }
    if (frame==992) click(session,game,Action::BoardFree);
    if(frame==995) { session.action(Action::Menu); session.action(Action::Start); }
    if(frame==997) { click(session,game,Action::ConfirmChallenge); require(session.ui.duplicateConfirmation,"Existing name did not require confirmation."); }
    if(frame==999) { click(session,game,Action::RewriteName); for(char c:std::string("New Pilot")) session.type(c); require(!session.ui.nameExistsWarning,"Rewrite did not clear duplicate warning."); }
    if(frame==1003) { session.action(Action::RewriteName); for(char c:game.playerName) session.type(c); session.action(Action::ConfirmChallenge); }
    if(frame==1005) { click(session,game,Action::ConfirmChallenge); require(session.ui.screen==Screen::Playing && game.levels.config.number==1,"Confirmed existing name failed to restart Challenge."); }
    return 1.f/60;
}
// Name selected smoke-test frames so screenshots correspond to known UI/game states.
std::string modeSmokeCapture(int frame) {
    if (frame==0) return "developer-menu";
    if (frame>=1 && frame<=700 && (frame-1)%100==92) return "developer-"+std::to_string((frame-1)/100+1);
    if(frame==794) return "npc-death-impact";
    if(frame==795) return "npc-fallen";
    if(frame==995) return "challenge-name";
    if(frame==998) return "duplicate-confirmation";
    if(frame==1001) return "rewritten-name";
    if (frame==793) return "free-playing";
    if (frame==792) return "human-model";
    if (frame==821) return "free-respawn-warning";
    if (frame==974) return "free-ending";
    if (frame==977) return "free-result";
    if (frame==981 || frame==988) return frame==981?"overhead":"overhead-marker";
    if (frame==984) return "observation";
    if (frame==993) return "free-leaderboard";
    return {};
}
// Check final scripted-session outcomes and signal when the smoke run may stop.
bool modeSmokeFinished(int frame,SessionController&,Game& game,Leaderboard& board) {
    if (frame!=1010) return false;
    board.load();
    require(board.rank(game.playerName,GameMode::Challenge)>0 && board.rank(game.playerName,GameMode::Free)>0,"Leaderboard reload lost results.");
    require(board.sorted(GameMode::Developer).empty() && board.sorted(GameMode::BirdsEye).empty(),"Inspection mode wrote a competitive record.");
    std::cout<<"PASS: seven clickable Developer levels, full 180-second Free simulation and 30-second respawn, Free leaderboard, overhead picking/observation, persistent reload, permanent NPC deaths and Challenge name confirmation/rewrite.\n";
    return true;
}
}
