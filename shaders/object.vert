#version 330 core
#include "lighting.glsl"
layout(location=0) in vec3 localPosition;
layout(location=1) in vec3 localNormal;
uniform mat4 model, view, projection;
uniform mat3 normalMatrix;
uniform vec3 patternScale, patternOffset;
uniform vec3 textureRepeat;
out vec2 materialUV;
out vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
out vec3 vertexDiffuse, vertexSpecular;
flat out vec3 flatDiffuse, flatSpecular;
// Run once per cube vertex to send its position, normal, texture coordinates, and optional lighting onward.
void main() {
    // Model matrix places the local cube in the world: world=model*(x,y,z,1).
    vec4 world=model*vec4(localPosition,1.0);
    worldPosition=world.xyz;
    // Inverse-transpose normal matrix keeps surface directions correct under scale and shear.
    worldNormal=normalMatrix*localNormal;
    shapePosition=localPosition*patternScale+patternOffset;
    shapeNormal=localNormal;
    // Face-local planar UVs, scaled in meters; follows rotation and shear without swimming.
    vec3 uvPosition=(localPosition+vec3(0.5))*textureRepeat;
    materialUV=abs(localNormal.y)>.5?uvPosition.xz:
        abs(localNormal.x)>.5?uvPosition.zy:uvPosition.xy;
    vertexDiffuse=vec3(0); vertexSpecular=vec3(0);
    // Flat and Gouraud compute vertex lighting here; Gouraud interpolates it, while flat keeps one vertex's value.
    if(shadingMode!=2) lighting(worldPosition,worldNormal,vertexDiffuse,vertexSpecular);
    // Flat uses one provoking vertex result for the entire triangle.
    flatDiffuse=vertexDiffuse; flatSpecular=vertexSpecular;
    // clip=projection*view*model*local; the GPU divides by clip.w to obtain screen-relative coordinates.
    gl_Position=projection*view*world;
}
