#pragma once
#include <glad/gl.h>
#include <filesystem>

namespace shooter {
// One immutable, shared array; no per-object texture allocation or loading.
class TextureCache {
public:
    // Free the OpenGL texture array while the graphics context is still available.
    ~TextureCache();
    // Validate and load the nine material images into one GPU texture array.
    void initialize(const std::filesystem::path& directory);
    // Select the texture array on texture unit zero for the material shader.
    void bind() const;
private:
    GLuint texture=0;
};
}
