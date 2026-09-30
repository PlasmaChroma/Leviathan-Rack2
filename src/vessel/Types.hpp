#pragma once

#include <array>
#include <cstddef>

namespace vessel {

constexpr std::size_t maxPairs = 12;
constexpr std::size_t maxModes = 2 * maxPairs;
constexpr double pi = 3.14159265358979323846;
constexpr double log1000 = 6.9077552789821370521;

// Plain aggregates keep generated profile data usable by Rack's C++11 build.
// Eigenvectors have unit peak radial amplitude; masses include both components.
struct ModePairDescriptor {
    int order;
    double ratio, splitCents, orientation;
    double massA, massB, t60A, t60B, radiationA, radiationB;
};

struct BowlDescriptor {
    const char* stableId;
    int version;
    double rimRadius;
    std::size_t pairCount;
    std::array<ModePairDescriptor, maxPairs> pairs;
};

struct MalletDescriptor {
    const char* stableId;
    double mass, stiffness, exponent, loadingDamping, patchWidth;
    double muS, muK, weakeningVelocity, regularizationVelocity;
};

using ModalVector = std::array<double, maxModes>;

struct ModalState {
    double x = 0.0;
    double y = 0.0;
};

struct ModeCoefficients {
    double frequency = 0.0;
    double t60 = 0.0;
    double mass = 1.0;
    double inverseRootMass = 1.0;
    double omega = 0.0;
    double sigma = 0.0;
    double a = 0.0;
    double inverseD = 1.0;
    double admittanceWeight = 0.0;
    double staticComplianceRatio = 1.0;
};

struct ModalStepAudit {
    double energyBefore = 0.0;
    double energyAfter = 0.0;
    double work = 0.0;
    double dampingLoss = 0.0;
    double residual = 0.0;
};

} // namespace vessel
