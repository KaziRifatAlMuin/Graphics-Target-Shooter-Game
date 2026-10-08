#pragma once
#include "gameplay/SessionController.h"
#include <string>
namespace shooter {
// Drive scripted session actions at specific frames to exercise Free, Developer, and Bird's-Eye modes.
float modeSmokeStep(int frame,SessionController& session,Game& game,Leaderboard& board);
// Name selected smoke-test frames so screenshots correspond to known UI/game states.
std::string modeSmokeCapture(int frame);
// Check final scripted-session outcomes and signal when the smoke run may stop.
bool modeSmokeFinished(int frame,SessionController& session,Game& game,Leaderboard& board);
}
