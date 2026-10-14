// Rack-linked Phase 0 probe for save preparation versus plain serialization.
// This does not exercise Rack's GUI duplication or patch archive writer.
#include <context.hpp>
#include <engine/Engine.hpp>
#include <engine/Module.hpp>
#include <cstdio>
#include <cstdlib>

namespace rack {
// Stack-owned fixture; avoid Rack's private application teardown.
Context::~Context() {}
}

static void require(bool value, const char* what) {
    if (!value) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        std::exit(1);
    }
}

struct SaveProbe : rack::engine::Module {
    int liveRevision = 0;
    int durableRevision = 0;
    int saveCalls = 0;
    int serializations = 0;

    SaveProbe() { config(0, 0, 0, 0); }
    void process(const ProcessArgs&) override {}
    void onSave(const SaveEvent&) override {
        ++saveCalls;
        durableRevision = liveRevision;
    }
    json_t* dataToJson() override {
        ++serializations;
        json_t* data = json_object();
        json_object_set_new(data, "durableRevision", json_integer(durableRevision));
        json_object_set_new(data, "unsaved", json_boolean(liveRevision != durableRevision));
        return data;
    }
};

int main() {
    rack::Context context;
    rack::contextSet(&context);
    {
        rack::engine::Engine engine;
        context.engine = &engine;
        SaveProbe probe;
        engine.addModule(&probe);

        probe.liveRevision = 1;
        json_t* periodic = probe.dataToJson();
        require(probe.saveCalls == 0 && json_is_true(json_object_get(periodic, "unsaved")),
                "plain serialization does not prepare external audio");
        require(json_integer_value(json_object_get(periodic, "durableRevision")) == 0,
                "plain serialization references only the prior durable revision");
        json_decref(periodic);

        engine.prepareSaveModule(&probe);
        json_t* duplicate = probe.dataToJson();
        require(probe.saveCalls == 1 && json_is_false(json_object_get(duplicate, "unsaved")),
                "single-module save preparation precedes serialization");
        require(json_integer_value(json_object_get(duplicate, "durableRevision")) == 1,
                "prepared serialization identifies the new durable revision");
        json_decref(duplicate);

        probe.liveRevision = 2;
        engine.prepareSave();
        require(probe.saveCalls == 2 && probe.durableRevision == 2,
                "whole-engine save preparation visits the module");
        engine.removeModule(&probe);
        context.engine = nullptr;
    }
    rack::contextSet(nullptr);
    std::puts("PASS: Rack save preparation and plain-serialization contract");
}
