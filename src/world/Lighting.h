#pragma once
#include "world/Arena.h"

namespace shooter {
// A point source has position and RGB intensity; it shines in all directions with distance falloff.
struct PointLight { Vec3 position, color; };
// A spotlight adds a unit direction and inner/outer cone cosines for a soft beam edge.
struct SpotLight { Vec3 position, direction, color; float innerCos, outerCos; };
// Collect constant ambient light, a directional sun, and the point/spot source lists.
struct LightingRig {
    Vec3 ambient, sunDirection, sunColor;
    std::vector<PointLight> points;
    std::vector<SpotLight> spots;
};
// Configure ambient background, parallel sunlight, omnidirectional point lamps, and cone-shaped spotlights.
LightingRig createLighting(bool night);
// Build visible lamp housings and towers at the same positions used by the lighting calculations.
std::vector<SceneObject> createLightFixtures();
}
