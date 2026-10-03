#pragma once
#include <glad/gl.h>
#include <filesystem>

namespace shooter {
// One immutable, shared array; no per-object texture allocation or loading.
class TextureCache {
public:
    ~TextureCache();
    void initialize(const std::filesystem::path& directory);
    void bind() const;
private:
    GLuint texture=0;
};
}
