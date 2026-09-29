#version 330 core
in vec3 worldPosition, worldNormal, shapePosition, shapeNormal;
uniform vec3 objectColor, eyePosition, ambientColor, sunDirection, sunColor;
uniform float materialSpecular, shininess, emission, targetFlash;
uniform bool targetPattern;
struct PointLight { vec3 position; vec3 color; };
struct SpotLight { vec3 position; vec3 direction; vec3 color; float innerCos; float outerCos; };
uniform PointLight points[8];
uniform SpotLight spots[6];
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
vec3 illuminate(vec3 base,vec3 N,vec3 V,vec3 L,vec3 color) {
    float diffuse=max(dot(N,L),0.0);
    vec3 H=normalize(L+V);
    float specular=diffuse>0.0?materialSpecular*pow(max(dot(N,H),0.0),shininess):0.0;
    return color*(base*diffuse+vec3(specular));
}
void main() {
    vec3 base=surfaceColor(), N=normalize(worldNormal), V=normalize(eyePosition-worldPosition);
    vec3 lit=ambientColor*base;
    lit+=illuminate(base,N,V,normalize(sunDirection),sunColor);
    for (int i=0;i<8;++i) {
        vec3 offset=points[i].position-worldPosition;
        float distance=length(offset);
        float attenuation=1.0/(1.0+0.045*distance+0.003*distance*distance);
        lit+=illuminate(base,N,V,normalize(offset),points[i].color*attenuation);
    }
    for (int i=0;i<6;++i) {
        vec3 offset=spots[i].position-worldPosition;
        float distance=length(offset);
        float cone=dot(normalize(-offset),normalize(spots[i].direction));
        float intensity=smoothstep(spots[i].outerCos,spots[i].innerCos,cone);
        float attenuation=1.0/(1.0+0.025*distance+0.002*distance*distance);
        lit+=illuminate(base,N,V,normalize(offset),spots[i].color*intensity*attenuation);
    }
    lit=mix(lit,base,emission);
    fragmentColor=vec4(pow(clamp(lit,0.0,1.0),vec3(1.0/2.2)),1.0);
}
