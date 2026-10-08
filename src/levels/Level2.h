#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 2 settings through the shared level configuration interface.
class Level2 final : public LevelBase {
public:
    // Add one horizontally moving target while keeping the player position fixed.
    LevelConfig configure() const override;
};
}
