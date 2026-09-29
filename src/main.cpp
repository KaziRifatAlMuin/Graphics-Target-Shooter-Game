#include "Renderer.h"
#include "Sound.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs=std::filesystem;
using namespace shooter;

namespace {
fs::path projectDirectory(const char* executable) {
#ifdef SHOOTER_SOURCE_DIR
    const fs::path configured=SHOOTER_SOURCE_DIR;
    if (fs::exists(configured/"project.md")) return configured;
#endif
    for (auto candidate:{fs::absolute(executable).parent_path(),fs::current_path()}) {
        for (;;) {
            if (fs::exists(candidate/"project.md") && fs::exists(candidate/"shaders"/"object.vert")) return candidate;
            const auto parent=candidate.parent_path();
            if (parent==candidate || parent.empty()) break;
            candidate=parent;
        }
    }
    throw std::runtime_error("Cannot locate project.md and shaders beside the executable.");
}
double verifyFrame(int width,int height,const fs::path& capture,bool checkScene) {
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width)*height*3);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    std::size_t ui=0,visible=0;
    double brightness=0;
    for (std::size_t i=0;i<pixels.size();i+=3) {
        const int r=pixels[i],g=pixels[i+1],b=pixels[i+2];
        brightness+=(r+g+b)/3.0;
        if (r+g+b>45) ++visible;
        if (r<30 && g<50 && b<65) ++ui;
    }
    if (checkScene) {
        std::vector<float> depth(static_cast<std::size_t>(width)*height);
        glReadPixels(0,0,width,height,GL_DEPTH_COMPONENT,GL_FLOAT,depth.data());
        const auto surfaces=std::count_if(depth.begin(),depth.end(),[](float d) { return d<.99999f; });
        if (surfaces<5000 || visible<5000) throw std::runtime_error("Lit scene has no visible geometry.");
    }
    if (!checkScene && ui<500) throw std::runtime_error("UI overlay did not render.");
    if (!capture.empty()) {
        std::ofstream out(capture,std::ios::binary);
        out<<"P6\n"<<width<<' '<<height<<"\n255\n";
        for (int row=height-1;row>=0;--row)
            out.write(reinterpret_cast<const char*>(pixels.data()+static_cast<std::size_t>(row)*width*3),width*3);
        out.close();
        if (!out) throw std::runtime_error("Cannot save capture: "+capture.string());
    }
    if (glGetError()!=GL_NO_ERROR) throw std::runtime_error("OpenGL rendering error.");
    return brightness/(width*height);
}
void runScene(GLFWwindow* window,const fs::path& root,Game& game,const fs::path& csvPath,
              bool smoke,const fs::path& capture) {
    Renderer renderer;
    renderer.initialize(root/"shaders");
    Sound sound;
    game.soundAvailable=sound.initialize();
    const bool audioOpened=game.soundAvailable;
    std::cout<<(game.soundAvailable?"Audio: output device opened.\n":"Audio: no output device; continuing with sound unavailable.\n");
    sound.setEnabled(game.soundEnabled&&!smoke);
    glEnable(GL_DEPTH_TEST);
    Screen screen=Screen::Menu,controlsReturn=Screen::Menu;
    bool pointerUnlocked=false,captured=false,previousClick=false,snapshotDirty=false,fireArmed=false;
    std::array<bool,GLFW_KEY_LAST+1> previousKeys{};
    double previous=glfwGetTime(),lastX=0,lastY=0;
    float sinceSnapshot=0;
    int frame=0;
    double dayBrightness=0;
    auto action=[&](Action a) {
        switch (a) {
        case Action::Start: game.reset(); screen=Screen::Playing; pointerUnlocked=false; fireArmed=false; snapshotDirty=true; break;
        case Action::Resume: screen=Screen::Playing; pointerUnlocked=false; fireArmed=false; break;
        case Action::Controls: controlsReturn=screen; screen=Screen::Controls; break;
        case Action::Back: screen=controlsReturn; break;
        case Action::Menu: screen=screen==Screen::Playing?Screen::Paused:Screen::Menu; snapshotDirty=true; break;
        case Action::Exit: glfwSetWindowShouldClose(window,GLFW_TRUE); break;
        case Action::Pistol: game.weapon=WeaponType::Pistol; game.recoil=0; snapshotDirty=true; break;
        case Action::Shotgun: game.weapon=WeaponType::Shotgun; game.recoil=0; snapshotDirty=true; break;
        case Action::Rifle: game.weapon=WeaponType::Rifle; game.recoil=0; snapshotDirty=true; break;
        case Action::DayNight: game.night=!game.night; snapshotDirty=true; break;
        case Action::Sound: game.soundEnabled=!game.soundEnabled; break;
        default: break;
        }
        sound.setEnabled(game.soundEnabled&&!smoke);
        if (a!=Action::None) sound.play(SoundEvent::Click);
    };
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const double now=glfwGetTime();
        const float dt=smoke?1.0f/60:static_cast<float>(std::min(now-previous,.05));
        previous=now;
        std::array<bool,GLFW_KEY_LAST+1> keys{};
        for (int key=GLFW_KEY_SPACE;key<=GLFW_KEY_LAST;++key) keys[key]=glfwGetKey(window,key)==GLFW_PRESS;
        auto pressed=[&](int key) { return keys[key]&&!previousKeys[key]; };
        if (pressed(GLFW_KEY_N)) action(Action::DayNight);
        if (pressed(GLFW_KEY_M)) action(Action::Sound);
        const bool click=glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;
        const bool clickEdge=click&&!previousClick;
        const bool focused=glfwGetWindowAttrib(window,GLFW_FOCUSED)!=0;
        if (!focused && !smoke && screen==Screen::Playing) screen=Screen::Paused;
        bool consumed=false;
        if (pressed(GLFW_KEY_ESCAPE)) {
            if (screen==Screen::Playing) action(Action::Menu);
            else if (screen==Screen::Paused) action(Action::Resume);
            else if (screen==Screen::Controls) action(Action::Back);
            else action(Action::Exit);
            consumed=true;
        }
        if (pressed(GLFW_KEY_ENTER) && (screen==Screen::Menu || screen==Screen::Paused)) {
            action(screen==Screen::Menu?Action::Start:Action::Resume); consumed=true;
        }
        double mouseX,mouseY;
        glfwGetCursorPos(window,&mouseX,&mouseY);
        int windowW,windowH;
        glfwGetWindowSize(window,&windowW,&windowH);
        const float ux=float(mouseX)*1280/std::max(1,windowW),uy=float(mouseY)*800/std::max(1,windowH);
        bool pointerFree=screen!=Screen::Playing || pointerUnlocked || (game.cameraMode!=1 && game.cameraMode!=4);
        if (clickEdge && pointerFree && !consumed) {
            const auto selected=clickedAction(screen,ux,uy);
            action(selected); consumed=selected!=Action::None;
        }
        if (screen==Screen::Playing && (focused || smoke)) {
            if (pressed(GLFW_KEY_TAB)) pointerUnlocked=!pointerUnlocked;
            for (int mode=1;mode<=4;++mode) if (pressed(GLFW_KEY_F1+mode-1)) game.setCamera(mode);
            if (pressed(GLFW_KEY_1)) action(Action::Pistol);
            if (pressed(GLFW_KEY_2)) action(Action::Shotgun);
            if (pressed(GLFW_KEY_3)) action(Action::Rifle);
            if (pressed(GLFW_KEY_R)) { game.resetTargets(); snapshotDirty=true; }
            if (pressed(GLFW_KEY_F5)) snapshotDirty=true;
            const bool fast=keys[GLFW_KEY_LEFT_SHIFT]||keys[GLFW_KEY_RIGHT_SHIFT];
            if (game.cameraMode==4)
                game.freeCamera.move(float(keys[GLFW_KEY_W]-keys[GLFW_KEY_S]),float(keys[GLFW_KEY_D]-keys[GLFW_KEY_A]),
                                     float(keys[GLFW_KEY_E]-keys[GLFW_KEY_Q]),dt,fast);
            else game.movePlayer(float(keys[GLFW_KEY_W]-keys[GLFW_KEY_S]),float(keys[GLFW_KEY_D]-keys[GLFW_KEY_A]),dt,fast);
        }
        pointerFree=screen!=Screen::Playing || pointerUnlocked || (game.cameraMode!=1 && game.cameraMode!=4);
        const bool captureMouse=!pointerFree && focused && !smoke;
        if (captureMouse!=captured) {
            captured=captureMouse;
            glfwSetInputMode(window,GLFW_CURSOR,captured?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
            glfwGetCursorPos(window,&lastX,&lastY);
        }
        if (captured) {
            double x,y; glfwGetCursorPos(window,&x,&y);
            Camera& lookCamera=game.cameraMode==4?game.freeCamera:game.player;
            lookCamera.look(float(x-lastX),float(y-lastY)); lastX=x; lastY=y;
        }
        // A click used to resume or select UI must be released before it can fire.
        if (pointerFree || consumed) fireArmed=false;
        else if (!click && !keys[GLFW_KEY_SPACE]) fireArmed=true;
        if (screen==Screen::Playing && focused && game.cameraMode==1 && !pointerUnlocked && !consumed && fireArmed) {
            const bool fire=game.weapon==WeaponType::Rifle?(click||keys[GLFW_KEY_SPACE]):(clickEdge||pressed(GLFW_KEY_SPACE));
            if (fire) game.fire();
        }
        if (smoke) {
            // Exercise the same menu actions as pointer/keyboard input, then each view/weapon.
            if (frame==1) action(Action::Controls);
            if (frame==2) action(Action::Back);
            if (frame==3) action(Action::Start);
            if (frame==4) game.fire();
            if (frame==30) { action(Action::Shotgun); game.fire(); }
            if (frame==90) { action(Action::Rifle); game.fire(); }
            if (frame==120) game.setCamera(2);
            if (frame==121) game.setCamera(3);
            if (frame==122) game.setCamera(4);
            if (frame==123) action(Action::Menu);
            if (frame==124) { action(Action::Resume); game.setCamera(1); }
            if (frame==126) { action(Action::DayNight); game.setCamera(2); }
            if (frame==128) game.setCamera(1);
            if (frame==130) action(Action::Menu);
            if (frame==131) action(Action::Resume);
            if (frame==132 || frame==133) action(Action::Sound);
            if (frame==134) action(Action::DayNight);
            if (frame==136 || frame==137) {
                game.setCamera(4);
                game.targets[0].movement=3; game.targets[0].yaw=0; game.targets[0].respawn=0;
                game.freeCamera.position=game.targets[0].base+Vec3{0,0,frame==136?3.0f:-3.0f};
                game.freeCamera.yaw=frame==136?-90:90; game.freeCamera.pitch=0;
            }
        }
        if (screen==Screen::Playing) {
            game.update(dt); sinceSnapshot+=dt;
            if (sinceSnapshot>=1) snapshotDirty=true;
        }
        for (auto event:game.soundEvents) sound.play(event);
        game.soundEvents.clear(); sound.update(); game.soundAvailable=sound.available();
        if (snapshotDirty) {
            writeCalculations(game.calculationObjects(),csvPath);
            snapshotDirty=false; sinceSnapshot=0;
        }
        int width,height; glfwGetFramebufferSize(window,&width,&height);
        previousKeys=keys; previousClick=click;
        if (width==0 || height==0) { glfwWaitEventsTimeout(.05); continue; }
        glViewport(0,0,width,height);
        glClearColor(.56f,.73f,.86f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        const Camera camera=screen==Screen::Playing?game.activeCamera():Camera{};
        const auto objects=game.scene(screen!=Screen::Playing || game.cameraMode!=1);
        renderer.drawArena(objects,camera.getViewMatrix(),makePerspective(60,float(width)/height,.05f,400),camera.position,game.night);
        if (smoke && (frame==120 || frame==125 || frame==127 || frame==129 || frame==136 || frame==137)) {
            const double brightness=verifyFrame(width,height,{},true);
            if (frame==120) dayBrightness=brightness;
            if (frame==127 && brightness>=dayBrightness*.9) throw std::runtime_error("Night lighting did not visibly change the scene.");
        }
        renderer.drawInterface(buildInterface(game,screen,ux,uy,pointerFree));
        if (smoke && (frame==0 || frame==1 || frame==120 || frame==125 || frame==127 || frame==129 || frame==136 || frame==137)) {
            fs::path output;
            if (!capture.empty()) {
                const std::string suffix=frame==0?"-menu":frame==1?"-controls":frame==120?"-arena":
                    frame==127?"-night-arena":frame==129?"-night":frame==136?"-target-front":frame==137?"-target-back":"";
                output=capture.parent_path()/(capture.stem().string()+suffix+capture.extension().string());
            }
            verifyFrame(width,height,output,false);
        }
        if (smoke && frame==138) {
            if (audioOpened&&!sound.available()) throw std::runtime_error("Audio device failed while queuing playback buffers.");
            std::cout<<"PASS: menus, cameras, weapons, day/night lighting, round target front/back, sound toggles, scene and UI rendering.\n";
            glfwSetWindowShouldClose(window,GLFW_TRUE);
        }
        glfwSwapBuffers(window); ++frame;
    }
    writeCalculations(game.calculationObjects(),csvPath);
}
}
int main(int argc,char** argv) {
    GLFWwindow* window=nullptr; bool initialized=false;
    try {
        bool exportOnly=false,smoke=false;
        fs::path csvPath,capture;
        for (int i=1;i<argc;++i) {
            const std::string arg=argv[i];
            if (arg=="--export-calc") exportOnly=true;
            else if (arg=="--smoke-test") smoke=true;
            else if ((arg=="--calc" || arg=="--capture") && i+1<argc) (arg=="--calc"?csvPath:capture)=argv[++i];
            else if (arg=="--help") {
                std::cout<<"3D Target Shooter - Complete game (Phases 1-6)\n"
                    "Start menu: START SESSION / VIEW CONTROLS / EXIT. Enter starts; Esc pauses.\n"
                    "WASD move, mouse aim, click/Space fire, 1/2/3 weapons, Shift sprint, Tab pointer.\n"
                    "F1 player, F2 arena, F3 side, F4 free camera, Q/E fly, R reset targets, F5 snapshot.\n"
                    "N day/night, M sound on/off. Front-only scoring: 1 center shot through 6 outer-ring shots.\n"
                    "--export-calc : export a deterministic starting snapshot without OpenGL\n"
                    "--calc PATH   : override calculation output (default: project root/calc.csv)\n"
                    "--smoke-test  : run scripted menu/gameplay rendering checks in a hidden window\n"
                    "--capture PATH: with --smoke-test, save player/menu/controls/arena PPM previews\n";
                return 0;
            } else throw std::runtime_error("Unknown or incomplete argument: "+arg);
        }
        if ((!capture.empty()&&!smoke)||(exportOnly&&smoke)) throw std::runtime_error("Use --capture with --smoke-test; run --export-calc separately.");
        const auto root=projectDirectory(argv[0]);
        Game game;
        if (csvPath.empty()) csvPath=root/"calc.csv";
        const auto calculations=game.calculationObjects();
        writeCalculations(calculations,csvPath);
        std::cout<<"Generated "<<csvPath.string()<<" ("<<calculations.size()<<" objects).\n";
        if (exportOnly) return 0;
        glfwSetErrorCallback([](int code,const char* message) { std::cerr<<"GLFW "<<code<<": "<<message<<'\n'; });
        if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW.");
        initialized=true;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        if (smoke) glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        window=glfwCreateWindow(1280,800,"3D Target Shooter | N: day/night | M: sound | Esc: menu",nullptr,nullptr);
        if (!window) throw std::runtime_error("Cannot create an OpenGL 3.3 window.");
        glfwMakeContextCurrent(window);
        if (!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) || !GLAD_GL_VERSION_3_3)
            throw std::runtime_error("OpenGL 3.3 is required.");
        glfwSwapInterval(smoke?0:1);
        std::cout<<"OpenGL: "<<glGetString(GL_VERSION)<<"\nRenderer: "<<glGetString(GL_RENDERER)<<'\n';
        runScene(window,root,game,csvPath,smoke,capture);
        glfwDestroyWindow(window); glfwTerminate(); return 0;
    } catch (const std::exception& error) {
        std::cerr<<"Error: "<<error.what()<<'\n';
        if (window) glfwDestroyWindow(window);
        if (initialized) glfwTerminate();
        return 1;
    }
}
