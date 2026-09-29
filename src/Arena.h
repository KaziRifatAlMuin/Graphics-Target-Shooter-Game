#pragma once
#include "Transform.h"
#include <filesystem>
#include <string>
#include <vector>

namespace shooter {
struct SceneObject {
    std::string id, type, component;
    Transform transform;
    Vec3 color;
    std::string notes;
};
std::vector<SceneObject> createArena();
void writeCalculations(const std::vector<SceneObject>& objects, const std::filesystem::path& path);
}
