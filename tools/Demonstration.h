#pragma once
#include "rendering/Renderer.h"
#include <fstream>
#include <map>

namespace shooter::demo {
namespace fs = std::filesystem;
// A capture view stores eye/look-at positions plus near/far distances used for clipping.
struct View { Vec3 eye, center; float nearPlane=.01f, farPlane=500; };
// Group a model's cube parts, their source information, and named alternate poses.
struct Example {
    std::vector<SceneObject> parts;
    std::string provenance;
    std::map<std::string,std::vector<SceneObject>> states;
};
using Catalogue = std::map<std::string, Example>;
// Format numeric values with nine significant digits for reproducible demonstration tables.
std::string number(float value);
// Print XYZ with three decimal places for the generated scene reference.
std::string vector(Vec3 value);
// Convert labels into lowercase filename components, replacing punctuation with hyphens.
std::string slug(std::string value);
// Create the document's directory and make output failures raise an exception.
std::ofstream document(const fs::path& path);
// Embed the current source file verbatim so generated guides match the implementation.
void source(std::ostream& out, const fs::path& root, const std::string& file);
// Print matrix entries by row for human reading, regardless of their column-major storage.
void matrix(std::ostream& out, const Mat4& value);
// Transform all cube corners, find the bounding box, and position a camera far enough to frame it.
View fit(const std::vector<SceneObject>& objects, Vec3 direction={1,.65f,1});
// Use the game renderer to save lossless documentation images and count successful captures.
class Capture {
public:
    // Initialize the actual game renderer for documentation screenshots.
    explicit Capture(const fs::path& root);
    Renderer renderer;
    std::size_t images=0;
    // Render a chosen view and save its exact framebuffer pixels as a lossless PNG.
    void save(const fs::path& path, const std::vector<SceneObject>& objects,
              const View& view, bool night=false, int mask=15, int shading=2,
              int width=480, int height=360, float fieldOfView=45);
};
Catalogue demonstrateObjects(Capture& capture, const fs::path& root);
void demonstrateLighting(Capture& capture, const fs::path& root, const Catalogue& examples);
}
