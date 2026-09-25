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
#define CHIMERA_MANUAL_CONTROL_TEST 1 // Deterministic state/ownership fixtures.
#include "../src/Chimera.cpp"

static void need(bool value, const char* message) {
    if (!value) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static void pump(Chimera& module, bool expectLoad = false) {
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f; args.sampleTime = 1.f / 48000.f;
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((!module.controlActiveHandle || (expectLoad && !module.reel)) &&
           std::chrono::steady_clock::now() < deadline) {
        module.serviceStep(); module.process(args); module.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(module.controlActiveHandle && module.reel, "adopt prepared or loaded Reel");
}
int main() {
    rack::Context context;
    rack::contextSet(&context);
    rack::settings::headless = true;
    rack::engine::Engine engine;
    context.engine = &engine;
    rack::history::State history;
    context.history = &history;
    rack::patch::Manager* patch = new rack::patch::Manager;
    context.patch = patch;
    const std::string runRoot = "build/tests/chimera_patch_" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    checkpointRoot = rack::system::join(runRoot, "plugin_cache");
    need(rack::system::createDirectories(runRoot), "isolated patch test root");
    patch->autosavePath = rack::system::join(runRoot, "autosave");
    rack::plugin::Plugin plugin;
    plugin.slug = "Leviathan"; plugin.version = "0.0.0";
    modelChimera->plugin = &plugin;
    Chimera source;
    source.model = modelChimera;
    engine.addModule(&source);
    need(source.requestPreparedStore(2, 2) == chimera::IoService::Accepted,
         "prepare small Rack-hosted Reel");
    pump(source);
    for (std::uint32_t i = 0; i < 300; ++i)
        need(source.reel->write(i, {float(i) / 100.f, -float(i) / 200.f}, i),
             "record fixture samples");
    need(source.reel->addMarker(100), "record fixture marker");
    const std::string archive = rack::system::join(runRoot, "saved.vcv");
    patch->save(archive);
    json_t* state = source.dataToJson();
    json_t* storage = json_object_get(state, "storage");
    const char* manifestText = json_string_value(json_object_get(storage, "manifest"));
    need(manifestText && !source.saveFailure.load() &&
         json_string_value(json_object_get(state, "audioStatus")),
         "save hook publishes embedded manifest before JSON");
    const std::string manifest(manifestText);
    const std::string sourceStorage = source.getPatchStorageDirectory();
    need(bool(chimera::bundle::load(sourceStorage, manifest, 2)),
         "Rack save created loadable embedded Reel");
    const std::string extracted = rack::system::join(runRoot, "clean_extract");
    need(rack::system::createDirectories(extracted), "create clean extract directory");
    rack::system::unarchiveToDirectory(archive, extracted);
    json_error_t parseError;
    json_t* archivedPatch = json_load_file(
        rack::system::join(extracted, "patch.json").c_str(), 0, &parseError);
    const char* archivedManifest = archivedPatch ? json_string_value(json_object_get(
        json_object_get(json_object_get(json_array_get(
            json_object_get(archivedPatch, "modules"), 0), "data"),
            "storage"), "manifest")) : nullptr;
    need(archivedManifest && manifest == archivedManifest,
         "archived patch JSON references the committed manifest");
    json_decref(archivedPatch);
    const std::string archivedStorage = rack::system::join(extracted, "modules",
        std::to_string(source.id));
    chimera::bundle::LoadResult portable =
        chimera::bundle::load(archivedStorage, manifest, 2);
    need(bool(portable) && portable.reel->readActive(200).r == -1.f,
         "archived patch reopens its embedded Reel independently of plugin cache");
    need(source.reel->write(0, {77.f, 77.f}, 301), "make an unsaved recording change");
    patch->saveAutosave();
    json_t* autosave = source.dataToJson();
    need(std::string(json_string_value(json_object_get(json_object_get(autosave,
         "storage"), "manifest"))) == manifest,
         "periodic serialization keeps prior committed audio cut");
    json_decref(autosave);
    // Rack's single-module duplicate prepares storage, then copies its
    // module directory before loading JSON into the new independent instance.
    engine.prepareSaveModule(&source);
    json_decref(state);
    state = source.dataToJson();
    storage = json_object_get(state, "storage");
    manifestText = json_string_value(json_object_get(storage, "manifest"));
    need(manifestText && std::string(manifestText) != manifest,
         "duplicate preparation captures newer revision");
    need(!rack::system::isFile(rack::system::join(sourceStorage, manifest)) &&
         !rack::system::isFile(rack::system::join(sourceStorage,
             manifest.substr(0, manifest.size() - 5) + ".wav")) &&
         bool(chimera::bundle::load(sourceStorage, manifestText, 2)),
         "successful save prunes obsolete assets before Rack copies module storage");
    Chimera clone;
    clone.model = modelChimera;
    clone.id = source.id + 1;
    const std::string cloneStorage = rack::system::join(patch->autosavePath,
        "modules", std::to_string(clone.id));
    need(rack::system::copy(sourceStorage, cloneStorage),
         "copy module-relative assets for duplicate");
    clone.dataFromJson(state);
    engine.addModule(&clone);
    pump(clone, true);
    need(clone.reel != source.reel && clone.reel->validFrames() == 300 &&
         clone.reel->readActive(0).l == 77.f,
         "duplicate loads its own embedded audio without source file");
    json_t* reopenedState = clone.dataToJson();
    need(std::string(json_string_value(json_object_get(reopenedState, "audioStatus"))) ==
         "embedded" && clone.committedAudioRevision == clone.reel->audioRevision(),
         "reopened Reel restores committed revision status");
    json_decref(reopenedState);
    need(clone.reel->write(0, {13.f, 13.f}, 302) &&
         source.reel->readActive(0).l == 77.f,
         "duplicate and original have independent mutable stores");
    const std::string interchange = rack::system::join(runRoot, "selected-reel.wav");
    std::string error;
    need(source.exportWav(interchange, false, error) && rack::system::isFile(interchange),
         "user-selected WAV export writes one Reel");
    need(clone.requestImportWav(interchange, false, error),
         "user-selected WAV import queues off-audio decode");
    const auto importDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f; args.sampleTime = 1.f / 48000.f;
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < importDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && !clone.awaitingHandle &&
         clone.reel->readActive(0).l == 77.f &&
         clone.unsavedImport.load(std::memory_order_acquire),
         "import replaces a Reel at the audio-owner handoff and marks it unsaved");
    {
        const auto recoveryDeadline = std::chrono::steady_clock::now() +
            std::chrono::seconds(10);
        while ((clone.snapshotRequestId || clone.recoveryPurpose || clone.recoveryTicket ||
                clone.recoveryPostPending.load()) &&
               std::chrono::steady_clock::now() < recoveryDeadline) {
            clone.serviceStep(); clone.process(args); clone.serviceStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        need(!clone.snapshotRequestId && !clone.recoveryPurpose &&
             !clone.recoveryTicket && !clone.recoveryPostPending.load(),
             "import recovery checkpoint settles before destructive edit");
    }
    need(clone.reel->write(0, {19.f, 19.f}, 303) &&
         source.reel->readActive(0).l == 77.f,
         "imported Reel is independent of exported source");
    chimera::edit::Request erase;
    erase.kind = chimera::edit::EraseSplice;
    erase.splice = 0;
    need(clone.requestEdit(erase, error), "queue frozen destructive edit with checkpoint");
    const auto editDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < editDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && !clone.awaitingHandle && !clone.undoCheckpoint.empty() &&
         rack::system::isFile(clone.undoCheckpoint) &&
         clone.reel->readActive(0).l == 0.f &&
         clone.reel->readActive(100).l == 1.f,
         "erase publishes zeroed Splice only after durable Undo checkpoint");
    const std::string firstUndo = clone.undoCheckpoint;
    need(clone.requestUndo(false, error), "queue Undo from immutable checkpoint");
    clone.pruneEditCheckpoints();
    need(rack::system::isFile(firstUndo), "cleanup preserves checkpoint leased by restore worker");
    const auto undoDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < undoDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && clone.reel->readActive(0).l == 19.f &&
         !clone.redoCheckpoint.empty(),
         "Undo restores previous audio and publishes a Redo checkpoint");
    need(!rack::system::exists(firstUndo) && clone.editCheckpointFiles.size() == 1,
         "Undo removes superseded full-reel checkpoint after restore finishes");
    const std::string firstRedo = clone.redoCheckpoint;
    need(clone.requestUndo(true, error), "queue Redo from immutable checkpoint");
    const auto redoDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < redoDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && clone.reel->readActive(0).l == 0.f,
         "Redo re-applies the selected destructive edit");
    need(!rack::system::exists(firstRedo) && clone.editCheckpointFiles.size() == 1,
         "Redo keeps only the reachable history checkpoint");
    need(clone.requestEdit(erase, error), "queue a second fenced edit");
    const std::string rejectedCheckpoint = clone.loadTicket->checkpointPath;
    need(clone.reel->write(0, {42.f, 42.f}, 304),
         "mutate audio after the edit snapshot but before adoption");
    const auto staleDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < staleDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && !clone.awaitingHandle &&
         clone.reel->readActive(0).l == 42.f && clone.ioError.load(),
         "stale edit is rejected without replacing a newer recording change");
    need(!rack::system::exists(rejectedCheckpoint) && clone.editCheckpointFiles.size() == 1,
         "rejected adoption removes its unused checkpoint and retains valid Undo");
    clone.ioError.store(false);
    chimera::edit::Request clear;
    clear.kind = chimera::edit::ClearReel;
    need(clone.requestEdit(clear, error), "queue explicit Clear Reel with Undo checkpoint");
    const auto clearDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < clearDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && clone.reel->validFrames() == 0,
         "Clear Reel adopts an actually empty store");
    engine.prepareSaveModule(&clone);
    json_t* emptyState = clone.dataToJson();
    const char* emptyManifest = json_string_value(json_object_get(
        json_object_get(emptyState, "storage"), "manifest"));
    need(emptyManifest && bool(chimera::bundle::load(cloneStorage, emptyManifest, 2)),
         "empty Reel saves a valid manifest without dummy full-length audio");
    json_decref(emptyState);
    need(clone.requestUndo(false, error), "Undo can restore an explicitly cleared Reel");
    const auto restoreDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < restoreDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && clone.reel->readActive(0).l == 42.f,
         "Undo restores the pre-clear audio checkpoint");
    auto settleRecovery = [&] {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        do {
            clone.serviceStep(); clone.process(args); clone.serviceStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } while ((clone.snapshotRequestId || clone.recoveryPurpose || clone.recoveryTicket ||
                  clone.recoveryPostPending.load()) &&
                 std::chrono::steady_clock::now() < deadline);
        need(!clone.snapshotRequestId && !clone.recoveryPurpose &&
             !clone.recoveryTicket && !clone.recoveryPostPending.load(),
             "recovery snapshot reaches durable idle state");
    };
    settleRecovery();
    clone.params[Chimera::SOS_PARAM].setValue(0.f);
    clone.menuCommand.store(1); clone.process(args); // Current, first frame overwritten.
    need(clone.slice.recordState() == chimera::Slice::Current &&
         clone.reel->readActive(0).l == 0.f,
         "record starts on its exact core frame while pre-cut is protected");
    settleRecovery();
    clone.lastRecoveryNs = Chimera::steadyNs() - UINT64_C(10000000000);
    clone.recordStartNs.store(clone.lastRecoveryNs, std::memory_order_release);
    clone.serviceStep();
    need(clone.recoveryPurpose == 2 && clone.snapshotRequestId,
         "active recording requests a new cut at the ten-second cadence");
    settleRecovery();
    const auto periodicJournal = chimera::recovery::inspect(clone.recoveryRoot());
    need(bool(periodicJournal.latest), "periodic recording cut is durable");
    clone.serviceStep();
    need(!clone.snapshotRequestId && !clone.recoveryPurpose,
         "periodic capture does not immediately repeat before ten seconds");
    clone.menuCommand.store(3); clone.process(args);
    settleRecovery();
    auto recoveryJournal = chimera::recovery::inspect(clone.recoveryRoot());
    need(bool(recoveryJournal.preRecord) && bool(recoveryJournal.latest) &&
         recoveryJournal.latest.capturedAtMs >= recoveryJournal.preRecord.capturedAtMs,
         "pre-record and completed-session checkpoints are durably journaled");
    auto preRecord = chimera::recovery::load(clone.recoveryRoot(),
                                             recoveryJournal.preRecord, 2);
    auto completed = chimera::recovery::load(clone.recoveryRoot(),
                                             recoveryJournal.latest, 2);
    need(bool(preRecord) && bool(completed) &&
         preRecord.reel->readActive(0).l == 42.f &&
         completed.reel->readActive(0).l == 0.f,
         "journaled pre-cut survives the first overwritten frame");
    need(clone.requestRecovery(true, error), "queue explicit pre-record restore");
    const auto recoverDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while ((clone.loadTicket || clone.awaitingHandle || clone.retiringHandle) &&
           std::chrono::steady_clock::now() < recoverDeadline) {
        clone.serviceStep(); clone.process(args); clone.serviceStep();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    need(!clone.loadTicket && clone.reel->readActive(0).l == 42.f,
         "explicit pre-record recovery adopts the old audio off thread");
    const auto oldMarkerCount = clone.reel->markerCount();
    need(oldMarkerCount > 1, "marker save fixture has multiple splices");
    chimera::edit::Request removeMarker;
    removeMarker.kind = chimera::edit::RemoveMarker;
    removeMarker.splice = 1;
    need(clone.requestEdit(removeMarker, error) && clone.markerRequestId,
         "queue metadata edit immediately before stopped-engine Save");
    engine.prepareSaveModule(&clone);
    auto markerSave = chimera::bundle::load(cloneStorage, clone.committedManifest, 2);
    need(!clone.markerRequestId && !clone.saveFailure.load() && bool(markerSave) &&
         markerSave.reel->markerCount() == oldMarkerCount - 1 &&
         clone.publishedMarkerCount.load() == oldMarkerCount - 1 &&
         markerSave.reel->readActive(0).l == 42.f,
         "Save drains metadata handoff without audio callbacks and persists updated markers");
    json_decref(state);
    engine.removeModule(&clone);
    engine.removeModule(&source);
    delete patch;
    context.patch = nullptr;
    context.history = nullptr;
    context.engine = nullptr;
    rack::contextSet(nullptr);
    std::puts("PASS: Rack Chimera patch, import/export, destructive edit, Undo/Redo");
}
