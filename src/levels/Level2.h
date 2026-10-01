#pragma once
#include "LevelBase.h"
namespace shooter {
class Level2 final : public LevelBase {
public:
    LevelConfig configure() const override;
};
}
