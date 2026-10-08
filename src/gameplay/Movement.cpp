#include "gameplay/Movement.h"
namespace shooter {
// Bounded motion on each axis: offset(t)=amplitude*sin(frequency*t+phase); frequency is in radians/second.
Vec3 movementOffset(const MovementPattern& m, float t) {
    return {m.amplitude.x*std::sin(t*m.frequency.x+m.phase.x),
            m.amplitude.y*std::sin(t*m.frequency.y+m.phase.y),
            m.amplitude.z*std::sin(t*m.frequency.z+m.phase.z)};
}
}
