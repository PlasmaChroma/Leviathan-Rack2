#pragma once

#include "ChimeraReel.hpp"
#include <array>
#include <atomic>

namespace chimera {

struct MarkerDisplay {
    std::array<std::uint32_t, kMaxSplices> markers{};
    std::uint32_t frames = 0;
    std::uint16_t count = 0;
};

// Latest-value mailbox: one core owner publishes, one UI overlay consumes.
// Three exclusive slots keep both sides bounded, allocation-free, and race-free
// even when the editor is closed or skips frames. Never reads audio pages.
class MarkerDisplayMailbox {
    MarkerDisplay slots_[3];
    unsigned writing_ = 0, reading_ = 2;
    std::atomic<unsigned> middle_{1};
    static constexpr unsigned kDirty = 4;
    std::uint32_t lastHandle_ = UINT32_MAX;
    std::uint64_t lastRevision_ = UINT64_MAX;
public:
    void publish(const Reel* reel, std::uint32_t handle) {
        const std::uint64_t revision = reel ? reel->documentRevision() : 0;
        if (handle == lastHandle_ && revision == lastRevision_) return;
        auto& next = slots_[writing_];
        next.frames = reel ? reel->validFrames() : 0;
        next.count = reel ? reel->markerCount() : 0;
        for (unsigned i = 0; i < next.count; ++i)
            next.markers[i] = reel->region(i).begin;
        writing_ = middle_.exchange(writing_ | kDirty, std::memory_order_acq_rel) & 3u;
        lastHandle_ = handle;
        lastRevision_ = revision;
    }
    bool consume(MarkerDisplay& result) {
        const bool changed = middle_.load(std::memory_order_acquire) & kDirty;
        if (changed)
            reading_ = middle_.exchange(reading_, std::memory_order_acq_rel) & 3u;
        result = slots_[reading_];
        return changed;
    }
};

} // namespace chimera
