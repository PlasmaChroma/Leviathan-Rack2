#include "DualBowlAdapter.hpp"
#include <algorithm>
#include <cmath>

namespace vessel {
bool DualBowlAdapter::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, double separation, double rate, ProcessingQuality quality) noexcept {
    if (!std::isfinite(separation) || separation < 0 || separation > 33
        || !std::isfinite(settings.frequency) || settings.frequency < 20 || settings.frequency > 2000) return false;
    const double center = std::max(20+.5*separation, std::min(2000-.5*separation, settings.frequency));
    auto leftSettings = settings, rightSettings = settings;
    leftSettings.frequency = center-.5*separation;
    rightSettings.frequency = center+.5*separation;
    const bool wantsDual = !allowSingle_ || separation > 0;
    const bool wakeRight = wantsDual && !rightActive_;
    const bool initial = hostRate_ == 0;
    // Validate both modal configurations before mutating either live adapter.
    // Ordinary retuning leaves filter histories and contact ledgers in place.
    VesselEngine::PreparedConfiguration nextLeft, nextRight;
    unsigned nextFactor = 0;
    const auto& rightSource = wakeRight ? left_ : right_;
    const unsigned maximum = HostRateAdapter::factorForRate(rate);
    bool prepared = false;
    // Keep both bowls at the same rate. Fall back only as far as Reference;
    // unsupported descriptors still reject transactionally.
    for (unsigned factor = HostRateAdapter::factorForRate(rate, quality);
        factor && factor <= maximum; factor *= 2) {
        if (!left_.prepareConfiguration(bowl, mallet, leftSettings, rate*factor, nextLeft)) continue;
        if ((rightActive_ || wantsDual)
            && !rightSource.prepareConfiguration(bowl, mallet, rightSettings, rate*factor, nextRight)) continue;
        nextFactor = factor;
        prepared = true;
        break;
    }
    if (!prepared) return false;
    // Clone pre-retune mechanics, including an active striker. The shared FIR
    // retains its history across wake/fold; it never belongs to either bowl.
    if (wakeRight) right_ = left_;
    left_.applyConfiguration(bowl, mallet, leftSettings, nextLeft);
    if (rightActive_ || wantsDual) right_.applyConfiguration(bowl, mallet, rightSettings, nextRight);
    if (rate != hostRate_ || nextFactor != decimator_.factor()) {
        decimator_.configure(nextFactor);
        transitionGain_ = initial ? 1.0 : 0.0;
        transitionFrom_ = lastOutput_;
        transitionIncrement_ = 1.0/(0.005*rate);
        hostRate_ = rate;
    }
    dualMixIncrement_ = 1.0/(.05*rate*nextFactor);
    if (wakeRight) { rightActive_ = true; if (initial) dualMix_ = 1.0; }
    centerFrequency_ = center; separationHz_ = separation;
    return true;
}
void DualBowlAdapter::reset() noexcept {
    left_.reset(); right_.reset();
    decimator_.reset();
    transitionGain_ = 1.0;
    lastOutput_ = {}; transitionFrom_ = {};
    rightActive_ = !allowSingle_ || separationHz_ > 0;
    dualMix_ = rightActive_ ? 1.0 : 0.0;
}
void DualBowlAdapter::setAuditEnabled(bool enabled) noexcept {
    left_.setAuditEnabled(enabled); right_.setAuditEnabled(enabled);
}
void DualBowlAdapter::updateHighEnergyDamping() noexcept {
    left_.updateHighEnergyDamping();
    if (rightActive_) right_.updateHighEnergyDamping();
}
DualBowlFrame DualBowlAdapter::process(const HostControls& controls) noexcept {
    DualBowlFrame frame;
    if (hostRate_ == 0.0) { frame.fault = true; return frame; }
    const double speed = std::isfinite(controls.speed) ? std::max(-2.0, std::min(2.0, controls.speed)) : 0.0;
    const double pressure = std::isfinite(controls.pressure) ? std::max(0.0, std::min(15.0, controls.pressure)) : 0.0;
    left_.setRotation(controls.rotate, speed, pressure);
    if (controls.strikeEvent) left_.strike(controls.velocity, controls.strikeVelocityScale);
    if (rightActive_) {
        right_.setRotation(controls.rotate, speed, pressure);
        if (controls.strikeEvent) right_.strike(controls.velocity, controls.strikeVelocityScale);
    }
    for (unsigned i = 0; i < decimator_.factor(); ++i) {
        const auto left = left_.step();
        const auto right = rightActive_ ? right_.step() : left;
        frame.fault = frame.fault || left.fault || right.fault;
        if (rightActive_) {
            if (allowSingle_ && separationHz_ == 0) {
                dualMix_ = std::max(0.0, dualMix_-dualMixIncrement_);
                if (dualMix_ < 1e-12) { dualMix_ = 0; rightActive_ = false; }
            } else {
                dualMix_ = std::min(1.0, dualMix_+dualMixIncrement_);
                if (1-dualMix_ < 1e-12) dualMix_ = 1;
            }
        }
        StereoSample sample;
        sample.left = left.leftVelocity;
        // Fade before filtering so no filter history is replaced at zero.
        // Linear gains preserve level when the two pickups coincide.
        sample.right = dualMix_ == 1 ? right.rightVelocity : dualMix_ == 0 ? left.rightVelocity
            : left.rightVelocity+dualMix_*(right.rightVelocity-left.rightVelocity);
        decimator_.push(sample, frame.audio);
    }
    frame.leftEnergy = left_.bowl().energy();
    frame.rightEnergy = rightActive_ ? right_.bowl().energy() : frame.leftEnergy;
    frame.bowlEnergy = dualMix_ == 1 ? .5*(frame.leftEnergy+frame.rightEnergy)
        : frame.leftEnergy+.5*dualMix_*(frame.rightEnergy-frame.leftEnergy);
    if (frame.fault || !std::isfinite(frame.audio.left) || !std::isfinite(frame.audio.right)) {
        decimator_.reset(); frame.audio = {}; frame.fault = true;
        transitionGain_ = 0.0;
        transitionFrom_ = {};
    } else {
        transitionGain_ = std::min(1.0, transitionGain_+transitionIncrement_);
        frame.audio.left = transitionFrom_.left*(1.0-transitionGain_)+frame.audio.left*transitionGain_;
        frame.audio.right = transitionFrom_.right*(1.0-transitionGain_)+frame.audio.right*transitionGain_;
    }
    lastOutput_ = frame.audio;
    return frame;
}
} // namespace vessel
