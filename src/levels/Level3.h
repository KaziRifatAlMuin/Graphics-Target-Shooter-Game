#pragma once
#include "LevelBase.h"
namespace shooter {
// Provide Level 3 settings through the shared level configuration interface.
class Level3 final : public LevelBase {
public:
    // Introduce separate X, Y, and Z target motions so each axis can be observed.
    LevelConfig configure() const override;
};
}
