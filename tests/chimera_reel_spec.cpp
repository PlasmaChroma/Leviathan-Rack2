#include "ChimeraReel.hpp"
#include <cstdio>
#include <cstdlib>
#include <new>

static bool trap = false;
static std::size_t allocations = 0;
void* operator new(std::size_t n) {
    if (trap) ++allocations;
    void* p = std::malloc(n);
    if (!p) throw std::bad_alloc();
    return p;
}
void* operator new[](std::size_t n) {
    if (trap) ++allocations;
    void* p = std::malloc(n);
    if (!p) throw std::bad_alloc();
    return p;
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}

int main() {
    using namespace chimera;
    {
        Reel reel(4, 4);
        for (std::uint32_t i = 0; i < 4*kPageFrames; ++i)
            need(reel.write(i, StereoFrame{float(i), -float(i)}, i), "populate small Reel");
        need(!reel.addMarker(0) && reel.addMarker(512) && !reel.addMarker(512),
             "implicit zero marker and stable unique additions");
        need(reel.beginSnapshot(1023), "begin snapshot at exact cut");
        // Page 3 has not been scanned. The write barrier must freeze it first.
        need(reel.write(3*kPageFrames, StereoFrame{9999, -9999}, 1024),
             "overwrite unscanned page");
        need(reel.cowCopies() == 1 && reel.freePages() == 3,
             "unscanned page cloned exactly once");
        need(reel.maintenanceTick() == 4 && reel.readyForWorker(),
             "bounded scan publishes completed snapshot");
        need(reel.snapshotMetadata().validFrames == 1024 &&
             reel.snapshotMetadata().markerCount == 2 &&
             reel.snapshotMetadata().markers[1].frame == 512 &&
             reel.snapshotMetadata().capturedThroughFrame == 1023,
             "frozen audio/marker/time metadata stays coherent");
        need(reel.readSnapshot(3*kPageFrames).l == 768 &&
             reel.readActive(3*kPageFrames).l == 9999,
             "worker sees cut audio, core sees new audio");
        const std::uint64_t beforeCopies = reel.cowCopies();
        trap = true;
        const std::size_t beforeAlloc = allocations;
        for (int i = 0; i < 5000; ++i)
            need(reel.write(3*kPageFrames, StereoFrame{float(i), -float(i)}, 1025+i),
                 "repeat same protected-page write");
        trap = false;
        need(allocations == beforeAlloc && reel.cowCopies() == beforeCopies,
             "repeated writes make no allocation or extra COW clone");
        need(!reel.beginSnapshot(6000) && reel.beginRelease(),
             "second cut blocked while lease lives");
        need(reel.maintenanceTick() == 4 && reel.state() == Reel::Idle &&
             reel.freePages() == 4, "reclamation returns frozen-only pages");
        need(reel.beginSnapshot(6000), "new snapshot allowed after reclaim");
    }
    {
        Reel partial(2, 2);
        for (std::uint32_t i = 0; i < 257; ++i)
            need(partial.write(i, StereoFrame{float(i), 0}, i), "populate partial last page");
        need(partial.beginSnapshot(256), "cut partial page");
        need(partial.write(257, StereoFrame{257, 0}, 257), "post-cut append into partial page");
        need(partial.cowCopies() == 1, "partial last page protected");
        partial.maintenanceTick();
        need(partial.snapshotMetadata().validFrames == 257 &&
             partial.readSnapshot(256).l == 256 && partial.readSnapshot(257).l == 0,
             "snapshot keeps cut length and partial-page contents");
        need(partial.write(300, StereoFrame{300, 0}, 300), "append into post-cut page");
        need(partial.cowCopies() == 1, "post-cut append needs no second clone");
        need(partial.beginRelease(), "release partial lease");
        need(partial.maintenanceTick() == 2 && partial.freePages() == 2,
             "partial snapshot reclaims without freeing active page");
    }
    {
        Reel exhausted(1, 0);
        need(exhausted.write(0, StereoFrame{1,1}, 0) && exhausted.beginSnapshot(0),
             "prepare deliberate zero-reserve invariant failure");
        need(!exhausted.write(0, StereoFrame{2,2}, 1) && exhausted.overflowed() &&
             exhausted.recordingStopped() && exhausted.readActive(0).l == 1,
             "pool exhaustion stops safely without corrupting active audio");
    }
    {
        Reel full(kMaxPages, kMaxPages);
        need(full.rawAudioBytes() == 133632000ull && full.payloadBytes() == 158296500ull &&
             full.capacityFrames() == kMaxReelFrames, "full off-audio allocation/budget");
        for (std::uint32_t i = 0; i < kMaxReelFrames; ++i)
            need(full.write(i, StereoFrame{float(i % 4096), -float(i % 4096)}, i),
                 "populate full Reel");
        need(full.beginSnapshot(kMaxReelFrames-1), "cut full Reel");
        std::uint32_t maxScanned = 0;
        std::uint32_t ticks = 0;
        trap = true;
        const std::size_t beforeAlloc = allocations;
        for (std::uint32_t i = 0; i < kMaxReelFrames; ++i) {
            if (!full.readyForWorker()) {
                const std::uint32_t scanned = full.maintenanceTick();
                if (scanned > maxScanned) maxScanned = scanned;
                ++ticks;
            }
            need(full.write(i, StereoFrame{7777, -7777}, kMaxReelFrames+i),
                 "write every frame while snapshotting");
        }
        trap = false;
        need(allocations == beforeAlloc && maxScanned <= 8 && ticks == 4079 &&
             full.cowCopies() == kMaxPages && full.freePages() == 0,
             "full scan/COW bounded with no core allocations");
        for (std::uint32_t i = 0; i < kMaxReelFrames; ++i) {
            const StereoFrame old = full.readSnapshot(i);
            if (old.l != float(i % 4096) || old.r != -float(i % 4096))
                need(false, "full frozen encoder differs from exact cut");
        }
        need(full.beginRelease(), "release full lease");
        std::uint32_t reclaimTicks = 0;
        while (full.state() == Reel::Reclaiming) {
            need(full.maintenanceTick() <= 8, "bounded reclaim tick");
            ++reclaimTicks;
        }
        need(reclaimTicks == 4079 && full.freePages() == kMaxPages,
             "full reclaim returns entire reserve");
    }
    std::puts("PASS: Chimera Reel exact cuts, repeated COW, partial pages, full capacity, bounded scan/reclaim");
}
