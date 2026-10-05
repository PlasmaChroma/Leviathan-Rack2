#pragma once
// Independently expressed equations from the supplied Lúbadh 2.1.0 binary.
// Research reference, not a full instrument and not a bit-exact ARM port.
// No allocation, disk access, thread creation, Rack dependency, or firmware execution.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
namespace lubadh_research {
constexpr float legacyDspRate = 49170.25390625f;
constexpr std::size_t legacyDeckSamples = 29502000;
inline float unit(float x) noexcept { return std::clamp(x,0.0f,1.0f); }
inline float wear(float x, float amount) noexcept {
    return std::fma(unit(amount), x*std::abs(x)-x, x);
}
struct SoftClipper {
    float knee=0, compensation=0;
    float process(float x) const noexcept {
        constexpr float A=28.274333953857422f, B=9.42477798461914f;
        const float k=unit(knee), c=unit(compensation);
        if (static_cast<double>(k)<=0.01) {
            x *= 1.0f+c;
            const float x2=x*x;
            return std::clamp(x*(A+x2)/std::fma(B,x2,A),-1.0f,1.0f);
        }
        x=std::clamp(x,-1.0f,1.0f);
        float m=std::abs(x);
        if (m>k) {
            const float a=m-k, r=a/(1.0f-k);
            m=k+a/(1.0f+r*r);
        }
        const float gain=1.0f+c*(2.0f/(1.0f+k)-1.0f);
        return std::copysign(m*gain,x);
    }
};
inline float cubic(float a,float b,float c,float d,float t) noexcept {
    // InputBuffer::interpolate arithmetic, not its addressing/boundary rules.
    constexpr float inv6=0.16666670143604279f;
    const float delta=c-b;
    return b+t*(delta-(1.0f-t)*inv6*(d+2.0f*a-3.0f*b+t*(d-a-3.0f*delta)));
}
struct OnePole {
    float coefficient=1, previousInput=0, previousLow=0;
    float low(float x) noexcept {
        const float y=(x+previousInput-(1.0f-coefficient)*previousLow)/(1.0f+coefficient);
        previousInput=x; previousLow=y; return y;
    }
    float high(float x) noexcept { return x-low(x); }
    void reset() noexcept {previousInput=previousLow=0;}
};
struct TapeAgeFilter {
    // Active TapeFilter, NOT the unused standalone TapeAge class.
    // Exact stored coefficients for legacy timing; modern-rate adaptation is a separate design.
    OnePole hp100{156.5137939453125f}, hp150{104.34252166748047f};
    OnePole lp500{31.30275535583496f},lp1000{15.65137767791748f},lp3000{5.21712589263916f};
    float process(float x,float amount) noexcept {
        const float h=static_cast<float>(0.6*static_cast<double>(hp100.high(x)+hp150.high(x)));
        const float wet=static_cast<float>(0.6*static_cast<double>(lp500.low(h)+lp1000.low(h)))+lp3000.low(0.2f*x);
        return std::fma(unit(amount),wet-x,x);
    }
    void reset() noexcept {hp100.reset();hp150.reset();lp500.reset();lp1000.reset();lp3000.reset();}
};
struct TapeDiffuser {
    // Active TapeAllpass: finite signed feedforward network, not physical magnetic hysteresis.
    static constexpr std::array<std::size_t,4> lengths{68,159,251,375};
    std::array<std::array<float,375>,4> buffers{};
    std::array<std::size_t,4> positions{};
    float process(float x,float amount) noexcept {
        std::array<float,4> d{};
        for(std::size_t i=0;i<4;++i) d[i]=buffers[i][positions[i]];
        const std::array<float,4> writes{x,x-d[0],x+d[0]-d[1],x+d[0]+d[1]-d[2]};
        for(std::size_t i=0;i<4;++i){buffers[i][positions[i]]=writes[i];positions[i]=(positions[i]+1)%lengths[i];}
        return std::fma(unit(amount),0.175f*(x+d[0]+d[1]+d[2]+d[3])-x,x);
    }
    void reset() noexcept {buffers={};positions={};}
};
inline float idealHybridFade(float t) noexcept {
    // Analytic shape. Firmware indexes a generated 256-entry float table instead.
    t=unit(t); return 0.5f*(t+std::sin(1.5707963267948966f*t));
}
struct ReverbControls {float dry,wet,decay;};
inline ReverbControls reverbControls(float tapeAmount,float presetAmount) noexcept {
    const float u=unit(tapeAmount*presetAmount),v=1.0f-u;
    const float norm=std::sqrt(u*u+v*v);
    return {v/norm,u/norm,std::min(2.0f*u,0.9f)};
}
inline float presetVOctSpeed(float controlVolts) noexcept {
    const float v=std::clamp(controlVolts,0.0f,5.0f);
    return v<1.0f ? 0.25f*v : std::exp2(v-3.0f);
}
inline float writeSample(float inputContribution,float oldSample,float feedback,const SoftClipper& clip) noexcept {
    // AudioData::add equation only; caller owns write position, envelopes and clip parameters.
    return clip.process(std::fma(feedback,oldSample,inputContribution));
}
} // namespace lubadh_research
