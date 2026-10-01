#pragma once
#include "Transform.h"
namespace shooter {
// Zero amplitude disables an axis. Frequency is radians/sec; phases are radians.
struct MovementPattern {
    Vec3 amplitude{}, frequency{1,1,1}, phase{};
    float spinSpeed=0;
};
Vec3 movementOffset(const MovementPattern& pattern, float time);
}
