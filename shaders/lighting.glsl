// Shared by vertex and fragment shaders. Phong reflection, not Blinn-Phong.
uniform vec3 eyePosition, ambientColor, sunDirection, sunColor;
uniform float materialSpecular, shininess;
uniform int shadingMode; // 0 flat, 1 Gouraud, 2 Phong (default)
struct PointLight { vec3 position; vec3 color; };
struct SpotLight { vec3 position; vec3 direction; vec3 color; float innerCos; float outerCos; };
uniform PointLight points[8];
uniform SpotLight spots[6];
// Accumulate one light's diffuse (matte) and specular (shiny) reflection using unit directions.
void addLight(vec3 N, vec3 V, vec3 L, vec3 color, inout vec3 diffuse, inout vec3 specular) {
    // Lambert diffuse = lightColor*max(N.L,0); a surface facing away receives no direct light.
    float ndotl=max(dot(N,L),0.0);
    diffuse+=color*ndotl;
    // R is the reflection of incoming light; V points toward the viewer.
    vec3 R=reflect(-L,N);
    // Phong highlight = lightColor*ks*max(R.V,0)^shininess; larger shininess narrows the highlight.
    if (ndotl>0.0) specular+=color*materialSpecular*pow(max(dot(R,V),0.0),shininess);
}
// Ambient is constant background light; directional light has parallel rays, points radiate, spots form cones.
void lighting(vec3 position, vec3 normal, out vec3 diffuse, out vec3 specular) {
    // Normalize the surface normal N and view direction V before dot-product angle calculations.
    vec3 N=normalize(normal), V=normalize(eyePosition-position);
    diffuse=ambientColor; specular=vec3(0);
    addLight(N,V,normalize(sunDirection),sunColor,diffuse,specular);
    for(int i=0;i<8;++i) {
        if(dot(points[i].color,points[i].color)==0.0) continue;
        vec3 delta=points[i].position-position;
        float d=max(length(delta),0.001);
        // Point falloff = 1/(1+0.09*d+0.032*d^2), where d is distance to the light.
        float attenuation=1.0/(1.0+0.09*d+0.032*d*d);
        addLight(N,V,delta/d,points[i].color*attenuation,diffuse,specular);
    }
    for(int i=0;i<6;++i) {
        if(dot(spots[i].color,spots[i].color)==0.0) continue;
        vec3 delta=spots[i].position-position;
        float d=max(length(delta),0.001);
        // Cosine of the spotlight angle = directionFromLight.spotDirection; larger values are nearer its center.
        float cone=dot(-delta/d,spots[i].direction);
        // Soft cone edge: t=clamp((cosAngle-outerCos)/(innerCos-outerCos),0,1), intensity=t*t*(3-2*t).
        float intensity=smoothstep(spots[i].outerCos,spots[i].innerCos,cone);
        // Spot falloff = 1/(1+0.025*d+0.002*d^2); multiply by cone intensity to limit its beam.
        float attenuation=1.0/(1.0+0.025*d+0.002*d*d);
        addLight(N,V,delta/d,spots[i].color*intensity*attenuation,diffuse,specular);
    }
}
