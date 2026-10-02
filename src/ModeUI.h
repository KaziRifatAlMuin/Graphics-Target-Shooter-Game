#pragma once
#include "Interface.h"
namespace shooter {
bool drawModePage(ui::Painter& painter,const Game& game,Screen screen,const UiState& state);
void drawModeHud(ui::Painter& painter,const Game& game,const UiState& state);
std::string clockText(double seconds);
}
