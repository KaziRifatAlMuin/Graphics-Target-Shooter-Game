#pragma once
#include "core/SceneObject.h"
namespace shooter {
// Find the first ray hit, p(t)=origin+t*direction, up to maximum; infinity means no contact.
float intersectCube(Vec3 origin, Vec3 direction, const Transform& box, float maximum);
// Reject positions outside the arena or inside an obstacle's padded horizontal footprint.
bool canStandAt(Vec3 position, const std::vector<SceneObject>& objects);
}
