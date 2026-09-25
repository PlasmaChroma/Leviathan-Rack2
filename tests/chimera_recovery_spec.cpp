#include "ChimeraRecovery.hpp"
#include <system.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main() {
    const std::string root = "build/tests/chimera_recovery_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    chimera::Reel reel(2, 2);
    for (std::uint32_t i = 0; i < 300; ++i)
        need(reel.write(i, {float(i) / 100.f, -float(i) / 200.f}, i), "seed Reel");
    need(reel.addMarker(100), "seed marker");
    need(reel.beginSnapshot(300), "freeze pre-record cut");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    const auto pre = chimera::recovery::commit(root, "one", reel,
                                                chimera::recovery::PreRecord, 1000);
    need(bool(pre), "publish pre-record checkpoint and journal");
    need(reel.beginRelease(), "release pre-record cut");
    while (reel.state() != chimera::Reel::Idle) reel.maintenanceTick();
    need(reel.write(0, {77.f, 77.f}, 301), "record new data");
    need(reel.beginSnapshot(301), "freeze completed recording");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    const auto latest = chimera::recovery::commit(root, "two", reel,
                                                   chimera::recovery::Latest, 2000);
    need(bool(latest), "publish latest checkpoint over existing journal");
    auto journal = chimera::recovery::inspect(root);
    need(journal.preRecord.manifest == pre.entry.manifest &&
         journal.latest.manifest == latest.entry.manifest &&
         journal.latest.capturedAtMs == 2000,
         "journal retains latest and pre-record entries");
    auto recovered = chimera::recovery::load(root, journal.preRecord, 2);
    need(bool(recovered) && recovered.reel->readActive(0).l == 0.f &&
         recovered.reel->markerId(1) == reel.markerId(1),
         "pre-record restore keeps audio and marker IDs");
    recovered = chimera::recovery::load(root, journal.latest, 2);
    need(bool(recovered) && recovered.reel->readActive(0).l == 77.f,
         "latest checkpoint restores completed recording");
    need(reel.beginRelease(), "release latest cut");
    while (reel.state() != chimera::Reel::Idle) reel.maintenanceTick();
    need(reel.write(1, {88.f, 88.f}, 302), "write next generation");
    need(reel.beginSnapshot(302), "freeze next generation");
    while (!reel.readyForWorker()) reel.maintenanceTick();
    need(bool(chimera::bundle::commit(root, "orphan", reel)),
         "simulate crash after asset commit but before journal publication");
    {
        std::ofstream interrupted((root + "/journal.json.tmp").c_str());
        interrupted << "incomplete journal";
    }
    need(chimera::recovery::inspect(root).latest.manifest == latest.entry.manifest,
         "interrupted publication leaves the prior committed journal visible");
    const auto next = chimera::recovery::commit(root, "three", reel,
                                                 chimera::recovery::Latest, 3000);
    need(bool(next), "publish third generation");
    need(!rack::system::isFile(root + "/chimera/reel-two.wav") &&
         !rack::system::isFile(root + "/chimera/reel-orphan.wav") &&
         rack::system::isFile(root + "/chimera/reel-one.wav") &&
         rack::system::isFile(root + "/chimera/reel-three.wav"),
         "prune only unreferenced checkpoint generations");
    journal = chimera::recovery::inspect(root);
    need(journal.latest.manifest == next.entry.manifest &&
         bool(chimera::recovery::load(root, journal.latest, 2)),
         "latest journal survives repeated commits");
    std::puts("PASS: Chimera recovery journal, restore, and retention");
}
