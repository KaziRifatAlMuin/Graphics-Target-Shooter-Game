#version 330 core
in float shade;
uniform vec3 objectColor;
out vec4 fragmentColor;
void main() {
    // Fixed face tones expose cube geometry. Lighting is added in Phase 5.
    fragmentColor = vec4(objectColor * shade, 1.0);
}
