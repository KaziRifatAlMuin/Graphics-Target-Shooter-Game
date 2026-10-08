#version 330 core
in vec2 uv;
uniform bool night;
out vec4 fragmentColor;
// Blend horizon and overhead sky colors by height; smoothstep uses t*t*(3-2*t) for a soft transition.
void main() {
    vec3 horizon=night?vec3(0.045,0.065,0.12):vec3(0.67,0.81,0.93);
    vec3 zenith=night?vec3(0.006,0.012,0.04):vec3(0.20,0.48,0.78);
    fragmentColor=vec4(mix(horizon,zenith,smoothstep(0.15,1.0,uv.y)),1.0);
}
