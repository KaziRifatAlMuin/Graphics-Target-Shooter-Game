#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 5 settings through the shared level configuration interface.
class Level5 final : public LevelBase {
public:
    // Extend Level 4 with birds and two targets combining motion along multiple axes.
    LevelConfig configure() const override;
};
}
