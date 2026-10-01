#pragma once
#include "Npc.h"
namespace shooter {
struct Bird : NpcState {};
Bird createBird(std::size_t zone, Vec3 home, unsigned seed);
void updateBird(Bird& bird, float dt, const std::vector<SceneObject>& obstacles);
std::vector<SceneObject> createBirdObjects(const Bird& bird, std::size_t id);
}
