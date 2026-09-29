#pragma once
#include "Transform.h"
#include <filesystem>
#include <string>
#include <vector>

namespace shooter {
enum class Primitive { Cube, RoundTarget };
struct SceneObject {
    std::string id, type, component;
    Transform transform;
    Vec3 color;
    std::string notes;
    Primitive primitive=Primitive::Cube;
    float specular=.12f, shininess=24, emission=0, flash=0;
};
std::vector<SceneObject> createArena();
void writeCalculations(const std::vector<SceneObject>& objects, const std::filesystem::path& path);
}
