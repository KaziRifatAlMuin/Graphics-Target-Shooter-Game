#version 330 core
#include "lighting.glsl"
in vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
uniform vec3 objectColor;
uniform float emission, targetFlash;
in vec3 vertexDiffuse, vertexSpecular;
flat in vec3 flatDiffuse, flatSpecular;
uniform bool targetPattern;
out vec4 fragmentColor;
vec3 surfaceColor() {
    if (!targetPattern) return objectColor;
    if (shapeNormal.z<0.5) return vec3(0.15,0.18,0.21);
    float radius=length(shapePosition.xy)*2.0;
    int ring=clamp(int(radius*6.0),0,5);
    vec3 colors[6]=vec3[6](vec3(0.96,0.10,0.07),vec3(1.0,0.75,0.12),
        vec3(0.12,0.44,0.84),vec3(0.94,0.93,0.85),vec3(0.82,0.12,0.10),vec3(0.94,0.93,0.85));
    vec3 color=colors[ring];
    if (fract(radius*6.0)>0.95) color=vec3(0.08);
    return mix(color,vec3(1.0,0.76,0.2),targetFlash*0.5);
}
void main() {
    vec3 base=surfaceColor(), diffuse, specular;
    if(shadingMode==0) { diffuse=flatDiffuse; specular=flatSpecular; }
    else if(shadingMode==1) { diffuse=vertexDiffuse; specular=vertexSpecular; }
    else lighting(worldPosition,worldNormal,diffuse,specular);
    vec3 lit=base*diffuse+specular;
    lit=mix(lit,base,emission);
    fragmentColor=vec4(pow(clamp(lit,0.0,1.0),vec3(1.0/2.2)),1.0);
}
