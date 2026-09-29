#include "Arena.h"
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>

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
    // Four 8 m walls, with a 10 m entrance in the south wall.
    add("WALL_N","Boundary","North wall",{0,4,-100},{60,8,1},stone);
    add("WALL_W","Boundary","West wall",{-30,4,-50},{100,8,1},stone,90);
    add("WALL_E","Boundary","East wall",{30,4,-50},{100,8,1},stone,90);
    add("WALL_S_W","Boundary","South wall left",{-17.5f,4,0},{25,8,1},stone);
    add("WALL_S_E","Boundary","South wall right",{17.5f,4,0},{25,8,1},stone);
    add("GATE_LINTEL","Boundary","Entrance lintel",{0,7,0},{10,2,2},top);
    int block = 0;
    for (int x=-27; x<=27; x+=6) {
        add("BATTLEMENT_"+std::to_string(++block),"Boundary","North top block",
            {float(x),8.75f,-100},{2,1.5f,1.5f},top);
        if (x < -5 || x > 5)
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
    return objects;
}

namespace {
std::string vectorText(Vec3 v) {
    std::ostringstream s;
    s.imbue(std::locale::classic());
    s << std::fixed << std::setprecision(6) << '(' << v.x << ',' << v.y << ',' << v.z << ')';
    return s.str();
}
std::string csvQuote(const std::string& value) {
    std::string result = "\"";
    for (char c : value) { if (c == '"') result += '"'; result += c; }
    return result + '"';
}
}
void writeCalculations(const std::vector<SceneObject>& objects, const std::filesystem::path& path) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) throw std::runtime_error("Cannot write calculations: " + path.string());
    out.imbue(std::locale::classic());
    out << "Object_ID,Object_Type,Component,Primitive,Local_Point,Scale,Shear,Rotation_X_deg,"
           "Rotation_Y_deg,Rotation_Z_deg,Translation,Matrix_Order,Matrix_or_Operation,"
           "Result_World_Point,Lighting_or_Use,Notes\n";
    out << std::fixed << std::setprecision(6);
    // One actual cube corner per drawn instance. Never copy example answers from calc-init.csv.
    for (const auto& object : objects) {
        const auto& t = object.transform;
        const Mat4 model = composeModelMatrix(t);
        const Vec4 point = transformPoint(model, {0.5f,0.5f,0.5f,1});
        std::ostringstream shear, matrix;
        shear.imbue(std::locale::classic()); matrix.imbue(std::locale::classic());
        shear << std::fixed << std::setprecision(6) << '(';
        for (int i=0; i<6; ++i) { if (i) shear << ','; shear << t.shear[i]; }
        shear << ')';
        matrix << std::fixed << std::setprecision(6) << "M_model rows: ";
        for (int row=0; row<4; ++row) {
            if (row) matrix << "; ";
            matrix << '[';
            for (int col=0; col<4; ++col) { if (col) matrix << ' '; matrix << model.at(row,col); }
            matrix << ']';
        }
        out << csvQuote(object.id) << ',' << csvQuote(object.type) << ',' << csvQuote(object.component)
            << ",Unit Cube,\"(0.5,0.5,0.5)\"," << csvQuote(vectorText(t.scale)) << ','
            << csvQuote(shear.str()) << ',' << t.rotation.x << ',' << t.rotation.y << ',' << t.rotation.z
            << ',' << csvQuote(vectorText(t.position)) << ",T*Rz*Ry*Rx*H*S,"
            << csvQuote(matrix.str()) << ',' << csvQuote(vectorText({point.x,point.y,point.z}))
            << ",Flat face colors (Phase 1)," << csvQuote(object.notes) << '\n';
    }
    out.close();
    if (!out) throw std::runtime_error("Failed to finish calculations: " + path.string());
}
}
