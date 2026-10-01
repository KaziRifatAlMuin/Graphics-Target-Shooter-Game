#pragma once
#include "LevelBase.h"
namespace shooter {
class Level4 final : public LevelBase {
public:
    LevelConfig configure() const override;
};
}
