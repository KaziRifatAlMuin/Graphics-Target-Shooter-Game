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
    // Dispatch a named UI command through the shared session flow.
    void action(Action selected);
    // Advance timers and gameplay, then process completed results and screen transitions.
    void update(float dt);
    // Move the leaderboard viewport while clamping it to valid rows.
    void scroll(int rows);
    // Append one printable character during name entry.
    void type(unsigned int codepoint);
    // Remove the last typed name character.
    void backspace();
    // Trim surrounding spaces and keep editing if the name is empty.
    void acceptName();
    // Reload saved rankings and report persistence errors in UI state.
    void refreshBoard();
    // Refresh the warning and rank for an already-used player name.
    void checkChallengeName();
private:
    Screen animatedScreen=Screen::Menu;
    Game& game;
    Leaderboard& leaderboard;
    std::vector<EligibleResult> pendingSaves;
    // Initialize a named game session and reset input state.
    void begin(GameMode mode,int level=1);
    // Open name entry for the mode the user selected.
    void requestName(GameMode mode);
    // Start play or open Developer level selection after name validation.
    void launchNamedMode();
    // Retry queued completed results without losing unsaved statistics.
    void saveResults();
};
}
