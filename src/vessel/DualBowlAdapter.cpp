#include "DualBowlAdapter.hpp"
#include <algorithm>
#include <cmath>

namespace vessel {
bool DualBowlAdapter::configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
    const EngineSettings& settings, double separation, double rate) noexcept {
    if (!std::isfinite(separation) || separation < 0 || separation > 33
        || !std::isfinite(settings.frequency) || settings.frequency < 20 || settings.frequency > 2000) return false;
    const double center = std::max(20+.5*separation, std::min(2000-.5*separation, settings.frequency));
    auto leftSettings = settings, rightSettings = settings;
    leftSettings.frequency = center-.5*separation;
    rightSettings.frequency = center+.5*separation;
    const bool wantsDual = !allowSingle_ || separation > 0;
    const bool wakeRight = wantsDual && !rightActive_;
    const bool initial = left_.hostRate() == 0;
    // Validate both modal configurations before mutating either live adapter.
    // Ordinary retuning leaves filter histories and contact ledgers in place.
    HostRateAdapter::PreparedConfiguration nextLeft, nextRight;
    if (!left_.prepareConfiguration(bowl, mallet, leftSettings, rate, nextLeft)) return false;
    const auto& rightSource = wakeRight ? left_ : right_;
    if ((rightActive_ || wantsDual)
        && !rightSource.prepareConfiguration(bowl, mallet, rightSettings, rate, nextRight)) return false;
    // Waking still clones the complete pre-retune history, including a latched
    // strike and both FIR channels. Preparation above must use that same source.
    if (wakeRight) right_ = left_;
    left_.applyConfiguration(bowl, mallet, leftSettings, rate, nextLeft);
    if (rightActive_ || wantsDual) right_.applyConfiguration(bowl, mallet, rightSettings, rate, nextRight);
    if (wakeRight) { rightActive_ = true; if (initial) dualMix_ = 1.0; }
    centerFrequency_ = center; separationHz_ = separation;
    return true;
}
void DualBowlAdapter::reset() noexcept {
    left_.reset(); right_.reset();
    rightActive_ = !allowSingle_ || separationHz_ > 0;
    dualMix_ = rightActive_ ? 1.0 : 0.0;
}
void DualBowlAdapter::setAuditEnabled(bool enabled) noexcept {
    left_.setAuditEnabled(enabled); right_.setAuditEnabled(enabled);
}
DualBowlFrame DualBowlAdapter::process(const HostControls& controls) noexcept {
    const auto left = left_.process(controls);
    const auto right = rightActive_ ? right_.process(controls) : left;
    if (rightActive_) {
        const double increment = 1.0/(.05*hostRate());
        if (allowSingle_ && separationHz_ == 0) {
            dualMix_ = std::max(0.0, dualMix_-increment);
            if (dualMix_ < 1e-12) { dualMix_ = 0; rightActive_ = false; }
        } else {
            dualMix_ = std::min(1.0, dualMix_+increment);
            if (1-dualMix_ < 1e-12) dualMix_ = 1;
        }
    }
    DualBowlFrame frame;
    frame.fault = left.fault || right.fault;
    frame.leftEnergy = left.bowlEnergy;
    frame.rightEnergy = rightActive_ ? right.bowlEnergy : left.bowlEnergy;
    frame.bowlEnergy = dualMix_ == 1 ? .5*(left.bowlEnergy+right.bowlEnergy)
        : left.bowlEnergy+.5*dualMix_*(right.bowlEnergy-left.bowlEnergy);
    if (!frame.fault) {
        frame.audio.left = left.audio.left;
        // Linear fade avoids an equal-power boost when the paths coincide.
        frame.audio.right = dualMix_ == 1 ? right.audio.right : dualMix_ == 0 ? left.audio.right
            : left.audio.right+dualMix_*(right.audio.right-left.audio.right);
    }
    return frame;
}
} // namespace vessel
