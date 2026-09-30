#include "VesselEngine.hpp"
#include "SeedProfiles.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {

VesselEngine::VesselEngine() {
    configure(seedBowls[0], seedMallets[1], EngineSettings{}, 192000.0);
}
bool VesselEngine::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                              const EngineSettings& settings, double rate) noexcept {
    if (!validMallet(mallet) || !std::isfinite(settings.strikeAngle)
        || !std::isfinite(settings.observerCenter) || !std::isfinite(settings.observerSeparation)
        || settings.observerSeparation < 0.0 || settings.observerSeparation > pi
        || settings.frequency < 20.0 || settings.frequency > 2000.0
        || settings.decayMultiplier < 0.25 || settings.decayMultiplier > 4.0) return false;
    if (!bank_.configure(bowl, settings.frequency, settings.decayMultiplier, settings.imperfection, rate)) return false;
    settings_ = settings;
    mallet_ = mallet;
    observerL_ = bank_.observer(settings.observerCenter-0.5*settings.observerSeparation);
    observerR_ = bank_.observer(settings.observerCenter+0.5*settings.observerSeparation);
    // Mass changes alter the bowl port, while angle/footprint/contact potential
    // belong to the latched active striker. Compression remains continuous.
    if (active_) strikePort_ = bank_.radialPort(activeAngle_, activeMallet_.patchWidth, true);
    return true;
}
void VesselEngine::reset() noexcept {
    bank_.clear();
    active_ = false;
    compression_ = 0.0;
    strikerVelocity_ = 0.0;
    ledger_ = {};
}
double VesselEngine::strikerEnergy() const noexcept {
    return active_ ? 0.5*activeMallet_.mass*strikerVelocity_*strikerVelocity_
        + strikePotential(compression_, activeMallet_.stiffness) : 0.0;
}
bool VesselEngine::strike(double normalizedVelocity) noexcept {
    if (!std::isfinite(normalizedVelocity) || normalizedVelocity <= 0.0) return false;
    const double launch = std::min(normalizedVelocity, 1.0); // relative approach, m/s
    double before = 0.0;
    if (!active_) {
        activeMallet_ = mallet_;
        activeAngle_ = settings_.strikeAngle;
        strikePort_ = bank_.radialPort(activeAngle_, activeMallet_.patchWidth, true);
        compression_ = 0.0;
        strikerVelocity_ = bank_.velocity(strikePort_);
        active_ = true;
    } else before = 0.5*activeMallet_.mass*strikerVelocity_*strikerVelocity_;
    const double idealVelocity = strikerVelocity_+launch;
    const double idealEnergy = 0.5*activeMallet_.mass*idealVelocity*idealVelocity;
    strikerVelocity_ = std::max(-4.0, std::min(4.0, idealVelocity));
    const double actualEnergy = 0.5*activeMallet_.mass*strikerVelocity_*strikerVelocity_;
    ledger_.launchWork += idealEnergy-before;
    ledger_.speedCapWork += actualEnergy-idealEnergy;
    if (strikerVelocity_ != idealVelocity) ++ledger_.speedCaps;
    ++ledger_.strikes;
    return true;
}

EngineFrame VesselEngine::step() noexcept {
    EngineFrame frame;
    const double before = audit_ ? totalEnergy() : 0.0;
    const double h = bank_.timeStep();
    const auto free = bank_.freeMidpoint();
    ModalVector force {};
    StrikeSolution solution;
    double contactLoss = 0.0, retired = 0.0, recovered = 0.0;
    if (active_) {
        solution = solveStrike(compression_, strikerVelocity_, bank_.midpointVelocity(strikePort_, free),
                               bank_.admittance(strikePort_, strikePort_), h, activeMallet_);
        ledger_.maxSolverIterations = std::max(ledger_.maxSolverIterations, solution.iterations);
        if (!solution.converged) {
            // No unconverged force is ever committed. Discard the contact's
            // kinetic/potential energy explicitly and retain the finite bowl.
            recovered = strikerEnergy();
            ledger_.recoveryLoss += recovered;
            ++ledger_.solverFaults;
            active_ = false;
            compression_ = 0.0;
            strikerVelocity_ = 0.0;
            frame.fault = true;
        } else {
            for (std::size_t j = 0; j < bank_.size(); ++j) force[j] = strikePort_[j]*solution.force;
            contactLoss = h*solution.dampingForce*solution.compressionVelocity;
            frame.strikeForce = solution.force;
            frame.compression = solution.compression;
        }
    }
    const auto modalAudit = bank_.commit(free, force, audit_);
    if (active_) {
        compression_ = solution.compression;
        strikerVelocity_ -= h*solution.force/activeMallet_.mass;
        const double relativeEnd = strikerVelocity_-bank_.velocity(strikePort_);
        if (compression_ <= 0.0 && relativeEnd <= 0.0) {
            retired = strikerEnergy();
            ledger_.retiredEnergy += retired;
            active_ = false;
            compression_ = 0.0;
            strikerVelocity_ = 0.0;
            frame.separated = true;
        } else if (compression_ < 0.0) {
            // An interval force can cross the boundary while the endpoint
            // already approaches again. Do not keep a negative-overlap contact:
            // reject this unresolved event and expose its explicit recovery.
            recovered += strikerEnergy();
            ledger_.recoveryLoss += strikerEnergy();
            ++ledger_.solverFaults;
            active_ = false;
            compression_ = 0.0;
            strikerVelocity_ = 0.0;
            frame.fault = true;
        }
    }
    if (!bank_.finite() || !std::isfinite(compression_) || !std::isfinite(strikerVelocity_)) {
        bank_.clear();
        active_ = false;
        compression_ = 0.0;
        strikerVelocity_ = 0.0;
        ++ledger_.nonfiniteResets;
        frame.fault = true;
        return frame;
    }
    if (audit_) {
        ledger_.modalLoss += modalAudit.dampingLoss;
        ledger_.contactLoss += contactLoss;
        frame.stepEnergyResidual = totalEnergy()-before+modalAudit.dampingLoss+contactLoss+retired+recovered;
        ledger_.maxStepResidual = std::max(ledger_.maxStepResidual, std::abs(frame.stepEnergyResidual));
    }
    frame.leftVelocity = bank_.velocity(observerL_);
    frame.rightVelocity = bank_.velocity(observerR_);
    if (!std::isfinite(frame.leftVelocity) || !std::isfinite(frame.rightVelocity)) {
        bank_.clear();
        active_ = false;
        compression_ = 0.0;
        strikerVelocity_ = 0.0;
        ++ledger_.nonfiniteResets;
        frame.leftVelocity = frame.rightVelocity = 0.0;
        frame.fault = true;
    }
    return frame;
}

} // namespace vessel
