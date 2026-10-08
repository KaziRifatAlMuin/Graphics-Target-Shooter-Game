#include "world/Arena.h"
#include "camera/Camera.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>

using namespace shooter;
// Fail the test immediately with a readable message when an expected condition is false.
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
// Allow floating-point rounding error: require |actual-expected| < 0.0001.
void near(float actual, float expected) {
    require(std::abs(actual-expected)<0.0001f,"Numeric result differs from the expected mapping.");
}
#include "game_tests.h"
// Check known transform results, camera projection, and arena geometry before running gameplay tests.
int main() {
    try {
        Transform t;
        t.scale={2,4,6}; t.shear[0]=0.5f;
        t.rotation={90,90,90}; t.position={10,20,30};
        const auto p=transformPoint(composeModelMatrix(t),{0.5f,0.5f,0.5f,1});
        // Independent hand calculation: (1,2,3) -> (2,2,3) -> (2,-3,2)
        // -> (2,-3,-2) -> (3,2,-2) -> (13,22,28).
        near(p.x,13); near(p.y,22); near(p.z,28); near(p.w,1);
        const auto trace=traceTransformPoint(t,{0.5f,0.5f,0.5f,1});
        const Vec3 expectedTrace[]={{.5f,.5f,.5f},{1,2,3},{2,2,3},{2,-3,2},
                                    {2,-3,-2},{3,2,-2},{13,22,28}};
        for (std::size_t i=0; i<trace.size(); ++i) {
            near(trace[i].x,expectedTrace[i].x); near(trace[i].y,expectedTrace[i].y);
            near(trace[i].z,expectedTrace[i].z); near(trace[i].w,1);
        }
        const auto sheared=transformPoint(makeShear(.1f,.2f,.3f,.4f,.5f,.6f),{1,2,3,1});
        near(sheared.x,1.8f); near(sheared.y,3.5f); near(sheared.z,4.7f);

        const Vec3 eye{3,5,7};
        const auto view=makeLookAt(eye,{3,5,6},{0,1,0});
        const auto origin=transformPoint(view,{3,5,7,1});
        near(origin.x,0); near(origin.y,0); near(origin.z,0);
        near(transformPoint(view,{3,5,6,1}).z,-1);
        const auto projection=makePerspective(60,1.6f,0.1f,400);
        // After perspective division, OpenGL maps the near plane to z=-1 and the far plane to z=+1.
        const auto nearClip=transformPoint(projection,{0,0,-0.1f,1});
        const auto farClip=transformPoint(projection,{0,0,-400,1});
        near(nearClip.z/nearClip.w,-1); near(farClip.z/farClip.w,1);

        Camera camera;
        const auto start=camera.position;
        camera.move(1,1,1,1,false);
        const auto moved=camera.position-start;
        near(std::sqrt(dot(moved,moved)),12);
        camera.look(0,100000); near(camera.pitch,-89);
        camera.look(0,-100000); near(camera.pitch,89);

        const auto scene=createArena();
        std::set<std::string> ids;
        for (const auto& object : scene) require(ids.insert(object.id).second,"Duplicate object ID.");
        require(scene.size()>50,"Boundary is missing repeated cube components.");
        const auto floor=transformPoint(composeModelMatrix(scene.front().transform),{.5f,.5f,.5f,1});
        near(floor.x,30); near(floor.y,0); near(floor.z,0);
        const auto floorOther=transformPoint(composeModelMatrix(scene.front().transform),{-.5f,.5f,-.5f,1});
        near(floorOther.x,-30); near(floorOther.y,0); near(floorOther.z,-100);
        const auto northWall=std::find_if(scene.begin(),scene.end(),[](const SceneObject& o) { return o.id=="WALL_N"; });
        require(northWall!=scene.end(),"North boundary is missing.");
        const auto north=transformPoint(composeModelMatrix(northWall->transform),{.5f,.5f,.5f,1});
        near(north.x,30); near(north.y,8); near(north.z,-99.5f);
        struct Bounds { Vec3 min, max; };
        // Transform all eight cube corners to independently measure world-space minimum and maximum coordinates.
        auto bounds=[](const SceneObject& object) {
            Bounds b{{1e6f,1e6f,1e6f},{-1e6f,-1e6f,-1e6f}};
            for (float x : {-0.5f,0.5f}) for (float y : {-0.5f,0.5f}) for (float z : {-0.5f,0.5f}) {
                const auto p=transformPoint(composeModelMatrix(object.transform),{x,y,z,1});
                b.min={std::min(b.min.x,p.x),std::min(b.min.y,p.y),std::min(b.min.z,p.z)};
                b.max={std::max(b.max.x,p.x),std::max(b.max.y,p.y),std::max(b.max.z,p.z)};
                const auto steps=traceTransformPoint(object.transform,{x,y,z,1});
                near(steps.back().x,p.x); near(steps.back().y,p.y); near(steps.back().z,p.z);
            }
            return b;
        };
        auto findObject=[&](const char* id) -> const SceneObject& {
            const auto found=std::find_if(scene.begin(),scene.end(),[&](const SceneObject& o) { return o.id==id; });
            require(found!=scene.end(),"Missing continuous boundary wall.");
            return *found;
        };
        // Each side must be a solid cube spanning the entire floor edge from Y=0 to Y=8.
        for (const char* id : {"WALL_N","WALL_S"}) {
            const auto b=bounds(findObject(id));
            near(b.min.x,-30); near(b.max.x,30); near(b.min.y,0); near(b.max.y,8);
            const float center=std::string(id)=="WALL_N"?-100.0f:0.0f;
            near(b.min.z,center-.5f); near(b.max.z,center+.5f);
        }
        for (const char* id : {"WALL_W","WALL_E"}) {
            const auto b=bounds(findObject(id));
            near(b.min.z,-100); near(b.max.z,0); near(b.min.y,0); near(b.max.y,8);
            const float center=std::string(id)=="WALL_W"?-30.0f:30.0f;
            near(b.min.x,center-.5f); near(b.max.x,center+.5f);
        }
        bool shearVisible=false, rotateX=false, rotateY=false, rotateZ=false;
        for (const auto& object : scene) {
            const auto b=bounds(object);
            if (object.component=="Sheared stone support") {
                near(b.min.y,0);
                shearVisible |= object.transform.shear[0]!=0;
                rotateX |= object.transform.rotation.x!=0;
                rotateY |= object.transform.rotation.y!=0;
                rotateZ |= object.transform.rotation.z!=0;
            }
        }
        require(shearVisible && rotateX && rotateY && rotateZ,"Scene must demonstrate shear and all rotation axes.");
        std::cout << "PASS: transformation stages, six shears, view/projection, camera, closed boundary, grounded supports and object IDs.\n";
        gameTests();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
