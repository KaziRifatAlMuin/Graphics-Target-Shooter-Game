#pragma once
#include "Npc.h"
namespace shooter {
struct Human : NpcState {};
Human createHuman(std::size_t zone, Vec3 home, unsigned seed, const std::vector<SceneObject>& obstacles,
                  const std::vector<Target>& targets={});
void updateHuman(Human& human, float dt, const std::vector<SceneObject>& obstacles, const std::vector<Target>& targets);
std::vector<SceneObject> createHumanObjects(const Human& human, std::size_t id);
}
