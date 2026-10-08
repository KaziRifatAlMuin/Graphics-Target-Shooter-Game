#include "Level6.h"
#include "Level5.h"
namespace shooter {
// Extend Level 5 with more birds and two faster targets moving on all three axes.
LevelConfig Level6::configure() const {
    auto c=Level5{}.configure(); c.number=6; c.title="FASTER THREE-AXIS TARGETS"; c.birds=16;
    c.targets.push_back(configuredTarget({-8,3.8f,-88},{2.1f,1.0f,1.8f},2.4f,135,9));
    c.targets.push_back(configuredTarget({8,4.0f,-89},{1.9f,1.1f,1.9f},2.6f,145,10));
    return c;
}
}
