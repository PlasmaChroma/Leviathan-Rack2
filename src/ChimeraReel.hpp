#pragma once

#include "ChimeraTypes.hpp"
#include <atomic>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <stdexcept>

namespace chimera {

struct Marker { std::uint32_t frame; std::uint32_t id; };
struct SnapshotMetadata {
    std::uint32_t validFrames;
    std::uint32_t pageCount;
    std::uint16_t markerCount;
    Marker markers[kMaxSplices];
    std::uint64_t documentRevision;
    std::uint64_t audioRevision;
    std::uint64_t capturedThroughFrame;
};

// A Reel is prepared/destroyed off audio. After adoption, exactly one core
// owner mutates it. A worker reads only a ready leased snapshot; its service
// retains the Reel's lifetime until every reader is done.
class Reel {
public:
    enum SnapshotState { Idle, Capturing, Ready, Reclaiming };
    static const std::uint32_t kInvalidPage = 0xffffffffu;
    static const std::uint32_t kScanPerTick = 8;

    Reel(std::uint32_t pages, std::uint32_t reservePages)
        : pages_(checkedPages(pages, reservePages)), reservePages_(reservePages),
          capacityFrames_(pages_ * kPageFrames),
          pool_(new Page[pages_ + reservePages_]), active_(new std::uint32_t[pages_]),
          frozen_(new std::uint32_t[pages_]), captured_(new std::uint8_t[pages_]),
          free_(new std::uint32_t[reservePages ? reservePages : 1]),
          freeCount_(reservePages), validFrames_(0), markerCount_(0), nextMarkerId_(1),
          documentRevision_(0), audioRevision_(0), capturedThroughFrame_(0),
          state_(Idle), publishedReady_(false), scan_(0), reclaim_(0),
          cowCopies_(0), overflow_(false), recordingStopped_(false) {
        for (std::uint32_t i = 0; i < pages_; ++i) {
            active_[i] = i;
            frozen_[i] = kInvalidPage;
            captured_[i] = 0;
        }
        for (std::uint32_t i = 0; i < reservePages_; ++i) free_[i] = pages_ + i;
        for (std::uint32_t i = 0; i < pages_ + reservePages_; ++i)
            std::memset(&pool_[i], 0, sizeof(Page)); // Prefault off audio.
        std::memset(&snapshot_, 0, sizeof(snapshot_));
    }
    Reel(const Reel&) = delete;
    Reel& operator=(const Reel&) = delete;

    std::uint32_t capacityFrames() const { return capacityFrames_; }
    std::uint32_t validFrames() const { return validFrames_; }
    std::uint16_t markerCount() const { return markerCount_; }
    std::uint64_t documentRevision() const { return documentRevision_; } // Core owner only.
    std::uint64_t audioRevision() const { return audioRevision_; } // Core owner only.
    void restoreRevisions(std::uint64_t document, std::uint64_t audio) {
        // Loading owns this newly decoded Reel off audio, before adoption.
        documentRevision_ = document;
        audioRevision_ = audio;
    }
    std::uint32_t markerId(std::uint16_t index) const {
        return index < markerCount_ ? markers_[index].id : 0;
    }
    std::uint16_t findMarkerId(std::uint32_t id) const {
        for (std::uint16_t i = 0; i < markerCount_; ++i)
            if (markers_[i].id == id) return i;
        return markerCount_;
    }
    Region region(std::uint16_t index) const {
        if (index >= markerCount_) return Region{0, 0};
        return Region{markers_[index].frame,
                      index + 1 < markerCount_ ? markers_[index + 1].frame : validFrames_};
    }
    std::uint32_t freePages() const { return freeCount_; }
    std::uint32_t scannedPages() const { return scan_; }
    std::uint32_t reclaimCursor() const { return reclaim_; }
    std::uint64_t cowCopies() const { return cowCopies_; }
    bool overflowed() const { return overflow_; }
    bool recordingStopped() const { return recordingStopped_; }
    SnapshotState state() const { return state_; } // Core owner only.
    bool readyForWorker() const { return publishedReady_.load(std::memory_order_acquire); }
    const SnapshotMetadata& snapshotMetadata() const { return snapshot_; } // After ready.
    std::uint64_t rawAudioBytes() const {
        return static_cast<std::uint64_t>(pages_ + reservePages_) * sizeof(Page);
    }

    // Core-owner operation. Valid-length advancement and samples share a cut.
    bool write(std::uint32_t frame, StereoFrame value, std::uint64_t coreFrame) {
        if (recordingStopped_ || frame >= capacityFrames_) return false;
        const std::uint32_t logical = frame / kPageFrames;
        if (state_ == Capturing || state_ == Ready) {
            if (logical < snapshot_.pageCount && !captured_[logical]) capturePage(logical);
            if (logical < snapshot_.pageCount && active_[logical] == frozen_[logical]) {
                if (!freeCount_) {
                    overflow_ = true;
                    recordingStopped_ = true;
                    return false;
                }
                const std::uint32_t replacement = free_[--freeCount_];
                std::memcpy(&pool_[replacement], &pool_[active_[logical]], sizeof(Page));
                active_[logical] = replacement;
                ++cowCopies_;
            }
        }
        pool_[active_[logical]].frames[frame % kPageFrames] = value;
        if (!validFrames_) {
            markers_[0] = Marker{0, nextMarkerId_++};
            markerCount_ = 1;
            ++documentRevision_;
        }
        if (frame >= validFrames_) validFrames_ = frame + 1;
        ++audioRevision_; // Phase 3 will coalesce 256-frame write blocks.
        capturedThroughFrame_ = coreFrame;
        return true;
    }

    StereoFrame readActive(std::uint32_t frame) const {
        if (frame >= validFrames_) return StereoFrame{0.f, 0.f};
        return pool_[active_[frame / kPageFrames]].frames[frame % kPageFrames];
    }

    bool addMarker(std::uint32_t frame) {
        if (frame >= validFrames_ || markerCount_ >= kMaxSplices) return false;
        std::uint16_t at = 0;
        while (at < markerCount_ && markers_[at].frame < frame) ++at;
        if (at < markerCount_ && markers_[at].frame == frame) return false;
        for (std::uint16_t i = markerCount_; i > at; --i) markers_[i] = markers_[i - 1];
        markers_[at] = Marker{frame, nextMarkerId_++};
        ++markerCount_;
        ++documentRevision_;
        return true;
    }

    // Off-audio import/restore path. Validate the complete table before
    // replacing the active marker identity and ordering.
    bool replaceMarkers(const Marker* markers, std::uint16_t count) {
        if (!validFrames_ || !markers || !count || count > kMaxSplices ||
            markers[0].frame != 0) return false;
        std::uint32_t greatestId = 0;
        for (std::uint16_t i = 0; i < count; ++i) {
            if (!markers[i].id || markers[i].frame >= validFrames_ ||
                (i && markers[i].frame <= markers[i - 1].frame)) return false;
            for (std::uint16_t j = 0; j < i; ++j)
                if (markers[i].id == markers[j].id) return false;
            if (markers[i].id > greatestId) greatestId = markers[i].id;
        }
        if (greatestId == UINT32_MAX) return false;
        for (std::uint16_t i = 0; i < count; ++i) markers_[i] = markers[i];
        markerCount_ = count;
        nextMarkerId_ = greatestId + 1;
        ++documentRevision_;
        return true;
    }

    bool beginSnapshot(std::uint64_t capturedThroughFrame) {
        if (state_ != Idle || overflow_) return false;
        snapshot_.validFrames = validFrames_;
        snapshot_.pageCount = (validFrames_ + kPageFrames - 1) / kPageFrames;
        snapshot_.markerCount = markerCount_;
        for (std::uint16_t i = 0; i < markerCount_; ++i) snapshot_.markers[i] = markers_[i];
        snapshot_.documentRevision = documentRevision_;
        snapshot_.audioRevision = audioRevision_;
        snapshot_.capturedThroughFrame = capturedThroughFrame;
        scan_ = reclaim_ = 0;
        publishedReady_.store(false, std::memory_order_release);
        state_ = Capturing;
        if (!snapshot_.pageCount) publishReady();
        return true;
    }

    // One call per core frame, or under Maintenance ownership while stopped.
    std::uint32_t maintenanceTick() {
        std::uint32_t processed = 0;
        if (state_ == Capturing) {
            while (processed < kScanPerTick && scan_ < snapshot_.pageCount) {
                capturePage(scan_++);
                ++processed;
            }
            if (scan_ == snapshot_.pageCount) publishReady();
        }
        else if (state_ == Reclaiming) {
            while (processed < kScanPerTick && reclaim_ < snapshot_.pageCount) {
                const std::uint32_t i = reclaim_++;
                if (captured_[i]) {
                    if (active_[i] != frozen_[i]) free_[freeCount_++] = frozen_[i];
                    frozen_[i] = kInvalidPage;
                    captured_[i] = 0;
                }
                ++processed;
            }
            if (reclaim_ == snapshot_.pageCount) state_ = Idle;
        }
        return processed;
    }

    // Service sends release only after its final reader has finished. The core
    // owner calls this from its command path, never from a worker thread.
    bool beginRelease() {
        if (state_ != Ready) return false;
        publishedReady_.store(false, std::memory_order_release);
        state_ = Reclaiming;
        reclaim_ = 0;
        if (!snapshot_.pageCount) state_ = Idle;
        return true;
    }

    // Called only while the service owns a ready lease. Snapshot IDs and
    // samples stay immutable until all consumers release it.
    StereoFrame readSnapshot(std::uint32_t frame) const {
        if (!readyForWorker() || frame >= snapshot_.validFrames) return StereoFrame{0.f, 0.f};
        const std::uint32_t logical = frame / kPageFrames;
        return pool_[frozen_[logical]].frames[frame % kPageFrames];
    }

private:
    struct Page { StereoFrame frames[kPageFrames]; };
    static std::uint32_t checkedPages(std::uint32_t pages, std::uint32_t reservePages) {
        if (!pages || pages > kMaxPages || reservePages > pages)
            throw std::invalid_argument("Chimera Reel capacity/reserve outside profile-1 bounds");
        return pages;
    }
    void capturePage(std::uint32_t logical) {
        if (!captured_[logical]) {
            frozen_[logical] = active_[logical];
            captured_[logical] = 1;
        }
    }
    void publishReady() {
        state_ = Ready;
        publishedReady_.store(true, std::memory_order_release);
    }

    const std::uint32_t pages_, reservePages_, capacityFrames_;
    std::unique_ptr<Page[]> pool_;
    std::unique_ptr<std::uint32_t[]> active_, frozen_;
    std::unique_ptr<std::uint8_t[]> captured_;
    std::unique_ptr<std::uint32_t[]> free_;
    std::uint32_t freeCount_, validFrames_;
    Marker markers_[kMaxSplices];
    std::uint16_t markerCount_;
    std::uint32_t nextMarkerId_;
    std::uint64_t documentRevision_, audioRevision_, capturedThroughFrame_;
    SnapshotMetadata snapshot_;
    SnapshotState state_;
    std::atomic<bool> publishedReady_;
    std::uint32_t scan_, reclaim_;
    std::uint64_t cowCopies_;
    bool overflow_, recordingStopped_;
};

} // namespace chimera
