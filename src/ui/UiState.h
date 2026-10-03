#pragma once
#include "persistence/Leaderboard.h"
namespace shooter {
enum class Screen { Menu, Controls, Playing, Paused, LevelComplete, Victory, FreeComplete, Developer, Leaderboard, ChallengeName };
enum class Action {
    None, Start, Free, Developer, BirdsEye, Practice, NextLevel, Replay, Resume, Controls, Back, Menu, Exit,
    Pistol, Shotgun, Rifle, DayNight, Sound, Overview, EditName, Leaderboard, BoardFree, BoardChallenge,
    ScrollUp, ScrollDown, RetrySave, ConfirmChallenge, UseExistingName, RewriteName,
    Level1, Level2, Level3, Level4, Level5, Level6, Level7
};
struct UiState {
    Screen screen=Screen::Menu,controlsReturn=Screen::Menu,boardReturn=Screen::Menu;
    std::string name,message;
    GameMode pendingMode=GameMode::Challenge;
    int shadingMode=2;
    bool editingName=false,personalBest=false,hasResult=false,saveFailed=false;
    bool nameExistsWarning=false,duplicateConfirmation=false;
    int existingRank=0,existingScore=0,existingLevels=0;
    GameMode boardMode=GameMode::Challenge;
    std::vector<LeaderboardRecord> rows;
    int firstRow=0,rank=0;
    float fps=0;
    float animationTime=0,screenTime=0;
};
inline bool resultScreen(Screen s) { return s==Screen::LevelComplete || s==Screen::Victory || s==Screen::FreeComplete; }
inline constexpr int leaderboardVisibleRows=7;
}
