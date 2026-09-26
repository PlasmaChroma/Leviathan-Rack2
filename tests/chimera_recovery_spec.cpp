#include "ChimeraRecovery.hpp"
#include "ChimeraCheckpointSession.hpp"
#include <system.hpp>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <atomic>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#include <vector>
#endif

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main(int argc, char** argv) {
    if (argc == 3 && std::string(argv[1]) == "--commit-child") {
        chimera::Reel childReel(2, 2);
        childReel.write(0, {.5f, -.5f}, 0);
        childReel.beginSnapshot(1);
        while (!childReel.readyForWorker()) childReel.maintenanceTick();
        return chimera::recovery::commit(argv[2], "child", childReel,
            chimera::recovery::Latest, 5000) ? 0 : 2;
    }
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
    recovered = chimera::recovery::load(root, latest.entry, 2);
    need(!recovered && recovered.error == "recovery_entry_superseded",
         "a pruned generation is rejected before opening its old asset");
    journal = chimera::recovery::inspect(root);
    need(journal.latest.manifest == next.entry.manifest &&
         bool(chimera::recovery::load(root, journal.latest, 2)),
         "latest journal survives repeated commits");

    // Two instances of one saved patch share a recovery root. Their journal
    // transactions and pruning must remain valid across simultaneous workers.
    const std::string sharedRoot = root + "_shared";
    for (unsigned attempt = 0; attempt < 32; ++attempt) {
        std::atomic<unsigned> arrived{0};
        chimera::recovery::CommitResult first, second;
        const std::string preId = "pre-" + std::to_string(attempt);
        const std::string latestId = "latest-" + std::to_string(attempt);
        auto publish = [&](bool preRecord) {
            arrived.fetch_add(1, std::memory_order_acq_rel);
            while (arrived.load(std::memory_order_acquire) != 2)
                std::this_thread::yield();
            auto committed = chimera::recovery::commit(sharedRoot,
                preRecord ? preId : latestId, reel,
                preRecord ? chimera::recovery::PreRecord : chimera::recovery::Latest,
                4000 + attempt);
            if (preRecord) first = std::move(committed);
            else second = std::move(committed);
        };
        std::thread a(publish, true), b(publish, false);
        a.join(); b.join();
        need(bool(first) && bool(second), "concurrent recovery commits complete");
        const auto selected = chimera::recovery::inspect(sharedRoot);
        need(selected.preRecord.manifest == first.entry.manifest &&
             selected.latest.manifest == second.entry.manifest,
             "concurrent commits retain both journal roles");
        need(bool(chimera::recovery::load(sharedRoot, selected.preRecord, 2)) &&
             bool(chimera::recovery::load(sharedRoot, selected.latest, 2)),
             "both selected recovery assets remain loadable after pruning");
    }
#ifdef _WIN32
    const std::string processRoot = root + "_process";
    need(rack::system::createDirectories(processRoot), "create process fixture root");
    chimera::CheckpointLease processLease;
    need(processLease.open(processRoot + "/journal.lock"),
         "parent acquires recovery journal lease");
    std::wstring executable(MAX_PATH, L'\0');
    const DWORD executableLength = GetModuleFileNameW(nullptr, &executable[0], MAX_PATH);
    need(executableLength > 0 && executableLength < MAX_PATH,
         "locate native recovery test executable");
    executable.resize(executableLength);
    const std::wstring wideRoot(processRoot.begin(), processRoot.end());
    std::wstring command = L"\"" + executable + L"\" --commit-child \"" +
        wideRoot + L"\"";
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');
    STARTUPINFOW startup{}; startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    need(CreateProcessW(executable.c_str(), mutableCommand.data(), nullptr,
                        nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr,
                        &startup, &process), "launch separate recovery writer process");
    need(WaitForSingleObject(process.hProcess, 200) == WAIT_TIMEOUT,
         "separate process waits while the journal lease is held");
    processLease.close();
    need(WaitForSingleObject(process.hProcess, 10000) == WAIT_OBJECT_0,
         "separate writer completes after lease release");
    DWORD childCode = 1;
    need(GetExitCodeProcess(process.hProcess, &childCode) && childCode == 0,
         "separate writer committed after lease release");
    CloseHandle(process.hProcess); CloseHandle(process.hThread);
    const auto processJournal = chimera::recovery::inspect(processRoot);
    need(bool(chimera::recovery::load(processRoot, processJournal.latest, 2)),
         "separate writer's selected asset loads");
#endif
    std::puts("PASS: Chimera recovery journal, restore, and retention");
}
