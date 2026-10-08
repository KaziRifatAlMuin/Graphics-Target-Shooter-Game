#pragma once
#include "gameplay/Npc.h"
#include "world/Dimensions.h"
namespace shooter {
// A human reuses shared character state and adds a height used to scale its cube body.
struct Human : NpcState { float height=dimensions::humanHeight; };
// Create a walker with seeded speed/height variation and a collision-free starting position.
Human createHuman(std::size_t zone, Vec3 home, unsigned seed, const std::vector<SceneObject>& obstacles,
                  const std::vector<Target>& targets={});
// Handle falling, resolve overlaps with moving stands, and walk toward bounded waypoints.
void updateHuman(Human& human, float dt, const std::vector<SceneObject>& obstacles, const std::vector<Target>& targets);
// Assemble a proportionally scaled cube person with opposite arm and leg swings.
std::vector<SceneObject> createHumanObjects(const Human& human, std::size_t id);
}
