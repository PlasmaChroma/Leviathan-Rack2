#pragma once

#include "TiamatBuffer.hpp"
#include "TiamatClock.hpp"
#include "TiamatEvents.hpp"

namespace tiamat {
class Corrupt;

struct BufferBlockInput {
    PrimaryControls primary;
    ControlVoltages cv;
    BlockEvents events;
    std::array<bool, blockFrames> clockRises {};
    bool restoreSecondary = false, restartRandom = false;
    std::uint64_t seed = 1;
    bool updateSettings = false;
    SecondarySettings settings;
};

// Headless fixed-rate buffer stage. Output is wet, before Corrupt/mix/Tone;
// Core composes those stages. This is not a host-rate Rack adapter.
class BufferEngine {
public:
    BufferEngine() = default;
    EventState& eventState() noexcept { return events_; } // audio owner only
    const Buffer& buffer() const noexcept { return buffer_; }
    const Clock& clock() const noexcept { return clock_; }
    const CorruptRoutingState& corruptRouting() const noexcept { return corrupt_; }
    const MappedControls& mapped() const noexcept { return mapped_; }
    std::uint64_t randomState() const noexcept { return random_.state(); }
    std::uint64_t clockBoundaryCount() const noexcept { return boundaries_; }
    void processBlock(const float* input, float* wet, const BufferBlockInput&) noexcept;
    // Keep Dropout on the same stream, after both Buffer passes.
    void processCorrupt(Corrupt&, const float* input, float* output) noexcept;
private:
    Buffer buffer_;
    Clock clock_;
    Random random_;
    ControlMapper mapper_;
    EventState events_;
    CorruptRoutingState corrupt_;
    MappedControls mapped_;
    std::uint64_t frames_ = 0, boundaries_ = 0;
};

} // namespace tiamat
