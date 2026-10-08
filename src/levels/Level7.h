#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 7 settings through the shared level configuration interface.
class Level7 final : public LevelBase {
public:
    // Extend Level 6 to twelve targets, with four birds and three humans assigned to each target.
    LevelConfig configure() const override;
};
}
