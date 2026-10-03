#pragma once
#include "TiamatCore.hpp"
#include "TiamatRateBridge.hpp"
#include <atomic>

namespace tiamat {
// One audio owner and one serialized preparation owner (UI or worker). Call
// prepare() off audio regularly. Construction/destruction require quiescent
// audio/preparation owners. No mutex, allocation or deletion in step().
class Transport {
public:
    explicit Transport(unsigned initialRate = 48000) : active_(new RateBridge(initialRate)),
        requested_(initialRate), activeRate_(initialRate) {}
    ~Transport() {
        delete active_;
        delete prepared_.exchange(nullptr);
        delete retired_.exchange(nullptr);
    }
    Transport(const Transport&) = delete;
    Transport& operator=(const Transport&) = delete;
    Core& core() noexcept { return core_; } // audio owner only
    const Core& core() const noexcept { return core_; } // audio owner only
    const RateBridge* bridge() const noexcept { return active_; } // audio owner only
    unsigned requestedRate() const noexcept { return requested_.load(std::memory_order_acquire); }
    bool preparationFailed() const noexcept { return preparationFailed_.load(std::memory_order_acquire); }
    bool failed() const noexcept { return commandFault_ || (active_ && active_->failed()); }

    // Can be called on a service worker; does not inspect or mutate live Core.
    void prepare() noexcept {
        delete retired_.exchange(nullptr, std::memory_order_acq_rel);
        const unsigned wanted = requested_.load(std::memory_order_acquire);
        if (!RateBridge::supported(float(wanted)) || wanted == activeRate_.load(std::memory_order_acquire)) return;
        if (preparedRate_ == wanted && prepared_.load(std::memory_order_acquire)) return;
        delete prepared_.exchange(nullptr, std::memory_order_acq_rel);
        std::unique_ptr<RateBridge> next;
        try { next.reset(new RateBridge(wanted)); } catch (...) { preparationFailed_.store(true, std::memory_order_release); return; }
        if (!next->valid()) { preparationFailed_.store(true, std::memory_order_release); return; }
        if (requested_.load(std::memory_order_acquire) != wanted) return;
        RateBridge* empty = nullptr;
        if (prepared_.compare_exchange_strong(empty, next.get(), std::memory_order_release, std::memory_order_relaxed)) {
            next.release(); preparedRate_ = wanted; preparationFailed_.store(false, std::memory_order_release);
        }
    }

    HostAudio step(const HostFrame& host, float hostRate) noexcept {
        if (commandFault_) return {};
        // Preserve host commands arriving while a replacement is being prepared.
        // Exhaustion is an explicit transport fault, never silent command loss.
        if (host.commandCount > host.commands.size() || pendingWrite_ - pendingRead_ + host.commandCount > pending_.size()) {
            commandFault_ = true; return {};
        }
        for (unsigned i = 0; i < host.commandCount; ++i) pending_[pendingWrite_++ % pending_.size()] = host.commands[i];
        const unsigned wanted = RateBridge::supported(hostRate) ? unsigned(hostRate) : 0;
        requested_.store(wanted, std::memory_order_release);
        if (!wanted) return {};
        if (!active_ || active_->rate() != wanted) {
            // One retirement slot makes ownership transfer bounded; a busy
            // preparation owner yields silence without dropping a pointer.
            if (retired_.load(std::memory_order_acquire)) return {};
            RateBridge* next = prepared_.exchange(nullptr, std::memory_order_acq_rel);
            if (!next) return {};
            if (next->rate() != wanted) { retired_.store(next, std::memory_order_release); return {}; }
            if (active_) {
                next->resumeGates(active_->gateHigh());
                if (!next->carryCommands(*active_)) { retired_.store(next, std::memory_order_release); return {}; }
            }
            next->fadeIn();
            RateBridge* previous = active_; active_ = next;
            activeRate_.store(wanted, std::memory_order_release);
            retired_.store(previous, std::memory_order_release);
        }
        auto render = [this](const float* input, float* output, const BufferBlockInput& block) {
            core_.processBlock(input, output, block);
        };
        if (pendingWrite_ == pendingRead_) return active_->step(host, render);
        HostFrame forwarded = host;
        forwarded.commandCount = unsigned(std::min<std::uint64_t>(pendingWrite_ - pendingRead_, forwarded.commands.size()));
        for (unsigned i = 0; i < forwarded.commandCount; ++i) forwarded.commands[i] = pending_[(pendingRead_ + i) % pending_.size()];
        auto result = active_->step(forwarded, render);
        if (!active_->failed()) pendingRead_ += forwarded.commandCount;
        return result;
    }
private:
    static_assert(ATOMIC_POINTER_LOCK_FREE == 2 && ATOMIC_INT_LOCK_FREE == 2, "Realtime transport requires lock-free pointer/int atomics");
    Core core_;
    RateBridge* active_ = nullptr;
    std::atomic<RateBridge*> prepared_ {nullptr}, retired_ {nullptr};
    std::atomic<unsigned> requested_, activeRate_;
    std::atomic<bool> preparationFailed_ {false};
    unsigned preparedRate_ = 0; // preparation owner only
    std::array<HostCommand, 64> pending_ {};
    std::uint64_t pendingRead_ = 0, pendingWrite_ = 0;
    bool commandFault_ = false;
};
} // namespace tiamat
