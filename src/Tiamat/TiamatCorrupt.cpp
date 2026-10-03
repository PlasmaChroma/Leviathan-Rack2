#include "TiamatCorrupt.hpp"
#include "TiamatControls.hpp"
#include "TiamatCorruptTables.hpp"
#include "TiamatMath.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace tiamat {
namespace {
float clamp(float x, float a, float b) noexcept { return std::max(a, std::min(b, x)); }
// Defined modulo-2^32 multiplication followed by signed interpretation, without
// signed overflow or implementation-defined unsigned-to-signed conversion.
float bipolar(std::uint32_t bits) noexcept {
    const std::int64_t signedValue = bits < UINT32_C(0x80000000)
        ? std::int64_t(bits) : std::int64_t(bits) - INT64_C(4294967296);
    return float(double(signedValue) / 2147483648.0);
}
}

void SvfState::cutoff(float hz) noexcept {
    hz = clamp(hz, .000001f, coefficientRate / 3.f);
    frequency = 2.f * std::sin(float(3.141592653589793) * std::min(.25f, hz / (coefficientRate + coefficientRate)));
    // Resonance .7, drive zero. Constant fourth-root is evaluated at setup only.
    const float resonanceDamping = 2.f * (1.f - float(std::pow(.7f, .25)));
    damping = std::min(resonanceDamping, std::min(2.f, multiplyAdd(-.5f, frequency, 2.f / frequency)));
}
float SvfState::process(float x, bool highpass) noexcept {
    const float low1 = multiplyAdd(band, frequency, low);
    const float high1 = multiplyAdd(-damping, band, x) - low1;
    const float band1 = multiplyAdd(frequency, high1, band);
    low = multiplyAdd(frequency, band1, low1);
    const float high2 = multiplyAdd(-damping, band1, x) - low;
    band = multiplyAdd(frequency, high2, band1);
    const float result = highpass ? multiplyAdd(.5f, high2, .5f * high1) : multiplyAdd(.5f, low, .5f * low1);
    if (!std::isfinite(result) || !std::isfinite(band) || !std::isfinite(low)) {
        low = band = 0.f;
        return 0.f;
    }
    return result;
}
void AToneState::cutoff(float hz) noexcept {
    const float b = 2.f - std::cos((float(6.283185307179586) * hz) / coefficientRate);
    coefficient = b - std::sqrt(multiplyAdd(b, b, -1.f));
}
float AToneState::process(float x) noexcept {
    const float output = (x + previous) * coefficient;
    previous = output - x;
    if (!std::isfinite(output) || !std::isfinite(previous)) { previous = 0.f; return 0.f; }
    return output;
}

Corrupt::Corrupt(std::uint64_t seed) noexcept {
    for (auto& hp : vinyl_.noiseHighpass) hp.cutoff(3000.f);
    restartRandom(seed);
}
float Corrupt::dustRandom() noexcept {
    vinyl_.dustSeed = (UINT32_C(1103515245) * vinyl_.dustSeed + UINT32_C(12345)) & UINT32_C(0x7fffffff);
    return float(vinyl_.dustSeed) * (1.f / 2147483648.f);
}
void Corrupt::restartRandom(std::uint64_t seed) noexcept {
    vinyl_.dustSeed = std::uint32_t(seed) & UINT32_C(0x7fffffff);
    if (!vinyl_.dustSeed) vinyl_.dustSeed = 1;
    for (unsigned i = 0; i < 5; ++i) dustRandom();
    vinyl_.slowSeed = vinyl_.fastSeed = 1;
    vinyl_.slowCounter = vinyl_.modulationCounter = 0;
    vinyl_.modulationPeriod = 48014;
    vinyl_.modulation = vinyl_.slowValue = 0.f;
    for (auto& d : vinyl_.dust) { d.counter = 0; d.value = 0.f; }
}
void Corrupt::configure(Effect effect, float u) noexcept {
    if (configured_ == effect && amount_ == u) return;
    configured_ = effect;
    amount_ = u;
    switch (effect) {
    case Effect::Decimate: {
        const float doubled = u + u;
        const unsigned index = std::min(16u, unsigned(std::floor(double(std::fmod(doubled, 1.01f) * 16.f) + .5)));
        discardedBits_ = unsigned(16.f * decimateBits[index]);
        const float factor = decimateRates[index] * (doubled > 1.f ? 1.f : .25f);
        holdThreshold_ = unsigned((factor * factor) * 96.f);
        break;
    }
    case Effect::Destroy: {
        const float t = (u * u) + (u * u);
        exponent_ = std::max(2.f, multiplyAdd(normalized(t), 86.f, 1.f));
        drive_ = multiplyAdd(normalized(t - 1.f), 8.f, 1.f);
        attenuation_ = multiplyAdd(std::sin(u * float(1.5707963267948966)), -.375f, .5f);
        blendTarget_ = u >= .02f ? 1.f : 0.f;
        break;
    }
    case Effect::DjFilter: {
        const float lo = std::exp(multiplyAdd(normalized(u + u), 9.798127174377441f - 4.605170249938965f, 4.605170249938965f));
        const float hi = std::exp(multiplyAdd(normalized(multiplyAdd(u, 2.f, -1.f)), 8.006367683410645f - 3.2188758850097656f, 3.2188758850097656f));
        for (auto& f : lowpass_) f.cutoff(lo);
        for (auto& f : highpass_) f.cutoff(hi);
        break;
    }
    case Effect::Vinyl: {
        vinylEnabled_ = u > .05f;
        const float t = std::exp(std::max(u, .001f)) - 1.f;
        const float a = float(double(t) / 1.7183) * 1.25f;
        for (unsigned i = 0; i < 5; ++i) {
            auto& d = vinyl_.dust[i];
            d.threshold = ((i < 2 ? 500.f : 15.f) * a) * (1.f / coefficientRate);
            d.scale = d.threshold > 0.f ? 2.f / d.threshold : 0.f;
            d.amplitude = i < 2 ? multiplyAdd(a, .07f, .05f) : multiplyAdd(a, .2f, .5f);
        }
        slowAmplitude_ = a * .03f;
        fastAmplitude_ = a * .01f;
        for (auto& hp : vinyl_.signalHighpass) hp.cutoff(700.f * a);
        vinylGain_ = multiplyAdd(-a, .15f, 1.f);
        break;
    }
    default: break;
    }
}
float Corrupt::decimate(float x, unsigned channel) noexcept {
    auto& s = decimators_[channel];
    if (++s.counter > holdThreshold_) { s.counter = 0; s.held = x; }
    // Saturate only at the int32 conversion boundary (roughly +/-32768), not
    // the nominal audio range. Avoid undefined float conversion/negative shift.
    const double scaled = double(s.held * 65536.f);
    const std::int32_t q = scaled >= 2147483647.0 ? INT32_MAX
        : scaled <= -2147483648.0 ? INT32_MIN : std::int32_t(scaled);
    const std::uint32_t masked = std::uint32_t(q) & ~( (UINT32_C(1) << discardedBits_) - 1u );
    const std::int64_t signedValue = masked < UINT32_C(0x80000000) ? std::int64_t(masked) : std::int64_t(masked) - INT64_C(4294967296);
    return float(signedValue) / 65536.f;
}
float Corrupt::destroy(float x) const noexcept {
    // Retain exp for this reference-sensitive nonlinear transfer; coefficients
    // are block-cached. A replacement approximation requires its own A/B oracle.
    const float e = std::exp(-std::abs(x) * exponent_);
    const float curved = x > 0.f ? 1.f - e : e - 1.f;
    return clamp(drive_ * curved, -.6f, .6f) * attenuation_;
}
float Corrupt::dust(unsigned i) noexcept {
    auto& d = vinyl_.dust[i];
    if (++d.counter == (i < 2 ? 5u : 15u)) {
        d.counter = 0;
        const float r = dustRandom();
        // Original subtract/multiply promoted to double here.
        d.value = r < d.threshold ? float(double(d.amplitude) * (double(r * d.scale) - 1.0)) : 0.f;
    }
    return d.value;
}
void Corrupt::vinylFrame(float& left, float& right) noexcept {
    if (!vinylEnabled_) return;
    if (++vinyl_.modulationCounter == vinyl_.modulationPeriod) {
        vinyl_.modulationCounter = 0;
        vinyl_.modulation = bipolar(vinyl_.slowSeed) * .3f;
    }
    if (++vinyl_.slowCounter == 10) {
        vinyl_.slowCounter = 0;
        vinyl_.slowSeed *= UINT32_C(16807);
        vinyl_.slowValue = (bipolar(vinyl_.slowSeed) * slowAmplitude_) * vinyl_.modulation;
    }
    vinyl_.fastSeed *= UINT32_C(16807);
    const float hiss = bipolar(vinyl_.fastSeed) * fastAmplitude_;
    const float h0 = vinyl_.noiseHighpass[0].process((dust(0) + vinyl_.slowValue) + hiss);
    const float h1 = vinyl_.noiseHighpass[1].process((dust(1) + vinyl_.slowValue) + hiss);
    const float d3 = dust(3), d2 = dust(2), d4 = dust(4);
    const float n0 = (multiplyAdd(h1, .3f, h0) + d2) + d4;
    const float n1 = (multiplyAdd(h0, .3f, h1) + d3) + d4;
    left = multiplyAdd(n0, 1.2f, vinyl_.signalHighpass[0].process(left) * vinylGain_);
    right = multiplyAdd(n1, 1.2f, vinyl_.signalHighpass[1].process(right) * vinylGain_);
}
void Corrupt::processBlock(const float* x, float* y, CorruptRoutingState& route, Random& random) noexcept {
    processFrames(x, y, blockFrames, route, random);
}
void Corrupt::processFrames(const float* x, float* y, unsigned frames, CorruptRoutingState& route, Random& random) noexcept {
    const Effect effect = route.primary != Effect::Retained ? route.primary
        : route.retainedEnabled ? route.retained : Effect::Retained;
    const float u = normalized(route.primary != Effect::Retained ? route.amount : route.retainedAmount);
    configure(effect, u);
    if (effect == Effect::Dropout) {
        if (u <= .03f) route.dropoutOpen = true;
        // 0x08001e08 squares first; regrouping changes exact threshold decisions.
        else if (float(random.next() % 1024) < (u * u) * 376.f) route.dropoutOpen = !route.dropoutOpen;
    }
    for (unsigned frame = 0; frame < frames; ++frame) {
        float l = finiteOrZero(x[frame * 2]), r = finiteOrZero(x[frame * 2 + 1]);
        switch (effect) {
        case Effect::Decimate: l = decimate(l, 0); r = decimate(r, 1); break;
        case Effect::Dropout: if (!route.dropoutOpen) l = r = 0.f; break;
        case Effect::Destroy: {
            blend_ = multiplyAdd(blendTarget_ - blend_, .001f, blend_);
            const float dry = 1.f - blend_;
            l = multiplyAdd(l, dry, blend_ * destroy(l));
            r = multiplyAdd(r, dry, blend_ * destroy(r));
            break;
        }
        case Effect::DjFilter:
            l = std::tanh(highpass_[0].process(lowpass_[0].process(l, false), true));
            r = std::tanh(highpass_[1].process(lowpass_[1].process(r, false), true));
            break;
        case Effect::Vinyl: vinylFrame(l, r); break;
        default: break;
        }
        y[frame * 2] = finiteOrZero(l);
        y[frame * 2 + 1] = finiteOrZero(r);
    }
}
} // namespace tiamat
