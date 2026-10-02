#pragma once
#include "SessionController.h"
#include <string>
namespace shooter {
float modeSmokeStep(int frame,SessionController& session,Game& game,Leaderboard& board);
std::string modeSmokeCapture(int frame);
bool modeSmokeFinished(int frame,SessionController& session,Game& game,Leaderboard& board);
}
