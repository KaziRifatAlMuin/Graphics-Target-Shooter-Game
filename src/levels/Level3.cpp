#include "Level3.h"
namespace shooter {
LevelConfig Level3::configure() const {
    LevelConfig c; c.number=3; c.title="MULTI-AXIS INTRODUCTION";
    c.targets={configuredTarget({-8,2.8f,-30},{2,0,0},1.2f,0,1),
        configuredTarget({-3,3.2f,-39},{0,.9f,0},1.5f,0,2),
        configuredTarget({3,2.8f,-34},{0,0,2.5f},1.05f,0,3),
        configuredTarget({8,3.4f,-45},{1.6f,0,0},1.7f,0,4)};
    return c;
}
}
