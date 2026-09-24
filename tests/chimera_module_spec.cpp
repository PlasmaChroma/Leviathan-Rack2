#include "../src/plugin.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <new>
#include <thread>

static bool trapAllocations = false;
static std::size_t audioAllocations = 0;
void* operator new(std::size_t size) {
    if (trapAllocations) ++audioAllocations;
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
void* operator new[](std::size_t size) {
    if (trapAllocations) ++audioAllocations;
    void* p = std::malloc(size);
    if (!p) throw std::bad_alloc();
    return p;
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }

Plugin* pluginInstance = nullptr;
#include "../src/Chimera.cpp"

static void need(bool ok, const char* what) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", what); std::exit(1); }
}
int main() {
    need(modelChimera && modelChimera->slug == "Chimera", "registered model slug");
    Chimera module;
    need(module.getNumParams() == 12 && module.getNumInputs() == 13 &&
         module.getNumOutputs() == 4 && module.getNumLights() == 9,
         "Rack-facing schema counts");
    need(module.reel->capacityFrames() == chimera::kMaxReelFrames,
         "full off-audio Reel preparation");
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f/48000.f;
    module.inputs[Chimera::AUDIO_L_INPUT].channels = 1;
    module.inputs[Chimera::AUDIO_L_INPUT].setVoltage(5.f);
    trapAllocations = true;
    module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0, "initial audio adoption allocates no heap");
    module.serviceStep();
    need(module.readyForPrepare && module.awaitingHandle == 0 &&
         module.stores.chargedBytes() == 133632000ull,
         "audio accepted registry-owned handle through bounded command queue");
    need(std::fabs(module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage()-5.f) < 0.001f &&
         std::fabs(module.outputs[Chimera::AUDIO_R_OUTPUT].getVoltage()-5.f) < 0.001f,
         "L-only input monitors to both channels");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    trapAllocations = true;
    module.process(args);
    module.params[Chimera::REC_PARAM].setValue(0.f);
    for (int i = 0; i < 3; ++i) module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0, "recording callbacks allocate no heap");
    need(module.reel->validFrames() == 4, "Rack process records four exact frames");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.reel->validFrames() == 4 && module.slice.recordState() == chimera::Slice::Idle,
         "stop edge excludes current frame");
    json_t* data = module.dataToJson();
    need(data && json_is_string(json_object_get(data, "audioStatus")),
         "development JSON explicitly declares unsaved audio");
    json_object_set_new(data, "inop", json_true());
    module.dataFromJson(data);
    need(module.inopSetting.load(), "writer source option persists through JSON");
    json_decref(data);
    args.sampleRate = 96000.f;
    args.sampleTime = 1.f/96000.f;
    module.process(args);
    need(module.reel->validFrames() == 4 && module.lights[Chimera::ERROR_LIGHT].getBrightness() == 1.f,
         "unsupported host rate never writes incorrect Reel time");
    need(module.requestPreparedStore(1, 1) == chimera::IoService::Accepted,
         "worker preparation accepted off audio");
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (module.awaitingHandle == 0 && std::chrono::steady_clock::now() < deadline) {
        module.serviceStep();
        std::this_thread::yield();
    }
    need(module.awaitingHandle == 2 && module.stores.chargedBytes() == 133632000ull + 4096ull,
         "worker result charged and queued without touching audio");
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f/48000.f;
    trapAllocations = true;
    module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0, "store adoption callback allocates no heap");
    need(module.stores.chargedBytes() == 133632000ull + 4096ull,
         "retired store stays charged before off-audio acknowledgment handling");
    module.serviceStep();
    need(module.awaitingHandle == 0 && module.reel->capacityFrames() == 256,
         "audio acknowledges new handle before old Reel is released");
    const auto retirementDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (module.retiringHandle && std::chrono::steady_clock::now() < retirementDeadline) {
        module.serviceStep();
        std::this_thread::yield();
    }
    need(module.retiringHandle == 0 && module.stores.chargedBytes() == 4096ull,
         "worker retirement releases old payload credit off audio");
    std::shared_ptr<chimera::JobGeneration> removedToken;
    {
        Chimera removed;
        removed.process(args);
        removed.serviceStep();
        removedToken = removed.generation;
        need(removed.requestPreparedStore(chimera::kMaxPages, chimera::kMaxPages) ==
             chimera::IoService::Accepted,
             "worker may prepare while a second module is removed");
    }
    chimera::shutdownChimeraIoService();
    need(removedToken->closed.load() && removedToken->outstanding.load() == 0,
         "module removal leaves no pending worker credit or dangling module pointer");
    std::puts("PASS: Chimera registered Rack module schema, monitoring, recording, and rate guard");
}
