#pragma once
#include "LevelBase.h"
namespace shooter {
class Level1 final : public LevelBase {
public:
    LevelConfig configure() const override;
};
}
