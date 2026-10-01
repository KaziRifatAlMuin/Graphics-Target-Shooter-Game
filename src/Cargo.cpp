#include "Cargo.h"
#include <algorithm>
#include <random>
#include <stdexcept>

namespace shooter {
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
                objects.push_back(crate);
                for (int band:{-1,1}) {
                    const auto bandOffset=asVec3(transformPoint(rotation,{band<0?-size*.28f:0,0,0,0}));
                    const Vec3 bandSize=band<0?Vec3{size*.055f,size+.008f,size+.008f}:
                        Vec3{size+.008f,size*.055f,size+.008f};
                    objects.push_back(makeCube(id+"_BAND_"+std::to_string(band),"Cargo","Crate band",
                        crate.transform.position+bandOffset,bandSize,{.19f,.23f,.23f},yaw));
                }
            }
        };
        for (int i=0;i<run;++i) column(i-run/2,0);
        // L/T formations, with no duplicate column at the joint.
        if (style) for (int j=1;j<=2;++j) column(style==1?-run/2:0,j);
    }
    return objects;
}
}
