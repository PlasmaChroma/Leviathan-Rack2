#include "TiamatOutput.hpp"
#include "TiamatMath.hpp"
#include <algorithm>
#include <cmath>

namespace tiamat {
Output::Output() noexcept : tone_(outputToneCoefficients()) {
    for (unsigned i = 0; i < sine_.size(); ++i)
        sine_[i] = float(std::sin(double(i) * (1.57079632679489661923 / 4096.0)));
}
float Output::sineGain(float phase) const noexcept {
    const float position = phase * 4096.f;
    const unsigned index = std::min(unsigned(position), 4095u);
    return multiplyAdd(position - float(index), sine_[index + 1] - sine_[index], sine_[index]);
}
void Output::width(float& l, float& r, float a) noexcept {
    l = multiplyAdd(a, r, (1.f - a) * l);
    r = multiplyAdd(a, l, (1.f - a) * r);
}
void Output::processBlock(const float* dry, const float* wet, float* out, float target, float crossfeed) noexcept {
    processFrames(dry, wet, out, blockFrames, target, crossfeed);
}
void Output::processFrames(const float* dry, const float* wet, float* out, unsigned frames, float target, float a) noexcept {
    target = normalized(target);
    a = .5f * normalized(a * 2.f);
    for (unsigned f = 0; f < frames; ++f) {
        state_.mix = multiplyAdd(target - state_.mix, .001f, state_.mix);
        const float dryGain = sineGain(1.f - state_.mix), wetGain = sineGain(state_.mix);
        float pair[2];
        for (unsigned c = 0; c < 2; ++c) {
            const unsigned i = f * 2 + c;
            const float mixed = multiplyAdd(wetGain, finiteOrZero(wet[i]), finiteOrZero(dry[i]) * dryGain);
            const float value = multiplyAdd(tone_.feedforward, mixed, tone_.feedback * state_.toneHistory[c]);
            state_.toneHistory[c] = pair[c] = finiteOrZero(value);
        }
        width(pair[0], pair[1], a);
        out[f * 2] = finiteOrZero(pair[0]);
        out[f * 2 + 1] = finiteOrZero(pair[1]);
    }
}
} // namespace tiamat
