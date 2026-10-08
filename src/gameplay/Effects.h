#pragma once
#include "core/SceneObject.h"
namespace shooter {
// Cosmetic cubes only: no score, collider or gameplay clock side effects.
// Return animated confetti at the requested time; its parabolic fall uses y=y0+vy*t-2.5*t^2.
std::vector<SceneObject> celebrationObjects(float time,bool victory);
}
