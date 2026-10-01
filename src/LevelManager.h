#pragma once
#include "levels/LevelBase.h"
#include "ScoreSystem.h"
namespace shooter {
enum class LevelStage { Intro, Active, Complete, Finished };
class LevelManager {
public:
    LevelConfig config;
    LevelStage stage=LevelStage::Intro;
    float transition=0, levelTime=0;
    RunStats completedStats;
    void load(int number);
    void updateTransition(float dt);
    bool finishIfComplete(const std::vector<Target>& targets, RunStats& stats);
    bool advance();
    bool readyToAdvance() const;
};
LevelConfig levelConfiguration(int number);
}
