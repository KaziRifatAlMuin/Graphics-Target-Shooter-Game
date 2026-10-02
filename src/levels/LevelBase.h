#pragma once
#include "gameplay/Target.h"
namespace shooter {
struct LevelConfig {
    int number=1;
    std::string title;
    bool playerMovement=false;
    unsigned seed=2107042;
    int birds=0, birdsPerTarget=0, humansPerTarget=0;
    std::vector<Target> targets;
};
// Levels provide data only; Game and LevelManager own the shared lifecycle.
class LevelBase {
public:
    virtual ~LevelBase()=default;
    virtual LevelConfig configure() const=0;
};
Target configuredTarget(Vec3 position, Vec3 amplitude={}, float speed=1, float spin=0, int variation=0);
}
