#version 330 core
layout(location=0) in vec3 localPosition;
layout(location=1) in vec3 localNormal;
uniform mat4 model, view, projection;
uniform mat3 normalMatrix;
uniform vec3 patternScale, patternOffset;
out vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
void main() {
    vec4 world=model*vec4(localPosition,1.0);
    worldPosition=world.xyz;
    worldNormal=normalMatrix*localNormal;
    shapePosition=localPosition*patternScale+patternOffset;
    shapeNormal=localNormal;
    gl_Position=projection*view*world;
}
