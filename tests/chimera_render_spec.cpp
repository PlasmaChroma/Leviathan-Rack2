#include "../src/plugin.hpp"
#include <cstdio>
#include <cstdlib>
#include <thread>
Plugin* pluginInstance = nullptr;
bool isDragonKingDebugEnabled() { return false; }
std::string leviathanPluginUserRootPath() { return "build/tests/chimera_render_cache"; }
// The Pro Context destructor requires a fully initialized host; this harness
// owns only an event state, like the existing headless patch/dispatch harnesses.
namespace rack { Context::~Context() { delete event; rack::contextSet(nullptr); } }
#define CHIMERA_MANUAL_CONTROL_TEST 1
#define CHIMERA_HEADLESS_TEST 1
#include "../src/Chimera.cpp"
static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

static void displayCacheRegression() {
    Chimera module;
    chimera::Reel reel(200, 0);
    for (unsigned i = 0; i < 48000; ++i) reel.write(i, {0.f, 0.f}, i);
    reel.addMarker(12000); reel.addMarker(24000);
    module.markerDisplay.publish(&reel, 1);
    module.publishedValidFrames.store(48000);
    module.publishedMarkerCount.store(3);
    ChimeraDisplayOverlay display;
    display.initializeLayers();
    display.box.size = Vec(399.f, 77.f);
    display.owner = &module;
    display.step();
    const auto clean = [&] {
        display.markerCache->setDirty(false);
        display.foregroundCache->setDirty(false);
    };
    clean();
    module.publishedPlayFrame.store(1000);
    display.step();
    need(!display.markerCache->dirty && !display.foregroundCache->dirty,
         "playhead-only motion retains both caches");
    module.publishedRequestedRegion.store(1);
    display.step();
    need(!display.markerCache->dirty && display.foregroundCache->dirty,
         "selection updates footer and live highlights without rebuilding ordinary markers");
    clean();
    module.publishedValidFrames.store(48001);
    display.step();
    need(display.markerCache->dirty && !display.foregroundCache->dirty,
         "recording growth within the same second retains the footer");
    need(display.foregroundCache->box.pos.y == 63.f &&
         display.foregroundCache->box.size.y == 14.f && display.layers[4]->box.size.y == 14.f,
         "footer framebuffer covers only its visible strip");
    clean();
    reel.addMarker(36000);
    module.markerDisplay.publish(&reel, 1);
    module.publishedMarkerCount.store(4);
    display.step();
    need(display.markerCache->dirty && display.foregroundCache->dirty && display.markers.count == 4,
         "marker edits still refresh geometry and status");
    clean();
    display.box.size = Vec(500.f, 100.f);
    display.step();
    need(display.markerCache->dirty && display.foregroundCache->dirty &&
         display.foregroundCache->box.size.y == 18.f, "resize updates both cache extents");
    ChimeraDisplayOverlay reopened;
    reopened.owner = &module;
    reopened.step();
    need(reopened.markers.count == 4, "new overlay receives retained mailbox contents");
}

static void serviceHandoffRegression() {
    Chimera module;
    std::atomic<bool> locked{false}, release{false};
    std::thread service([&] {
        std::lock_guard<std::recursive_mutex> lock(module.controlMutex);
        locked.store(true);
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
        while (!release.load() && std::chrono::steady_clock::now() < deadline)
            std::this_thread::yield();
    });
    while (!locked.load()) std::this_thread::yield();
    const bool accepted = module.refreshRecoveryRootFromUi();
    release.store(true);
    service.join();
    need(!accepted, "UI identity publication skips a busy control dispatcher");
    need(module.refreshRecoveryRootFromUi() && !module.cachedRecoveryRoot.empty(),
         "UI identity publication retries when control becomes available");
}

static void visibilityRegression(bool recording, bool recovery) {
    Chimera module;
    module.service.reset(new chimera::IoService(0));
    std::unique_ptr<chimera::Reel> reel(new chimera::Reel(4, 4));
    reel->write(0, {0.25f, -0.25f}, 0);
    need(module.stores.accept(1, reel, chimera::StoreBudget::Active), "register display reel");
    module.audioActiveHandle = module.controlActiveHandle = 1;
    module.reel = module.stores.lookup(1);
    module.slice.setReel(module.reel);
    module.recordingActive.store(recording);
    module.recoveryPostPending.store(recovery);
    // Expired visibility must behave like a completely hidden display.
    module.displayHeartbeatNs.store(Chimera::steadyNs() - UINT64_C(600000000));
    module.recoveryStep();
    if (recovery) {
        need(module.snapshotRequestId && module.recoveryPurpose == 2,
             "required recovery proceeds even with no visible display");
    }
    else {
        need(!module.snapshotRequestId, "hidden displays do not request optional waveform scans");
        module.displayHeartbeatNs.store(Chimera::steadyNs());
        module.recoveryStep();
        need(module.snapshotRequestId && module.recoveryPurpose == 3,
             "visible display resumes optional waveform refresh");
    }
}

int main() {
    rack::Context context;
    context.event = new rack::widget::EventState;
    rack::contextSet(&context);
    displayCacheRegression();
    serviceHandoffRegression();
    visibilityRegression(false, false);
    visibilityRegression(true, false);
    visibilityRegression(false, true);
    chimera::RenderFrameMetrics frame;
    frame.glStepUs = 3.f;
    frame.addLayer(2.f, -1); frame.addDraw(10.f); frame.addLayer(5.f, 1);
    frame.cacheUs = 7.f; // Component subsets must not be counted again.
    need(frame.drew && frame.drawUs == 10.f && frame.lightUs == 5.f && frame.glStepUs == 3.f,
         "Draw contains only normal calls; layer and step work stay separate");
    { chimera::RenderComponentTimer disabled(&frame, true); }
    need(frame.cacheRenders == 0, "disabled debug mode does not collect component timings");
    ChimeraReelsWidget reels;
    reels.centers[0] = Vec(40.f, 30.f);
    reels.radii[0] = 25.f;
    reels.intakePoints[0] = Vec(80.f, 65.f);
    need(reels.drawBounds(0).contains(Rect(Vec(15.f, 5.f), Vec(50.f, 50.f))) &&
         reels.drawBounds(0).contains(reels.intakePoints[0]) &&
         !reels.drawBounds(0).intersects(Rect(Vec(0.f, 100.f), Vec(400.f, 200.f))),
         "reel culling includes the disk and tape intake but excludes lower controls");
    std::puts("PASS: Chimera display invalidation, footer bounds, nonblocking UI handoff, visibility gating, and full-frame timing");
}
