#include "gameplay/LevelManager.h"
#include "levels/Level1.h"
#include "levels/Level2.h"
#include "levels/Level3.h"
#include "levels/Level4.h"
#include "levels/Level5.h"
#include "levels/Level6.h"
#include "levels/Level7.h"
#include <algorithm>
#include <stdexcept>
namespace shooter {
// Select one of the seven level definitions; reject out-of-range numbers.
LevelConfig levelConfiguration(int number) {
    switch (number) {
    case 1:return Level1{}.configure(); case 2:return Level2{}.configure();
    case 3:return Level3{}.configure(); case 4:return Level4{}.configure();
    case 5:return Level5{}.configure(); case 6:return Level6{}.configure();
    case 7:return Level7{}.configure();
    default:throw std::invalid_argument("Level must be 1 through 7.");
    }
}
// Load a level with a fresh intro timer and zero level time.
void LevelManager::load(int number) {
    config=levelConfiguration(number); stage=LevelStage::Intro; transition=0; levelTime=0;
}
// Advance non-playing transition time and start gameplay after the 1.5-second intro.
void LevelManager::updateTransition(float dt) {
    if (stage==LevelStage::Active) return;
    transition+=dt;
    if (stage==LevelStage::Intro && transition>=1.5f) { stage=LevelStage::Active; transition=0; }
}
// Finish only when all targets are eliminated, then checkpoint statistics for restarts.
bool LevelManager::finishIfComplete(const std::vector<Target>& targets, RunStats& stats) {
    if (stage!=LevelStage::Active || targets.empty() ||
        !std::all_of(targets.begin(),targets.end(),[](const Target& t) { return t.eliminated; })) return false;
    ++stats.levelsCleared; completedStats=stats; transition=0;
    stage=config.number==7?LevelStage::Finished:LevelStage::Complete;
    return true;
}
// Require a completed level and a one-second results delay before continuing.
bool LevelManager::readyToAdvance() const { return stage==LevelStage::Complete && transition>=1.0f; }
// Load the next numbered level only after the completion delay.
bool LevelManager::advance() {
    if (!readyToAdvance()) return false;
    load(config.number+1); return true;
}
}
