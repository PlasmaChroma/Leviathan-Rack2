#include "ChimeraEdit.hpp"
#include <cstdio>
#include <cstdlib>

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
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
