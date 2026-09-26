#include "ChimeraReel.hpp"
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <new>

static thread_local bool failAllocation = false;
void* operator new(std::size_t size) {
    if (failAllocation) throw std::bad_alloc();
    void* result = std::malloc(size);
    if (!result) throw std::bad_alloc();
    return result;
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* value) noexcept { std::free(value); }
void operator delete[](void* value) noexcept { std::free(value); }

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static void fill(chimera::Reel& reel, float value) {
    for (unsigned i = 0; i < reel.capacityFrames(); ++i)
        need(reel.write(i, {value, -value}, i), "fill active version");
}
int main() {
    using chimera::Reel;
    {
        Reel reel(8, 8);
        reel.prepareRecordingSnapshots(false);
        fill(reel, 1.f);
        for (unsigned slot = 0; slot < 3; ++slot)
            need(reel.beginSnapshot(slot, slot), "cuts may share identical physical pages");
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        const auto free = reel.freePages();
        need(reel.write(4*256, {2.f, 2.f}, 1) && reel.cowCopies() == 1,
             "three shared cuts need only one page copy on overwrite");
        for (unsigned slot : {1u, 0u}) {
            reel.beginRelease(slot);
            while (reel.snapshotWorkPending()) reel.maintenanceTick();
            need(reel.freePages() == free - 1 && reel.readSnapshot(4*256, 2).l == 1.f,
                 "partial release cannot reclaim another snapshot's shared page");
        }
        reel.beginRelease(2);
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        need(reel.freePages() == free, "last shared reader releases exactly one version page");
    }
    {
        Reel reel(2048, 2048);
        reel.prepareRecordingSnapshots(false);
        const auto budget = reel.budgetBytes();
        need(reel.scratchPages() == 1024, "only the initial 2 MiB chunk is allocated");
        fill(reel, 1.f);
        for (unsigned slot = 0; slot < 3; ++slot) {
            need(reel.beginSnapshot(slot, slot), "independent exact snapshot boundary");
            for (unsigned i = 0; i < reel.capacityFrames(); ++i) {
                need(reel.write(i, {float(slot+2), -float(slot+2)}, i), "overwrite with live cuts");
                need(reel.maintenanceTick() <= Reel::kScanPerTick, "globally bounded capture/reclaim");
                if ((i & 255u) == 0) reel.replenishScratchForTest();
            }
            while (reel.snapshotWorkPending()) reel.maintenanceTick();
        }
        need(reel.scratchPages() > 1024 && reel.scratchPages() <= 4096 &&
             reel.budgetBytes() == budget, "growth remains inside its precharged cap");
        for (unsigned slot = 0; slot < 3; ++slot)
            for (unsigned frame = 0; frame < reel.capacityFrames(); ++frame)
                need(reel.readSnapshot(frame, slot).l == float(slot+1),
                     "each cut retains its exact audio through other versions and scratch growth");
        // Out-of-order release must not free pages still referenced by another slot.
        for (unsigned slot : {1u, 0u, 2u}) {
            need(reel.beginRelease(slot), "release independently");
            while (reel.snapshotWorkPending()) reel.maintenanceTick();
        }
        need(!reel.hasSnapshots() && reel.freePages() == 2048 + reel.scratchPages(),
             "all version-only pages recycle exactly once");
        fill(reel, 5.f);
        need(reel.beginSnapshot(9, 2), "reuse reclaimed slot");
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        need(reel.readSnapshot(0, 2).l == 5.f, "reused slot has no stale page references");
    }
    {
        Reel reel(2048, 0);
        reel.prepareRecordingSnapshots(false);
        fill(reel, 1.f); reel.beginSnapshot(0);
        need(reel.write(0, {2.f, 2.f}, 0), "use initial reserve before allocation failure");
        reel.maintenanceTick();
        failAllocation = true;
        reel.replenishScratchForTest();
        failAllocation = false;
        need(reel.scratchPages() == 1024 && !reel.recordingStopped(),
             "failed allocation publishes no partial chunk and leaves reserve usable");
        reel.maintenanceTick(); reel.replenishScratchForTest();
        need(reel.scratchPages() == 2048, "allocation can recover on a later replenishment request");
    }
    {
        Reel reel(2048, 0);
        reel.prepareRecordingSnapshots(false); // Simulate a stalled allocation worker.
        fill(reel, 7.f);
        need(reel.beginSnapshot(0), "cut before scratch starvation");
        for (unsigned page = 0; page < 1024; ++page) {
            need(reel.write(page*256, {8.f, 8.f}, page), "consume reserved scratch");
            reel.maintenanceTick();
        }
        need(!reel.write(1024*256, {9.f, 9.f}, 1024) && reel.recordingStopped() &&
             reel.readActive(1024*256).l == 7.f,
             "exhaustion stops before corrupting either active or frozen audio");
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        for (unsigned page = 0; page < 2048; ++page)
            need(reel.readSnapshot(page*256).l == 7.f, "starved snapshot is intact");
        need(!reel.prepareRecording(), "cannot restart into outstanding exhausted cuts");
        reel.beginRelease();
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        need(reel.prepareRecording() && reel.write(1024*256, {9.f, 9.f}, 1025),
             "a fresh take can start after held versions drain and pages recycle");
    }
    {
        Reel reel(2048, 0);
        reel.prepareRecordingSnapshots();
        fill(reel, 3.f);
        need(reel.beginSnapshot(0), "cut for concurrent readers");
        while (reel.snapshotWorkPending()) reel.maintenanceTick();
        std::atomic<bool> done{false};
        std::thread reader([&] {
            while (!done.load(std::memory_order_acquire))
                for (unsigned i = 0; i < reel.capacityFrames(); i += 257)
                    need(reel.readSnapshot(i).l == 3.f, "worker reads stable pages during growth");
        });
        // Request growth while plenty of prefaulted capacity remains.
        reel.maintenanceTick();
        for (unsigned i = 0; i < 256; ++i) {
            need(reel.write(i*256, {4.f, 4.f}, i), "write before replenishment");
            reel.maintenanceTick();
        }
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (reel.scratchPages() == 1024 && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        need(reel.scratchPages() > 1024, "independent allocation worker replenishes without disk/control pumping");
        for (unsigned i = 256; i < 2048; ++i) {
            reel.maintenanceTick();
            need(reel.write(i*256, {4.f, 4.f}, i), "use published scratch with concurrent reader");
        }
        done.store(true, std::memory_order_release); reader.join();
    }
    std::puts("PASS: overlapping Reel snapshots, bounded scratch growth, concurrent reads, exhaustion and recycling");
}
