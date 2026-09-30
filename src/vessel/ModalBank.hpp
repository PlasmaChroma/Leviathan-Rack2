#pragma once

#include "Types.hpp"

namespace vessel {

class ModalBank {
public:
    // Setup/control boundary only: expensive transforms are cached here.
    // Failure leaves both coefficients and the persistent state untouched.
    bool configure(const BowlDescriptor& bowl, double frequency, double decayMultiplier,
                   double imperfection, double internalRate) noexcept;
    void clear() noexcept;
    std::size_t size() const noexcept { return count_; }
    double timeStep() const noexcept { return h_; }
    const ModeCoefficients& coefficients(std::size_t i) const noexcept { return coefficients_[i]; }
    const ModalState& state(std::size_t i) const noexcept { return states_[i]; }
    bool setState(std::size_t i, double x, double y) noexcept;
    double energy() const noexcept;
    bool finite() const noexcept;

    ModalVector freeMidpoint() const noexcept;
    double velocity(const ModalVector& port) const noexcept;
    double midpointVelocity(const ModalVector& port, const ModalVector& free) const noexcept;
    double admittance(const ModalVector& a, const ModalVector& b) const noexcept;
    ModalStepAudit commit(const ModalVector& free, const ModalVector& force,
                         bool audit = false) noexcept;

    // Shapes use outward radial and increasing-angle tangential coordinates.
    // The outside striker uses inward=true for both injection and observation.
    ModalVector radialPort(double angle, double patchWidth, bool inward = false) const noexcept;
    ModalVector tangentialPort(double angle, double patchWidth) const noexcept;
    ModalVector observer(double angle) const noexcept;

private:
    std::array<ModeCoefficients, maxModes> coefficients_ {};
    std::array<ModalState, maxModes> states_ {};
    std::array<ModePairDescriptor, maxPairs> pairs_ {};
    // Contiguous coefficients for the audio loops; preserve scalar operation order.
    ModalVector hotA_ {}, hotInverseD_ {}, hotWeight_ {}, hotOmega_ {}, hotSigma_ {};
    std::size_t count_ = 0;
    double h_ = 1.0 / 192000.0;
};

// Small numerical loops are visible to the audio compiler; finite-state
// validation and coefficient generation remain in the strict-math .cpp.
inline ModalVector ModalBank::freeMidpoint() const noexcept {
    ModalVector free {};
    for (std::size_t j = 0; j < count_; ++j)
        free[j] = (states_[j].y - hotA_[j]*states_[j].x)*hotInverseD_[j];
    return free;
}
inline double ModalBank::velocity(const ModalVector& port) const noexcept {
    double v = 0.0;
    for (std::size_t j = 0; j < count_; ++j) v += port[j]*states_[j].y;
    return v;
}
inline double ModalBank::midpointVelocity(const ModalVector& port, const ModalVector& free) const noexcept {
    double v = 0.0;
    for (std::size_t j = 0; j < count_; ++j) v += port[j]*free[j];
    return v;
}
inline double ModalBank::admittance(const ModalVector& a, const ModalVector& b) const noexcept {
    double y = 0.0;
    for (std::size_t j = 0; j < count_; ++j) y += a[j]*b[j]*hotWeight_[j];
    return y;
}
inline ModalStepAudit ModalBank::commit(const ModalVector& free, const ModalVector& force, bool audit) noexcept {
    ModalStepAudit result;
    if (audit) result.energyBefore = energy();
    for (std::size_t j = 0; j < count_; ++j) {
        auto& s = states_[j];
        const double mid = free[j] + hotWeight_[j]*force[j];
        s.x += h_*hotOmega_[j]*mid;
        s.y = 2.0*mid - s.y;
        if (audit) {
            result.work += h_*force[j]*mid;
            result.dampingLoss += 2.0*h_*hotSigma_[j]*mid*mid;
        }
    }
    if (audit) {
        result.energyAfter = energy();
        result.residual = result.energyAfter-result.energyBefore-result.work+result.dampingLoss;
    }
    return result;
}
} // namespace vessel
