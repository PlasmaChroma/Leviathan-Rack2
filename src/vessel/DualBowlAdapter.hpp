#pragma once
#include "PassiveTail.hpp"

namespace vessel {
struct DualBowlFrame : HostFrame {
    double leftEnergy = 0.0, rightEnergy = 0.0;
};

// Two independent states with an optional zero-separation single-bowl path.
// Both independent engines feed one stereo filter. Pass false to disable sleep.
class DualBowlAdapter {
public:
    explicit DualBowlAdapter(bool allowSingle = true) noexcept
        : allowSingle_(allowSingle), rightActive_(!allowSingle), dualMix_(allowSingle ? 0.0 : 1.0) {}
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
        const EngineSettings& centerSettings, double separationHz, double hostRate,
        ProcessingQuality quality = ProcessingQuality::Reference) noexcept;
    void reset() noexcept;
    void setAuditEnabled(bool enabled) noexcept;
    // Reference switch for offline equivalence/timing. Disabling a live tail
    // uses the same immediate-contact handoff as a new strike.
    void setComposedTailEnabled(bool enabled) noexcept;
    bool composedTailActive() const noexcept { return tailRunning_; }
    bool tailHandoffActive() const noexcept { return handoffRemaining_!=0; }
    bool tailCacheReady() const noexcept { return tailLeft_.ready() && (!rightActive_ || tailRight_.ready()); }
    void updateHighEnergyDamping() noexcept;
    DualBowlFrame process(const HostControls& controls) noexcept;
    const VesselEngine& engine() const noexcept { return left_; }
    const VesselEngine& rightEngine() const noexcept { return rightActive_ ? right_ : left_; }
    double hostRate() const noexcept { return hostRate_; }
    double internalRate() const noexcept { return hostRate_*decimator_.factor(); }
    double latencySeconds() const noexcept { return hostRate_ > 0 ? decimator_.latencyHostSamples()/hostRate_ : 0.0; }
    double centerFrequency() const noexcept { return centerFrequency_; }
    double separationHz() const noexcept { return separationHz_; }
    double meanEnergy() const noexcept {
        const double left = engine().bowl().energy();
        const double right = rightEngine().bowl().energy();
        return dualMix_ == 1 ? .5*(left+right) : left+.5*dualMix_*(right-left);
    }
    bool secondBowlActive() const noexcept { return rightActive_; }
    double dualMix() const noexcept { return dualMix_; }
private:
    void beginTailHandoff() noexcept;
    void invalidateTailCache() noexcept;
    unsigned tailHistoryFrames() const noexcept;
    VesselEngine left_, right_;
    PassiveTail tailLeft_, tailRight_, baselineLeft_, baselineRight_;
    StereoDecimator baselineFilter_;
    unsigned tailWarm_=0, handoffRemaining_=0, tailHistoryFrames_=1;
    bool tailEnabled_=true, tailRunning_=false, baselineDual_=false;
    StereoDecimator decimator_;
    double hostRate_ = 0.0;
    double transitionGain_ = 1.0, transitionIncrement_ = 1.0;
    StereoSample lastOutput_, transitionFrom_;
    double centerFrequency_ = 261.625565, separationHz_ = 0.0;
    bool allowSingle_, rightActive_;
    double dualMix_;
    double dualMixIncrement_ = 0.0;
};
} // namespace vessel
