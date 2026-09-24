#include "plugin.hpp"
#include "ChimeraSlice.hpp"
#include "ChimeraClock.hpp"
#include "ChimeraOwnership.hpp"
#include "ChimeraService.hpp"
#include "visual/ApertureLight.hpp"
#include <ui/TextField.hpp>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <memory>

struct Chimera : Module {
    struct Gate {
        bool high = false;
        bool update(float voltage) {
            if (high) {
                if (voltage <= 1.f) high = false;
            }
            else if (voltage >= 2.5f) high = true;
            return high;
        }
    };
    enum ParamIds {
        SOS_PARAM, GENE_SIZE_PARAM, VARISPEED_PARAM, MORPH_PARAM,
        SLIDE_PARAM, ORGANIZE_PARAM, GENE_ATT_PARAM, VARISPEED_ATT_PARAM,
        SLIDE_ATT_PARAM, REC_PARAM, SPLICE_PARAM, SHIFT_PARAM, NUM_PARAMS
    };
    enum InputIds {
        AUDIO_L_INPUT, AUDIO_R_INPUT, SOS_CV_INPUT, GENE_SIZE_CV_INPUT,
        VARISPEED_CV_INPUT, MORPH_CV_INPUT, SLIDE_CV_INPUT, ORGANIZE_CV_INPUT,
        CLOCK_INPUT, PLAY_INPUT, REC_INPUT, SPLICE_INPUT, SHIFT_INPUT, NUM_INPUTS
    };
    enum OutputIds { AUDIO_L_OUTPUT, AUDIO_R_OUTPUT, CV_OUTPUT, EOSG_OUTPUT, NUM_OUTPUTS };
    enum LightIds {
        REC_LIGHT, REC_ARMED_LIGHT, PLAY_LIGHT, PENDING_LIGHT, CLOCK_LIGHT,
        PM_LIGHT, IO_BUSY_LIGHT, CLIP_LIGHT, ERROR_LIGHT, NUM_LIGHTS
    };
    static_assert(int(NUM_PARAMS) == int(chimera::NUM_PARAMS) &&
                  int(NUM_INPUTS) == int(chimera::NUM_INPUTS) &&
                  int(NUM_OUTPUTS) == int(chimera::NUM_OUTPUTS) &&
                  int(NUM_LIGHTS) == int(chimera::NUM_LIGHTS),
                  "Chimera Rack schema drift");
#define CHIMERA_ASSERT_ID(id) static_assert(int(id) == int(chimera::id), "Chimera ID drift: " #id)
    CHIMERA_ASSERT_ID(SOS_PARAM);
    CHIMERA_ASSERT_ID(GENE_SIZE_PARAM);
    CHIMERA_ASSERT_ID(VARISPEED_PARAM);
    CHIMERA_ASSERT_ID(MORPH_PARAM);
    CHIMERA_ASSERT_ID(SLIDE_PARAM);
    CHIMERA_ASSERT_ID(ORGANIZE_PARAM);
    CHIMERA_ASSERT_ID(GENE_ATT_PARAM);
    CHIMERA_ASSERT_ID(VARISPEED_ATT_PARAM);
    CHIMERA_ASSERT_ID(SLIDE_ATT_PARAM);
    CHIMERA_ASSERT_ID(REC_PARAM);
    CHIMERA_ASSERT_ID(SPLICE_PARAM);
    CHIMERA_ASSERT_ID(SHIFT_PARAM);
    CHIMERA_ASSERT_ID(AUDIO_L_INPUT);
    CHIMERA_ASSERT_ID(AUDIO_R_INPUT);
    CHIMERA_ASSERT_ID(SOS_CV_INPUT);
    CHIMERA_ASSERT_ID(GENE_SIZE_CV_INPUT);
    CHIMERA_ASSERT_ID(VARISPEED_CV_INPUT);
    CHIMERA_ASSERT_ID(MORPH_CV_INPUT);
    CHIMERA_ASSERT_ID(SLIDE_CV_INPUT);
    CHIMERA_ASSERT_ID(ORGANIZE_CV_INPUT);
    CHIMERA_ASSERT_ID(CLOCK_INPUT);
    CHIMERA_ASSERT_ID(PLAY_INPUT);
    CHIMERA_ASSERT_ID(REC_INPUT);
    CHIMERA_ASSERT_ID(SPLICE_INPUT);
    CHIMERA_ASSERT_ID(SHIFT_INPUT);
    CHIMERA_ASSERT_ID(AUDIO_L_OUTPUT);
    CHIMERA_ASSERT_ID(AUDIO_R_OUTPUT);
    CHIMERA_ASSERT_ID(CV_OUTPUT);
    CHIMERA_ASSERT_ID(EOSG_OUTPUT);
    CHIMERA_ASSERT_ID(REC_LIGHT);
    CHIMERA_ASSERT_ID(REC_ARMED_LIGHT);
    CHIMERA_ASSERT_ID(PLAY_LIGHT);
    CHIMERA_ASSERT_ID(PENDING_LIGHT);
    CHIMERA_ASSERT_ID(CLOCK_LIGHT);
    CHIMERA_ASSERT_ID(PM_LIGHT);
    CHIMERA_ASSERT_ID(IO_BUSY_LIGHT);
    CHIMERA_ASSERT_ID(CLIP_LIGHT);
    CHIMERA_ASSERT_ID(ERROR_LIGHT);
#undef CHIMERA_ASSERT_ID

    chimera::StoreRegistry stores;
    chimera::ServiceToAudio commands;
    chimera::AudioToService completions;
    chimera::CoreOwnership ownership;
    chimera::SnapshotReaders snapshotReaders;
    chimera::Reel* reel = nullptr; // Borrowed from the off-audio registry.
    chimera::Slice slice;
    chimera::ClockEstimator clockEstimator;
    std::shared_ptr<chimera::IoService> service;
    std::shared_ptr<chimera::JobGeneration> generation{new chimera::JobGeneration};
    std::uint64_t nextRequestId = 1, pendingRequestId = 0, retirementRequestId = 0;
    std::uint32_t nextHandle = 1, awaitingHandle = 0, retiringHandle = 0;
    std::unique_ptr<chimera::Reel> retiringPayload; // Control-side until worker accepts it.
    std::uint32_t audioActiveHandle = 0; // Audio callback only.
    std::uint32_t controlActiveHandle = 0; // Control dispatcher only.
    std::uint64_t snapshotRequestId = 0; // Control dispatcher only.
    std::uint64_t coreSnapshotRequestId = 0, coreReleaseRequestId = 0; // Core owner only.
    bool coreSnapshotReadySent = false; // Core owner only.
    bool snapshotReady = false; // Control dispatcher only.
    bool snapshotReleasePending = false; // Control dispatcher only.
    chimera::Reel* snapshotReel = nullptr; // Protected by snapshot lease.
    std::uint64_t audioHeartbeatNs = 0; // Audio callback only.
    std::uint32_t heartbeatDivider = 0; // Audio callback only.
    bool readyForPrepare = true; // Control dispatcher only.
    std::atomic<bool> ioBusy{false}, ioError{false}, recordNotReady{false};
    std::atomic<bool> prepareRequested{false};
    std::atomic<int> menuCommand{0};
    std::atomic<bool> inopSetting{false};
    std::atomic<bool> gnsmSetting{false}, cvopSetting{false}, omodSetting{false}, pminSetting{false};
    std::atomic<int> pmodSetting{0}, ckopSetting{0};
    std::atomic<float> mcrSetting[3]{{2.f}, {3.f}, {4.f}};
    bool lastRec = false;
    bool lastRecJack = false;
    bool lastClock = false;
    bool lastShiftButton = false;
    bool lastShiftJack = false;
    bool lastSpliceButton = false;
    bool lastSpliceJack = false;
    bool playInitialized = false, lastPlayLogical = true, transportPlay = true;
    bool stopAtPrimaryBoundary = false;
    int lastPlayMode = 0;
    enum ArmState { NoArm, ArmCurrent, ArmAppend, ArmStop };
    ArmState recordArm = NoArm;
    Gate playGate, recGate, clockGate, shiftGate, spliceGate;

    Chimera() : slice(nullptr) {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SOS_PARAM, 0.f, 1.f, 0.f, "S.O.S.");
        configParam(GENE_SIZE_PARAM, 0.f, 1.f, 0.f, "Gene Size");
        configParam(VARISPEED_PARAM, 0.f, 1.f, 5.f/6.f, "Vari-Speed");
        configParam(MORPH_PARAM, 0.f, 1.f, 1.f/6.f, "Morph");
        configParam(SLIDE_PARAM, 0.f, 1.f, 0.f, "Slide");
        configParam(ORGANIZE_PARAM, 0.f, 1.f, 0.f, "Organize");
        configParam(GENE_ATT_PARAM, -1.f, 1.f, 0.f, "Gene CV amount");
        configParam(VARISPEED_ATT_PARAM, -1.f, 1.f, 0.f, "Vari-Speed CV amount");
        configParam(SLIDE_ATT_PARAM, -1.f, 1.f, 0.f, "Slide CV amount");
        configButton(REC_PARAM, "Record");
        configButton(SPLICE_PARAM, "Splice");
        configButton(SHIFT_PARAM, "Shift");
        configInput(AUDIO_L_INPUT, "Audio left");
        configInput(AUDIO_R_INPUT, "Audio right");
        configInput(SOS_CV_INPUT, "S.O.S. CV");
        configInput(GENE_SIZE_CV_INPUT, "Gene Size CV");
        configInput(VARISPEED_CV_INPUT, "Vari-Speed CV");
        configInput(MORPH_CV_INPUT, "Morph CV");
        configInput(SLIDE_CV_INPUT, "Slide CV");
        configInput(ORGANIZE_CV_INPUT, "Organize CV");
        configInput(CLOCK_INPUT, "Clock");
        configInput(PLAY_INPUT, "Play");
        configInput(REC_INPUT, "Record gate");
        configInput(SPLICE_INPUT, "Splice gate");
        configInput(SHIFT_INPUT, "Shift gate");
        configOutput(AUDIO_L_OUTPUT, "Audio left");
        configOutput(AUDIO_R_OUTPUT, "Audio right");
        configOutput(CV_OUTPUT, "CV");
        configOutput(EOSG_OUTPUT, "EOSG");
        configLight(REC_LIGHT, "Recording");
        configLight(REC_ARMED_LIGHT, "Record armed");
        configLight(PLAY_LIGHT, "Playing");
        configLight(PENDING_LIGHT, "Splice selection pending");
        configLight(CLOCK_LIGHT, "Clock high");
        configLight(PM_LIGHT, "Phase modulation active");
        configLight(IO_BUSY_LIGHT, "Reel operation busy");
        configLight(CLIP_LIGHT, "Input overload or invalid audio source");
        configLight(ERROR_LIGHT, "Reel or recording error");
        configBypass(AUDIO_L_INPUT, AUDIO_L_OUTPUT);
        configBypass(AUDIO_R_INPUT, AUDIO_R_OUTPUT);
    }
    ~Chimera() override {
        if (service) service->cancel(generation); // No worker carries this Module pointer.
        else generation->close();
    }
    chimera::IoService::Status requestPreparedStore(std::uint32_t pages,
                                                     std::uint32_t reservePages) {
        // Only the non-RT control dispatcher calls this and serviceStep().
        if (!readyForPrepare || pendingRequestId || awaitingHandle || retiringHandle ||
            snapshotRequestId || snapshotReaders.count())
            return chimera::IoService::Busy;
        if (!service) service = chimera::chimeraIoService();
        if (!service) return chimera::IoService::Closed;
        const std::uint64_t requestId = nextRequestId++;
        const chimera::IoService::Status status =
            service->prepare(generation, requestId, pages, reservePages);
        if (status == chimera::IoService::Accepted) {
            pendingRequestId = requestId;
            ioBusy.store(true, std::memory_order_release);
        }
        else ioError.store(true, std::memory_order_release);
        return status;
    }
    bool requestSnapshot() {
        // One control dispatcher is the SPSC producer. No worker touches the
        // live page table; a ready cut keeps its store resident until release.
        if (!controlActiveHandle || pendingRequestId || awaitingHandle || retiringHandle ||
            snapshotRequestId || snapshotReaders.count()) return false;
        const std::uint64_t id = nextRequestId++;
        const chimera::AudioCommand command = {
            generation->value.load(std::memory_order_acquire), id, 0, 2,
            controlActiveHandle, nullptr};
        if (!commands.tryPush(command)) return false;
        snapshotRequestId = id;
        snapshotReel = stores.lookup(controlActiveHandle);
        return true;
    }
    bool finishSnapshotReader() {
        if (!snapshotReady || !snapshotReaders.count()) return false;
        if (!snapshotReaders.finish()) return true; // Other readers retain it.
        snapshotReady = false;
        snapshotReleasePending = true;
        return true;
    }
    void enqueueSnapshotRelease() {
        if (!snapshotReleasePending) return;
        const chimera::AudioCommand command = {
            generation->value.load(std::memory_order_acquire), snapshotRequestId,
            0, 3, controlActiveHandle, nullptr};
        if (commands.tryPushCritical(command)) snapshotReleasePending = false;
    }
    static std::uint64_t steadyNs() {
        return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    }
    void coreCommands() {
        chimera::AudioCommand command{};
        for (int i = 0; i < 4 && commands.tryPop(command); ++i) {
            if (command.moduleGeneration != generation->value.load(std::memory_order_acquire))
                continue;
            if (command.kind == 1 && command.prepared) {
                const std::uint32_t oldHandle = audioActiveHandle;
                audioActiveHandle = command.handle;
                reel = command.prepared;
                slice.setReel(command.prepared);
                recordArm = NoArm;
                const chimera::AudioCompletion ack = {command.moduleGeneration, command.requestId,
                                                      1, command.handle, oldHandle};
                completions.tryPushCritical(ack);
            }
            else if (command.kind == 2) {
                const bool accepted = reel && reel->beginSnapshot(slice.frame());
                if (accepted) {
                    coreSnapshotRequestId = command.requestId;
                    coreSnapshotReadySent = false;
                }
                const chimera::AudioCompletion ack = {command.moduleGeneration, command.requestId,
                                                      2, command.handle, accepted ? 1u : 0u};
                completions.tryPushCritical(ack);
            }
            else if (command.kind == 3) {
                const bool released = reel && reel->beginRelease();
                if (released) coreReleaseRequestId = command.requestId;
                const chimera::AudioCompletion ack = {command.moduleGeneration, command.requestId,
                                                      3, command.handle, released ? 1u : 0u};
                completions.tryPushCritical(ack);
            }
        }
    }
    void coreSnapshotProgress() {
        if (!reel) return;
        const std::uint64_t generationValue = generation->value.load(std::memory_order_acquire);
        if (coreSnapshotRequestId && !coreSnapshotReadySent && reel->readyForWorker()) {
            const chimera::AudioCompletion ready = {generationValue, coreSnapshotRequestId,
                                                    4, audioActiveHandle, 1};
            coreSnapshotReadySent = completions.tryPushCritical(ready);
        }
        if (coreReleaseRequestId && reel->state() == chimera::Reel::Idle) {
            const chimera::AudioCompletion done = {generationValue, coreReleaseRequestId,
                                                   5, audioActiveHandle, 1};
            if (completions.tryPushCritical(done)) {
                coreReleaseRequestId = 0;
                coreSnapshotRequestId = 0;
                coreSnapshotReadySent = false;
            }
        }
    }
    void maintenanceStep() {
        if (!commands.size() && !snapshotRequestId) return;
        const std::uint64_t now = steadyNs();
        if (!ownership.tryMaintenance(now)) return;
        coreCommands();
        // Metadata-only progress while Rack's audio callbacks are stopped.
        // The owner token excludes a concurrent audio callback throughout.
        if (reel) {
            while (reel->state() == chimera::Reel::Capturing ||
                   reel->state() == chimera::Reel::Reclaiming)
                reel->maintenanceTick();
        }
        coreSnapshotProgress();
        ownership.releaseMaintenance(now);
    }
    void serviceStep() {
        if (prepareRequested.exchange(false, std::memory_order_acq_rel) &&
            !ioError.load(std::memory_order_acquire) &&
            !controlActiveHandle && !pendingRequestId && !awaitingHandle &&
            requestPreparedStore(chimera::kMaxPages, chimera::kMaxPages) !=
                chimera::IoService::Accepted)
            ioError.store(true, std::memory_order_release);
        enqueueSnapshotRelease();
        maintenanceStep();
        chimera::AudioCompletion ack{};
        while (completions.tryPop(ack)) {
            if (ack.moduleGeneration != generation->value.load(std::memory_order_acquire))
                continue;
            if (ack.kind == 2 && ack.requestId == snapshotRequestId) {
                if (!ack.status) {
                    snapshotRequestId = 0;
                    snapshotReel = nullptr;
                    ioError.store(true, std::memory_order_release);
                }
                continue;
            }
            if (ack.kind == 4 && ack.requestId == snapshotRequestId) {
                snapshotReady = snapshotReaders.begin();
                if (!snapshotReady) ioError.store(true, std::memory_order_release);
                continue;
            }
            if (ack.kind == 3 && ack.requestId == snapshotRequestId) {
                if (!ack.status) ioError.store(true, std::memory_order_release);
                continue;
            }
            if (ack.kind == 5 && ack.requestId == snapshotRequestId) {
                snapshotRequestId = 0;
                snapshotReel = nullptr;
                continue;
            }
            if (ack.kind != 1) continue;
            if (ack.handle != awaitingHandle) continue;
            controlActiveHandle = ack.handle;
            stores.transition(ack.handle, chimera::StoreBudget::Active);
            if (ack.status) {
                stores.transition(ack.status, chimera::StoreBudget::Retired);
                retiringHandle = ack.status;
                retiringPayload = stores.detachRetired(retiringHandle);
            }
            awaitingHandle = 0;
            readyForPrepare = true;
            ioBusy.store(retiringHandle != 0, std::memory_order_release);
        }
        if (!service) return;
        if (retiringHandle && retiringPayload && !retirementRequestId) {
            const std::uint64_t requestId = nextRequestId++;
            const chimera::IoService::Status status =
                service->retire(generation, requestId, retiringPayload);
            if (status == chimera::IoService::Accepted) retirementRequestId = requestId;
            else if (status != chimera::IoService::Busy)
                ioError.store(true, std::memory_order_release);
        }
        chimera::IoService::Result result;
        if (!service->pollFor(generation, result)) return;
        if (result.requestId == retirementRequestId) {
            if (result.status != chimera::IoService::Retired)
                ioError.store(true, std::memory_order_release);
            stores.releaseOffAudio(retiringHandle);
            retiringHandle = 0;
            retirementRequestId = 0;
            ioBusy.store(false, std::memory_order_release);
            return;
        }
        if (!pendingRequestId) return;
        if (result.requestId != pendingRequestId || result.status != chimera::IoService::Ready ||
            !result.prepared) {
            pendingRequestId = 0;
            ioBusy.store(false, std::memory_order_release);
            ioError.store(true, std::memory_order_release);
            return;
        }
        const std::uint32_t handle = nextHandle++;
        if (!stores.accept(handle, result.prepared, chimera::StoreBudget::Prepared)) {
            pendingRequestId = 0;
            ioBusy.store(false, std::memory_order_release);
            ioError.store(true, std::memory_order_release);
            return;
        }
        const chimera::AudioCommand adopt = {generation->value.load(std::memory_order_acquire),
                                             pendingRequestId, 0, 1, handle, stores.lookup(handle)};
        pendingRequestId = 0;
        if (!commands.tryPush(adopt)) {
            stores.releaseOffAudio(handle);
            ioBusy.store(false, std::memory_order_release);
            ioError.store(true, std::memory_order_release);
            return;
        }
        awaitingHandle = handle;
    }

    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "audioStatus", json_string("unsaved-development-slice"));
        json_object_set_new(root, "inop", json_boolean(inopSetting.load(std::memory_order_acquire)));
        json_object_set_new(root, "gnsm", json_integer(gnsmSetting.load(std::memory_order_acquire) ? 1 : 0));
        json_object_set_new(root, "cvop", json_integer(cvopSetting.load(std::memory_order_acquire) ? 1 : 0));
        json_object_set_new(root, "omod", json_integer(omodSetting.load(std::memory_order_acquire) ? 1 : 0));
        json_object_set_new(root, "pmin", json_integer(pminSetting.load(std::memory_order_acquire) ? 1 : 0));
        json_object_set_new(root, "pmod", json_integer(pmodSetting.load(std::memory_order_acquire)));
        json_object_set_new(root, "ckop", json_integer(ckopSetting.load(std::memory_order_acquire)));
        for (int i = 0; i < 3; ++i) {
            const char* key = i == 0 ? "mcr1" : (i == 1 ? "mcr2" : "mcr3");
            json_object_set_new(root, key, json_real(mcrSetting[i].load(std::memory_order_acquire)));
        }
        return root;
    }
    void dataFromJson(json_t* root) override {
        json_t* inop = json_object_get(root, "inop");
        if (json_is_boolean(inop))
            inopSetting.store(json_is_true(inop), std::memory_order_release);
        json_t* gnsm = json_object_get(root, "gnsm");
        if (json_is_integer(gnsm) && (json_integer_value(gnsm) == 0 || json_integer_value(gnsm) == 1))
            gnsmSetting.store(json_integer_value(gnsm) == 1, std::memory_order_release);
        json_t* cvop = json_object_get(root, "cvop");
        if (json_is_integer(cvop) && (json_integer_value(cvop) == 0 || json_integer_value(cvop) == 1))
            cvopSetting.store(json_integer_value(cvop) == 1, std::memory_order_release);
        json_t* omod = json_object_get(root, "omod");
        if (json_is_integer(omod) && (json_integer_value(omod) == 0 || json_integer_value(omod) == 1))
            omodSetting.store(json_integer_value(omod) == 1, std::memory_order_release);
        json_t* pmin = json_object_get(root, "pmin");
        if (json_is_integer(pmin) && (json_integer_value(pmin) == 0 || json_integer_value(pmin) == 1))
            pminSetting.store(json_integer_value(pmin) == 1, std::memory_order_release);
        json_t* pmod = json_object_get(root, "pmod");
        if (json_is_integer(pmod) && json_integer_value(pmod) >= 0 && json_integer_value(pmod) <= 2)
            pmodSetting.store(static_cast<int>(json_integer_value(pmod)), std::memory_order_release);
        json_t* ckop = json_object_get(root, "ckop");
        if (json_is_integer(ckop) && json_integer_value(ckop) >= 0 && json_integer_value(ckop) <= 2)
            ckopSetting.store(static_cast<int>(json_integer_value(ckop)), std::memory_order_release);
        for (int i = 0; i < 3; ++i) {
            const char* key = i == 0 ? "mcr1" : (i == 1 ? "mcr2" : "mcr3");
            json_t* ratio = json_object_get(root, key);
            const double value = json_number_value(ratio);
            if (json_is_number(ratio) && chimera::profile1::finite(value) &&
                std::fabs(value) >= 0.0625 && std::fabs(value) <= 16.0)
                mcrSetting[i].store(static_cast<float>(value), std::memory_order_release);
        }
    }

    void process(const ProcessArgs& args) override {
        if (!ownership.tryAudio()) {
            outputs[AUDIO_L_OUTPUT].setVoltage(0.f);
            outputs[AUDIO_R_OUTPUT].setVoltage(0.f);
            outputs[CV_OUTPUT].setVoltage(0.f);
            outputs[EOSG_OUTPUT].setVoltage(0.f);
            return;
        }
        coreCommands();
        processOwned(args);
        coreSnapshotProgress();
        if (!audioHeartbeatNs || ++heartbeatDivider == 256) {
            audioHeartbeatNs = steadyNs();
            heartbeatDivider = 0;
        }
        ownership.releaseAudio(audioHeartbeatNs);
    }
    void processOwned(const ProcessArgs& args) {
        if (!reel && (inputs[AUDIO_L_INPUT].isConnected() ||
                      inputs[AUDIO_R_INPUT].isConnected()))
            prepareRequested.store(true, std::memory_order_release);
        const float l = inputs[AUDIO_L_INPUT].isConnected() ?
            chimera::profile1::audio(inputs[AUDIO_L_INPUT].getVoltage()) : 0.f;
        const float r = inputs[AUDIO_R_INPUT].isConnected() ?
            chimera::profile1::audio(inputs[AUDIO_R_INPUT].getVoltage()) : l;
        // The host-rate converter arrives in Phase 6. Do not record 48 kHz
        // Reel frames at an incorrect host rate in this development slice.
        if (args.sampleRate != 48000.f) {
            slice.stopRecord();
            recordArm = NoArm;
            menuCommand.exchange(0, std::memory_order_acq_rel);
            lastRec = params[REC_PARAM].getValue() > 0.5f;
            playInitialized = false;
            stopAtPrimaryBoundary = false;
            lastRecJack = recGate.update(inputs[REC_INPUT].isConnected() ?
                inputs[REC_INPUT].getVoltage() : 0.f);
            lastClock = clockGate.update(inputs[CLOCK_INPUT].isConnected() ?
                inputs[CLOCK_INPUT].getVoltage() : 0.f);
            lastShiftButton = params[SHIFT_PARAM].getValue() > 0.5f;
            lastShiftJack = shiftGate.update(inputs[SHIFT_INPUT].isConnected() ?
                inputs[SHIFT_INPUT].getVoltage() : 0.f);
            lastSpliceButton = params[SPLICE_PARAM].getValue() > 0.5f;
            lastSpliceJack = spliceGate.update(inputs[SPLICE_INPUT].isConnected() ?
                inputs[SPLICE_INPUT].getVoltage() : 0.f);
            if (reel) reel->maintenanceTick();
            outputs[AUDIO_L_OUTPUT].setVoltage(l);
            outputs[AUDIO_R_OUTPUT].setVoltage(r);
            outputs[CV_OUTPUT].setVoltage(0.f);
            outputs[EOSG_OUTPUT].setVoltage(0.f);
            lights[PM_LIGHT].setBrightness(0.f);
            lights[ERROR_LIGHT].setBrightness(1.f);
            return;
        }
        const bool playConnected = inputs[PLAY_INPUT].isConnected();
        const bool playLogical = !playConnected || playGate.update(inputs[PLAY_INPUT].getVoltage());
        if (!playConnected) playGate.high = true;
        const int playMode = pmodSetting.load(std::memory_order_relaxed);
        if (!playInitialized) {
            transportPlay = playLogical;
            stopAtPrimaryBoundary = false;
            playInitialized = true;
        }
        else {
            if (playMode != lastPlayMode && !playLogical) {
                if (playMode == 0) stopAtPrimaryBoundary = transportPlay;
                else {
                    stopAtPrimaryBoundary = false;
                    if (playMode == 1) transportPlay = false;
                }
            }
            if (playLogical && !lastPlayLogical) {
                transportPlay = true;
                stopAtPrimaryBoundary = false;
                slice.setPlay(true);
                slice.requestPlayRetrigger();
            }
            else if (!playLogical && lastPlayLogical) {
                if (playMode == 0) stopAtPrimaryBoundary = transportPlay;
                else if (playMode == 1) transportPlay = false;
            }
            if (stopAtPrimaryBoundary && slice.primaryBoundaryDue()) {
                transportPlay = false;
                stopAtPrimaryBoundary = false;
            }
        }
        slice.setPlay(transportPlay);
        lastPlayLogical = playLogical;
        lastPlayMode = playMode;
        slice.setInop(inopSetting.load(std::memory_order_relaxed));
        slice.setSmoothGenes(gnsmSetting.load(std::memory_order_relaxed));
        slice.setRampCv(cvopSetting.load(std::memory_order_relaxed));
        slice.setImmediateTransitions(omodSetting.load(std::memory_order_relaxed));
        slice.setPmEnabled(pminSetting.load(std::memory_order_relaxed));
        slice.setChordRatios(mcrSetting[0].load(std::memory_order_relaxed),
                             mcrSetting[1].load(std::memory_order_relaxed),
                             mcrSetting[2].load(std::memory_order_relaxed));
        const bool shiftButton = params[SHIFT_PARAM].getValue() > 0.5f;
        const bool shiftJack = shiftGate.update(inputs[SHIFT_INPUT].isConnected() ?
            inputs[SHIFT_INPUT].getVoltage() : 0.f);
        if ((!shiftButton && lastShiftButton) || (shiftJack && !lastShiftJack))
            slice.requestShift();
        lastShiftButton = shiftButton;
        lastShiftJack = shiftJack;
        const bool spliceButton = params[SPLICE_PARAM].getValue() > 0.5f;
        const bool spliceJack = spliceGate.update(inputs[SPLICE_INPUT].isConnected() ?
            inputs[SPLICE_INPUT].getVoltage() : 0.f);
        if ((!spliceButton && lastSpliceButton) || (spliceJack && !lastSpliceJack))
            slice.requestSplice();
        lastSpliceButton = spliceButton;
        lastSpliceJack = spliceJack;
        const bool rec = params[REC_PARAM].getValue() > 0.5f;
        const bool recJack = recGate.update(inputs[REC_INPUT].isConnected() ?
            inputs[REC_INPUT].getVoltage() : 0.f);
        const bool clockConnected = inputs[CLOCK_INPUT].isConnected();
        const bool clock = clockGate.update(clockConnected ?
            inputs[CLOCK_INPUT].getVoltage() : 0.f);
        const bool clockRise = clock && !lastClock;
        lastClock = clock;
        const chimera::ClockEstimator::Update clockUpdate =
            clockEstimator.step(clockConnected, clockRise, slice.frame());
        const int command = menuCommand.exchange(0, std::memory_order_acq_rel);
        if (!clockConnected) recordArm = NoArm;
        if (command == 3) {
            recordArm = NoArm;
            slice.stopRecord();
        }
        else if (command == 1 || command == 2 ||
                 (rec && !lastRec) || (recJack && !lastRecJack)) {
            if (!reel) prepareRequested.store(true, std::memory_order_release);
            const bool append = command == 2;
            if (clockConnected) {
                if (recordArm != NoArm) recordArm = NoArm;
                else if (slice.recordState() == chimera::Slice::Idle && !reel)
                    recordNotReady.store(true, std::memory_order_release);
                else recordArm = slice.recordState() == chimera::Slice::Idle ?
                    (append ? ArmAppend : ArmCurrent) : ArmStop;
            }
            else if (slice.recordState() != chimera::Slice::Idle) slice.stopRecord();
            else beginRecording(append);
        }
        // Resolve REC before this same-frame Clock edge. A coincident arm and
        // edge therefore includes the start frame and excludes the stop frame.
        if (clockRise && recordArm != NoArm) {
            if (recordArm == ArmStop) slice.stopRecord();
            else beginRecording(recordArm == ArmAppend);
            recordArm = NoArm;
        }
        lastRec = rec;
        lastRecJack = recJack;
        const int clockOption = ckopSetting.load(std::memory_order_relaxed);
        if (stopAtPrimaryBoundary && clockUpdate.acceptedEdge && clockConnected &&
            (clockOption == 1 || (clockOption == 0 && !slice.hybridStretch()))) {
            transportPlay = false;
            stopAtPrimaryBoundary = false;
            slice.setPlay(false);
        }
        slice.setClockPlayback(clockConnected, clockUpdate.acceptedEdge,
            clockEstimator.havePeriod() ? clockEstimator.periodFrames() : 0,
            clockEstimator.waiting(), clockOption);
        chimera::CoreInput in{};
        in.live = chimera::StereoFrame{l, r};
        in.pmRightVolts = inputs[AUDIO_R_INPUT].isConnected() ?
            inputs[AUDIO_R_INPUT].getVoltage() : 0.f;
        in.pmRightConnected = inputs[AUDIO_R_INPUT].isConnected();
        chimera::ControlFrame& c = in.controls;
        c.sos = params[SOS_PARAM].getValue();
        c.gene = params[GENE_SIZE_PARAM].getValue();
        c.rate = params[VARISPEED_PARAM].getValue();
        c.morph = params[MORPH_PARAM].getValue();
        c.slide = params[SLIDE_PARAM].getValue();
        c.organize = params[ORGANIZE_PARAM].getValue();
        c.geneAtt = params[GENE_ATT_PARAM].getValue();
        c.rateAtt = params[VARISPEED_ATT_PARAM].getValue();
        c.slideAtt = params[SLIDE_ATT_PARAM].getValue();
        c.sosPatched = inputs[SOS_CV_INPUT].isConnected();
        c.sosCv = inputs[SOS_CV_INPUT].getVoltage();
        c.geneCv = inputs[GENE_SIZE_CV_INPUT].getVoltage();
        c.rateCv = inputs[VARISPEED_CV_INPUT].getVoltage();
        c.morphCv = inputs[MORPH_CV_INPUT].getVoltage();
        c.slideCv = inputs[SLIDE_CV_INPUT].getVoltage();
        c.organizeCv = inputs[ORGANIZE_CV_INPUT].getVoltage();
        const chimera::Slice::Output out = slice.step(in);
        outputs[AUDIO_L_OUTPUT].setVoltage(clamp(out.audio.l * 5.f, -12.f, 12.f));
        outputs[AUDIO_R_OUTPUT].setVoltage(clamp(out.audio.r * 5.f, -12.f, 12.f));
        outputs[CV_OUTPUT].setVoltage(out.cv);
        outputs[EOSG_OUTPUT].setVoltage(out.eosg ? 10.f : 0.f);
        lights[REC_LIGHT].setBrightness(out.recording ? 1.f : 0.f);
        lights[REC_ARMED_LIGHT].setBrightness(recordArm != NoArm ? 1.f : 0.f);
        lights[CLOCK_LIGHT].setBrightness(clock ? 1.f : 0.f);
        lights[PLAY_LIGHT].setBrightness(transportPlay ? 1.f : 0.f);
        lights[PENDING_LIGHT].setBrightness(slice.requestedRegion() != slice.currentRegion() ? 1.f : 0.f);
        lights[PM_LIGHT].setBrightness(slice.pmBlend());
        lights[CLIP_LIGHT].setBrightness(slice.overloaded() ? 1.f : 0.f);
        lights[IO_BUSY_LIGHT].setBrightness(ioBusy.load(std::memory_order_acquire) ? 1.f : 0.f);
        lights[ERROR_LIGHT].setBrightness(out.full || ioError.load(std::memory_order_acquire) ||
            recordNotReady.load(std::memory_order_acquire) ? 1.f : 0.f);
    }
    void beginRecording(bool append) {
        const bool started = append ? slice.startAppend() : slice.startCurrent();
        if (started) recordNotReady.store(false, std::memory_order_release);
        else if (!reel) recordNotReady.store(true, std::memory_order_release);
        else ioError.store(true, std::memory_order_release);
    }
};

struct ChimeraRatioField : ui::TextField {
    Chimera* owner = nullptr;
    int slot = 0;
    void onSelectKey(const event::SelectKey& e) override {
        if (e.action == GLFW_PRESS &&
            (e.isKeyCommand(GLFW_KEY_ENTER) || e.isKeyCommand(GLFW_KEY_KP_ENTER))) {
            char* end = nullptr;
            const float value = std::strtof(text.c_str(), &end);
            while (end && *end == ' ') ++end;
            if (owner && end != text.c_str() && end && *end == '\0' &&
                chimera::profile1::finite(value) &&
                std::fabs(value) >= 0.0625f && std::fabs(value) <= 16.f) {
                owner->mcrSetting[slot].store(value, std::memory_order_release);
                if (ui::MenuOverlay* overlay = getAncestorOfType<ui::MenuOverlay>())
                    overlay->requestDelete();
            }
            e.consume(this);
            return;
        }
        ui::TextField::onSelectKey(e);
    }
};

struct ChimeraWidget : ModuleWidget {
    ChimeraWidget(Chimera* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Chimera.svg")));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15, 42)), module, Chimera::SOS_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(42, 42)), module, Chimera::GENE_SIZE_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(71, 42)), module, Chimera::VARISPEED_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100, 42)), module, Chimera::MORPH_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(127, 42)), module, Chimera::SLIDE_PARAM));
        addParam(createParamCentered<LEDButton>(mm2px(Vec(71, 98)), module, Chimera::REC_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25, 70)), module, Chimera::AUDIO_L_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53, 70)), module, Chimera::AUDIO_R_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(89, 70)), module, Chimera::AUDIO_L_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117, 70)), module, Chimera::AUDIO_R_OUTPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20, 98)), module, Chimera::PLAY_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45, 98)), module, Chimera::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(97, 98)), module, Chimera::REC_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45, 117)), module, Chimera::CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(97, 117)), module, Chimera::EOSG_OUTPUT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(Vec(10, 26)), module, Chimera::REC_LIGHT));
        addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(Vec(25, 26)), module, Chimera::REC_ARMED_LIGHT));
        addChild(createLightCentered<SmallAperture<GreenApertureLight>>(mm2px(Vec(40, 26)), module, Chimera::PLAY_LIGHT));
        addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(Vec(56, 26)), module, Chimera::PENDING_LIGHT));
        addChild(createLightCentered<SmallAperture<BlueApertureLight>>(mm2px(Vec(72, 26)), module, Chimera::CLOCK_LIGHT));
        addChild(createLightCentered<SmallAperture<VioletApertureLight>>(mm2px(Vec(88, 26)), module, Chimera::PM_LIGHT));
        addChild(createLightCentered<SmallAperture<WhiteApertureLight>>(mm2px(Vec(103, 26)), module, Chimera::IO_BUSY_LIGHT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(Vec(118, 26)), module, Chimera::CLIP_LIGHT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(Vec(133, 26)), module, Chimera::ERROR_LIGHT));
    }
    void step() override {
        if (Chimera* m = dynamic_cast<Chimera*>(module)) m->serviceStep();
        ModuleWidget::step();
    }
    void appendContextMenu(Menu* menu) override {
        Chimera* m = dynamic_cast<Chimera*>(module);
        if (!m) return;
        const char* memoryStatus = m->controlActiveHandle ? "Reel ready" :
            (m->ioError.load(std::memory_order_acquire) ? "Reel preparation failed" :
             (m->pendingRequestId || m->awaitingHandle ?
                "Preparing memory - press REC again when ready" :
                "Reel idle - connect audio or press REC"));
        menu->addChild(createMenuLabel(memoryStatus));
        menu->addChild(createMenuLabel("Development build: recorded audio is not saved"));
        if (!m->controlActiveHandle && m->ioError.load(std::memory_order_acquire))
            menu->addChild(createMenuItem("Retry Reel preparation", "", [m] {
                m->ioError.store(false, std::memory_order_release);
                m->prepareRequested.store(true, std::memory_order_release);
            }));
        menu->addChild(createMenuItem("Start Append", "", [m] { m->menuCommand.store(2, std::memory_order_release); }));
        menu->addChild(createMenuItem("Stop recording", "", [m] { m->menuCommand.store(3, std::memory_order_release); }));
        menu->addChild(createMenuItem("Writer: live input only", "", [m] {
            m->inopSetting.store(!m->inopSetting.load(std::memory_order_relaxed), std::memory_order_release);
        }));
        menu->addChild(createCheckMenuItem("Smooth Gene window", "",
            [m] { return m->gnsmSetting.load(std::memory_order_acquire); },
            [m] { m->gnsmSetting.store(!m->gnsmSetting.load(std::memory_order_relaxed), std::memory_order_release); }));
        menu->addChild(createCheckMenuItem("CV OUT: primary ramp", "",
            [m] { return m->cvopSetting.load(std::memory_order_acquire); },
            [m] { m->cvopSetting.store(!m->cvopSetting.load(std::memory_order_relaxed), std::memory_order_release); }));
        menu->addChild(createCheckMenuItem("Immediate transitions", "",
            [m] { return m->omodSetting.load(std::memory_order_acquire); },
            [m] { m->omodSetting.store(!m->omodSetting.load(std::memory_order_relaxed), std::memory_order_release); }));
        menu->addChild(createCheckMenuItem("Right input: phase modulation (pmin)", "",
            [m] { return m->pminSetting.load(std::memory_order_acquire); },
            [m] { m->pminSetting.store(!m->pminSetting.load(std::memory_order_relaxed), std::memory_order_release); }));
        menu->addChild(createSubmenuItem("PLAY mode (pmod)", "", [m](Menu* submenu) {
            const char* labels[3] = {"Stop at primary boundary", "Stop immediately", "Retrigger only"};
            for (int mode = 0; mode < 3; ++mode)
                submenu->addChild(createCheckMenuItem(labels[mode], "",
                    [m, mode] { return m->pmodSetting.load(std::memory_order_acquire) == mode; },
                    [m, mode] { m->pmodSetting.store(mode, std::memory_order_release); }));
        }));
        menu->addChild(createSubmenuItem("CLOCK mode (ckop)", "", [m](Menu* submenu) {
            const char* labels[3] = {"Hybrid by Morph", "Gene Shift", "Stretch"};
            for (int mode = 0; mode < 3; ++mode)
                submenu->addChild(createCheckMenuItem(labels[mode], "",
                    [m, mode] { return m->ckopSetting.load(std::memory_order_acquire) == mode; },
                    [m, mode] { m->ckopSetting.store(mode, std::memory_order_release); }));
        }));
        menu->addChild(createSubmenuItem("Chord ratios (mcr1–3)", "", [m](Menu* submenu) {
            for (int i = 0; i < 3; ++i) {
                submenu->addChild(createMenuLabel(i == 0 ? "Slot 1 ratio" :
                    (i == 1 ? "Slot 2 ratio" : "Slot 3 ratio")));
                ChimeraRatioField* field = new ChimeraRatioField;
                field->owner = m;
                field->slot = i;
                field->box.size = Vec(180.f, 24.f);
                field->setText(std::to_string(m->mcrSetting[i].load(std::memory_order_acquire)));
                submenu->addChild(field);
            }
        }));
    }
};

Model* modelChimera = createModel<Chimera, ChimeraWidget>("Chimera");
