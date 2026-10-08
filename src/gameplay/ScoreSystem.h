#pragma once
#include <string>
#include <vector>
namespace shooter {
// Accumulate scoring and event counts for a run; elapsed time is measured in seconds.
struct RunStats {
    int score=0, shots=0, hits=0, destroyed=0, bullseyes=0;
    int birdHits=0, humanHits=0, levelsCleared=0;
    double elapsed=0;
};
// A temporary on-screen score message expires when its remaining life reaches zero.
struct ScorePopup { std::string text; float life=1.5f; bool penalty=false; };
// Centralize target rewards, character penalties, and score-message lifetimes.
class ScoreSystem {
public:
    // Award 150 for a one-shot destruction or 100 otherwise, and queue a score popup.
    static void target(RunStats& stats, std::vector<ScorePopup>& feedback, bool oneShot);
    // Subtract 200 for a human or 100 for a bird and record the matching penalty.
    static void penalty(RunStats& stats, std::vector<ScorePopup>& feedback, bool human);
    // Decrease popup lifetimes, remove expired messages, and retain at most six.
    static void update(std::vector<ScorePopup>& feedback, float dt);
};
}
