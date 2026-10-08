#version 330 core
in vec4 vertexColor;
out vec4 fragmentColor;
// Pass interpolated RGBA (red, green, blue, opacity) to the framebuffer for blending.
void main() { fragmentColor=vertexColor; }
