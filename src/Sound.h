#pragma once
#include "Game.h"
#include <memory>

namespace shooter {
std::vector<float> synthesizeSound(SoundEvent event);
class Sound {
public:
    Sound();
    ~Sound();
    bool initialize();
    void setEnabled(bool enabled);
    void play(SoundEvent event);
    void update();
    bool available() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
}
