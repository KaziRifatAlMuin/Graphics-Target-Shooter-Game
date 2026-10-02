#pragma once
#include "ui/UiPainter.h"
#include "ui/UiState.h"
namespace shooter {
void drawLeaderboard(ui::Painter& painter,const UiState& state,const std::string& highlightedName);
}
