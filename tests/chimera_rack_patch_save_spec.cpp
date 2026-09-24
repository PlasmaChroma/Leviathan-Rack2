// Phase 0 host-path probe. Uses private Rack constructors only in this test.
#include <context.hpp>
#include <engine/Engine.hpp>
#include <engine/Module.hpp>
#include <patch.hpp>
#include <history.hpp>
#include <plugin/Plugin.hpp>
#include <plugin/Model.hpp>
#include <settings.hpp>
#include <system.hpp>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <cstdlib>

namespace rack { Context::~Context() {} }

static void require(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}

struct Probe : rack::engine::Module {
    int live = 0;
    int committed = 0;
    int saves = 0;
    bool fail = false;
    bool ioFailure = false;
    int checkpointRevision = 0;
    std::string cacheRoot;
    Probe() { config(0, 0, 0, 0); }
    void process(const ProcessArgs&) override {}
    void publishCheckpoint() {
        require(rack::system::isDirectory(cacheRoot) || rack::system::createDirectories(cacheRoot),
                "create same-process immutable cache");
        const std::string path = rack::system::join(cacheRoot,
            "checkpoint-" + std::to_string(live) + ".bin");
        FILE* f = std::fopen(path.c_str(), "wb");
        require(f != nullptr, "open immutable checkpoint");
        const bool written = std::fprintf(f, "revision=%d\n", live) > 0;
        const bool closed = std::fclose(f) == 0;
        require(written && closed,
                "write immutable checkpoint");
        checkpointRevision = live;
    }
    void onSave(const SaveEvent&) override {
        ++saves;
        const std::string root = rack::system::join(createPatchStorageDirectory(), "chimera");
        if (!rack::system::isDirectory(root) && !rack::system::createDirectories(root)) {
            ioFailure = true; return;
        }
        const std::string audio = rack::system::join(root, "audio-" + std::to_string(live) + ".bin");
        const std::string audioTmp = audio + ".tmp";
        FILE* f = std::fopen(audioTmp.c_str(), "wb");
        if (!f) { ioFailure = true; return; }
        const bool audioWritten = std::fprintf(f, "revision=%d\n", live) > 0;
        const bool audioFlushed = std::fflush(f) == 0;
        const bool audioClosed = std::fclose(f) == 0;
        if (!audioWritten || !audioFlushed || !audioClosed ||
                !rack::system::rename(audioTmp, audio)) { ioFailure = true; return; }
        // Inject a failure after the new audio exists but before its manifest is
        // published. A later Rack archive may include an orphan, never a ref.
        if (fail) { ioFailure = true; return; }
        const std::string manifest = rack::system::join(root, "bank-" + std::to_string(live) + ".json");
        const std::string manifestTmp = manifest + ".tmp";
        f = std::fopen(manifestTmp.c_str(), "wb");
        if (!f) { ioFailure = true; return; }
        const bool manifestWritten = std::fprintf(f, "{\"revision\":%d,\"audio\":\"audio-%d.bin\"}\n", live, live) > 0;
        const bool manifestFlushed = std::fflush(f) == 0;
        const bool manifestClosed = std::fclose(f) == 0;
        if (!manifestWritten || !manifestFlushed || !manifestClosed ||
                !rack::system::rename(manifestTmp, manifest)) { ioFailure = true; return; }
        committed = live;
        ioFailure = false;
    }
    json_t* dataToJson() override {
        json_t* j = json_object();
        json_object_set_new(j, "committed", json_integer(committed));
        json_object_set_new(j, "unsaved", json_boolean(live != committed));
        json_object_set_new(j, "saveFailure", json_boolean(ioFailure));
        json_object_set_new(j, "manifest", committed ? json_string(("chimera/bank-" + std::to_string(committed) + ".json").c_str()) : json_null());
        // A same-process selection duplicate can resolve this through the
        // plugin cache, even though Rack does not copy module storage there.
        json_object_set_new(j, "cacheRef", checkpointRevision == live ?
            json_string(("checkpoint-" + std::to_string(live) + ".bin").c_str()) : json_null());
        return j;
    }
};

static json_t* readPatch(const std::string& dir) {
    const std::string path = rack::system::join(dir, "patch.json");
    json_error_t error;
    json_t* root = json_load_file(path.c_str(), 0, &error);
    require(root != nullptr, "read Rack autosave patch.json");
    json_t* modules = json_object_get(root, "modules");
    require(json_array_size(modules) == 1, "one module serialized");
    json_t* data = json_object_get(json_array_get(modules, 0), "data");
    json_t* copy = json_deep_copy(data);
    json_decref(root);
    return copy;
}

static std::string readText(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios::binary);
    require(file.good(), "referenced asset exists");
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

static void checkArchive(const std::string& archive, const std::string& extracted,
                         int64_t moduleId, int expectedRevision, bool failed) {
    rack::system::createDirectories(extracted);
    rack::system::unarchiveToDirectory(archive, extracted);
    json_t* data = readPatch(extracted);
    require(json_integer_value(json_object_get(data, "committed")) == expectedRevision,
            "archive references expected durable revision");
    require(json_is_true(json_object_get(data, "saveFailure")) == failed,
            "archive carries accurate save diagnostic");
    const char* manifestRel = json_string_value(json_object_get(data, "manifest"));
    require(manifestRel != nullptr, "archive has a manifest reference");
    const std::string moduleRoot = rack::system::join(extracted, "modules", std::to_string(moduleId));
    const std::string manifestPath = rack::system::join(moduleRoot, manifestRel);
    json_error_t error;
    json_t* manifest = json_load_file(manifestPath.c_str(), 0, &error);
    require(manifest && json_integer_value(json_object_get(manifest, "revision")) == expectedRevision,
            "referenced manifest is complete in archive");
    const char* audioRel = json_string_value(json_object_get(manifest, "audio"));
    require(audioRel != nullptr, "manifest identifies audio");
    const std::string audio = rack::system::join(rack::system::join(moduleRoot, "chimera"), audioRel);
    require(readText(audio) == "revision=" + std::to_string(expectedRevision) + "\n",
            "referenced audio matches manifest revision");
    json_decref(manifest);
    json_decref(data);
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
    // The target is an isolated directory inside this checkout's build tree.
    const std::string runRoot = rack::system::join("build", "tests", "chimera_phase0_run_" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    require(rack::system::createDirectories(runRoot), "create isolated Rack test root");
    const std::string dir = rack::system::join(runRoot, "autosave");
    patch->autosavePath = dir;
    rack::plugin::Plugin plugin;
    plugin.slug = "ChimeraPhase0Probe";
    plugin.version = "0.0.0";
    rack::plugin::Model model;
    model.plugin = &plugin;
    model.slug = "SaveProbe";
    Probe probe;
    probe.model = &model;
    probe.cacheRoot = rack::system::join(runRoot, "plugin_cache");
    engine.addModule(&probe);

    probe.live = 1;
    patch->saveAutosave();
    json_t* data = readPatch(dir);
    require(probe.saves == 0 && json_integer_value(json_object_get(data, "committed")) == 0 &&
            json_is_true(json_object_get(data, "unsaved")) &&
            json_is_null(json_object_get(data, "manifest")), "autosave does not prepare or invent audio");
    json_decref(data);

    probe.fail = true;
    const std::string firstFailedPath = rack::system::join(runRoot, "first_failed_save.vcv");
    patch->save(firstFailedPath);
    const std::string firstFailedExtract = rack::system::join(runRoot, "first_failed_extract");
    require(rack::system::createDirectories(firstFailedExtract), "create failed-save extract root");
    rack::system::unarchiveToDirectory(firstFailedPath, firstFailedExtract);
    data = readPatch(firstFailedExtract);
    require(probe.saves == 1 && json_is_null(json_object_get(data, "manifest")) &&
            json_is_true(json_object_get(data, "unsaved")) &&
            json_is_true(json_object_get(data, "saveFailure")),
            "failure without prior bundle publishes missing audio, not partial asset");
    json_decref(data);
    probe.fail = false;

    const std::string savePath = rack::system::join(runRoot, "save.vcv");
    patch->save(savePath);
    data = readPatch(dir);
    require(probe.saves == 2 && json_integer_value(json_object_get(data, "committed")) == 1 &&
            json_is_false(json_object_get(data, "unsaved")), "explicit Save prepares current revision");
    json_decref(data);
    checkArchive(savePath, rack::system::join(runRoot, "save_extract"),
                 probe.id, 1, false);

    probe.live = 2;
    patch->saveAutosave();
    data = readPatch(dir);
    require(probe.saves == 2 && json_integer_value(json_object_get(data, "committed")) == 1 &&
            json_is_true(json_object_get(data, "unsaved")) &&
            json_is_false(json_object_get(data, "saveFailure")),
            "periodic autosave retains prior durable bundle without calling onSave");
    json_decref(data);
    probe.fail = true;
    const std::string saveAsPath = rack::system::join(runRoot, "save_as.vcv");
    patch->save(saveAsPath);
    data = readPatch(dir);
    require(probe.saves == 3 && json_integer_value(json_object_get(data, "committed")) == 1 &&
            json_is_true(json_object_get(data, "unsaved")) &&
            json_is_true(json_object_get(data, "saveFailure")),
            "failed module save retains old cut and serializes diagnostic");
    json_decref(data);
    checkArchive(saveAsPath, rack::system::join(runRoot, "save_as_extract"),
                 probe.id, 1, true);

    engine.bypassModule(&probe, true);
    probe.fail = false;
    patch->save(savePath);
    require(probe.saves == 4 && probe.committed == 2, "bypassed module still receives save");
    probe.live = 3;
    engine.prepareSaveModule(&probe);
    json_t* single = engine.moduleToJson(&probe);
    json_t* singleData = json_object_get(single, "data");
    require(probe.saves == 5 && json_integer_value(json_object_get(singleData, "committed")) == 3,
            "single duplicate prepares and serializes current audio");
    const std::string sourceStorage = probe.getPatchStorageDirectory();
    const std::string cloneStorage = rack::system::join(dir, "modules", std::to_string(probe.id + 1));
    require(rack::system::copy(sourceStorage, cloneStorage), "single duplicate copies module storage");
    const std::string cloneManifest = rack::system::join(cloneStorage, "chimera", "bank-3.json");
    require(readText(cloneManifest).find("\"revision\":3") != std::string::npos,
            "single duplicate receives prepared manifest");
    json_decref(single);

    probe.live = 4;
    probe.publishCheckpoint();
    json_t* selection = engine.moduleToJson(&probe);
    json_t* selectionData = json_object_get(selection, "data");
    require(probe.saves == 5 && json_integer_value(json_object_get(selectionData, "committed")) == 3 &&
            json_is_true(json_object_get(selectionData, "unsaved")),
            "selection serialization does not prepare module assets");
    const char* cacheRef = json_string_value(json_object_get(selectionData, "cacheRef"));
    require(cacheRef != nullptr, "selection JSON includes same-process checkpoint reference");
    const std::string cacheSource = rack::system::join(probe.cacheRoot, cacheRef);
    const std::string selectionClone = rack::system::join(runRoot, "selection_clone_audio.bin");
    require(rack::system::copy(cacheSource, selectionClone),
            "selection clone can materialize checkpoint into independent storage");
    require(readText(selectionClone) == "revision=4\n", "selection clone has unsaved source audio");
    {
        std::ofstream edit(selectionClone.c_str(), std::ios::binary | std::ios::trunc);
        edit << "clone-edited\n";
    }
    require(readText(cacheSource) == "revision=4\n", "selection clone edit does not mutate source checkpoint");
    require(!rack::system::exists(rack::system::join(runRoot, "other_machine_cache", cacheRef)),
            "bare selection JSON cannot promise cross-machine audio");
    json_decref(selection);

    delete patch;
    require(probe.saves == 6 && probe.committed == 4, "shutdown prepares module save");
    engine.removeModule(&probe);
    context.patch = nullptr;
    context.history = nullptr;
    context.engine = nullptr;
    rack::contextSet(nullptr);
    std::puts("PASS: Rack autosave, archived Save, failed Save As, bypassed/stopped save, both clone contracts, shutdown");
}
