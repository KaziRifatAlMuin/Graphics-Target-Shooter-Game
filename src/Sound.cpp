#include "Sound.h"
#include "Transform.h"
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
std::vector<float> synthesizeSound(SoundEvent event) {
    const int kind=static_cast<int>(event);
    const float durations[]={.16f,.28f,.11f,.13f,.34f,.05f};
    const float frequencies[]={170,95,230,1250,650,850};
    const float duration=durations[kind];
    std::vector<float> samples(static_cast<std::size_t>(duration*sampleRate));
    std::uint32_t random=2107042+kind;
    for (std::size_t i=0;i<samples.size();++i) {
        const float t=float(i)/sampleRate,u=t/duration;
        random=random*1664525u+1013904223u;
        const float noise=float((random>>8)&65535)/32767.5f-1;
        const float envelope=std::min(t/.003f,1.0f)*(1-u)*(1-u);
        const float tone=std::sin(2*pi*frequencies[kind]*(t-.32f*t*t/duration));
        const float noisy=kind<3?.78f:kind==4?.65f:kind==3?.10f:0;
        samples[i]=.38f*envelope*(noisy*noise+(1-noisy)*tone);
    }
    return samples;
}
struct Sound::Impl {
    bool ready=false,enabled=true;
    std::array<std::vector<float>,6> effects;
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
Sound::Sound():impl(std::make_unique<Impl>()) {
    for (int i=0;i<6;++i) impl->effects[i]=synthesizeSound(static_cast<SoundEvent>(i));
}
Sound::~Sound()=default;
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
bool Sound::available() const { return impl->ready; }
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
void Sound::play(SoundEvent event) {
    if (!impl->ready || !impl->enabled) return;
    if (impl->voices.size()>=16) impl->voices.erase(impl->voices.begin());
    impl->voices.push_back({static_cast<int>(event),0});
}
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
