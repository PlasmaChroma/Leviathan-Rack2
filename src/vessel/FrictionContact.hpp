#pragma once

#include "ModalBank.hpp"

namespace vessel {

struct FrictionValue {
    double force = 0.0;
    double derivative = 0.0;
};
struct FrictionSolution {
    double force = 0.0;
    double slip = 0.0;
    double derivative = 0.0;
    double residual = 0.0;
    unsigned iterations = 0;
    bool converged = false;
};

// Analytic reference curve. Saturated tanh/Gaussian tails skip expensive math;
// Production solveFriction uses PreparedFrictionLaw: analytic Gaussian and
// monotone cubic tanh with a derivative from the same interpolant.
FrictionValue frictionValue(double slip, double load, const MalletDescriptor& mallet) noexcept;
double frictionNegativeSlopeBound(double load, const MalletDescriptor& mallet) noexcept;
FrictionSolution solveFriction(double delta, double admittance, double load,
                               const MalletDescriptor& mallet, double previousForce = 0.0) noexcept;

// Persistent orbit: two half-angle rotations give midpoint ports and endpoint
// state. Small audio-rate increments use matched polynomial sine/cosine;
// periodic normalization bounds roundoff without audio-rate transcendental work.
class ContactOrbit {
public:
    void configure(const BowlDescriptor& bowl, const ModalBank& bank,
                   double patchWidth, double angle) noexcept;
    void midpoint(double angleIncrement, ModalVector& tangent, ModalVector& inward,
                  bool includeNormal = true) noexcept;
    void finish() noexcept;
    // Configuration-only footprint lookup, independent of pitch and angle.
    double patchFactor(std::size_t pair, int order, double width) const noexcept;
private:
    std::size_t pairs_ = 0;
    unsigned ticks_ = 0;
    std::array<int, maxPairs> orders_ {};
    std::array<double, maxPairs> cosine_ {}, sine_ {}, dc_ {}, ds_ {};
    ModalVector gains_ {};
    ModalVector tangentGains_ {};
    double cachedIncrement_ = 0.0;
    bool incrementCached_ = false;
    std::array<double, maxPairs> patches_ {};
    double cachedWidth_ = -1.0;
    void rotate() noexcept;
};

} // namespace vessel
