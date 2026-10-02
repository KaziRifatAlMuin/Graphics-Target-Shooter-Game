#pragma once
#include "gameplay/Game.h"
#include "ui/UiState.h"
namespace shooter {
// Platform-independent mode/menu/persistence flow, shared by GLFW and tests.
class SessionController {
public:
    SessionController(Game& game,Leaderboard& board);
    UiState ui;
    bool pointerUnlocked=false,exitRequested=false,snapshotDirty=true,inputReset=false;
    void action(Action selected);
    void update(float dt);
    void scroll(int rows);
    void type(unsigned int codepoint);
    void backspace();
    void acceptName();
    void refreshBoard();
private:
    Screen animatedScreen=Screen::Menu;
    Game& game;
    Leaderboard& leaderboard;
    std::vector<EligibleResult> pendingSaves;
    void begin(GameMode mode,int level=1);
    void saveResults();
};
}
