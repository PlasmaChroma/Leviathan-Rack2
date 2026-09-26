#pragma once

#include "ChimeraTypes.hpp"
#include "ChimeraBlockMoments.hpp"
#include "ChimeraScratchPages.hpp"
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
    static const unsigned kSnapshotSlots = 3;
private:
    struct Snapshot {
        std::unique_ptr<std::uint32_t[]> frozen;
        std::unique_ptr<std::uint8_t[]> captured;
        SnapshotMetadata metadata{};
        SnapshotState state = Idle;
        std::atomic<bool> ready{false};
        unsigned scan = 0, reclaim = 0;
        void prepare(unsigned pages) {
            frozen.reset(new std::uint32_t[pages]);
            captured.reset(new std::uint8_t[pages]{});
            for (unsigned i = 0; i < pages; ++i) frozen[i] = kInvalidPage;
        }
    };
    typedef ReelPage Page;
public:

    Reel(std::uint32_t pages, std::uint32_t reservePages)
        : pages_(checkedPages(pages, reservePages)), reservePages_(reservePages),
          capacityFrames_(pages_ * kPageFrames), moments_(capacityFrames_),
          pool_(new Page[pages_ + reservePages_]), active_(new std::uint32_t[pages_]),
          free_(new std::uint32_t[pages_ + reservePages_]),
          references_(new std::uint8_t[pages_ + reservePages_]{}),
          freeCount_(reservePages), validFrames_(0), markerCount_(0), nextMarkerId_(1),
          documentRevision_(0), audioRevision_(0), capturedThroughFrame_(0),
          cowCopies_(0), overflow_(false), recordingStopped_(false) {
        snapshots_[0].prepare(pages_);
        for (unsigned i = 0; i < pages_; ++i) { active_[i] = i; references_[i] = 1; }
        for (unsigned i = 0; i < reservePages_; ++i) free_[i] = pages_ + i;
        std::memset(pool_.get(), 0, (pages_ + reservePages_) * sizeof(Page));
    }
    // Off-audio, before registry admission or publication. Reserve the maximum
    // budget, but allocate only a small initial scratch chunk. Standalone Reels
    // retain the original single-snapshot behavior unless explicitly enabled.
    void prepareRecordingSnapshots(bool background = true) {
        if (scratch_) return;
        const unsigned maximum = std::min(2 * pages_, unsigned(ScratchPages::kMaxPages));
        const unsigned physical = pages_ + reservePages_ + maximum;
        std::unique_ptr<std::uint32_t[]> free(new std::uint32_t[physical]);
        std::unique_ptr<std::uint8_t[]> refs(new std::uint8_t[physical]{});
        std::copy(free_.get(), free_.get() + freeCount_, free.get());
        std::copy(references_.get(), references_.get() + pages_ + reservePages_, refs.get());
        for (unsigned i = 1; i < kSnapshotSlots; ++i) snapshots_[i].prepare(pages_);
        auto scratch = std::make_shared<ScratchPages>(maximum);
        if (background) ScratchPageService::instance().add(scratch);
        free_ = std::move(free); references_ = std::move(refs); scratch_ = std::move(scratch);
        adoptScratch(scratch_->readyPages());
    }
    unsigned scratchPages() const { return scratch_ ? scratch_->readyPages() : 0; }
    void replenishScratchForTest() { if (scratch_) scratch_->replenish(); }
    bool supportsRecordingSnapshots() const { return bool(scratch_); }
    bool prepareRecording() {
        // Exhaustion never resumes a stopped take automatically. Once readers
        // have drained, a fresh REC may safely begin with recycled storage.
        if (recordingStopped_ && !hasSnapshots() && freeCount_) {
            recordingStopped_ = false;
            overflow_ = false;
        }
        return !recordingStopped_;
    }
    bool hasSnapshots() const {
        return activeSnapshotMask_ != 0;
    }
    bool snapshotWorkPending() const {
        return snapshotWorkMask_ != 0;
    }
    std::uint64_t budgetBytes() const {
        // Charge the growth cap, snapshot tables, refcounts, and freelist up front.
        const unsigned extra = scratch_ ? scratch_->maximumPages() : 0;
        const unsigned slots = scratch_ ? kSnapshotSlots : 1;
        return rawAudioBytes() + moments_.bytes() + std::uint64_t(extra) * sizeof(Page) +
            std::uint64_t(pages_ + reservePages_ + extra) * 5 +
            std::uint64_t(pages_) * (4 + slots * 5);
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
    std::uint32_t scannedPages() const { return snapshots_[0].scan; }
    std::uint32_t reclaimCursor() const { return snapshots_[0].reclaim; }
    std::uint64_t cowCopies() const { return cowCopies_; }
    bool overflowed() const { return overflow_; }
    bool recordingStopped() const { return recordingStopped_; }
    SnapshotState state(unsigned slot = 0) const { return snapshots_[slot].state; } // Core owner only.
    bool readyForWorker(unsigned slot = 0) const {
        return slot < kSnapshotSlots && snapshots_[slot].ready.load(std::memory_order_acquire);
    }
    const SnapshotMetadata& snapshotMetadata(unsigned slot = 0) const { return snapshots_[slot].metadata; } // After ready.
    std::uint64_t rawAudioBytes() const {
        return static_cast<std::uint64_t>(pages_ + reservePages_) * sizeof(Page);
    }
    std::uint64_t payloadBytes() const { return rawAudioBytes() + moments_.bytes(); }
    const BlockMoments::Block& playbackMoments(unsigned frame, unsigned size) const {
        return moments_.get(frame, size);
    }

    // Core-owner operation. Valid-length advancement and samples share a cut.
    bool write(std::uint32_t frame, StereoFrame value, std::uint64_t coreFrame) {
        return writeImpl<true>(frame, value, coreFrame);
    }

    // Off-audio only, on a fresh, exclusively owned Reel. Append sequentially,
    // then finishImport() before publishing it or using playback moments.
    bool appendImported(StereoFrame value) {
        if (hasSnapshots()) return false;
        return writeImpl<false>(validFrames_, value, validFrames_);
    }
    void finishImport() {
        moments_.buildImported(validFrames_, [this](unsigned frame) { return readActive(frame); });
    }

private:
    template<bool UpdateMoments>
    bool writeImpl(std::uint32_t frame, StereoFrame value, std::uint64_t coreFrame) {
        if (recordingStopped_ || frame >= capacityFrames_) return false;
        const std::uint32_t logical = frame / kPageFrames;
        if (activeSnapshotMask_) {
            for (auto& cut : snapshots_) {
                if (cut.state == Capturing && logical < cut.metadata.pageCount)
                    capturePage(cut, logical);
            }
        }
        if (activeSnapshotMask_ && references_[active_[logical]] > 1) {
            if (!freeCount_) {
                // Never overwrite a worker's immutable pages. Existing cuts
                // remain loadable even if allocation fails or the cap is reached.
                overflow_ = recordingStopped_ = true;
                return false;
            }
            const unsigned oldPage = active_[logical];
            const unsigned replacement = free_[--freeCount_];
            std::memcpy(&page(replacement), &page(oldPage), sizeof(Page));
            --references_[oldPage];
            references_[replacement] = 1;
            active_[logical] = replacement;
            ++cowCopies_;
        }
        StereoFrame& destination = page(active_[logical]).frames[frame % kPageFrames];
        const StereoFrame old = destination;
        destination = value;
        if (UpdateMoments) moments_.update(frame, old, value, [this](unsigned i) {
            return page(active_[i/kPageFrames]).frames[i%kPageFrames];
        });
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

public:
    StereoFrame readActive(std::uint32_t frame) const {
        if (frame >= validFrames_) return StereoFrame{0.f, 0.f};
        return page(active_[frame / kPageFrames]).frames[frame % kPageFrames];
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

    // Core-owned bounded metadata edits. Snapshot metadata is a separate cut.
    bool editMarker(std::uint16_t index, std::uint32_t frame, bool remove,
                    Marker (&previous)[kMaxSplices], std::uint16_t& previousCount) {
        if (!index || index >= markerCount_ || (!remove &&
            (frame <= markers_[index-1].frame || frame >= region(index).end))) return false;
        previousCount = markerCount_;
        for (unsigned i = 0; i < markerCount_; ++i) previous[i] = markers_[i];
        if (remove) {
            for (unsigned i = index; i + 1 < markerCount_; ++i) markers_[i] = markers_[i+1];
            --markerCount_;
        }
        else markers_[index].frame = frame;
        ++documentRevision_;
        return true;
    }
    void swapMarkerHistory(Marker (&previous)[kMaxSplices], std::uint16_t& count) {
        // Only accepts this same Reel's saved, valid table. The caller fences
        // document revision and invalidates history when the Reel is replaced.
        const unsigned total = count > markerCount_ ? count : markerCount_;
        for (unsigned i = 0; i < total; ++i) {
            const Marker old = i < markerCount_ ? markers_[i] : Marker{0, 0};
            markers_[i] = i < count ? previous[i] : Marker{0, 0};
            previous[i] = old;
        }
        const std::uint16_t oldCount = markerCount_;
        markerCount_ = count;
        count = oldCount;
        ++documentRevision_;
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

    bool beginSnapshot(std::uint64_t capturedThroughFrame, unsigned slot = 0) {
        if (slot >= kSnapshotSlots || !snapshots_[slot].frozen) return false;
        auto& cut = snapshots_[slot];
        if (cut.state != Idle || overflow_) return false;
        auto& meta = cut.metadata;
        meta.validFrames = validFrames_;
        meta.pageCount = (validFrames_ + kPageFrames - 1) / kPageFrames;
        meta.markerCount = markerCount_;
        for (unsigned i = 0; i < markerCount_; ++i) meta.markers[i] = markers_[i];
        meta.documentRevision = documentRevision_;
        meta.audioRevision = audioRevision_;
        meta.capturedThroughFrame = capturedThroughFrame;
        cut.scan = cut.reclaim = 0;
        cut.ready.store(false, std::memory_order_release);
        cut.state = Capturing;
        activeSnapshotMask_ |= 1u << slot;
        snapshotWorkMask_ |= 1u << slot;
        if (!meta.pageCount) publishReady(cut, slot);
        return true;
    }

    // One globally bounded page-work budget, round-robin across snapshot slots.
    std::uint32_t maintenanceTick() {
        if (!activeSnapshotMask_) return 0;
        adoptScratch(kScanPerTick);
        unsigned processed = 0;
        for (unsigned visit = 0; snapshotWorkMask_ && visit < kSnapshotSlots && processed < kScanPerTick; ++visit) {
            const unsigned slot = nextScanSlot_++ % kSnapshotSlots;
            auto& cut = snapshots_[slot];
            if (cut.state == Capturing) {
                while (processed < kScanPerTick && cut.scan < cut.metadata.pageCount) {
                    capturePage(cut, cut.scan++); ++processed;
                }
                if (cut.scan == cut.metadata.pageCount) publishReady(cut, slot);
            }
            else if (cut.state == Reclaiming) {
                while (processed < kScanPerTick && cut.reclaim < cut.metadata.pageCount) {
                    const unsigned i = cut.reclaim++;
                    if (cut.captured[i]) {
                        const unsigned physical = cut.frozen[i];
                        if (--references_[physical] == 0) free_[freeCount_++] = physical;
                        cut.frozen[i] = kInvalidPage; cut.captured[i] = 0;
                    }
                    ++processed;
                }
                if (cut.reclaim == cut.metadata.pageCount) {
                    cut.state = Idle;
                    activeSnapshotMask_ &= ~(1u << slot);
                    snapshotWorkMask_ &= ~(1u << slot);
                }
            }
        }
        if (scratch_ && scratch_->readyPages() < scratch_->maximumPages() &&
            freeCount_ < ScratchPages::kChunkPages &&
            adoptedScratch_ == scratch_->readyPages()) scratch_->requestMore();
        return processed;
    }

    // Caller releases a slot only after its last worker has finished reading.
    bool beginRelease(unsigned slot = 0) {
        if (slot >= kSnapshotSlots) return false;
        auto& cut = snapshots_[slot];
        if (cut.state != Ready) return false;
        cut.ready.store(false, std::memory_order_release);
        cut.state = Reclaiming; cut.reclaim = 0;
        snapshotWorkMask_ |= 1u << slot;
        if (!cut.metadata.pageCount) {
            cut.state = Idle;
            activeSnapshotMask_ &= ~(1u << slot);
            snapshotWorkMask_ &= ~(1u << slot);
        }
        return true;
    }
    StereoFrame readSnapshot(std::uint32_t frame, unsigned slot = 0) const {
        if (slot >= kSnapshotSlots) return StereoFrame{0.f, 0.f};
        const auto& cut = snapshots_[slot];
        if (!readyForWorker(slot) || frame >= cut.metadata.validFrames) return StereoFrame{0.f, 0.f};
        return page(cut.frozen[frame / kPageFrames]).frames[frame % kPageFrames];
    }

private:
    Page& page(unsigned physical) const {
        const unsigned base = pages_ + reservePages_;
        return physical < base ? pool_[physical] : scratch_->page(physical - base);
    }
    void adoptScratch(unsigned limit) {
        if (!scratch_) return;
        const unsigned ready = scratch_->readyPages();
        while (adoptedScratch_ < ready && limit--) {
            free_[freeCount_++] = pages_ + reservePages_ + adoptedScratch_++;
        }
    }
    static std::uint32_t checkedPages(std::uint32_t pages, std::uint32_t reservePages) {
        if (!pages || pages > kMaxPages || reservePages > pages)
            throw std::invalid_argument("Chimera Reel capacity/reserve outside profile-1 bounds");
        return pages;
    }
    void capturePage(Snapshot& cut, unsigned logical) {
        if (!cut.captured[logical]) {
            cut.frozen[logical] = active_[logical];
            ++references_[active_[logical]];
            cut.captured[logical] = 1;
        }
    }
    void publishReady(Snapshot& cut, unsigned slot) {
        cut.state = Ready;
        snapshotWorkMask_ &= ~(1u << slot);
        cut.ready.store(true, std::memory_order_release);
    }

    const std::uint32_t pages_, reservePages_, capacityFrames_;
    BlockMoments moments_;
    std::unique_ptr<Page[]> pool_;
    std::unique_ptr<std::uint32_t[]> active_, free_;
    std::unique_ptr<std::uint8_t[]> references_;
    Snapshot snapshots_[kSnapshotSlots];
    std::shared_ptr<ScratchPages> scratch_;
    unsigned adoptedScratch_ = 0, nextScanSlot_ = 0;
    unsigned activeSnapshotMask_ = 0, snapshotWorkMask_ = 0;
    std::uint32_t freeCount_, validFrames_;
    Marker markers_[kMaxSplices];
    std::uint16_t markerCount_;
    std::uint32_t nextMarkerId_;
    std::uint64_t documentRevision_, audioRevision_, capturedThroughFrame_;
    std::uint64_t cowCopies_;
    bool overflow_, recordingStopped_;
};

} // namespace chimera
