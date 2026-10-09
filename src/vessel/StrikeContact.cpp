#include "StrikeContact.hpp"
#include "StrikeGradient.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {

bool validMallet(const MalletDescriptor& m) noexcept {
    return std::isfinite(m.mass) && m.mass > 0.0
        && std::isfinite(m.stiffness) && m.stiffness > 0.0
        && m.exponent == 1.5 && std::isfinite(m.loadingDamping) && m.loadingDamping >= 0.0
        && std::isfinite(m.patchWidth) && m.patchWidth >= 0.0 && m.patchWidth <= 2.0*pi
        && std::isfinite(m.muS) && std::isfinite(m.muK) && m.muS >= m.muK && m.muK >= 0.0
        && std::isfinite(m.weakeningVelocity) && m.weakeningVelocity > 0.0
        && std::isfinite(m.regularizationVelocity) && m.regularizationVelocity > 0.0;
}

double strikePotential(double d, double k) noexcept {
    return d > 0.0 ? 0.4*k*d*d*std::sqrt(d) : 0.0;
}
double strikeDiscreteGradient(double before, double after, double k) noexcept {
    if (before <= 0.0 && after <= 0.0) return 0.0;
    if (before < 0.0 || after < 0.0)
        return (strikePotential(after, k)-strikePotential(before, k))/(after-before);
    // Factor (b^5-a^5)/(b^2-a^2) analytically: no subtractive cancellation
    // even at equal compression or over a tiny finite increment.
    const double a = std::sqrt(before), b = std::sqrt(after);
    if (a+b == 0.0) return 0.0;
    return 0.4*k*(after*after + after*b*a + after*before + b*a*before + before*before)/(a+b);
}

StrikeSolution solveStrike(double d0, double velocity, double freeVelocity,
                           double Yss, double h, const MalletDescriptor& m) noexcept {
    StrikeSolution result;
    if (!validMallet(m) || !std::isfinite(d0) || d0 < 0.0
        || !std::isfinite(velocity) || !std::isfinite(freeVelocity)
        || !std::isfinite(Yss) || Yss < 0.0 || !std::isfinite(h) || h <= 0.0) return result;
    const double A = Yss + h/(2.0*m.mass);
    const double freeCompression = d0 + h*(velocity-freeVelocity);
    const StrikeGradient gradient(d0, m.stiffness);
    StrikeGradientValue elastic;
    auto trial = [&](double force) {
        StrikeSolution s;
        s.force = force;
        s.compression = freeCompression-h*A*force;
        s.compressionVelocity = (s.compression-d0)/h;
        s.dampingForce = std::max(d0, s.compression) > 0.0
            ? m.loadingDamping*std::max(s.compressionVelocity, 0.0) : 0.0;
        elastic = gradient.evaluate(s.compression);
        s.residual = force-elastic.force-s.dampingForce;
        return s;
    };
    const auto atZero = trial(0.0);
    if (!std::isfinite(atZero.residual)) return result;
    // In the isolated impact problem contact force decreases as trial force
    // reduces compression. The zero-trial constitutive force is an upper bound.
    double lo = 0.0, hi = std::max(0.0, -atZero.residual);
    double force = 0.5*hi;
    for (unsigned iteration = 0; iteration < 80; ++iteration) {
        result = trial(force);
        result.iterations = iteration+1;
        if (!std::isfinite(result.residual) || !std::isfinite(result.compression)) return result;
        const double tolerance = 1e-12 + 1e-11*std::max(1.0, hi);
        if (std::abs(result.residual) <= tolerance) {
            result.converged = true;
            return result;
        }
        if (result.residual > 0.0) hi = force;
        else lo = force;
        double next = 0.5*(lo+hi);
        if (iteration < 16) {
            const double elasticDerivative = gradient.derivative(result.compression, elastic);
            const double dampingDerivative = result.compression > d0 ? m.loadingDamping/h : 0.0;
            const double derivative = 1.0+h*A*(std::max(0.0, elasticDerivative)+dampingDerivative);
            const double candidate = force-result.residual/derivative;
            if (std::isfinite(candidate) && candidate > lo && candidate < hi) next = candidate;
        }
        force = next;
    }
    return result;
}

} // namespace vessel
