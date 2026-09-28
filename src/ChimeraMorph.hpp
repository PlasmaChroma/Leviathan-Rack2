#pragma once
#include <cstdint>
#include <cstring>
#include "ChimeraFirmwareTables.hpp"

namespace chimera { namespace morph {
// Recovered MG204 ADC bins and nominal ratio labels. Density differs from
// launch rate below unity: stages 0..2 contain one voice followed by silence.
struct Stage { unsigned numerator, denominator, cycleDenominator, tableIndex; };
static constexpr Stage stages[22] = {
    {2,1,1,0}, {3,2,2,1}, {4,3,3,2}, {1,1,1,3}, {4,5,5,4}, {3,4,4,5},
    {2,3,3,6}, {3,5,5,7}, {4,7,7,8}, {1,2,2,9}, {4,9,9,10}, {3,7,7,11},
    {2,5,5,12}, {3,8,8,13}, {4,11,11,14}, {1,3,3,15}, {4,13,13,16},
    {3,10,10,17}, {2,7,7,18}, {3,11,11,19}, {4,15,15,20}, {1,4,4,21}
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
    return 1.0 / firmware::morph_launch[stageIndex(normalized)];
}
inline double density(float normalized) {
    return firmware::morph_density[stageIndex(normalized)];
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

// Preserve the extracted ROM interval rather than replacing it by a rational.
// Fractional phase carries across launches, avoiding per-hop rounding drift.
// Changing controls preserves normalized progress; voices are never reset.
class Scheduler {
    double phase_ = 0, threshold_ = 1;
public:
    void configure(std::uint32_t duration, const Stage& s) {
        const double threshold = double(duration ? duration : 1) * firmware::morph_launch[s.tableIndex];
        if (threshold != threshold_) {
            phase_ = phase_ * threshold / threshold_;
            threshold_ = threshold;
        }
    }
    void reset() { phase_ = 0; }
    bool step() {
        phase_ += 1.0;
        if (phase_ < threshold_) return false;
        phase_ -= threshold_;
        return true;
    }
};
} }
