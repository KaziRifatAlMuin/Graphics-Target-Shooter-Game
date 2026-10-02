#pragma once
#include "world/Arena.h"

namespace shooter {
struct PointLight { Vec3 position, color; };
struct SpotLight { Vec3 position, direction, color; float innerCos, outerCos; };
struct LightingRig {
    Vec3 ambient, sunDirection, sunColor;
    std::vector<PointLight> points;
    std::vector<SpotLight> spots;
};
LightingRig createLighting(bool night);
std::vector<SceneObject> createLightFixtures();
}
