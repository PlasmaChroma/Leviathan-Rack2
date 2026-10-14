#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "ChimeraFirmwareTables.hpp"

namespace chimera { namespace geneSize {

// MG204 normal Gene Size path. Whole-splice is a semantic traversal mode:
// its boundaries cover the full selected splice, regardless of the folded LUT.
inline int adc(float control) {
    std::uint32_t bits;
    std::memcpy(&bits, &control, sizeof(bits));
    if ((bits & 0x7f800000u) == 0x7f800000u) control = 0.f;
    if (control <= 0.f) return 0;
    if (control >= 1.f) return 4095;
    return static_cast<int>(std::lround(control * 4095.f));
}
inline bool wholeSplice(int code) { return code <= 199; }

inline const std::array<float, 1024>& lut() {
    return firmware::gene_size_exp;
}
inline float foldedBase(std::uint32_t spliceSamples) {
    float base = static_cast<float>(spliceSamples);
    while (base > 576000.f) base *= 0.5f;
    return base;
}
inline float quantize(float duration, std::uint32_t spliceSamples) {
    if (!spliceSamples) return 8.f;
    if (duration <= 8.f) return duration;
    const std::uint32_t bits = 0x3f2aaa9fU;
    float twoThirds;
    std::memcpy(&twoThirds, &bits, sizeof(twoThirds));
    const float span = float(spliceSamples);
    float current = twoThirds * span;
    if (duration >= current) return span;
    bool threeQuarters = true;
    for (;;) {
        const float next = current * (threeQuarters ? 0.75f : twoThirds);
        if (duration >= next) return current;
        current = next;
        threeQuarters = !threeQuarters;
    }
}
struct Mapping {
    bool wholeSpliceMode;
    float durationSamples; // Output samples in ordinary mode; source span in whole mode.
};
inline Mapping mapAdc(std::uint32_t spliceSamples, int code, bool validClock = false) {
    code = code < 0 ? 0 : (code > 4095 ? 4095 : code);
    if (wholeSplice(code)) return {true, float(spliceSamples)};
    const float value = lut()[1073 - (code >> 2)];
    const float square = value * value;
    const float cube = square * value;
    float duration = cube * foldedBase(spliceSamples);
    if (duration < 8.f) duration = 8.f;
    if (validClock) duration = quantize(duration, spliceSamples);
    return {false, duration};
}
inline Mapping map(std::uint32_t spliceSamples, float control, bool validClock = false) {
    return mapAdc(spliceSamples, adc(control), validClock);
}

} } // namespace chimera::geneSize
