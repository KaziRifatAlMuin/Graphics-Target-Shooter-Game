#version 330 core
layout (location = 0) in vec3 localPosition;
layout (location = 1) in float faceShade;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out float shade;
void main() {
    gl_Position = projection * view * model * vec4(localPosition, 1.0);
    shade = faceShade;
}
