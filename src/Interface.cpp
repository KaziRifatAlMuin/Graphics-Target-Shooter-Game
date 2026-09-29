#include "Interface.h"
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
    if (screen==Screen::Playing) return {{1000,22,122,40,"MENU",Action::Menu},{1134,22,112,40,"EXIT",Action::Exit},
        {28,716,220,52,"1  PISTOL",Action::Pistol},{262,716,220,52,"2  SHOTGUN",Action::Shotgun},
        {496,716,274,52,"3  ASSAULT RIFLE",Action::Rifle}};
    if (screen==Screen::Controls) return {{96,660,430,56,"BACK",Action::Back}};
    const bool paused=screen==Screen::Paused;
    return {{96,400,430,56,paused?"RESUME SESSION":"START SESSION",paused?Action::Resume:Action::Start},
            {96,472,430,56,"VIEW CONTROLS",Action::Controls},
            {96,544,430,56,paused?"MAIN MENU":"EXIT",paused?Action::Menu:Action::Exit},
            {96,616,430,56,paused?"EXIT":"",paused?Action::Exit:Action::None}};
}
Action clickedAction(Screen screen,float x,float y) {
    for (const auto& b:screenButtons(screen)) if (inside(b,x,y)) return b.action;
    return Action::None;
}
std::vector<UiVertex> buildInterface(const Game& game,Screen screen,float mx,float my,bool pointerFree) {
    Painter p;
    if (screen!=Screen::Playing) {
        p.rect(56,66,530,668,{.035f,.065f,.09f}); p.rect(56,66,5,668,teal);
        p.text(96,104,"RANGE / 2107042",2,teal);
        if (screen==Screen::Controls) {
            p.text(96,158,"CONTROLS",4,ink);
            const char* lines[]={"WASD       WALK / FREE CAMERA","MOUSE      LOOK AND AIM","LEFT CLICK / SPACE    FIRE",
                "1 / 2 / 3  SELECT WEAPON","SHIFT      MOVE FASTER","F1         PLAYER VIEW","F2 / F3    ARENA / SIDE VIEW",
                "F4         FREE CAMERA","Q / E      FREE CAMERA DOWN / UP","R          RESET TARGETS","TAB        RELEASE / CAPTURE MOUSE",
                "ESC        PAUSE / RESUME","F5         SAVE CALC SNAPSHOT"};
            float y=222;
            for (const char* line:lines) { p.text(96,y,line,1.8f,ink); y+=30; }
        } else {
            p.text(96,162,screen==Screen::Paused?"SESSION":"TARGET",5,ink);
            p.text(96,212,screen==Screen::Paused?"PAUSED":"SHOOTER",5,ink);
            p.text(96,280,"MOVING TARGETS. THREE WEAPONS.",2,amber);
            p.text(96,320,"WASD TO WALK. MOUSE TO AIM.",2,muted);
            p.text(96,348,"LEFT CLICK TO FIRE. ESC FOR MENU.",1.9f,muted);
            if (screen==Screen::Menu) p.text(96,644,"READ THE CONTROLS BEFORE YOU START.",1.7f,muted);
        }
        p.rect(648,584,562,150,{.035f,.065f,.09f});
        p.text(676,610,"THE TRAINING GROUND",2.8f,ink);
        p.text(676,652,"60 X 100 M  /  FULLY ENCLOSED ARENA",1.9f,muted);
        p.text(676,687,"PISTOL 25 M  /  SHOTGUN 18 M  /  RIFLE 70 M",1.8f,teal);
    } else {
        p.rect(20,16,940,88,{.035f,.065f,.09f});
        p.text(40,32,weaponSpec(game.weapon).name,2.8f,ink);
        const char* modes[]={"","PLAYER","ARENA","SIDE","FREE"};
        p.text(430,32,std::string("CAMERA: ")+modes[game.cameraMode],2,teal);
        float distance=0; const int target=game.aimedTarget(distance);
        std::ostringstream range; range<<std::fixed<<std::setprecision(1);
        range<<"TARGET: "; if (target<0) range<<"--"; else range<<distance<<" M";
        range<<"  /  RANGE: "<<int(weaponSpec(game.weapon).range)<<" M";
        p.text(40,72,range.str(),1.9f,target>=0&&distance>weaponSpec(game.weapon).range?amber:muted);
        p.text(650,72,"HITS "+std::to_string(game.hits)+"  CLEARED "+std::to_string(game.destroyed),1.8f,ink);
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
    for (const auto& b:screenButtons(screen)) {
        if (b.action==Action::None) continue;
        const bool hover=pointerFree&&inside(b,mx,my);
        const bool selected=(b.action==Action::Pistol&&game.weapon==WeaponType::Pistol)
            ||(b.action==Action::Shotgun&&game.weapon==WeaponType::Shotgun)||(b.action==Action::Rifle&&game.weapon==WeaponType::Rifle);
        const bool primary=b.action==Action::Start||b.action==Action::Resume;
        p.rect(b.x,b.y,b.w,b.h,primary||selected?teal:(hover?Vec3{.20f,.32f,.36f}:Vec3{.10f,.16f,.20f}));
        if (hover) p.rect(b.x,b.y,4,b.h,amber);
        p.text(b.x+18,b.y+(b.h-14)/2,b.text,2,primary||selected?Vec3{.03f,.08f,.10f}:ink);
    }
    return p.vertices;
}
}
