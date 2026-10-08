#pragma once
#include "core/SceneObject.h"
#include "world/Dimensions.h"
namespace shooter {
// Set the human reference size and number of crate-layout rows.
struct CargoConfig {
    float humanHeight=dimensions::humanHeight;
    int rows=7;
};
// Use a fixed random seed to reproduce crate stacks while preserving walking corridors.
std::vector<SceneObject> generateCargoLayout(unsigned seed, const CargoConfig& config={});
// Describe a level's targets, movement permission, seed, and character counts without running it.
struct LevelConfig;
// Add level-dependent cover and remove crates that overlap the targets' possible movement area.
std::vector<SceneObject> generateChallengeCargo(const LevelConfig& level);
}
