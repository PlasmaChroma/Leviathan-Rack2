#pragma once

#include <cstdint>
#include <cstring>

namespace levi_math {
// Scalar range-reduced audio math, independent of Rack/SIMD headers. Uses the
// polynomial/exponent-splitting approach already used by Phonex and Rack's
// exp2_taylor5, with a centered interval and extra terms for integer-frame Gene
// timing. No tables, allocation, lazy initialization, or libm calls.
// Preconditions: finite x in [-1022, 1022]. Exact at integer octaves.
inline double exp2Audio(double x) {
    const int exponent = int(x >= 0 ? x + 0.5 : x - 0.5);
    const double y = (x-exponent) * 0.69314718055994530942;
    const std::uint64_t bits = std::uint64_t(exponent+1023) << 52;
    double scale;
    std::memcpy(&scale, &bits, sizeof(scale));
    const double polynomial = 1.0 + y*(1.0 + y*(0.5 + y*(1.0/6 + y*(1.0/24 +
        y*(1.0/120 + y*(1.0/720 + y*(1.0/5040 + y*(1.0/40320 +
        y*(1.0/362880 + y*(1.0/3628800))))))))));
    return scale*polynomial;
}

// Preconditions: positive, finite, normal double. Normalize to [1/sqrt(2),
// sqrt(2)] so the atanh series has |y| <= 0.172, unlike a linear bit-log estimate.
inline double log2Audio(double x) {
    std::uint64_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    int exponent = int(bits >> 52) - 1023;
    bits = (bits & UINT64_C(0x000fffffffffffff)) | UINT64_C(0x3ff0000000000000);
    double m;
    std::memcpy(&m, &bits, sizeof(m));
    if (m > 1.4142135623730950488) { m *= 0.5; ++exponent; }
    const double y = (m-1.0)/(m+1.0), square = y*y;
    return exponent + 2.8853900817779268147*y*(1.0 + square*(1.0/3 +
        square*(1.0/5 + square*(1.0/7 + square*(1.0/9 +
        square*(1.0/11 + square*(1.0/13)))))));
}
// Reuse Andy Simper's coefficients from Rack's exp2_taylor5 (also used by
// Flux, Proc, Bifurx and Wyrm). Scalar double exponent splitting avoids Rack's
// float offset rounding. <0.001 cent across Chimera's pitch/rate domain.
// Preconditions: finite x in [-1022, 1022], as for exp2Audio().
inline double exp2Pitch(double x) {
    int exponent = int(x);
    if (x < exponent) --exponent;
    const double f = x-exponent;
    const std::uint64_t bits = std::uint64_t(exponent+1023) << 52;
    double scale;
    std::memcpy(&scale, &bits, sizeof(scale));
    return scale*(1.0 + f*(0.69315169353961 + f*(0.2401595990753 +
        f*(0.055817908652 + f*(0.008991698010 + f*0.001879100722)))));
}
inline double powUnitAudio(double x, double exponent) {
    return x <= 0 ? 0.0 : exp2Pitch(log2Audio(x)*exponent);
}
}
