#include "gameplay/GameMode.h"
namespace shooter {
// Convert an internal mode value to its stable text label for UI and saved records.
const char* modeName(GameMode mode) {
    switch (mode) {
    case GameMode::Challenge:return "Challenge";
    case GameMode::Free:return "Free";
    case GameMode::Developer:return "Developer";
    case GameMode::BirdsEye:return "BirdsEye";
    default:return "Practice";
    }
}
}
