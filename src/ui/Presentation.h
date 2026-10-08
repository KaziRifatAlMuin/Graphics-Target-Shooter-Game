#pragma once
#include "gameplay/Game.h"
#include "ui/UiState.h"
#include "ui/UiPainter.h"
namespace shooter {
// Choose how long each completion animation lasts before showing the result page.
float resultAnimationDuration(Screen screen);
// Draw an intro countdown or completion overlay while its transition timer is active.
bool drawPresentation(ui::Painter& painter,const Game& game,Screen screen,const UiState& state);
}
