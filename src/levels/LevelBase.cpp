#include "LevelBase.h"
namespace shooter {
// Set base position and sinusoidal motion; variation changes phase/direction while spin uses degrees/second.
Target configuredTarget(Vec3 p, Vec3 amplitude, float speed, float spin, int variation) {
    Target t; t.base=t.position=p; t.movement=4;
    t.motion.amplitude=amplitude;
    const float direction=variation%2?-1.0f:1.0f;
    t.motion.frequency={speed*direction,speed*1.13f,speed*.87f*direction};
    t.motion.phase={variation*.63f,variation*.47f,variation*.81f};
    t.motion.spinSpeed=spin*direction;
    return t;
}
}
