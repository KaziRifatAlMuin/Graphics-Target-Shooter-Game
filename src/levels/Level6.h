#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 6 settings through the shared level configuration interface.
class Level6 final : public LevelBase {
public:
    // Extend Level 5 with more birds and two faster targets moving on all three axes.
    LevelConfig configure() const override;
};
}
