#pragma once

#include "ChimeraTypes.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <vector>

namespace chimera {

struct ReelPage { StereoFrame frames[kPageFrames]; };

// A single off-audio producer owns fixed-address chunks. Publication is
// monotonic; no chunk is resized or freed while any Reel reader exists.
class ScratchPages {
public:
    static constexpr unsigned kChunkPages = 1024; // 2 MiB, prefaulted before publication.
    static constexpr unsigned kMaxPages = 8192;   // 16 MiB per resident Reel.
    explicit ScratchPages(unsigned maximum) : maximum_(std::min(maximum, unsigned(kMaxPages))) {
        grow();
    }
    unsigned readyPages() const { return ready_.load(std::memory_order_acquire); }
    unsigned maximumPages() const { return maximum_; }
    ReelPage& page(unsigned index) const { return chunks_[index / kChunkPages][index % kChunkPages]; }
    void requestMore() { wanted_.store(true, std::memory_order_release); }
    // Only the replenishment worker (or a deterministic test) calls this.
    void replenish() {
        if (!wanted_.exchange(false, std::memory_order_acq_rel)) return;
        try { grow(); } catch (...) { failed_.store(true, std::memory_order_release); }
    }
    bool allocationFailed() const { return failed_.load(std::memory_order_acquire); }
private:
    void grow() {
        const unsigned start = ready_.load(std::memory_order_relaxed);
        if (start >= maximum_) return;
        const unsigned count = std::min(unsigned(kChunkPages), maximum_ - start);
        std::unique_ptr<ReelPage[]> pages(new ReelPage[count]);
        std::memset(pages.get(), 0, count * sizeof(ReelPage));
        chunks_[start / kChunkPages] = std::move(pages);
        ready_.store(start + count, std::memory_order_release);
        failed_.store(false, std::memory_order_release);
    }
    const unsigned maximum_;
    std::array<std::unique_ptr<ReelPage[]>, kMaxPages / kChunkPages> chunks_;
    std::atomic<unsigned> ready_{0};
    std::atomic<bool> wanted_{false}, failed_{false};
};

// Independent of the two disk workers and module control/save mutexes. Weak
// registrations prevent this service from extending module/Reel lifetimes.
class ScratchPageService {
public:
    static ScratchPageService& instance() { static ScratchPageService service; return service; }
    void add(const std::shared_ptr<ScratchPages>& pages) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (stopped_) throw std::runtime_error("Chimera scratch service is closed");
        pools_.push_back(pages);
        if (!worker_.joinable()) worker_ = std::thread([this] { run(); });
    }
    void shutdown() {
        { std::lock_guard<std::mutex> lock(mutex_); stopped_ = true; }
        if (worker_.joinable()) worker_.join();
    }
    ~ScratchPageService() { shutdown(); }
private:
    void run() {
        std::vector<std::shared_ptr<ScratchPages>> work;
        std::unique_lock<std::mutex> lock(mutex_);
        while (!stopped_) {
            for (auto it = pools_.begin(); it != pools_.end();) {
                if (auto pool = it->lock()) { work.push_back(std::move(pool)); ++it; }
                else it = pools_.erase(it);
            }
            lock.unlock();
            for (const auto& pool : work) pool->replenish();
            work.clear(); // Destruction stays off audio and outside registration lock.
            // No audio-side wakeup/syscall; this worker polls demand independently
            // of disk I/O. Shutdown waits at most one polling interval plus work.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            lock.lock();
        }
    }
    std::mutex mutex_;
    std::vector<std::weak_ptr<ScratchPages>> pools_;
    std::thread worker_;
    bool stopped_ = false;
};

} // namespace chimera
