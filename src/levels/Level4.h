#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 4 settings through the shared level configuration interface.
class Level4 final : public LevelBase {
public:
    // Enable walking and cover with six targets, including spinning plates.
    LevelConfig configure() const override;
};
}
