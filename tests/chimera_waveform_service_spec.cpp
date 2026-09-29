#include "ChimeraWaveformService.hpp"
#include "ChimeraJobs.hpp"
#include <cstdio>
#include <cstdlib>
#include <limits>

using namespace chimera;
static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static void freeze(Reel& reel) {
    need(reel.beginSnapshot(0), "capture cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
}
static void release(Reel& reel) {
    need(reel.beginRelease(), "release cut");
    while (reel.snapshotWorkPending()) reel.maintenanceTick();
}
static void equivalent(const WaveformSummary& a, const WaveformSummary& b) {
    need(a.frames == b.frames && a.markerCount == b.markerCount && a.markers == b.markers &&
         a.documentRevision == b.documentRevision && a.audioRevision == b.audioRevision &&
         a.peak == b.peak && a.stereo == b.stereo && a.leftLow == b.leftLow &&
         a.leftHigh == b.leftHigh && a.rightLow == b.rightLow && a.rightHigh == b.rightHigh,
         "incremental summary exactly matches full scan, including bin edges");
}
static std::shared_ptr<WaveformJob> build(Reel& reel, std::shared_ptr<WaveformPageCache> cache) {
    auto job = std::make_shared<WaveformJob>(&reel, cache);
    while (!job->advance()) {}
    need(bool(job->result), "complete summary published");
    equivalent(*job->result, *WaveformSummary::fromSnapshot(reel));
    return job;
}
static void incremental() {
    auto cache = std::make_shared<WaveformPageCache>();
    Reel reel(2048, 2048);
    freeze(reel); build(reel, cache); release(reel);
    for (unsigned length : {1u, 255u, 256u, 511u, 513u, 8193u, 300001u}) {
        for (unsigned i = reel.validFrames(); i < length; ++i)
            need(reel.appendImported({float(int(i % 83) - 40) / 40.f,
                                      float(int(i % 71) - 35) / 35.f}), "append");
        freeze(reel); build(reel, cache); release(reel);
    }
    reel.addMarker(16000);
    freeze(reel);
    auto unchanged = build(reel, cache);
    need(unchanged->pagesRebuilt == 0 && unchanged->framesRead <= 511 * kPageFrames,
         "marker-only refresh reuses all peaks with bounded exact edge reads");
    release(reel);
    reel.restoreRevisions(1, 1); // Serialized revision changes cannot alias page versions.
    reel.write(16001, {4.f, -3.f}, 0);
    reel.write(16002, {std::numeric_limits<float>::quiet_NaN(), 5.f}, 0);
    reel.write(16003, {3.f, std::numeric_limits<float>::infinity()}, 0);
    freeze(reel);
    // A later write COWs this page; its new revision must not leak into the cut.
    reel.write(16001, {8.f, 8.f}, 0);
    auto changed = build(reel, cache);
    need(changed->pagesRebuilt == 1 && changed->result->peak == 4.f,
         "one dirty page rebuilt from its frozen version");
    release(reel);
    freeze(reel);
    need(build(reel, cache)->result->peak == 8.f, "next cut sees later COW write");
    release(reel);
    Reel replacement(2, 0);
    replacement.appendImported({-9.f, 1.f}); freeze(replacement);
    need(build(replacement, cache)->result->peak == 9.f, "new Reel invalidates old cache");
    release(replacement);
}
static void fairnessAndCancellation() {
    Reel reel(100, 0);
    for (unsigned i = 0; i < reel.capacityFrames(); ++i) reel.appendImported({0.5f, 0.f});
    freeze(reel);
    WaveformService service(false);
    std::vector<std::shared_ptr<WaveformJob>> jobs;
    for (unsigned i = 0; i < 8; ++i) {
        jobs.push_back(std::make_shared<WaveformJob>(&reel, std::make_shared<WaveformPageCache>()));
        need(service.submit(jobs.back()), "admit independent module");
    }
    need(!service.submit(std::make_shared<WaveformJob>(&reel, jobs[0]->cache)),
         "coalescing bounds each module to one admitted job");
    for (unsigned round = 1; round <= 3; ++round) {
        for (unsigned i = 0; i < jobs.size(); ++i) {
            need(service.runOneTurn(), "run bounded turn");
            need(jobs[i]->turns == round && jobs[i]->framesRead == round * 8192,
                 "busy module cannot take a second turn ahead of another");
        }
    }
    jobs[0]->cancelled.store(true);
    service.runOneTurn();
    need(jobs[0]->done.load() && !jobs[0]->result, "cancellation never publishes partial bins");
    for (unsigned i = 1; i < jobs.size(); ++i) {
        service.runOneTurn();
        equivalent(*jobs[i]->result, *WaveformSummary::fromSnapshot(reel));
    }
    auto resume = std::make_shared<WaveformJob>(&reel, jobs[0]->cache);
    service.submit(resume);
    while (service.runOneTurn()) {}
    need(resume->pagesRebuilt == 4, "cancelled pass retains completed page-cache progress");
    for (unsigned i = 0; i < WaveformService::kMaxJobs; ++i)
        need(service.submit(std::make_shared<WaveformJob>(&reel,
             std::make_shared<WaveformPageCache>())), "bounded queue admits up to cap");
    auto rejected = std::make_shared<WaveformJob>(&reel, std::make_shared<WaveformPageCache>());
    need(!service.submit(rejected), "queue rejects excess work without retaining its cut");
    service.shutdown();
    need(!service.submit(rejected), "shutdown closes admission");
    release(reel);
}
static void threadedIsolation() {
    Reel reel(512, 512);
    for (unsigned i = 0; i < reel.capacityFrames(); ++i) reel.appendImported({0.5f, -0.5f});
    freeze(reel);
    IoService io;
    auto token = std::make_shared<JobGeneration>();
    std::atomic<unsigned> entered{0};
    std::atomic<bool> unblock{false};
    for (unsigned i = 0; i < 2; ++i) io.execute(token, i, [&] {
        entered.fetch_add(1);
        while (!unblock.load()) std::this_thread::yield();
    });
    while (entered.load() != 2) std::this_thread::yield();
    WaveformService worker;
    auto job = std::make_shared<WaveformJob>(&reel, std::make_shared<WaveformPageCache>());
    worker.submit(job);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!job->done.load(std::memory_order_acquire) && std::chrono::steady_clock::now() < deadline) {
        // Concurrent active writes exercise frozen version/page ownership.
        reel.write(42, {9.f, 9.f}, 0);
        std::this_thread::yield();
    }
    const bool completed = job->done.load(std::memory_order_acquire);
    unblock.store(true); io.shutdown(); worker.shutdown();
    need(completed && job->result && job->result->peak == 0.5f,
         "waveform completes while both I/O workers are blocked and core writes continue");
    release(reel);
}
static void manyInstanceBenchmark() {
    const unsigned modules = 12, frames = 480000;
    WaveformService scheduler(false);
    std::vector<std::unique_ptr<Reel>> reels;
    std::vector<std::shared_ptr<WaveformPageCache>> caches;
    for (unsigned i = 0; i < modules; ++i) {
        reels.emplace_back(new Reel(frames / kPageFrames, 256));
        for (unsigned f = 0; f < frames; ++f) reels.back()->appendImported({float(f % 53) / 53.f, 0.f});
        caches.push_back(std::make_shared<WaveformPageCache>());
    }
    for (unsigned pass = 0; pass < 2; ++pass) {
        std::vector<std::shared_ptr<WaveformJob>> jobs;
        for (unsigned i = 0; i < modules; ++i) {
            if (pass) for (unsigned f = 0; f < 48000; ++f) reels[i]->write(f, {0.75f, -0.25f}, f);
            freeze(*reels[i]);
            jobs.push_back(std::make_shared<WaveformJob>(reels[i].get(), caches[i]));
            scheduler.submit(jobs.back());
        }
        std::vector<double> turns;
        const auto start = std::chrono::steady_clock::now();
        for (;;) {
            const auto turn = std::chrono::steady_clock::now();
            if (!scheduler.runOneTurn()) break;
            turns.push_back(std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - turn).count());
        }
        const double total = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        std::sort(turns.begin(), turns.end());
        std::uint64_t reads = 0;
        for (unsigned i = 0; i < modules; ++i) {
            need(jobs[i]->done.load() && jobs[i]->result, "every busy module completes");
            equivalent(*jobs[i]->result, *WaveformSummary::fromSnapshot(*reels[i]));
            reads += jobs[i]->framesRead;
            release(*reels[i]);
        }
        std::printf("12 distinct 10-second Reels, %s: %.2f ms total; turn p99 %.2f us, max %.2f us; %llu frames read\n",
            pass ? "1-second overdub refresh" : "cold", total, turns[turns.size()*99/100], turns.back(),
            static_cast<unsigned long long>(reads));
        if (pass) need(reads < std::uint64_t(frames) * modules / 2, "dirty refresh avoids most source reads");
    }
}
int main() {
    incremental(); fairnessAndCancellation(); threadedIsolation(); manyInstanceBenchmark();
    std::puts("PASS: exact incremental waveforms, fair bounded turns, coalescing, cancellation, COW and I/O isolation");
}
