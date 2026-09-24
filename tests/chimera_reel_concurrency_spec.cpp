#include "ChimeraReel.hpp"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}

int main() {
    chimera::Reel reel(8, 8);
    const std::uint32_t frames = 8 * chimera::kPageFrames;
    for (std::uint32_t i = 0; i < frames; ++i)
        need(reel.write(i, chimera::StereoFrame{float(i), -float(i)}, i), "populate snapshot source");
    need(reel.beginSnapshot(frames-1), "create immutable cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    std::atomic<bool> readerDone(false);
    std::atomic<bool> mismatch(false);
    std::thread reader([&] {
        for (int pass = 0; pass < 500; ++pass) {
            for (std::uint32_t i = 0; i < frames; ++i) {
                const chimera::StereoFrame frozen = reel.readSnapshot(i);
                if (frozen.l != float(i) || frozen.r != -float(i)) mismatch.store(true);
            }
        }
        readerDone.store(true, std::memory_order_release);
    });
    for (int pass = 0; pass < 500; ++pass) {
        for (std::uint32_t i = 0; i < frames; ++i)
            need(reel.write(i, chimera::StereoFrame{float(pass+10000), -float(pass+10000)},
                            frames + pass*frames + i), "core rewrites active pages");
    }
    reader.join();
    need(readerDone.load(std::memory_order_acquire) && !mismatch.load() &&
         reel.cowCopies() == 8, "worker saw only immutable pages during active writes");
    need(reel.beginRelease(), "release only after worker joins");
    while (reel.state() == chimera::Reel::Reclaiming) reel.maintenanceTick();
    need(reel.freePages() == 8, "all frozen pages reclaimed after final reader");
    std::puts("PASS: Chimera concurrent worker snapshot read and core active-page writes");
}
