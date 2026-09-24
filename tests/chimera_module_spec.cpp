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
bool isDragonKingDebugEnabled() { return false; }
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
    auto ratePosition = [&args](int mode, float knob) {
        chimera::Reel rateReel(4, 4);
        for (std::uint32_t frame = 0; frame < 1000; ++frame)
            need(rateReel.write(frame, chimera::StereoFrame{0.f, 0.f}, frame),
                 "prepare Vari-Speed mode fixture");
        Chimera rateModule;
        rateModule.reel = &rateReel;
        rateModule.slice.setReel(&rateReel);
        rateModule.vsopSetting.store(mode);
        rateModule.params[Chimera::VARISPEED_PARAM].setValue(knob);
        rateModule.params[Chimera::VARISPEED_ATT_PARAM].setValue(1.f);
        rateModule.inputs[Chimera::VARISPEED_CV_INPUT].channels = 1;
        rateModule.inputs[Chimera::VARISPEED_CV_INPUT].setVoltage(1.f);
        rateModule.process(args);
        return rateModule.slice.primaryPosition();
    };
    const double forwardPitch = ratePosition(1, 5.f/6.f);
    const double reversePitch = ratePosition(1, 1.f/6.f);
    const double forwardOnlyPitch = ratePosition(2, 0.75f);
    const double forwardOnlyStop = ratePosition(2, 0.f);
    need(std::fabs(forwardPitch - 2.0) < 0.001 &&
         std::fabs(reversePitch - 997.0) < 0.001 &&
         std::fabs(forwardOnlyPitch - 2.0) < 0.001 &&
         std::fabs(forwardOnlyStop) < 0.001,
         "module vsop modes drive bidirectional and forward-only 1 V/oct playback");
    {
        chimera::Reel eventReel(40, 40);
        for (std::uint32_t frame = 0; frame < 9600; ++frame)
            need(eventReel.write(frame, chimera::StereoFrame{0.f, 0.f}, frame),
                 "prepare same-frame event Reel");
        need(eventReel.addMarker(4800), "prepare second same-frame event Splice");
        Chimera eventModule;
        eventModule.reel = &eventReel;
        eventModule.slice.setReel(&eventReel);
        eventModule.pmodSetting.store(2);
        eventModule.ckopSetting.store(1);
        eventModule.params[Chimera::GENE_SIZE_PARAM].setValue(static_cast<float>(
            std::log(480.0 / 4800.0) / std::log(16.0 / 4800.0)));
        eventModule.inputs[Chimera::PLAY_INPUT].channels = 1;
        eventModule.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        eventModule.inputs[Chimera::CLOCK_INPUT].channels = 1;
        eventModule.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
        eventModule.inputs[Chimera::SHIFT_INPUT].channels = 1;
        eventModule.inputs[Chimera::SHIFT_INPUT].setVoltage(0.f);
        for (int frame = 0; frame < 480; ++frame) {
            if (frame == 479) eventModule.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
            eventModule.process(args);
        }
        need(eventModule.slice.primaryBoundaryDue(),
             "Rack callback reaches primary completion before coincident events");
        const std::uint64_t onsetBefore = eventModule.slice.onsetCount();
        eventModule.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        eventModule.inputs[Chimera::SHIFT_INPUT].setVoltage(2.5f);
        eventModule.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
        eventModule.menuCommand.store(1);
        eventModule.process(args);
        need(eventModule.slice.currentRegion() == 1 &&
             eventModule.slice.recordState() == chimera::Slice::Current &&
             eventModule.slice.writerPosition() == 4801 &&
             eventModule.slice.onsetCount() == onsetBefore + 1 &&
             eventModule.outputs[Chimera::EOSG_OUTPUT].getVoltage() == 10.f,
             "natural completion plus Shift/PLAY/REC/Clock commits once and retains EOSG");
        Chimera boundaryStop;
        boundaryStop.reel = &eventReel;
        boundaryStop.slice.setReel(&eventReel);
        boundaryStop.params[Chimera::GENE_SIZE_PARAM].setValue(static_cast<float>(
            std::log(480.0 / 4800.0) / std::log(16.0 / 4800.0)));
        boundaryStop.inputs[Chimera::PLAY_INPUT].channels = 1;
        boundaryStop.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        for (int frame = 0; frame < 480; ++frame) {
            if (frame == 479) boundaryStop.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
            boundaryStop.process(args);
        }
        need(boundaryStop.stopAtPrimaryBoundary && boundaryStop.slice.primaryBoundaryDue(),
             "pmod=0 low waits for an actual primary boundary");
        boundaryStop.process(args);
        need(!boundaryStop.transportPlay &&
             boundaryStop.outputs[Chimera::EOSG_OUTPUT].getVoltage() == 10.f,
             "pmod=0 boundary stop preserves completion pulse on its stop frame");
        boundaryStop.process(args);
        need(boundaryStop.outputs[Chimera::EOSG_OUTPUT].getVoltage() == 0.f,
             "stopped Rack callback clears EOSG after the due completion");
    }
    {
        chimera::Reel regions(4, 4);
        for (std::uint32_t frame = 0; frame < 960; ++frame)
            need(regions.write(frame, chimera::StereoFrame{0.f, 0.f}, frame),
                 "prepare Rack-facing Shift fixture");
        need(regions.addMarker(480), "prepare two Rack-facing Splices");
        Chimera selectionModule;
        selectionModule.reel = &regions;
        selectionModule.slice.setReel(&regions);
        selectionModule.process(args);
        selectionModule.params[Chimera::SHIFT_PARAM].setValue(1.f);
        selectionModule.process(args);
        need(selectionModule.slice.requestedRegion() == 0,
             "SHIFT button press waits for release");
        selectionModule.params[Chimera::SHIFT_PARAM].setValue(0.f);
        selectionModule.process(args);
        need(selectionModule.slice.requestedRegion() == 1 &&
             selectionModule.slice.currentRegion() == 0 &&
             selectionModule.lights[Chimera::PENDING_LIGHT].getBrightness() == 1.f,
             "SHIFT button release queues next Splice and lights pending state");
        selectionModule.inputs[Chimera::SHIFT_INPUT].channels = 1;
        selectionModule.inputs[Chimera::SHIFT_INPUT].setVoltage(2.5f);
        selectionModule.process(args);
        need(selectionModule.slice.requestedRegion() == 0 &&
             selectionModule.lights[Chimera::PENDING_LIGHT].getBrightness() == 0.f,
             "SHIFT jack rise wraps requested Splice without waiting for release");
        selectionModule.inputs[Chimera::SHIFT_INPUT].setVoltage(1.8f);
        selectionModule.process(args);
        need(selectionModule.slice.requestedRegion() == 0,
             "SHIFT jack Schmitt band does not repeat an event");
        selectionModule.params[Chimera::SPLICE_PARAM].setValue(1.f);
        selectionModule.process(args);
        need(regions.markerCount() == 2,
             "SPLICE button press waits for release");
        const std::uint32_t buttonAddress = static_cast<std::uint32_t>(
            std::floor(selectionModule.slice.primaryPosition()));
        selectionModule.params[Chimera::SPLICE_PARAM].setValue(0.f);
        selectionModule.process(args);
        need(regions.markerCount() == 3 && regions.region(1).begin == buttonAddress,
             "SPLICE button release captures Rack-facing primary cursor");
        selectionModule.inputs[Chimera::SPLICE_INPUT].channels = 1;
        selectionModule.inputs[Chimera::SPLICE_INPUT].setVoltage(2.5f);
        const std::uint32_t jackAddress = static_cast<std::uint32_t>(
            std::floor(selectionModule.slice.primaryPosition()));
        selectionModule.process(args);
        need(regions.markerCount() == 4 && regions.region(2).begin == jackAddress,
             "SPLICE jack rising edge captures next primary cursor");
        selectionModule.inputs[Chimera::SPLICE_INPUT].setVoltage(1.8f);
        selectionModule.process(args);
        need(regions.markerCount() == 4,
             "SPLICE jack Schmitt band does not repeat");
    }
    {
        chimera::Reel playReel(2, 2);
        for (std::uint32_t frame = 0; frame < 256; ++frame)
            need(playReel.write(frame, chimera::StereoFrame{1.f, 1.f}, frame),
                 "prepare Play mode Reel");
        Chimera initialLow;
        initialLow.reel = &playReel;
        initialLow.slice.setReel(&playReel);
        initialLow.inputs[Chimera::PLAY_INPUT].channels = 1;
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
        initialLow.process(args);
        need(!initialLow.transportPlay && !initialLow.slice.playing() &&
             initialLow.slice.onsetCount() == 0,
             "patched-low PLAY at initialization never starts playback");
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(2.5f);
        initialLow.process(args);
        need(initialLow.transportPlay && initialLow.slice.onsetCount() == 1,
             "PLAY rise starts a stopped transport");
        for (int frame = 0; frame < 15; ++frame) initialLow.process(args);
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
        initialLow.process(args);
        need(initialLow.transportPlay && initialLow.stopAtPrimaryBoundary,
             "default PLAY fall queues a primary-boundary stop");
        for (int frame = 0; frame < 240; ++frame) initialLow.process(args);
        need(!initialLow.transportPlay && !initialLow.stopAtPrimaryBoundary &&
             !initialLow.slice.playing(),
             "default PLAY mode stops at the primary boundary");
        initialLow.inputs[Chimera::PLAY_INPUT].channels = 0;
        initialLow.process(args);
        need(initialLow.transportPlay && initialLow.slice.onsetCount() >= 2,
             "unpatched PLAY normal-high creates one rise after logical low");
        const std::uint64_t beforeHighCable = initialLow.slice.onsetCount();
        initialLow.inputs[Chimera::PLAY_INPUT].channels = 1;
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        initialLow.process(args);
        need(initialLow.slice.onsetCount() == beforeHighCable,
             "inserting an already-high PLAY cable does not retrigger");
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
        initialLow.process(args);
        need(initialLow.stopAtPrimaryBoundary && initialLow.transportPlay,
             "second default PLAY fall waits for its boundary");
        initialLow.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        const std::uint64_t beforeCancelRise = initialLow.slice.onsetCount();
        initialLow.process(args);
        need(!initialLow.stopAtPrimaryBoundary && initialLow.transportPlay &&
             initialLow.slice.onsetCount() == beforeCancelRise + 1,
             "PLAY rise cancels pending boundary stop and retriggers once");

        Chimera clockStop;
        clockStop.reel = &playReel;
        clockStop.slice.setReel(&playReel);
        clockStop.ckopSetting.store(1);
        clockStop.inputs[Chimera::CLOCK_INPUT].channels = 1;
        clockStop.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
        clockStop.inputs[Chimera::PLAY_INPUT].channels = 1;
        clockStop.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        clockStop.process(args);
        clockStop.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
        clockStop.process(args);
        need(clockStop.stopAtPrimaryBoundary && clockStop.transportPlay,
             "PLAY fall queues stop before explicit Gene Shift edge");
        const std::uint64_t beforeClockStop = clockStop.slice.onsetCount();
        clockStop.inputs[Chimera::CLOCK_INPUT].setVoltage(5.f);
        clockStop.process(args);
        need(!clockStop.transportPlay && !clockStop.stopAtPrimaryBoundary &&
             clockStop.slice.onsetCount() == beforeClockStop,
             "Gene Shift Clock edge stops pending PLAY without a new onset");

        Chimera retriggerOnly;
        retriggerOnly.reel = &playReel;
        retriggerOnly.slice.setReel(&playReel);
        retriggerOnly.pmodSetting.store(2);
        retriggerOnly.process(args);
        const std::uint64_t beforeLow = retriggerOnly.slice.onsetCount();
        retriggerOnly.inputs[Chimera::PLAY_INPUT].channels = 1;
        retriggerOnly.inputs[Chimera::PLAY_INPUT].setVoltage(0.f);
        retriggerOnly.process(args);
        need(retriggerOnly.transportPlay && retriggerOnly.slice.onsetCount() == beforeLow,
             "retrigger-only PLAY low leaves running transport active");
        retriggerOnly.inputs[Chimera::PLAY_INPUT].setVoltage(5.f);
        retriggerOnly.process(args);
        need(retriggerOnly.transportPlay &&
             retriggerOnly.slice.onsetCount() == beforeLow + 1,
             "retrigger-only PLAY rise forces exactly one onset");
    }
    {
        chimera::Reel pmRegion(4, 4);
        for (std::uint32_t frame = 0; frame < 1000; ++frame)
            need(pmRegion.write(frame, chimera::StereoFrame{
                frame < 500 ? 1.f : -1.f, frame < 500 ? 1.f : -1.f}, frame),
                "prepare Rack-facing PM fixture");
        Chimera pmModule;
        pmModule.reel = &pmRegion;
        pmModule.slice.setReel(&pmRegion);
        pmModule.slice.setConditioning(false);
        pmModule.pminSetting.store(true);
        pmModule.params[Chimera::SOS_PARAM].setValue(1.f);
        pmModule.params[Chimera::VARISPEED_PARAM].setValue(0.5f);
        pmModule.params[Chimera::SLIDE_PARAM].setValue(0.25f);
        pmModule.inputs[Chimera::AUDIO_L_INPUT].channels = 1;
        pmModule.inputs[Chimera::AUDIO_L_INPUT].setVoltage(0.f);
        pmModule.inputs[Chimera::AUDIO_R_INPUT].channels = 1;
        pmModule.inputs[Chimera::AUDIO_R_INPUT].setVoltage(5.f);
        for (int frame = 0; frame < 144240; ++frame) pmModule.process(args);
        need(pmModule.slice.pmActive() &&
             pmModule.lights[Chimera::PM_LIGHT].getBrightness() > 0.99f &&
             pmModule.outputs[Chimera::AUDIO_L_OUTPUT].getVoltage() < -4.5f,
             "quiet connected left input permits PM after dwell and sounds at Stop");
        pmModule.inputs[Chimera::AUDIO_L_INPUT].setVoltage(5.f);
        for (int frame = 0; frame < 17; ++frame) pmModule.process(args);
        need(!pmModule.slice.pmActive(),
             "Rack left signal exits PM after presence threshold");
        json_t* pmJson = pmModule.dataToJson();
        need(json_integer_value(json_object_get(pmJson, "pmin")) == 1,
             "PM enable persists in module state");
        json_decref(pmJson);
    }
    {
        Chimera unprepared;
        unprepared.inputs[Chimera::CLOCK_INPUT].channels = 1;
        unprepared.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
        unprepared.menuCommand.store(1);
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
    module.menuCommand.store(1);
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
    module.menuCommand.store(1);
    trapAllocations = true;
    module.process(args);
    need(!module.recordNotReady.load(), "successful retry clears readiness error");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    for (int i = 0; i < 3; ++i) module.process(args);
    trapAllocations = false;
    need(audioAllocations == 0, "recording callbacks allocate no heap");
    need(module.reel->validFrames() == 4, "Rack process records four exact frames");
    module.menuCommand.store(1);
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
    module.pmodSetting.store(1);
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
    module.menuCommand.store(1);
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
         module.slice.writerPosition() == 1 &&
         module.clockEstimator.haveEdge() && !module.clockEstimator.havePeriod(),
         "Clock start includes its edge frame");
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.process(args);
    module.menuCommand.store(1);
    module.process(args);
    need(module.recordArm == Chimera::ArmStop &&
         module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 3,
         "clock-connected stop request keeps writing while armed");
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.recordState() == chimera::Slice::Idle &&
         module.slice.writerPosition() == 3 &&
         module.clockEstimator.havePeriod() && module.clockEstimator.periodFrames() == 3,
         "Clock stop excludes its edge frame");
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.process(args);
    module.menuCommand.store(1);
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
    need(!module.clockEstimator.connected() && !module.clockEstimator.havePeriod(),
         "Clock disconnect resets playback estimator");
    need(module.reel->addMarker(2) && module.slice.selectRegion(0),
         "prepare two committed regions for quantized writer destination");
    module.inputs[Chimera::CLOCK_INPUT].channels = 1;
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.menuCommand.store(1);
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
    need(module.slice.selectRegion(0), "prepare selection for coincident Shift and Clock");
    module.ckopSetting.store(1);
    module.inputs[Chimera::SHIFT_INPUT].channels = 1;
    module.inputs[Chimera::SHIFT_INPUT].setVoltage(2.5f);
    module.menuCommand.store(1);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.process(args);
    need(module.slice.currentRegion() == 1 &&
         module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 3,
         "coincident Shift, REC, and Clock latch the newly selected Splice");
    module.menuCommand.store(3);
    module.process(args);
    module.inputs[Chimera::SHIFT_INPUT].setVoltage(0.f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(0.f);
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.process(args);
    need(module.slice.selectRegion(0), "prepare Organize and Shift priority fixture");
    module.params[Chimera::ORGANIZE_PARAM].setValue(1.f);
    module.inputs[Chimera::SHIFT_INPUT].setVoltage(2.5f);
    module.inputs[Chimera::CLOCK_INPUT].setVoltage(2.5f);
    module.menuCommand.store(1);
    module.process(args);
    need(module.slice.organizeBin() == 1 && module.slice.currentRegion() == 0 &&
         module.slice.recordState() == chimera::Slice::Current &&
         module.slice.writerPosition() == 1,
         "same-frame Organize selects first, then Shift wraps before Clock starts Current");
    module.menuCommand.store(3);
    module.process(args);
    module.params[Chimera::ORGANIZE_PARAM].setValue(0.f);
    module.inputs[Chimera::SHIFT_INPUT].channels = 0;
    module.params[Chimera::REC_PARAM].setValue(0.f);
    module.inputs[Chimera::CLOCK_INPUT].channels = 0;
    need(module.slice.selectRegion(0), "restore first region for snapshot overwrite fixture");
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
    json_object_set_new(data, "pmod", json_integer(2));
    json_object_set_new(data, "ckop", json_integer(1));
    json_object_set_new(data, "vsop", json_integer(2));
    json_object_set_new(data, "mcr1", json_real(-2.5));
    json_object_set_new(data, "mcr2", json_real(0.0));
    module.dataFromJson(data);
    need(module.inopSetting.load() && module.gnsmSetting.load() &&
         module.cvopSetting.load() && module.omodSetting.load(),
         "writer, smooth-window, ramp, and immediate options persist through JSON");
    json_t* persistedPlay = module.dataToJson();
    need(module.pmodSetting.load() == 2 &&
         json_integer_value(json_object_get(persistedPlay, "pmod")) == 2 &&
         module.ckopSetting.load() == 1 &&
         json_integer_value(json_object_get(persistedPlay, "ckop")) == 1 &&
         module.vsopSetting.load() == 2 &&
         json_integer_value(json_object_get(persistedPlay, "vsop")) == 2,
         "PLAY, CLOCK, and Vari-Speed modes persist through JSON");
    json_decref(persistedPlay);
    json_object_set_new(data, "pmod", json_integer(3));
    json_object_set_new(data, "ckop", json_integer(-1));
    json_object_set_new(data, "vsop", json_integer(3));
    module.dataFromJson(data);
    need(module.pmodSetting.load() == 2 && module.ckopSetting.load() == 1 &&
         module.vsopSetting.load() == 2,
         "invalid PLAY, CLOCK, and Vari-Speed modes are ignored");
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
    module.menuCommand.store(1);
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
    chimera::Reel optionReel(1, 1);
    for (std::uint32_t i = 0; i < 4; ++i)
        need(optionReel.write(i, chimera::StereoFrame{0.f, 0.f}, i),
             "prepare REC assignment fixture");
    {
        Chimera optionModule;
        optionModule.reel = &optionReel;
        optionModule.slice.setReel(&optionReel);
        optionModule.rsopSetting.store(1);
        optionModule.params[Chimera::REC_PARAM].setValue(1.f);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Idle,
             "REC button press waits for release");
        optionModule.params[Chimera::REC_PARAM].setValue(0.f);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Append &&
             optionModule.slice.writerPosition() == 5,
             "REC button release uses rsop=1 Append assignment");
        optionModule.menuCommand.store(3);
        optionModule.process(args);
        optionModule.menuCommand.store(4);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Current,
             "rsop=1 assigns alternate REC start to Current");
        optionModule.menuCommand.store(3);
        optionModule.process(args);
        optionModule.rsopSetting.store(0);
        optionModule.menuCommand.store(4);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Append,
             "rsop=0 assigns alternate REC start to Append");
        json_t* options = optionModule.dataToJson();
        need(json_integer_value(json_object_get(options, "rsop")) == 0 &&
             json_integer_value(json_object_get(options, "inputGain")) == 1,
             "REC assignment and gain persist in patch state");
        json_decref(options);
        json_t* edits = json_object();
        json_object_set_new(edits, "rsop", json_integer(1));
        json_object_set_new(edits, "inputGain", json_integer(3));
        optionModule.dataFromJson(edits);
        need(optionModule.rsopSetting.load() == 1 && optionModule.inputGainSetting.load() == 3,
             "valid REC assignment and gain reload");
        json_object_set_new(edits, "rsop", json_integer(2));
        json_object_set_new(edits, "inputGain", json_integer(-1));
        optionModule.dataFromJson(edits);
        need(optionModule.rsopSetting.load() == 1 && optionModule.inputGainSetting.load() == 3,
             "invalid REC assignment and gain retain previous values");
        json_decref(edits);
        optionModule.menuCommand.store(3);
        optionModule.process(args);
        optionModule.inputs[Chimera::REC_INPUT].channels = 1;
        optionModule.inputs[Chimera::REC_INPUT].setVoltage(0.f);
        optionModule.params[Chimera::SPLICE_PARAM].setValue(1.f);
        optionModule.process(args);
        optionModule.inputs[Chimera::REC_INPUT].setVoltage(2.5f);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Append,
             "REC gate remains independent of a held SPLICE button");
        optionModule.menuCommand.store(3);
        optionModule.process(args);
        optionModule.params[Chimera::SPLICE_PARAM].setValue(0.f);
        optionModule.process(args);
        optionModule.inputs[Chimera::REC_INPUT].channels = 0;
        args.sampleRate = 96000.f;
        args.sampleTime = 1.f/96000.f;
        optionModule.params[Chimera::REC_PARAM].setValue(1.f);
        optionModule.process(args);
        args.sampleRate = 48000.f;
        args.sampleTime = 1.f/48000.f;
        optionModule.process(args);
        optionModule.params[Chimera::REC_PARAM].setValue(0.f);
        optionModule.process(args);
        need(optionModule.slice.recordState() == chimera::Slice::Idle,
             "REC held through unsupported host rate does not fire on release");
    }
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
