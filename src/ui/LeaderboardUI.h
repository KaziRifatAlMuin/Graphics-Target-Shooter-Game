#pragma once
#include "ui/UiPainter.h"
#include "ui/UiState.h"
namespace shooter {
// Draw a scrolling window of ranking rows, highlighting the current player's name.
void drawLeaderboard(ui::Painter& painter,const UiState& state,const std::string& highlightedName);
}
