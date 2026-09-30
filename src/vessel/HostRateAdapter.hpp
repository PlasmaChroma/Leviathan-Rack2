#pragma once

#include "VesselEngine.hpp"

namespace vessel {

struct StereoSample { double left = 0.0, right = 0.0; };

// Causal 2x stages, fixed storage. Each stage has a 129-tap symmetric
// Blackman-windowed sinc: passband <=0.40 of its output rate, stopband
// >=0.50. Response/latency are measured in vessel_host_rate_spec.
class StereoDecimator {
public:
    static constexpr unsigned taps = 129;
    StereoDecimator() noexcept;
    bool configure(unsigned factor) noexcept;
    void reset() noexcept;
    bool push(const StereoSample& input, StereoSample& output) noexcept;
    unsigned factor() const noexcept { return factor_; }
    // Output samples are tagged at the host frame's first internal sample.
    double latencyHostSamples() const noexcept;
    double coefficient(unsigned tap) const noexcept;
private:
    struct Stage {
        std::array<StereoSample, taps> history {};
        unsigned position = 0, phase = 0;
    };
    std::array<double, (taps+1)/2> coefficients_ {};
    std::array<Stage, 3> stages_ {};
    unsigned factor_ = 1, stageCount_ = 0;
    bool pushStage(Stage& stage, const StereoSample& input, StereoSample& output) noexcept;
};

struct HostControls {
    // Already detected at host rate; a held gate is not an event here.
    bool strikeEvent = false;
    double velocity = 0.5;
    bool rotate = false;
    double speed = 0.4, pressure = 2.5;
};
struct HostFrame {
    StereoSample audio; // Physical pickup velocities, before output gain.
    double bowlEnergy = 0.0; // Current mechanics; not delayed/filtered audio RMS.
    bool fault = false;
};

// One audio owner. Setup/rate changes use configure(); process() has no
// allocations, locks, coefficient generation or UI/Rack dependencies.
class HostRateAdapter {
public:
    bool configure(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                   const EngineSettings& settings, double hostRate) noexcept;
    void reset() noexcept;
    void setAuditEnabled(bool enabled) noexcept { engine_.setAuditEnabled(enabled); }
    HostFrame process(const HostControls& controls) noexcept;
    const VesselEngine& engine() const noexcept { return engine_; }
    double hostRate() const noexcept { return hostRate_; }
    double internalRate() const noexcept { return hostRate_*decimator_.factor(); }
    unsigned factor() const noexcept { return decimator_.factor(); }
    double latencySeconds() const noexcept { return hostRate_ > 0 ? decimator_.latencyHostSamples()/hostRate_ : 0.0; }
    static unsigned factorForRate(double hostRate) noexcept;
private:
    VesselEngine engine_;
    StereoDecimator decimator_;
    double hostRate_ = 0.0;
    double transitionGain_ = 1.0, transitionIncrement_ = 1.0;
    StereoSample lastOutput_, transitionFrom_;
};

} // namespace vessel
