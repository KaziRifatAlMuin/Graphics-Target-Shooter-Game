#include "Level7.h"
#include "Level6.h"
namespace shooter {
LevelConfig Level7::configure() const {
    auto c=Level6{}.configure(); c.number=7; c.title="FINAL ARENA";
    c.birds=0; c.birdsPerTarget=4; c.humansPerTarget=3;
    c.targets.push_back(configuredTarget({0,4.0f,-36},{2.2f,1.1f,1.9f},3.4f,165,11));
    c.targets.push_back(configuredTarget({0,4.5f,-72},{2.1f,1.2f,2.0f},3.7f,180,12));
    return c;
}
}
