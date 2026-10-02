#pragma once
#include "gameplay/Game.h"
#include "ui/UiState.h"
#include "ui/UiPainter.h"
namespace shooter {
float resultAnimationDuration(Screen screen);
bool drawPresentation(ui::Painter& painter,const Game& game,Screen screen,const UiState& state);
}
