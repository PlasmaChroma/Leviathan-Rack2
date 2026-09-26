#pragma once
#include <cstdint>
#include <cstring>

namespace chimera { namespace morph {
// Recovered MG204 ADC bins and rational launch factors. Density differs from
// launch rate below unity: stages 0..2 contain one voice followed by silence.
struct Stage { unsigned numerator, denominator, cycleDenominator; };
static constexpr Stage stages[22] = {
    {2,1,1}, {3,2,2}, {4,3,3}, {1,1,1}, {4,5,5}, {3,4,4},
    {2,3,3}, {3,5,5}, {4,7,7}, {1,2,2}, {4,9,9}, {3,7,7},
    {2,5,5}, {3,8,8}, {4,11,11}, {1,3,3}, {4,13,13},
    {3,10,10}, {2,7,7}, {3,11,11}, {4,15,15}, {1,4,4}
};
inline int adc(float normalized) {
    std::uint32_t bits;
    std::memcpy(&bits, &normalized, sizeof(bits));
    if ((bits & 0x7f800000u) == 0x7f800000u || normalized <= 0.f) return 0;
    if (normalized >= 1.f) return 4095;
    return static_cast<int>(normalized * 4096.f);
}
inline int stageIndex(float normalized) {
    const int index = static_cast<int>((adc(normalized) / 4096.f) * 33.99f);
    return index < 21 ? index : 21;
}
inline const Stage& stage(float normalized) { return stages[stageIndex(normalized)]; }
inline double launchRate(float normalized) {
    const Stage& s = stage(normalized);
    return double(s.denominator) / s.numerator;
}
inline double density(float normalized) {
    const double rate = launchRate(normalized);
    return rate < 1.0 ? 1.0 : rate;
}
// Recovered clock integration boundary; complete ckop behavior is provisional.
inline bool stretchCandidate(float normalized) {
    const Stage& s = stage(normalized);
    return 2 * s.numerator <= s.denominator;
}
struct Continuous {
    float value = 0.f;
    float step(float normalized) {
        const float target = adc(normalized) / 4096.f;
        value += (target - value) * 0.01f; // Recovered audio-rate smoothing.
        return value;
    }
};
// Provisional depth curves. Thresholds are recovered; distribution is not.
inline double depth(double value, double threshold) {
    const double d = (value - threshold) / (1.0 - threshold);
    return d <= 0 ? 0 : (d >= 1 ? 1 : d);
}
inline double panDepth(double value) { return depth(value, 0.5); }
inline double pitchDepth(double value) { return depth(value, 0.6); }

// Exact rational intervals for a constant stage/duration. The remainder spans
// the cycle denominator, avoiding independently rounded launch intervals.
// Changing controls preserves normalized progress; voices are never reset.
class Scheduler {
    std::uint64_t phase_ = 0, threshold_ = 1;
    unsigned increment_ = 1;
public:
    void configure(std::uint32_t duration, const Stage& s) {
        const std::uint64_t threshold = std::uint64_t(duration ? duration : 1) * s.numerator;
        if (threshold != threshold_) {
            phase_ = phase_ * threshold / threshold_;
            threshold_ = threshold;
        }
        increment_ = s.denominator;
    }
    void reset() { phase_ = 0; }
    bool step() {
        phase_ += increment_;
        if (phase_ < threshold_) return false;
        phase_ -= threshold_;
        return true;
    }
};
} }
