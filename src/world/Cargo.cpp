#include "world/Cargo.h"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <set>
#include "levels/LevelBase.h"

namespace shooter {
namespace {
// Add a crate plus bands and a label, rotating each detail's offset with its parent.
void addCrate(std::vector<SceneObject>& objects,SceneObject crate) {
    const auto id=crate.id;
    crate.parent=id; crate.specular=.09f;
    objects.push_back(crate);
    const float size=crate.transform.scale.x;
    auto detail=[&](const std::string& suffix,const std::string& name,Vec3 offset,Vec3 scale,Vec3 color) {
        auto part=makeCube(id+suffix,"Cargo",name,crate.transform.position+
            asVec3(transformPoint(makeRotationY(crate.transform.rotation.y),{offset.x,offset.y,offset.z,0})),
            scale,color,crate.transform.rotation.y);
        part.parent=id; part.notes=crate.notes; objects.push_back(part);
    };
    detail("_BAND_VERTICAL","Crate band",{-size*.28f,0,0},{size*.055f,size+.008f,size+.008f},{.19f,.23f,.23f});
    detail("_BAND_HORIZONTAL","Crate band",{0,0,0},{size+.008f,size*.055f,size+.008f},{.19f,.23f,.23f});
    detail("_LABEL","Crate inventory plate",{size*.15f,size*.19f,size*.5f+.006f},{size*.27f,size*.16f,.012f},{.74f,.70f,.49f});
}
}

// Add level-dependent cover and remove crates that overlap the targets' possible movement area.
std::vector<SceneObject> generateChallengeCargo(const LevelConfig& level) {
    auto objects=generateCargoLayout(level.seed,{dimensions::humanHeight,level.number<4?4:7});
    constexpr float size=dimensions::crateSize;
    if (level.number<4) return objects;
    // Short warehouse screens block spawn sightlines but leave both ends walkable.
    // Additional screens appear as the target population expands.
    const int screens=2*(level.number-3);
    for (int group=0;group<screens;++group) {
        const auto& t=level.targets[group];
        const float x=t.base.x*.75f,z=t.base.z+6;
        for (int column=0;column<7;++column) {
            const int height=column==0||column==6?4:5;
            for (int layer=0;layer<height;++layer) {
                const auto id="COVER_"+std::to_string(group)+"_"+std::to_string(column)+"_"+std::to_string(layer);
                auto box=makeCube(id,"Cargo","Crate",{x+(column-3)*(size+.005f),size*(layer+.5f),z},
                    {size,size,size},{.39f+.025f*(group%3),.34f,.24f});
                box.notes="Human reference height/3="+std::to_string(size)+" m; cargo screen; navigable ends; group="+std::to_string(group);
                addCrate(objects,box);
            }
        }
    }
    // Remove entire generated stack columns conflicting with a target's swept area.
    // Includes stands and safety margin; deterministic filtering preserves the seed.
    std::set<std::string> blocked;
    for (const auto& o:objects) if (o.component=="Crate") {
        for (const auto& t:level.targets)
            if (std::abs(o.transform.position.x-t.base.x)<t.motion.amplitude.x+1.5f &&
                std::abs(o.transform.position.z-t.base.z)<t.motion.amplitude.z+1.2f) blocked.insert(o.id);
    }
    objects.erase(std::remove_if(objects.begin(),objects.end(),[&](const SceneObject& o) {
        return blocked.count(o.parent)!=0;
    }),objects.end());
    return objects;
}
// Use a fixed random seed to reproduce crate stacks while preserving walking corridors.
std::vector<SceneObject> generateCargoLayout(unsigned seed, const CargoConfig& config) {
    if (config.humanHeight<1 || config.humanHeight>2.4f || config.rows<1 || config.rows>8)
        throw std::invalid_argument("Cargo configuration exceeds sandbox reserved zones.");
    std::mt19937 random(seed);
    std::vector<SceneObject> objects;
    const float size=config.humanHeight/3, spacing=size+.035f;
    const Vec3 colors[]={{.48f,.32f,.17f},{.36f,.42f,.30f},{.35f,.40f,.44f},{.56f,.43f,.28f}};
    int stack=0;
    // Fixed cells reserve central target corridors (|X| < 13 m), side walkways,
    // and cross aisles. Jitter stays inside each cell; clusters cannot join up.
    for (int row=0;row<config.rows;++row) for (float lane:{-20.0f,-15.0f,15.0f,20.0f}) {
        const Vec3 center{lane+(int(random()%7)-3)*.08f,0,-20-row*10.0f};
        const int run=3+random()%5;
        const float yaw=90.0f*(random()%4);
        const auto rotation=makeRotationY(yaw);
        const int style=random()%3;
        const Vec3 color=colors[random()%4];
        auto column=[&](int x,int z) {
            // Sum of small uniform draws favors middle heights; range is 1..5.
            const int levels=1+int(random()%3)+int(random()%3);
            const auto offset=asVec3(transformPoint(rotation,{x*spacing,0,z*spacing,0}));
            const std::string group="CARGO_"+std::to_string(stack++);
            for (int level=0;level<levels;++level) {
                const auto id=group+"_"+std::to_string(level);
                auto crate=makeCube(id,"Cargo","Crate",center+offset+Vec3{0,size*(level+.5f),0},
                    {size,size,size},color,yaw);
                crate.notes="Seed="+std::to_string(seed)+"; human height/3; stack="+group+"; reserved navigation corridors";
                addCrate(objects,crate);
            }
        };
        for (int i=0;i<run;++i) column(i-run/2,0);
        // L/T formations, with no duplicate column at the joint.
        if (style) for (int j=1;j<=2;++j) column(style==1?-run/2:0,j);
    }
    return objects;
}
}
