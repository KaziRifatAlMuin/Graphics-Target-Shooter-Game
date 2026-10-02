#pragma once
#include "gameplay/Game.h"
#include "ui/UiState.h"
#include "ui/UiPainter.h"

namespace shooter {
struct Button { float x,y,w,h; std::string text; Action action; };
std::vector<Button> screenButtons(Screen screen,GameMode mode=GameMode::Practice);
Action clickedAction(Screen screen, float x, float y,GameMode mode=GameMode::Practice);
std::vector<UiVertex> buildInterface(const Game& game, Screen screen, float mouseX, float mouseY, bool pointerFree,const UiState* state=nullptr);
}
