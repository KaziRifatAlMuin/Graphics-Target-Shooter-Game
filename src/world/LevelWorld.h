#pragma once
#include "levels/LevelBase.h"
namespace shooter {
// Combine the arena, fixtures, and cargo, retrying up to four seeded layouts if accessibility fails.
std::vector<SceneObject> createLevelWorld(const LevelConfig& config);
// Throws with a specific target ID if a generated layout is unfair.
void validateLevelWorld(const LevelConfig& config, const std::vector<SceneObject>& objects);
// Cast a ray from one point to another; any closer cube contact blocks the line of sight.
bool clearSightline(Vec3 from, Vec3 to, const std::vector<SceneObject>& objects);
}
