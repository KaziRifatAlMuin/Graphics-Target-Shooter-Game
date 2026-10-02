#include "ui/ModeUI.h"
#include "ui/LeaderboardUI.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
namespace shooter {
std::string clockText(double seconds) {
    const int s=int(std::ceil(std::max(0.0,seconds-1e-5)));
    std::ostringstream out; out<<std::setfill('0')<<std::setw(2)<<s/60<<':'<<std::setw(2)<<s%60; return out.str();
}
bool drawModePage(ui::Painter& p,const Game& game,Screen screen,const UiState& state) {
    using namespace ui;
    if (screen==Screen::Menu) {
        p.rect(56,66,1168,654,{.035f,.065f,.09f}); p.rect(56,66,5,654,teal);
        p.text(96,98,"3D TARGET SHOOTER",4.6f,ink);
        p.text(96,151,"THE FORTIFIED RANGE / CHOOSE YOUR MODE",1.9f,teal);
        p.rect(96,177,480*(.5f+.5f*std::sin(state.animationTime*.7f)),2,teal);
        p.text(650,206,"TARGET +100 / ONE SHOT +150",2.2f,amber);
        p.text(650,240,"BIRD -100 / HUMAN -200",2,muted);
        p.text(96,258,state.editingName?"TYPE NAME - BACKSPACE TO EDIT - ENTER TO ACCEPT":"CLICK YOUR NAME TO EDIT BEFORE STARTING",1.7f,state.editingName?amber:muted);
        return true;
    }
    if (screen==Screen::Developer) {
        p.rect(36,60,1208,670,{.035f,.065f,.09f});
        p.text(66,90,"DEVELOPER / SELECT A LEVEL",3.5f,teal);
        p.text(66,142,"DIRECT LAUNCH. SHARED LEVELS. NO COMPETITIVE RECORDS.",2,muted);
        return true;
    }
    if (screen==Screen::Leaderboard || resultScreen(screen)) {
        p.rect(36,58,1208,690,{.035f,.065f,.09f});
        const bool developer=game.mode==GameMode::Developer && resultScreen(screen);
        const std::string title=screen==Screen::Leaderboard?std::string(modeName(state.boardMode))+" LEADERBOARD":
            screen==Screen::FreeComplete?"TIME! / FREE MODE COMPLETE":screen==Screen::Victory?"ALL SEVEN LEVELS COMPLETE!":"LEVEL "+std::to_string(game.levels.config.number)+" COMPLETE";
        p.text(66,84,title,3.1f,teal);
        if (resultScreen(screen)) {
            p.rect(66,122,1140*std::min(1.f,game.levels.transition),3,teal);
            p.text(66,146,"PLAYER "+game.playerName+"   SCORE "+std::to_string(game.score)+"   TIME "+clockText(game.elapsed),2.4f,amber);
            p.text(66,184,"LEVELS "+std::to_string(game.levelsCleared)+"   TARGETS "+std::to_string(game.destroyed)+"   BULLSEYES "+std::to_string(game.bullseyes)+
                "   BIRDS "+std::to_string(game.birdHits)+"   HUMANS "+std::to_string(game.humanHits),1.9f,ink);
            p.text(66,219,"SHOTS "+std::to_string(game.shots)+"   TARGET HITS "+std::to_string(game.hits),1.9f,muted);
            p.text(66,256,developer?"DEVELOPER RESULT / NOT SUBMITTED":state.saveFailed?"RESULT NOT SAVED - RETRY BELOW":
                state.personalBest?"NEW PERSONAL BEST!":"PERSONAL BEST RETAINED",2.2f,state.saveFailed?Vec3{1,.3f,.25f}:teal);
        } else p.text(66,147,"SCORE DESCENDING / LOWER TIME BREAKS TIES",1.9f,muted);
        if (!developer) {
            p.text(760,256,state.rank?"YOUR BEST RANK: "+std::to_string(state.rank):"YOUR BEST RANK: --",2,amber);
            drawLeaderboard(p,state,state.hasResult?game.playerName:state.name);
        } else {
            p.text(90,380,"R RELOADS THIS LEVEL. CHOOSE ANOTHER LEVEL BELOW.",2.3f,ink);
            p.text(90,430,"CHALLENGE AND FREE RECORDS REMAIN SEPARATE.",2,muted);
        }
        return true;
    }
    if (screen==Screen::Playing && game.mode==GameMode::BirdsEye) {
        p.rect(20,16,930,80,{.035f,.065f,.09f});
        p.text(40,32,game.birdEye.observing?"OBSERVATION CAMERA / LEVEL 7":"BIRD'S-EYE VIEW / LEVEL 7",2.7f,teal);
        p.text(40,72,game.birdEye.observing?"MOUSE LOOK / WASD WALK / B OR F2 FOR OVERHEAD":"CLICK OPEN GROUND / WASD PAN / WHEEL ZOOM",1.8f,ink);
        p.rect(28,655,1200,40,{.035f,.065f,.09f});
        p.text(44,668,state.message.empty()?"INSPECTION ONLY / NO SHOOTING / SAME LEVEL 7 TARGETS, CARGO AND NPCS":state.message.substr(0,96),1.8f,muted);
        return true;
    }
    return false;
}
void drawModeHud(ui::Painter& p,const Game& game,const UiState& state) {
    using namespace ui;
    p.rect(20,262,680,62,{.035f,.065f,.09f});
    p.text(42,274,std::string(modeName(game.mode))+" / "+game.playerName,1.8f,teal);
    if (game.mode==GameMode::Challenge) p.text(42,302,"LEVEL TIME "+clockText(game.levels.levelTime)+" / CLEARED "+std::to_string(game.levelsCleared)+" OF 7",1.6f,muted);
    if (game.mode==GameMode::Free) {
        p.rect(976,104,270,96,{.035f,.065f,.09f});
        p.text(995,116,"FREE / TIME LEFT",1.8f,muted);
        p.text(1000,151,clockText(game.freeRemaining),4,game.freeRemaining<15?Vec3{1,.3f,.25f}:amber);
        float next=freeRespawnSeconds; int waiting=0;
        for (const auto& t:game.targets) if (t.respawn>0) { ++waiting; next=std::min(next,t.respawn); }
        if (waiting) p.text(42,302,"RESPAWNING "+std::to_string(waiting)+" / NEXT IN "+std::to_string(int(std::ceil(next)))+" S",1.6f,amber);
    }
    if (game.mode==GameMode::Developer) {
        p.rect(956,112,290,478,{.035f,.065f,.09f});
        p.text(970,128,"LEVEL "+std::to_string(game.levels.config.number)+" / FPS "+std::to_string(int(state.fps)),1.7f,teal);
        p.text(970,156,"BIRDS "+std::to_string(game.birds.size())+" HUMANS "+std::to_string(game.humans.size()),1.6f,ink);
        p.text(970,184,"PLAYER X "+std::to_string(int(game.player.position.x))+" Z "+std::to_string(int(game.player.position.z)),1.6f,ink);
        for (std::size_t i=0;i<game.targets.size();++i) {
            const auto& t=game.targets[i]; const auto a=t.motion.amplitude;
            std::string axes; if(a.x)axes+="X"; if(a.y)axes+="Y"; if(a.z)axes+="Z"; if(axes.empty())axes="STATIC";
            p.text(970,220+float(i)*28,"T"+std::to_string(i+1)+" "+(t.eliminated?"DONE":std::to_string(int(t.health))+"HP")+" "+axes+(t.motion.spinSpeed?" SPIN":""),1.45f,muted);
        }
    }
}
}
