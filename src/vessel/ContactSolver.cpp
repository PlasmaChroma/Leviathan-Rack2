#include "ContactSolver.hpp"
#include "ContactWorkProfile.hpp"
#include <algorithm>
#include <cmath>

namespace vessel {
CoupledSolution solveContacts(double d0, double velocity, double vs0, double vt0,
    double Yss, double Yst, double Ytt, double h,
    const MalletDescriptor& striker, const MalletDescriptor& rubber,
    double speed, double load, double previous) noexcept {
    VESSEL_WORK(ContactWorkScope profileScope);
    CoupledSolution result;
    if (!validMallet(striker) || !std::isfinite(d0) || d0 < 0.0
        || !std::isfinite(velocity) || !std::isfinite(vs0) || !std::isfinite(vt0)
        || !std::isfinite(Yss) || Yss < 0.0 || !std::isfinite(Yst)
        || !std::isfinite(Ytt) || Ytt < 0.0 || !std::isfinite(h) || h <= 0.0
        || Yst*Yst > Yss*Ytt+1e-14*std::max(1e-30, Yss*Ytt)) return result;
    const double A = Yss+h/(2.0*striker.mass);
    const double freeCompression = d0+h*(velocity-vs0);
    unsigned innerIterations = 0;
    auto trial = [&](double force) {
        VESSEL_WORK(++contactWork().outerTrials);
        CoupledSolution s;
        s.friction = solveFriction(speed-vt0-Yst*force, Ytt, load, rubber, previous);
        innerIterations = std::max(innerIterations, s.friction.iterations);
        if (!s.friction.converged) return s;
        s.strike.force = force;
        s.strike.compression = freeCompression-h*(A*force+Yst*s.friction.force);
        s.strike.compressionVelocity = (s.strike.compression-d0)/h;
        s.strike.dampingForce = std::max(d0, s.strike.compression) > 0.0
            ? striker.loadingDamping*std::max(s.strike.compressionVelocity, 0.0) : 0.0;
        s.strike.residual = force-strikeDiscreteGradient(d0, s.strike.compression, striker.stiffness)-s.strike.dampingForce;
        return s;
    };
    auto zero = trial(0.0);
    if (!zero.friction.converged || !std::isfinite(zero.strike.residual)) return result;
    if (zero.strike.residual == 0.0) {
        zero.strike.converged = zero.converged = true;
        zero.innerIterations = innerIterations;
        return zero;
    }
    double lo = 0.0, hi = std::max(1e-6, -zero.strike.residual);
    auto upper = trial(hi);
    for (unsigned i = 0; i < 24 && upper.friction.converged && upper.strike.residual < 0.0; ++i) {
        VESSEL_WORK(++contactWork().bracketExpansions);
        hi *= 2.0; upper = trial(hi);
    }
    if (!upper.friction.converged || !std::isfinite(upper.strike.residual) || upper.strike.residual < 0.0) return result;
    double force = 0.5*hi;
    const double scale = hi;
    for (unsigned i = 0; i < 80; ++i) {
        result = trial(force);
        result.innerIterations = innerIterations;
        result.strike.iterations = i+1;
        if (!result.friction.converged || !std::isfinite(result.strike.residual)) return result;
        if (std::abs(result.strike.residual) <= 1e-12+1e-11*std::max(1.0, scale)) {
            result.strike.converged = result.converged = true;
            return result;
        }
        if (result.strike.residual > 0.0) hi = force;
        else lo = force;
        const double d1 = result.strike.compression, delta = d1-d0;
        double elasticDerivative;
        if (std::abs(delta) < 1e-6*std::max(1e-12, std::max(d0, d1)))
            elasticDerivative = 0.75*striker.stiffness*std::sqrt(std::max(0.0, 0.5*(d0+d1)));
        else {
            const double instantaneous = d1 > 0.0 ? striker.stiffness*d1*std::sqrt(d1) : 0.0;
            elasticDerivative = (instantaneous-strikeDiscreteGradient(d0, d1, striker.stiffness))/delta;
        }
        const double dampingDerivative = d1 > d0 ? striker.loadingDamping/h : 0.0;
        const double af = result.friction.derivative;
        const double effectiveA = A-af*Yst*Yst/(1.0+af*Ytt);
        const double derivative = 1.0+h*effectiveA*(std::max(0.0, elasticDerivative)+dampingDerivative);
        const double candidate = force-result.strike.residual/derivative;
        VESSEL_WORK((i < 12 && std::isfinite(candidate) && candidate > lo && candidate < hi)
            ? ++contactWork().outerNewton : ++contactWork().outerFallback);
        force = i < 12 && std::isfinite(candidate) && candidate > lo && candidate < hi ? candidate : 0.5*(lo+hi);
    }
    return result;
}
} // namespace vessel
