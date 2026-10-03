#pragma once

#include "TiamatControls.hpp"
#include "TiamatRandom.hpp"
#include <memory>

namespace tiamat {

struct BufferChannel {
    ReaderState reader;
    WriterState writer;
    MacroState macro;
    unsigned subdivisions = 1, candidate = 0, audibleFrames = 1, audibleEnd = 1;
    unsigned sampleCounter = 0, lastBankChange = 0, wraps = 0, cyclesRemaining = 0;
    float priorSample = 0.f;
    bool resizePending = false, writerCycle = false, rerollPending = false;
};

struct BufferControls {
    float frequency = coefficientRate / 24000.f;
    float baseRate = 1.f, rateSlew = 1.f, silence = 0.f, traverse = 0.f, window = .02f;
    float macroBend = 0.f, macroBreak = 0.f;
    unsigned repeatsExponent = 0;
    bool timeChanging = false, freezeRequested = false, momentaryFreeze = false;
    bool clockRequest = false, unique = true;
    unsigned transitionGuard = 0, transitionCycles = 0;
};

struct ReaderResult { float sample; bool wrapped; };
// Independent primitive for fixture playback. State is owned by the caller.
ReaderResult readLinear(const float* plane, unsigned capacity, ReaderState& reader,
    float baseRate, float macroRate, float slew, bool frozen,
    unsigned writeBank, unsigned captureFrames) noexcept;
float sliceWindow(unsigned audibleFrames, unsigned phase, float squaredWindow) noexcept;
float captureWindow(unsigned captureFrames, unsigned advancedWritePhase) noexcept;
unsigned traverseIndex(unsigned subdivisions, float amount, unsigned previous) noexcept;

// Constructor/destructor are preparation/retirement operations, never audio
// callbacks. No processing method allocates, clears or relinquishes the planes.
// A reduced capacity and coefficient rate are allowed only for reference tests.
class Buffer {
public:
    explicit Buffer(unsigned capacity = planeFrames, float coefficients = coefficientRate,
        unsigned initialFrames = 24000);
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    void processBlock(const float* interleavedInput, float* interleavedOutput,
        const BufferControls&, Random&) noexcept;
    unsigned capacity() const noexcept { return capacity_; }
    const BufferChannel& channel(unsigned ch) const noexcept { return channels_[ch]; }
    const TransitionState& transition() const noexcept { return transition_; }
    float frequency() const noexcept { return frequency_; }
private:
    friend struct BufferTestAccess;
    unsigned capacity_;
    float coefficients_, frequency_, inverseFrequency_;
    std::unique_ptr<float[]> memory_;
    std::array<BufferChannel, 2> channels_;
    TransitionState transition_;
    unsigned microIndex_ = 0, transitionCycles_ = 0;
#ifdef TIAMAT_BUFFER_TEST_HOOKS
    void (*observer_)(const Buffer&, unsigned, void*) = nullptr;
    void* observerData_ = nullptr;
#endif
    void processFrames(const float*, float*, unsigned frames, const BufferControls&, Random&) noexcept;
    void write(unsigned channel, float input, bool timeChanging) noexcept;
    void history(unsigned channel, float input) noexcept;
    void changeBank(BufferChannel&) noexcept;
};

} // namespace tiamat
