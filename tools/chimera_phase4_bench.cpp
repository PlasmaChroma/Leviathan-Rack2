#include "ChimeraSlice.hpp"
#include <chrono>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstdlib>

// Offline 48 kHz core benchmark, intentionally outside test-fast.
// Build on Linux with:
// g++ -std=c++11 -O3 -march=nehalem -funsafe-math-optimizations \
//   -fno-fast-math -fno-unsafe-math-optimizations -Isrc \
//   tools/chimera_phase4_bench.cpp -o /tmp/chimera_phase4_bench
// Includes full Reel memory, a 3 s PM presence warmup, dense finite Genes,
// Current recording, and one full-length snapshot capture/release per run.
// Timing excludes preparation and omits Rack, host SRC, and GUI overhead.
int main(int argc, char** argv) {
    const double geneFrames = argc > 1 ? std::strtod(argv[1], nullptr) : 480.0;
    const bool modulated = argc > 2;
    if (geneFrames < 16 || geneFrames > chimera::kMaxReelFrames) return 5;
    chimera::Reel reel(chimera::kMaxPages, chimera::kMaxPages);
    for (unsigned i = 0; i < chimera::kMaxReelFrames; ++i)
        if (!reel.write(i, chimera::StereoFrame{float(i % 37) / 37.f,
                                                float(i % 53) / 53.f}, i)) return 1;
    chimera::Slice slice(&reel);
    slice.setConditioning(false);
    slice.setPmEnabled(true);
    chimera::CoreInput in{};
    in.live = chimera::StereoFrame{0.f, 5.f};
    in.pmRightVolts = 5.f;
    in.pmRightConnected = true;
    in.controls.sos = 0.5f;
    in.controls.gene = float(std::log(geneFrames / double(chimera::kMaxReelFrames)) /
                             std::log(16.0 / double(chimera::kMaxReelFrames)));
    in.controls.rate = 5.f / 6.f;
    in.controls.morph = 1.f;
    if (modulated) { slice.setRateMode(1); in.controls.rateAtt = 1.f; }
    for (int i = 0; i < 144240; ++i) slice.step(in);
    if (!slice.pmActive() || !slice.startCurrent()) return 2;
    volatile float sink = 0.f;
    for (int run = 0; run < 4; ++run) {
        const std::uint64_t priorCopies = reel.cowCopies();
        std::vector<double> blocks;
        blocks.reserve(1000);
        const auto start = std::chrono::steady_clock::now();
        auto blockStart = start;
        for (int i = 0; i < 480000; ++i) {
            if (modulated) in.controls.rateCv = float(i % 997) / 997.f - 0.5f;
            if (i == 100000 && !reel.beginSnapshot(slice.frame())) return 3;
            if (i == 200000 && !reel.beginRelease()) return 4;
            const chimera::Slice::Output out = slice.step(in);
            sink += out.audio.l + out.cv * 1e-6f;
            if ((i + 1) % 480 == 0) {
                const auto now = std::chrono::steady_clock::now();
                blocks.push_back(std::chrono::duration<double, std::milli>(now - blockStart).count());
                blockStart = now;
            }
        }
        const auto end = std::chrono::steady_clock::now();
        const double ms = std::chrono::duration<double, std::milli>(end - start).count();
        std::sort(blocks.begin(), blocks.end());
        std::printf("run=%d ms_per_10s=%.3f core_percent=%.3f block_p50_ms=%.4f block_p95_ms=%.4f block_p99_ms=%.4f block_max_ms=%.4f cow=%llu\n",
                    run, ms, ms / 100.0, blocks[499], blocks[949], blocks[989], blocks.back(),
                    static_cast<unsigned long long>(reel.cowCopies() - priorCopies));
    }
    std::printf("sink=%f\n", sink);
}
