#include "Interface.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace shooter {
namespace {
const Vec3 ink{.88f,.92f,.94f}, muted{.53f,.63f,.69f}, teal{.26f,.82f,.71f}, amber{.98f,.70f,.31f};
// Small built-in 5x7 font. No OS fonts, texture downloads, or extra dependencies.
std::array<unsigned char,7> glyph(char character) {
    switch (std::toupper(static_cast<unsigned char>(character))) {
    case 'A': return {14,17,17,31,17,17,17}; case 'B': return {30,17,17,30,17,17,30};
    case 'C': return {14,17,16,16,16,17,14}; case 'D': return {30,17,17,17,17,17,30};
    case 'E': return {31,16,16,30,16,16,31}; case 'F': return {31,16,16,30,16,16,16};
    case 'G': return {14,17,16,23,17,17,15}; case 'H': return {17,17,17,31,17,17,17};
    case 'I': return {14,4,4,4,4,4,14}; case 'J': return {7,2,2,2,2,18,12};
    case 'K': return {17,18,20,24,20,18,17}; case 'L': return {16,16,16,16,16,16,31};
    case 'M': return {17,27,21,21,17,17,17}; case 'N': return {17,25,21,19,17,17,17};
    case 'O': return {14,17,17,17,17,17,14}; case 'P': return {30,17,17,30,16,16,16};
    case 'Q': return {14,17,17,17,21,18,13}; case 'R': return {30,17,17,30,20,18,17};
    case 'S': return {15,16,16,14,1,1,30}; case 'T': return {31,4,4,4,4,4,4};
    case 'U': return {17,17,17,17,17,17,14}; case 'V': return {17,17,17,17,17,10,4};
    case 'W': return {17,17,17,21,21,21,10}; case 'X': return {17,17,10,4,10,17,17};
    case 'Y': return {17,17,10,4,4,4,4}; case 'Z': return {31,1,2,4,8,16,31};
    case '0': return {14,17,19,21,25,17,14}; case '1': return {4,12,4,4,4,4,14};
    case '2': return {14,17,1,2,4,8,31}; case '3': return {30,1,1,14,1,1,30};
    case '4': return {2,6,10,18,31,2,2}; case '5': return {31,16,16,30,1,1,30};
    case '6': return {14,16,16,30,17,17,14}; case '7': return {31,1,2,4,8,8,8};
    case '8': return {14,17,17,14,17,17,14}; case '9': return {14,17,17,15,1,1,14};
    case ':': return {0,4,4,0,4,4,0}; case '.': return {0,0,0,0,0,6,6};
    case '-': return {0,0,0,31,0,0,0}; case '/': return {1,1,2,4,8,16,16};
    case '+': return {0,4,4,31,4,4,0}; case '>': return {16,8,4,2,4,8,16};
    case '!': return {4,4,4,4,4,0,4};
    case '(': return {2,4,8,8,8,4,2}; case ')': return {8,4,2,2,2,4,8};
    default: return {};
    }
}
struct Painter {
    std::vector<UiVertex> vertices;
    void rect(float x,float y,float w,float h,Vec3 c) {
        for (auto p:std::array<std::array<float,2>,6>{{{x,y},{x+w,y},{x+w,y+h},{x,y},{x+w,y+h},{x,y+h}}})
            vertices.push_back({p[0],p[1],c.x,c.y,c.z});
    }
    void text(float x,float y,const std::string& value,float scale,Vec3 c) {
        for (char ch:value) {
            const auto rows=glyph(ch);
            for (int row=0; row<7; ++row) for (int col=0; col<5; ++col)
                if (rows[row]&(1<<(4-col))) rect(x+col*scale,y+row*scale,scale,scale,c);
            x+=6*scale;
        }
    }
    void line(float x1,float y1,float x2,float y2,float thickness,Vec3 c) {
        const float dx=x2-x1,dy=y2-y1, length=std::sqrt(dx*dx+dy*dy);
        if (length==0) return;
        const float x=-dy/length*thickness/2, y=dx/length*thickness/2;
        const float points[][2]={{x1+x,y1+y},{x2+x,y2+y},{x2-x,y2-y},{x1+x,y1+y},{x2-x,y2-y},{x1-x,y1-y}};
        for (auto& p:points) vertices.push_back({p[0],p[1],c.x,c.y,c.z});
    }
};
bool inside(const Button& b,float x,float y) { return x>=b.x && x<=b.x+b.w && y>=b.y && y<=b.y+b.h; }
}
std::vector<Button> screenButtons(Screen screen) {
    if (screen==Screen::LevelComplete || screen==Screen::Victory) return {
        {96,544,430,56,screen==Screen::Victory?"PLAY CHALLENGE AGAIN":"NEXT LEVEL",screen==Screen::Victory?Action::Start:Action::NextLevel},
        {96,616,430,56,"MAIN MENU",Action::Menu},{1000,110,210,52,"EXIT",Action::Exit}};
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
            {96,616,430,56,paused?"EXIT":"PRACTICE SANDBOX",paused?Action::Exit:Action::Practice},
            {650,466,264,52,"",Action::DayNight},{932,466,278,52,"",Action::Sound}};
}
Action clickedAction(Screen screen,float x,float y) {
    for (const auto& b:screenButtons(screen)) if (inside(b,x,y)) return b.action;
    return Action::None;
}
std::vector<UiVertex> buildInterface(const Game& game,Screen screen,float mx,float my,bool pointerFree) {
    Painter p;
    if (screen!=Screen::Playing) {
        p.rect(56,66,530,668,{.035f,.065f,.09f}); p.rect(56,66,5,668,teal);
        p.text(96,104,"PHASE 2 / SEVEN LEVEL CHALLENGE",2,teal);
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
        if (screen==Screen::LevelComplete || screen==Screen::Victory) {
            p.rect(80,145,480,380,{.035f,.065f,.09f});
            p.text(96,170,screen==Screen::Victory?"CHALLENGE":"LEVEL",4.5f,ink);
            p.text(96,216,"COMPLETE!",4.5f,teal);
            p.text(96,286,"SCORE  "+std::to_string(game.score),3,amber);
            p.text(96,330,"TOTAL TIME  "+std::to_string(int(game.elapsed))+" S",2.5f,ink);
            p.text(96,370,"TARGETS "+std::to_string(game.destroyed)+"  BULLSEYES "+std::to_string(game.bullseyes),2,ink);
            p.text(96,408,"BIRDS "+std::to_string(game.birdHits)+"  HUMANS "+std::to_string(game.humanHits),2,ink);
            p.text(96,446,"LEVELS CLEARED  "+std::to_string(game.levelsCleared),2,teal);
            p.text(96,486,screen==Screen::Victory?"ALL SEVEN LEVELS CLEARED.":"ENTER TO CONTINUE WHEN READY.",1.9f,muted);
            p.rect(96,518,430*std::min(1.0f,game.levels.transition),4,teal);
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
        if (game.challenge) {
            p.rect(20,164,680,90,{.035f,.065f,.09f});
            p.text(40,178,"LEVEL "+std::to_string(game.levels.config.number)+" / 7   REMAINING "+std::to_string(game.remainingTargets()),2,teal);
            p.text(40,205,"TIME "+std::to_string(int(game.elapsed))+" S   BIRDS "+std::to_string(game.birdHits)+"   HUMANS "+std::to_string(game.humanHits),1.9f,ink);
            p.text(40,231,game.levels.config.playerMovement?"MOVEMENT ENABLED - FIND A CLEAR FIRING POSITION":"POSITION LOCKED - AIM AND SWITCH WEAPONS",1.6f,muted);
            if (game.levels.stage==LevelStage::Intro) {
                p.rect(350,300,620,120,{.035f,.065f,.09f});
                p.text(385,320,"LEVEL "+std::to_string(game.levels.config.number)+" - GET READY",3,teal);
                p.text(385,365,game.levels.config.title,2,ink);
                p.text(385,396,"DESTROY ALL TARGETS TO CONTINUE",1.8f,muted);
                p.rect(350,416,620*std::min(1.0f,game.levels.transition/1.5f),4,teal);
            }
        }
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
    if (game.dangerTime>0) {
        const float edge=4+6*game.dangerTime;
        p.rect(0,0,1280,edge,red); p.rect(0,800-edge,1280,edge,red);
        p.rect(0,0,edge,800,red); p.rect(1280-edge,0,edge,800,red);
    }
    float popupY=520; int popupIndex=0;
    const bool completed=screen==Screen::LevelComplete || screen==Screen::Victory;
    if (completed && !game.scoreFeedback.empty()) p.rect(648,448,562,120,{.035f,.065f,.09f});
    for (const auto& popup:game.scoreFeedback) {
        const float x=completed?676+260*(popupIndex/3):790;
        const float y=completed?478+30*(popupIndex%3):popupY;
        p.text(x,y-(1.5f-popup.life)*20,popup.text,completed?2.f:2.5f,popup.penalty?red:teal); popupY+=30; ++popupIndex;
    }
    for (const auto& b:screenButtons(screen)) {
        if (b.action==Action::None) continue;
        const bool hover=pointerFree&&inside(b,mx,my);
        const bool selected=(b.action==Action::Pistol&&game.weapon==WeaponType::Pistol)
            ||(b.action==Action::Shotgun&&game.weapon==WeaponType::Shotgun)||(b.action==Action::Rifle&&game.weapon==WeaponType::Rifle);
        const bool primary=b.action==Action::Start||b.action==Action::Resume;
        p.rect(b.x,b.y,b.w,b.h,primary||selected?teal:(hover?Vec3{.20f,.32f,.36f}:Vec3{.10f,.16f,.20f}));
        if (hover) p.rect(b.x,b.y,4,b.h,amber);
        const std::string label=b.action==Action::NextLevel&&!game.levels.readyToAdvance()?"LEVEL CLEARED...":b.action==Action::DayNight?(game.night?"N  NIGHT":"N  DAY"):
            b.action==Action::Sound?(!game.soundAvailable?"NO AUDIO DEVICE":game.soundEnabled?"M  SOUND ON":"M  SOUND OFF"):b.text;
        p.text(b.x+18,b.y+(b.h-14)/2,label,2,primary||selected?Vec3{.03f,.08f,.10f}:ink);
    }
    return p.vertices;
}
}
