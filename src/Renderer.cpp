#include "Renderer.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace shooter {
namespace {
GLuint compileShader(GLenum kind, const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Cannot read shader: " + path.string());
    std::ostringstream buffer;
    buffer << file.rdbuf();
    const std::string source = buffer.str();
    const char* text = source.c_str();
    const GLuint shader = glCreateShader(kind);
    glShaderSource(shader,1,&text,nullptr);
    glCompileShader(shader);
    GLint ok=0;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
    if (!ok) {
        char log[4096]{};
        glGetShaderInfoLog(shader,sizeof(log),nullptr,log);
        glDeleteShader(shader);
        throw std::runtime_error("Shader compile failed (" + path.string() + "): " + log);
    }
    return shader;
}
}
Renderer::~Renderer() {
    if (uiVbo) glDeleteBuffers(1,&uiVbo);
    if (uiVao) glDeleteVertexArrays(1,&uiVao);
    if (uiProgram) glDeleteProgram(uiProgram);
    if (vbo) glDeleteBuffers(1,&vbo);
    if (vao) glDeleteVertexArrays(1,&vao);
    if (program) glDeleteProgram(program);
}
void Renderer::initialize(const std::filesystem::path& directory) {
    const GLuint vertex=compileShader(GL_VERTEX_SHADER,directory/"object.vert");
    GLuint fragment=0;
    try { fragment=compileShader(GL_FRAGMENT_SHADER,directory/"object.frag"); }
    catch (...) { glDeleteShader(vertex); throw; }
    program=glCreateProgram();
    glAttachShader(program,vertex); glAttachShader(program,fragment);
    glLinkProgram(program);
    glDeleteShader(vertex); glDeleteShader(fragment);
    GLint ok=0;
    glGetProgramiv(program,GL_LINK_STATUS,&ok);
    if (!ok) {
        char log[4096]{};
        glGetProgramInfoLog(program,sizeof(log),nullptr,log);
        throw std::runtime_error(std::string("Shader link failed: ")+log);
    }
    modelLocation=glGetUniformLocation(program,"model");
    viewLocation=glGetUniformLocation(program,"view");
    projectionLocation=glGetUniformLocation(program,"projection");
    colorLocation=glGetUniformLocation(program,"objectColor");
    if (modelLocation<0 || viewLocation<0 || projectionLocation<0 || colorLocation<0)
        throw std::runtime_error("Required shader uniform missing.");

    // Shared unit cube: 8 local corners, expanded to 36 triangle vertices.
    const Vec3 corners[] = {
        {-0.5f,-0.5f,-0.5f},{0.5f,-0.5f,-0.5f},{0.5f,0.5f,-0.5f},{-0.5f,0.5f,-0.5f},
        {-0.5f,-0.5f,0.5f},{0.5f,-0.5f,0.5f},{0.5f,0.5f,0.5f},{-0.5f,0.5f,0.5f}
    };
    const int faces[6][4] = {{4,5,6,7},{1,0,3,2},{0,4,7,3},{5,1,2,6},{3,7,6,2},{0,1,5,4}};
    const int triangles[] = {0,1,2,0,2,3};
    const float tones[] = {0.86f,0.70f,0.76f,0.90f,1.0f,0.55f};
    std::vector<float> vertices;
    for (int face=0; face<6; ++face) for (int corner : triangles) {
        const Vec3 p=corners[faces[face][corner]];
        vertices.insert(vertices.end(),{p.x,p.y,p.z,tones[face]});
    }
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(float)),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,4*sizeof(float),nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,4*sizeof(float),reinterpret_cast<void*>(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    const GLuint uiVertex=compileShader(GL_VERTEX_SHADER,directory/"ui.vert");
    GLuint uiFragment=0;
    try { uiFragment=compileShader(GL_FRAGMENT_SHADER,directory/"ui.frag"); }
    catch (...) { glDeleteShader(uiVertex); throw; }
    uiProgram=glCreateProgram();
    glAttachShader(uiProgram,uiVertex); glAttachShader(uiProgram,uiFragment); glLinkProgram(uiProgram);
    glDeleteShader(uiVertex); glDeleteShader(uiFragment);
    glGetProgramiv(uiProgram,GL_LINK_STATUS,&ok);
    if (!ok) throw std::runtime_error("UI shader link failed.");
    glGenVertexArrays(1,&uiVao); glGenBuffers(1,&uiVbo);
    glBindVertexArray(uiVao); glBindBuffer(GL_ARRAY_BUFFER,uiVbo);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(UiVertex),nullptr); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(UiVertex),reinterpret_cast<void*>(2*sizeof(float)));
    glEnableVertexAttribArray(1); glBindVertexArray(0);
}
void Renderer::drawInterface(const std::vector<UiVertex>& vertices) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(uiProgram); glBindVertexArray(uiVao); glBindBuffer(GL_ARRAY_BUFFER,uiVbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(UiVertex)),vertices.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0); glEnable(GL_DEPTH_TEST);
}
void Renderer::drawTransformedCube(const SceneObject& object) {
    const Mat4 model=composeModelMatrix(object.transform);
    glUniformMatrix4fv(modelLocation,1,GL_FALSE,model.data.data());
    glUniform3f(colorLocation,object.color.x,object.color.y,object.color.z);
    glDrawArrays(GL_TRIANGLES,0,36);
}
void Renderer::drawArena(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& projection) {
    glUseProgram(program);
    glUniformMatrix4fv(viewLocation,1,GL_FALSE,view.data.data());
    glUniformMatrix4fv(projectionLocation,1,GL_FALSE,projection.data.data());
    glBindVertexArray(vao);
    for (const auto& object : objects) drawTransformedCube(object);
    glBindVertexArray(0);
}
}
