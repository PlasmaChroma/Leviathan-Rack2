#pragma once

#include "TiamatControls.hpp"
#include <cstdint>

namespace tiamat {

struct ClockTick {
    std::uint64_t boundaries = 0;
    float bufferFrequency = coefficientRate / 24000.f;
    bool timeChanging = false, lost = false, capacityLimited = false;
};

// Renderer-timeline scheduler. The rate bridge must transport host edges onto
// this timeline; it must not call this with undelayed live host-jack values.
class Clock {
public:
    Clock() noexcept;
    void configure(float effectiveTime, ClockSource source) noexcept;
    void resetPhase() noexcept;
    void resetPhaseAt(double seconds) noexcept;
    ClockTick advance(double seconds, bool externalRise = false) noexcept;
    double requestedPeriod() const noexcept { return period_; }
    double estimatedInputPeriod() const noexcept { return median_; }
    const ClockState& state() const noexcept { return state_; }
private:
    ClockState state_;
    ClockSource source_ = ClockSource::Internal;
    double now_ = 0., period_, median_ = .5, lastInterval_ = .5;
    float time_ = .5f, ratio_ = 1.f, smoothedMs_;
    unsigned divisor_ = 1;
    bool primeDivider_ = false;
    std::uint64_t advanceSchedule(double until, bool inclusive) noexcept;
};

} // namespace tiamat
