#include "../src/plugin.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <vector>

Plugin* pluginInstance = nullptr;
bool isDragonKingDebugEnabled() { return false; }
#include "../src/Chimera.cpp"

// Offline Rack-linked callback benchmark, intentionally outside test-fast.
// Build on Linux with:
// g++ -std=c++11 -O3 -DNDEBUG -Wno-unused-parameter -Isrc \
//   -I../Rack-SDK/include -I../Rack-SDK/dep/include \
//   tools/chimera_phase4_module_bench.cpp src/ChimeraService.cpp \
//   src/visual/ApertureLight.cpp src/NvgGraphicsLifecycle.cpp \
//   -L../Rack-SDK -lRack -lGL -Wl,-rpath,$PWD/../Rack-SDK -pthread \
//   -o /tmp/chimera_phase4_module_bench
// Includes Chimera::process(), but not Rack's graph scheduler, GUI, or host SRC.
int main() {
    chimera::Reel reel(chimera::kMaxPages, chimera::kMaxPages);
    for (unsigned i = 0; i < chimera::kMaxReelFrames; ++i)
        if (!reel.write(i, chimera::StereoFrame{float(i % 37) / 37.f,
                                                float(i % 53) / 53.f}, i)) return 1;
    Chimera module;
    module.reel = &reel;
    module.slice.setReel(&reel);
    module.slice.setConditioning(false);
    module.pminSetting.store(true);
    module.params[Chimera::SOS_PARAM].setValue(0.5f);
    module.params[Chimera::GENE_SIZE_PARAM].setValue(float(
        std::log(480.0 / double(chimera::kMaxReelFrames)) /
        std::log(16.0 / double(chimera::kMaxReelFrames))));
    module.params[Chimera::VARISPEED_PARAM].setValue(5.f / 6.f);
    module.params[Chimera::MORPH_PARAM].setValue(1.f);
    module.inputs[Chimera::AUDIO_R_INPUT].channels = 1;
    module.inputs[Chimera::AUDIO_R_INPUT].setVoltage(5.f);
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f / 48000.f;
    for (int i = 0; i < 144240; ++i) module.process(args);
    if (!module.slice.pmActive() || !module.slice.startCurrent()) return 2;
    volatile float sink = 0.f;
    for (int run = 0; run < 4; ++run) {
        const std::uint64_t priorCopies = reel.cowCopies();
        std::vector<double> blocks;
        blocks.reserve(1000);
        const auto start = std::chrono::steady_clock::now();
        auto blockStart = start;
        for (int i = 0; i < 480000; ++i) {
            if (i == 100000 && !reel.beginSnapshot(module.slice.frame())) return 3;
            if (i == 200000 && !reel.beginRelease()) return 4;
            module.process(args);
            sink += module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage() * 1e-3f;
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
