#include "../tools/Demonstration.h"
#include "gameplay/Game.h"
#include "gameplay/Effects.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>

namespace shooter::demo {
namespace {
// Identify a weapon family from its scene object ID for documentation grouping.
std::string weaponName(const SceneObject& o) {
    return o.id.find("PISTOL")!=std::string::npos?"pistol":o.id.find("SHOTGUN")!=std::string::npos?"shotgun":"rifle";
}
// Assign a stable example name and group from an object's type, component, and ID.
std::pair<std::string,std::string> identity(const SceneObject& o) {
    if(o.type=="Cargo") return {"cargo-crate",o.parent};
    if(o.type=="Weapon") return {"weapon-"+weaponName(o),o.parent+(o.id.find("DISPLAY_")!=std::string::npos?"-display":"-held")};
    if(o.type=="Target" || o.type=="Bird" || o.type=="Human") return {slug(o.type),o.parent};
    if(o.type=="Player" || o.type=="Camera reference") return {slug(o.type),o.type};
    if(o.type=="Lighting") {
        auto p=o.id.find("LAMP_"); bool lamp=p!=std::string::npos;
        if(!lamp) p=o.id.find("FLOOD_");
        if(p==std::string::npos) throw std::runtime_error("Unclassified fixture: "+o.id);
        const auto end=o.id.find('_',p+(lamp?5:6)); const auto id=o.id.substr(0,end);
        const int n=std::stoi(o.id.substr(p+(lamp?5:6)));
        return {lamp?(n<=2?"lamp-post":"wall-lamp"):"stadium-floodlight",id};
    }
    if(o.type=="Environment" && o.id.find("EQUIPMENT_")!=std::string::npos) return {"equipment-table","table"};
    return {slug(o.type+"-"+o.component),o.id};
}
// List the implementation files explaining this example's geometry and behavior.
std::vector<std::string> sources(const std::string& name,const Example& ex) {
    const auto& type=ex.parts.front().type;
    if(type=="Boundary" || type=="Arena") return {"src/world/Arena.cpp"};
    if(type=="Cargo") return {"src/world/Cargo.cpp","src/world/Dimensions.h"};
    if(type=="Target") return {"src/gameplay/Target.cpp","src/gameplay/Target.h","src/gameplay/Movement.cpp"};
    if(type=="Weapon") return {"src/gameplay/Weapon.cpp"};
    if(type=="Bird" || type=="Human") return {"src/gameplay/"+type+".cpp","src/gameplay/Npc.cpp"};
    if(type=="Lighting") return {"src/world/Lighting.cpp"};
    if(type=="Environment") return {"src/world/Environment.cpp"};
    if(type=="Camera reference") return {"src/camera/BirdEyeCamera.cpp"};
    if(type=="Celebration") return {"src/gameplay/Effects.cpp"};
    if(name.find("projectile")!=std::string::npos) return {"src/gameplay/Projectile.cpp","src/gameplay/Weapon.cpp"};
    return {"src/gameplay/GameScene.cpp","src/gameplay/Game.cpp"};
}
// Document a cube's transform stages, intermediate coordinates, and matching rendered images.
void describePart(Capture& capture,std::ostream& out,const fs::path& folder,const SceneObject& o,const std::string& stem) {
    const auto& t=o.transform;
    out<<"\n### "<<o.component<<" — `"<<o.id<<"`\n\n"<<o.notes<<"\n\n"
       <<"Position T = "<<vector(t.position)<<" m; scale S = "<<vector(t.scale)<<"; rotation (Rx,Ry,Rz) = "<<vector(t.rotation)<<" degrees.\n\n"
       <<"Shear (xy,xz,yx,yz,zx,zy) = (";
    for(int i=0;i<6;++i) out<<(i?", ":"")<<number(t.shear[i]);
    out<<"). Color = "<<vector(o.color)<<"; specular = "<<number(o.specular)<<"; shininess = "<<number(o.shininess)
       <<"; emission = "<<number(o.emission)<<"; flash = "<<number(o.flash)<<"; material layer = "<<int(o.material)
       <<"; texture scale = "<<number(o.textureScale)<<". Pattern enabled = "<<o.targetPattern
       <<", pattern scale = "<<vector(o.patternScale)<<", offset = "<<vector(o.patternOffset)<<".\n\n";
    std::vector<SceneObject> stages;
    for(int stage=0;stage<7;++stage) {
        auto p=o; p.transform=Transform{};
        if(stage>=1) p.transform.scale=t.scale;
        if(stage>=2) p.transform.shear=t.shear;
        if(stage>=3) p.transform.rotation.x=t.rotation.x;
        if(stage>=4) p.transform.rotation.y=t.rotation.y;
        if(stage>=5) p.transform.rotation.z=t.rotation.z;
        if(stage>=6) p.transform.position=t.position;
        stages.push_back(p);
    }
    const auto localView=fit(std::vector<SceneObject>(stages.begin(),stages.begin()+6));
    const char* names[]={"Unit cube","Scale","Shear","Rotate X","Rotate Y","Rotate Z","Translate to world"};
    out<<"| Step | Actual renderer image |\n| --- | --- |\n";
    for(int i=0;i<7;++i) {
        const auto image=stem+"-"+std::to_string(i)+".png";
        capture.save(folder/image,{stages[i]},i==6?fit({o}):i==0?fit({stages[0]}):localView);
        out<<"| "<<names[i]<<" | !["<<names[i]<<"]("<<image<<") |\n";
    }
    out<<"\nSteps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. "
          "Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. "
          "Intermediate shapes are instructional states, while step 6 uses the unmodified game object.\n\n"
          "Each stage below gives its exact numeric matrix and all eight corner multiplications. "
          "For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.\n";
    // Signed coordinate diagram makes translation visible even though the final
    // OpenGL close-up tracks the object (otherwise small objects become subpixel).
    const auto diagram=stem+"-translation.svg";
    auto svg=document(folder/diagram);
    svg<<"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"240\" viewBox=\"0 0 800 240\">"
          "<rect width=\"800\" height=\"240\" fill=\"#102332\"/><g font-family=\"monospace\" font-size=\"15\" fill=\"white\">"
          "<text x=\"20\" y=\"28\">Translation: center (0,0,0) to "<<vector(t.position)<<" m</text>";
    const float extent=std::max({std::abs(t.position.x),std::abs(t.position.y),std::abs(t.position.z),.001f});
    const float values[]={t.position.x,t.position.y,t.position.z};const char* colors[]={"#ff937f","#85eb9b","#8bbaff"};
    for(int axis=0;axis<3;++axis) {
        const int y=72+axis*58;const float x=380+values[axis]/extent*260;
        svg<<"<line x1=\"120\" y1=\""<<y<<"\" x2=\"640\" y2=\""<<y<<"\" stroke=\"#596878\"/>"
           <<"<line x1=\"380\" y1=\""<<y-10<<"\" x2=\"380\" y2=\""<<y+10<<"\" stroke=\"white\"/>"
           <<"<line x1=\"380\" y1=\""<<y<<"\" x2=\""<<x<<"\" y2=\""<<y<<"\" stroke=\""<<colors[axis]<<"\" stroke-width=\"5\"/>"
           <<"<circle cx=\""<<x<<"\" cy=\""<<y<<"\" r=\"6\" fill=\""<<colors[axis]<<"\"/>"
           <<"<text x=\"20\" y=\""<<y+5<<"\">"<<char('X'+axis)<<"</text><text x=\"650\" y=\""<<y+5<<"\">"<<number(values[axis])<<" m</text>";
    }
    svg<<"<text x=\"240\" y=\"232\">White ticks = 0; all axes share one scale</text></g></svg>";
    out<<"\n![Signed translation components]("<<diagram<<")\n\n";
    const auto operations=modelTransformStages(t);
    for(int s=0;s<6;++s) {
        out<<"\n#### "<<operations[s].name<<"\n\n"; matrix(out,operations[s].matrix);
        out<<"\n| Input corner (w=1) | Output corner (w=1) |\n| --- | --- |\n";
        for(float x:{-.5f,.5f}) for(float y:{-.5f,.5f}) for(float z:{-.5f,.5f}) {
            const auto trace=traceTransformPoint(t,{x,y,z,1});
            out<<"| "<<vector(asVec3(trace[s]))<<" | "<<vector(asVec3(trace[s+1]))<<" |\n";
            const auto direct=transformPoint(composeModelMatrix(t),{x,y,z,1});
            if(length(asVec3(direct)-asVec3(trace[6]))>1e-3f) throw std::runtime_error("Transform trace mismatch: "+o.id);
        }
    }
    out<<"\nFinal M = T Rz Ry Rx H S:\n\n"; matrix(out,composeModelMatrix(t));
}
}
// Collect real scene assemblies and alternate states, then generate their illustrated construction guides.
Catalogue demonstrateObjects(Capture& capture,const fs::path& root) {
    Catalogue examples;
    std::map<std::string,std::string> covered;
    auto scan=[&](const std::vector<SceneObject>& scene,const std::string& provenance) {
        std::map<std::pair<std::string,std::string>,std::vector<SceneObject>> instances;
        // Preserve scene order when selecting one instance of a repeated type.
        std::vector<std::pair<std::string,std::string>> order;
        for(const auto& o:scene) {
            const auto key=identity(o); if(!instances.count(key)) order.push_back(key);
            instances[key].push_back(o); covered[o.type+" / "+o.component]=key.first;
        }
        for(const auto& key:order) if(!examples.count(key.first)) examples[key.first]={instances[key],provenance,{}};
    };
    Game practice; scan(practice.scene(true),"Practice startup, simulation time 0; cargo seed 2107042. First encountered instance per assembly type.");
    for(int level=1;level<=7;++level) {
        Game game; game.startMode(GameMode::Developer,level);
        scan(game.scene(true),"Developer level "+std::to_string(level)+", time 0; level seed "+std::to_string(game.levels.config.seed)+"; original seeded NPC values.");
        auto target=game.targets.front(); updateTarget(target,1,0);
        examples.at("target").states["level-"+std::to_string(level)+"-time-1"]=createTargetObjects(target,0);
    }
    Game overhead; overhead.startMode(GameMode::BirdsEye); scan(overhead.birdEye.referenceObjects(),"Bird's-Eye reset observation camera.");
    for(auto type:{WeaponType::Pistol,WeaponType::Shotgun,WeaponType::Rifle}) {
        const auto parts=createWeapon(type,practice.player,1);
        auto& ex=examples.at("weapon-"+weaponName(parts.front()));
        ex.states["held-startup"]=createWeapon(type,practice.player,0);
        ex.states["fired-recoil-1"]=parts;
    }
    Game transient; transient.startMode(GameMode::Free); transient.levels.stage=LevelStage::Active;
    const auto bird=transient.birds.front(); const auto human=transient.humans.front();
    transient.applyNpcHit(false,0,1); transient.applyNpcHit(true,0,2);
    transient.update(.25f);
    examples.at("bird").states["falling-time-0-25"]=createBirdObjects(transient.birds.front(),0);
    examples.at("human").states["falling-time-0-25"]=createHumanObjects(transient.humans.front(),0);
    transient.update(.75f);
    examples.at("human").states["fallen-time-1"]=createHumanObjects(transient.humans.front(),0);
    transient.update(1);
    examples.at("bird").states["fallen-time-2"]=createBirdObjects(transient.birds.front(),0);
    examples.at("bird").states["free-level-7-alive"]=createBirdObjects(bird,0);
    examples.at("human").states["free-level-7-alive"]=createHumanObjects(human,0);
    // First process-local blood RNG seed is 133742. Actual game hit code produced these values.
    Game effects; effects.applyTargetHit(0,0,10); effects.spawnBlood({0,1.7f,-5},1); effects.update(.1f);
    std::vector<SceneObject> particles;
    for(const auto& o:effects.scene(true)) if(o.type=="Blood" || o.type=="Hit effect") particles.push_back(o);
    scan(particles,"Real Game::applyTargetHit and spawnBlood; fixed call order; update(0.1), integration step <= 1/120 s. Blood RNG follows the two NPC hits above.");
    effects.update(.5f);
    for(const auto& o:effects.scene(true)) if(o.type=="Blood") {
        examples.at(identity(o).first).states["time-0-6"]={o}; break;
    }
    auto warning=overhead.targets.front(); warning.respawn=2; warning.eliminated=false;
    examples.at("target").states["respawn-countdown-2"]=createTargetObjects(warning,0);
    auto hit=practice.targets.front(); hit.hitTime=.25f;
    examples.at("target").states["hit-flash-0-25"]=createTargetObjects(hit,0);
    const auto confetti=celebrationObjects(1,true);
    scan({confetti.front()},"celebrationObjects(time=1 second, victory=true), particle index 0.");
    // Ensure the catalogue also tracks components that appear only in alternate states.
    for(const auto& [name,ex]:examples) for(const auto& [state,parts]:ex.states)
        for(const auto& o:parts) covered[o.type+" / "+o.component]=name;
    auto index=document(root/"objects"/"README.md");
    index<<"# Generated object demonstrations\n\nRun `mingw32-make`, `build.bat`, or build CMake target `main`. "
           "The demonstration generator runs on every build, including when compilation is up to date. "
           "Manual regeneration: `demonstrateObject.exe <project-root>`. Requires an OpenGL 3.3 context.\n\n"
           "One directory per unique assembly/component type; repeated instances are not duplicated. "
           "All seven levels and Practice are inspected. Alternate poses and temporary states share their type's directory. "
           "Values come from actual builders; fixed seeds, call order and simulation times make examples repeatable on the same toolchain/GPU. "
           "GPU rasterization may differ slightly across drivers. All PNGs are real lossless renderer captures, 480 x 360. "
           "No boolean cube union exists: assemblies are overlapping independent cube draw calls. "
           "UI text/menus and the fullscreen procedural sky are not SceneObjects; sky is documented in lighting.\n\n"
           "| Object type | Parts in representative | Guide |\n| --- | ---: | --- |\n";
    for(const auto& [name,ex]:examples) {
        std::cout<<"Demonstrating "<<name<<" ("<<ex.parts.size()<<" parts)\n";
        const auto folder=root/"objects"/name; auto out=document(folder/(name+".md"));
        index<<"| "<<name<<" | "<<ex.parts.size()<<" | [Open]("<<name<<"/"<<name<<".md) |\n";
        out<<"# "<<name<<"\n\n"<<ex.provenance<<"\n\n"
             "All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). "
             "The renderer uses column vectors and column-major matrix storage. The mathematical application order is:\n\n"
             "$$p_w=T R_z R_y R_x H S p_l,\\qquad p_{clip}=P V p_w.$$\n\n"
             "Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). "
             "For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); "
             "Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). "
             "Translation adds (tx,ty,tz). Each stage uses the previous stage's output. "
             "No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. "
             "Source listings at the end give exact construction, offset, animation and random-value formulas.\n\n";
        const auto view=fit(ex.parts);
        out<<"Assembly view eye="<<vector(view.eye)<<", center="<<vector(view.center)
           <<", FOV=45 degrees, aspect=4/3, near="<<number(view.nearPlane)<<", far="<<number(view.farPlane)<<".\n\n";
        capture.save(folder/"complete.png",ex.parts,view);
        capture.save(folder/"reverse.png",ex.parts,fit(ex.parts,{-1,.65f,-1}));
        out<<"![Complete assembly](complete.png)\n\n![Opposite view of the same assembly](reverse.png)\n\n## Cube assembly order\n\nEach image adds one real component, preserving builder order and world transforms.\n\n";
        std::vector<SceneObject> assembly;
        for(std::size_t i=0;i<ex.parts.size();++i) {
            assembly.push_back(ex.parts[i]); const auto file="assembly-"+std::to_string(i+1)+".png";
            capture.save(folder/file,assembly,view);
            out<<"### Add "<<i+1<<": "<<ex.parts[i].component<<"\n\n![Assembly "<<i+1<<"]("<<file<<")\n\n";
        }
        out<<"## Every component transformation\n";
        for(std::size_t i=0;i<ex.parts.size();++i) describePart(capture,out,folder,ex.parts[i],"part-"+std::to_string(i));
        for(const auto& [state,parts]:ex.states) {
            out<<"\n## Alternate state: "<<state<<"\n\nGenerated with the shared game builder at the named fixed time/condition. "
                  "These are separate snapshots, not additional parts attached to the representative.\n\n";
            const auto file=state+".png"; capture.save(folder/file,parts,fit(parts)); out<<"!["<<state<<"]("<<file<<")\n";
            for(std::size_t i=0;i<parts.size();++i) describePart(capture,out,folder,parts[i],state+"-part-"+std::to_string(i));
        }
        out<<"\n## Exact construction implementation\n\nThe following files are copied from this build's source, not hand-maintained snippets.\n";
        for(const auto& file:sources(name,ex)) source(out,root,file);
        source(out,root,"src/core/Transform.h");
    }
    index<<"\n## Component coverage\n\n| Observed game type / component | Demonstration directory |\n| --- | --- |\n";
    for(const auto& [component,name]:covered) {
        bool found=false; const auto& ex=examples.at(name);
        auto check=[&](const auto& parts) {for(const auto& o:parts) if(o.type+" / "+o.component==component) found=true;};
        check(ex.parts); for(const auto& state:ex.states) check(state.second);
        if(!found) throw std::runtime_error("Missing representative component: "+component);
        index<<"| "<<component<<" | ["<<name<<"]("<<name<<"/"<<name<<".md) |\n";
    }
    return examples;
}
}
// Initialize hidden OpenGL rendering and generate object and lighting demonstrations from the project.
int main(int argc,char** argv) {
    using namespace shooter::demo;
    GLFWwindow* window=nullptr;
    try {
        if(argc!=2) throw std::runtime_error("Usage: demonstrateObject PROJECT_ROOT");
        const auto root=fs::absolute(argv[1]);
        if(!fs::exists(root/"src/core/Transform.h")) throw std::runtime_error("Expected source checkout root");
        if(!glfwInit()) throw std::runtime_error("Demonstrations require OpenGL 3.3: GLFW initialization failed");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_VISIBLE,GLFW_FALSE);
        window=glfwCreateWindow(480,360,"Game demonstrations",nullptr,nullptr);
        if(!window) throw std::runtime_error("Cannot create hidden demonstration context");
        glfwMakeContextCurrent(window);glfwSwapInterval(0);
        if(!gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress))) throw std::runtime_error("Cannot load OpenGL");
        {
            Capture capture(root); const auto examples=demonstrateObjects(capture,root);
            demonstrateLighting(capture,root,examples);
            std::cout<<"Generated "<<examples.size()<<" object guides and lighting guide; "<<capture.images<<" PNG images.\n";
        }
        glfwDestroyWindow(window);glfwTerminate();return 0;
    } catch(const std::exception& e) {
        std::cerr<<"Demonstration generation failed: "<<e.what()<<'\n';
        if(window) glfwDestroyWindow(window);
        glfwTerminate();return 1;
    }
}
