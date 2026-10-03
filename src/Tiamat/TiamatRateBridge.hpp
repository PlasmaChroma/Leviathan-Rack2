#pragma once

#include "TiamatBufferEngine.hpp"
#include <speex/speex_resampler.h>
#include <algorithm>
#include <cmath>
#include <memory>

namespace tiamat {

enum class CommandKind { Control, Settings, RestoreSecondary, RestartRandom };
struct HostCommand {
    CommandKind kind = CommandKind::Control;
    ControlEvent control;
    SecondarySettings settings;
    std::uint64_t seed = 1;
};
struct HostControls { PrimaryControls primary; ControlVoltages cv; };
struct HostFrame {
    HostControls controls;
    float leftVolts = 0.f, rightVolts = 0.f;
    bool leftConnected = false, rightConnected = false;
    // Clock, Freeze, Bend, Break, Corrupt. Unpatched ports supply zero volts.
    std::array<float, 5> gateVolts {};
    std::array<HostCommand, 8> commands {};
    unsigned commandCount = 0;
};
struct HostAudio { float left = 0.f, right = 0.f; }; // Rack volts

// Construct/destroy off the audio thread. A bridge owns transport only: replacing
// it never clears the fixed-rate Core's sample memory, filters or random state.
// At 48 kHz frame h is returned at h+96. At other rates the transport delay is
// ceil(96*hostRate/48000), plus the input/output resampler group delays.
class RateBridge {
public:
    static constexpr unsigned capacity = 4096;
    static constexpr unsigned maxCorePerHost = 8;
    static constexpr unsigned maxHostPerBlock = 1538;
    static bool supported(float rate) noexcept {
        return std::isfinite(rate) && rate >= 8000.f && rate <= 768000.f && std::floor(rate) == rate;
    }
    explicit RateBridge(unsigned rate) : rate_(rate), history_(new HostControls[capacity]),
        events_(new TimedEvent[capacity]), output_(new HostAudio[capacity]) {
        if (!supported(float(rate))) return;
        if (rate != 48000) {
            int error = 0;
            inputResampler_ = speex_resampler_init(2, rate, 48000, 5, &error);
            if (!inputResampler_ || error != RESAMPLER_ERR_SUCCESS) return;
            outputResampler_ = speex_resampler_init(2, 48000, rate, 5, &error);
            if (!outputResampler_ || error != RESAMPLER_ERR_SUCCESS) return;
            inputLatencyCore_ = unsigned(speex_resampler_get_output_latency(inputResampler_));
            outputLatencyHost_ = unsigned(speex_resampler_get_output_latency(outputResampler_));
        }
        delayHost_ = unsigned(ceilDivide(std::uint64_t(blockFrames) * rate_, 48000));
        outputWrite_ = delayHost_; // value-initialized silent FIFO
        valid_ = delayHost_ + maxHostPerBlock < capacity
            && ceilDivide(std::uint64_t(inputLatencyCore_) * rate_, 48000) + 32 < capacity;
    }
    ~RateBridge() {
        if (inputResampler_) speex_resampler_destroy(inputResampler_);
        if (outputResampler_) speex_resampler_destroy(outputResampler_);
    }
    RateBridge(const RateBridge&) = delete;
    RateBridge& operator=(const RateBridge&) = delete;
    bool valid() const noexcept { return valid_; }
    bool failed() const noexcept { return fault_; }
    unsigned rate() const noexcept { return rate_; }
    unsigned inputLatencyCore() const noexcept { return inputLatencyCore_; }
    unsigned outputLatencyHost() const noexcept { return outputLatencyHost_; }
    unsigned transportDelayHost() const noexcept { return delayHost_; }
    double latencyHostFrames() const noexcept {
        return delayHost_ + double(inputLatencyCore_) * rate_ / 48000.0 + outputLatencyHost_;
    }
    std::uint64_t coreFrames() const noexcept { return coreFrames_; }
    std::uint64_t renderedBlocks() const noexcept { return coreFrames_ / blockFrames; }
    std::uint64_t underruns() const noexcept { return underruns_; }
    unsigned queueHighWater() const noexcept { return queueHighWater_; }
    unsigned pendingOutput() const noexcept { return unsigned(outputWrite_ - outputRead_); }
    unsigned pendingEvents() const noexcept { return unsigned(eventWrite_ - eventRead_); }
    unsigned pendingCommands() const noexcept { return unsigned(commandWrite_ - commandRead_); }
    const std::array<bool, 5>& gateHigh() const noexcept { return gateHigh_; }

    // Preserve Schmitt hysteresis when a prepared bridge replaces a used one.
    // Current voltages still override it outside the hysteresis band.
    void resumeGates(const std::array<bool, 5>& previous) noexcept {
        if (!hostFrames_) { gateHigh_ = previous; haveGateBaseline_ = true; }
    }
    // A replacement bridge may fade its output after its silent transport prime.
    // Duration is 240 core frames converted to host time, not 240 host samples.
    void fadeIn() noexcept { if (!hostFrames_) fadeFrames_ = unsigned(ceilDivide(std::uint64_t(240) * rate_, 48000)); }
    // Transfer accepted but not rendered UI commands across a transport swap.
    // Gate edges belong to the abandoned audio timeline; only gate levels rebase.
    bool carryCommands(const RateBridge& previous) noexcept {
        if (hostFrames_ || pendingCommands()) return false;
        for (auto i = previous.commandRead_; i != previous.commandWrite_; ++i) {
            const auto& command = previous.commands_[i % previous.commands_.size()];
            if (!queueCommand(inputLatencyCore_, command)) return false;
        }
        return true;
    }

    template <typename Renderer>
    HostAudio step(const HostFrame& host, Renderer&& render) noexcept {
        if (!valid_ || fault_) return {};
        history_[hostFrames_ % capacity] = host.controls;
        if (!queueHost(host)) { fault_ = true; return {}; }
        const float l = host.leftConnected ? finiteOrZero(host.leftVolts) / 5.f : 0.f;
        const float r = host.rightConnected ? finiteOrZero(host.rightVolts) / 5.f : l;
        const float input[2] = {l, r};
        float converted[maxCorePerHost * 2];
        spx_uint32_t used = 1, produced = maxCorePerHost;
        if (rate_ == 48000) { converted[0] = l; converted[1] = r; produced = 1; }
        else if (speex_resampler_process_interleaved_float(inputResampler_, input, &used, converted, &produced)
                 != RESAMPLER_ERR_SUCCESS || used != 1 || produced > maxCorePerHost) {
            fault_ = true; return {};
        }
        for (unsigned i = 0; i < produced; ++i)
            if (!coreFrame(converted + 2 * i, render)) { fault_ = true; return {}; }
        ++hostFrames_;
        if (outputRead_ == outputWrite_) { ++underruns_; fault_ = true; return {}; }
        HostAudio result = output_[outputRead_++ % capacity];
        if (fadeFrames_) {
            const double beginning = latencyHostFrames();
            const double age = double(hostFrames_ - 1) - beginning;
            if (age >= fadeFrames_) fadeFrames_ = 0;
            else {
                const float gain = float(std::max(0.0, age / fadeFrames_));
                result.left *= gain; result.right *= gain;
            }
        }
        return result;
    }
private:
    struct TimedEvent { std::uint64_t frame = 0; HostCommand command; bool clock = false, user = false; };
    unsigned rate_ = 0, delayHost_ = 0, inputLatencyCore_ = 0, outputLatencyHost_ = 0, fadeFrames_ = 0;
    bool valid_ = false, fault_ = false, haveGateBaseline_ = false, hasClockEdge_ = false;
    std::array<bool, 5> gateHigh_ {};
    std::unique_ptr<HostControls[]> history_;
    std::unique_ptr<TimedEvent[]> events_;
    std::unique_ptr<HostAudio[]> output_;
    SpeexResamplerState* inputResampler_ = nullptr;
    SpeexResamplerState* outputResampler_ = nullptr;
    std::uint64_t hostFrames_ = 0, coreFrames_ = 0, lastClockEdge_ = 0;
    std::uint64_t eventRead_ = 0, eventWrite_ = 0, outputRead_ = 0, outputWrite_ = 0, underruns_ = 0;
    unsigned queueHighWater_ = 0;
    std::array<HostCommand, 64> commands_ {};
    std::uint64_t commandRead_ = 0, commandWrite_ = 0;
    unsigned blockCommandCount_ = 0;
    BufferBlockInput block_;
    std::array<float, blockFrames * 2> blockAudio_ {}, blockOutput_ {};
    static std::uint64_t ceilDivide(std::uint64_t n, unsigned d) noexcept { return (n + d - 1) / d; }
    bool push(std::uint64_t frame, const HostCommand& command, bool clock = false, bool user = false) noexcept {
        if (eventWrite_ - eventRead_ == capacity) return false;
        auto& event = events_[eventWrite_++ % capacity]; event.frame = frame; event.command = command; event.clock = clock; event.user = user;
        queueHighWater_ = std::max(queueHighWater_, unsigned(eventWrite_ - eventRead_));
        return true;
    }
    bool queueCommand(std::uint64_t target, const HostCommand& command) noexcept {
        if (pendingCommands() == commands_.size() || !push(target, command, false, true)) return false;
        commands_[commandWrite_++ % commands_.size()] = command;
        return true;
    }
    bool queueHost(const HostFrame& host) noexcept {
        if (host.commandCount > host.commands.size()) return false;
        const auto target = ceilDivide(hostFrames_ * 48000, rate_) + inputLatencyCore_;
        const Action actions[] = {Action::ClockReset, Action::Freeze, Action::Bend, Action::Break, Action::Corrupt};
        for (unsigned i = 0; i < 5; ++i) {
            const bool previous = gateHigh_[i];
            const float v = finiteOrZero(host.gateVolts[i]);
            gateHigh_[i] = (haveGateBaseline_ || hostFrames_) && previous ? v > .1f : v >= 1.f;
            const bool rising = hostFrames_ && !previous && gateHigh_[i];
            HostCommand command;
            if (!i) {
                if (!rising) continue;
                // Reject physically too-close input edges before timestamp
                // quantization; they must not poison the clock median estimate.
                if (hasClockEdge_ && (hostFrames_ - lastClockEdge_) * 48000 < rate_) continue;
                hasClockEdge_ = true; lastClockEdge_ = hostFrames_;
                if (!push(target, command, true)) return false;
            } else if (!hostFrames_ || previous != gateHigh_[i]) {
                command.control.kind = EventKind::Gate; command.control.action = actions[i];
                command.control.high = gateHigh_[i]; command.control.rising = rising;
                if (!push(target, command)) return false;
            }
        }
        for (unsigned i = 0; i < host.commandCount; ++i)
            if (!queueCommand(target, host.commands[i])) return false;
        return true;
    }
    template <typename Renderer>
    bool coreFrame(const float* input, Renderer& render) noexcept {
        const unsigned frame = unsigned(coreFrames_ % blockFrames);
        if (!frame) {
            block_ = BufferBlockInput{};
            blockCommandCount_ = 0;
            const std::uint64_t source = coreFrames_ < inputLatencyCore_ ? 0
                : (coreFrames_ - inputLatencyCore_) * rate_ / 48000;
            const auto host = std::min(source, hostFrames_);
            if (hostFrames_ - host >= capacity) return false;
            const auto& controls = history_[host % capacity];
            block_.primary = controls.primary; block_.cv = controls.cv;
        }
        unsigned count = 0;
        while (eventRead_ != eventWrite_ && events_[eventRead_ % capacity].frame <= coreFrames_) {
            if (++count > 256) return false;
            const auto& event = events_[eventRead_++ % capacity];
            if (event.user) ++blockCommandCount_;
            if (event.clock) { block_.clockRises[frame] = true; continue; }
            const auto& command = event.command;
            switch (command.kind) {
            case CommandKind::Control: {
                auto control = command.control; control.frame = frame;
                if (!block_.events.append(control)) return false;
                break;
            }
            case CommandKind::Settings: block_.updateSettings = true; block_.settings = command.settings; break;
            case CommandKind::RestoreSecondary: block_.restoreSecondary = true; break;
            case CommandKind::RestartRandom: block_.restartRandom = true; block_.seed = command.seed; break;
            }
        }
        blockAudio_[frame * 2] = finiteOrZero(input[0]); blockAudio_[frame * 2 + 1] = finiteOrZero(input[1]);
        ++coreFrames_;
        if (frame + 1 != blockFrames) return true;
        render(blockAudio_.data(), blockOutput_.data(), block_);
        commandRead_ += blockCommandCount_;
        float converted[maxHostPerBlock * 2];
        const float* result = blockOutput_.data();
        spx_uint32_t used = blockFrames, produced = maxHostPerBlock;
        if (rate_ == 48000) produced = blockFrames;
        else {
            if (speex_resampler_process_interleaved_float(outputResampler_, result, &used, converted, &produced)
                != RESAMPLER_ERR_SUCCESS || used != blockFrames || produced > maxHostPerBlock) return false;
            result = converted;
        }
        if (outputWrite_ - outputRead_ + produced > capacity) return false;
        for (unsigned i = 0; i < produced; ++i) {
            auto& out = output_[outputWrite_++ % capacity];
            out.left = finiteOrZero(result[i * 2] * 5.f); out.right = finiteOrZero(result[i * 2 + 1] * 5.f);
        }
        return true;
    }
};
} // namespace tiamat
