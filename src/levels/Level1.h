#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 1 settings through the shared level configuration interface.
class Level1 final : public LevelBase {
public:
    // Define three stationary targets for basic shooting from a fixed player position.
    LevelConfig configure() const override;
};
}
