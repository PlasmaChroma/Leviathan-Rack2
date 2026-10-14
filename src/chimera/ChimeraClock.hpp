#pragma once

#include <cstdint>

namespace chimera {

// Core-frame Clock timing for playback. Recording uses the raw Schmitt rise,
// including rises that are too fast for the playback estimator.
class ClockEstimator {
public:
    static const std::uint64_t kMaxPeriod = 2880000;
    static const std::uint64_t kInitialTimeout = 48000;

    struct Update {
        bool acceptedEdge;
        bool tooFast;
        bool newPhase;
    };

    Update step(bool connected, bool rising, std::uint64_t frame) {
        if (!connected) {
            connected_ = false;
            haveEdge_ = havePeriod_ = waiting_ = false;
            return Update{false, false, false};
        }
        if (!connected_) {
            connected_ = true;
            connectedFrame_ = frame;
            haveEdge_ = havePeriod_ = false;
        }
        Update result{false, false, false};
        if (rising) {
            if (!haveEdge_) {
                haveEdge_ = true;
                lastEdgeFrame_ = frame;
                result.acceptedEdge = result.newPhase = true;
            }
            else {
                const std::uint64_t interval = frame - lastEdgeFrame_;
                if (interval < 2) {
                    result.tooFast = true;
                    ++tooFastCount_;
                }
                else {
                    lastEdgeFrame_ = frame;
                    result.acceptedEdge = true;
                    if (interval > kMaxPeriod) {
                        havePeriod_ = false;
                        result.newPhase = true;
                    }
                    else {
                        periodFrames_ = static_cast<std::uint32_t>(interval);
                        havePeriod_ = true;
                    }
                }
            }
        }
        const std::uint64_t anchor = haveEdge_ ? lastEdgeFrame_ : connectedFrame_;
        const std::uint64_t timeout = havePeriod_ &&
            2 * std::uint64_t(periodFrames_) > kInitialTimeout ?
            2 * std::uint64_t(periodFrames_) : kInitialTimeout;
        waiting_ = frame - anchor >= timeout;
        return result;
    }

    bool connected() const { return connected_; }
    bool haveEdge() const { return haveEdge_; }
    bool havePeriod() const { return havePeriod_; }
    bool waiting() const { return waiting_; }
    std::uint32_t periodFrames() const { return periodFrames_; }
    std::uint64_t lastEdgeFrame() const { return lastEdgeFrame_; }
    std::uint32_t tooFastCount() const { return tooFastCount_; }

private:
    bool connected_ = false, haveEdge_ = false, havePeriod_ = false, waiting_ = false;
    std::uint64_t connectedFrame_ = 0, lastEdgeFrame_ = 0;
    std::uint32_t periodFrames_ = 0, tooFastCount_ = 0;
};

} // namespace chimera
