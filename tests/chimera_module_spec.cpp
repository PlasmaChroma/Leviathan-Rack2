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
    need(module.reel == nullptr && module.stores.chargedBytes() == 0 && !module.service,
         "browser-preview constructor allocates no Reel or worker");
    Module::ProcessArgs args{};
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f/48000.f;
    {
        Chimera unprepared;
        unprepared.inputs[Chimera::CLOCK_INPUT].channels = 1;
        unprepared.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
        unprepared.params[Chimera::REC_PARAM].setValue(1.f);
        unprepared.process(args);
        need(unprepared.recordArm == Chimera::NoArm &&
             unprepared.recordNotReady.load() && unprepared.prepareRequested.load(),
             "early clock-connected REC requests memory but never arms a delayed start");
    }
    module.process(args);
    module.serviceStep();
    need(!module.service && !module.prepareRequested.load() &&
         module.stores.chargedBytes() == 0,
         "untouched empty module starts no worker or full Reel");
    module.inputs[Chimera::AUDIO_L_INPUT].channels = 1;
    module.inputs[Chimera::AUDIO_L_INPUT].setVoltage(5.f);
    module.params[Chimera::REC_PARAM].setValue(1.f);
    trapAllocations = true;
    module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0 && module.slice.recordState() == chimera::Slice::Idle &&
         module.recordNotReady.load() && !module.ioError.load(),
         "early REC is rejected visibly without audio allocation");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    need(module.prepareRequested.load(), "first audio connection requests lazy preparation");
    module.serviceStep();
    need(module.pendingRequestId != 0 && module.ioBusy.load(),
         "control dispatcher submits preparation off audio");
    const auto prepareDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (module.awaitingHandle == 0 && std::chrono::steady_clock::now() < prepareDeadline) {
        module.serviceStep();
        std::this_thread::yield();
    }
    need(module.awaitingHandle == 1 && module.reel == nullptr,
         "worker prepares full Reel before audio adoption");
    trapAllocations = true;
    module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0 && module.reel &&
         module.reel->capacityFrames() == chimera::kMaxReelFrames &&
         module.slice.recordState() == chimera::Slice::Idle,
         "audio adopts full worker-prepared Reel without replaying early REC");
    module.serviceStep();
    need(module.readyForPrepare && module.awaitingHandle == 0 &&
         module.stores.chargedBytes() == 133632000ull,
         "audio accepted registry-owned handle through bounded command queue");
    need(module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage() > 4.9f &&
         std::fabs(module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage() -
                   module.outputs[Chimera::AUDIO_R_OUTPUT].getVoltage()) < 0.001f,
         "L-only input monitors to both channels");
    need(module.outputs[Chimera::CV_OUTPUT].getVoltage() > 0.f,
         "Rack CV output follows the conditioned audio envelope");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    trapAllocations = true;
    module.process(args);
    need(!module.recordNotReady.load(), "successful retry clears readiness error");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    for (int i = 0; i < 3; ++i) module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0, "recording callbacks allocate no heap");
    need(module.reel->validFrames() == 4, "Rack process records four exact frames");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.reel->validFrames() == 4 && module.slice.recordState() == chimera::Slice::Idle,
         "stop edge excludes current frame");
    Chimera::Gate gate;
    need(!gate.update(2.0f) && gate.update(2.5f) && gate.update(1.8f) &&
         !gate.update(1.f), "PLAY/REC Schmitt gate holds through intermediate voltage");
    bool sawPulse = false, sawLow = false;
    for (int i = 0; i < 32; ++i) {
        module.process(args);
        const float voltage = module.outputs[Chimera::EOSG_OUTPUT].getVoltage();
        sawPulse |= voltage == 10.f;
        sawLow |= voltage == 0.f;
    }
    need(sawPulse && sawLow, "Rack EOSG emits a core-timed 0/10 V boundary pulse");
    module.inputs[Chimera::PLAY_INPUT].channels = 1;
    module.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
    module.process(args);
    need(!module.playGate.high && module.lights[Chimera::PLAY_LIGHT].getBrightness() == 0.f,
         "patched low PLAY stops the development slice");
    module.inputs[Chimera::PLAY_INPUT].setVoltage(2.5f);
    module.process(args);
    module.inputs[Chimera::PLAY_INPUT].setVoltage(1.8f);
    module.process(args);
    need(module.playGate.high && module.lights[Chimera::PLAY_LIGHT].getBrightness() == 1.f,
         "intermediate PLAY voltage stays logically high");
    module.inputs[Chimera::PLAY_INPUT].setVoltage(1.f);
    module.process(args);
    need(!module.playGate.high, "PLAY falls only at its low threshold");
    module.inputs[Chimera::PLAY_INPUT].channels = 0;
    module.process(args);
    need(module.playGate.high, "unpatched PLAY restores normal high");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    module.inputs[Chimera::REC_INPUT].channels = 1;
    module.inputs[Chimera::REC_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Current,
         "REC jack rising edge starts Current recording");
    module.inputs[Chimera::REC_INPUT].setVoltage(1.8f);
    module.process(args);
    module.inputs[Chimera::REC_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Current,
         "REC jack jitter in Schmitt band does not toggle recording");
    module.inputs[Chimera::REC_INPUT].setVoltage(1.f);
    module.process(args);
    module.inputs[Chimera::REC_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Idle,
         "new REC jack rise stops recording");
    module.inputs[Chimera::REC_INPUT].channels = 0;
    module.inputs[Chimera::CLOCK_INPUT].channels = 1;
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.recordArm == Chimera::ArmCurrent &&
         module.slice.recordState() == chimera::Slice::Idle &&
         module.lights[Chimera::REC_ARMED_LIGHT].getBrightness() == 1.f,
         "clock-connected REC arms without an early write");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.recordArm == Chimera::NoArm &&
         module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 1,
         "Clock start includes its edge frame");
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.process(args);
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.recordArm == Chimera::ArmStop &&
         module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 3,
         "clock-connected stop request keeps writing while armed");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Idle &&
         module.slice.writerPosition() == 3,
         "Clock stop excludes its edge frame");
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.process(args);
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 1,
         "coincident REC and Clock resolve start before the same-frame edge");
    module.menuCommand.store(3);
    module.process(args);
    module.inputs[Chimera::CLOCK_INPUT].channels = 0;
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    need(module.reel->addMarker(2) && module.slice.selectRegion(0),
         "prepare two committed regions for quantized writer destination");
    module.inputs[Chimera::CLOCK_INPUT].channels = 1;
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    need(module.recordArm == Chimera::ArmCurrent && module.slice.selectRegion(1),
         "selection may change while Current start is armed");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 3 && module.slice.selectRegion(0),
         "Clock start latches selection committed on its edge");
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.process(args);
    need(module.slice.writerPosition() == 2,
         "later playback selection leaves active writer in its latched region");
    module.menuCommand.store(3);
    module.process(args);
    module.inputs[Chimera::CLOCK_INPUT].channels = 0;
    const chimera::StereoFrame frozenFirst = module.reel->readActive(0);
    need(module.requestSnapshot(), "module queues an exact core snapshot cut");
    trapAllocations = true;
    module.process(args);
    trapAllocations = false;
    module.serviceStep();
    need(audioAllocations == 0 && module.snapshotReady && module.snapshotReel &&
         module.snapshotReel->snapshotMetadata().validFrames == 4,
         "audio publishes frozen Reel metadata without callback allocation");
    need(module.snapshotReaders.tryAcquire(), "second snapshot consumer shares ready cut");
    need(module.slice.startCurrent(), "writer continues after snapshot publication");
    module.inputs[Chimera::AUDIO_L_INPUT].setVoltage(-3.f);
    module.process(args);
    module.slice.stopRecord();
    need(module.reel->readActive(0).l != frozenFirst.l &&
         module.snapshotReel->readSnapshot(0).l == frozenFirst.l,
         "worker-facing frozen page survives concurrent active overwrite");
    need(module.finishSnapshotReader() && module.snapshotReady &&
         module.finishSnapshotReader() && !module.snapshotReady,
         "lease remains until its last consumer finishes");
    module.serviceStep(); // Queue core-side release.
    module.process(args);
    module.serviceStep();
    need(module.snapshotRequestId == 0 && module.reel->state() == chimera::Reel::Idle,
         "audio reclaims snapshot pages before a new cut");
    need(module.requestSnapshot(), "second snapshot can be requested after reclaim");
    std::this_thread::sleep_for(std::chrono::milliseconds(260)); // Simulate stopped Rack audio.
    for (int i = 0; i < 4 && !module.snapshotReady; ++i) module.serviceStep();
    need(module.snapshotReady && module.snapshotReel->readyForWorker(),
         "stopped-host maintenance finishes capture without audio callbacks");
    need(module.finishSnapshotReader(), "stopped snapshot reader releases lease");
    for (int i = 0; i < 4 && module.snapshotRequestId; ++i) module.serviceStep();
    need(module.snapshotRequestId == 0 && module.reel->state() == chimera::Reel::Idle,
         "stopped-host maintenance reclaims lease for the next save");
    json_t* data = module.dataToJson();
    need(data && json_is_string(json_object_get(data, "audioStatus")),
         "development JSON explicitly declares unsaved audio");
    json_object_set_new(data, "inop", json_true());
    json_object_set_new(data, "gnsm", json_integer(1));
    json_object_set_new(data, "cvop", json_integer(1));
    json_object_set_new(data, "omod", json_integer(1));
    json_object_set_new(data, "mcr1", json_real(-2.5));
    json_object_set_new(data, "mcr2", json_real(0.0));
    module.dataFromJson(data);
    need(module.inopSetting.load() && module.gnsmSetting.load() &&
         module.cvopSetting.load() && module.omodSetting.load(),
         "writer, smooth-window, ramp, and immediate options persist through JSON");
    need(module.mcrSetting[0].load() == -2.5f && module.mcrSetting[1].load() == 3.f,
         "signed chord ratio loads while invalid zero retains default");
    json_decref(data);
    module.params[Chimera::GENE_SIZE_PARAM].setValue(1.f);
    module.params[Chimera::MORPH_PARAM].setValue(1.f);
    trapAllocations = true;
    for (int i = 0; i < 100; ++i) module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0 && module.slice.onsetCount() > 0 &&
         std::isfinite(module.outputs[Chimera::CV_OUTPUT].getVoltage()),
         "Rack finite-Gene audio and CV path allocate no heap");
    module.params[Chimera::GENE_SIZE_PARAM].setValue(0.f);
    for (int i = 0; i < 1000; ++i) module.process(args); // Cross Gene hysteresis.
    trapAllocations = true;
    for (int i = 0; i < 100; ++i) module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0 &&
         std::isfinite(module.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage()),
         "Rack full-Splice high-Morph scheduler allocates no heap");
    args.sampleRate = 96000.f;
    args.sampleTime = 1.f/96000.f;
    module.process(args);
    need(module.reel->validFrames() == 4 && module.lights[Chimera::ERROR_LIGHT].getBrightness() == 1.f &&
         module.outputs[Chimera::CV_OUTPUT].getVoltage() == 0.f &&
         module.outputs[Chimera::EOSG_OUTPUT].getVoltage() == 0.f,
         "unsupported host rate never writes incorrect Reel time");
    module.params[Chimera::REC_PARAM].setValue(1.f);
    module.process(args);
    args.sampleRate = 48000.f;
    args.sampleTime = 1.f/48000.f;
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Idle,
         "REC held across unsupported-rate interval is not replayed late");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    need(module.requestPreparedStore(1, 1) == chimera::IoService::Accepted,
         "worker preparation accepted off audio");
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
    while (module.awaitingHandle == 0 && std::chrono::steady_clock::now() < deadline) {
        module.serviceStep();
        std::this_thread::yield();
    }
    need(module.awaitingHandle == 2 && module.stores.chargedBytes() == 133632000ull + 4096ull,
         "worker result charged and queued without touching audio");
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
