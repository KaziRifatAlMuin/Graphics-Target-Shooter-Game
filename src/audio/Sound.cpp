#include "audio/Sound.h"
#include "core/Transform.h"
#include <cstdint>
#include <algorithm>
#include <array>
#include <cmath>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#endif

namespace shooter {
namespace { constexpr int sampleRate=22050, blockSamples=512; }
// Generate each effect as sampled noise plus a sine tone, using 22050 samples per second.
std::vector<float> synthesizeSound(SoundEvent event) {
    const int kind=static_cast<int>(event);
    const float durations[]={.16f,.28f,.11f,.13f,.34f,.05f,.4f,.65f,.45f,.65f,1.25f,.8f,.42f,.70f};
    const float frequencies[]={170,95,230,1250,650,850,160,95,520,660,880,330,1400,240};
    const float duration=durations[kind];
    std::vector<float> samples(static_cast<std::size_t>(duration*sampleRate));
    std::uint32_t random=2107042+kind;
    for (std::size_t i=0;i<samples.size();++i) {
        const float t=float(i)/sampleRate,u=t/duration;
        random=random*1664525u+1013904223u;
        const float noise=float((random>>8)&65535)/32767.5f-1;
        // Volume envelope=min(t/0.003,1)*(1-t/duration)^2 gives a short attack and a fading tail.
        const float envelope=std::min(t/.003f,1.0f)*(1-u)*(1-u);
        float frequency=frequencies[kind];
        if (event==SoundEvent::Victory || event==SoundEvent::Complete || event==SoundEvent::Start)
            frequency*=1+float(int(u*4))*.25f;
        if (event==SoundEvent::HumanPenalty) frequency*=1+.3f*std::sin(t*32);
        if (event==SoundEvent::BirdDie) frequency=(1500.0f*(1.0f-u*0.75f))*(1.0f+0.35f*std::sin(t*140.0f));
        if (event==SoundEvent::HumanDie) frequency=(240.0f*(1.0f-u*0.65f))*(1.0f+0.15f*std::sin(t*28.0f));
        // Tone=sin(2*pi*frequency*(t-0.32*t^2/duration)); the quadratic phase bends pitch over time.
        const float tone=std::sin(2*pi*frequency*(t-.32f*t*t/duration));
        const float noisy=kind<3?.78f:kind==4?.65f:kind==3?.10f:event==SoundEvent::BirdDie?.35f:event==SoundEvent::HumanDie?.42f:0;
        samples[i]=.38f*envelope*(noisy*noise+(1-noisy)*tone);
    }
    return samples;
}
// Hide platform-specific audio buffers and track each active sound's playback cursor.
struct Sound::Impl {
    bool ready=false,enabled=true;
    std::array<std::vector<float>,static_cast<int>(SoundEvent::Count)> effects;
    struct Voice { int kind; std::size_t cursor=0; };
    std::vector<Voice> voices;
#ifdef _WIN32
    HWAVEOUT device=nullptr;
    struct Block { std::array<short,blockSamples> samples{}; WAVEHDR header{}; };
    std::array<Block,3> blocks;
    ~Impl() {
        if (!device) return;
        waveOutReset(device);
        for (auto& b:blocks) if (b.header.dwFlags&WHDR_PREPARED) waveOutUnprepareHeader(device,&b.header,sizeof(WAVEHDR));
        waveOutClose(device);
    }
#endif
};
// Precompute all sound effects once so playback only mixes existing samples.
Sound::Sound():impl(std::make_unique<Impl>()) {
    for (int i=0;i<static_cast<int>(SoundEvent::Count);++i) impl->effects[i]=synthesizeSound(static_cast<SoundEvent>(i));
}
// Destroy the owned audio implementation, which releases the Windows device and prepared buffers.
Sound::~Sound()=default;
// Open mono 16-bit PCM output and prepare three reusable audio blocks on Windows.
bool Sound::initialize() {
#ifdef _WIN32
    if (impl->device) return impl->ready;
    WAVEFORMATEX format{};
    format.wFormatTag=WAVE_FORMAT_PCM; format.nChannels=1; format.nSamplesPerSec=sampleRate;
    format.wBitsPerSample=16; format.nBlockAlign=2; format.nAvgBytesPerSec=sampleRate*2;
    if (waveOutOpen(&impl->device,WAVE_MAPPER,&format,0,0,CALLBACK_NULL)!=MMSYSERR_NOERROR) return false;
    for (auto& b:impl->blocks) {
        b.header.lpData=reinterpret_cast<LPSTR>(b.samples.data());
        b.header.dwBufferLength=sizeof(b.samples);
        if (waveOutPrepareHeader(impl->device,&b.header,sizeof(WAVEHDR))!=MMSYSERR_NOERROR) return false;
    }
    impl->ready=true;
#endif
    return impl->ready;
}
// Report whether the audio device is ready for playback.
bool Sound::available() const { return impl->ready; }
// Toggle sound; disabling clears voices and stops queued Windows playback.
void Sound::setEnabled(bool enabled) {
    if (enabled==impl->enabled) return;
    impl->enabled=enabled;
    if (!enabled) {
        impl->voices.clear();
#ifdef _WIN32
        if (impl->device) waveOutReset(impl->device);
#endif
    }
}
// Queue an effect from sample zero, dropping the oldest voice if the 16-voice limit is reached.
void Sound::play(SoundEvent event) {
    if (!impl->ready || !impl->enabled) return;
    if (impl->voices.size()>=16) impl->voices.erase(impl->voices.begin());
    impl->voices.push_back({static_cast<int>(event),0});
}
// Refill finished output blocks by summing active voices and removing those that have ended.
void Sound::update() {
#ifdef _WIN32
    if (!impl->ready) return;
    for (auto& block:impl->blocks) {
        if (block.header.dwFlags&WHDR_INQUEUE) continue;
        for (auto& sample:block.samples) {
            float mix=0;
            if (impl->enabled) for (auto& voice:impl->voices) {
                const auto& effect=impl->effects[voice.kind];
                if (voice.cursor<effect.size()) mix+=effect[voice.cursor++];
            }
            // Convert mixed float audio to 16-bit PCM: sample=clamp(sum,-0.92,0.92)*32767.
            sample=static_cast<short>(std::clamp(mix,-.92f,.92f)*32767);
        }
        impl->voices.erase(std::remove_if(impl->voices.begin(),impl->voices.end(),[&](const Impl::Voice& v) {
            return v.cursor>=impl->effects[v.kind].size();
        }),impl->voices.end());
        if (waveOutWrite(impl->device,&block.header,sizeof(WAVEHDR))!=MMSYSERR_NOERROR) impl->ready=false;
    }
#endif
}
}
