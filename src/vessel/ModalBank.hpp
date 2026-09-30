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
    std::size_t count_ = 0;
    double h_ = 1.0 / 192000.0;
};

} // namespace vessel
