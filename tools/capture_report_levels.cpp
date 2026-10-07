#include "Demonstration.h"
#include "gameplay/Game.h"
#include <GLFW/glfw3.h>
#include <iostream>

// Report photographs share the game's builders, shaders, textures and seeded levels.
int main(int argc, char** argv) {
    using namespace shooter;
    using namespace shooter::demo;
    GLFWwindow* window=nullptr;
    try {
        if(argc!=2) throw std::runtime_error("Usage: capture_report_levels PROJECT_ROOT");
        const auto root=fs::absolute(argv[1]);
        if(!glfwInit()) throw std::runtime_error("GLFW initialization failed");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        window=glfwCreateWindow(480,360,"Report level views",nullptr,nullptr);
        if(!window) throw std::runtime_error("OpenGL context creation failed");
        glfwMakeContextCurrent(window);
        if(!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)))
            throw std::runtime_error("OpenGL loading failed");
        {
            Capture capture(root);
            const View top{{0,140,-25},{0,0,-50},.05f,400};
            for(int level=1;level<=7;++level) {
                Game game;
                game.startMode(GameMode::Developer,level);
                capture.save(root/"docs/images"/("level-top-"+std::to_string(level)+".png"),
                             game.scene(true),top);
            }
            std::cout<<"Saved seven day/Phong top-angle level views at initial simulation state.\n";
        }
        glfwDestroyWindow(window);glfwTerminate();return 0;
    } catch(const std::exception& e) {
        std::cerr<<e.what()<<'\n';
        if(window) glfwDestroyWindow(window);
        glfwTerminate();return 1;
    }
}
