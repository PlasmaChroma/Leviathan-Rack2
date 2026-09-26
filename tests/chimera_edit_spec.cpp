#include "ChimeraEdit.hpp"
#include "ChimeraPlaybackReader.hpp"
#include <cstring>
#include <limits>
#include <cstdio>
#include <cstdlib>

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static void bulkEditEquivalence() {
    chimera::Reel source(8, 8);
    for (unsigned i = 0; i < 1403; ++i) {
        chimera::StereoFrame sample{float(int(i % 37) - 18) / 19.f,
                                   float(int(i % 53) - 26) / 27.f};
        if (i == 18) sample.l = std::numeric_limits<float>::quiet_NaN();
        if (i == 700) sample.r = std::numeric_limits<float>::infinity();
        need(source.write(i, sample, i), "seed nonfinite and partial-block edit source");
    }
    source.addMarker(17); source.addMarker(257); source.addMarker(1025);
    source.restoreRevisions(8000, 7000);
    need(source.beginSnapshot(0), "freeze bulk equivalence source");
    while (!source.readyForWorker()) source.maintenanceTick();
    for (auto kind : {chimera::edit::EraseSplice, chimera::edit::DeleteSplice,
                      chimera::edit::MoveMarker, chimera::edit::RemoveMarker,
                      chimera::edit::ClearReel}) {
        for (unsigned splice : {0u, 1u, 2u, 3u}) {
            if (!splice && (kind == chimera::edit::MoveMarker ||
                            kind == chimera::edit::RemoveMarker)) continue;
            chimera::edit::Request request;
            request.kind = kind; request.splice = splice;
            request.frame = source.region(splice).begin + 1;
            auto edited = chimera::edit::build(source, request, 8, 8);
            need(bool(edited), "build fresh bulk edit");
            const auto range = source.region(splice);
            const unsigned frames = kind == chimera::edit::ClearReel ? 0 :
                source.validFrames() - (kind == chimera::edit::DeleteSplice ? range.end-range.begin : 0);
            chimera::Reel reference(8, 8);
            for (unsigned i = 0; i < frames; ++i) {
                const unsigned from = kind == chimera::edit::DeleteSplice && i >= range.begin ?
                    i + range.end - range.begin : i;
                auto sample = source.readSnapshot(from);
                if (kind == chimera::edit::EraseSplice && i >= range.begin && i < range.end)
                    sample = {0.f, 0.f};
                reference.write(i, sample, i);
                const auto actual = edited.reel->readActive(i);
                need(std::memcmp(&actual, &sample, sizeof(sample)) == 0,
                     "bulk edit preserves exact samples including nonfinite values and splice edges");
            }
            const bool audioEdit = kind == chimera::edit::EraseSplice ||
                kind == chimera::edit::DeleteSplice || kind == chimera::edit::ClearReel;
            need(edited.reel->validFrames() == frames &&
                 edited.reel->documentRevision() == 8001 &&
                 edited.reel->audioRevision() == (audioEdit ? 7001u : 7000u),
                 "bulk construction preserves edit revision semantics");
            for (unsigned size : {16u, 64u, 256u}) {
                for (unsigned i = 0; i < frames; i += size) {
                    const auto& a = edited.reel->playbackMoments(i, size);
                    const auto& b = reference.playbackMoments(i, size);
                    need(a.invalid == b.invalid, "bulk edit retains invalid-sample moment counts");
                    for (unsigned j = 0; j < 4; ++j)
                        need(std::fabs(a.left[j]-b.left[j]) < 0.001f &&
                             std::fabs(a.right[j]-b.right[j]) < 0.001f,
                             "bulk edit moments match live construction at every level");
                }
            }
            chimera::PlaybackReader reader;
            for (unsigned region = 0; region < edited.reel->markerCount(); ++region) {
                const auto span = edited.reel->region(region);
                for (double speed : {-512., -37., 1.25, 32., 512.}) {
                    bool invalidA = false, invalidB = false;
                    const auto a = reader.read(*edited.reel, span, span.begin + .75, speed, invalidA);
                    const auto b = reader.read(reference, span, span.begin + .75, speed, invalidB);
                    need(invalidA == invalidB && std::fabs(a.l-b.l) < 1e-5f &&
                         std::fabs(a.r-b.r) < 1e-5f,
                         "accelerated edited playback agrees with live reference in both directions");
                }
            }
        }
    }
}
int main() {
    bulkEditEquivalence();
    chimera::Reel reel(3, 3);
    for (std::uint32_t i = 0; i < 600; ++i)
        need(reel.write(i, {float(i), -float(i)}, i), "seed edit Reel");
    need(reel.addMarker(100) && reel.addMarker(300), "seed three Splices");
    const std::uint32_t lastId = reel.markerId(2);
    need(reel.beginSnapshot(600), "freeze edit source");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    chimera::edit::Request request;
    request.kind = chimera::edit::EraseSplice; request.splice = 1;
    chimera::edit::Result erased = chimera::edit::build(reel, request, 3, 3);
    need(bool(erased) && erased.reel->validFrames() == 600 &&
         erased.reel->readActive(99).l == 99.f &&
         erased.reel->readActive(100).l == 0.f &&
         erased.reel->readActive(299).r == 0.f &&
         erased.reel->readActive(300).l == 300.f &&
         reel.readSnapshot(100).l == 100.f,
         "erase zeros only the selected frozen region");
    request.kind = chimera::edit::DeleteSplice;
    chimera::edit::Result deleted = chimera::edit::build(reel, request, 3, 3);
    need(bool(deleted) && deleted.reel->validFrames() == 400 &&
         deleted.reel->markerCount() == 2 &&
         deleted.reel->markerId(1) == lastId &&
         deleted.reel->region(1).begin == 100 &&
         deleted.reel->readActive(100).l == 300.f,
         "delete compacts frames and preserves surviving marker IDs");
    request.kind = chimera::edit::MoveMarker; request.splice = 1; request.frame = 150;
    chimera::edit::Result moved = chimera::edit::build(reel, request, 3, 3);
    need(bool(moved) && moved.reel->region(1).begin == 150 &&
         moved.reel->markerId(1) == reel.markerId(1) &&
         moved.reel->readActive(150).l == 150.f,
         "marker move retains audio and stable identity");
    request.frame = 300;
    need(!chimera::edit::build(reel, request, 3, 3),
         "marker move cannot cross neighbor");
    request.kind = chimera::edit::RemoveMarker; request.splice = 1;
    chimera::edit::Result removed = chimera::edit::build(reel, request, 3, 3);
    need(bool(removed) && removed.reel->markerCount() == 2 &&
         removed.reel->region(1).begin == 300,
         "remove marker merges adjacent regions without touching samples");
    request.splice = 0;
    need(!chimera::edit::build(reel, request, 3, 3),
         "frame-zero marker cannot be removed");
    request.kind = chimera::edit::ClearReel;
    chimera::edit::Result cleared = chimera::edit::build(reel, request, 3, 3);
    need(bool(cleared) && cleared.reel->validFrames() == 0 &&
         cleared.reel->markerCount() == 0,
         "clear creates a genuinely empty Reel");
    std::puts("PASS: Chimera frozen worker edits and stable marker remapping");
}
