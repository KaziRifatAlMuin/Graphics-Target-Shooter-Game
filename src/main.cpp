#include "Renderer.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace shooter;

namespace {
fs::path projectDirectory(const char* executable) {
#ifdef SHOOTER_SOURCE_DIR
    const fs::path configured = SHOOTER_SOURCE_DIR;
    if (fs::exists(configured/"project.md")) return configured;
#endif
    // Works from the repo, build directories, or an executable launched from another CWD.
    for (auto candidate : {fs::absolute(executable).parent_path(),fs::current_path()}) {
        for (;;) {
            if (fs::exists(candidate/"project.md") && fs::exists(candidate/"shaders"/"object.vert"))
                return candidate;
            const auto parent=candidate.parent_path();
            if (parent==candidate || parent.empty()) break;
            candidate=parent;
        }
    }
    throw std::runtime_error("Cannot locate project.md and shaders beside the project executable.");
}
void processInput(GLFWwindow* window, Camera& camera, float dt) {
    auto down=[&](int key) { return glfwGetKey(window,key)==GLFW_PRESS; };
    if (down(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window,GLFW_TRUE);
    if (down(GLFW_KEY_HOME)) camera=Camera{};
    if (!glfwGetWindowAttrib(window,GLFW_FOCUSED)) return;
    camera.move(float(down(GLFW_KEY_W)-down(GLFW_KEY_S)),
                float(down(GLFW_KEY_D)-down(GLFW_KEY_A)),
                float(down(GLFW_KEY_E)-down(GLFW_KEY_Q)),dt,
                down(GLFW_KEY_LEFT_SHIFT)||down(GLFW_KEY_RIGHT_SHIFT));
}
void verifyFrame(int width, int height, const fs::path& capture) {
    // Read the actual GPU result: a blank clear-color window must fail the smoke test.
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::size_t ground=0,wall=0;
    for (std::size_t i=0; i<pixels.size(); i+=3) {
        const int r=pixels[i], g=pixels[i+1], b=pixels[i+2];
        if (g>r+8 && g>b+8) ++ground;
        if (r>55 && r<175 && g>r && b>=g && b<195) ++wall;
    }
    if (ground<500 || wall<500) throw std::runtime_error("Rendered frame is missing visible floor or walls.");
    if (!capture.empty()) {
        std::ofstream out(capture,std::ios::binary);
        out << "P6\n" << width << ' ' << height << "\n255\n";
        for (int row=height-1; row>=0; --row)
            out.write(reinterpret_cast<const char*>(pixels.data()+static_cast<std::size_t>(row)*width*3),width*3);
        out.close();
        if (!out) throw std::runtime_error("Cannot write frame capture: "+capture.string());
    }
    std::cout << "Frame verified: " << ground << " floor pixels; " << wall << " wall pixels.\n";
}
void runScene(GLFWwindow* window, const fs::path& root,
              const std::vector<SceneObject>& objects, bool smoke, const fs::path& capture) {
    Renderer renderer;
    renderer.initialize(root/"shaders");
    glEnable(GL_DEPTH_TEST);
    Camera camera;
    double previous=glfwGetTime(), lastX=0,lastY=0;
    bool looking=false;
    int frames=0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const double now=glfwGetTime();
        const float dt=static_cast<float>(std::min(now-previous,0.1));
        previous=now;
        processInput(window,camera,dt);
        const bool wantsLook=glfwGetWindowAttrib(window,GLFW_FOCUSED)
            && glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS;
        if (wantsLook!=looking) {
            looking=wantsLook;
            glfwSetInputMode(window,GLFW_CURSOR,looking?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(window,&lastX,&lastY);
        }
        if (looking) {
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            camera.look(static_cast<float>(x-lastX),static_cast<float>(y-lastY));
            lastX=x; lastY=y;
        }
        int width,height;
        glfwGetFramebufferSize(window,&width,&height);
        if (width==0 || height==0) { glfwWaitEventsTimeout(0.05); continue; }
        glViewport(0,0,width,height);
        glClearColor(0.56f,0.73f,0.86f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        renderer.drawArena(objects,camera.getViewMatrix(),
                           makePerspective(60,float(width)/float(height),0.1f,400));
        if (smoke && ++frames==3) {
            verifyFrame(width,height,capture);
            if (glGetError()!=GL_NO_ERROR) throw std::runtime_error("OpenGL reported a rendering error.");
            glfwSetWindowShouldClose(window,GLFW_TRUE);
        }
        glfwSwapBuffers(window);
    }
} // Renderer releases GPU resources before the window/context is destroyed.
}

int main(int argc, char** argv) {
    GLFWwindow* window=nullptr;
    bool initialized=false;
    try {
        bool exportOnly=false,smoke=false;
        fs::path csvPath,capture;
        for (int i=1; i<argc; ++i) {
            const std::string arg=argv[i];
            if (arg=="--export-calc") exportOnly=true;
            else if (arg=="--smoke-test") smoke=true;
            else if ((arg=="--calc" || arg=="--capture") && i+1<argc) {
                (arg=="--calc"?csvPath:capture)=argv[++i];
            } else if (arg=="--help") {
                std::cout << "3D Target Shooter - Phase 1\n"
                    "WASD: fly | Q/E: down/up | Shift: faster | hold RMB: look | Home: reset | Esc: exit\n"
                    "--export-calc : regenerate CSV without opening OpenGL\n"
                    "--calc PATH   : override CSV output (default: project root/calc.csv)\n"
                    "--smoke-test  : render and verify three frames in a hidden window, then exit\n"
                    "--capture PATH: with --smoke-test, save the verified frame as PPM\n";
                return 0;
            } else throw std::runtime_error("Unknown or incomplete argument: "+arg);
        }
        if ((!capture.empty() && !smoke) || (exportOnly && smoke))
            throw std::runtime_error("Use --capture with --smoke-test; run --export-calc separately.");
        const fs::path root=projectDirectory(argv[0]);
        const auto objects=createArena();
        if (csvPath.empty()) csvPath=root/"calc.csv";
        writeCalculations(objects,csvPath);
        std::cout << "Generated " << csvPath.string() << " (" << objects.size() << " objects).\n";
        if (exportOnly) return 0;

        glfwSetErrorCallback([](int code,const char* message) {
            std::cerr << "GLFW " << code << ": " << message << '\n';
        });
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW.");
        initialized=true;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        if (smoke) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        window=glfwCreateWindow(1280,800,
            "3D Target Shooter | Phase 1 | WASD + QE: move | RMB: look | Home: reset | Esc: exit",
            nullptr,nullptr);
        if (!window) throw std::runtime_error("Cannot create an OpenGL 3.3 window.");
        glfwMakeContextCurrent(window);
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) || !GLAD_GL_VERSION_3_3)
            throw std::runtime_error("OpenGL 3.3 is required.");
        glfwSwapInterval(smoke?0:1);
        std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\nRenderer: " << glGetString(GL_RENDERER) << '\n';
        runScene(window,root,objects,smoke,capture);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        if (window) glfwDestroyWindow(window);
        if (initialized) glfwTerminate();
        return 1;
    }
}
