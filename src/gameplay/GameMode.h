#pragma once
#include "gameplay/ScoreSystem.h"
namespace shooter {
enum class GameMode { Practice, Challenge, Free, Developer, BirdsEye };
const char* modeName(GameMode mode);
inline constexpr double freeSessionSeconds=180.0;
inline constexpr float freeRespawnSeconds=30.0f;
// Emitted once by Game at a completed-level or completed-session boundary.
struct EligibleResult { std::string name; GameMode mode; RunStats stats; };
}
