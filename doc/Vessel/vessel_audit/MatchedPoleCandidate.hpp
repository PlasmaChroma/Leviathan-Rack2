#pragma once
#include <cmath>
namespace vessel_audit {
struct MatchedPoleCoefficients {
    double a=0.0, omega=0.0, sigma=0.0, inverseD=0.0, admittanceWeight=0.0;
};
// Algebraically equivalent to the zero-extra-damping transform, not bit-exact.
// Candidate only: must pass Vessel's ledger and long-decay tests before use.
inline bool matchedPoleCandidate(double frequency, double t60, double sampleRate,
                                 MatchedPoleCoefficients& out) noexcept {
    if (!std::isfinite(sampleRate) || sampleRate < 32000.0 || sampleRate > 4000000.0 ||
        !std::isfinite(frequency) || frequency <= 0.0 || frequency >= .4*sampleRate ||
        !std::isfinite(t60) || t60 < .1 || t60 > 120.0) return false;
    constexpr double pi=3.14159265358979323846;
    constexpr double log1000=6.9077552789821370521;
    const double h=1.0/sampleRate;
    const double d=-std::expm1(-log1000*h/t60);
    const double r=1.0-d;
    const double phase=pi*frequency*h;
    const double s=std::sin(phase), c=std::cos(phase);
    const double P=d*d+4.0*r*s*s;
    const double Q=d*d+4.0*r*c*c;
    out.a=std::sqrt(P/Q);
    out.omega=2.0*out.a/h;
    out.sigma=(2.0/h)*d*(2.0-d)/Q;
    out.inverseD=Q*.25;
    out.admittanceWeight=.5*h*out.inverseD;
    return std::isfinite(out.omega) && std::isfinite(out.sigma);
}
} // namespace vessel_audit
