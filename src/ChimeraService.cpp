#include "ChimeraService.hpp"
#include "ChimeraRateBridge.hpp"
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace chimera {
namespace {
std::mutex serviceMutex;
std::shared_ptr<IoService> service;
bool closing = false;

class RatePreparationService {
public:
    ~RatePreparationService() { shutdown(); }
    void add(const RateBridgeSlot& slot) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closing_) return;
        for (const RateBridgeSlot& existing : slots_)
            if (existing.prepared == slot.prepared) return;
        slots_.push_back(slot);
        if (!worker_.joinable()) worker_ = std::thread(&RatePreparationService::run, this);
        cv_.notify_one();
    }
    void remove(std::atomic<RateBridge*>* identity) {
        std::lock_guard<std::mutex> lock(mutex_);
        slots_.erase(std::remove_if(slots_.begin(), slots_.end(),
            [identity](const RateBridgeSlot& slot) {
                return slot.prepared == identity;
            }), slots_.end());
    }
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closing_ = true;
            slots_.clear();
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
    }
private:
    void run() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!closing_) {
            for (const RateBridgeSlot& slot : slots_) {
                delete slot.retired->exchange(nullptr, std::memory_order_acq_rel);
                const unsigned wanted = slot.requested->load(std::memory_order_acquire);
                const unsigned active = slot.active->load(std::memory_order_acquire);
                RateBridge* pending = slot.prepared->load(std::memory_order_acquire);
                if (pending && (wanted == active || wanted == 48000 ||
                                wanted == 0 || pending->rate() != wanted)) {
                    delete slot.prepared->exchange(nullptr, std::memory_order_acq_rel);
                    pending = nullptr;
                }
                if (wanted == 0 || wanted == 48000 || wanted == active || pending)
                    continue;
                try {
                    std::unique_ptr<RateBridge> next(new RateBridge(wanted));
                    if (!next->valid()) {
                        slot.error->store(true, std::memory_order_release);
                        continue;
                    }
                    // A callback may have requested a newer rate while Speex
                    // was prepared. Publish only the still-current request.
                    if (slot.requested->load(std::memory_order_acquire) != wanted)
                        continue;
                    RateBridge* empty = nullptr;
                    if (slot.prepared->compare_exchange_strong(empty, next.get(),
                            std::memory_order_acq_rel)) next.release();
                }
                catch (...) { slot.error->store(true, std::memory_order_release); }
            }
            cv_.wait_for(lock, std::chrono::milliseconds(5));
        }
    }
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_;
    std::vector<RateBridgeSlot> slots_;
    bool closing_ = false;
};
RatePreparationService ratePreparationService;
}

void registerRateBridge(const RateBridgeSlot& slot) { ratePreparationService.add(slot); }
void unregisterRateBridge(std::atomic<RateBridge*>* prepared) {
    ratePreparationService.remove(prepared);
}

std::shared_ptr<IoService> chimeraIoService() {
    std::lock_guard<std::mutex> lock(serviceMutex);
    if (closing) return std::shared_ptr<IoService>();
    if (!service) service.reset(new IoService(2));
    return service;
}

void shutdownChimeraIoService() {
    ratePreparationService.shutdown();
    std::shared_ptr<IoService> local;
    {
        std::lock_guard<std::mutex> lock(serviceMutex);
        closing = true;
        local.swap(service);
    }
    if (local) local->shutdown(); // Join off audio and outside the global lock.
}

} // namespace chimera
