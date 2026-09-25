#include "ChimeraWaveform.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>

static void need(bool yes, const char* message) {
    if (!yes) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main() {
    chimera::Reel reel(2, 2);
    for (std::uint32_t i = 0; i < 256; ++i)
        need(reel.write(i, {i == 16 ? 0.75f : -0.25f,
                            i == 16 ? 0.75f : -0.25f}, i), "populate Reel");
    need(reel.addMarker(128), "add marker");
    need(reel.beginSnapshot(256), "freeze Reel");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    need(reel.write(16, {-0.9f, -0.9f}, 257), "overwrite while snapshot remains leased");
    const auto before = chimera::WaveformSummary::fromSnapshot(reel);
    const auto after = chimera::WaveformSummary::fromActive(reel);
    need(before->frames == 256 && before->markerCount == 2 &&
         before->markers[1] == 128, "frozen display retains valid length and marker");
    need(before->audioRevision + 1 == after->audioRevision &&
         before->peak == 0.75f && after->peak == 0.9f,
         "waveform summaries distinguish a protected cut from newer audio");
    need(before->high[8] == 0.75f && after->low[8] == -0.9f,
         "peak bins include the exact overwritten sample");
    chimera::Reel antiphase(1, 0);
    need(antiphase.write(0, {0.7f, -0.7f}, 0), "populate antiphase Reel");
    const auto stereo = chimera::WaveformSummary::fromActive(antiphase);
    need(stereo->peak == 0.7f && stereo->high[0] == 0.7f &&
         stereo->low[0] == -0.7f, "stereo opposition remains visible");
    chimera::Reel full(chimera::kMaxPages, 0);
    for (std::uint32_t i = 0; i < chimera::kMaxReelFrames; ++i)
        need(full.write(i, {(i % 4096) == 0 ? 0.8f : 0.f, 0.f}, i),
             "populate full-length Reel");
    for (std::uint32_t i = 1; i < chimera::kMaxSplices; ++i)
        need(full.addMarker(i * (chimera::kMaxReelFrames / chimera::kMaxSplices)),
             "populate maximum marker table");
    const auto start = std::chrono::steady_clock::now();
    const auto display = chimera::WaveformSummary::fromActive(full);
    const double milliseconds = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - start).count();
    need(display->frames == chimera::kMaxReelFrames &&
         display->markerCount == chimera::kMaxSplices && display->peak == 0.8f,
         "full-length waveform keeps a bounded 128-bin and 300-marker summary");
    std::printf("Full-capacity worker waveform build: %.2f ms\n", milliseconds);
    std::puts("PASS: Chimera worker waveform peaks, markers, and COW cut");
}
