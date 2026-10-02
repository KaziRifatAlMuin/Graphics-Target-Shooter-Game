#include "ui/Interface.h"
#include "ui/ModeUI.h"
#include "ui/Presentation.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace shooter {
using namespace ui;
namespace {
bool inside(const Button& b,float x,float y) { return x>=b.x && x<=b.x+b.w && y>=b.y && y<=b.y+b.h; }
}
std::vector<Button> screenButtons(Screen screen,GameMode mode) {
    if (screen==Screen::Menu) return {
        {96,194,500,52,"",Action::EditName},
        {96,294,500,90,"FREE MODE / 3 MINUTES",Action::Free},{650,294,560,90,"DEVELOPER MODE",Action::Developer},
        {96,400,500,90,"CHALLENGE / SEVEN LEVELS",Action::Start},{650,400,560,90,"BIRD'S-EYE VIEW",Action::BirdsEye},
        {96,552,245,52,"CONTROLS",Action::Controls},{355,552,241,52,"PRACTICE",Action::Practice},
        {650,552,560,52,"LEADERBOARD",Action::Leaderboard},{96,628,500,52,"EXIT",Action::Exit},
        {650,628,264,52,"",Action::DayNight},{932,628,278,52,"",Action::Sound}};
    if (screen==Screen::ChallengeName) return {
        {96,240,640,56,"",Action::EditName},
        {96,440,400,60,"CHECK NAME / START",Action::ConfirmChallenge},
        {520,440,240,60,"REWRITE NAME",Action::RewriteName},
        {784,440,240,60,"BACK TO MENU",Action::Back},
        {96,628,264,52,"",Action::DayNight},{380,628,278,52,"",Action::Sound}};
    if (screen==Screen::Developer) {
        std::vector<Button> buttons;
        for (int i=0;i<7;++i) buttons.push_back({float(56+396*(i%3)),float(205+130*(i/3)),376,110,
            "LEVEL "+std::to_string(i+1)+" / "+std::to_string(levelConfiguration(i+1).targets.size())+" TARGETS",Action(int(Action::Level1)+i)});
        buttons.push_back({56,654,376,48,"MAIN MENU",Action::Menu}); return buttons;
    }
    if (screen==Screen::Leaderboard) return {
        {66,202,270,52,"CHALLENGE",Action::BoardChallenge},{354,202,270,52,"FREE",Action::BoardFree},
        {96,698,250,40,"BACK",Action::Back},{410,698,250,40,"RETRY LOAD/SAVE",Action::RetrySave},
        {900,662,140,32,"UP",Action::ScrollUp},{1050,662,140,32,"DOWN",Action::ScrollDown}};
    if (resultScreen(screen)) {
        if (mode==GameMode::Developer) return {{66,698,300,40,"RELOAD LEVEL",Action::Replay},
            {400,698,340,40,"SELECT LEVEL",Action::Developer},{780,698,300,40,"MAIN MENU",Action::Menu}};
        return {{66,698,300,40,screen==Screen::LevelComplete?"NEXT LEVEL":"PLAY AGAIN",screen==Screen::LevelComplete?Action::NextLevel:Action::Replay},
            {384,698,230,40,"MAIN MENU",Action::Menu},{632,698,270,40,"RESTART RUN",Action::Replay},
            {920,698,288,40,"RETRY SAVE",Action::RetrySave},{900,662,140,32,"UP",Action::ScrollUp},{1050,662,140,32,"DOWN",Action::ScrollDown}};
    }
    if (screen==Screen::Playing && mode==GameMode::BirdsEye) return {
        {1000,22,122,40,"MENU",Action::Menu},{1134,22,112,40,"EXIT",Action::Exit},
        {28,716,360,52,"B / F2  OVERHEAD",Action::Overview},
        {802,716,208,52,"",Action::DayNight},{1024,716,224,52,"",Action::Sound}};
    if (screen==Screen::Playing) return {{1000,22,122,40,"MENU",Action::Menu},{1134,22,112,40,"EXIT",Action::Exit},
        {28,716,220,52,"1  PISTOL",Action::Pistol},{262,716,220,52,"2  SHOTGUN",Action::Shotgun},
        {496,716,274,52,"3  ASSAULT RIFLE",Action::Rifle},
        {802,716,208,52,"",Action::DayNight},{1024,716,224,52,"",Action::Sound}};
    if (screen==Screen::Controls) return {{96,660,430,56,"BACK",Action::Back},
        {650,466,264,52,"",Action::DayNight},{932,466,278,52,"",Action::Sound}};
    const bool paused=screen==Screen::Paused;
    return {{96,400,430,56,paused?"RESUME SESSION":"START CHALLENGE",paused?Action::Resume:Action::Start},
            {96,472,430,56,"VIEW CONTROLS",Action::Controls},
            {96,544,430,56,paused?"MAIN MENU":"EXIT",paused?Action::Menu:Action::Exit},
            {96,616,430,56,paused?(mode==GameMode::Developer?"SELECT LEVEL":"EXIT"):"PRACTICE SANDBOX",paused?(mode==GameMode::Developer?Action::Developer:Action::Exit):Action::Practice},
            {650,466,264,52,"",Action::DayNight},{932,466,278,52,"",Action::Sound}};
}
Action clickedAction(Screen screen,float x,float y,GameMode mode) {
    for (const auto& b:screenButtons(screen,mode)) if (inside(b,x,y)) return b.action;
    return Action::None;
}
std::vector<UiVertex> buildInterface(const Game& game,Screen screen,float mx,float my,bool pointerFree,const UiState* supplied) {
    Painter p;
    const UiState defaults;
    const auto& state=supplied?*supplied:defaults;
    if (resultScreen(screen) && drawPresentation(p,game,screen,state)) return p.vertices;
    if (drawModePage(p,game,screen,state)) {
        // Mode pages share the same button hit testing and painter as the gameplay HUD.
    } else if (screen==Screen::ChallengeName) {
        p.rect(56,66,1168,668,{.035f,.065f,.09f}); p.rect(56,66,5,668,teal);
        p.text(96,104,"CHALLENGE MODE / 7-LEVEL GAUNTLET",2,teal);
        p.text(96,154,"PLAYER CALLSIGN REGISTRATION",4,ink);
        p.text(96,204,"ENTER YOUR NAME (1-24 CHARACTERS). BACKSPACE TO EDIT. ENTER TO CONTINUE.",1.8f,muted);
        if (state.nameExistsWarning) {
            p.rect(96,316,1088,116,{.16f,.06f,.06f}); p.rect(96,316,5,116,amber);
            p.text(118,337,"NAME ALREADY EXISTS IN CHALLENGE / RANK "+std::to_string(state.existingRank),2.2f,amber);
            p.text(118,364,state.name,2,ink);
            p.text(118,385,"EXISTING BEST RECORD: SCORE "+std::to_string(state.existingScore)+"   /   LEVELS CLEARED: "+std::to_string(state.existingLevels),1.9f,ink);
            p.text(118,415,"ONLY A BETTER ELIGIBLE RESULT REPLACES THE SAVED BEST.",1.7f,muted);
        } else {
            p.rect(96,316,1088,88,{.04f,.12f,.12f}); p.rect(96,316,5,88,teal);
            p.text(118,344,state.name.empty()?"ENTER YOUR NAME TO CONTINUE":"NEW CHALLENGE NAME: "+state.name,2.2f,teal);
            p.text(118,380,"PRESS ENTER OR CHECK NAME / START TO CONTINUE.",1.8f,ink);
        }
        p.rect(96,524,1088,84,{.025f,.045f,.07f});
        p.text(118,548,"COMPETITIVE RULES: SEVEN SEQUENTIAL LEVELS WITH PROGRESSIVE TARGET SPEEDS & DISTANCES.",1.8f,ink);
        p.text(118,576,"ACCURACY PENALTIES: BIRDS -100 PTS  |  CIVILIANS -200 PTS. STRIKES ARE FATAL AND DO NOT RESPAWN!",1.8f,amber);
    } else if (screen!=Screen::Playing) {
        p.rect(56,66,530,668,{.035f,.065f,.09f}); p.rect(56,66,5,668,teal);
        p.text(96,104,"ARENA / CONTROLS AND SETTINGS",2,teal);
        if (screen==Screen::Controls) {
            p.text(96,158,"CONTROLS",4,ink);
            const char* lines[]={"WASD       WALK / FREE CAMERA","MOUSE      LOOK AND AIM","LEFT CLICK / SPACE    FIRE",
                "1 / 2 / 3  SELECT WEAPON","SHIFT      MOVE FASTER","F1         PLAYER VIEW","F2 / F3    ARENA / SIDE VIEW",
                "F4         FREE CAMERA","Q / E      FREE CAMERA DOWN / UP","R          RESTART CURRENT LEVEL","TAB        RELEASE / CAPTURE MOUSE",
                "ESC        PAUSE / RESUME","F5         SAVE CALC SNAPSHOT","N          DAY / NIGHT","M          SOUND ON / OFF"};
            float y=222;
            for (const char* line:lines) { p.text(96,y,line,1.8f,ink); y+=27; }
        } else {
            p.text(96,162,screen==Screen::Paused?"SESSION":"TARGET",5,ink);
            p.text(96,212,screen==Screen::Paused?"PAUSED":"SHOOTER",5,ink);
            p.text(96,280,"MOVING TARGETS. THREE WEAPONS.",2,amber);
            p.text(96,320,"WASD TO WALK. MOUSE TO AIM.",2,muted);
            p.text(96,348,"LEFT CLICK TO FIRE. ESC FOR MENU.",1.9f,muted);
        }
        p.rect(648,584,562,150,{.035f,.065f,.09f});
        p.text(676,610,"THE TRAINING GROUND",2.8f,ink);
        p.text(676,652,"60 X 100 M  /  FULLY ENCLOSED ARENA",1.9f,muted);
        p.text(676,687,"PISTOL 25 M  /  SHOTGUN 18 M  /  RIFLE 70 M",1.8f,teal);
        p.rect(648,198,562,240,{.035f,.065f,.09f});
        p.text(676,222,"SIX RINGS. FRONT HITS ONLY.",2.5f,ink);
        p.text(676,266,"CENTER: 1 SHOT TO BREAK",2.3f,amber);
        p.text(676,300,"THEN 2 / 3 / 4 / 5 / 6 SHOTS OUTWARD",1.9f,muted);
        p.text(676,338,"TARGET +100 / FIRST SHOT KILL +150",2,teal);
        p.text(676,378,"BIRD -100 / HUMAN -200",2,amber);
        p.text(676,406,"LEVELS 1-3: FIXED PLAYER. 4-7: WALK.",1.8f,muted);
        if (screen==Screen::Controls) {
            p.rect(648,198,562,240,{.035f,.065f,.09f});
            p.text(676,222,"BIRD'S-EYE / LEADERBOARD",2.5f,ink);
            p.text(676,266,"CLICK OPEN GROUND TO OBSERVE",2,teal);
            p.text(676,300,"WASD PAN / WALK. MOUSE LOOK.",1.9f,muted);
            p.text(676,334,"WHEEL ZOOM. B / F2 RETURN OVERHEAD.",1.8f,muted);
            p.text(676,372,"TABLE: WHEEL / PAGE UP / PAGE DOWN",1.8f,amber);
            p.text(676,406,"DEVELOPER: PAUSE > SELECT LEVEL",1.8f,muted);
        }
    } else {
        p.rect(20,16,940,88,{.035f,.065f,.09f});
        p.text(40,32,weaponSpec(game.weapon).name,2.8f,ink);
        const char* modes[]={"","PLAYER","ARENA","SIDE","FREE"};
        p.text(430,32,std::string("CAMERA: ")+modes[game.cameraMode],2,teal);
        float distance=0; const int target=game.aimedTarget(distance);
        std::ostringstream range; range<<std::fixed<<std::setprecision(1);
        range<<(game.cameraMode==1?"TARGET: ":"PLAYER AIM: "); if (target<0) range<<"--"; else range<<distance<<" M";
        range<<"  /  RANGE: "<<int(weaponSpec(game.weapon).range)<<" M";
        p.text(40,72,range.str(),1.9f,target>=0&&distance>weaponSpec(game.weapon).range?amber:muted);
        p.text(650,72,"HITS "+std::to_string(game.hits)+"  CLEARED "+std::to_string(game.destroyed),1.8f,ink);
        p.rect(20,112,630,44,{.035f,.065f,.09f});
        p.text(40,127,"SCORE "+std::to_string(game.score)+"  /  BULLSEYES "+std::to_string(game.bullseyes),2,amber);
        if (game.usesLevel()) {
            p.rect(20,164,680,90,{.035f,.065f,.09f});
            p.text(40,178,"LEVEL "+std::to_string(game.levels.config.number)+" / 7   REMAINING "+std::to_string(game.remainingTargets()),2,teal);
            p.text(40,205,"TIME "+std::to_string(int(game.elapsed))+" S   BIRDS "+std::to_string(game.birdHits)+"   HUMANS "+std::to_string(game.humanHits),1.9f,ink);
            p.text(40,231,game.levels.config.playerMovement?"MOVEMENT ENABLED - FIND A CLEAR FIRING POSITION":"POSITION LOCKED - AIM AND SWITCH WEAPONS",1.6f,muted);

        }
        drawModeHud(p,game,state);
        if (target>=0 && game.cameraMode==1) {
            p.rect(28,608,400,40,{.035f,.065f,.09f});
            p.text(42,620,"TARGET HEALTH "+std::to_string(int(game.targets[target].health))+" / 60",2,ink);
        }
        if (game.feedbackTime>0) p.text(500,468,game.lastRing==0?"CENTER HIT!":"RING "+std::to_string(game.lastRing+1)+" HIT",2.5f,amber);
        p.rect(28,665,740,34,{.035f,.065f,.09f});
        p.text(42,676,game.cameraMode==1?(pointerFree?"CLICK MENU OR WEAPON. TAB TO RESUME AIM.":"WASD MOVE  /  CLICK FIRE  /  ESC MENU  /  TAB POINTER"):
            "F1 TO AIM AND FIRE  /  F2 ARENA  /  F3 SIDE  /  F4 FREE",1.8f,muted);
        if (game.cameraMode==1 && !pointerFree) {
            const Vec3 color=target>=0?teal:ink;
            const float cx=640,cy=400;
            if (game.weapon==WeaponType::Shotgun) {
                const float radius=38+game.recoil*12;
                for (int i=0; i<48; ++i) {
                    const float a=i*2*pi/48,b=(i+1)*2*pi/48;
                    p.line(cx+std::cos(a)*radius,cy+std::sin(a)*radius,cx+std::cos(b)*radius,cy+std::sin(b)*radius,2,color);
                }
                for (int i=0; i<8; ++i) { const float a=i*pi/4;
                    p.line(cx+std::cos(a)*(radius+5),cy+std::sin(a)*(radius+5),cx+std::cos(a)*(radius+10),cy+std::sin(a)*(radius+10),2,color); }
            } else {
                const float gap=game.weapon==WeaponType::Pistol?8:5;
                p.rect(cx-1,cy-1,2,2,color);
                p.line(cx-gap-8,cy,cx-gap,cy,2,color); p.line(cx+gap,cy,cx+gap+8,cy,2,color);
                p.line(cx,cy-gap-8,cx,cy-gap,2,color); p.line(cx,cy+gap,cx,cy+gap+8,2,color);
            }
            for (const auto& t:game.targets) if (t.hitTime>0) {
                for (int x:{-1,1}) for (int y:{-1,1}) p.line(cx+x*18,cy+y*18,cx+x*25,cy+y*25,3,amber);
                break;
            }
        }
    }
    const Vec3 red{1,.23f,.20f};
    if (screen==Screen::Playing && game.dangerTime>0) {
        const float edge=(4+7*game.dangerTime)*(game.dangerHuman?1.7f:1.f);
        p.opacity=.65f+.35f*std::abs(std::sin(game.dangerTime*15));
        p.rect(0,0,1280,edge,red); p.rect(0,800-edge,1280,edge,red);
        p.rect(0,0,edge,800,red); p.rect(1280-edge,0,edge,800,red);
        p.opacity=1;
        p.text(430,570,game.dangerHuman?"CIVILIAN HIT / -200":"BIRD HIT / -100",2.4f,red);
    }
    float popupY=520;
    if (screen==Screen::Playing) for (const auto& popup:game.scoreFeedback) {
        p.text(790,popupY-(1.5f-popup.life)*20,popup.text,2.5f,popup.penalty?red:teal); popupY+=30;
    }
    if (resultScreen(screen) && !game.scoreFeedback.empty()) {
        const auto& popup=game.scoreFeedback.back();
        p.text(810,226-(1.5f-popup.life)*8,popup.text,2,popup.penalty?red:teal);
    }
    if (screen!=Screen::Playing && !state.message.empty()) p.text(66,675,state.message.substr(0,78),1.6f,amber);
    if (screen==Screen::Playing && drawPresentation(p,game,screen,state)) return p.vertices;
    for (const auto& b:screenButtons(screen,game.mode)) {
        if (b.action==Action::None) continue;
        const bool hover=pointerFree&&inside(b,mx,my);
        const bool selected=(b.action==Action::Pistol&&game.weapon==WeaponType::Pistol)
            ||(b.action==Action::Shotgun&&game.weapon==WeaponType::Shotgun)||(b.action==Action::Rifle&&game.weapon==WeaponType::Rifle);
        const bool primary=b.action==Action::Start||b.action==Action::Resume||b.action==Action::ConfirmChallenge;
        p.rect(b.x,b.y,b.w,b.h,primary||selected?teal:(hover?Vec3{.20f,.32f,.36f}:Vec3{.10f,.16f,.20f}));
        if (hover) {
            p.rect(b.x,b.y,4+2*std::sin(state.animationTime*7),b.h,amber);
            p.rect(b.x,b.y+b.h-2,b.w,2,teal);
        }
        const std::string label=b.action==Action::ConfirmChallenge&&state.duplicateConfirmation?"USE EXISTING NAME":b.action==Action::NextLevel&&!game.levels.readyToAdvance()?"LEVEL CLEARED...":b.action==Action::DayNight?(game.night?"N  NIGHT":"N  DAY"):
            b.action==Action::Sound?(!game.soundAvailable?"NO AUDIO DEVICE":game.soundEnabled?"M  SOUND ON":"M  SOUND OFF"):
            b.action==Action::EditName?"NAME: "+state.name+(state.editingName?"_":""):b.text;
        p.text(b.x+18,b.y+(b.h-14)/2,label,2,primary||selected?Vec3{.03f,.08f,.10f}:ink);
    }
    if (supplied && state.screenTime<.3f) {
        p.opacity=.72f*(1-state.screenTime/.3f);
        p.rect(0,0,1280,800,{.015f,.025f,.04f});
    }
    return p.vertices;
}
}
