#include "ScoreSystem.h"
#include <algorithm>
namespace shooter {
void ScoreSystem::target(RunStats& s, std::vector<ScorePopup>& f, bool oneShot) {
    s.score+=oneShot?150:100; ++s.destroyed; if (oneShot) ++s.bullseyes;
    f.push_back({oneShot?"+150 BULLSEYE":"+100 TARGET",1.5f,false});
}
void ScoreSystem::penalty(RunStats& s, std::vector<ScorePopup>& f, bool human) {
    s.score-=human?200:100;
    if (human) ++s.humanHits; else ++s.birdHits;
    f.push_back({human?"-200 HUMAN":"-100 BIRD",1.5f,true});
}
void ScoreSystem::update(std::vector<ScorePopup>& f, float dt) {
    for (auto& p:f) p.life-=dt;
    f.erase(std::remove_if(f.begin(),f.end(),[](const ScorePopup& p) { return p.life<=0; }),f.end());
    if (f.size()>6) f.erase(f.begin(),f.end()-6);
}
}
