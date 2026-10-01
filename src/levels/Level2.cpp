#include "Level2.h"
namespace shooter {
LevelConfig Level2::configure() const {
    LevelConfig c; c.number=2; c.title="FIRST MOVING TARGET";
    c.targets={configuredTarget({-6,2.5f,-26}),configuredTarget({0,3,-31},{2.3f,0,0},1.1f),configuredTarget({6,2.5f,-26})};
    return c;
}
}
