#pragma once
#include "core/SceneObject.h"
namespace shooter {
// Create the daytime sun's visible cube; actual sunlight is calculated separately by the lighting rig.
std::vector<SceneObject> createCelestialObjects(bool night);
// Reuse the actual weapon and projectile builders to construct a stationary equipment exhibit.
std::vector<SceneObject> createEquipmentDisplay();
}
