#pragma once
#include "gameplay/Game.h"
#include "ui/UiState.h"
#include "ui/UiPainter.h"

namespace shooter {
// A clickable rectangle stores its screen coordinates, label, and action.
struct Button { float x,y,w,h; std::string text; Action action; };
// Define each screen's button rectangles and actions, shared by drawing and mouse hit testing.
std::vector<Button> screenButtons(Screen screen,GameMode mode=GameMode::Practice);
// Return the first button containing the pointer, or None when no button is hit.
Action clickedAction(Screen screen, float x, float y,GameMode mode=GameMode::Practice);
// Build colored triangles for the current page, status display, crosshair, buttons, and animations.
std::vector<UiVertex> buildInterface(const Game& game, Screen screen, float mouseX, float mouseY, bool pointerFree,const UiState* state=nullptr);
}
