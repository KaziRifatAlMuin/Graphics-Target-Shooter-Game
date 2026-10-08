#pragma once
#include "gameplay/ScoreSystem.h"
namespace shooter {
// Select the ruleset: practice, progression, timed play, direct level testing, or camera-only inspection.
enum class GameMode { Practice, Challenge, Free, Developer, BirdsEye };
// Convert an internal mode value to its stable text label for UI and saved records.
const char* modeName(GameMode mode);
inline constexpr double freeSessionSeconds=180.0;
inline constexpr float freeRespawnSeconds=30.0f;
// Emitted once by Game at a completed-level or completed-session boundary.
struct EligibleResult { std::string name; GameMode mode; RunStats stats; };
}
