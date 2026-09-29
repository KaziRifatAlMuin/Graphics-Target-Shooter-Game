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
    // Four continuous 8 m walls. Supports decorate them without cutting openings.
    add("WALL_N","Boundary","North wall",{0,4,-100},{60,8,1},stone);
    add("WALL_W","Boundary","West wall",{-30,4,-50},{100,8,1},stone,90);
    add("WALL_E","Boundary","East wall",{30,4,-50},{100,8,1},stone,90);
    add("WALL_S","Boundary","South wall",{0,4,0},{60,8,1},stone);
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
        const auto stages = modelTransformStages(t);
        const auto trace = traceTransformPoint(t, {0.5f,0.5f,0.5f,1});
        std::ostringstream shear, matrix;
        shear.imbue(std::locale::classic()); matrix.imbue(std::locale::classic());
        shear << std::fixed << std::setprecision(6) << '(';
        for (int i=0; i<6; ++i) { if (i) shear << ','; shear << t.shear[i]; }
        shear << ')';
        // Keep calc-init.csv's 16 columns, while exposing every intermediate point.
        matrix << std::fixed << std::setprecision(6);
        for (std::size_t i=0; i<stages.size(); ++i) {
            const auto& p=trace[i+1];
            matrix << stages[i].name << ": " << vectorText({p.x,p.y,p.z}) << "; ";
        }
        matrix << "M_model rows: ";
        for (int row=0; row<4; ++row) {
            if (row) matrix << "; ";
            matrix << '[';
            for (int col=0; col<4; ++col) { if (col) matrix << ' '; matrix << model.at(row,col); }
            matrix << ']';
        }
        out << csvQuote(object.id) << ',' << csvQuote(object.type) << ',' << csvQuote(object.component)
            << ",Unit Cube,\"(0.5,0.5,0.5)\"," << csvQuote(vectorText(t.scale)) << ','
            << csvQuote(shear.str()) << ',' << t.rotation.x << ',' << t.rotation.y << ',' << t.rotation.z
            << ',' << csvQuote(vectorText(t.position)) << ',' << modelMatrixOrder << ','
            << csvQuote(matrix.str()) << ',' << csvQuote(vectorText({point.x,point.y,point.z}))
            << ",Flat face colors; transformation demonstration," << csvQuote(object.notes) << '\n';
    }
    out.close();
    if (!out) throw std::runtime_error("Failed to finish calculations: " + path.string());
}
}
