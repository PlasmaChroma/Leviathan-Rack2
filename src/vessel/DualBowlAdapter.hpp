#pragma once
#include "HostRateAdapter.hpp"

namespace vessel {
struct DualBowlFrame : HostFrame {
    double leftEnergy = 0.0, rightEnergy = 0.0;
};

// Two independent states with an optional zero-separation single-bowl path.
// Pass false to retain the always-dual scalar reference for comparisons.
class DualBowlAdapter {
public:
    explicit DualBowlAdapter(bool allowSingle = true) noexcept
        : allowSingle_(allowSingle), rightActive_(!allowSingle), dualMix_(allowSingle ? 0.0 : 1.0) {}
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
        const EngineSettings& centerSettings, double separationHz, double hostRate,
        ProcessingQuality quality = ProcessingQuality::Reference) noexcept;
    void reset() noexcept;
    void setAuditEnabled(bool enabled) noexcept;
    DualBowlFrame process(const HostControls& controls) noexcept;
    const VesselEngine& engine() const noexcept { return left_.engine(); }
    const VesselEngine& rightEngine() const noexcept { return rightActive_ ? right_.engine() : left_.engine(); }
    double hostRate() const noexcept { return left_.hostRate(); }
    double internalRate() const noexcept { return left_.internalRate(); }
    double latencySeconds() const noexcept { return left_.latencySeconds(); }
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
    HostRateAdapter left_, right_;
    double centerFrequency_ = 261.625565, separationHz_ = 0.0;
    bool allowSingle_, rightActive_;
    double dualMix_;
};
} // namespace vessel
