#include <context.hpp>
#include <engine/Engine.hpp>
#include <patch.hpp>
#include <history.hpp>
#include <plugin/Plugin.hpp>
#include <settings.hpp>
#include <system.hpp>
#include "../src/plugin.hpp"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace rack { Context::~Context() {} }
Plugin* pluginInstance = nullptr;
bool isDragonKingDebugEnabled() { return false; }
static std::string checkpointRoot;
std::string leviathanPluginUserRootPath() { return checkpointRoot; }
#define CHIMERA_HEADLESS_TEST 1
#include "../src/Chimera.cpp"

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

int main() {
    rack::Context context;
    rack::contextSet(&context);
    rack::settings::headless = true;
    rack::engine::Engine engine;
    rack::history::State history;
    rack::patch::Manager* patch = new rack::patch::Manager;
    context.engine = &engine; context.history = &history; context.patch = patch;
    const std::string root = "build/tests/chimera_dispatch_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    checkpointRoot = root + "/cache";
    rack::system::createDirectories(root);
    patch->autosavePath = root + "/autosave";
    rack::plugin::Plugin plugin; plugin.slug = "Leviathan"; plugin.version = "0.0.0";
    modelChimera->plugin = &plugin;
    {
        Chimera m; m.model = modelChimera;
        engine.addModule(&m); // Real onAdd starts the dispatcher, no test switch.
        need(m.controlThread.joinable(), "Rack onAdd starts background dispatch");
        need(m.requestPreparedStore(4, 4) == chimera::IoService::Accepted, "headless preparation");
        Module::ProcessArgs args{}; args.sampleRate = 48000; args.sampleTime = 1.f/48000;
        auto waitFor = [&](const std::function<bool()>& done, bool audio) {
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
            while (std::chrono::steady_clock::now() < deadline) {
                if (audio) for (unsigned i = 0; i < 128; ++i) m.process(args);
                { std::lock_guard<std::recursive_mutex> lock(m.controlMutex);
                  if (done()) return; }
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
            }
            need(false, "headless completion deadline");
        };
        waitFor([&] { return m.controlActiveHandle && !m.awaitingHandle; }, true);
        m.menuCommand.store(2);
        waitFor([&] { return m.publishedValidFrames.load() >= 300; }, true);
        m.menuCommand.store(3);
        waitFor([&] { return !m.recordingActive.load() && !m.recoveryPostPending.load() &&
            !m.recoveryTicket && !m.overlapActive.load() && !m.recoveryPurpose && !m.snapshotRequestId; }, true);
        need(bool(chimera::recovery::inspect(m.recoveryRoot()).latest), "cached host path recovery");
        engine.prepareSaveModule(&m);
        need(!m.saveFailure.load(), "stopped host Save cooperates with dispatcher");
        std::string error;
        const std::string exported = root + "/input.wav";
        need(m.exportWav(exported, false, error), "headless saved audio export fixture");
        need(m.requestImportWav(exported, false, error), "queue import while window closed");
        waitFor([&] { return !m.loadTicket && !m.awaitingHandle && !m.retiringHandle; }, false);
        need(m.publishedValidFrames.load() >= 300, "headless import finishes");
        // Reopen: normal widget control polling may overlap the background pump.
        for (unsigned i = 0; i < 100; ++i) { m.serviceStep(); m.process(args); }
        engine.prepareSaveModule(&m);
        need(!m.saveFailure.load(), "save after resumed widget stepping");
        need(m.requestImportWav(exported, false, error), "queue import before removal");
        engine.removeModule(&m);
        need(!m.controlThread.joinable(), "onRemove joins before module destruction");
    }
    delete patch;
    context.patch = nullptr; context.history = nullptr; context.engine = nullptr;
    rack::contextSet(nullptr);
    std::puts("PASS: automatic headless dispatcher, host-path recovery, stopped Save, import, reopen and removal");
}
