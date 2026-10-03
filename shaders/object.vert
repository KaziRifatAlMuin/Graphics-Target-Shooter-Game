#version 330 core
#include "lighting.glsl"
layout(location=0) in vec3 localPosition;
layout(location=1) in vec3 localNormal;
uniform mat4 model, view, projection;
uniform mat3 normalMatrix;
uniform vec3 patternScale, patternOffset;
out vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
out vec3 vertexDiffuse, vertexSpecular;
flat out vec3 flatDiffuse, flatSpecular;
void main() {
    vec4 world=model*vec4(localPosition,1.0);
    worldPosition=world.xyz;
    worldNormal=normalMatrix*localNormal;
    shapePosition=localPosition*patternScale+patternOffset;
    shapeNormal=localNormal;
    vertexDiffuse=vec3(0); vertexSpecular=vec3(0);
    if(shadingMode!=2) lighting(worldPosition,worldNormal,vertexDiffuse,vertexSpecular);
    // Flat uses one provoking vertex result for the entire triangle.
    flatDiffuse=vertexDiffuse; flatSpecular=vertexSpecular;
    gl_Position=projection*view*world;
}
