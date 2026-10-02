#include "world/Arena.h"

namespace shooter {
std::vector<SceneObject> createArena() {
    std::vector<SceneObject> objects;
    const Vec3 stone{0.49f,0.55f,0.61f}, top{0.62f,0.68f,0.72f};
    auto add = [&](std::string id, std::string type, std::string component,
                   Vec3 position, Vec3 scale, Vec3 color, float yaw = 0,
                   std::string notes = "Fortified boundary; 1 unit = 1 m") {
        Transform t;
        t.position=position; t.scale=scale; t.rotation.y=yaw;
        objects.push_back({id,type,component,t,color,notes});
    };
    add("ARENA_FLOOR","Arena","Floor slab",{0,-0.1f,-50},{60,0.2f,100},
        {0.29f,0.36f,0.30f},0,"60 x 100 m; top face at Y = 0; forward = -Z");
    for (int lane:{-1,1}) for (int z=-8;z>=-94;z-=4)
        add("AISLE_MARK_"+std::to_string(lane)+"_"+std::to_string(-z),"Arena","Painted route marker",
            {lane*2.8f,.012f,float(z)},{.075f,.018f,1.35f},{.74f,.66f,.35f},0,"Thin cube paint strip; navigable central aisle");
    // Four continuous 8 m walls. Supports decorate them without cutting openings.
    add("WALL_N","Boundary","North wall",{0,4,-100},{60,8,1},stone);
    add("WALL_W","Boundary","West wall",{-30,4,-50},{100,8,1},stone,90);
    add("WALL_E","Boundary","East wall",{30,4,-50},{100,8,1},stone,90);
    add("WALL_S","Boundary","South wall",{0,4,0},{60,8,1},stone);
    for (int course=1;course<=3;++course) {
        for (int side:{-1,1})
            add("STONE_COURSE_"+std::to_string(side)+"_"+std::to_string(course),"Boundary","Masonry string course",
                {side*29.44f,float(course)*2,-50},{.14f,.13f,99},{.37f,.41f,.46f});
        for (int end:{0,-100})
            add("END_COURSE_"+std::to_string(end)+"_"+std::to_string(course),"Boundary","Masonry string course",
                {0,float(course)*2,end==0?-.56f:-99.44f},{59,.13f,.14f},{.37f,.41f,.46f});
    }
    int block = 0;
    for (int x=-27; x<=27; x+=6) {
        add("BATTLEMENT_"+std::to_string(++block),"Boundary","North top block",
            {float(x),8.75f,-100},{2,1.5f,1.5f},top);
        add("BATTLEMENT_"+std::to_string(++block),"Boundary","South top block",
            {float(x),8.75f,0},{2,1.5f,1.5f},top);
    }
    for (int z=-6; z>=-94; z-=6) {
        for (int side : {-1,1})
            add("BATTLEMENT_"+std::to_string(++block),"Boundary","Side top block",
                {side*30.0f,8.75f,float(z)},{2,1.5f,1.5f},top,90);
    }
    int corner = 0;
    for (int x : {-30,30}) for (int z : {-100,0}) {
        const auto suffix = std::to_string(++corner);
        add("CORNER_"+suffix,"Boundary","Thick corner tower",
            {float(x),4.75f,float(z)},{3.5f,9.5f,3.5f},stone);
        add("CORNER_CAP_"+suffix,"Boundary","Corner cap",
            {float(x),10,float(z)},{4.2f,1,4.2f},top);
    }
    // Leaning stone supports demonstrate non-identity shear and all rotation axes.
    // The enclosing walls above stay vertical and continuous on all four sides.
    auto support = [&](const std::string& id, Vec3 position, Vec3 rotation) {
        Transform t;
        t.position=position;
        t.scale={1.5f,6,2};
        t.shear[0]=0.22f; // x' = x + 0.22*y, applied after scaling.
        t.rotation=rotation;
        // Anchor the lowest transformed cube corner exactly to ground level.
        t.position.y=0;
        float lowest=0;
        const Mat4 model=composeModelMatrix(t);
        for (float x : {-0.5f,0.5f}) for (float y : {-0.5f,0.5f}) for (float z : {-0.5f,0.5f}) {
            const float height=transformPoint(model,{x,y,z,1}).y;
            if (height<lowest) lowest=height;
        }
        t.position.y=-lowest;
        objects.push_back({id,"Boundary","Sheared stone support",t,{0.66f,0.59f,0.46f},
            "Scale -> XY shear -> Rx -> Ry -> Rz -> translation; lowest corner at Y = 0"});
    };
    for (int z : {-18,-42,-66,-90}) {
        support("SUPPORT_E_"+std::to_string(-z),{28,0,float(z)},{0,0,-8});
        support("SUPPORT_W_"+std::to_string(-z),{-28,0,float(z)},{0,180,8});
    }
    for (int x : {-20,-10,0,10,20}) {
        support("SUPPORT_N_"+std::to_string(x),{float(x),0,-98},{-8,90,0});
        support("SUPPORT_S_"+std::to_string(x),{float(x),0,-2},{8,-90,0});
    }
    return objects;
}
}
