#pragma once
#include "FrictionContact.hpp"
#include "StrikeContact.hpp"

namespace vessel {
struct CoupledSolution {
    StrikeSolution strike;
    FrictionSolution friction;
    unsigned innerIterations = 0;
    bool converged = false;
};

// Certified nested reference solver; both ports observe the same midpoint.
CoupledSolution solveContacts(double compression, double strikerVelocity,
    double freeStrikeVelocity, double freeTangentVelocity,
    double Yss, double Yst, double Ytt, double h,
    const MalletDescriptor& striker, const MalletDescriptor& rubber,
    double speed, double load, double previousFriction = 0.0) noexcept;
} // namespace vessel
