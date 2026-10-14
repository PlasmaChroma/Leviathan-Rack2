#pragma once
// Finite-input MP86 reference sections, not a complete firmware engine.
// See analysis/COLOR_HALO_ROUTING.md for exact entry boundaries and omissions.
#include "exact_tables.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace mp86_audit {
inline float color_envelope_scale(float repeats_gain, int zone) noexcept {
    const float factor = std::fma(repeats_gain-0.7f,0.7f,1.0f);
    return zone>1 && factor>=1.0f ? factor*1.5f : 1.5f;
}

struct ColorState {
    std::int32_t target = 0;                // +0
    float position = 0;                    // +4: smoothed table coordinate
    float coefficient = 0;                 // +8
    std::array<float,4> previous_input{};   // +12..24
    std::array<float,4> previous_allpass{}; // +28..40
};
static_assert(sizeof(ColorState) == 44);
static_assert(offsetof(ColorState, previous_allpass) == 28);

struct ColorChannel {
    ColorState state;
    float envelope = 0; // separate RAM word, not part of the 44-byte structure

    // Input is AFTER the preceding DC blocker. Target and starting position
    // must be in [0,254]. scale is live s10 at the entry, recovered upstream.
    float process(float input, float scale, const std::array<float,4>& c) noexcept {
        auto& s = state;
        s.position = std::fma(float(s.target)-s.position, 0.001f, s.position);
        const float feedback = s.position <= 176.0f
            ? s.position * -0x1.745d18p-9f // bits 0xbb3a2e8c
            : (s.position-426.0f)*0.002f;
        const float history = (s.previous_allpass[3]+s.previous_input[3])*0.3f;
        float x = std::fma(feedback, history, input);
        const auto i = static_cast<std::size_t>(s.position);
        const float target_a = mp86_tables::color_allpass_coefficient[i];
        s.coefficient = std::fma(target_a-s.coefficient, 0.01f, s.coefficient);
        std::array<float,5> taps{};
        taps[0] = x*1.144f;
        constexpr float gains[3] = {1.012f,1.144f,1.277f};
        for (std::size_t stage=0; stage<4; ++stage) {
            const float ap = std::fma(s.coefficient, x-s.previous_allpass[stage],
                                      s.previous_input[stage]);
            s.previous_input[stage] = x;
            s.previous_allpass[stage] = ap;
            const float sum = x+ap;
            if (stage==3) taps[4] = sum*0.665f;
            x = sum*0.5f;
            if (stage<3) taps[stage+1] = x*gains[stage];
        }
        const float p = s.position*0.02083330042660236358642578125f;
        const auto tap = p >= 4 ? std::size_t(3) : static_cast<std::size_t>(p);
        const float fraction = p >= 4 ? 0.999989986419677734375f : p-float(tap);
        const float morphed = std::fma(taps[tap+1]-taps[tap], fraction, taps[tap]);
        const float magnitude = std::abs(morphed);
        envelope = magnitude > envelope
            ? std::fma(magnitude-envelope, 0.9f, envelope)
            : envelope*0.9999f;
        const float m = std::clamp(envelope*scale, 0.5f, 5.0f);
        const float p2 = std::fma(-c[3],m,c[2]);
        const float p1 = std::fma(p2,m,-c[1]);
        return std::fma(p1,m,c[0])*morphed;
    }
};

struct AllpassHighpass {
    float previous_input = 0;
    float previous_allpass = 0;
    // The caller handles firmware's channel-specific NaN/previous-value guards.
    float process(float input, float a) noexcept {
        float ap = std::fma(-previous_allpass,a,previous_input);
        ap = std::fma(input,a,ap);
        previous_input = input;
        previous_allpass = ap;
        return (input-ap)*0.5f;
    }
};

struct HaloInjectionBranch {
    float damping_memory = 0;
    AllpassHighpass highpass;
    float process(float matrix_value, float injection, float gain, float damping) noexcept {
        float x = std::fma(injection,0.25f,matrix_value*gain);
        x = std::clamp(x,-0x1.555556p-1f,0x1.555556p-1f);
        const float squared = x*x;
        const float cubic_coefficient = -(0x1.555556p-2f*squared);
        x = std::fma(cubic_coefficient,x,x);
        x = std::fma(damping_memory-x,damping,x);
        damping_memory = x;
        return highpass.process(x,-0x1.fca8eep-1f); // bits 0xbf7e5477
    }
};

struct HaloWriteValues {
    // In ascending ring-offset order: 0,1839,2242,2663,3129,3689,4252,4862.
    std::array<float,8> ring{};
    // New samples for short buffers: 969,803,1236,1511.
    std::array<float,4> allpass{};
};

// h is the UNNORMALIZED current H8 output. injection is the .65-scaled,
// mode-blended signal at 0x08024be2/0x08024bfa. The caller obtains the two
// fractional and two integer delayed allpass reads BEFORE calling this.
inline HaloWriteValues halo_write_values(const std::array<float,8>& h,
        const std::array<float,2>& injection, float gain, float damping,
        const std::array<float,4>& delayed,
        std::array<HaloInjectionBranch,2>& state) noexcept {
    HaloWriteValues out;
    out.ring[0] = state[0].process(h[0],injection[0],gain,damping);
    out.ring[1] = state[1].process(h[1],injection[1],gain,damping);
    out.ring[4] = h[4]*gain;
    out.ring[6] = h[6]*gain;
    constexpr std::size_t branches[4] = {2,3,5,7};
    for (std::size_t i=0;i<4;++i) {
        const auto branch = branches[i];
        const float coefficient = i<2 ? 0.7f : 0.4f;
        // Original last two paths fuse H*gain into coefficient*delayed.
        const float write = i<2 ? std::fma(delayed[i],coefficient,h[branch]*gain)
            : std::fma(h[branch],gain,delayed[i]*coefficient);
        out.allpass[i] = write;
        out.ring[branch] = std::fma(-write,coefficient,delayed[i]);
    }
    return out;
}

inline float final_wet_shape(float x) noexcept {
    x = std::clamp(x,-1.0f,1.0f);
    const float half_square = (x*x)*0.5f;
    return std::fma(x,1.5f,-x*half_square);
}
inline float final_mix(float dry, float wet_before_shape, float mix) noexcept {
    const float wet = final_wet_shape(wet_before_shape);
    float out = std::fma(wet,mix+mix,dry);
    out = std::fma(-(dry+wet),mix*mix,out);
    return std::clamp(out,-1.0f,1.0f);
}
} // namespace mp86_audit
