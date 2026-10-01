#pragma once
#include "SceneObject.h"
namespace shooter {
float intersectCube(Vec3 origin, Vec3 direction, const Transform& box, float maximum);
bool canStandAt(Vec3 position, const std::vector<SceneObject>& objects);
}
