#include "rendering/Renderer.h"
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
    std::string source = buffer.str();
    const std::string directive="#include \"lighting.glsl\"";
    const auto include=source.find(directive);
    if (include!=std::string::npos) {
        std::ifstream shared(path.parent_path()/"lighting.glsl");
        if (!shared) throw std::runtime_error("Cannot read shared lighting shader.");
        std::ostringstream code; code<<shared.rdbuf();
        source.replace(include,directive.size(),code.str());
    }
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
    if (skyProgram) glDeleteProgram(skyProgram);
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
    shadingLocation=glGetUniformLocation(program,"shadingMode");
    specularLocation=glGetUniformLocation(program,"materialSpecular");
    shininessLocation=glGetUniformLocation(program,"shininess");
    emissionLocation=glGetUniformLocation(program,"emission");
    patternLocation=glGetUniformLocation(program,"targetPattern");
    flashLocation=glGetUniformLocation(program,"targetFlash");
    patternScaleLocation=glGetUniformLocation(program,"patternScale");
    patternOffsetLocation=glGetUniformLocation(program,"patternOffset");
    if (modelLocation<0 || viewLocation<0 || projectionLocation<0 || colorLocation<0)
        throw std::runtime_error("Required shader uniform missing.");
    normalMatrixLocation=glGetUniformLocation(program,"normalMatrix");
    eyePositionLocation=glGetUniformLocation(program,"eyePosition");
    ambientColorLocation=glGetUniformLocation(program,"ambientColor");
    sunDirectionLocation=glGetUniformLocation(program,"sunDirection");
    sunColorLocation=glGetUniformLocation(program,"sunColor");
    for (int i=0;i<8;++i) {
        const std::string prefix="points["+std::to_string(i)+"].";
        pointLocations[i]={glGetUniformLocation(program,(prefix+"position").c_str()),
                           glGetUniformLocation(program,(prefix+"color").c_str())};
    }
    for (int i=0;i<6;++i) {
        const std::string prefix="spots["+std::to_string(i)+"].";
        spotLocations[i]={glGetUniformLocation(program,(prefix+"position").c_str()),
                          glGetUniformLocation(program,(prefix+"direction").c_str()),
                          glGetUniformLocation(program,(prefix+"color").c_str()),
                          glGetUniformLocation(program,(prefix+"innerCos").c_str()),
                          glGetUniformLocation(program,(prefix+"outerCos").c_str())};
    }

    // Shared unit cube: 8 local corners, expanded to 36 triangle vertices.
    const Vec3 corners[] = {
        {-0.5f,-0.5f,-0.5f},{0.5f,-0.5f,-0.5f},{0.5f,0.5f,-0.5f},{-0.5f,0.5f,-0.5f},
        {-0.5f,-0.5f,0.5f},{0.5f,-0.5f,0.5f},{0.5f,0.5f,0.5f},{-0.5f,0.5f,0.5f}
    };
    const int faces[6][4] = {{4,5,6,7},{1,0,3,2},{0,4,7,3},{5,1,2,6},{3,7,6,2},{0,1,5,4}};
    const int triangles[] = {0,1,2,0,2,3};
    const Vec3 normals[]={{0,0,1},{0,0,-1},{-1,0,0},{1,0,0},{0,1,0},{0,-1,0}};
    std::vector<float> vertices;
    for (int face=0; face<6; ++face) for (int corner : triangles) {
        const Vec3 p=corners[faces[face][corner]];
        const Vec3 n=normals[face];
        vertices.insert(vertices.end(),{p.x,p.y,p.z,n.x,n.y,n.z});
    }
    glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo);
    glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(float)),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),reinterpret_cast<void*>(3*sizeof(float)));
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
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,sizeof(UiVertex),reinterpret_cast<void*>(2*sizeof(float)));
    glEnableVertexAttribArray(1); glBindVertexArray(0);
    const GLuint skyVertex=compileShader(GL_VERTEX_SHADER,directory/"sky.vert");
    GLuint skyFragment=0;
    try { skyFragment=compileShader(GL_FRAGMENT_SHADER,directory/"sky.frag"); }
    catch (...) { glDeleteShader(skyVertex); throw; }
    skyProgram=glCreateProgram();
    glAttachShader(skyProgram,skyVertex); glAttachShader(skyProgram,skyFragment); glLinkProgram(skyProgram);
    glDeleteShader(skyVertex); glDeleteShader(skyFragment);
    glGetProgramiv(skyProgram,GL_LINK_STATUS,&ok);
    if (!ok) throw std::runtime_error("Sky shader link failed.");
}
void Renderer::drawInterface(const std::vector<UiVertex>& vertices) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(uiProgram); glBindVertexArray(uiVao); glBindBuffer(GL_ARRAY_BUFFER,uiVbo);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(UiVertex)),vertices.data(),GL_STREAM_DRAW);
    glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0); glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
}
void Renderer::drawTransformedCube(const SceneObject& object) {
    const Mat4 model=composeModelMatrix(object.transform);
    glUniformMatrix4fv(modelLocation,1,GL_FALSE,model.data.data());
    // Compute normal matrix on CPU to avoid per-vertex transpose(inverse(mat3(model))) in the shader.
    if (normalMatrixLocation>=0) {
        const Vec3 a{model.at(0,0),model.at(1,0),model.at(2,0)}, b{model.at(0,1),model.at(1,1),model.at(2,1)},
                   c{model.at(0,2),model.at(1,2),model.at(2,2)};
        const float det=dot(a,cross(b,c));
        if (std::abs(det)>1e-8f) {
            const float invDet=1.0f/det;
            const Vec3 r0=cross(b,c)*invDet, r1=cross(c,a)*invDet, r2=cross(a,b)*invDet;
            // Column-major mat3: each column is one of the cofactor rows.
            const float nm[9]={r0.x,r0.y,r0.z, r1.x,r1.y,r1.z, r2.x,r2.y,r2.z};
            glUniformMatrix3fv(normalMatrixLocation,1,GL_FALSE,nm);
        }
    }
    glUniform3f(colorLocation,object.color.x,object.color.y,object.color.z);
    glUniform1f(specularLocation,object.specular); glUniform1f(shininessLocation,object.shininess);
    glUniform1f(emissionLocation,object.emission); glUniform1f(flashLocation,object.flash);
    glUniform1i(patternLocation,object.targetPattern);
    glUniform3f(patternScaleLocation,object.patternScale.x,object.patternScale.y,object.patternScale.z);
    glUniform3f(patternOffsetLocation,object.patternOffset.x,object.patternOffset.y,object.patternOffset.z);
    glDrawArrays(GL_TRIANGLES,0,36);
}
void Renderer::drawArena(const std::vector<SceneObject>& objects, const Mat4& view, const Mat4& projection,Vec3 eye,bool night) {
    glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
    glUseProgram(skyProgram);
    if (skyNightLocation<0) skyNightLocation=glGetUniformLocation(skyProgram,"night");
    glUniform1i(skyNightLocation,night);
    glBindVertexArray(vao); glDrawArrays(GL_TRIANGLES,0,3);
    glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE);
    glUseProgram(program);
    glUniform1i(shadingLocation,shadingMode);
    const auto lighting=createLighting(night);
    // Use cached uniform locations instead of per-frame string lookups.
    glUniform3f(eyePositionLocation,eye.x,eye.y,eye.z);
    glUniform3f(ambientColorLocation,lighting.ambient.x,lighting.ambient.y,lighting.ambient.z);
    glUniform3f(sunDirectionLocation,lighting.sunDirection.x,lighting.sunDirection.y,lighting.sunDirection.z);
    glUniform3f(sunColorLocation,lighting.sunColor.x,lighting.sunColor.y,lighting.sunColor.z);
    for (std::size_t i=0;i<lighting.points.size();++i) {
        glUniform3f(pointLocations[i].position,lighting.points[i].position.x,lighting.points[i].position.y,lighting.points[i].position.z);
        glUniform3f(pointLocations[i].color,lighting.points[i].color.x,lighting.points[i].color.y,lighting.points[i].color.z);
    }
    for (std::size_t i=0;i<lighting.spots.size();++i) {
        const auto& light=lighting.spots[i];
        glUniform3f(spotLocations[i].position,light.position.x,light.position.y,light.position.z);
        glUniform3f(spotLocations[i].direction,light.direction.x,light.direction.y,light.direction.z);
        glUniform3f(spotLocations[i].color,light.color.x,light.color.y,light.color.z);
        glUniform1f(spotLocations[i].innerCos,light.innerCos);
        glUniform1f(spotLocations[i].outerCos,light.outerCos);
    }
    glUniformMatrix4fv(viewLocation,1,GL_FALSE,view.data.data());
    glUniformMatrix4fv(projectionLocation,1,GL_FALSE,projection.data.data());
    glBindVertexArray(vao);
    for (const auto& object : objects) drawTransformedCube(object);
    glBindVertexArray(0);
}
}
