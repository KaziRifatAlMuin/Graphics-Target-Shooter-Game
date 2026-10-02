#pragma once
#include <glad/gl.h>
#include "world/Arena.h"
#include "ui/Interface.h"
#include <filesystem>
#include <vector>

namespace shooter {
class Renderer {
public:
    Renderer() = default;
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    void initialize(const std::filesystem::path& shaderDirectory);
    void drawArena(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& projection, Vec3 eye, bool night);
    void drawInterface(const std::vector<UiVertex>& vertices);
private:
    GLuint program=0, vao=0, vbo=0;
    GLuint uiProgram=0, uiVao=0, uiVbo=0;
    GLuint skyProgram=0;
    GLint patternScaleLocation=-1, patternOffsetLocation=-1;
    GLint modelLocation=-1, viewLocation=-1, projectionLocation=-1, colorLocation=-1;
    GLint specularLocation=-1, shininessLocation=-1, emissionLocation=-1, patternLocation=-1, flashLocation=-1;
    GLint normalMatrixLocation=-1;
    // Cached lighting uniform locations to avoid per-frame glGetUniformLocation lookups.
    GLint eyePositionLocation=-1, ambientColorLocation=-1, sunDirectionLocation=-1, sunColorLocation=-1;
    GLint skyNightLocation=-1;
    struct PointLightLocations { GLint position, color; };
    struct SpotLightLocations { GLint position, direction, color, innerCos, outerCos; };
    PointLightLocations pointLocations[8]{};
    SpotLightLocations spotLocations[6]{};
    void drawTransformedCube(const SceneObject& object);
};
}
