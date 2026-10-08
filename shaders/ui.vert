#version 330 core
layout(location=0) in vec2 position;
layout(location=1) in vec4 color;
out vec4 vertexColor;
// Map 1280x800 UI coordinates to OpenGL: x'=2*x/1280-1, y'=1-2*y/800 (Y flips).
void main() {
    gl_Position=vec4(position.x/640.0-1.0,1.0-position.y/400.0,0.0,1.0);
    vertexColor=color;
}
