#pragma once
#include "gameplay/Npc.h"
namespace shooter {
// A bird reuses the shared NPC (non-player character) state and adds flying behavior.
struct Bird : NpcState {};
// Create a bird with repeatable speed, animation phase, and waypoints near its assigned target.
Bird createBird(std::size_t zone, Vec3 home, unsigned seed);
// Fly toward a waypoint while alive, or integrate gravity and tumbling after a hit.
void updateBird(Bird& bird, float dt, const std::vector<SceneObject>& obstacles);
// Assemble the bird from cubes; each part follows the bird's position, heading, and falling pose.
std::vector<SceneObject> createBirdObjects(const Bird& bird, std::size_t id);
}
