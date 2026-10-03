#pragma once
#include "TiamatBufferEngine.hpp"
#include "TiamatCorrupt.hpp"
#include "TiamatOutput.hpp"

namespace tiamat {
// Complete fixed-rate core. All pointers refer to 192 interleaved floats;
// input/output aliasing is supported. Host-rate transport belongs to the adapter.
class Core {
public:
    EventState& eventState() noexcept { return buffer_.eventState(); }
    const BufferEngine& bufferEngine() const noexcept { return buffer_; }
    const Corrupt& corrupt() const noexcept { return corrupt_; }
    const Output& output() const noexcept { return output_; }
    void processBlock(const float* input, float* output, const BufferBlockInput&) noexcept;
private:
    BufferEngine buffer_;
    Corrupt corrupt_;
    Output output_;
    std::array<float, blockFrames * 2> input_ {}, wet_ {};
};
} // namespace tiamat
