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
#ifdef CHIMERA_RATE_SERVICE_TEST_HOOKS
std::atomic<void (*)(unsigned)> ratePreparationHook{nullptr};
#endif

class RatePreparationService {
public:
    ~RatePreparationService() { shutdown(); }
    void add(const RateBridgeSlot& slot) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closing_) return;
        for (const Registration& existing : slots_)
            if (existing.slot.prepared == slot.prepared) return;
        slots_.push_back({slot, nextIdentity_++});
        if (!worker_.joinable()) worker_ = std::thread(&RatePreparationService::run, this);
        cv_.notify_one();
    }
    void remove(std::atomic<RateBridge*>* identity) {
        std::lock_guard<std::mutex> lock(mutex_);
        slots_.erase(std::remove_if(slots_.begin(), slots_.end(),
            [identity](const Registration& entry) {
                return entry.slot.prepared == identity;
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
    struct Registration { RateBridgeSlot slot; std::uint64_t identity; };
    struct Work {
        std::uint64_t identity;
        unsigned wanted;
        RateBridge* retired;
        RateBridge* discarded;
    };
    void run() {
        std::unique_lock<std::mutex> lock(mutex_);
        std::vector<Work> work;
        while (!closing_) {
            work.clear();
            work.reserve(slots_.size());
            for (const Registration& entry : slots_) {
                const RateBridgeSlot& slot = entry.slot;
                Work item{entry.identity, 0,
                    slot.retired->exchange(nullptr, std::memory_order_acq_rel), nullptr};
                const unsigned wanted = slot.requested->load(std::memory_order_acquire);
                const unsigned active = slot.active->load(std::memory_order_acquire);
                RateBridge* pending = slot.prepared->load(std::memory_order_acquire);
                if (pending && (wanted == active || wanted == 48000 ||
                                wanted == 0 || pending->rate() != wanted)) {
                    item.discarded = slot.prepared->exchange(nullptr, std::memory_order_acq_rel);
                    pending = nullptr;
                }
                if (wanted && wanted != 48000 && wanted != active && !pending)
                    item.wanted = wanted;
                work.push_back(item);
            }
            lock.unlock();
            for (const Work& item : work) {
                delete item.retired;
                delete item.discarded;
                if (!item.wanted) continue;
                std::unique_ptr<RateBridge> next;
                try {
#ifdef CHIMERA_RATE_SERVICE_TEST_HOOKS
                    if (auto hook = ratePreparationHook.load()) hook(item.wanted);
#endif
                    next.reset(new RateBridge(item.wanted));
                }
                catch (...) {}
                // No module-owned pointer is used while unlocked. Removal may
                // destroy a module during construction, and even reuse its
                // address: only the original registration identity may publish.
                lock.lock();
                for (const Registration& entry : slots_) {
                    if (entry.identity != item.identity) continue;
                    const RateBridgeSlot& slot = entry.slot;
                    if (slot.requested->load(std::memory_order_acquire) != item.wanted ||
                        slot.active->load(std::memory_order_acquire) == item.wanted) break;
                    if (!next || !next->valid()) slot.error->store(true, std::memory_order_release);
                    else {
                        RateBridge* empty = nullptr;
                        if (slot.prepared->compare_exchange_strong(empty, next.get(),
                                std::memory_order_acq_rel)) next.release();
                    }
                    break;
                }
                lock.unlock(); // Rejected bridges are also destroyed outside the lock.
            }
            lock.lock();
            cv_.wait_for(lock, std::chrono::milliseconds(5), [this] { return closing_; });
        }
    }
    std::mutex mutex_;
    std::condition_variable cv_;
    std::thread worker_;
    std::vector<Registration> slots_;
    std::uint64_t nextIdentity_ = 1;
    bool closing_ = false;
};
RatePreparationService ratePreparationService;
}

#ifdef CHIMERA_RATE_SERVICE_TEST_HOOKS
void setRatePreparationHook(void (*hook)(unsigned)) { ratePreparationHook.store(hook); }
#endif
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
    ScratchPageService::instance().shutdown();
    std::shared_ptr<IoService> local;
    {
        std::lock_guard<std::mutex> lock(serviceMutex);
        closing = true;
        local.swap(service);
    }
    if (local) local->shutdown(); // Join off audio and outside the global lock.
}

} // namespace chimera
