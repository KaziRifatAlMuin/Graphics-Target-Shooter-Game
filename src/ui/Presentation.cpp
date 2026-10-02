#include "ui/Presentation.h"
#include <algorithm>
namespace shooter {
float resultAnimationDuration(Screen s) { return s==Screen::Victory?2.4f:s==Screen::FreeComplete?1.6f:.85f; }
bool drawPresentation(ui::Painter& p,const Game& game,Screen screen,const UiState&) {
    using namespace ui;
    const bool intro=screen==Screen::Playing && game.usesLevel() && game.levels.stage==LevelStage::Intro;
    const bool ending=resultScreen(screen) && game.levels.transition<resultAnimationDuration(screen);
    if (!intro && !ending) return false;
    const float t=game.levels.transition;
    const float duration=intro?1.5f:resultAnimationDuration(screen);
    const float enter=std::clamp(t/.2f,0.f,1.f),exit=intro?std::clamp((duration-t)/.2f,0.f,1.f):1.f;
    p.opacity=.68f*enter*exit; p.rect(0,0,1280,800,{.015f,.025f,.045f});
    p.opacity=enter*exit;
    const float slide=(1-enter)*(1-enter)*50;
    p.rect(224,226+slide,832,342,{.025f,.055f,.078f});
    p.rect(224,226+slide,832*std::min(1.f,t/duration),5,teal);
    auto centered=[&](float y,const std::string& text,float size,Vec3 color) { p.text(640-text.size()*3*size,y+slide,text,size,color); };
    if (intro) {
        centered(265,std::string(modeName(game.mode))+" / LEVEL "+std::to_string(game.levels.config.number),3,teal);
        centered(311,game.levels.config.title,2.25f,ink);
        const char* count=t<.4f?"3":t<.8f?"2":t<1.2f?"1":"GO!";
        const float beat=std::fmod(t,.4f)/.4f;
        centered(364,count,9+3*(1-beat),t>=1.2f?teal:amber);
        centered(491,game.mode==GameMode::Free?"THREE MINUTES / TARGETS RETURN AFTER 30 SECONDS":
            game.levels.config.playerMovement?"FIND YOUR ANGLE / WATCH FOR PENALTY NPCS":"POSITION LOCKED / CLEAR EVERY TARGET",1.9f,muted);
    } else {
        const bool victory=screen==Screen::Victory,free=screen==Screen::FreeComplete;
        centered(269,victory?"ALL SEVEN LEVELS CLEARED":free?"SESSION COMPLETE":"LEVEL "+std::to_string(game.levels.config.number)+" CLEARED",2.8f,teal);
        centered(350,victory?"VICTORY!":free?"TIME!":"WELL DONE",6.5f+.4f*std::sin(t*8),amber);
        centered(437,"SCORE "+std::to_string(game.score)+" / TARGETS "+std::to_string(game.destroyed),2.6f,ink);
        centered(502,"YOUR RESULTS AND RANK ARE NEXT",1.9f,muted);
    }
    p.opacity=1;
    return true;
}
}
