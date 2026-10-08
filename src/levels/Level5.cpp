#include "Level5.h"
#include "Level4.h"
namespace shooter {
// Extend Level 4 with birds and two targets combining motion along multiple axes.
LevelConfig Level5::configure() const {
    auto c=Level4{}.configure(); c.number=5; c.title="BIRDS AND COMPOUND MOTION"; c.birds=8;
    c.targets.push_back(configuredTarget({-8,3.3f,-77},{1.9f,0,1.6f},1.65f,95,7));
    c.targets.push_back(configuredTarget({8,3.2f,-78},{0,.85f,1.8f},1.8f,110,8));
    return c;
}
}
