#pragma once

#include "ChimeraWaveform.hpp"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

namespace chimera {

struct WaveformExtrema {
    float lowL = 0.f, highL = 0.f, lowR = 0.f, highR = 0.f;
    bool stereo = false;
    void add(StereoFrame s) {
        if (!std::isfinite(s.l) || !std::isfinite(s.r)) return;
        lowL = std::min(lowL, s.l); highL = std::max(highL, s.l);
        lowR = std::min(lowR, s.r); highR = std::max(highR, s.r);
        stereo |= s.l != s.r;
    }
    void merge(WaveformSummary& out, unsigned bin) const {
        out.leftLow[bin] = std::min(out.leftLow[bin], lowL);
        out.leftHigh[bin] = std::max(out.leftHigh[bin], highL);
        out.rightLow[bin] = std::min(out.rightLow[bin], lowR);
        out.rightHigh[bin] = std::max(out.rightHigh[bin], highR);
        out.stereo |= stereo;
        out.peak = std::max(out.peak, std::max(std::max(-lowL, highL), std::max(-lowR, highR)));
    }
};

// One cache per module, touched only by the dedicated worker. Never exposed to
// rendering. Page versions survive physical COW moves; Reel identity prevents
// reuse across loads/edits even if an allocator recycles an address.
struct WaveformPageCache {
    struct Page {
        std::uint64_t revision = 0;
        unsigned frames = 0;
        WaveformExtrema extrema;
    };
    std::uint64_t identity = 0;
    std::vector<Page> pages;
};

struct WaveformJob {
    static const unsigned kPagesPerTurn = 32; // At most 8192 source frames.
    const Reel* source;
    const unsigned slot;
    const std::shared_ptr<WaveformPageCache> cache;
    std::atomic<bool> cancelled{false}, done{false};
    // Set by teardown before the module releases ownership. The worker never
    // reads this member; retaining the job retains the leased Reel.
    std::shared_ptr<Reel> teardownReel;
    std::shared_ptr<const WaveformSummary> result; // Read only after done acquire.
    std::uint64_t framesRead = 0, pagesRebuilt = 0, turns = 0;
    bool failed = false;

    WaveformJob(const Reel* reel, std::shared_ptr<WaveformPageCache> pageCache,
                unsigned snapshotSlot = 0)
        : source(reel), slot(snapshotSlot), cache(std::move(pageCache)) {}

    // Single worker (or deterministic test) only. A cancelled partial pass may
    // retain completed cache pages, but never publishes a partial display.
    bool advance() {
        if (cancelled.load(std::memory_order_acquire)) return finish(false);
        if (!summary_) {
            const auto& meta = source->snapshotMetadata(slot);
            summary_.reset(new WaveformSummary);
            summary_->frames = meta.validFrames;
            summary_->markerCount = meta.markerCount;
            summary_->documentRevision = meta.documentRevision;
            summary_->audioRevision = meta.audioRevision;
            for (unsigned i = 0; i < meta.markerCount; ++i)
                summary_->markers[i] = meta.markers[i].frame;
            pageCount_ = meta.pageCount;
            if (cache->identity != source->waveformIdentity()) {
                cache->pages.clear();
                cache->identity = source->waveformIdentity();
            }
            cache->pages.resize(pageCount_);
        }
        ++turns;
        const unsigned stop = std::min(pageCount_, cursor_ + kPagesPerTurn);
        for (; cursor_ < stop; ++cursor_) {
            const unsigned first = cursor_ * kPageFrames;
            const unsigned count = std::min(unsigned(kPageFrames), summary_->frames - first);
            auto& page = cache->pages[cursor_];
            const auto revision = source->snapshotPageRevision(cursor_, slot);
            const bool dirty = page.frames != count || page.revision != revision;
            const unsigned firstBin = bin(first), lastBin = bin(first + count - 1);
            // Only pages crossing a display-bin boundary require exact sample
            // placement. At most 511 pages do this, even for a full Reel.
            if (dirty || firstBin != lastBin) {
                const StereoFrame* samples = source->snapshotPage(cursor_, slot);
                WaveformExtrema all;
                for (unsigned i = 0; i < count; ++i) {
                    all.add(samples[i]);
                    if (firstBin != lastBin) {
                        WaveformExtrema one;
                        one.add(samples[i]);
                        one.merge(*summary_, bin(first + i));
                    }
                }
                framesRead += count;
                page.extrema = all; page.frames = count; page.revision = revision;
                if (dirty) ++pagesRebuilt;
            }
            if (firstBin == lastBin) page.extrema.merge(*summary_, firstBin);
        }
        if (cursor_ == pageCount_) return finish(true);
        return false;
    }
    void fail() { failed = true; finish(false); }
private:
    unsigned cursor_ = 0, pageCount_ = 0;
    std::shared_ptr<WaveformSummary> summary_;
    unsigned bin(unsigned frame) const {
        return unsigned(std::uint64_t(frame) * WaveformSummary::kBins / summary_->frames);
    }
    bool finish(bool publish) {
        if (publish && !cancelled.load(std::memory_order_acquire)) result = summary_;
        done.store(true, std::memory_order_release);
        return true;
    }
};

// Plugin-wide, one dedicated worker. Each admitted module gets one bounded
// turn before any module gets another. New arrivals join the tail. Control
// code coalesces newer revisions while its one job is in flight.
class WaveformService {
public:
    static const unsigned kMaxJobs = 64;
    explicit WaveformService(bool threaded = true) {
        if (threaded) worker_ = std::thread(&WaveformService::run, this);
    }
    ~WaveformService() { shutdown(); }
    bool submit(const std::shared_ptr<WaveformJob>& job) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (closing_ || jobs_.size() + bool(active_) >= kMaxJobs) return false;
        if (active_ && active_->cache == job->cache) return false;
        for (const auto& queued : jobs_) if (queued->cache == job->cache) return false;
        jobs_.push_back(job);
        cv_.notify_one();
        return true;
    }
    // Also used by deterministic fairness tests with threaded=false.
    bool runOneTurn() {
        std::shared_ptr<WaveformJob> job;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (jobs_.empty()) return false;
            job = jobs_.front(); jobs_.pop_front(); active_ = job;
        }
        bool complete = true;
        try { complete = job->advance(); } catch (...) { job->fail(); }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            active_.reset();
            if (!complete) jobs_.push_back(job);
        }
        return true;
    }
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closing_ = true;
            if (active_) active_->cancelled.store(true, std::memory_order_release);
            for (const auto& job : jobs_) job->cancelled.store(true, std::memory_order_release);
        }
        cv_.notify_all();
        if (worker_.joinable()) worker_.join();
        else while (runOneTurn()) {}
    }
private:
    void run() {
        for (;;) {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [&] { return closing_ || !jobs_.empty(); });
                if (closing_ && jobs_.empty()) return;
            }
            runOneTurn();
        }
    }
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<std::shared_ptr<WaveformJob>> jobs_;
    std::shared_ptr<WaveformJob> active_;
    bool closing_ = false;
    std::thread worker_;
};

} // namespace chimera
