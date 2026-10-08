#pragma once
#include "audio/SoundEvent.h"
#include <vector>
#include <memory>

namespace shooter {
// Generate each effect as sampled noise plus a sine tone, using 22050 samples per second.
std::vector<float> synthesizeSound(SoundEvent event);
class Sound {
public:
    // Precompute all sound effects once so playback only mixes existing samples.
    Sound();
    // Destroy the owned audio implementation, which releases the Windows device and prepared buffers.
    ~Sound();
    // Open mono 16-bit PCM output and prepare three reusable audio blocks on Windows.
    bool initialize();
    // Toggle sound; disabling clears voices and stops queued Windows playback.
    void setEnabled(bool enabled);
    // Queue an effect from sample zero, dropping the oldest voice if the 16-voice limit is reached.
    void play(SoundEvent event);
    // Refill finished output blocks by summing active voices and removing those that have ended.
    void update();
    // Report whether the audio device is ready for playback.
    bool available() const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
}
