#include "TiamatBuffer.hpp"
#include "TiamatMath.hpp"
#include "TiamatMacro.hpp"
#include <algorithm>
#include <cmath>

namespace tiamat {
namespace {
bool crossing(float a, float b) noexcept {
    return (a >= 0.f && b <= 0.f) || (a <= 0.f && b >= 0.f);
}
unsigned boundedLength(float value, unsigned maximum) noexcept {
    return unsigned(std::max(1.f, std::min(float(maximum), value)));
}
}

ReaderResult readLinear(const float* plane, unsigned capacity, ReaderState& r,
    float baseRate, float macroRate, float slew, bool frozen,
    unsigned writeBank, unsigned captureFrames) noexcept {
    if (!capacity) return {0.f, false};
    // Anchors may survive resizing. Bound both indexes, including ==capacity,
    // without relocating the anchor or carrying playback overshoot through.
    const float absolute = float(r.anchor % capacity) + std::max(0.f, finiteOrZero(r.position));
    const unsigned integer = unsigned(std::min(absolute, float(capacity) * 2.f));
    const unsigned index = integer % capacity;
    const unsigned next = index + 1 == capacity ? 0 : index + 1;
    const float first = plane[index];
    const float sample = multiplyAdd(absolute - float(integer), plane[next] - first, first);
    const float target = std::max(-8.f, std::min(8.f, finiteOrZero(baseRate * macroRate)));
    r.rate = multiplyAdd(normalized(slew), target - finiteOrZero(r.rate), finiteOrZero(r.rate));
    r.position = finiteOrZero(r.position) + r.rate;
    const unsigned start = std::min(r.sliceStart, capacity - 1);
    const unsigned frames = std::max(1u, std::min(r.sliceFrames, capacity - start));
    const float end = float(start + frames - 1);
    bool wrapped = false;
    if (r.position < float(start)) { r.position = end; wrapped = true; }
    else if (r.position > end) { r.position = float(start); wrapped = true; }
    if (wrapped && !frozen)
        r.anchor = r.rate > 0.f ? writeBank : (writeBank ? 0 : captureFrames);
    return {finiteOrZero(sample), wrapped};
}

float sliceWindow(unsigned audible, unsigned phase, float w) noexcept {
    if (!audible || phase >= audible) return 0.f;
    if (w <= .015f) return 1.f;
    unsigned fade = std::max(unsigned(float(audible) * normalized(w) * .5f), 24u);
    if (audible < 24) fade = std::min(fade, std::max(1u, audible / 2));
    if (phase < fade || phase > audible - fade) {
        if (phase < audible / 2) return normalized(float(phase) / float(fade));
        return normalized(float(audible - phase) / float(fade));
    }
    return 1.f;
}
float captureWindow(unsigned frames, unsigned phase) noexcept {
    const float z = float(phase);
    if (z < 240.f) return z / 240.f;
    if (z > float(frames) - 480.f)
        return normalized((float(frames) - (z + 240.f)) / 240.f);
    return 1.f;
}
unsigned traverseIndex(unsigned divisions, float amount, unsigned previous) noexcept {
    if (divisions <= 1) return 0;
    const float q = float(divisions - 1) * normalized(amount);
    const float h = divisions <= 4 ? .1f : .3f;
    return std::min(divisions - 1, unsigned(std::max(0.f, (q + (q > float(previous) ? -h : h)) + .5f)));
}

Buffer::Buffer(unsigned capacity, float coefficients, unsigned initialFrames)
    : capacity_(std::max(2u, std::min(capacity, planeFrames))),
      coefficients_(std::isfinite(coefficients) && coefficients > 0.f ? coefficients : coefficientRate),
      frequency_(coefficients_ / float(std::max(1u, std::min(initialFrames, capacity_ / 2)))),
      inverseFrequency_(1.f / frequency_), memory_(new float[std::size_t(capacity_) * 2]()) {
    const unsigned n = std::max(1u, std::min(initialFrames, capacity_ / 2));
    for (auto& ch : channels_) {
        ch.writer.captureFrames = n;
        ch.reader.sliceFrames = n;
        ch.audibleFrames = ch.audibleEnd = n;
    }
}

void Buffer::changeBank(BufferChannel& ch) noexcept {
    if (ch.sampleCounter - ch.lastBankChange > ch.writer.captureFrames / 4) {
        ch.lastBankChange = ch.sampleCounter;
        ch.writer.writeBank = ch.writer.writeBank ? 0 : ch.writer.captureFrames;
    }
}
void Buffer::write(unsigned channel, float input, bool timeChanging) noexcept {
    auto& ch = channels_[channel];
    auto& w = ch.writer;
    // During Time instability the original writer emits at the old phase once
    // before wrapping to the newly accepted extent. The bounded plane address
    // keeps this safe without dropping that last history/capture sample.
    if (!timeChanging && w.writePosition >= w.captureFrames) w.writePosition = 0;
    if (!transition_.freezeActive)
        memory_[std::size_t(channel) * capacity_ + (w.writeBank + w.writePosition) % capacity_] = input;
    ++w.writePosition;
    if (timeChanging) {
        ch.writerCycle = false;
        if (w.writePosition >= w.captureFrames) w.writePosition = 0;
        return;
    }
    if (w.writePosition < w.captureFrames) return;
    w.writePosition = 0;
    if (ch.cyclesRemaining) {
        --ch.cyclesRemaining;
        transition_.pendingAcceptance[channel] = true;
    }
    ch.writerCycle = true;
    if (!transition_.freezeActive) changeBank(ch);
    ch.rerollPending = true;
}
void Buffer::history(unsigned channel, float input) noexcept {
    auto& w = channels_[channel].writer;
    const unsigned boundary = std::min(capacity_, w.captureFrames * 2);
    // No free history at maximum capture: never write one past the plane.
    if (boundary == capacity_) return;
    if (w.historyPosition <= boundary || w.historyPosition >= capacity_)
        w.historyPosition = boundary;
    memory_[std::size_t(channel) * capacity_ + w.historyPosition++] = input;
}

void Buffer::processBlock(const float* input, float* output,
    const BufferControls& controls, Random& rng) noexcept {
    processFrames(input, output, blockFrames, controls, rng);
}

void Buffer::processFrames(const float* input, float* output, unsigned frames,
    const BufferControls& controls, Random& rng) noexcept {
    const float targetFrequency = std::max(coefficients_ / float(capacity_ / 2),
        std::min(coefficients_, finiteOrZero(controls.frequency)));
    transition_.freezeRequested = controls.freezeRequested;
    if (controls.momentaryFreeze) transition_.freezeActive = controls.freezeRequested;
    transitionCycles_ = controls.transitionCycles;
    if (controls.timeChanging) {
        channels_[0].resizePending = channels_[1].resizePending = true;
        transition_.clockPending = false;
    }
    else if (controls.clockRequest) {
        transition_.pendingAcceptance = {{true, true}};
        transition_.guard = controls.transitionGuard;
    }
    // Input is read only at the corresponding channel/frame. The complete left
    // pass owns the Time smoother; right uses its final value for this block.
    for (unsigned channel = 0; channel < 2; ++channel) {
        auto& ch = channels_[channel];
        auto& r = ch.reader;
        auto& w = ch.writer;
        for (unsigned frame = 0; frame < frames; ++frame) {
            if (!channel) {
                frequency_ = multiplyAdd(.001f, targetFrequency - frequency_, frequency_);
                inverseFrequency_ = 1.f / frequency_;
            }
            const unsigned requested = boundedLength(coefficients_ / frequency_, capacity_ / 2);
            if (r.position < float(r.sliceStart) || r.position > float(r.sliceStart + r.sliceFrames - 1))
                r.position = r.rate > 0.f ? float(r.sliceStart) : float(ch.audibleEnd ? ch.audibleEnd - 1 : r.sliceStart);
            if (ch.resizePending) {
                if (crossing(r.previousSample, ch.priorSample)) w.captureFrames = requested;
                ch.writerCycle = false;
            }
            else w.captureFrames = requested;
            const float duration = coefficients_ * inverseFrequency_;
            ch.audibleFrames = unsigned((1.f - normalized(controls.silence + ch.macro.silence)) * float(r.sliceFrames));
            if (float(ch.candidate) > duration - float(r.sliceFrames)) ch.candidate = 0;
            const float x = finiteOrZero(input[2 * frame + channel]);
            ReaderResult result;
            if (r.rate > 0.f) {
                write(channel, x, controls.timeChanging);
                result = readLinear(memory_.get() + std::size_t(channel) * capacity_, capacity_, r,
                    controls.baseRate, ch.macro.rate, std::min(controls.rateSlew, ch.macro.slew),
                    transition_.freezeActive, w.writeBank, w.captureFrames);
            }
            else {
                result = readLinear(memory_.get() + std::size_t(channel) * capacity_, capacity_, r,
                    controls.baseRate, ch.macro.rate, std::min(controls.rateSlew, ch.macro.slew),
                    transition_.freezeActive, w.writeBank, w.captureFrames);
                write(channel, x, controls.timeChanging);
            }
            history(channel, x);
            const float raw = ch.audibleFrames && ch.audibleEnd && r.position >= float(r.sliceStart)
                && r.position <= float(ch.audibleEnd - 1) ? result.sample : 0.f;
            float y = raw;
            if (controls.window > .015f) {
                const unsigned phase = unsigned(std::max(0.f, r.position - float(r.sliceStart)));
                y *= sliceWindow(ch.audibleFrames, phase, controls.window);
                y *= captureWindow(w.captureFrames, w.writePosition);
            }
            output[2 * frame + channel] = finiteOrZero(y);
            ch.priorSample = r.previousSample;
            r.previousSample = raw;
            if (result.wrapped) ++ch.wraps;
            const bool crossed = crossing(raw, ch.priorSample);
            if (transition_.pendingAcceptance[channel] && (crossed || (result.wrapped && !transition_.guard))) {
                transition_.guard = 0;
                if (ch.resizePending) { w.captureFrames = requested; ch.resizePending = false; }
                w.writePosition = 0;
                r.position = r.rate > 0.f ? float(r.sliceStart) : float(ch.audibleEnd ? ch.audibleEnd - 1 : r.sliceStart);
                ch.wraps = 0;
                transition_.pendingAcceptance[channel] = false;
                ch.writerCycle = false;
                ch.rerollPending = true;
                ch.cyclesRemaining = transitionCycles_;
                transition_.freezeActive = transition_.freezeRequested;
                if (!transition_.freezeActive && !controls.timeChanging) {
                    changeBank(ch);
                    r.anchor = r.rate > 0.f ? w.writeBank : (w.writeBank ? 0 : w.captureFrames);
                }
            }
            if (!channel) microIndex_ = traverseIndex(channels_[0].subdivisions, controls.traverse, microIndex_);
            if (result.wrapped) {
                if (ch.subdivisions > 1)
                    wrapDecision(ch.macro, channels_[0].macro, channel && !controls.unique, controls.macroBreak, rng);
                const unsigned index = std::min(ch.subdivisions - 1,
                    microIndex_ + unsigned(float(ch.subdivisions) * ch.macro.randomPosition));
                const unsigned available = w.captureFrames > r.sliceFrames ? w.captureFrames - r.sliceFrames : 0;
                ch.candidate = std::min(available, r.sliceFrames * index);
            }
            if (channel && transition_.guard) --transition_.guard;
            if (ch.rerollPending) {
                captureDecisions(ch.macro, channels_[0].macro, channel && !controls.unique,
                    controls.macroBend, controls.macroBreak, rng);
                ch.rerollPending = false;
            }
            if (crossed || result.wrapped) {
                ch.subdivisions = 1u << std::min(std::min(controls.repeatsExponent, 8u) + ch.macro.extraExponent, 8u);
                r.sliceFrames = boundedLength(duration / float(ch.subdivisions) + 1.f, w.captureFrames);
                r.sliceStart = std::min(ch.candidate, capacity_ - r.sliceFrames);
                ch.audibleEnd = std::min(capacity_, r.sliceStart + ch.audibleFrames);
            }
            ++ch.sampleCounter;
#ifdef TIAMAT_BUFFER_TEST_HOOKS
            if (observer_) observer_(*this, channel, observerData_);
#endif
        }
    }
}

} // namespace tiamat
