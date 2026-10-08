#version 330 core
out vec2 uv;
// Generate one oversized triangle covering the screen, without supplying a separate sky mesh.
void main() {
    // Vertex IDs 0,1,2 become corners (0,0),(2,0),(0,2) through bit shifts and masks.
    vec2 p=vec2((gl_VertexID<<1)&2,gl_VertexID&2);
    uv=p;
    gl_Position=vec4(p*2.0-1.0,1.0,1.0);
}
