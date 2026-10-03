#pragma once
#include "TiamatTransport.hpp"
#include "../SpscLatestSnapshot.hpp"
#include <chrono>
#include <mutex>
#include <thread>

namespace tiamat {
struct Preset { ControlState controls; SecondarySettings settings; };
struct RuntimeSnapshot {
    Preset preset;
    Status status;
    MappedControls mapped;
    EffectiveFlags effective;
    std::uint64_t generation = 0;
    bool busy = false, fault = false;
};

// Rack-independent ownership layer. Exactly one audio caller; control callers
// serialize through a mutex that the audio thread never touches. One worker
// prepares/retire transports and whole cleared cores, independent of widget life.
class Runtime {
public:
    Runtime() : active_(new Prepared(1, Preset{}, 48000)) {
        published_.store(active_, std::memory_order_release);
        worker_ = std::thread([this] { work(); });
    }
    ~Runtime() {
        stop_.store(true, std::memory_order_release);
        if (worker_.joinable()) worker_.join();
        delete active_; delete ready_.exchange(nullptr); delete retired_.exchange(nullptr);
    }
    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;
    std::uint64_t generation() const noexcept { return wanted_.load(std::memory_order_acquire); }
    const RuntimeSnapshot& audioSnapshot() const noexcept { return audioSnapshot_; }
    const Transport& audioTransport() const noexcept { return active_->transport; }

    // Patch load / full reset: cleared state is prepared by the worker. Old
    // commands carry their generation and cannot reappear in the replacement.
    void reset(Preset preset = Preset{}) {
        std::lock_guard<std::mutex> lock(controlMutex_);
        preset.controls.buttonFreeze = false;
        requested_ = preset;
        wanted_.fetch_add(1, std::memory_order_release);
        commandFault_.store(false, std::memory_order_release);
    }
    RuntimeSnapshot snapshot() const {
        std::lock_guard<std::mutex> lock(controlMutex_);
        RuntimeSnapshot result = snapshots_.readLatest();
        const auto wanted = wanted_.load(std::memory_order_acquire);
        if (result.generation != wanted) {
            result = RuntimeSnapshot{}; result.preset = requested_; result.busy = true; result.generation = wanted;
        }
        // Settings/seed are canonical control-side values even before adoption.
        result.preset.settings = requested_.settings;
        result.preset.controls.seed = requested_.controls.seed;
        result.fault |= commandFault_.load(std::memory_order_acquire) || preparationFault_.load(std::memory_order_acquire);
        return result;
    }
    template <typename Edit> bool editSettings(Edit edit) {
        std::lock_guard<std::mutex> lock(controlMutex_);
        HostCommand command; command.kind = CommandKind::Settings;
        command.settings = requested_.settings; edit(command.settings);
        if (!enqueue(command)) return false;
        requested_.settings = command.settings; return true;
    }
    bool restartRandom(std::uint64_t seed) {
        std::lock_guard<std::mutex> lock(controlMutex_);
        HostCommand command; command.kind = CommandKind::RestartRandom; command.seed = seed;
        if (!enqueue(command)) return false;
        requested_.controls.seed = seed; return true;
    }
    bool restoreSecondary() {
        std::lock_guard<std::mutex> lock(controlMutex_);
        HostCommand command; command.kind = CommandKind::RestoreSecondary;
        if (!enqueue(command)) return false;
        requested_.settings = SecondarySettings{}; return true;
    }
    bool action(Action action) {
        std::lock_guard<std::mutex> lock(controlMutex_);
        HostCommand command; command.control.action = action; return enqueue(command);
    }

    HostAudio step(const HostFrame& input, float rate) noexcept {
        hostRate_.store(RateBridge::supported(rate) ? unsigned(rate) : 48000u, std::memory_order_release);
        const auto wanted = wanted_.load(std::memory_order_acquire);
        if (seenGeneration_ != wanted) {
            seenGeneration_ = wanted; physicalRead_ = physicalWrite_ = 0;
        }
        // Reclaim obsolete command slots even while cleared memory is being
        // prepared. Rapid successive loads must not fill the queue with work
        // that has already been superseded.
        auto stale = commandRead_.load(std::memory_order_relaxed);
        const auto available = commandWrite_.load(std::memory_order_acquire);
        for (unsigned i = 0; stale != available && i < queueSize; ++i) {
            if (commands_[stale % queueSize].generation >= wanted) break;
            ++stale;
        }
        commandRead_.store(stale, std::memory_order_release);
        for (unsigned i = 0; i < input.commandCount && i < input.commands.size(); ++i) {
            if (physicalWrite_ - physicalRead_ == physical_.size()) { commandFault_.store(true); break; }
            physical_[physicalWrite_++ % physical_.size()] = input.commands[i];
        }
        // Finish only the old partial input block; then wait silently for a
        // prepared replacement. A failed bridge is also a valid reset boundary.
        if (active_->generation != wanted) {
            const auto* bridge = active_->transport.bridge();
            const bool boundary = active_->transport.failed() || !RateBridge::supported(rate)
                || !bridge || bridge->rate() != unsigned(rate) || bridge->coreFrames() % blockFrames == 0;
            if (boundary && !retired_.load(std::memory_order_acquire)) {
                Prepared* next = ready_.exchange(nullptr, std::memory_order_acq_rel);
                if (next) {
                    if (next->generation == wanted) {
                        Prepared* old = active_; active_ = next;
                        published_.store(next, std::memory_order_release);
                        retired_.store(old, std::memory_order_release);
                    } else retired_.store(next, std::memory_order_release);
                }
            }
            if (active_->generation != wanted && boundary) { publish(true); return {}; }
            if (active_->generation != wanted) {
                HostFrame finish = input; finish.commandCount = 0;
                active_->transport.step(finish, rate); publish(true); return {};
            }
        }
        HostFrame host = input; host.commandCount = 0;
        if (active_->initialSeed) {
            auto& seed = host.commands[host.commandCount++]; seed.kind = CommandKind::RestartRandom;
            seed.seed = active_->preset.controls.seed; active_->initialSeed = false;
        }
        auto read = commandRead_.load(std::memory_order_relaxed);
        const auto write = commandWrite_.load(std::memory_order_acquire);
        unsigned scanned = 0;
        while (read != write && host.commandCount < host.commands.size() && scanned++ < queueSize) {
            const auto& command = commands_[read % queueSize];
            if (command.generation > active_->generation) break;
            if (command.generation == active_->generation) host.commands[host.commandCount++] = command.value;
            ++read;
        }
        commandRead_.store(read, std::memory_order_release);
        while (physicalRead_ != physicalWrite_ && host.commandCount < host.commands.size())
            host.commands[host.commandCount++] = physical_[physicalRead_++ % physical_.size()];
        const auto result = active_->transport.step(host, rate);
        const auto* bridge = active_->transport.bridge();
        const auto blocks = bridge ? bridge->renderedBlocks() : 0;
        if (blocks != lastBlocks_ || audioSnapshot_.generation != active_->generation || active_->transport.failed()) {
            lastBlocks_ = blocks; publish(false);
        }
        return result;
    }
private:
    struct Prepared {
        Transport transport;
        Preset preset;
        std::uint64_t generation;
        bool initialSeed = true;
        Prepared(std::uint64_t generation, Preset preset, unsigned rate) : transport(rate), preset(preset), generation(generation) {
            this->preset.controls.buttonFreeze = false;
            transport.core().eventState().controls = this->preset.controls;
            transport.core().eventState().settings = preset.settings;
        }
    };
    struct Queued { HostCommand value; std::uint64_t generation = 0; };
    static constexpr unsigned queueSize = 64;
    std::array<Queued, queueSize> commands_ {};
    std::atomic<unsigned> commandRead_ {0}, commandWrite_ {0};
    std::array<HostCommand, 64> physical_ {};
    std::uint64_t physicalRead_ = 0, physicalWrite_ = 0, seenGeneration_ = 1, lastBlocks_ = 0;
    Prepared* active_;
    std::atomic<Prepared*> published_ {nullptr}, ready_ {nullptr}, retired_ {nullptr};
    std::atomic<std::uint64_t> wanted_ {1};
    std::atomic<unsigned> hostRate_ {48000};
    std::atomic<bool> stop_ {false}, commandFault_ {false}, preparationFault_ {false};
    mutable std::mutex controlMutex_;
    Preset requested_;
    std::thread worker_;
    mutable snapshot_transport::SpscLatestSnapshot<RuntimeSnapshot> snapshots_;
    RuntimeSnapshot audioSnapshot_;
    bool enqueue(const HostCommand& command) noexcept {
        const unsigned write = commandWrite_.load(std::memory_order_relaxed);
        if (write - commandRead_.load(std::memory_order_acquire) == queueSize) {
            commandFault_.store(true, std::memory_order_release); return false;
        }
        auto& slot = commands_[write % queueSize]; slot.value = command; slot.generation = wanted_.load(std::memory_order_acquire);
        commandWrite_.store(write + 1, std::memory_order_release); return true;
    }
    void publish(bool busy) noexcept {
        const auto& engine = active_->transport.core().bufferEngine();
        auto& events = active_->transport.core().eventState();
        RuntimeSnapshot value; value.preset.controls = events.controls; value.preset.settings = events.settings;
        value.generation = active_->generation; value.busy = busy;
        value.fault = active_->transport.failed() || active_->transport.preparationFailed() || commandFault_.load(std::memory_order_acquire);
        value.mapped = engine.mapped();
        value.effective = events.effective();
        value.status.mode = events.controls.mode; value.status.effect = events.controls.effect;
        value.status.freezeRequested = events.effective().freeze; value.status.freezeActive = engine.buffer().transition().freezeActive;
        value.status.clockLost = engine.clock().state().lost; value.status.capacityLimited = engine.clock().state().capacityLimited;
        for (unsigned ch = 0; ch < 2; ++ch) {
            const auto& c = engine.buffer().channel(ch);
            value.status.captureFrames[ch] = c.writer.captureFrames; value.status.sliceFrames[ch] = c.reader.sliceFrames; value.status.rates[ch] = c.reader.rate;
        }
        audioSnapshot_ = value; snapshots_.publish(value);
    }
    void work() noexcept {
        while (!stop_.load(std::memory_order_acquire)) {
            delete retired_.exchange(nullptr, std::memory_order_acq_rel);
            const auto wanted = wanted_.load(std::memory_order_acquire);
            Prepared* live = published_.load(std::memory_order_acquire);
            if (live) live->transport.prepare();
            // The worker alone deletes retired objects, at the start of the
            // next iteration; live remains valid through this iteration.
            if (live && live->generation != wanted) {
                Prepared* pending = ready_.load(std::memory_order_acquire);
                if (!pending || pending->generation != wanted) {
                    delete ready_.exchange(nullptr, std::memory_order_acq_rel);
                    Preset preset; std::uint64_t captured;
                    { std::lock_guard<std::mutex> lock(controlMutex_); preset = requested_; captured = wanted_.load(std::memory_order_acquire); }
                    std::unique_ptr<Prepared> next;
                    try { next.reset(new Prepared(captured, preset, hostRate_.load(std::memory_order_acquire))); }
                    catch (...) { preparationFault_.store(true, std::memory_order_release); }
                    if (next && wanted_.load(std::memory_order_acquire) == captured) {
                        Prepared* empty = nullptr;
                        if (ready_.compare_exchange_strong(empty, next.get(), std::memory_order_release)) next.release();
                        preparationFault_.store(false, std::memory_order_release);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
};
} // namespace tiamat
