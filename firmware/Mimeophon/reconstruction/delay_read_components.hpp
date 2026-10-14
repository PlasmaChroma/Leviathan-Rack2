#pragma once
// MP86 continuation: narrowly scoped, instruction-backed reference components.
// Not a complete delay engine. See ../analysis/RACK_RECONSTRUCTION.md.
// std::fma preserves the audited fused steps; benchmark/adapt before production use.
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace mp86_audit {
struct DelayHeadLayout {
    std::uint32_t selected;       // +0: pair-selection flag; not a universal enable
    float delay;                  // +4: tracked delay, samples
    float moving_delay;           // +8: alternate moving delay, samples
    float absolute_read_position; // +12: before integer masking
    std::uint32_t use_moving;      // +16: chooses +8 instead of +4
    std::uint32_t read_index;      // +20: masked integer index
    std::uint32_t unknown24;       // +24: initialized to 1; semantics unresolved
    float fraction;               // +28
    float gain;                   // +32: complementary pair crossfade gain
    std::uint32_t buffer_address; // +36: firmware address, NOT a host pointer
    std::uint32_t unknown40;       // +40: no semantic assignment yet
};
static_assert(sizeof(DelayHeadLayout) == 44);
static_assert(offsetof(DelayHeadLayout, gain) == 32);
static_assert(offsetof(DelayHeadLayout, buffer_address) == 36);

// 0x0802443a..0x08024478; six read sites share this algebra.
// A four-tap QUADRATIC in t, not a conventional cubic interpolator.
inline float delay_interpolate(float before, float center, float after,
                               float after2, float t) noexcept {
    const float half_t = t * 0.5f;
    const float outer = before + after2;
    float curve = outer - center;
    const float sum = center + outer;
    curve = curve - after;
    curve = curve * half_t;
    curve = std::fma(-sum, 0.5f, curve);
    curve = std::fma(after, 1.5f, curve);
    return std::fma(t, curve, center);
}

// Caller supplies a power-of-two ring, nonnegative finite delay coordinates,
// and a masked cursor. Full firmware mode/event logic is intentionally absent.
inline float read_delay_head(DelayHeadLayout& head, const float* ring,
                             std::uint32_t mask, std::uint32_t cursor,
                             float modulation) noexcept {
    const float delay = head.use_moving ? head.moving_delay : head.delay;
    const float position = std::max(delay + modulation, 2.0f) + float(cursor);
    const auto integer = static_cast<std::int32_t>(position);
    head.absolute_read_position = position;
    head.read_index = static_cast<std::uint32_t>(integer) & mask;
    head.fraction = position - float(integer);
    const auto i = head.read_index;
    float value = delay_interpolate(ring[(i - 1) & mask], ring[i],
                                   ring[(i + 1) & mask], ring[(i + 2) & mask],
                                   head.fraction);
    // 0x0802447c..0x0802448e: NaN only, not infinity, becomes +100 before gain.
    if (std::isnan(value)) value = 100.0f;
    return value * head.gain;
}

// 0x0802430c..0x08024368 with 0x08025baa/0x08025e0e/0x080268a8.
// Finite-input reference. Outer clamps are applied to the error BEFORE rational law.
inline float tracked_delay_step(float current, float target) noexcept {
    float delta = target - current;
    if (delta > 64.0f) delta = 0x1.c9c69cp+2f;
    else if (delta < -64.0f) delta = -0x1.c9c69cp+2f;
    else if (delta > 0.001f || delta < -0.001f) {
        const float squared = delta * delta;
        const float numerator = (squared + 27.0f) * delta;
        const float denominator = std::fma(squared, 9.0f, 27.0f);
        delta = numerator / denominator;
    }
    return current + delta;
}

// Pair (0,2) or (1,3), AFTER using the old gains to render the current sample.
// 0x0802572a..0x08025766, 0x0802687a..0x080268a2, 0x08025f08.
inline void advance_head_gains(DelayHeadLayout& first, DelayHeadLayout& second,
                               float increment) noexcept {
    first.gain = std::clamp(first.gain + (first.selected == 1 ? increment : -increment),
                            0.0f, 1.0f);
    second.gain = 1.0f - first.gain;
    // Transition request/status writes are NOT represented by this helper.
}

struct StereoInputAttenuation {
    float envelope = 0.0f; // RAM 0x20003884, initially zero
    void process(float& left, float& right) noexcept {
        // 0x08023f1e..0x08023f60, once per stereo frame.
        const float magnitude = std::abs(left) + std::abs(right);
        envelope = std::fma(magnitude - envelope, 0.01f, envelope);
        if (envelope < 0.0005f) {
            const float gain = (envelope * envelope) * 4000000.0f;
            left *= gain;
            right *= gain;
        }
    }
};
} // namespace mp86_audit
