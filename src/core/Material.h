#pragma once
#include <string>

namespace shooter {
// Layer order is shared with TextureCache and tools/generate_textures.cpp.
enum class Material { Plain, Concrete, Masonry, Gravel, Wood, Metal, Fabric, Paper, Rubber };
// Choose a texture by object role, such as wood for crates and fabric for characters.
inline Material defaultMaterial(const std::string& type,const std::string& component) {
    if (type=="Boundary") return Material::Masonry;
    if (type=="Arena") return component=="Floor slab"?Material::Gravel:Material::Concrete;
    if (type=="Cargo") return component=="Crate"?Material::Wood:
        component=="Crate inventory plate"?Material::Paper:Material::Metal;
    if (type=="Weapon") return component=="GRIP"||component=="STOCK"||component=="FORE_END"?Material::Rubber:Material::Metal;
    if (type=="Target") return component=="Cube-built six-ring plate"?Material::Paper:Material::Metal;
    if (type=="Lighting") return component=="Concrete footing"?Material::Concrete:Material::Metal;
    if (type=="Human" || type=="Bird" || type=="Player") return Material::Fabric;
    if (type=="Environment") return component=="Equipment mat"?Material::Rubber:Material::Metal;
    return Material::Plain;
}
// Return texture repeats per meter; higher values make the visible pattern smaller.
inline float materialTiling(Material material) {
    switch(material) {
        case Material::Masonry: return .5f;
        case Material::Gravel: return .65f;
        case Material::Wood: return 1.f;
        case Material::Metal: return 3.f;
        case Material::Fabric: return 6.f;
        case Material::Paper: return 2.f;
        case Material::Rubber: return 8.f;
        default: return 1.f;
    }
}
}
