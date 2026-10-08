#pragma once
#include "core/SceneObject.h"
namespace shooter {
// Construct the 60x100 m arena from scaled cubes; Y is height and the floor surface is Y=0.
std::vector<SceneObject> createArena();
}
