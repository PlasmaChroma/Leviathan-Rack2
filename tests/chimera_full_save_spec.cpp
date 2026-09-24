#include "ChimeraBundle.hpp"
#include <system.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
    const std::string root = "build/tests/chimera_full_save_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    need(rack::system::createDirectories(root + "/chimera"),
         "create full-length isolated storage");
    const auto begin = std::chrono::steady_clock::now();
    chimera::Reel reel(chimera::kMaxPages, chimera::kMaxPages);
    for (std::uint32_t i = 0; i < chimera::kMaxReelFrames; ++i)
        need(reel.write(i, {float(i % 101) / 100.f, -float(i % 89) / 100.f}, i),
             "fill full 174-second Reel");
    need(reel.addMarker(chimera::kMaxReelFrames - 1), "mark final valid frame");
    need(reel.beginSnapshot(chimera::kMaxReelFrames), "freeze full Reel");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    const auto prepared = std::chrono::steady_clock::now();
    const chimera::bundle::CommitResult committed =
        chimera::bundle::commit(root, "full", reel);
    need(bool(committed), "commit full-length embedded Reel");
    const auto saved = std::chrono::steady_clock::now();
    const std::string audio = root + "/chimera/reel-full.wav";
    std::ifstream file(audio.c_str(), std::ios::binary | std::ios::ate);
    need(bool(file) && file.tellg() > 0, "full-length WAV asset exists");
    const std::uint64_t bytes = std::uint64_t(file.tellg());
    file.close();
    need(bytes < std::uint64_t(chimera::kMaxReelFrames) * 8 + 1024 &&
         bytes > std::uint64_t(chimera::kMaxReelFrames) * 8,
         "embedded WAV stores only valid frames plus bounded RIFF metadata");
    chimera::bundle::LoadResult loaded = chimera::bundle::load(root, committed.manifest);
    const auto restored = std::chrono::steady_clock::now();
    need(bool(loaded) && loaded.reel->validFrames() == chimera::kMaxReelFrames &&
         loaded.reel->region(1).begin == chimera::kMaxReelFrames - 1 &&
         loaded.reel->readActive(chimera::kMaxReelFrames - 1).l ==
             float((chimera::kMaxReelFrames - 1) % 101) / 100.f,
         "full-length frame and terminal cue survive patch asset reload");
    std::printf("PASS: full Reel %llu bytes, prepare %.2f s, commit %.2f s, reload %.2f s\n",
        static_cast<unsigned long long>(bytes),
        std::chrono::duration<double>(prepared - begin).count(),
        std::chrono::duration<double>(saved - prepared).count(),
        std::chrono::duration<double>(restored - saved).count());
    loaded.reel.reset();
    const std::string allowed = rack::system::getCanonical("build/tests");
    const std::string target = rack::system::getCanonical(root);
    need(target.size() > allowed.size() &&
         target.compare(0, allowed.size(), allowed) == 0 &&
         (target[allowed.size()] == '/' || target[allowed.size()] == '\\'),
         "cleanup target stays inside build/tests");
    rack::system::removeRecursively(root);
}
