#pragma once
#include <glad/gl.h>
#include "world/Arena.h"
#include "ui/Interface.h"
#include <filesystem>
#include <vector>
#include "rendering/TextureCache.h"

namespace shooter {
// Own GPU resources and draw the 3D cube scene plus 2D interface triangles.
class Renderer {
public:
    Renderer() = default;
    // Release GPU programs and buffers while the OpenGL context still exists.
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Compile shaders, link drawing programs, and upload the shared cube and interface vertex layouts.
    void initialize(const std::filesystem::path& shaderDirectory);
    int shadingMode=2;
    // isolation is used only by documentation captures: 0 ambient, 1 sun, 2 points, 3 spots.
    // Documentation mask: ambient=1, sun=2, points=4, spots=8. Default preserves gameplay.
    void drawArena(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& projection, Vec3 eye, bool night, int isolation=-1, int lightMask=15);
    // Draw interface triangles over the scene; alpha blending gives C=alpha*front+(1-alpha)*back.
    void drawInterface(const std::vector<UiVertex>& vertices);
private:
    TextureCache textures;
    GLint textureLayerLocation=-1, textureRepeatLocation=-1;
    // Numeric draw-state cache: never copy SceneObject strings in the render loop.
    bool drawStateValid=false, lastPattern=false;
    Vec3 lastColor{},lastRepeat{},lastPatternScale{},lastPatternOffset{};
    float lastLayer=-1,lastSpecular=-1,lastShininess=-1,lastEmission=-1,lastFlash=-1;
    GLint shadingLocation=-1;
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
    // Set this cube's transform and material, then draw its 12 triangles (36 vertices).
    void drawTransformedCube(const SceneObject& object);
};
}
