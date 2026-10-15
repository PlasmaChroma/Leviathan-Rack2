#pragma once

#include "Types.hpp"

namespace vessel {

struct StrikeSolution {
    double force = 0.0;
    double compression = 0.0;
    double compressionVelocity = 0.0;
    double dampingForce = 0.0;
    double residual = 0.0;
    unsigned iterations = 0;
    bool converged = false;
};

// Initial contact exponent is explicitly restricted to 3/2. These functions
// implement one shared potential/force law instead of unrelated approximations.
bool validMallet(const MalletDescriptor& mallet) noexcept;
double strikePotential(double compression, double stiffness) noexcept;
double strikeDiscreteGradient(double before, double after, double stiffness) noexcept;
StrikeSolution solveStrike(double compression, double strikerVelocity,
                           double bowlFreeVelocity, double bowlAdmittance,
                           double h, const MalletDescriptor& mallet) noexcept;

} // namespace vessel
