#pragma once

#include "ModalBank.hpp"
#include "StrikeContact.hpp"

#include <cstdint>

namespace vessel {

struct EngineSettings {
    double frequency = 261.625565;
    double decayMultiplier = 1.0;
    double imperfection = 1.0;
    double strikeAngle = pi/8.0;
    double observerCenter = pi/9.0;
    double observerSeparation = pi/6.0 * 0.7;
};

struct EnergyLedger {
    double launchWork = 0.0;
    double speedCapWork = 0.0;
    double modalLoss = 0.0;
    double contactLoss = 0.0;
    double retiredEnergy = 0.0;
    double recoveryLoss = 0.0;
    double maxStepResidual = 0.0;
    std::uint64_t strikes = 0;
    std::uint64_t speedCaps = 0;
    std::uint64_t solverFaults = 0;
    std::uint64_t nonfiniteResets = 0;
    unsigned maxSolverIterations = 0;
};

struct EngineFrame {
    // Physical virtual-pickup velocities, before any audio gain or resampling.
    double leftVelocity = 0.0;
    double rightVelocity = 0.0;
    double strikeForce = 0.0;
    double compression = 0.0;
    double stepEnergyResidual = 0.0;
    bool separated = false;
    bool fault = false;
};

class VesselEngine {
public:
    VesselEngine();
    // Setup/control boundary: coefficients and observation ports are cached.
    // This internal-rate engine deliberately leaves Rack smoothing/rate conversion
    // to the future adapter. Rubbing is the next mechanical milestone.
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                   const EngineSettings& settings, double internalRate) noexcept;
    void reset() noexcept;
    // Enable before launch/reset for a complete cumulative ledger. Turning
    // auditing on halfway through an existing tail omits earlier losses.
    void setAuditEnabled(bool enabled) noexcept { audit_ = enabled; }
    bool strike(double normalizedVelocity) noexcept;
    EngineFrame step() noexcept;

    const ModalBank& bowl() const noexcept { return bank_; }
    const EngineSettings& settings() const noexcept { return settings_; }
    const EnergyLedger& ledger() const noexcept { return ledger_; }
    bool strikerActive() const noexcept { return active_; }
    double compression() const noexcept { return compression_; }
    double strikerVelocity() const noexcept { return strikerVelocity_; }
    double strikerEnergy() const noexcept;
    double totalEnergy() const noexcept { return bank_.energy()+strikerEnergy(); }

private:
    ModalBank bank_;
    EngineSettings settings_;
    MalletDescriptor mallet_ {};
    MalletDescriptor activeMallet_ {};
    ModalVector strikePort_ {}, observerL_ {}, observerR_ {};
    EnergyLedger ledger_;
    double activeAngle_ = 0.0;
    double compression_ = 0.0;
    double strikerVelocity_ = 0.0;
    bool active_ = false;
    bool audit_ = false;
};

} // namespace vessel
