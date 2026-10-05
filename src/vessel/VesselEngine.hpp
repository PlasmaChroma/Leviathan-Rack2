#pragma once

#include "ModalBank.hpp"
#include "StrikeContact.hpp"
#include "ContactSolver.hpp"

#include <cstdint>

namespace vessel {

struct EngineSettings {
    // Tunable basic frequency in Hz: geometric center of the lowest mode pair.
    // All higher pairs follow their specimen ratios; strike and rub share it.
    double frequency = 261.625565;
    double decayMultiplier = 1.0;
    double imperfection = 1.0;
    double strikeAngle = pi/8.0;
    double observerCenter = pi/9.0;
    double observerSeparation = pi/6.0 * 0.7;
    // Sensitivity experiment, not compliant normal contact/contact loss.
    bool prescribedRadialLoad = false;
};

struct EnergyLedger {
    double launchWork = 0.0;
    double speedCapWork = 0.0;
    double modalLoss = 0.0;
    double contactLoss = 0.0;
    double retiredEnergy = 0.0;
    double recoveryLoss = 0.0;
    double handWork = 0.0;
    double frictionLoss = 0.0;
    double radialWork = 0.0;
    double maxStepResidual = 0.0;
    std::uint64_t strikes = 0;
    std::uint64_t speedCaps = 0;
    std::uint64_t solverFaults = 0;
    std::uint64_t nonfiniteResets = 0;
    unsigned maxSolverIterations = 0;
    unsigned maxFrictionIterations = 0;
    std::array<std::uint64_t, 81> frictionIterationHistogram {};
    std::array<std::uint64_t, 81> strikeIterationHistogram {};
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
    double frictionForce = 0.0;
    double slip = 0.0;
    double normalLoad = 0.0;
    double handSpeed = 0.0;
    double uniquenessNumber = 0.0;
};

class VesselEngine {
public:
    VesselEngine();
    // Setup/control boundary: coefficients and observation ports are cached.
    // This internal-rate engine deliberately leaves Rack smoothing/rate conversion
    // to the future adapter. Rotation controls are smoothed internally.
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                   const EngineSettings& settings, double internalRate) noexcept;
    void reset() noexcept;
    // Optional performance policy, called by Vessel at its existing 1 kHz
    // control boundary. This is not a calibrated material law or energy cap.
    void updateHighEnergyDamping() noexcept;
    // Enable before launch/reset for a complete cumulative ledger. Turning
    // auditing on halfway through an existing tail omits earlier losses.
    void setAuditEnabled(bool enabled) noexcept { audit_ = enabled; }
    // Oracle switch for offline equivalence checks; copied with the engine.
    void setFastTailEnabled(bool enabled) noexcept { fastTail_ = enabled; }
    bool strike(double normalizedVelocity, double velocityScale = 1.0) noexcept;
    bool setRotation(bool engaged, double revolutionsPerSecond, double pressure) noexcept;
    EngineFrame step() noexcept;

    const ModalBank& bowl() const noexcept { return bank_; }
    const EngineSettings& settings() const noexcept { return settings_; }
    const EnergyLedger& ledger() const noexcept { return ledger_; }
    bool strikerActive() const noexcept { return active_; }
    double compression() const noexcept { return compression_; }
    double strikerVelocity() const noexcept { return strikerVelocity_; }
    double strikerEnergy() const noexcept;
    double totalEnergy() const noexcept { return bank_.energy()+strikerEnergy(); }
    double rotationAngle() const noexcept { return rotationAngle_; }
    double contactEngagement() const noexcept { return engagement_; }
    double maxFrictionUniqueness() const noexcept { return frictionCertificate_; }

private:
    friend class PassiveTail;
    friend class HostRateAdapter;
    friend class DualBowlAdapter;
    // Synchronous preparation on the audio owner. The small modal bank carries
    // its current state; contact state, ledgers and filter histories stay live.
    struct PreparedConfiguration {
        ModalBank bank;
        double certificate = 0.0;
    };
    bool prepareConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
        const EngineSettings& settings, double rate, PreparedConfiguration& next) const noexcept;
    void applyConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
        const EngineSettings& settings, const PreparedConfiguration& next) noexcept;
    ModalBank bank_;
    EngineSettings settings_;
    MalletDescriptor mallet_ {};
    MalletDescriptor activeMallet_ {};
    BowlDescriptor descriptor_ {};
    ContactOrbit orbit_;
    ModalVector strikePort_ {}, observerL_ {}, observerR_ {};
    EnergyLedger ledger_;
    double activeAngle_ = 0.0;
    double compression_ = 0.0;
    double strikerVelocity_ = 0.0;
    bool active_ = false;
    bool audit_ = false;
    bool fastTail_ = true;
    bool rotating_ = false;
    double rotationAngle_ = 0.0;
    double engagement_ = 0.0;
    double targetSpeed_ = 0.4, speed_ = 0.4;
    double targetPressure_ = 2.5, pressure_ = 2.5;
    double controlAlpha_ = 0.0;
    double previousFriction_ = 0.0;
    double frictionCertificate_ = 0.0;
};

} // namespace vessel
