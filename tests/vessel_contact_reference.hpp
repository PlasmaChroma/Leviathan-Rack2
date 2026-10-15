#pragma once
// Frozen pre-optimization safeguarded solvers from revision 61451e7048b943de.
// Test/benchmark oracle only: do not update with the production derivative.
#include "../src/vessel/ContactSolver.hpp"
#include <algorithm>
#include <cmath>
namespace vessel_contact_reference {
using namespace vessel;
inline StrikeSolution solveStrike(double d0, double velocity, double freeVelocity,
                           double Yss, double h, const MalletDescriptor& m) noexcept {
    StrikeSolution result;
    if (!validMallet(m) || !std::isfinite(d0) || d0 < 0.0
        || !std::isfinite(velocity) || !std::isfinite(freeVelocity)
        || !std::isfinite(Yss) || Yss < 0.0 || !std::isfinite(h) || h <= 0.0) return result;
    const double A = Yss + h/(2.0*m.mass);
    const double freeCompression = d0 + h*(velocity-freeVelocity);
    auto trial = [&](double force) {
        StrikeSolution s;
        s.force = force;
        s.compression = freeCompression-h*A*force;
        s.compressionVelocity = (s.compression-d0)/h;
        s.dampingForce = std::max(d0, s.compression) > 0.0
            ? m.loadingDamping*std::max(s.compressionVelocity, 0.0) : 0.0;
        s.residual = force-strikeDiscreteGradient(d0, s.compression, m.stiffness)-s.dampingForce;
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
            const double delta = result.compression-d0;
            double elasticDerivative;
            if (std::abs(delta) < 1e-6*std::max(1e-12, std::max(d0, result.compression)))
                elasticDerivative = 0.75*m.stiffness*std::sqrt(std::max(0.0, 0.5*(d0+result.compression)));
            else {
                const double instantaneous = result.compression > 0.0
                    ? m.stiffness*result.compression*std::sqrt(result.compression) : 0.0;
                elasticDerivative = (instantaneous-strikeDiscreteGradient(d0, result.compression, m.stiffness))/delta;
            }
            const double dampingDerivative = result.compression > d0 ? m.loadingDamping/h : 0.0;
            const double derivative = 1.0+h*A*(std::max(0.0, elasticDerivative)+dampingDerivative);
            const double candidate = force-result.residual/derivative;
            if (std::isfinite(candidate) && candidate > lo && candidate < hi) next = candidate;
        }
        force = next;
    }
    return result;
}

inline CoupledSolution solveContacts(double d0, double velocity, double vs0, double vt0,
    double Yss, double Yst, double Ytt, double h,
    const MalletDescriptor& striker, const MalletDescriptor& rubber,
    double speed, double load, double previous) noexcept {
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
        force = i < 12 && std::isfinite(candidate) && candidate > lo && candidate < hi ? candidate : 0.5*(lo+hi);
    }
    return result;
}
} // namespace vessel_contact_reference
