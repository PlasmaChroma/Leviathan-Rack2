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
    const std::string root = "build/tests/chimera_bundle_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    need(rack::system::createDirectories(root + "/chimera"), "create isolated module storage");
    chimera::Reel reel(2, 2);
    for (std::uint32_t i = 0; i < 300; ++i)
        need(reel.write(i, {float(i) / 100.f, -float(i) / 200.f}, i),
             "seed stereo Reel");
    need(reel.addMarker(100), "seed marker");
    need(reel.beginSnapshot(300), "take first cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    const chimera::bundle::CommitResult first =
        chimera::bundle::commit(root, "one", reel);
    need(bool(first) && first.manifest == "chimera/reel-one.json",
         "publish first complete bundle");
    chimera::bundle::LoadResult restored = chimera::bundle::load(root, first.manifest, 2);
    need(bool(restored) && restored.reel->validFrames() == 300 &&
         restored.reel->markerCount() == 2 &&
         restored.reel->readActive(200).r == -1.f,
         "load embedded audio and marker manifest without a source path");
    {
        const std::string staging = root + "/private-stage";
        need(rack::system::createDirectories(staging), "private staging directory");
        const auto staged = chimera::bundle::stage(staging, "stage-1", reel);
        need(bool(staged) && !rack::system::exists(root + "/chimera/reel-stage-1.wav"),
             "stage creates no patch-visible bundle");
        // Force publication to fail AFTER audio publication: the source
        // manifest disappears. The newly copied audio must be unwound.
        need(rack::system::remove(staging + "/reel-stage-1.json"), "inject missing staged manifest");
        need(!chimera::bundle::publishStaged(staging, root, staged) &&
             !rack::system::exists(root + "/chimera/reel-stage-1.wav") &&
             bool(chimera::bundle::load(root, first.manifest, 2)),
             "publication failure cleans new audio and preserves previous bundle");
        const auto fresh = chimera::bundle::stage(staging, "stage-2", reel);
        const auto published = chimera::bundle::publishStaged(staging, root, fresh);
        need(bool(published) && bool(chimera::bundle::load(root, published.manifest, 2)),
             "completed staged audio and manifest publish as a loadable bundle");
        need(!chimera::bundle::publishStaged(staging, root, fresh),
             "publication cannot overwrite an existing revision");
        chimera::Reel empty(1, 1);
        empty.beginSnapshot(0);
        const auto emptyStage = chimera::bundle::stage(staging, "stage-3", empty);
        const auto emptyPublished = chimera::bundle::publishStaged(staging, root, emptyStage);
        need(bool(emptyPublished) && bool(chimera::bundle::load(root, emptyPublished.manifest, 2)) &&
             !rack::system::exists(root + "/chimera/reel-stage-3.wav"), "empty staged Reel requires no WAV");
    }
    need(reel.beginRelease(), "release first cut");
    while (reel.state() != chimera::Reel::Idle) reel.maintenanceTick();
    need(reel.write(0, {77.f, 77.f}, 301), "mutate after first save");
    need(reel.beginSnapshot(301), "take second cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    const chimera::bundle::CommitResult failed =
        chimera::bundle::commit(root, "two", reel, true);
    need(!failed && failed.error == "injected_manifest_failure",
         "manifest-stage failure is reported");
    need(!rack::system::exists(root + "/chimera/reel-two.wav") &&
         !rack::system::exists(root + "/chimera/reel-two.wav.tmp") &&
         !rack::system::exists(root + "/chimera/reel-two.json.tmp"),
         "failed manifest leaves no new orphan audio or temporary files");
    restored = chimera::bundle::load(root, first.manifest, 2);
    need(bool(restored) && restored.reel->readActive(0).l == 0.f,
         "prior committed bundle survives failure");
    const chimera::bundle::CommitResult second =
        chimera::bundle::commit(root, "three", reel);
    need(bool(second), "new revision commits after failure");
    restored = chimera::bundle::load(root, second.manifest, 2);
    need(bool(restored) && restored.reel->readActive(0).l == 77.f,
         "new bundle contains newer frozen cut");
    {
        std::ofstream unrelated((root + "/chimera/user-notes.txt").c_str());
        unrelated << "leave this file alone";
    }
    need(chimera::bundle::pruneObsolete(root, second.manifest),
         "prune obsolete module-owned revisions after successful commit");
    need(!rack::system::isFile(root + "/chimera/reel-one.wav") &&
         !rack::system::isFile(root + "/chimera/reel-one.json") &&
         !rack::system::isFile(root + "/chimera/reel-two.wav") &&
         rack::system::isFile(root + "/chimera/reel-three.wav") &&
         rack::system::isFile(root + "/chimera/reel-three.json") &&
         rack::system::isFile(root + "/chimera/user-notes.txt"),
         "pruning keeps the referenced bundle and unrelated files");
    need(!chimera::bundle::load(root, "chimera/../reel-three.json", 2),
         "manifest traversal rejected before file access");
    need(!chimera::bundle::load(root, "chimera/reel-future.json", 2),
         "missing asset is an explicit error");
    std::fstream audio((root + "/chimera/reel-three.wav").c_str(),
                       std::ios::binary | std::ios::in | std::ios::out);
    need(bool(audio), "open embedded audio for integrity fault");
    audio.seekp(100, std::ios::beg); audio.put('\x7f'); audio.close();
    need(!chimera::bundle::load(root, second.manifest, 2),
         "tampered embedded audio fails hash verification");
    std::puts("PASS: Chimera revisioned patch bundle commit/load/fallback/pruning");
}
