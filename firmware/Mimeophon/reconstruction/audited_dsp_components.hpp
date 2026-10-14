#pragma once
// MP86: selected mathematical blocks transcribed from recovered instructions.
// This is NOT recovered source and is NOT a complete Mimeophon implementation.
// State/update wiring, modes and complete signal routing remain to be ported.
// Finite-input algebra is represented; bit-exact ARM FMA/rounding is not claimed.
#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace mp86_audit {
inline std::array<float, 8> hadamard8(std::array<float, 8> x) noexcept {
    // Unnormalized. Observed 0x08024c2a..0x08024d88.
    // Firmware's live gain is applied separately; do NOT normalize twice.
    for (std::size_t width = 1; width < 8; width *= 2)
        for (std::size_t start = 0; start < 8; start += width * 2)
            for (std::size_t j = 0; j < width; ++j) {
                const float a = x[start + j], b = x[start + j + width];
                x[start + j] = a + b;
                x[start + j + width] = a - b;
            }
    return x;
}
inline float clipped_cubic(float x) noexcept {
    // Observed around 0x08024d50..0x08024d7a; one of several nonlinear stages.
    x = std::clamp(x, -2.0f / 3.0f, 2.0f / 3.0f);
    return x - (x * x * x) / 3.0f;
}
struct AllpassAverage {
    float previous_input = 0.0f;
    float previous_allpass_output = 0.0f;
    float process(float x, float coefficient) noexcept {
        // a(z) = (coefficient + z^-1) / (1 + coefficient*z^-1)
        // Output = 0.5 * (input + allpass(input)).
        // Four such stages occur in each audited Color channel.
        const float ap = previous_input + coefficient * (x - previous_allpass_output);
        previous_input = x;
        previous_allpass_output = ap;
        return 0.5f * (x + ap);
    }
};
struct ColorCascade {
    std::array<AllpassAverage, 4> stages{};
    float coefficient = 0.0f;
    std::array<float, 5> process(float x, float coefficient_target) noexcept {
        coefficient += 0.01f * (coefficient_target - coefficient);
        std::array<float, 5> taps{};
        taps[0] = x;
        for (std::size_t i = 0; i < stages.size(); ++i)
            taps[i+1] = stages[i].process(taps[i], coefficient);
        return taps;
        // MP86 additionally crossfades/scales taps, applies feedback shaping,
        // and couples the network. Those operations are intentionally absent.
    }
};
inline float zone_polynomial_gain(float m, const std::array<float, 4>& c) noexcept {
    // 0x08024888..0x080248dc. Caller computes/clamps m from envelope state.
    return c[0] + m * (-c[1] + m * (c[2] - m * c[3]));
}
inline float allpass_delay_step(float input, float delayed, float coefficient,
                                float& sample_to_write) noexcept {
    // Audited Halo allpass equations at 0x08024fc2, 0x0802507c,
    // 0x080250e0 and 0x0802513c. Buffer/address logic is caller's job.
    sample_to_write = input + coefficient * delayed;
    return delayed - coefficient * sample_to_write;
}
struct ModulationRandomWalk {
    std::uint32_t state = 12345;
    float step(std::uint32_t& state_out) noexcept {
        state = state * UINT32_C(196314165) + UINT32_C(907633515);
        state_out = state;
        return (static_cast<float>(state) * 0x1p-32f - 0.5f) * 2.0e-6f;
    }
    // Two state-dependent clamp/reset branches surround this increment in MP86.
    // This helper exports only the unambiguous LCG and centered increment.
    // Do NOT assume it is output dither or the MP86 noise-floor fix.
};
inline float block_slew(float current, float target) noexcept {
    // Several control paths use this once per 4-frame callback.
    return current + 0.001f * (target - current);
}
} // namespace mp86_audit
