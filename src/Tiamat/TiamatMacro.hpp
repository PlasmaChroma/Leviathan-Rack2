#pragma once

#include "TiamatControls.hpp"
#include "TiamatMath.hpp"

namespace tiamat {

// The raw helper always consumes its first draw; the capture scheduler applies
// the <=.03 bypass before calling it. Keep that distinction for direct fixtures.
template <class Rng>
void chooseBend(MacroState& state, float amount, Rng& rng) noexcept {
    const float b = normalized(amount);
    const unsigned index = unsigned(rng.unit255() * b * 1.5f * 9.f);
    state.rate = index < bendRates.size() ? bendRates[index] : 1.5f;
    if (b > .667f) {
        if (rng.unit255() <= .25f) return; // sticky previous slew
        const unsigned modulus = unsigned(256.0 + (double(b) - .667) * 3.0 * 384.0);
        const unsigned value = rng.next() % modulus;
        state.slew = value < 128 ? 1.f : (value < 256 ? .005f : .0001f);
    }
    else state.slew = 1.f;
}

template <class Rng>
void chooseBreak(MacroState& state, const MacroState& left, bool copyLeft,
    float amount, Rng& rng) noexcept {
    if (copyLeft) {
        state.extraExponent = left.extraExponent;
        state.silence = left.silence;
        state.randomPosition = left.randomPosition;
        return;
    }
    const float b = normalized(amount);
    state.extraExponent = b > .03f ? unsigned(rng.unit255() * b * 8.f) : 0;
    state.silence = 0.f;
    if (b > .5f) {
        const float u = rng.unit255();
        const unsigned q = unsigned(4.f * normalized(multiplyAdd(2.f, b, -1.f)) + .5f);
        state.silence = breakSilences[unsigned(u * float(q))];
    }
    state.randomPosition = b > .33f ? rng.unit255() : 0.f;
}

template <class Rng>
void captureDecisions(MacroState& state, const MacroState& left, bool copyLeft,
    float bend, float brk, Rng& rng) noexcept {
    // Original helper order is Break then Bend.
    chooseBreak(state, left, copyLeft, brk, rng);
    if (copyLeft) { state.rate = left.rate; state.slew = left.slew; }
    else if (bend <= .03f) { state.rate = 1.f; state.slew = 1.f; }
    else chooseBend(state, bend, rng);
}

template <class Rng>
void wrapDecision(MacroState& state, const MacroState& left, bool copyLeft,
    float amount, Rng& rng) noexcept {
    if (copyLeft) state.randomPosition = left.randomPosition;
    else if (rng.unit255() > .75f && amount > .2f) state.randomPosition = rng.unit255();
}

template <class Rng>
void retainedCorruptDecision(CorruptRoutingState& state, float amount, Rng& rng) noexcept {
    state.retained = static_cast<Effect>(rng.next() % 3 + 1);
    const unsigned modulus = unsigned(256.f * normalized(amount));
    const auto draw = rng.next(); // still consumed at zero and while inaudible
    state.retainedAmount = modulus ? float(draw % modulus) / 255.f : 0.f;
}

} // namespace tiamat
