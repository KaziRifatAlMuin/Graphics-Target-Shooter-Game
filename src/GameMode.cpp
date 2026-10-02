#include "GameMode.h"
namespace shooter {
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
