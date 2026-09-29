#include "Arena.h"
#include "Camera.h"
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>

using namespace shooter;
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
void near(float actual, float expected) {
    require(std::abs(actual-expected)<0.0001f,"Numeric result differs from the expected mapping.");
}
int main() {
    try {
        Transform t;
        t.scale={2,4,6}; t.shear[0]=0.5f;
        t.rotation={90,90,90}; t.position={10,20,30};
        const auto p=transformPoint(composeModelMatrix(t),{0.5f,0.5f,0.5f,1});
        // Independent hand calculation: (1,2,3) -> (2,2,3) -> (2,-3,2)
        // -> (2,-3,-2) -> (3,2,-2) -> (13,22,28).
        near(p.x,13); near(p.y,22); near(p.z,28); near(p.w,1);
        const auto sheared=transformPoint(makeShear(.1f,.2f,.3f,.4f,.5f,.6f),{1,2,3,1});
        near(sheared.x,1.8f); near(sheared.y,3.5f); near(sheared.z,4.7f);

        const Vec3 eye{3,5,7};
        const auto view=makeLookAt(eye,{3,5,6},{0,1,0});
        const auto origin=transformPoint(view,{3,5,7,1});
        near(origin.x,0); near(origin.y,0); near(origin.z,0);
        near(transformPoint(view,{3,5,6,1}).z,-1);
        const auto projection=makePerspective(60,1.6f,0.1f,400);
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
        const auto north=transformPoint(composeModelMatrix(scene[1].transform),{.5f,.5f,.5f,1});
        near(north.x,30); near(north.y,8); near(north.z,-99.5f);
        std::cout << "PASS: transform order, six shears, view/projection, camera, arena bounds and object IDs.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
