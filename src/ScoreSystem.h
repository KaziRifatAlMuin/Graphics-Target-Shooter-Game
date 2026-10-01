#pragma once
#include <string>
#include <vector>
namespace shooter {
struct RunStats {
    int score=0, shots=0, hits=0, destroyed=0, bullseyes=0;
    int birdHits=0, humanHits=0, levelsCleared=0;
    float elapsed=0;
};
struct ScorePopup { std::string text; float life=1.5f; bool penalty=false; };
class ScoreSystem {
public:
    static void target(RunStats& stats, std::vector<ScorePopup>& feedback, bool oneShot);
    static void penalty(RunStats& stats, std::vector<ScorePopup>& feedback, bool human);
    static void update(std::vector<ScorePopup>& feedback, float dt);
};
}
