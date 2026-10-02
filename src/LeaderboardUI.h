#pragma once
#include "UiPainter.h"
#include "UiState.h"
namespace shooter {
void drawLeaderboard(ui::Painter& painter,const UiState& state,const std::string& highlightedName);
}
