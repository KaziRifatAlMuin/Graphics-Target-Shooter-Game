#pragma once
#include "rendering/Renderer.h"
#include <fstream>
#include <map>

namespace shooter::demo {
namespace fs = std::filesystem;
struct View { Vec3 eye, center; float nearPlane=.01f, farPlane=500; };
struct Example {
    std::vector<SceneObject> parts;
    std::string provenance;
    std::map<std::string,std::vector<SceneObject>> states;
};
using Catalogue = std::map<std::string, Example>;
std::string number(float value);
std::string vector(Vec3 value);
std::string slug(std::string value);
std::ofstream document(const fs::path& path);
void source(std::ostream& out, const fs::path& root, const std::string& file);
void matrix(std::ostream& out, const Mat4& value);
View fit(const std::vector<SceneObject>& objects, Vec3 direction={1,.65f,1});
class Capture {
public:
    explicit Capture(const fs::path& root);
    Renderer renderer;
    std::size_t images=0;
    void save(const fs::path& path, const std::vector<SceneObject>& objects,
              const View& view, bool night=false, int mask=15, int shading=2);
};
Catalogue demonstrateObjects(Capture& capture, const fs::path& root);
void demonstrateLighting(Capture& capture, const fs::path& root, const Catalogue& examples);
}
