#pragma once
// UI-side math only. No Rack, allocation, parsing, I/O, or graphics dependency.
#include "BodyMapData.hpp"
#include <algorithm>
#include <array>
#include <cmath>

namespace vtune_body {

enum class Mode : int { Report = 0, Symbolic = 1, Combined = 2, Off = 3 };
using Weights = std::array<float, kLayerCount>;

inline Mode sanitizeMode(int value) noexcept {
    // Retired Symbolic/Combined selections migrate to full-strength body shading.
    return value == int(Mode::Off) ? Mode::Off : Mode::Report;
}
inline float unit(float x) noexcept {
    return std::isfinite(x) ? std::max(0.f, std::min(1.f, x)) : 0.f;
}
inline float smoothstep01(float t) noexcept {
    t = unit(t);
    return t * t * (3.f - 2.f * t);
}
inline bool validFrequency(float hz) noexcept {
    return std::isfinite(hz) && hz >= kMinHz && hz <= kMaxHz;
}

// Frequencies outside the report's implemented domain are not clamped onto an
// anatomical region. A zero value also represents an unavailable source.
// The six boundary ramps form a partition of unity inside [20, 2000].
inline Weights evaluate(float hz, Mode mode) noexcept {
    Weights out{};
    if (!validFrequency(hz) || mode == Mode::Off)
        return out;
    if (mode == Mode::Report || mode == Mode::Combined) {
        const float gain = mode == Mode::Combined ? kCombinedReportGain : 1.f;
        float previous = 1.f;
        for (std::size_t i = 0; i + 1 < kBandCount; ++i) {
            const float distanceOctaves = std::log2(hz / kBands[i].highHz);
            const float boundary = smoothstep01(
                (distanceOctaves + kBoundaryHalfWidthOctaves)
                / (2.f * kBoundaryHalfWidthOctaves));
            out[i] = gain * std::max(0.f, previous - boundary);
            previous = boundary;
        }
        out[kBandCount - 1] = gain * previous;
    }
    if (mode == Mode::Symbolic || mode == Mode::Combined) {
        const float gain = mode == Mode::Combined ? kCombinedSymbolicGain : 1.f;
        for (std::size_t i = 0; i < kToneCount; ++i) {
            const float cents = std::abs(1200.f * std::log2(hz / kTones[i].frequencyHz));
            // Deliberately finite support, not unconditional nearest-tone choice.
            // This width is a UI tolerance, not a measured biological bandwidth.
            out[kBandCount + i] = gain * smoothstep01(1.f - cents / kToneSupportCents);
        }
    }
    return out;
}

// Smooth the spatial activations, NOT the frequency. A leap from 25 to 1500 Hz
// therefore crossfades the two endpoint maps without inventing activity at
// every intervening band. Equal time constants preserve the report sum bound.
class Animation {
    Weights current_{};
public:
    const Weights& weights() const noexcept { return current_; }
    void reset() noexcept { current_.fill(0.f); }
    void snap(const Weights& target) noexcept {
        for (std::size_t i = 0; i < kLayerCount; ++i) current_[i] = unit(target[i]);
    }
    void advance(const Weights& target, double dt) noexcept {
        if (!std::isfinite(dt) || dt <= 0.0) return;
        const float a = static_cast<float>(-std::expm1(-dt / kSmoothingSeconds));
        for (std::size_t i = 0; i < kLayerCount; ++i) {
            current_[i] += a * (unit(target[i]) - current_[i]);
            current_[i] = unit(current_[i]);
            if (current_[i] < 1e-6f && target[i] == 0.f) current_[i] = 0.f;
        }
    }
};

struct FitRect {
    float x, y, width, height;
    // Explicit construction keeps this header compatible with Rack's C++11
    // plugin builds as well as C++17 standalone test builds.
    FitRect(float xValue = 0.f, float yValue = 0.f,
            float widthValue = 0.f, float heightValue = 0.f) noexcept
        : x(xValue), y(yValue), width(widthValue), height(heightValue) {}
};
// One fit transform for ALL textures. Do not fit each cropped/mask image
// independently; their pixels share the original source canvas registration.
inline FitRect aspectFit(float width, float height) noexcept {
    if (!std::isfinite(width) || !std::isfinite(height) || width <= 0.f || height <= 0.f)
        return {};
    const float s = std::min(width / kSourceWidth, height / kSourceHeight);
    const float w = kSourceWidth * s, h = kSourceHeight * s;
    return {(width - w) * .5f, (height - h) * .5f, w, h};
}

} // namespace vtune_body
