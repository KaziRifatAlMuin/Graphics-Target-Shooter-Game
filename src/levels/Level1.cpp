#include "Level1.h"
namespace shooter {
LevelConfig Level1::configure() const {
    LevelConfig c; c.number=1; c.title="BASIC STATIC SHOOTING";
    c.targets={configuredTarget({-5,2.4f,-18}),configuredTarget({0,2.7f,-22}),configuredTarget({5,2.4f,-18})};
    return c;
}
}
