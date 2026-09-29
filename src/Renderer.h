#pragma once
#include <glad/gl.h>
#include "Arena.h"

namespace shooter {
class Renderer {
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    void initialize(const std::filesystem::path& shaderDirectory);
    void drawArena(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& projection);
private:
    GLuint program=0, vao=0, vbo=0;
    GLint modelLocation=-1, viewLocation=-1, projectionLocation=-1, colorLocation=-1;
    void drawTransformedCube(const SceneObject& object);
};
}
