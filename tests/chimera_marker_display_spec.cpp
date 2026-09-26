#include "ChimeraMarkerDisplay.hpp"
#include <cstdio>
#include <cstdlib>
#include <thread>

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main() {
    chimera::Reel reel(2, 2);
    for (unsigned i = 0; i < 512; ++i)
        need(reel.write(i, {0.f, 0.f}, i), "prepare display fixture");
    chimera::MarkerDisplayMailbox mailbox;
    chimera::MarkerDisplay view;
    mailbox.publish(&reel, 1);
    need(mailbox.consume(view) && view.count == 1, "initial markers");
    need(reel.addMarker(100), "add marker");
    mailbox.publish(&reel, 1);
    need(reel.addMarker(200), "add another marker while UI is idle");
    mailbox.publish(&reel, 1);
    need(mailbox.consume(view) && view.count == 3 && view.markers[2] == 200,
         "consumer receives latest table after skipping updates");
    chimera::MarkerDisplay reopened;
    need(!mailbox.consume(reopened) && reopened.count == 3,
         "reopened overlay receives retained markers without a new edit");

    std::atomic<bool> done{false};
    std::thread writer([&] {
        for (unsigned i = 0; i < 20000; ++i) {
            const unsigned frame = 10 + i % 200;
            chimera::Marker markers[] = {{0, 1}, {frame, 2}, {frame + 1, 3}};
            need(reel.replaceMarkers(markers, 3), "replace marker table");
            mailbox.publish(&reel, 1);
        }
        done.store(true, std::memory_order_release);
    });
    while (!done.load(std::memory_order_acquire)) {
        if (mailbox.consume(view))
            need(view.count == 3 && view.frames == 512 &&
                 view.markers[2] == view.markers[1] + 1,
                 "concurrent marker table is coherent");
    }
    writer.join();
    mailbox.consume(view);
    need(view.markers[1] == 209 && view.markers[2] == 210, "final table delivered");
    mailbox.publish(nullptr, 0);
    need(mailbox.consume(view) && !view.count && !view.frames, "clear removes old markers");
    mailbox.publish(&reel, 2);
    need(mailbox.consume(view) && view.count == 3, "new reel handle republishes markers");
    std::puts("PASS: immediate marker display, skipped frames, reopen, clear and concurrent publication");
}
