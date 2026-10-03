#include "persistence/CsvLogger.h"
#include "persistence/CsvFile.h"
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <set>
#include <chrono>
#include <ctime>

namespace shooter {
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
    const auto temporary=std::filesystem::path(path.wstring()+L".tmp");
    std::ofstream out(temporary, std::ios::trunc);
    if (!out) throw std::runtime_error("Cannot write calculations: " + path.string());
    out.imbue(std::locale::classic());
    out << "Phase,Object_ID,Object_Type,Component,Primitive,Local_Point,Scale,Shear,Rotation_X_deg,"
           "Rotation_Y_deg,Rotation_Z_deg,Translation,Matrix_Order,Matrix_or_Operation,"
           "Result_World_Point,Lighting_or_Use,Notes,Parent_or_Group,Purpose,Generated_UTC,Level,Mode,"
           "Color_RGB,Specular,Shininess,Emission,Target_Pattern,Snapshot_State,Observed_Time_seconds\n";
    const auto generated=utcTimestamp();
    out << std::fixed << std::setprecision(6);
    // One actual cube corner per drawn instance. Never copy example answers from calc-init.csv.
    for (const auto& object : objects) {
        const auto& t = object.transform;
        const Mat4 model = composeModelMatrix(t);
        const Vec4 local{.5f,.5f,.5f,1};
        const Vec4 point = transformPoint(model, local);
        const auto stages = modelTransformStages(t);
        const auto trace = traceTransformPoint(t, local);
        std::ostringstream shear, matrix;
        shear.imbue(std::locale::classic()); matrix.imbue(std::locale::classic());
        shear << std::fixed << std::setprecision(6) << '(';
        for (int i=0; i<6; ++i) { if (i) shear << ','; shear << t.shear[i]; }
        shear << ')';
        // Expose every intermediate point from the exact renderer matrix pipeline.
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
        out << "4," << csvQuote(object.id) << ',' << csvQuote(object.type) << ',' << csvQuote(object.component)
            << ",Unit Cube"
            << ',' << csvQuote(vectorText({local.x,local.y,local.z})) << ',' << csvQuote(vectorText(t.scale)) << ','
            << csvQuote(shear.str()) << ',' << t.rotation.x << ',' << t.rotation.y << ',' << t.rotation.z
            << ',' << csvQuote(vectorText(t.position)) << ',' << modelMatrixOrder << ','
            << csvQuote(matrix.str()) << ',' << csvQuote(vectorText({point.x,point.y,point.z}))
            << ',' << csvQuote("Ambient + Diffuse + Phong Specular; ks="+std::to_string(object.specular)+
                "; shininess="+std::to_string(object.shininess)+"; emission="+std::to_string(object.emission))
            << ',' << csvQuote(object.notes) << ',' << csvQuote(object.parent.empty()?object.type:object.parent)
            << ',' << csvQuote(object.component+"; actual scene cube corner mapping") << ',' << generated << ',' << object.level << ',' << csvQuote(object.mode)
            << ',' << csvQuote(vectorText(object.color)) << ',' << object.specular << ',' << object.shininess << ',' << object.emission
            << ',' << object.targetPattern << ',' << (object.retainedObservation?"Last observed":"Current") << ',' << object.observedTime << '\n';
    }
    out.close();
    if (!out) throw std::runtime_error("Failed to finish calculations: " + path.string());
    replaceFile(temporary,path);
}
void CsvLogger::observe(const std::vector<SceneObject>& objects, double time, bool night) {
    current=objects;
    for (auto& o:current) {
        o.observedTime=time;
        o.notes+="; observed at simulation time="+std::to_string(time)+" s; mode="+(night?"NIGHT":"DAY");
        // Bound history by component, not by projectile ID: unlimited play stays small.
        {
            const bool transient=(o.type=="Projectile"||o.type=="Hit effect"||o.type=="Blood") && o.id.find("DISPLAY_")==std::string::npos;
            const auto key=transient?o.mode+std::to_string(o.level)+o.type+o.component:o.id;
            observed[key]=o;
        }
    }
}
void CsvLogger::observeTransient(const std::vector<SceneObject>& objects,double time,bool night) {
    // Preserve short-lived effects and NPC death poses without copying every static crate each frame.
    for(const auto& source:objects) {
        const bool effect=source.type=="Projectile" || source.type=="Hit effect" || source.type=="Blood";
        if(!effect && source.type!="Weapon" && source.type!="Celebration" && source.type!="Bird" && source.type!="Human") continue;
        auto o=source; o.observedTime=time;
        o.notes+="; observed at simulation time="+std::to_string(time)+" s; mode="+(night?"NIGHT":"DAY");
        const auto key=effect&&o.id.find("DISPLAY_")==std::string::npos?o.mode+std::to_string(o.level)+o.type+o.component:o.id;
        observed[key]=std::move(o);
    }
}
std::vector<SceneObject> CsvLogger::snapshot() const {
    auto records=current;
    std::set<std::string> ids;
    for (auto& o:records) { ids.insert(o.id); o.notes+="; current scene snapshot"; }
    for (const auto& entry:observed) if (!ids.count(entry.second.id)) {
        auto o=entry.second;
        o.retainedObservation=true;
        o.notes+="; retained last actual observation from this run; no longer active";
        ids.insert(o.id);
        records.push_back(o);
    }
    return records;
}
void CsvLogger::save(const std::filesystem::path& path) const { writeCalculations(snapshot(),path); }
}
