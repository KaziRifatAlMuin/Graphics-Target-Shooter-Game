#pragma once
#include "core/SceneObject.h"
namespace shooter {
// Cosmetic cubes only: no score, collider or gameplay clock side effects.
std::vector<SceneObject> celebrationObjects(float time,bool victory);
}
