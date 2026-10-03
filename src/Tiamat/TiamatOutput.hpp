#pragma once
#include "TiamatControls.hpp"

namespace tiamat {
class Output {
public:
    Output() noexcept;
    void processBlock(const float* dry, const float* wet, float* output, float mixTarget, float crossfeed) noexcept;
    const OutputState& state() const noexcept { return state_; }
    static void width(float& left, float& right, float crossfeed) noexcept;
private:
    friend struct OutputTestAccess;
    void processFrames(const float*, const float*, float*, unsigned, float, float) noexcept;
    float sineGain(float normalizedPhase) const noexcept;
    OutputState state_;
    ToneCoefficients tone_;
    // Quarter sine, prepared off the audio thread. Interpolation error is
    // measured separately from the exact DSP fixtures; no decisions use it.
    std::array<float, 4097> sine_;
};
} // namespace tiamat
