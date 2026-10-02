#pragma once
#include "levels/LevelBase.h"
namespace shooter {
std::vector<SceneObject> createLevelWorld(const LevelConfig& config);
// Throws with a specific target ID if a generated layout is unfair.
void validateLevelWorld(const LevelConfig& config, const std::vector<SceneObject>& objects);
bool clearSightline(Vec3 from, Vec3 to, const std::vector<SceneObject>& objects);
}
