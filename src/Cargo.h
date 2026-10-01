#pragma once
#include "SceneObject.h"
namespace shooter {
struct CargoConfig {
    float humanHeight=1.8f;
    int rows=7;
};
std::vector<SceneObject> generateCargoLayout(unsigned seed, const CargoConfig& config={});
struct LevelConfig;
std::vector<SceneObject> generateChallengeCargo(const LevelConfig& level);
}
