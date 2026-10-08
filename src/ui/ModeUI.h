#pragma once
#include "ui/Interface.h"
namespace shooter {
// Draw mode-specific pages and return whether they replace the normal gameplay interface.
bool drawModePage(ui::Painter& painter,const Game& game,Screen screen,const UiState& state);
// Draw the mode timer, respawn notices, and Developer diagnostics over the game.
void drawModeHud(ui::Painter& painter,const Game& game,const UiState& state);
// Round remaining seconds upward and display minutes=floor(s/60), seconds=s%60.
std::string clockText(double seconds);
}
