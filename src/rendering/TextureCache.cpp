#include "rendering/TextureCache.h"
#include <fstream>
#include <stdexcept>
#include <vector>

namespace shooter {
// Delete the GPU texture array when its owner is destroyed.
TextureCache::~TextureCache() { if(texture) glDeleteTextures(1,&texture); }
// Load nine RGB images into one texture array; each material selects a layer instead of a new texture.
void TextureCache::initialize(const std::filesystem::path& directory) {
    if(texture) return;
    constexpr int size=256, layers=9;
    const char* names[]={"plain","concrete","masonry","gravel","wood","metal","fabric","paper","rubber"};
    // Validate every asset before allocating GPU storage. P6 linear RGB reflectance maps.
    std::vector<unsigned char> pixels(size*size*3*layers);
    for(int layer=0;layer<layers;++layer) {
        const auto path=directory/(std::string(names[layer])+".ppm");
        std::ifstream file(path,std::ios::binary);
        std::string magic; int width=0,height=0,maximum=0;
        file>>magic>>width>>height>>maximum;
        if(!file || magic!="P6" || width!=size || height!=size || maximum!=255)
            throw std::runtime_error("Expected 256x256 P6 texture: "+path.string());
        char separator=0; file.get(separator);
        if(separator=='\r' && file.peek()=='\n') file.get();
        else if(separator!='\n' && separator!=' ' && separator!='\t')
            throw std::runtime_error("Invalid texture header: "+path.string());
        file.read(reinterpret_cast<char*>(pixels.data()+layer*size*size*3),size*size*3);
        if(!file) throw std::runtime_error("Truncated texture: "+path.string());
    }
    glGenTextures(1,&texture); bind();
    // Use byte-aligned rows so OpenGL reads packed RGB pixels without assuming extra padding.
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RGB8,size,size,layers,0,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // Precompute smaller texture versions; filtered mipmaps reduce distant flicker and jagged patterns.
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    if(glGetError()!=GL_NO_ERROR) throw std::runtime_error("Material texture upload failed.");
}
// Make the material texture array available on texture unit zero.
void TextureCache::bind() const { glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D_ARRAY,texture); }
}
