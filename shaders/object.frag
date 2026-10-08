#version 330 core
#include "lighting.glsl"
in vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
uniform vec3 objectColor;
uniform float emission, targetFlash;
uniform sampler2DArray materialTextures;
uniform float textureLayer;
in vec2 materialUV;
in vec3 vertexDiffuse, vertexSpecular;
flat in vec3 flatDiffuse, flatSpecular;
uniform bool targetPattern;
out vec4 fragmentColor;
// Choose the cube color or calculate the target's six colored rings from its local face coordinates.
vec3 surfaceColor() {
    if (!targetPattern) return objectColor;
    if (shapeNormal.z<0.5) return vec3(0.15,0.18,0.21);
    // Normalized radius r=2*sqrt(x*x+y*y); ring=min(floor(6*r),5) selects one of six bands.
    float radius=length(shapePosition.xy)*2.0;
    int ring=clamp(int(radius*6.0),0,5);
    vec3 colors[6]=vec3[6](vec3(0.96,0.10,0.07),vec3(1.0,0.75,0.12),
        vec3(0.12,0.44,0.84),vec3(0.94,0.93,0.85),vec3(0.82,0.12,0.10),vec3(0.94,0.93,0.85));
    vec3 color=colors[ring];
    // fract(6*r) keeps the fractional part; its last 5% draws each ring's dark boundary.
    if (fract(radius*6.0)>0.95) color=vec3(0.08);
    return mix(color,vec3(1.0,0.76,0.2),targetFlash*0.5);
}
// Run per visible fragment (a candidate pixel) to combine texture, surface color, and lighting.
void main() {
    vec3 base=surfaceColor()*texture(materialTextures,vec3(materialUV,textureLayer)).rgb, diffuse, specular;
    // Flat uses constant triangle lighting, Gouraud interpolates vertex lighting, Phong evaluates lighting per fragment.
    if(shadingMode==0) { diffuse=flatDiffuse; specular=flatSpecular; }
    else if(shadingMode==1) { diffuse=vertexDiffuse; specular=vertexSpecular; }
    else lighting(worldPosition,worldNormal,diffuse,specular);
    // Surface light = baseColor*(ambient+diffuse)+specular, preserving the light's highlight color.
    vec3 lit=base*diffuse+specular;
    // Emission blends toward unlit color: result=(1-emission)*lit+emission*base; it does not light nearby objects.
    lit=mix(lit,base,emission);
    // Approximate display gamma correction: displayedRGB=clamp(linearRGB,0,1)^(1/2.2).
    fragmentColor=vec4(pow(clamp(lit,0.0,1.0),vec3(1.0/2.2)),1.0);
}
