#include "gameplay/Effects.h"
namespace shooter {
// Create short-lived decorative confetti cubes without affecting scoring or collisions.
std::vector<SceneObject> celebrationObjects(float time,bool victory) {
    std::vector<SceneObject> cubes;
    if (time<0 || time>4) return cubes;
    const Vec3 colors[]={{.12f,.85f,.68f},{1,.67f,.16f},{.26f,.54f,1},{.98f,.90f,.61f}};
    for (int i=0;i<(victory?84:28);++i) {
        const float delay=(i%7)*.045f,age=time-delay;
        if(age<0) continue;
        // Use roughly the golden angle (2.39996 radians) to spread successive pieces in different directions.
        const float angle=i*2.39996f,speed=5+(i%5)*1.4f;
        const Vec3 origin{0,22,-26};
        // Ballistic path: p=p0+v*t+(0,-2.5*t^2,0), equivalent to gravity g=5 m/s^2.
        const Vec3 p=origin+Vec3{std::cos(angle)*speed*age,(5+(i%4))*age-2.5f*age*age,std::sin(angle)*speed*age};
        auto cube=makeCube("CELEBRATION_"+std::to_string(i),"Celebration","Victory confetti",p,
            {.24f,.14f,.32f},colors[i%4]);
        cube.transform.rotation={age*(90+i),age*(120-i),age*150}; cube.emission=.65f;
        cube.notes="Cosmetic transformed cube; ballistic celebration time="+std::to_string(time)+" s; no scoring or collision";
        cubes.push_back(cube);
    }
    return cubes;
}
}
