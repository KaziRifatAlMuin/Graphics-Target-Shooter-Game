#pragma once
#include "core/SceneObject.h"
#include "world/Dimensions.h"
namespace shooter {
struct CargoConfig {
    float humanHeight=dimensions::humanHeight;
    int rows=7;
};
std::vector<SceneObject> generateCargoLayout(unsigned seed, const CargoConfig& config={});
struct LevelConfig;
std::vector<SceneObject> generateChallengeCargo(const LevelConfig& level);
}
