#pragma once
#include "Transform.h"
#include <string>
#include <vector>

namespace shooter {
// All world geometry is an instance of the centered unit cube.
struct SceneObject {
    std::string id, type, component;
    Transform transform;
    Vec3 color;
    std::string notes;
    float specular=.12f, shininess=24, emission=0, flash=0;
    bool targetPattern=false;
    Vec3 patternScale{1,1,1}, patternOffset{};
    int level=0;
};
inline Vec3 asVec3(Vec4 v) { return {v.x,v.y,v.z}; }
inline float length(Vec3 v) { return std::sqrt(dot(v,v)); }
inline SceneObject makeCube(std::string id, std::string type, std::string component,
                            Vec3 pos, Vec3 scale, Vec3 color, float yaw=0) {
    Transform t; t.position=pos; t.scale=scale; t.rotation.y=yaw;
    return {id,type,component,t,color,"1 unit = 1 m; shared cube transform pipeline"};
}
}
