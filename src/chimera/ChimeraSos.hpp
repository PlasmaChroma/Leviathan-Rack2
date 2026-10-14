#pragma once
#include "ChimeraTypes.hpp"
#include <cstdint>
#include <cstring>

namespace chimera { namespace soundOnSound {
// Recovered MG204 control calibration. Quantize normalized control to the
// same 12-bit range as the hardware before applying its endpoint margins.
inline int adc(float normalized) {
    std::uint32_t bits;
    std::memcpy(&bits, &normalized, sizeof(bits));
    if ((bits & 0x7f800000u) == 0x7f800000u || normalized <= 0.f) return 0;
    if (normalized >= 1.f) return 4095;
    return static_cast<int>(normalized * 4096.f);
}
inline float targetAdc(int code) {
    if (code <= 185) return 0.f;
    if (code >= 3981) return 1.f;
    return code * 0.0002635046258f - 0.04884000123f;
}
inline float target(float normalized) { return targetAdc(adc(normalized)); }
inline StereoFrame mix(StereoFrame live, StereoFrame playback, float coefficient) {
    return {live.l + coefficient * (playback.l - live.l),
            live.r + coefficient * (playback.r - live.r)};
}
} }
