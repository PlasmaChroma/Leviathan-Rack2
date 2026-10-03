#pragma once

#include "TiamatState.hpp"
#include "TiamatRandom.hpp"

namespace tiamat {

struct DecimateState { unsigned counter = 0; float held = 0.f; };
struct SvfState {
    float low = 0.f, band = 0.f, frequency = 0.f, damping = 0.f;
    void cutoff(float hz) noexcept;
    float process(float input, bool highpass) noexcept;
};
struct AToneState {
    float previous = 0.f, coefficient = .5f;
    void cutoff(float hz) noexcept;
    float process(float input) noexcept;
};
struct DustState {
    unsigned counter = 0;
    float value = 0.f, threshold = 0.f, scale = 0.f, amplitude = 0.f;
};
struct VinylState {
    std::array<DustState, 5> dust;
    std::array<AToneState, 2> noiseHighpass, signalHighpass;
    std::uint32_t dustSeed = 1, slowSeed = 1, fastSeed = 1;
    unsigned slowCounter = 0, modulationCounter = 0, modulationPeriod = 48014;
    float modulation = 0.f, slowValue = 0.f;
};

// Owned persistent states. Only the selected route advances; coefficients are
// cached by effect/amount and stochastic restart leaves filter history intact.
class Corrupt {
public:
    explicit Corrupt(std::uint64_t seed = 1) noexcept;
    void restartRandom(std::uint64_t seed) noexcept;
    void processBlock(const float* input, float* output, CorruptRoutingState&, Random&) noexcept;
    const VinylState& vinyl() const noexcept { return vinyl_; }
    float destroyBlend() const noexcept { return blend_; }
private:
    friend struct CorruptTestAccess;
    void processFrames(const float*, float*, unsigned, CorruptRoutingState&, Random&) noexcept;
    void configure(Effect, float) noexcept;
    float decimate(float, unsigned) noexcept;
    float destroy(float) const noexcept;
    float dust(unsigned) noexcept;
    float dustRandom() noexcept;
    void vinylFrame(float&, float&) noexcept;
    std::array<DecimateState, 2> decimators_;
    std::array<SvfState, 2> lowpass_, highpass_;
    VinylState vinyl_;
    Effect configured_ = Effect::Retained;
    float amount_ = -1.f, blend_ = 0.f;
    unsigned discardedBits_ = 0, holdThreshold_ = 0;
    float exponent_ = 2.f, drive_ = 1.f, attenuation_ = .5f, blendTarget_ = 0.f;
    float slowAmplitude_ = 0.f, fastAmplitude_ = 0.f, vinylGain_ = 1.f;
    bool vinylEnabled_ = false;
};

} // namespace tiamat
