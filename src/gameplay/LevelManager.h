#pragma once
#include "levels/LevelBase.h"
#include "gameplay/ScoreSystem.h"
namespace shooter {
// A level moves from intro to active play, then completion or the final finished state.
enum class LevelStage { Intro, Active, Complete, Finished };
// Track the current level, transition timers, and the last completed statistics checkpoint.
class LevelManager {
public:
    LevelConfig config;
    LevelStage stage=LevelStage::Intro;
    float transition=0, levelTime=0;
    RunStats completedStats;
    // Load a level with a fresh intro timer and zero level time.
    void load(int number);
    // Advance non-playing transition time and start gameplay after the 1.5-second intro.
    void updateTransition(float dt);
    // Finish only when all targets are eliminated, then checkpoint statistics for restarts.
    bool finishIfComplete(const std::vector<Target>& targets, RunStats& stats);
    // Load the next numbered level only after the completion delay.
    bool advance();
    // Require a completed level and a one-second results delay before continuing.
    bool readyToAdvance() const;
};
LevelConfig levelConfiguration(int number);
}
