#include "Level4.h"
namespace shooter {
LevelConfig Level4::configure() const {
    LevelConfig c; c.number=4; c.title="MOVE AROUND COVER"; c.playerMovement=true;
    c.targets={configuredTarget({-8,2.4f,-26},{1.5f,0,0},1.2f,0,1),
        configuredTarget({8,2.7f,-28},{0,.6f,0},1.4f,65,2),
        configuredTarget({-8,2.5f,-42},{0,0,1.6f},1.15f,0,3),
        configuredTarget({8,2.9f,-45},{1.8f,0,0},1.6f,80,4),
        configuredTarget({-8,3,-60},{0,.8f,0},1.8f,0,5),
        configuredTarget({8,2.6f,-63},{0,0,1.7f},1.35f,0,6)};
    return c;
}
}
