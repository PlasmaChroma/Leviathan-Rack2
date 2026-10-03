#include "TiamatControls.hpp"

#include <algorithm>
#include <cmath>

namespace tiamat {

float finiteOrZero(float x) noexcept { return std::isfinite(x) ? x : 0.f; }
float normalized(float x) noexcept { return std::max(0.f, std::min(1.f, finiteOrZero(x))); }

float snapOctave(float magnitude, bool secondPass, bool* snapped) noexcept {
    bool hit = false;
    for (unsigned i = 0; i < octaveTargets.size(); ++i) {
        const float target = octaveTargets[i];
        const float delta = target * (i == 0 ? .15f : (secondPass ? .015f : .025f));
        if (magnitude > target - delta && magnitude < target + delta) {
            magnitude = target;
            hit = true;
        }
    }
    if (snapped) *snapped = hit;
    return magnitude;
}

float microRate(float knob, float volts, bool reverse, bool* octave) noexcept {
    float rate = snapOctave(std::exp(4.1588830948f * normalized(knob)) * .125f, false);
    // Avoid overflowing exp2 for hostile finite voltages. Values above this
    // still exceed the reader's +8 limit; very slow negative CV is preserved.
    const float pitch = std::max(-126.f, std::min(120.f, finiteOrZero(volts)));
    rate = snapOctave(rate * std::exp2(pitch), true, octave);
    if (octave && rate > 8.f) *octave = true;
    return reverse ? -rate : rate;
}

float internalFrequency(float time) noexcept {
    return std::exp(7.15461540222168f * normalized(time)) * .0625f;
}
float externalRatio(float time) noexcept {
    return clockRatios[std::min(unsigned(8.25f * normalized(time)), 8u)];
}
float bufferFrequencyTarget(double seconds) noexcept {
    if (!std::isfinite(seconds)) seconds = .5;
    seconds = std::max(1. / rendererRate, std::min(double(maxCaptureFrames) / rendererRate, seconds));
    return float(coefficientRate / (rendererRate * seconds));
}

MappedControls ControlMapper::map(const PrimaryControls& p, const ControlVoltages& cv,
    const SecondarySettings& s, Mode mode, EffectiveFlags flags) noexcept {
    MappedControls out;
    out.repeatsExponent = std::min(unsigned(previousRepeats_ * 8.f), 8u);
    out.time = normalized(normalized(p.time) + finiteOrZero(cv.time) / 5.f);
    out.repeats = normalized(1.05f * normalized(p.repeats) + finiteOrZero(cv.repeats) / 5.f);
    previousRepeats_ = out.repeats;
    out.mixTarget = normalized(1.05f * normalized(p.mix) + finiteOrZero(cv.mix) / 5.f);
    if (out.mixTarget > .98f) out.mixTarget = 1.f;
    if (out.mixTarget < .03f) out.mixTarget = flags.freeze ? 1.f : 0.f;
    const float bend = normalized(normalized(p.bend) + normalized(s.bendDepth) * (finiteOrZero(cv.bend) / 5.f));
    const float brk = normalized(1.05f * normalized(p.brk) + normalized(s.breakDepth) * (finiteOrZero(cv.brk) / 5.f));
    out.corrupt = normalized(1.05f * normalized(p.corrupt) + normalized(s.corruptDepth) * (finiteOrZero(cv.corrupt) / 5.f));
    const float window = normalized(s.window);
    out.windowSquared = window * window;
    out.crossfeed = .5f * (1.f - normalized(s.separation));
    if (mode == Mode::Micro) {
        out.baseRate = microRate(p.bend, cv.bend, flags.bend, &out.octave);
        if (flags.brk) out.manualSilence = brk < .015f ? 0.f : brk;
        else out.traverse = brk;
    }
    else {
        out.macroBend = flags.bend ? bend : 0.f;
        out.macroBreak = flags.brk ? brk : 0.f;
    }
    return out;
}

ToneCoefficients outputToneCoefficients() noexcept {
    const float angle = (6.283185307179586f * 38000.f) / coefficientRate;
    const float b = 2.f - std::cos(angle);
    const float c = b - std::sqrt(std::fma(b, b, -1.f));
    return {1.f - c, c};
}

} // namespace tiamat
