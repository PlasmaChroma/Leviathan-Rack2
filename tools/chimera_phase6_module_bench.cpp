#include "../src/plugin.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>
#include "ChimeraBenchmarkGene.hpp"

Plugin* pluginInstance = nullptr;
bool isDragonKingDebugEnabled() { return false; }
std::string leviathanPluginUserRootPath() { return "build/tests/chimera_bench_cache"; }
#define CHIMERA_HEADLESS_TEST 1
#define CHIMERA_MANUAL_CONTROL_TEST 1
#include "../src/Chimera.cpp"

// Rack-linked optimized callback benchmark. Includes the prepared host-rate
// adapter and active 48 kHz Slice, but not Rack's graph scheduler or GUI.
int main(int argc, char** argv) {
    const unsigned rate = argc > 1 ? unsigned(std::strtoul(argv[1], nullptr, 10)) : 96000;
    if (!chimera::RateBridge::supported(float(rate))) return 1;
    chimera::Reel reel(chimera::kMaxPages, chimera::kMaxPages);
    for (unsigned i = 0; i < chimera::kMaxReelFrames; ++i)
        if (!reel.write(i, chimera::StereoFrame{float(i % 37) / 37.f,
                                                float(i % 53) / 53.f}, i)) return 2;
    Chimera module;
    reel.prepareRecordingSnapshots(); // Match production registry preparation.
    module.reel = &reel;
    module.slice.setReel(&reel);
    module.pminSetting.store(true);
    module.params[Chimera::SOS_PARAM].setValue(0.5f);
    module.params[Chimera::GENE_SIZE_PARAM].setValue(chimeraBenchmarkGene(reel.validFrames()));
    module.params[Chimera::VARISPEED_PARAM].setValue(5.f / 6.f);
    module.params[Chimera::MORPH_PARAM].setValue(1.f);
    module.inputs[Chimera::AUDIO_R_INPUT].channels = 1;
    module.inputs[Chimera::AUDIO_R_INPUT].setVoltage(5.f);
    Module::ProcessArgs args{};
    args.sampleRate = float(rate);
    args.sampleTime = 1.f / float(rate);
    if (rate != 48000) {
        module.process(args);
        module.serviceStep(); // Prepare Speex/FIFOs outside process().
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (!module.preparedBridge.load(std::memory_order_acquire) &&
               std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
        if (!module.preparedBridge.load(std::memory_order_acquire)) return 7;
    }
    for (unsigned i = 0; i < rate * 4; ++i) module.process(args);
    if (!module.slice.pmActive() || !module.slice.startCurrent()) return 3;
    const unsigned blockFrames = rate / 100;
    if (!blockFrames || blockFrames * 100 != rate) return 4;
    volatile float sink = 0.f;
    std::vector<double> blocks;
    blocks.reserve(500);
    const auto begin = std::chrono::steady_clock::now();
    auto blockStart = begin;
    const std::uint64_t priorCopies = reel.cowCopies();
    for (unsigned i = 0; i < rate * 5; ++i) {
        if (i == rate && !reel.beginSnapshot(module.slice.frame())) return 5;
        if (i == rate * 2 && !reel.beginRelease()) return 6;
        module.process(args);
        sink += module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage() * 1e-3f;
        if ((i + 1) % blockFrames == 0) {
            const auto now = std::chrono::steady_clock::now();
            blocks.push_back(std::chrono::duration<double, std::milli>(now - blockStart).count());
            blockStart = now;
        }
    }
    const auto end = std::chrono::steady_clock::now();
    std::sort(blocks.begin(), blocks.end());
    const double elapsed = std::chrono::duration<double, std::milli>(end - begin).count();
    std::printf("rate=%u total_ms=%.3f core_percent=%.3f block10ms_p50=%.4f p95=%.4f p99=%.4f max=%.4f cow=%llu sink=%f\n",
                rate, elapsed, elapsed / 50.0, blocks[249], blocks[474],
                blocks[494], blocks.back(),
                static_cast<unsigned long long>(reel.cowCopies() - priorCopies), sink);
}
