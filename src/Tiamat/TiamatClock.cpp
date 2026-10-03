#include "TiamatClock.hpp"
#include "TiamatMath.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace tiamat {

Clock::Clock() noexcept : period_(1. / internalFrequency(.5f)), smoothedMs_(float(period_ * 1000.)) {
    state_.nextBoundary = period_;
}
void Clock::configure(float time, ClockSource source) noexcept {
    time = normalized(time);
    if (time == time_ && source == source_) return;
    const double oldPeriod = period_;
    const auto oldSource = source_;
    time_ = time;
    source_ = source;
    if (source_ == ClockSource::Internal) state_.lost = false;
    ratio_ = externalRatio(time);
    divisor_ = ratio_ < 1.f ? unsigned(1.f / ratio_) : 1;
    period_ = source_ == ClockSource::Internal ? 1. / internalFrequency(time_) : median_ / ratio_;
    // Keep fractional progress when Time changes. A source switch starts a
    // fresh period; external divider counting starts from subsequent edges.
    const double remaining = oldSource == source_ ? std::max(0., state_.nextBoundary - now_) / oldPeriod : 1.;
    state_.nextBoundary = now_ + std::min(1., remaining) * period_;
    if (oldSource != source_) state_.dividerCount = 0;
}
void Clock::resetPhase() noexcept {
    if (source_ == ClockSource::Internal) state_.nextBoundary = now_ + period_;
    else primeDivider_ = true;
}
void Clock::resetPhaseAt(double seconds) noexcept {
    if (std::isfinite(seconds) && seconds >= now_) now_ = seconds;
    resetPhase();
}
std::uint64_t Clock::advanceSchedule(double until, bool inclusive) noexcept {
    // Repeated thirds (x3) can land a few binary64 ULPs either side of an
    // incoming timestamp. Treat those as the same boundary, not two clocks.
    const double epsilon = std::min(period_ * .25,
        8. * std::numeric_limits<double>::epsilon() * std::max(1., std::abs(until)));
    const double elapsed = until - state_.nextBoundary + (inclusive ? epsilon : -epsilon);
    if (elapsed < 0.) return 0;
    double count = std::floor(elapsed / period_) + 1.;
    if (count <= 0) return 0;
    // No backlog loop, and no out-of-range float-to-integer conversions.
    const double limit = double(std::numeric_limits<std::uint64_t>::max() / 2);
    if (count > limit) {
        state_.nextBoundary = until + period_;
        return std::numeric_limits<std::uint64_t>::max() / 2;
    }
    state_.nextBoundary += count * period_;
    return std::uint64_t(count);
}
ClockTick Clock::advance(double seconds, bool rise) noexcept {
    ClockTick out;
    if (!std::isfinite(seconds) || seconds < now_) {
        out.bufferFrequency = bufferFrequencyTarget(period_);
        out.lost = state_.lost;
        out.capacityLimited = period_ > double(maxCaptureFrames) / rendererRate;
        out.timeChanging = std::abs(float(period_ * 1000.) - smoothedMs_) > 5.f;
        return out;
    }
    now_ = seconds;
    if (source_ == ClockSource::Internal) out.boundaries = advanceSchedule(now_, true);
    else {
        const double interval = now_ - state_.lastEdge;
        const double edgeEpsilon = 8. * std::numeric_limits<double>::epsilon() * std::max(1., std::abs(now_));
        const bool validEdge = rise && (!state_.hasEdge || interval + edgeEpsilon >= 1. / rendererRate);
        const bool wasLost = state_.hasEdge && interval > 4. * lastInterval_;
        // Count intervening multiplier/loss boundaries, but leave an equal-time
        // boundary for an incoming edge to avoid two requests at that instant.
        if (state_.hasEdge && (divisor_ == 1 || wasLost))
            out.boundaries = advanceSchedule(now_, !validEdge);
        if (validEdge) {
            if (state_.hasEdge) {
                lastInterval_ = interval;
                state_.intervals[state_.intervalCursor] = interval;
                state_.intervalCursor = (state_.intervalCursor + 1) % 3;
                const auto& v = state_.intervals;
                median_ = std::max(std::min(v[0], v[1]), std::min(std::max(v[0], v[1]), v[2]));
            }
            state_.hasEdge = true;
            state_.lastEdge = now_;
            state_.lost = false;
            period_ = median_ / ratio_;
            ++state_.dividerCount;
            if (primeDivider_ || state_.dividerCount >= divisor_) {
                ++out.boundaries;
                state_.dividerCount = 0;
                primeDivider_ = false;
                state_.nextBoundary = now_ + period_;
            }
            else state_.nextBoundary = now_ + (divisor_ - state_.dividerCount) * median_;
        }
        else state_.lost = wasLost;
    }
    const float periodMs = float(std::min(period_ * 1000., double(std::numeric_limits<float>::max())));
    smoothedMs_ = multiplyAdd(periodMs, .00013083219528198242f, smoothedMs_ * .999869167804718f);
    out.timeChanging = std::abs(periodMs - smoothedMs_) > 5.f;
    out.bufferFrequency = bufferFrequencyTarget(period_);
    state_.capacityLimited = period_ > double(maxCaptureFrames) / rendererRate;
    out.lost = source_ == ClockSource::External && state_.lost;
    out.capacityLimited = state_.capacityLimited;
    return out;
}

} // namespace tiamat
