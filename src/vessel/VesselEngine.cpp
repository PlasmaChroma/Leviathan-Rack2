#include "VesselEngine.hpp"
#include "SeedProfiles.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {
namespace {
// Compare physical fields explicitly: descriptors contain padding and metadata
// pointers, so bytewise equality is neither necessary nor safe for this cache.
bool sameBowl(const BowlDescriptor& a, const BowlDescriptor& b) noexcept {
    if (a.rimRadius != b.rimRadius || a.pairCount != b.pairCount) return false;
    for (std::size_t i=0; i<a.pairCount; ++i) {
        const auto& x = a.pairs[i]; const auto& y = b.pairs[i];
        if (x.order != y.order || x.ratio != y.ratio || x.splitCents != y.splitCents
            || x.orientation != y.orientation || x.massA != y.massA || x.massB != y.massB
            || x.t60A != y.t60A || x.t60B != y.t60B
            || x.radiationA != y.radiationA || x.radiationB != y.radiationB) return false;
    }
    return true;
}
bool sameMallet(const MalletDescriptor& a, const MalletDescriptor& b) noexcept {
    return a.mass == b.mass && a.stiffness == b.stiffness && a.exponent == b.exponent
        && a.loadingDamping == b.loadingDamping && a.patchWidth == b.patchWidth
        && a.muS == b.muS && a.muK == b.muK && a.weakeningVelocity == b.weakeningVelocity
        && a.regularizationVelocity == b.regularizationVelocity;
}
}

VesselEngine::VesselEngine() {
    configure(seedBowls[0], seedMallets[1], EngineSettings{}, 192000.0);
}
bool VesselEngine::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                              const EngineSettings& settings, double rate) noexcept {
    PreparedConfiguration next;
    if (!prepareConfiguration(bowl, mallet, settings, rate, next)) return false;
    applyConfiguration(bowl, mallet, settings, next);
    return true;
}
bool VesselEngine::prepareConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, double rate, PreparedConfiguration& prepared) const noexcept {
    prepared.reuseMechanics = false;
    if (!validMallet(mallet) || !std::isfinite(settings.strikeAngle)
        || !std::isfinite(settings.observerCenter) || !std::isfinite(settings.observerSeparation)
        || settings.observerSeparation < 0.0 || settings.observerSeparation > pi
        || settings.frequency < 20.0 || settings.frequency > 2000.0
        || settings.decayMultiplier < 0.25 || settings.decayMultiplier > 4.0) return false;
    if (fastObserverConfiguration_ && bank_.size() && rate >= 32000.0 && rate <= 4000000.0
        && bank_.timeStep() == 1.0/rate
        && settings.frequency == settings_.frequency && settings.decayMultiplier == settings_.decayMultiplier
        && settings.imperfection == settings_.imperfection && settings.strikeAngle == settings_.strikeAngle
        && settings.prescribedRadialLoad == settings_.prescribedRadialLoad
        && sameBowl(bowl,descriptor_) && sameMallet(mallet,mallet_)) {
        // Coefficients depend on the rounded timestep, but the strict modal
        // band limit depends on rate itself. Adjacent rates can share a timestep.
        for (std::size_t j=0; j<bank_.size(); ++j)
            if (bank_.coefficients(j).frequency >= .40*rate) return false;
        prepared.reuseMechanics = true;
        return true;
    }
    auto& next = prepared.bank;
    next = bank_;
    if (!next.configure(bowl, settings.frequency, settings.decayMultiplier, settings.imperfection, rate)) return false;
    // A conservative angle-independent upper bound; each pair contributes
    // max(weightA/mA, weightB/mB)*patch^2/n^2, not the sum of both maxima.
    double maxYtt = 0.0;
    for (std::size_t n = 0; n < bowl.pairCount; ++n) {
        const double order = bowl.pairs[n].order;
        const double half = 0.5*order*mallet.patchWidth;
        const double patch = std::abs(half) < 1e-8 ? 1.0-half*half/6.0 : std::sin(half)/half;
        const auto& a = next.coefficients(2*n);
        const auto& b = next.coefficients(2*n+1);
        maxYtt += patch*patch/(order*order)*std::max(a.admittanceWeight/a.mass, b.admittanceWeight/b.mass);
    }
    const double certificate = maxYtt*frictionNegativeSlopeBound(15.0, mallet);
    if (!std::isfinite(certificate) || certificate > 0.9) return false;
    prepared.certificate = certificate;
    return true;
}
void VesselEngine::applyConfiguration(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, const PreparedConfiguration& prepared) noexcept {
    if (!prepared.reuseMechanics) {
        bank_ = prepared.bank;
        frictionCertificate_ = prepared.certificate;
        controlAlpha_ = -std::expm1(-bank_.timeStep()/0.01);
    }
    descriptor_ = bowl;
    // Keep the established orbit resynchronization even on the observer path:
    // skipping it would change accumulated rotation rounding and tick phase.
    orbit_.configure(bowl, bank_, mallet.patchWidth, rotationAngle_);
    settings_ = settings;
    mallet_ = mallet;
    observerL_ = bank_.observer(settings.observerCenter-0.5*settings.observerSeparation);
    observerR_ = bank_.observer(settings.observerCenter+0.5*settings.observerSeparation);
    // Mass changes alter the bowl port, while angle/footprint/contact potential
    // belong to the latched active striker. Compression remains continuous.
    if (active_ && !prepared.reuseMechanics) strikePort_ = bank_.radialPort(activeAngle_, activeMallet_.patchWidth, true);
}
void VesselEngine::reset() noexcept {
    bank_.clear();
    bank_.setAdditionalDamping(0.0);
    active_ = false;
    compression_ = 0.0;
    strikerVelocity_ = 0.0;
    ledger_ = {};
    engagement_ = 0.0;
    rotating_ = false;
    rotationAngle_ = 0.0;
    speed_ = targetSpeed_;
    pressure_ = targetPressure_;
    previousFriction_ = 0.0;
    orbit_.configure(descriptor_, bank_, mallet_.patchWidth, rotationAngle_);
}
void VesselEngine::updateHighEnergyDamping() noexcept {
    // Leave ordinary tails untouched. Smooth onset above 40 mJ, half of the
    // maximum additional loss at 80 mJ; asymptotic amplitude rate 0.5 / s.
    // Bound the argument before squaring; no expensive coefficient rebuild.
    const double excess = std::min(1e6, std::max(0.0, (bank_.energy()-0.04)/0.04));
    const double squared = excess*excess;
    bank_.setAdditionalDamping(0.5*squared/(1.0+squared));
}
bool VesselEngine::setRotation(bool engaged, double speed, double pressure) noexcept {
    if (!std::isfinite(speed) || speed < -2.0 || speed > 2.0
        || !std::isfinite(pressure) || pressure < 0.0 || pressure > 15.0) return false;
    rotating_ = engaged;
    targetSpeed_ = speed;
    targetPressure_ = pressure;
    return true;
}
double VesselEngine::strikerEnergy() const noexcept {
    return active_ ? 0.5*activeMallet_.mass*strikerVelocity_*strikerVelocity_
        + strikePotential(compression_, activeMallet_.stiffness) : 0.0;
}
bool VesselEngine::strike(double normalizedVelocity, double velocityScale) noexcept {
    if (!std::isfinite(normalizedVelocity) || normalizedVelocity <= 0.0
        || !std::isfinite(velocityScale) || velocityScale <= 0.0 || velocityScale > 2.0) return false;
    const double launch = std::min(normalizedVelocity, 1.0) * velocityScale; // relative approach, m/s
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
    if (fastTail_ && !audit_ && !active_ && !rotating_ && engagement_ == 0.0) {
        // Keep every internal sample and every FIR history. Fuse only the
        // unforced modal update, finite checks and pickup projections.
        speed_ += controlAlpha_*(targetSpeed_-speed_);
        pressure_ += controlAlpha_*(targetPressure_-pressure_);
        previousFriction_ = 0.0;
        if (!bank_.advanceFree(observerL_, observerR_, frame.leftVelocity, frame.rightVelocity)
            || !std::isfinite(compression_) || !std::isfinite(strikerVelocity_)) {
            bank_.clear();
            active_ = false; compression_ = strikerVelocity_ = 0.0;
            ++ledger_.nonfiniteResets;
            frame.leftVelocity = frame.rightVelocity = 0.0;
            frame.fault = true;
        }
        return frame;
    }
    const double before = audit_ ? totalEnergy() : 0.0;
    const double h = bank_.timeStep();
    const auto free = bank_.freeMidpoint();
    ModalVector force {};
    StrikeSolution solution;
    FrictionSolution friction;
    ModalVector tangent {}, normal {};
    double radialWork = 0.0;
    const double oldSpeed = speed_, oldPressure = pressure_, oldEngagement = engagement_;
    speed_ += controlAlpha_*(targetSpeed_-speed_);
    pressure_ += controlAlpha_*(targetPressure_-pressure_);
    engagement_ = rotating_ ? std::min(1.0, engagement_+h/0.01) : std::max(0.0, engagement_-h/0.005);
    const double midEngagement = 0.5*(oldEngagement+engagement_);
    const double load = midEngagement*0.5*(oldPressure+pressure_);
    const double U = 2.0*pi*descriptor_.rimRadius*0.5*(oldSpeed+speed_);
    const bool orbitActive = midEngagement > 0.0;
    const double increment = orbitActive ? 2.0*pi*0.5*(oldSpeed+speed_)*h : 0.0;
    if (orbitActive) {
        orbit_.midpoint(increment, tangent, normal, settings_.prescribedRadialLoad);
        if (settings_.prescribedRadialLoad)
            for (std::size_t j = 0; j < bank_.size(); ++j) force[j] = normal[j]*load;
    }
    // Known normal forcing shifts both free port velocities before solving.
    const double vs0 = active_ ? bank_.midpointVelocity(strikePort_, free)
        + (settings_.prescribedRadialLoad ? load*bank_.admittance(strikePort_, normal) : 0.0) : 0.0;
    const double vt0 = load > 0.0 ? bank_.midpointVelocity(tangent, free)
        + (settings_.prescribedRadialLoad ? load*bank_.admittance(tangent, normal) : 0.0) : 0.0;
    const double Ytt = load > 0.0 ? bank_.admittance(tangent, tangent) : 0.0;
    frame.normalLoad = load;
    frame.handSpeed = orbitActive ? U : 0.0;
    frame.uniquenessNumber = Ytt*frictionNegativeSlopeBound(load, mallet_);
    double contactLoss = 0.0, retired = 0.0, recovered = 0.0;
    if (active_) {
        if (load > 0.0) {
            const auto coupled = solveContacts(compression_, strikerVelocity_, vs0, vt0,
                bank_.admittance(strikePort_, strikePort_), bank_.admittance(strikePort_, tangent),
                Ytt, h, activeMallet_, mallet_, U, load, previousFriction_);
            solution = coupled.strike;
            friction = coupled.friction;
            solution.converged = coupled.converged;
            ledger_.maxFrictionIterations = std::max(ledger_.maxFrictionIterations, coupled.innerIterations);
        } else {
            solution = solveStrike(compression_, strikerVelocity_, vs0,
                                   bank_.admittance(strikePort_, strikePort_), h, activeMallet_);
        }
        ledger_.maxSolverIterations = std::max(ledger_.maxSolverIterations, solution.iterations);
        ++ledger_.strikeIterationHistogram[std::min(80u, solution.iterations)];
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
            for (std::size_t j = 0; j < bank_.size(); ++j) force[j] += strikePort_[j]*solution.force;
            contactLoss = h*solution.dampingForce*solution.compressionVelocity;
            frame.strikeForce = solution.force;
            frame.compression = solution.compression;
        }
    } else if (load > 0.0) {
        friction = solveFriction(U-vt0, Ytt, load, mallet_, previousFriction_);
        ledger_.maxFrictionIterations = std::max(ledger_.maxFrictionIterations, friction.iterations);
        if (!friction.converged) { ++ledger_.solverFaults; frame.fault = true; }
    }
    if (frame.fault) {
        force = {};
        friction = {};
        rotating_ = false;
        engagement_ = 0.0;
    } else if (load > 0.0) {
        for (std::size_t j = 0; j < bank_.size(); ++j) force[j] += tangent[j]*friction.force;
        frame.frictionForce = friction.force;
        frame.slip = friction.slip;
        ledger_.maxFrictionIterations = std::max(ledger_.maxFrictionIterations, friction.iterations);
        ++ledger_.frictionIterationHistogram[std::min(80u, friction.iterations)];
    }
    previousFriction_ = frame.frictionForce;
    if (audit_ && settings_.prescribedRadialLoad && !frame.fault)
        radialWork = h*load*(bank_.midpointVelocity(normal, free)+bank_.admittance(normal, force));
    ModalStepAudit modalAudit;
    bool finiteState;
    if (audit_) {
        modalAudit = bank_.commit(free, force, true);
        finiteState = bank_.finite();
    } else {
        finiteState = bank_.commitObserved(free, force, observerL_, observerR_,
            frame.leftVelocity, frame.rightVelocity);
    }
    if (orbitActive) {
        orbit_.finish();
        // Supported speed/rate bounds advance less than one turn per step.
        rotationAngle_ += increment;
        if (rotationAngle_ > pi) rotationAngle_ -= 2.0*pi;
        else if (rotationAngle_ < -pi) rotationAngle_ += 2.0*pi;
    }
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
    if (!finiteState || !std::isfinite(compression_) || !std::isfinite(strikerVelocity_)) {
        bank_.clear();
        active_ = false;
        compression_ = 0.0;
        strikerVelocity_ = 0.0;
        ++ledger_.nonfiniteResets;
        frame.fault = true;
        frame.leftVelocity = frame.rightVelocity = 0.0;
        return frame;
    }
    if (audit_) {
        ledger_.modalLoss += modalAudit.dampingLoss;
        ledger_.contactLoss += contactLoss;
        const double handWork = h*frame.frictionForce*frame.handSpeed;
        const double frictionLoss = h*frame.frictionForce*frame.slip;
        ledger_.handWork += handWork;
        ledger_.frictionLoss += frictionLoss;
        ledger_.radialWork += radialWork;
        frame.stepEnergyResidual = totalEnergy()-before+modalAudit.dampingLoss+contactLoss+retired+recovered
            +frictionLoss-handWork-radialWork;
        ledger_.maxStepResidual = std::max(ledger_.maxStepResidual, std::abs(frame.stepEnergyResidual));
    }
    if (audit_) {
        frame.leftVelocity = bank_.velocity(observerL_);
        frame.rightVelocity = bank_.velocity(observerR_);
    }
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
