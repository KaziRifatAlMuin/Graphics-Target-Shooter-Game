#include "ui/LeaderboardUI.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
namespace shooter {
void drawLeaderboard(ui::Painter& p,const UiState& state,const std::string& name) {
    using namespace ui;
    p.rect(56,310,1168,342,{.035f,.065f,.09f});
    const float columns[]={74,134,398,506,600,712,822,930,1040};
    const char* headings[]={"RANK","NAME","SCORE","LEVELS","TARGETS","BULLS","BIRDS","HUMANS","TIME (S)"};
    for (int i=0;i<9;++i) p.text(columns[i],327,headings[i],1.65f,teal);
    for (int line=0;line<leaderboardVisibleRows;++line) {
        const int index=state.firstRow+line;
        if (index>=int(state.rows.size())) break;
        const auto& r=state.rows[index]; const auto& s=r.stats; const float y=362+line*38.f;
        const bool selected=r.name==name;
        p.rect(64,y-8,1152,34,selected?Vec3{.14f,.30f,.29f}:line%2?Vec3{.055f,.095f,.12f}:Vec3{.035f,.065f,.09f});
        std::ostringstream time; time<<std::fixed<<std::setprecision(2)<<s.elapsed;
        const std::string values[]={std::to_string(index+1),r.name.substr(0,24),std::to_string(s.score),
            std::to_string(s.levelsCleared),std::to_string(s.destroyed),std::to_string(s.bullseyes),
            std::to_string(s.birdHits),std::to_string(s.humanHits),time.str()};
        for (int i=0;i<9;++i) p.text(columns[i],y,values[i],1.65f,selected?amber:ink);
    }
    if (state.rows.empty()) p.text(90,395,"NO COMPLETED RESULTS YET. FINISH A LEVEL OR A FREE SESSION.",2,muted);
    p.text(74,625,"ROWS "+std::to_string(state.rows.empty()?0:state.firstRow+1)+"-"+
        std::to_string(std::min(int(state.rows.size()),state.firstRow+leaderboardVisibleRows))+" / "+std::to_string(state.rows.size())+
        "   MOUSE WHEEL / PAGE UP / PAGE DOWN",1.6f,muted);
}
}
