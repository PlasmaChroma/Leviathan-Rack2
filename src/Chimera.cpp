#include "plugin.hpp"
#include "ChimeraSlice.hpp"
#include "ChimeraClock.hpp"
#include "ChimeraRateBridge.hpp"
#include "ChimeraOptionsText.hpp"
#include "ChimeraOwnership.hpp"
#include "ChimeraService.hpp"
#include "ChimeraBundle.hpp"
#include "ChimeraRecovery.hpp"
#include "ChimeraEdit.hpp"
#include "ChimeraCheckpointSession.hpp"
#include "ChimeraWaveform.hpp"
#include "DebugTerminalMetrics.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/VisualAssets.hpp"
#include <ui/TextField.hpp>
#include <osdialog.h>
#include <system.hpp>
#include <patch.hpp>
#include <atomic>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <mutex>
#include <set>
#include <thread>
#include <ctime>

namespace { std::atomic<std::uint32_t> gChimeraDebugInstanceCounter{1u}; }

struct ChimeraRateQuantity : ParamQuantity {
    std::atomic<int>* rateMode = nullptr;

    double speedAt(float knob) const {
        const int mode = rateMode ? rateMode->load(std::memory_order_relaxed) : 0;
        return mode == 2 ? chimera::profile1::forwardBaseRate(knob)
                         : chimera::profile1::classicRate(knob);
    }

    float getDisplayValue() override { return static_cast<float>(speedAt(getValue())); }

    void setDisplayValue(float speed) override {
        const bool forwardOnly = rateMode && rateMode->load(std::memory_order_relaxed) == 2;
        if (!std::isfinite(speed)) return;
        const float target = clamp(speed, forwardOnly ? 0.f : -2.f, 2.f);
        float low = 0.f, high = 1.f;
        for (int i = 0; i < 24; ++i) {
            const float mid = 0.5f * (low + high);
            if (speedAt(mid) < target) low = mid;
            else high = mid;
        }
        setImmediateValue(0.5f * (low + high));
    }

    std::string getDisplayValueString() override {
        return string::f("%+.2f", getDisplayValue());
    }
};

struct ChimeraBipolarHaloKnob : LeviathanHaloKnob2 {
    ChimeraBipolarHaloKnob() : LeviathanHaloKnob2(bipolarConfig()) {}
    static Config bipolarConfig() {
        Config config;
        config.bipolar = true;
        return config;
    }
};

struct Chimera : Module {
    static constexpr std::uint64_t kAutomaticSnapshotId = UINT64_MAX;
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

    // Control callers (UI, save, and the background pump) share one serialized
    // owner. Audio never takes this mutex; CoreOwnership still fences stopped-
    // engine maintenance. Recursive entry allows synchronous save/edit pumping.
    std::recursive_mutex controlMutex;
    std::thread controlThread;
    std::atomic<bool> controlStop{false};
    std::string cachedRecoveryRoot;
    std::string checkpointSessionRoot;
    std::uint64_t nextCheckpointSweepNs = 0;
    void startControlDispatcher() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (controlThread.joinable()) return;
        cachedRecoveryRoot = recoveryRoot(); // Capture host context on its caller.
        checkpointSessionRoot = rack::system::join(leviathanPluginUserRootPath(), "Chimera/checkpoints-v1");
        controlStop.store(false, std::memory_order_release);
        controlThread = std::thread([this] {
            while (!controlStop.load(std::memory_order_acquire)) {
                {
                    std::unique_lock<std::recursive_mutex> lock(controlMutex, std::try_to_lock);
                    if (lock.owns_lock()) {
                        try { serviceStep(false); }
                        catch (...) {
                            ioError.store(true, std::memory_order_release);
                            setIoMessage("Background Reel service failed");
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    void stopControlDispatcher() {
        controlStop.store(true, std::memory_order_release);
        if (controlThread.joinable()) controlThread.join();
    }
    chimera::StoreRegistry stores;
    debug_terminal::BaselineModuleMetrics debugMetrics;
    chimera::ServiceToAudio commands;
    chimera::AudioToService completions;
    struct MarkerCommand {
        std::uint64_t id, revision;
        unsigned index, frame, kind; // 0 move, 1 remove, 2 undo, 3 redo.
    };
    chimera::SpscRing<MarkerCommand, 8> markerCommands;
    std::uint64_t markerRequestId = 0; // Control dispatcher only.
    std::atomic<unsigned> publishedMarkerHistory{0};
    chimera::CoreOwnership ownership;
    chimera::SnapshotReaders snapshotReaders;
    chimera::Reel* reel = nullptr; // Borrowed from the off-audio registry.
    chimera::Slice slice;
    chimera::ClockEstimator clockEstimator;
    std::shared_ptr<chimera::IoService> service;
    std::shared_ptr<chimera::JobGeneration> generation{new chimera::JobGeneration};
    std::uint64_t nextRequestId = 1, pendingRequestId = 0, retirementRequestId = 0;
    std::uint32_t nextHandle = 1, awaitingHandle = 0, retiringHandle = 0;
    std::uint64_t awaitingDocumentRevision = 0, awaitingAudioRevision = 0;
    std::unique_ptr<chimera::Reel> retiringPayload; // Control-side until worker accepts it.
    std::uint32_t audioActiveHandle = 0; // Audio callback only.
    std::uint32_t controlActiveHandle = 0; // Control dispatcher only.
    std::uint64_t snapshotRequestId = 0; // Control dispatcher only.
    std::uint64_t coreSnapshotRequestId = 0, coreReleaseRequestId = 0; // Core owner only.
    bool coreSnapshotReadySent = false; // Core owner only.
    bool automaticSnapshotAnnounced = false; // Core owner only.
    bool snapshotReady = false; // Control dispatcher only.
    bool snapshotReleasePending = false; // Control dispatcher only.
    bool snapshotAbandoned = false; // Release a timed-out request once capture acknowledges.
    std::atomic<bool> snapshotClaimed{false}; // Control and audio coordinate starts.
    chimera::Reel* snapshotReel = nullptr; // Protected by snapshot lease.
    std::uint64_t audioHeartbeatNs = 0; // Audio callback only.
    std::uint32_t heartbeatDivider = 0; // Audio callback only.
    bool readyForPrepare = true; // Control dispatcher only.
    std::atomic<bool> ioBusy{false}, ioError{false}, recordNotReady{false};
    std::atomic<bool> bridgeError{false};
    std::atomic<bool> prepareRequested{false};
    std::atomic<int> menuCommand{0};
    std::atomic<unsigned> selectionMenuCommands{0}; // 1 next Splice, 2 add marker.
    std::atomic<bool> bandlimitedPlayback{false};
    std::atomic<bool> inopSetting{false};
    std::atomic<bool> gnsmSetting{false}, cvopSetting{false}, omodSetting{false}, pminSetting{false};
    std::atomic<int> pmodSetting{0}, ckopSetting{0}, vsopSetting{0};
    std::atomic<int> rsopSetting{0}, inputGainSetting{1};
    std::atomic<float> mcrSetting[3]{{2.f}, {3.f}, {4.f}};
    // 0 idle, 2 control-side writing, 1 ready, 3 audio-side adopting.
    std::atomic<int> optionsTextStageState{0};
    chimera::optionsText::Values stagedOptionsText;
    std::map<std::string, std::string> optionsTextExtras;
    std::mutex optionsTextExtrasMutex; // Control/save side only, never process().
    struct LoadTicket {
        std::atomic<bool> done{false};
        std::shared_ptr<chimera::Reel> teardownReel; // Assigned only during module teardown.
        chimera::bundle::LoadResult result;
        std::shared_ptr<const chimera::WaveformSummary> waveform;
        unsigned adoptionKind = 1; // 1 patch load, 4 external import, 5 fenced edit.
        bool usesSnapshot = false;
        std::uint64_t expectedDocumentRevision = 0, expectedAudioRevision = 0;
        std::string warning;
    };
    std::shared_ptr<LoadTicket> loadTicket; // Control dispatcher only.
    std::shared_ptr<const chimera::WaveformSummary> waveform;
    std::shared_ptr<const chimera::WaveformSummary> awaitingWaveform;
    std::mutex persistenceMutex;
    std::mutex ioMessageMutex;
    std::string lastIoMessage;
    std::string lastIoWarning;
    std::string committedManifest;
    std::uint64_t committedDocumentRevision = 0, committedAudioRevision = 0;
    std::atomic<std::uint64_t> savedDocumentRevision{0};
    std::atomic<std::uint64_t> savedAudioRevision{0};
    std::atomic<bool> hasSavedReel{false};
    std::atomic<std::uint64_t> publishedDocumentRevision{0}, publishedAudioRevision{0};
    std::atomic<bool> saveFailure{false};
    std::atomic<bool> unsavedImport{false}, recordingOrArmed{false};
    std::atomic<bool> recordingActive{false}, recoveryPostPending{false};
    std::atomic<std::uint64_t> displayHeartbeatNs{0}; // UI widget step; no Reel access.
    std::atomic<std::uint64_t> recordStartNs{0};
    struct SaveTicket {
        std::atomic<bool> done{false};
        std::shared_ptr<chimera::Reel> teardownReel;
        chimera::CheckpointSession staging;
        chimera::bundle::CommitResult result;
        std::string id;
        bool abandoned = false; // Control owner only.
#ifdef CHIMERA_MANUAL_CONTROL_TEST
        std::function<void()> beforeEncode;
#endif
        ~SaveTicket() {
            try {
                if (staging.directory().empty() || id.empty()) return;
                const std::string base = staging.directory() + "/reel-" + id;
                for (const char* suffix : {".wav", ".wav.tmp", ".json", ".json.tmp"})
                    rack::system::remove(base + suffix);
            }
            catch (...) {} // Failed cleanup remains eligible for the session sweep.
        }
    };
    std::shared_ptr<SaveTicket> pendingSave; // Holds its snapshot until the worker finishes.
#ifdef CHIMERA_MANUAL_CONTROL_TEST
    std::uint64_t controlWaitNs = UINT64_C(2000000000);
    std::function<void()> saveEncodingHookForTest;
#else
    static constexpr std::uint64_t controlWaitNs = UINT64_C(2000000000);
#endif
    std::atomic<bool> saveInProgress{false};
    std::uint64_t lastRecoveryNs = 0; // Control dispatcher only.
    int recoveryPurpose = 0; // 0 none, 1 pre-record, 2 latest, 3 display refresh.
    std::uint64_t lastDisplayAttemptNs = 0;
    struct RecoveryTicket {
        std::atomic<bool> done{false};
        std::shared_ptr<chimera::Reel> teardownReel; // Worker captures this ticket.
        chimera::recovery::CommitResult result;
        std::shared_ptr<const chimera::WaveformSummary> waveform;
        bool displayOnly = false;
    };
    std::shared_ptr<RecoveryTicket> recoveryTicket;
    std::atomic<std::uint32_t> publishedValidFrames{0};
    std::atomic<std::uint16_t> publishedRegion{0};
    std::atomic<std::uint16_t> publishedRequestedRegion{0}, publishedMarkerCount{0};
    std::atomic<std::uint32_t> publishedPlayFrame{0}, publishedRecordFrame{0};
    std::atomic<std::uint32_t> publishedRecordStartFrame{0};
    std::atomic<int> publishedRecordState{0}; // Idle, armed Current/Append/Stop, recording Current/Append.
    unsigned awaitingAdoptionKind = 1; // Control dispatcher only.
    void setIoMessage(const std::string& message) {
        std::lock_guard<std::mutex> lock(ioMessageMutex);
        lastIoMessage = message;
    }
    void setIoWarning(const std::string& warning) {
        std::lock_guard<std::mutex> lock(ioMessageMutex);
        lastIoWarning = warning;
    }
    bool lastRecJack = false;
    bool lastRecButton = false;
    bool lastClock = false;
    bool lastShiftJack = false;
    bool lastShiftButton = false;
    bool lastSpliceJack = false;
    bool lastSpliceButton = false;
    bool ignoreRecRelease = false, ignoreShiftRelease = false, ignoreSpliceRelease = false;
    bool playInitialized = false, lastPlayLogical = true, transportPlay = true;
    bool stopAtPrimaryBoundary = false;
    int lastPlayMode = 0;
    enum ArmState { NoArm, ArmCurrent, ArmAppend, ArmStop };
    ArmState recordArm = NoArm;
    Gate playGate, recGate, clockGate, shiftGate, spliceGate;
    // The widget/control dispatcher prepares and reclaims bridges. The audio
    // callback only swaps raw pointers after a matching rate has been published.
    std::atomic<unsigned> requestedHostRate{48000}, activeHostRate{48000};
    bool rateServiceRegistered = false; // Control dispatcher only.
    std::atomic<int> bridgeInputLatencyHost{0}, bridgeOutputLatencyHost{0};
    std::atomic<chimera::RateBridge*> preparedBridge{nullptr}, retiredBridge{nullptr};
    chimera::RateBridge* activeBridge = nullptr; // Audio owner only.
    bool bypassActive = false;
    bool bridgeResumePending = false;
    unsigned unbypassFadeRemaining = 0;

    Chimera() : slice(nullptr) {
        debugMetrics.assignInstanceId(gChimeraDebugInstanceCounter);
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SOS_PARAM, 0.f, 1.f, 0.f, "S.O.S.");
        configParam(GENE_SIZE_PARAM, 0.f, 1.f, 0.f, "Gene Size");
        auto* rateQuantity = configParam<ChimeraRateQuantity>(VARISPEED_PARAM, 0.f, 1.f,
            5.f/6.f, "Vari-Speed", "×");
        rateQuantity->rateMode = &vsopSetting;
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
        stopControlDispatcher();
        // Audio and control have stopped. Transfer the leased source to the
        // tickets captured by workers instead of waiting for disk I/O here.
        // Workers never access teardownReel itself; their ticket lifetime keeps
        // the raw snapshot pointer valid even after this module is gone.
        if ((loadTicket && loadTicket->usesSnapshot) || recoveryTicket || pendingSave) {
            stores.transition(controlActiveHandle, chimera::StoreBudget::Retired);
            std::shared_ptr<chimera::Reel> retained(stores.detachRetired(controlActiveHandle));
            if (loadTicket && loadTicket->usesSnapshot) {
                loadTicket->teardownReel = retained;
            }
            if (recoveryTicket) recoveryTicket->teardownReel = retained;
            if (pendingSave) pendingSave->teardownReel = retained;
        }
        if (rateServiceRegistered) chimera::unregisterRateBridge(&preparedBridge);
        if (service) service->cancel(generation); // No worker carries this Module pointer.
        else generation->close();
        loadTicket.reset();
        delete activeBridge;
        delete preparedBridge.exchange(nullptr, std::memory_order_acq_rel);
        delete retiredBridge.exchange(nullptr, std::memory_order_acq_rel);
    }
    void onSampleRateChange(const SampleRateChangeEvent& e) override {
        requestedHostRate.store(chimera::RateBridge::supported(e.sampleRate) ?
            static_cast<unsigned>(e.sampleRate) : 0u, std::memory_order_release);
    }
    void ensureRateService() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (rateServiceRegistered) return;
        chimera::registerRateBridge({&requestedHostRate, &activeHostRate,
            &preparedBridge, &retiredBridge, &bridgeError});
        rateServiceRegistered = true;
    }
    void onAdd(const AddEvent&) override {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        ensureRateService();
#ifndef CHIMERA_MANUAL_CONTROL_TEST
        startControlDispatcher();
#endif
        std::string manifest;
        {
            std::lock_guard<std::mutex> lock(persistenceMutex);
            manifest = committedManifest;
        }
        if (manifest.empty()) return;
        if (!service) service = chimera::chimeraIoService();
        if (!service) { ioError.store(true, std::memory_order_release); return; }
        const std::string root = getPatchStorageDirectory();
        loadTicket.reset(new LoadTicket);
        const std::shared_ptr<LoadTicket> ticket = loadTicket;
        const chimera::IoService::Status queued = service->execute(generation,
            nextRequestId++, [ticket, root, manifest] {
                try { ticket->result = chimera::bundle::load(root, manifest); }
                catch (...) { ticket->result.error = "patch_load_exception"; }
                if (ticket->result.reel) {
                    try { ticket->waveform = chimera::WaveformSummary::fromActive(*ticket->result.reel); }
                    catch (...) {} // Audio loading must not depend on display allocation.
                }
                ticket->done.store(true, std::memory_order_release);
            });
        if (queued != chimera::IoService::Accepted) {
            loadTicket.reset();
            ioError.store(true, std::memory_order_release);
        }
        else ioBusy.store(true, std::memory_order_release);
    }
    void onSave(const SaveEvent&) override {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        saveInProgress.store(true, std::memory_order_release);
        struct SaveScope {
            std::atomic<bool>& flag;
            ~SaveScope() { flag.store(false, std::memory_order_release); }
        } saveScope{saveInProgress};
        // One budget for all worker/capture waits, rather than a new 30-second
        // deadline per phase. Filesystem publication itself is synchronous:
        // Rack needs a completed manifest before it builds the patch archive.
        const std::uint64_t deadline = steadyNs() + controlWaitNs;
        const auto failSave = [&](const std::string& reason) {
            saveFailure.store(true, std::memory_order_release);
            ioError.store(true, std::memory_order_release);
            std::string retained;
            {
                std::lock_guard<std::mutex> lock(persistenceMutex);
                retained = committedManifest.empty() ? "; Reel audio has not been saved." :
                    "; previous saved Reel retained.";
            }
            setIoMessage("Save incomplete: " + reason + "; retry Save" + retained);
        };
        while ((loadTicket || awaitingHandle || markerRequestId || pendingSave) && steadyNs() < deadline) {
            serviceStep();
            if (loadTicket || awaitingHandle || markerRequestId || pendingSave)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (loadTicket || awaitingHandle || markerRequestId || pendingSave) {
            failSave("Reel operation is still running"); return;
        }
        if (!controlActiveHandle) {
            std::lock_guard<std::mutex> lock(persistenceMutex);
            saveFailure.store(!committedManifest.empty(), std::memory_order_release);
            return;
        }
        while (steadyNs() < deadline) {
            serviceStep();
            if (!recoveryPurpose && !recoveryTicket && snapshotRequestId != kAutomaticSnapshotId) {
                if (snapshotReady) break;
                if (!snapshotRequestId) requestSnapshot();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (!snapshotReady || !snapshotReel || recoveryPurpose || recoveryTicket ||
            snapshotRequestId == kAutomaticSnapshotId) {
            if (!recoveryPurpose && !recoveryTicket && snapshotRequestId != kAutomaticSnapshotId)
                abandonSnapshot();
            failSave("snapshot is not ready"); return;
        }
        const auto ticket = std::make_shared<SaveTicket>();
        ticket->id = std::to_string(steadyNs()) + "-" + std::to_string(nextRequestId);
#ifdef CHIMERA_MANUAL_CONTROL_TEST
        ticket->beforeEncode = saveEncodingHookForTest;
#endif
        bool submitted = false;
        try {
            if (ticket->staging.ensure(rack::system::join(
                    leviathanPluginUserRootPath(), "Chimera/save-staging-v1"))) {
                chimera::Reel* const frozen = snapshotReel;
                submitted = service->execute(generation, nextRequestId++, [ticket, frozen] {
                    try {
#ifdef CHIMERA_MANUAL_CONTROL_TEST
                        if (ticket->beforeEncode) ticket->beforeEncode();
#endif
                        ticket->result = chimera::bundle::stage(ticket->staging.directory(), ticket->id, *frozen);
                    }
                    catch (...) { ticket->result.error = "patch_save_exception"; }
                    ticket->done.store(true, std::memory_order_release);
                }) == chimera::IoService::Accepted;
            }
        }
        catch (...) {}
        if (!submitted) {
            finishSnapshotReader(); failSave("could not stage Reel audio"); return;
        }
        pendingSave = ticket;
        while (!ticket->done.load(std::memory_order_acquire) && steadyNs() < deadline) {
            serviceStep();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (!ticket->done.load(std::memory_order_acquire)) {
            ticket->abandoned = true;
            // The dispatcher releases this reader only after done. Teardown
            // transfers its Reel to the ticket if a worker is still running.
            failSave("audio encoding exceeded the wait budget"); return;
        }
        chimera::bundle::CommitResult published;
        std::string storageRoot;
        try {
            if (ticket->result) {
                storageRoot = createPatchStorageDirectory();
                published = chimera::bundle::publishStaged(ticket->staging.directory(), storageRoot, ticket->result);
            }
            else published.error = ticket->result.error;
        }
        catch (...) { published.error = "patch_publish_exception"; }
        finishSnapshotReader();
        pendingSave.reset();
        if (!published) failSave(published.error);
        else {
            {
                std::lock_guard<std::mutex> lock(persistenceMutex);
                committedManifest = published.manifest;
                committedDocumentRevision = published.documentRevision;
                committedAudioRevision = published.audioRevision;
            }
            savedAudioRevision.store(published.audioRevision, std::memory_order_release);
            savedDocumentRevision.store(published.documentRevision, std::memory_order_release);
            hasSavedReel.store(true, std::memory_order_release);
            unsavedImport.store(false, std::memory_order_release);
            saveFailure.store(false, std::memory_order_release);
            ioError.store(false, std::memory_order_release);
            setIoMessage("");
            if (!chimera::bundle::pruneObsolete(storageRoot, published.manifest))
                setIoWarning("Obsolete Reel assets could not all be pruned");
        }
        // Reclamation can finish on the dispatcher after Rack serializes.
        while (snapshotRequestId && steadyNs() < deadline) {
            serviceStep();
            if (snapshotRequestId) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    void onRemove(const RemoveEvent&) override {
        stopControlDispatcher();
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (rateServiceRegistered) {
            chimera::unregisterRateBridge(&preparedBridge);
            rateServiceRegistered = false;
        }
    }
    bool requestImportWav(const std::string& path, bool truncate, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (markerRequestId || loadTicket || awaitingHandle || pendingRequestId || retiringHandle ||
            snapshotRequestId || recordingOrArmed.load(std::memory_order_acquire)) {
            error = "Reel operation or recording is active"; return false;
        }
        if (!service) service = chimera::chimeraIoService();
        if (!service) { error = "Reel I/O service unavailable"; return false; }
        std::shared_ptr<LoadTicket> ticket(new LoadTicket);
        ticket->adoptionKind = 4;
        const chimera::IoService::Status queued = service->execute(generation,
            nextRequestId++, [ticket, path, truncate] {
                try {
                    std::ifstream input;
                    if (rack::system::isFile(path)) input.open(path.c_str(), std::ios::binary);
                    if (!input.is_open()) ticket->result.error = "source_wav_missing_or_not_regular";
                    else {
                        chimera::wav::ImportResult imported =
                            chimera::wav::readConvenience(input, truncate);
                        if (imported) {
                            ticket->result.reel = std::move(imported.reel);
                            for (const std::string& warning : imported.warnings) {
                                if (!ticket->warning.empty()) ticket->warning += ", ";
                                ticket->warning += warning;
                            }
                            if (imported.nonfiniteSamples)
                                ticket->warning += " (" + std::to_string(
                                    imported.nonfiniteSamples) + " nonfinite samples)";
                        }
                        else {
                            ticket->result.error = imported.error;
                            if (imported.error == "reel_too_long" && imported.sourceRate)
                                ticket->result.error += " (" + std::to_string(
                                    double(imported.sourceFrames) / imported.sourceRate) +
                                    " s; choose explicit truncate to 174 s)";
                        }
                    }
                }
                catch (...) { ticket->result.error = "wav_import_exception"; }
                if (ticket->result.reel) {
                    try { ticket->waveform = chimera::WaveformSummary::fromActive(*ticket->result.reel); }
                    catch (...) {}
                }
                ticket->done.store(true, std::memory_order_release);
            });
        if (queued != chimera::IoService::Accepted) {
            error = "Reel I/O queue is busy"; return false;
        }
        loadTicket = ticket;
        ioBusy.store(true, std::memory_order_release);
        return true;
    }
    bool requestRecovery(bool preRecord, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (markerRequestId || loadTicket || awaitingHandle || pendingRequestId || retiringHandle ||
            snapshotRequestId || recordingOrArmed.load(std::memory_order_acquire)) {
            error = "Reel operation or recording is active"; return false;
        }
        const std::string root = recoveryRoot();
        const chimera::recovery::Journal journal = chimera::recovery::inspect(root);
        const chimera::recovery::Entry entry = preRecord ?
            journal.preRecord : journal.latest;
        if (!entry) { error = "No committed recovery checkpoint"; return false; }
        if (!service) service = chimera::chimeraIoService();
        if (!service) { error = "Reel I/O service unavailable"; return false; }
        std::shared_ptr<LoadTicket> ticket(new LoadTicket);
        ticket->adoptionKind = 4;
        ticket->warning = "Restored checkpoint captured at Unix ms " +
            std::to_string(entry.capturedAtMs);
        const chimera::IoService::Status queued = service->execute(generation,
            nextRequestId++, [ticket, root, entry] {
                try { ticket->result = chimera::recovery::load(root, entry); }
                catch (...) { ticket->result.error = "recovery_load_exception"; }
                if (ticket->result.reel) {
                    try { ticket->waveform = chimera::WaveformSummary::fromActive(*ticket->result.reel); }
                    catch (...) {}
                }
                ticket->done.store(true, std::memory_order_release);
            });
        if (queued != chimera::IoService::Accepted) {
            error = "Recovery I/O queue is busy"; return false;
        }
        loadTicket = ticket;
        ioBusy.store(true, std::memory_order_release);
        return true;
    }
    bool exportWav(const std::string& path, bool overwrite, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (rack::system::exists(path) && !overwrite) {
            error = "Target WAV exists"; return false;
        }
        onSave(SaveEvent{});
        if (saveFailure.load(std::memory_order_acquire)) {
            error = "Could not capture Reel for export"; return false;
        }
        std::string manifest;
        {
            std::lock_guard<std::mutex> lock(persistenceMutex);
            manifest = committedManifest;
        }
        if (manifest.empty()) { error = "Reel is empty"; return false; }
        const std::string source = rack::system::join(getPatchStorageDirectory(),
            manifest.substr(0, manifest.size() - 5) + ".wav");
        if (!rack::system::isFile(source)) { error = "Reel is empty"; return false; }
        if (source == path) { error = "Choose a different export path"; return false; }
        if (!rack::system::copy(source, path)) {
            error = "WAV export failed"; return false;
        }
        return true;
    }
    bool requestHeavyEdit(const chimera::edit::Request& request, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        const std::uint64_t deadline = steadyNs() + controlWaitNs;
        if (!recordingOrArmed.load(std::memory_order_acquire) &&
            (recoveryPurpose || recoveryTicket || recoveryPostPending.load())) {
            while ((recoveryPurpose || recoveryTicket || recoveryPostPending.load() ||
                    snapshotRequestId) && steadyNs() < deadline) {
                serviceStep();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        if (!controlActiveHandle || !service || markerRequestId || loadTicket || pendingRequestId ||
            awaitingHandle || retiringHandle || snapshotRequestId ||
            recordingOrArmed.load(std::memory_order_acquire)) {
            error = "Reel operation or recording is active"; return false;
        }
        if (!requestSnapshot()) { error = "Reel snapshot is busy"; return false; }
        while (!snapshotReady && snapshotRequestId && steadyNs() < deadline) {
            serviceStep();
            if (!snapshotReady) std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        if (!snapshotReady || !snapshotReel) {
            abandonSnapshot();
            error = "Reel snapshot failed"; return false;
        }
        const chimera::SnapshotMetadata metadata = snapshotReel->snapshotMetadata();
        chimera::Reel* const frozen = snapshotReel;
        std::shared_ptr<LoadTicket> ticket(new LoadTicket);
        ticket->adoptionKind = 5;
        ticket->usesSnapshot = true;
        ticket->expectedDocumentRevision = metadata.documentRevision;
        ticket->expectedAudioRevision = metadata.audioRevision;
        const chimera::IoService::Status queued = service->execute(generation,
            nextRequestId++, [ticket, frozen, request] {
                try {
                    chimera::edit::Result edited = chimera::edit::build(*frozen, request);
                    if (edited) ticket->result.reel = std::move(edited.reel);
                    else ticket->result.error = edited.error;
                }
                catch (...) { ticket->result.error = "edit_worker_exception"; }
                if (ticket->result.reel) {
                    try { ticket->waveform = chimera::WaveformSummary::fromActive(*ticket->result.reel); }
                    catch (...) {}
                }
                ticket->done.store(true, std::memory_order_release);
            });
        if (queued != chimera::IoService::Accepted) {
            finishSnapshotReader();
            error = "Reel I/O queue is busy"; return false;
        }
        loadTicket = ticket;
        ioBusy.store(true, std::memory_order_release);
        return true;
    }
    bool requestEdit(const chimera::edit::Request& request, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (request.kind == chimera::edit::MoveMarker || request.kind == chimera::edit::RemoveMarker)
            return requestMarkerEdit(request.splice, request.frame,
                request.kind == chimera::edit::RemoveMarker ? 1u : 0u, error);
        return requestHeavyEdit(request, error);
    }
    bool requestMarkerEdit(unsigned index, unsigned frame, unsigned kind, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (!controlActiveHandle || markerRequestId || loadTicket || awaitingHandle ||
            pendingRequestId || retiringHandle || recordingOrArmed.load(std::memory_order_acquire)) {
            error = "Reel operation or recording is active"; return false;
        }
        const std::uint64_t id = nextRequestId++;
        const MarkerCommand command{id, publishedDocumentRevision.load(std::memory_order_acquire),
            index, frame, kind};
        if (!markerCommands.tryPush(command)) { error = "Marker edit queue is busy"; return false; }
        markerRequestId = id;
        ioBusy.store(true, std::memory_order_release);
        return true;
    }
    bool requestUndo(bool redo, std::string& error) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (publishedMarkerHistory.load(std::memory_order_acquire) == (redo ? 2u : 1u))
            return requestMarkerEdit(0, 0, redo ? 3u : 2u, error);
        error = redo ? "No marker edit to redo" : "No marker edit to undo";
        return false;
    }
    chimera::IoService::Status requestPreparedStore(std::uint32_t pages,
                                                     std::uint32_t reservePages) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
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
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        // One control dispatcher is the SPSC producer. No worker touches the
        // live page table; a ready cut keeps its store resident until release.
        if (!controlActiveHandle || pendingRequestId || awaitingHandle || retiringHandle ||
            snapshotRequestId || snapshotReaders.count()) return false;
        bool unclaimed = false;
        if (!snapshotClaimed.compare_exchange_strong(unclaimed, true,
                std::memory_order_acq_rel)) return false;
        const std::uint64_t id = nextRequestId++;
        const chimera::AudioCommand command = {
            generation->value.load(std::memory_order_acquire), id, 0, 2,
            controlActiveHandle, nullptr, 0};
        if (!commands.tryPush(command)) {
            snapshotClaimed.store(false, std::memory_order_release);
            return false;
        }
        snapshotAbandoned = false;
        snapshotRequestId = id;
        snapshotReel = stores.lookup(controlActiveHandle);
        return true;
    }
    void abandonSnapshot() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        snapshotAbandoned = snapshotRequestId != 0;
        if (snapshotReady) finishSnapshotReader();
        // Keep the request ID and claim until core capture/reclaim completes.
        // Clearing them here would admit a second snapshot onto the same Reel.
    }
    bool finishSnapshotReader() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (!snapshotReady || !snapshotReaders.count()) return false;
        if (!snapshotReaders.finish()) return true; // Other readers retain it.
        snapshotReady = false;
        snapshotReleasePending = true;
        return true;
    }
    void enqueueSnapshotRelease() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (!snapshotReleasePending) return;
        const chimera::AudioCommand command = {
            generation->value.load(std::memory_order_acquire), snapshotRequestId,
            0, 3, controlActiveHandle, nullptr, 0};
        if (commands.tryPushCritical(command)) snapshotReleasePending = false;
    }
    static std::uint64_t steadyNs() {
        return static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    }
    std::string recoveryRoot() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        const std::string patchPath = APP && APP->patch ? APP->patch->path : "";
        const std::string source = patchPath.empty() ?
            (APP && APP->patch ? getPatchStorageDirectory() : "unattached-patch") : patchPath;
        const std::string identity = source +
            "#" + std::to_string(getId());
        std::uint64_t hash = UINT64_C(14695981039346656037);
        for (unsigned char c : identity) {
            hash ^= c;
            hash *= UINT64_C(1099511628211);
        }
        return rack::system::join(leviathanPluginUserRootPath(),
            "Chimera/recovery/" + std::to_string(hash));
    }
    static std::string checkpointTime(const chimera::recovery::Entry& entry) {
        const std::time_t seconds = static_cast<std::time_t>(entry.capturedAtMs / 1000);
        std::tm utc{};
#ifdef _WIN32
        if (gmtime_s(&utc, &seconds)) return "time unavailable";
#else
        if (!gmtime_r(&seconds, &utc)) return "time unavailable";
#endif
        char text[32];
        if (!std::strftime(text, sizeof(text), "%Y-%m-%d %H:%M UTC", &utc))
            return "time unavailable";
        return text;
    }
    void recoveryStep() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (recoveryTicket && recoveryTicket->done.load(std::memory_order_acquire)) {
            if (recoveryTicket->waveform)
                std::atomic_store_explicit(&waveform, recoveryTicket->waveform,
                                           std::memory_order_release);
            if (!recoveryTicket->displayOnly && !recoveryTicket->result) {
                setIoWarning("Recovery checkpoint failed: " + recoveryTicket->result.error);
                ioError.store(true, std::memory_order_release);
            }
            recoveryTicket.reset();
            recoveryPurpose = 0;
            finishSnapshotReader();
        }
        if (recoveryPurpose && snapshotReady && snapshotReel && !recoveryTicket) {
            const std::string root = cachedRecoveryRoot;
            const std::string id = std::to_string(steadyNs()) + "-" +
                std::to_string(nextRequestId);
            const std::uint64_t wallMs = std::uint64_t(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count());
            const chimera::recovery::Role role = recoveryPurpose == 1 ?
                chimera::recovery::PreRecord : chimera::recovery::Latest;
            const bool displayOnly = recoveryPurpose == 3;
            chimera::Reel* const frozen = snapshotReel;
            std::shared_ptr<RecoveryTicket> ticket(new RecoveryTicket);
            ticket->displayOnly = displayOnly;
            const chimera::IoService::Status queued = service->execute(generation,
                nextRequestId++, [ticket, root, id, frozen, role, wallMs, displayOnly] {
                    try {
                        ticket->waveform = chimera::WaveformSummary::fromSnapshot(*frozen);
                        if (!displayOnly) ticket->result = chimera::recovery::commit(
                            root, id, *frozen, role, wallMs);
                    }
                    catch (...) { ticket->result.error = "recovery_worker_exception"; }
                    ticket->done.store(true, std::memory_order_release);
                });
            if (queued == chimera::IoService::Accepted) recoveryTicket = ticket;
            else {
                if (!displayOnly) setIoWarning("Recovery checkpoint queue is busy");
                recoveryPurpose = 0;
                finishSnapshotReader();
            }
            return;
        }
        if (recoveryPurpose || recoveryTicket || snapshotRequestId ||
            saveInProgress.load(std::memory_order_acquire) || !controlActiveHandle ||
            loadTicket || awaitingHandle || pendingRequestId || retiringHandle) return;
        const std::uint64_t now = steadyNs();
        const bool stopped = recoveryPostPending.load(std::memory_order_acquire);
        const std::uint64_t start = recordStartNs.load(std::memory_order_acquire);
        const std::uint64_t since = lastRecoveryNs > start ? lastRecoveryNs : start;
        const bool periodic = recordingActive.load(std::memory_order_acquire) &&
            since && now - since >= UINT64_C(10000000000);
        const std::uint64_t displayStep = displayHeartbeatNs.load(std::memory_order_acquire);
        const bool displayRecentlyStepped = displayStep && now >= displayStep &&
            now - displayStep < UINT64_C(500000000);
        if ((stopped || periodic) && requestSnapshot()) {
            recoveryPurpose = 2;
            lastRecoveryNs = now;
            if (stopped) recoveryPostPending.store(false, std::memory_order_release);
        }
        else if (!stopped && now - lastDisplayAttemptNs >= UINT64_C(1000000000) &&
                 (!recordingActive.load(std::memory_order_acquire) ||
                  displayRecentlyStepped)) {
            const auto cached = std::atomic_load_explicit(&waveform,
                std::memory_order_acquire);
            const std::uint64_t document = publishedDocumentRevision.load(std::memory_order_acquire);
            const std::uint64_t audio = publishedAudioRevision.load(std::memory_order_acquire);
            if ((!cached || cached->documentRevision != document ||
                 cached->audioRevision != audio) && requestSnapshot()) {
                recoveryPurpose = 3;
                lastDisplayAttemptNs = now;
            }
        }
    }
    void coreCommands() {
        MarkerCommand marker{};
        if (markerCommands.tryPop(marker)) {
            const bool accepted = reel && recordArm == NoArm &&
                reel->documentRevision() == marker.revision &&
                slice.editMarker(marker.index, marker.frame, marker.kind == 1,
                    marker.kind >= 2 ? marker.kind - 1 : 0);
            publishedMarkerHistory.store(slice.markerHistoryState(), std::memory_order_release);
            if (reel) {
                publishedDocumentRevision.store(reel->documentRevision(), std::memory_order_release);
                publishedMarkerCount.store(reel->markerCount(), std::memory_order_release);
                publishedRegion.store(slice.currentRegion(), std::memory_order_release);
                publishedRequestedRegion.store(slice.requestedRegion(), std::memory_order_release);
            }
            const chimera::AudioCompletion ack{generation->value.load(std::memory_order_acquire),
                marker.id, 8, 0, accepted ? 1u : 0u};
            completions.tryPushCritical(ack);
        }
        chimera::AudioCommand command{};
        for (int i = 0; i < 4 && commands.tryPop(command); ++i) {
            if (command.moduleGeneration != generation->value.load(std::memory_order_acquire))
                continue;
            if ((command.kind == 1 || command.kind == 4 || command.kind == 5) &&
                command.prepared) {
                // Automatic recording snapshots may start after control queued
                // adoption. Claim the same fence as snapshot admission before
                // swapping; the old store must have no readers or reclaim work.
                bool unclaimed = false;
                if ((command.kind != 1 &&
                    (slice.recordState() != chimera::Slice::Idle || recordArm != NoArm ||
                     (command.kind == 5 && reel &&
                      (reel->documentRevision() != command.expectedDocumentRevision ||
                       reel->audioRevision() != command.expectedAudioRevision)))) ||
                    (reel && reel->state() != chimera::Reel::Idle) ||
                    !snapshotClaimed.compare_exchange_strong(unclaimed, true,
                        std::memory_order_acq_rel)) {
                    const chimera::AudioCompletion rejected = {command.moduleGeneration,
                        command.requestId, 6, command.handle, 0};
                    completions.tryPushCritical(rejected);
                    continue;
                }
                const std::uint32_t oldHandle = audioActiveHandle;
                audioActiveHandle = command.handle;
                reel = command.prepared;
                slice.setReel(command.prepared);
                publishedMarkerHistory.store(0, std::memory_order_release);
                recordArm = NoArm;
                snapshotClaimed.store(false, std::memory_order_release);
                const chimera::AudioCompletion ack = {command.moduleGeneration, command.requestId,
                                                      1, command.handle, oldHandle};
                completions.tryPushCritical(ack);
            }
            else if (command.kind == 2) {
                const bool accepted = reel && command.handle == audioActiveHandle &&
                    reel->beginSnapshot(slice.frame());
                if (accepted) {
                    coreSnapshotRequestId = command.requestId;
                    coreSnapshotReadySent = false;
                }
                const chimera::AudioCompletion ack = {command.moduleGeneration, command.requestId,
                                                      2, command.handle, accepted ? 1u : 0u};
                completions.tryPushCritical(ack);
            }
            else if (command.kind == 3) {
                const bool released = reel && command.handle == audioActiveHandle &&
                    command.requestId == coreSnapshotRequestId && reel->beginRelease();
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
        if (coreSnapshotRequestId == kAutomaticSnapshotId && !automaticSnapshotAnnounced) {
            const chimera::AudioCompletion started = {generationValue,
                kAutomaticSnapshotId, 7, audioActiveHandle, 1};
            automaticSnapshotAnnounced = completions.tryPushCritical(started);
            if (!automaticSnapshotAnnounced) return;
        }
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
                automaticSnapshotAnnounced = false;
            }
        }
    }
    void maintenanceStep() {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (!commands.size() && !markerCommands.size() && !snapshotRequestId) return;
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
    void serviceStep(bool hostContext = true) {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        if (hostContext) cachedRecoveryRoot = recoveryRoot();
        const auto now = steadyNs();
        if (!checkpointSessionRoot.empty() && now >= nextCheckpointSweepNs) {
            chimera::CheckpointSession::sweep(checkpointSessionRoot, std::time(nullptr));
            nextCheckpointSweepNs = now + UINT64_C(60000000000);
        }
        ensureRateService();
        if (pendingSave && pendingSave->abandoned && pendingSave->done.load(std::memory_order_acquire)) {
            finishSnapshotReader();
            pendingSave.reset(); // Never publish an attempt whose caller timed out.
        }
        if (loadTicket && loadTicket->done.load(std::memory_order_acquire) &&
            loadTicket->usesSnapshot && snapshotReady && snapshotReaders.count())
            finishSnapshotReader();
        if (loadTicket && loadTicket->done.load(std::memory_order_acquire) &&
            !snapshotRequestId && !snapshotReaders.count() &&
            !snapshotClaimed.load(std::memory_order_acquire)) {
            const unsigned adoptionKind = loadTicket->adoptionKind;
            const std::uint64_t expectedDocument = loadTicket->expectedDocumentRevision;
            const std::uint64_t expectedAudio = loadTicket->expectedAudioRevision;
            const std::string warning = loadTicket->warning;
            const std::shared_ptr<const chimera::WaveformSummary> loadedWaveform =
                std::move(loadTicket->waveform);
            chimera::bundle::LoadResult loaded = std::move(loadTicket->result);
            const std::uint64_t loadedDocumentRevision = loaded.documentRevision;
            const std::uint64_t loadedAudioRevision = loaded.audioRevision;
            loadTicket.reset();
            if (!loaded || (adoptionKind == 1 && controlActiveHandle) ||
                awaitingHandle || pendingRequestId) {
                setIoWarning("");
                setIoMessage(loaded.error.empty() ?
                    "Reel replacement was not ready" : loaded.error);
                ioError.store(true, std::memory_order_release);
                if (adoptionKind == 1)
                    saveFailure.store(true, std::memory_order_release);
                ioBusy.store(false, std::memory_order_release);
            }
            else {
                setIoWarning(warning);
                const std::uint32_t handle = nextHandle++;
                if (!stores.accept(handle, loaded.reel,
                        chimera::StoreBudget::Prepared)) {
                    setIoMessage("Reel memory budget exceeded");
                    ioError.store(true, std::memory_order_release);
                    ioBusy.store(false, std::memory_order_release);
                }
                else {
                    const chimera::AudioCommand adopt = {
                        generation->value.load(std::memory_order_acquire),
                        nextRequestId++, expectedDocument, adoptionKind,
                        handle, stores.lookup(handle), expectedAudio};
                    if (!commands.tryPush(adopt)) {
                        stores.releaseOffAudio(handle);
                        setIoMessage("Reel adoption queue is full");
                        ioError.store(true, std::memory_order_release);
                        ioBusy.store(false, std::memory_order_release);
                    }
                    else {
                        awaitingHandle = handle;
                        awaitingAdoptionKind = adoptionKind;
                        awaitingDocumentRevision = loadedDocumentRevision;
                        awaitingAudioRevision = loadedAudioRevision;
                        awaitingWaveform = loadedWaveform;
                    }
                }
            }
        }
        if (!loadTicket && prepareRequested.exchange(false, std::memory_order_acq_rel) &&
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
            if (ack.kind == 8 && ack.requestId == markerRequestId) {
                markerRequestId = 0;
                ioBusy.store(false, std::memory_order_release);
                if (ack.status) {
                    recoveryPostPending.store(true, std::memory_order_release);
                    ioError.store(false, std::memory_order_release);
                    setIoMessage("");
                }
                else {
                    ioError.store(true, std::memory_order_release);
                    setIoMessage("Marker edit rejected: invalid marker, changed Reel, or recording active");
                }
                continue;
            }
            if (ack.kind == 7 && ack.requestId == kAutomaticSnapshotId) {
                snapshotRequestId = kAutomaticSnapshotId;
                snapshotReel = stores.lookup(ack.handle);
                recoveryPurpose = 1;
                lastRecoveryNs = steadyNs();
                continue;
            }
            if (ack.kind == 2 && ack.requestId == snapshotRequestId) {
                if (!ack.status) {
                    snapshotRequestId = 0;
                    snapshotAbandoned = false;
                    snapshotReel = nullptr;
                    snapshotClaimed.store(false, std::memory_order_release);
                    recoveryPurpose = 0;
                    ioError.store(true, std::memory_order_release);
                }
                continue;
            }
            if (ack.kind == 4 && ack.requestId == snapshotRequestId) {
                snapshotReady = snapshotReaders.begin();
                if (snapshotReady && snapshotAbandoned) finishSnapshotReader();
                else if (!snapshotReady) {
                    snapshotReleasePending = true;
                    recoveryPurpose = 0;
                    ioError.store(true, std::memory_order_release);
                }
                continue;
            }
            if (ack.kind == 3 && ack.requestId == snapshotRequestId) {
                if (!ack.status) ioError.store(true, std::memory_order_release);
                continue;
            }
            if (ack.kind == 5 && ack.requestId == snapshotRequestId) {
                snapshotRequestId = 0;
                snapshotAbandoned = false;
                snapshotReel = nullptr;
                snapshotClaimed.store(false, std::memory_order_release);
                continue;
            }
            if (ack.kind == 6 && ack.handle == awaitingHandle) {
                stores.releaseOffAudio(awaitingHandle);
                setIoMessage("Reel changed, recording started, or a snapshot is active; retry the edit/import");
                setIoWarning("");
                awaitingHandle = 0;
                awaitingAdoptionKind = 1;
                awaitingWaveform.reset();
                ioBusy.store(false, std::memory_order_release);
                ioError.store(true, std::memory_order_release);
                continue;
            }
            if (ack.kind != 1) continue;
            if (ack.handle != awaitingHandle) continue;
            ioError.store(false, std::memory_order_release);
            setIoMessage("");
            controlActiveHandle = ack.handle;
            std::atomic_store_explicit(&waveform, awaitingWaveform,
                                       std::memory_order_release);
            awaitingWaveform.reset();
            if (awaitingAdoptionKind == 1) {
                std::lock_guard<std::mutex> lock(persistenceMutex);
                committedDocumentRevision = awaitingDocumentRevision;
                committedAudioRevision = awaitingAudioRevision;
                savedAudioRevision.store(awaitingAudioRevision, std::memory_order_release);
                savedDocumentRevision.store(awaitingDocumentRevision, std::memory_order_release);
                hasSavedReel.store(true, std::memory_order_release);
            }
            stores.transition(ack.handle, chimera::StoreBudget::Active);
            if (ack.status) {
                stores.transition(ack.status, chimera::StoreBudget::Retired);
                retiringHandle = ack.status;
                retiringPayload = stores.detachRetired(retiringHandle);
            }
            awaitingHandle = 0;
            if (awaitingAdoptionKind != 1)
                unsavedImport.store(true, std::memory_order_release);
            if (awaitingAdoptionKind == 4 || awaitingAdoptionKind == 5)
                recoveryPostPending.store(true, std::memory_order_release);
            awaitingAdoptionKind = 1;
            readyForPrepare = true;
            ioBusy.store(retiringHandle != 0, std::memory_order_release);
        }
        recoveryStep();
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
                                             pendingRequestId, 0, 1, handle, stores.lookup(handle), 0};
        pendingRequestId = 0;
        if (!commands.tryPush(adopt)) {
            stores.releaseOffAudio(handle);
            ioBusy.store(false, std::memory_order_release);
            ioError.store(true, std::memory_order_release);
            return;
        }
        awaitingHandle = handle;
    }

    chimera::optionsText::Values currentOptionsText() const {
        const int stage = optionsTextStageState.load(std::memory_order_acquire);
        if (stage == 1 || stage == 3) return stagedOptionsText;
        chimera::optionsText::Values v;
        v.ckop = ckopSetting.load(std::memory_order_acquire);
        v.vsop = vsopSetting.load(std::memory_order_acquire);
        v.inop = inopSetting.load(std::memory_order_acquire);
        v.pmin = pminSetting.load(std::memory_order_acquire);
        v.omod = omodSetting.load(std::memory_order_acquire);
        v.gnsm = gnsmSetting.load(std::memory_order_acquire);
        v.rsop = rsopSetting.load(std::memory_order_acquire);
        v.pmod = pmodSetting.load(std::memory_order_acquire);
        v.cvop = cvopSetting.load(std::memory_order_acquire);
        for (int i = 0; i < 3; ++i)
            v.mcr[i] = mcrSetting[i].load(std::memory_order_acquire);
        return v;
    }
    bool importOptionsText(const std::string& source, std::string& error,
                           std::vector<std::string>& warnings) {
        std::lock_guard<std::mutex> lock(optionsTextExtrasMutex);
        const chimera::optionsText::Result parsed =
            chimera::optionsText::parse(source, currentOptionsText());
        if (!parsed.valid) {
            error = parsed.line ? "Line " + std::to_string(parsed.line) + ": " +
                                  parsed.error : parsed.error;
            return false;
        }
        int expected = 0;
        if (!optionsTextStageState.compare_exchange_strong(expected, 2,
                std::memory_order_acq_rel)) {
            error = "A prior options edit is still being applied";
            return false;
        }
        stagedOptionsText = parsed.values;
        optionsTextExtras = parsed.extras;
        warnings = parsed.warnings;
        optionsTextStageState.store(1, std::memory_order_release);
        return true;
    }
    std::string exportOptionsText() {
        std::lock_guard<std::mutex> lock(optionsTextExtrasMutex);
        return chimera::optionsText::exportText(currentOptionsText(), optionsTextExtras);
    }
    void adoptOptionsText() {
        int expected = 1;
        if (!optionsTextStageState.compare_exchange_strong(expected, 3,
                std::memory_order_acq_rel)) return;
        const chimera::optionsText::Values& v = stagedOptionsText;
        ckopSetting.store(v.ckop, std::memory_order_release);
        vsopSetting.store(v.vsop, std::memory_order_release);
        inopSetting.store(v.inop != 0, std::memory_order_release);
        pminSetting.store(v.pmin != 0, std::memory_order_release);
        omodSetting.store(v.omod != 0, std::memory_order_release);
        gnsmSetting.store(v.gnsm != 0, std::memory_order_release);
        rsopSetting.store(v.rsop, std::memory_order_release);
        pmodSetting.store(v.pmod, std::memory_order_release);
        cvopSetting.store(v.cvop != 0, std::memory_order_release);
        for (int i = 0; i < 3; ++i)
            mcrSetting[i].store(v.mcr[i], std::memory_order_release);
        optionsTextStageState.store(0, std::memory_order_release);
    }

    json_t* dataToJson() override {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        std::lock_guard<std::mutex> lock(optionsTextExtrasMutex);
        json_t* root = json_object();
        const chimera::optionsText::Values settings = currentOptionsText();
        std::string manifest;
        std::uint64_t documentRevision = 0, audioRevision = 0;
        {
            std::lock_guard<std::mutex> persistenceLock(persistenceMutex);
            manifest = committedManifest;
            documentRevision = committedDocumentRevision;
            audioRevision = committedAudioRevision;
        }
        json_object_set_new(root, "schemaVersion", json_integer(1));
        json_object_set_new(root, "dspProfile", json_integer(1));
        json_object_set_new(root, "bandlimitedPlayback", json_boolean(bandlimitedPlayback.load()));
        const bool unsavedAudio = unsavedImport.load(std::memory_order_acquire) ||
            publishedAudioRevision.load(std::memory_order_acquire) > audioRevision;
        json_object_set_new(root, "audioStatus", json_string(manifest.empty() ?
            (saveFailure.load(std::memory_order_acquire) ? "missing-audio" : "empty-or-unsaved") :
            (saveFailure.load(std::memory_order_acquire) ? "save-failed-prior-bundle" :
             (unsavedAudio ? "embedded-prior-cut" : "embedded"))));
        json_object_set_new(root, "saveFailure",
            json_boolean(saveFailure.load(std::memory_order_acquire)));
        json_t* storage = json_object();
        json_object_set_new(storage, "mode", json_string(manifest.empty() ? "none" : "embedded"));
        json_object_set_new(storage, "manifest", manifest.empty() ?
            json_null() : json_string(manifest.c_str()));
        json_object_set_new(storage, "savedDocumentRevision",
            json_string(std::to_string(documentRevision).c_str()));
        json_object_set_new(storage, "savedAudioRevision",
            json_string(std::to_string(audioRevision).c_str()));
        json_object_set_new(root, "storage", storage);
        json_object_set_new(root, "inop", json_boolean(settings.inop != 0));
        json_object_set_new(root, "gnsm", json_integer(settings.gnsm));
        json_object_set_new(root, "cvop", json_integer(settings.cvop));
        json_object_set_new(root, "omod", json_integer(settings.omod));
        json_object_set_new(root, "pmin", json_integer(settings.pmin));
        json_object_set_new(root, "pmod", json_integer(settings.pmod));
        json_object_set_new(root, "ckop", json_integer(settings.ckop));
        json_object_set_new(root, "vsop", json_integer(settings.vsop));
        json_object_set_new(root, "rsop", json_integer(settings.rsop));
        json_object_set_new(root, "inputGain", json_integer(inputGainSetting.load(std::memory_order_acquire)));
        for (int i = 0; i < 3; ++i) {
            const char* key = i == 0 ? "mcr1" : (i == 1 ? "mcr2" : "mcr3");
            json_object_set_new(root, key, json_real(settings.mcr[i]));
        }
        json_t* extras = json_object();
        for (const auto& entry : optionsTextExtras)
            json_object_set_new(extras, entry.first.c_str(), json_string(entry.second.c_str()));
        json_object_set_new(root, "optionsTextExtras", extras);
        return root;
    }
    void dataFromJson(json_t* root) override {
        std::lock_guard<std::recursive_mutex> controlLock(controlMutex);
        json_t* schema = json_object_get(root, "schemaVersion");
        json_t* profile = json_object_get(root, "dspProfile");
        if ((schema && (!json_is_integer(schema) || json_integer_value(schema) != 1)) ||
            (profile && (!json_is_integer(profile) || json_integer_value(profile) != 1))) {
            setIoMessage("Unsupported Chimera schema or DSP profile");
            ioError.store(true, std::memory_order_release);
            return;
        }
        bandlimitedPlayback.store(json_is_true(json_object_get(root, "bandlimitedPlayback")));
        json_t* storage = json_object_get(root, "storage");
        json_t* manifest = json_object_get(storage, "manifest");
        if (json_is_string(manifest)) {
            const std::string reference = json_string_value(manifest);
            if (chimera::bundle::validManifestReference(reference)) {
                std::lock_guard<std::mutex> lock(persistenceMutex);
                committedManifest = reference;
            }
            else {
                setIoMessage("Invalid embedded Reel manifest path");
                ioError.store(true, std::memory_order_release);
            }
        }
        saveFailure.store(json_is_true(json_object_get(root, "saveFailure")),
                          std::memory_order_release);
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
        json_t* vsop = json_object_get(root, "vsop");
        if (json_is_integer(vsop) && json_integer_value(vsop) >= 0 && json_integer_value(vsop) <= 2)
            vsopSetting.store(static_cast<int>(json_integer_value(vsop)), std::memory_order_release);
        json_t* rsop = json_object_get(root, "rsop");
        if (json_is_integer(rsop) && (json_integer_value(rsop) == 0 || json_integer_value(rsop) == 1))
            rsopSetting.store(static_cast<int>(json_integer_value(rsop)), std::memory_order_release);
        json_t* gain = json_object_get(root, "inputGain");
        if (json_is_integer(gain) && json_integer_value(gain) >= 0 && json_integer_value(gain) <= 3)
            inputGainSetting.store(static_cast<int>(json_integer_value(gain)), std::memory_order_release);
        for (int i = 0; i < 3; ++i) {
            const char* key = i == 0 ? "mcr1" : (i == 1 ? "mcr2" : "mcr3");
            json_t* ratio = json_object_get(root, key);
            const double value = json_number_value(ratio);
            if (json_is_number(ratio) && chimera::profile1::finite(value) &&
                std::fabs(value) >= 0.0625 && std::fabs(value) <= 16.0)
                mcrSetting[i].store(static_cast<float>(value), std::memory_order_release);
        }
        json_t* extras = json_object_get(root, "optionsTextExtras");
        if (json_is_object(extras)) {
            std::map<std::string, std::string> loaded;
            const char* key = nullptr;
            json_t* value = nullptr;
            json_object_foreach(extras, key, value) {
                if (loaded.size() >= 64 || !json_is_string(value)) continue;
                const std::string name(key);
                const std::string token(json_string_value(value));
                if (chimera::optionsText::safeKey(name) &&
                    !chimera::optionsText::recognizedKey(name) &&
                    chimera::optionsText::safeValue(token)) loaded[name] = token;
            }
            std::lock_guard<std::mutex> lock(optionsTextExtrasMutex);
            optionsTextExtras.swap(loaded);
        }
    }

    void process(const ProcessArgs& args) override {
        const bool measurePerf = isDragonKingDebugEnabled();
        const auto processStart = debug_terminal::debugTimerStart(measurePerf);
        if (!ownership.tryAudio()) {
            outputs[AUDIO_L_OUTPUT].setVoltage(0.f);
            outputs[AUDIO_R_OUTPUT].setVoltage(0.f);
            outputs[CV_OUTPUT].setVoltage(0.f);
            outputs[EOSG_OUTPUT].setVoltage(0.f);
            if (measurePerf) debugMetrics.recordProcess(
                debug_terminal::elapsedNsSince(processStart));
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
        if (measurePerf) debugMetrics.recordProcess(
            debug_terminal::elapsedNsSince(processStart));
    }
    void processBypass(const ProcessArgs& args) override {
        const bool measurePerf = isDragonKingDebugEnabled();
        const auto processStart = debug_terminal::debugTimerStart(measurePerf);
        (void) args;
        if (ownership.tryAudio()) {
            coreCommands();
            if (recordingActive.exchange(false, std::memory_order_acq_rel))
                recoveryPostPending.store(true, std::memory_order_release);
            slice.stopRecord();
            recordArm = NoArm;
            recordingOrArmed.store(false, std::memory_order_release);
            publishedRecordState.store(0, std::memory_order_release);
            menuCommand.exchange(0, std::memory_order_acq_rel);
            selectionMenuCommands.exchange(0, std::memory_order_acq_rel);
            if (reel) reel->maintenanceTick();
            coreSnapshotProgress();
            ownership.releaseAudio(steadyNs());
        }
        bypassActive = true;
        const chimera::HostState host = captureHost();
        lastRecButton = host.params[REC_PARAM] > 0.5f;
        lastSpliceButton = host.params[SPLICE_PARAM] > 0.5f;
        lastShiftButton = host.params[SHIFT_PARAM] > 0.5f;
        ignoreRecRelease = lastRecButton;
        ignoreSpliceRelease = lastSpliceButton;
        ignoreShiftRelease = lastShiftButton;
        lastRecJack = recGate.update(host.connected[REC_INPUT] ? host.volts[REC_INPUT] : 0.f);
        lastClock = clockGate.update(host.connected[CLOCK_INPUT] ? host.volts[CLOCK_INPUT] : 0.f);
        lastShiftJack = shiftGate.update(host.connected[SHIFT_INPUT] ? host.volts[SHIFT_INPUT] : 0.f);
        lastSpliceJack = spliceGate.update(host.connected[SPLICE_INPUT] ? host.volts[SPLICE_INPUT] : 0.f);
        const float left = host.connected[AUDIO_L_INPUT] ?
            chimera::profile1::audio(host.volts[AUDIO_L_INPUT]) : 0.f;
        const float right = host.connected[AUDIO_R_INPUT] ?
            chimera::profile1::audio(host.volts[AUDIO_R_INPUT]) : left;
        writeOutput({left, right, 0.f, false});
        lights[REC_LIGHT].setBrightness(0.f);
        lights[REC_ARMED_LIGHT].setBrightness(0.f);
        if (measurePerf) debugMetrics.recordProcess(
            debug_terminal::elapsedNsSince(processStart));
    }
    chimera::HostState captureHost() {
        chimera::HostState state;
        state.command = menuCommand.exchange(0, std::memory_order_acq_rel);
        state.selectionCommands =
            selectionMenuCommands.exchange(0, std::memory_order_acq_rel);
        for (int i = 0; i < NUM_PARAMS; ++i)
            state.params[i] = params[i].getValue();
        for (int i = 0; i < NUM_INPUTS; ++i) {
            state.connected[i] = inputs[i].isConnected();
            state.volts[i] = state.connected[i] ? inputs[i].getVoltage() : 0.f;
        }
        return state;
    }
    void writeOutput(const chimera::HostOutput& out) {
        outputs[AUDIO_L_OUTPUT].setVoltage(out.left);
        outputs[AUDIO_R_OUTPUT].setVoltage(out.right);
        outputs[CV_OUTPUT].setVoltage(out.cv);
        outputs[EOSG_OUTPUT].setVoltage(out.eosg ? 10.f : 0.f);
    }
    void processOwned(const ProcessArgs& args) {
        if (!reel && (inputs[AUDIO_L_INPUT].isConnected() ||
                      inputs[AUDIO_R_INPUT].isConnected()))
            prepareRequested.store(true, std::memory_order_release);
        const chimera::HostState host = captureHost();
        if (bypassActive) {
            bypassActive = false;
            playInitialized = false;
            unbypassFadeRemaining = 48;
            bridgeResumePending = true;
            // Reuse the off-audio retirement/preparation path. A used SRC
            // must not retain pre-bypass audio, commands or gate history.
            if (activeBridge) activeHostRate.store(0, std::memory_order_release);
        }
        const bool supported = chimera::RateBridge::supported(args.sampleRate);
        const unsigned rate = supported ? static_cast<unsigned>(args.sampleRate) : 0u;
        requestedHostRate.store(rate, std::memory_order_release);
        if (!rate) {
            if (recordingActive.exchange(false, std::memory_order_acq_rel))
                recoveryPostPending.store(true, std::memory_order_release);
            slice.stopRecord();
            recordArm = NoArm;
            publishedRecordState.store(0, std::memory_order_release);
            // A resumed supported rate must adopt a freshly primed bridge.
            // Keep the old allocation for off-audio retirement below.
            activeHostRate.store(0, std::memory_order_release);
            menuCommand.exchange(0, std::memory_order_acq_rel);
            selectionMenuCommands.exchange(0, std::memory_order_acq_rel);
            lastRecButton = host.params[REC_PARAM] > 0.5f;
            lastSpliceButton = host.params[SPLICE_PARAM] > 0.5f;
            lastShiftButton = host.params[SHIFT_PARAM] > 0.5f;
            ignoreRecRelease = lastRecButton;
            ignoreSpliceRelease = lastSpliceButton;
            ignoreShiftRelease = lastShiftButton;
            playInitialized = false;
            stopAtPrimaryBoundary = false;
            lastRecJack = recGate.update(host.connected[REC_INPUT] ? host.volts[REC_INPUT] : 0.f);
            lastClock = clockGate.update(host.connected[CLOCK_INPUT] ? host.volts[CLOCK_INPUT] : 0.f);
            lastShiftJack = shiftGate.update(host.connected[SHIFT_INPUT] ? host.volts[SHIFT_INPUT] : 0.f);
            lastSpliceJack = spliceGate.update(host.connected[SPLICE_INPUT] ? host.volts[SPLICE_INPUT] : 0.f);
            if (reel) reel->maintenanceTick();
            writeOutput({});
            bridgeError.store(true, std::memory_order_release);
            lights[IO_BUSY_LIGHT].setBrightness(0.f);
            lights[PM_LIGHT].setBrightness(0.f);
            lights[ERROR_LIGHT].setBrightness(1.f);
            return;
        }
        if (rate != activeHostRate.load(std::memory_order_relaxed)) {
            if (recordingActive.exchange(false, std::memory_order_acq_rel))
                recoveryPostPending.store(true, std::memory_order_release);
            slice.stopRecord();
            recordArm = NoArm;
            publishedRecordState.store(0, std::memory_order_release);
            playInitialized = false;
            stopAtPrimaryBoundary = false;
            menuCommand.exchange(0, std::memory_order_acq_rel);
            selectionMenuCommands.exchange(0, std::memory_order_acq_rel);
            lastRecButton = host.params[REC_PARAM] > 0.5f;
            lastSpliceButton = host.params[SPLICE_PARAM] > 0.5f;
            lastShiftButton = host.params[SHIFT_PARAM] > 0.5f;
            ignoreRecRelease = lastRecButton;
            ignoreSpliceRelease = lastSpliceButton;
            ignoreShiftRelease = lastShiftButton;
            lastRecJack = recGate.update(host.connected[REC_INPUT] ? host.volts[REC_INPUT] : 0.f);
            lastClock = clockGate.update(host.connected[CLOCK_INPUT] ? host.volts[CLOCK_INPUT] : 0.f);
            lastShiftJack = shiftGate.update(host.connected[SHIFT_INPUT] ? host.volts[SHIFT_INPUT] : 0.f);
            lastSpliceJack = spliceGate.update(host.connected[SPLICE_INPUT] ? host.volts[SPLICE_INPUT] : 0.f);
            if (activeBridge) {
                chimera::RateBridge* empty = nullptr;
                if (!retiredBridge.compare_exchange_strong(empty, activeBridge,
                        std::memory_order_acq_rel)) {
                    writeOutput({});
                    lights[IO_BUSY_LIGHT].setBrightness(1.f);
                    return;
                }
                activeBridge = nullptr;
            }
            if (rate != 48000) {
                if (retiredBridge.load(std::memory_order_acquire)) {
                    writeOutput({});
                    lights[IO_BUSY_LIGHT].setBrightness(1.f);
                    return;
                }
                chimera::RateBridge* ready = preparedBridge.exchange(nullptr,
                    std::memory_order_acq_rel);
                if (!ready || ready->rate() != rate) {
                    if (ready) {
                        chimera::RateBridge* empty = nullptr;
                        retiredBridge.compare_exchange_strong(empty, ready,
                            std::memory_order_acq_rel);
                    }
                    writeOutput({});
                    lights[IO_BUSY_LIGHT].setBrightness(1.f);
                    return;
                }
                activeBridge = ready;
                bridgeInputLatencyHost.store(ready->inputLatencyHost(),
                    std::memory_order_release);
                bridgeOutputLatencyHost.store(ready->outputLatencyHost(),
                    std::memory_order_release);
            }
            else {
                bridgeInputLatencyHost.store(0, std::memory_order_release);
                bridgeOutputLatencyHost.store(0, std::memory_order_release);
            }
            lastRecButton = host.params[REC_PARAM] > 0.5f;
            lastSpliceButton = host.params[SPLICE_PARAM] > 0.5f;
            lastShiftButton = host.params[SHIFT_PARAM] > 0.5f;
            ignoreRecRelease = lastRecButton;
            ignoreSpliceRelease = lastSpliceButton;
            ignoreShiftRelease = lastShiftButton;
            lastRecJack = recGate.update(host.connected[REC_INPUT] ? host.volts[REC_INPUT] : 0.f);
            lastClock = clockGate.update(host.connected[CLOCK_INPUT] ? host.volts[CLOCK_INPUT] : 0.f);
            lastShiftJack = shiftGate.update(host.connected[SHIFT_INPUT] ? host.volts[SHIFT_INPUT] : 0.f);
            lastSpliceJack = spliceGate.update(host.connected[SPLICE_INPUT] ? host.volts[SPLICE_INPUT] : 0.f);
            activeHostRate.store(rate, std::memory_order_release);
            bridgeError.store(false, std::memory_order_release);
            lights[IO_BUSY_LIGHT].setBrightness(0.f);
        }
        if (rate == 48000) {
            bridgeResumePending = false;
            processCore(host);
            return;
        }
        if (bridgeResumePending) {
            // Clear pre-bypass delayed commands/audio and seed held gates.
            // Also covers a freshly prepared rate after a bypass-time change.
            // Construction and stale-bridge destruction stay off audio.
            const bool gates[5] = {
                playGate.update(host.connected[PLAY_INPUT] ? host.volts[PLAY_INPUT] : 0.f),
                clockGate.update(host.connected[CLOCK_INPUT] ? host.volts[CLOCK_INPUT] : 0.f),
                recGate.update(host.connected[REC_INPUT] ? host.volts[REC_INPUT] : 0.f),
                spliceGate.update(host.connected[SPLICE_INPUT] ? host.volts[SPLICE_INPUT] : 0.f),
                shiftGate.update(host.connected[SHIFT_INPUT] ? host.volts[SHIFT_INPUT] : 0.f)};
            lastClock = gates[1];
            lastRecJack = gates[2];
            lastSpliceJack = gates[3];
            lastShiftJack = gates[4];
            activeBridge->seedGatesForResume(host, gates);
            bridgeResumePending = false;
        }
        const chimera::HostOutput out = activeBridge->step(host,
            [this](const chimera::HostState& delayed) { return processCore(delayed); });
        writeOutput(out);
        if (activeBridge->failed()) {
            slice.stopRecord();
            recordArm = NoArm;
            // The next callback retires this faulted converter; the service
            // prepares a replacement without work on the audio thread.
            activeHostRate.store(0, std::memory_order_release);
            bridgeError.store(true, std::memory_order_release);
            lights[ERROR_LIGHT].setBrightness(1.f);
        }
    }
    chimera::HostOutput processCore(const chimera::HostState& host) {
        const bool wasRecording = slice.recordState() != chimera::Slice::Idle;
        const float l = host.connected[AUDIO_L_INPUT] ?
            chimera::profile1::audio(host.volts[AUDIO_L_INPUT]) : 0.f;
        const float r = host.connected[AUDIO_R_INPUT] ?
            chimera::profile1::audio(host.volts[AUDIO_R_INPUT]) : l;
        adoptOptionsText();
        const bool playConnected = host.connected[PLAY_INPUT];
        const bool playLogical = !playConnected ||
            playGate.update(host.volts[PLAY_INPUT]) || host.rises[0] != 0;
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
        slice.setRateMode(vsopSetting.load(std::memory_order_relaxed));
        slice.setChordRatios(mcrSetting[0].load(std::memory_order_relaxed),
                             mcrSetting[1].load(std::memory_order_relaxed),
                             mcrSetting[2].load(std::memory_order_relaxed));
        const bool shiftButton = host.params[SHIFT_PARAM] > 0.5f;
        const bool spliceButton = host.params[SPLICE_PARAM] > 0.5f;
        const bool rec = host.params[REC_PARAM] > 0.5f;
        slice.setInputGain(inputGainSetting.load(std::memory_order_relaxed));
        const unsigned selectionCommands = host.selectionCommands;
        const bool shiftJack = shiftGate.update(host.connected[SHIFT_INPUT] ?
            host.volts[SHIFT_INPUT] : 0.f);
        if ((!shiftButton && lastShiftButton && !ignoreShiftRelease) ||
            (shiftJack && !lastShiftJack) || host.rises[4] || (selectionCommands & 1u))
            slice.requestShift();
        if (!shiftButton) ignoreShiftRelease = false;
        lastShiftButton = shiftButton;
        lastShiftJack = shiftJack;
        const bool spliceJack = spliceGate.update(host.connected[SPLICE_INPUT] ?
            host.volts[SPLICE_INPUT] : 0.f);
        if ((!spliceButton && lastSpliceButton && !ignoreSpliceRelease) ||
            (spliceJack && !lastSpliceJack) || host.rises[3] || (selectionCommands & 2u))
            slice.requestSplice();
        if (!spliceButton) ignoreSpliceRelease = false;
        lastSpliceButton = spliceButton;
        lastSpliceJack = spliceJack;
        const bool recButtonReleased = !rec && lastRecButton && !ignoreRecRelease;
        if (!rec) ignoreRecRelease = false;
        lastRecButton = rec;
        const bool recJack = recGate.update(host.connected[REC_INPUT] ?
            host.volts[REC_INPUT] : 0.f);
        const bool clockConnected = host.connected[CLOCK_INPUT];
        const bool clock = clockGate.update(clockConnected ?
            host.volts[CLOCK_INPUT] : 0.f);
        const bool clockRise = (clock && !lastClock) || host.rises[1] != 0;
        lastClock = clock;
        const chimera::ClockEstimator::Update clockUpdate =
            clockEstimator.step(clockConnected, clockRise, slice.frame());
        const int clockOption = ckopSetting.load(std::memory_order_relaxed);
        slice.setClockPlayback(clockConnected, clockUpdate.acceptedEdge,
            clockEstimator.havePeriod() ? clockEstimator.periodFrames() : 0,
            clockEstimator.waiting(), clockOption);
        chimera::CoreInput in{};
        in.live = chimera::StereoFrame{l, r};
        in.pmRightVolts = host.connected[AUDIO_R_INPUT] ?
            host.volts[AUDIO_R_INPUT] : 0.f;
        in.pmRightConnected = host.connected[AUDIO_R_INPUT];
        chimera::ControlFrame& c = in.controls;
        c.sos = host.params[SOS_PARAM];
        c.gene = host.params[GENE_SIZE_PARAM];
        c.rate = host.params[VARISPEED_PARAM];
        c.morph = host.params[MORPH_PARAM];
        c.slide = host.params[SLIDE_PARAM];
        c.organize = host.params[ORGANIZE_PARAM];
        c.geneAtt = host.params[GENE_ATT_PARAM];
        c.rateAtt = host.params[VARISPEED_ATT_PARAM];
        c.slideAtt = host.params[SLIDE_ATT_PARAM];
        c.sosPatched = host.connected[SOS_CV_INPUT];
        c.sosCv = host.volts[SOS_CV_INPUT];
        c.geneCv = host.volts[GENE_SIZE_CV_INPUT];
        c.rateCv = host.volts[VARISPEED_CV_INPUT];
        c.morphCv = host.volts[MORPH_CV_INPUT];
        c.slideCv = host.volts[SLIDE_CV_INPUT];
        c.organizeCv = host.volts[ORGANIZE_CV_INPUT];
        slice.prepareFrameSelection(in);
        const int command = host.command;
        if (!clockConnected) recordArm = NoArm;
        if (command == 3) {
            recordArm = NoArm;
            slice.stopRecord();
        }
        else {
            const unsigned jackRises = host.rises[2] ? host.rises[2] :
                ((recJack && !lastRecJack) ? 1u : 0u);
            const unsigned requests = jackRises +
                ((command == 1 || command == 2 || command == 4 || recButtonReleased) ? 1u : 0u);
            for (unsigned request = 0; request < requests; ++request) {
            if (!reel) prepareRequested.store(true, std::memory_order_release);
            const bool append = command == 2 ||
                (command != 1 && (command == 4 ?
                    rsopSetting.load(std::memory_order_relaxed) == 0 :
                    rsopSetting.load(std::memory_order_relaxed) == 1));
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
        }
        // Resolve REC before this same-frame Clock edge. A coincident arm and
        // edge therefore includes the start frame and excludes the stop frame.
        if (clockRise && recordArm != NoArm) {
            if (recordArm == ArmStop) slice.stopRecord();
            else beginRecording(recordArm == ArmAppend);
            recordArm = NoArm;
        }
        lastRecJack = recJack;
        if (stopAtPrimaryBoundary && clockUpdate.acceptedEdge && clockConnected &&
            slice.clockShiftMode()) {
            transportPlay = false;
            stopAtPrimaryBoundary = false;
            slice.setPlay(false);
        }
        slice.setBandlimitedPlayback(bandlimitedPlayback.load(std::memory_order_relaxed));
        const chimera::Slice::Output out = slice.step(in);
        recordingActive.store(out.recording, std::memory_order_release);
        publishedMarkerHistory.store(slice.markerHistoryState(), std::memory_order_release);
        if (wasRecording && !out.recording)
            recoveryPostPending.store(true, std::memory_order_release);
        if (reel) {
            publishedDocumentRevision.store(reel->documentRevision(), std::memory_order_release);
            publishedAudioRevision.store(reel->audioRevision(), std::memory_order_release);
            publishedValidFrames.store(reel->validFrames(), std::memory_order_release);
            publishedRegion.store(slice.currentRegion(), std::memory_order_release);
            publishedRequestedRegion.store(slice.requestedRegion(), std::memory_order_release);
            publishedMarkerCount.store(reel->markerCount(), std::memory_order_release);
        }
        const int displayRecordState = recordArm == ArmCurrent ? 1 :
            recordArm == ArmAppend ? 2 : recordArm == ArmStop ? 3 :
            slice.recordState() == chimera::Slice::Current ? 4 :
            slice.recordState() == chimera::Slice::Append ? 5 : 0;
        publishedRecordState.store(displayRecordState, std::memory_order_release);
        if ((slice.frame() & 255u) == 0 || wasRecording != out.recording) {
            const double position = slice.playbackPosition();
            publishedPlayFrame.store(position > 0.0 ?
                std::uint32_t(std::min(position, double(UINT32_MAX))) : 0u,
                std::memory_order_release);
            publishedRecordFrame.store(slice.writerFrame(), std::memory_order_release);
        }
        recordingOrArmed.store(out.recording || recordArm != NoArm,
                               std::memory_order_release);
        outputs[AUDIO_L_OUTPUT].setVoltage(clamp(out.audio.l * 5.f, -12.f, 12.f));
        outputs[AUDIO_R_OUTPUT].setVoltage(clamp(out.audio.r * 5.f, -12.f, 12.f));
        outputs[CV_OUTPUT].setVoltage(out.cv);
        outputs[EOSG_OUTPUT].setVoltage(out.eosg ? 10.f : 0.f);
        if (unbypassFadeRemaining) {
            const float gain = float(49 - unbypassFadeRemaining) / 48.f;
            outputs[AUDIO_L_OUTPUT].setVoltage(outputs[AUDIO_L_OUTPUT].getVoltage() * gain);
            outputs[AUDIO_R_OUTPUT].setVoltage(outputs[AUDIO_R_OUTPUT].getVoltage() * gain);
            --unbypassFadeRemaining;
        }
        lights[REC_LIGHT].setBrightness(out.recording ? 1.f : 0.f);
        lights[REC_ARMED_LIGHT].setBrightness(recordArm != NoArm ? 1.f : 0.f);
        lights[CLOCK_LIGHT].setBrightness(clock ? 1.f : 0.f);
        lights[PLAY_LIGHT].setBrightness(transportPlay ? 1.f : 0.f);
        lights[PENDING_LIGHT].setBrightness(slice.requestedRegion() != slice.currentRegion() ? 1.f : 0.f);
        lights[PM_LIGHT].setBrightness(slice.pmBlend());
        lights[CLIP_LIGHT].setBrightness(slice.overloaded() ? 1.f : 0.f);
        lights[IO_BUSY_LIGHT].setBrightness(ioBusy.load(std::memory_order_acquire) ? 1.f : 0.f);
        lights[ERROR_LIGHT].setBrightness(out.full || ioError.load(std::memory_order_acquire) ||
            bridgeError.load(std::memory_order_acquire) ||
            recordNotReady.load(std::memory_order_acquire) ? 1.f : 0.f);
        return {outputs[AUDIO_L_OUTPUT].getVoltage(),
                outputs[AUDIO_R_OUTPUT].getVoltage(), out.cv, out.eosg};
    }
    void beginRecording(bool append) {
        const bool started = append ? slice.startAppend() : slice.startCurrent();
        if (started) {
            recordNotReady.store(false, std::memory_order_release);
            recordStartNs.store(steadyNs(), std::memory_order_release);
            publishedRecordStartFrame.store(slice.writerFrame(), std::memory_order_release);
            bool unclaimed = false;
            if (reel && !saveInProgress.load(std::memory_order_acquire) &&
                snapshotClaimed.compare_exchange_strong(unclaimed, true,
                    std::memory_order_acq_rel)) {
                if (reel->beginSnapshot(slice.frame())) {
                    coreSnapshotRequestId = kAutomaticSnapshotId;
                    coreSnapshotReadySent = false;
                    automaticSnapshotAnnounced = false;
                }
                else snapshotClaimed.store(false, std::memory_order_release);
            }
        }
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

#include "ChimeraDisplay.hpp"

struct ChimeraWidget : ModuleWidget {
    debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
    widget::FramebufferWidget* waveformCache = nullptr;
    ChimeraWaveformLayer* waveformLayer = nullptr;
    ChimeraDisplayOverlay* displayOverlay = nullptr;
    std::shared_ptr<const chimera::WaveformSummary> displayedWaveform;
    ChimeraWidget(Chimera* module) {
        setModule(module);
        const std::string panelPath = asset::plugin(pluginInstance, "res/Chimera.panel.svg");
        setPanel(createPanel(panelPath));
        auto* labelCache = new widget::FramebufferWidget;
        labelCache->box.size = box.size;
        labelCache->oversample = 2.f;
        auto* labels = new widget::SvgWidget;
        labels->setSvg(window::Svg::load(asset::plugin(pluginInstance, "res/Chimera.labels.svg")));
        labels->box.size = box.size;
        labelCache->addChild(labels);
        addChild(labelCache);
        visual_assets::addPerfectWavePanelBranding(this, panelPath);
        visual_assets::addCompactLeviathanLogoBranding(this, panelPath);
        addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
        addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
        addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        auto point = [&](const char* id, Vec fallbackMm) {
            Vec anchor;
            return panel_svg::loadPointFromSvgMm(panelPath, id, &anchor) ? anchor : fallbackMm;
        };
        const Vec displayOrigin = mm2px(point("DISPLAY_ORIGIN", Vec(6, 14)));
        const Vec displayEnd = mm2px(point("DISPLAY_END", Vec(136, 33)));
        waveformCache = new widget::FramebufferWidget;
        waveformCache->box.pos = displayOrigin;
        waveformCache->box.size = displayEnd.minus(displayOrigin);
        waveformCache->oversample = 1.f;
        waveformLayer = new ChimeraWaveformLayer;
        waveformLayer->box.size = waveformCache->box.size;
        waveformCache->addChild(waveformLayer);
        addChild(waveformCache);
        displayOverlay = new ChimeraDisplayOverlay;
        displayOverlay->owner = module;
        displayOverlay->box.pos = displayOrigin;
        displayOverlay->box.size = waveformCache->box.size;
        addChild(displayOverlay);
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("SOS_PARAM", Vec(118.24, 51))), module, Chimera::SOS_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("GENE_SIZE_PARAM", Vec(24, 51))), module, Chimera::GENE_SIZE_PARAM));
        addParam(createParamCentered<ChimeraBipolarHaloKnob>(mm2px(point("VARISPEED_PARAM", Vec(71.12, 51))), module, Chimera::VARISPEED_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("MORPH_PARAM", Vec(24, 80))), module, Chimera::MORPH_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("SLIDE_PARAM", Vec(71.12, 80))), module, Chimera::SLIDE_PARAM));
        addParam(createParamCentered<Eclipse2Knob>(mm2px(point("ORGANIZE_PARAM", Vec(118.24, 80))), module, Chimera::ORGANIZE_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("GENE_ATT_PARAM", Vec(34, 67))), module, Chimera::GENE_ATT_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("VARISPEED_ATT_PARAM", Vec(81.12, 67))), module, Chimera::VARISPEED_ATT_PARAM));
        addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("SLIDE_ATT_PARAM", Vec(81.12, 93))), module, Chimera::SLIDE_ATT_PARAM));
        addParam(createParamCentered<LEDButton>(mm2px(point("REC_PARAM", Vec(60, 103))), module, Chimera::REC_PARAM));
        addParam(createParamCentered<LEDButton>(mm2px(point("SPLICE_PARAM", Vec(72, 103))), module, Chimera::SPLICE_PARAM));
        addParam(createParamCentered<LEDButton>(mm2px(point("SHIFT_PARAM", Vec(84, 103))), module, Chimera::SHIFT_PARAM));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("SOS_CV_INPUT", Vec(118.24, 67))), module, Chimera::SOS_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("GENE_SIZE_CV_INPUT", Vec(14, 67))), module, Chimera::GENE_SIZE_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("VARISPEED_CV_INPUT", Vec(61.12, 67))), module, Chimera::VARISPEED_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("MORPH_CV_INPUT", Vec(24, 93))), module, Chimera::MORPH_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("SLIDE_CV_INPUT", Vec(61.12, 93))), module, Chimera::SLIDE_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("ORGANIZE_CV_INPUT", Vec(118.24, 93))), module, Chimera::ORGANIZE_CV_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("CLOCK_INPUT", Vec(36, 113))), module, Chimera::CLOCK_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("PLAY_INPUT", Vec(48, 113))), module, Chimera::PLAY_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("REC_INPUT", Vec(60, 113))), module, Chimera::REC_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("SPLICE_INPUT", Vec(72, 113))), module, Chimera::SPLICE_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("SHIFT_INPUT", Vec(84, 113))), module, Chimera::SHIFT_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("AUDIO_L_INPUT", Vec(12, 113))), module, Chimera::AUDIO_L_INPUT));
        addInput(createInputCentered<Magitek2InputJack>(mm2px(point("AUDIO_R_INPUT", Vec(24, 113))), module, Chimera::AUDIO_R_INPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("AUDIO_L_OUTPUT", Vec(96, 113))), module, Chimera::AUDIO_L_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("AUDIO_R_OUTPUT", Vec(108, 113))), module, Chimera::AUDIO_R_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("CV_OUTPUT", Vec(120, 113))), module, Chimera::CV_OUTPUT));
        addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("EOSG_OUTPUT", Vec(132, 113))), module, Chimera::EOSG_OUTPUT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(point("REC_LIGHT", Vec(10, 37))), module, Chimera::REC_LIGHT));
        addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(point("REC_ARMED_LIGHT", Vec(25, 37))), module, Chimera::REC_ARMED_LIGHT));
        addChild(createLightCentered<SmallAperture<GreenApertureLight>>(mm2px(point("PLAY_LIGHT", Vec(40, 37))), module, Chimera::PLAY_LIGHT));
        addChild(createLightCentered<SmallAperture<AmberApertureLight>>(mm2px(point("PENDING_LIGHT", Vec(56, 37))), module, Chimera::PENDING_LIGHT));
        addChild(createLightCentered<SmallAperture<BlueApertureLight>>(mm2px(point("CLOCK_LIGHT", Vec(72, 37))), module, Chimera::CLOCK_LIGHT));
        addChild(createLightCentered<SmallAperture<VioletApertureLight>>(mm2px(point("PM_LIGHT", Vec(88, 37))), module, Chimera::PM_LIGHT));
        addChild(createLightCentered<SmallAperture<WhiteApertureLight>>(mm2px(point("IO_BUSY_LIGHT", Vec(103, 37))), module, Chimera::IO_BUSY_LIGHT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(point("CLIP_LIGHT", Vec(118, 37))), module, Chimera::CLIP_LIGHT));
        addChild(createLightCentered<SmallAperture<RedApertureLight>>(mm2px(point("ERROR_LIGHT", Vec(133, 37))), module, Chimera::ERROR_LIGHT));
    }
    void step() override {
        const bool measurePerf = isDragonKingDebugEnabled();
        const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
        Chimera* m = dynamic_cast<Chimera*>(module);
        displayOverlay->owner = m;
        if (m) {
            m->displayHeartbeatNs.store(Chimera::steadyNs(), std::memory_order_release);
            m->serviceStep();
            auto next = std::atomic_load_explicit(&m->waveform,
                std::memory_order_acquire);
            if (next != displayedWaveform) {
                displayedWaveform = next;
                waveformLayer->summary = next;
                displayOverlay->summary = next;
                waveformCache->setDirty();
            }
        }
        else if (displayedWaveform) {
            displayedWaveform.reset();
            waveformLayer->summary.reset();
            displayOverlay->summary.reset();
            waveformCache->setDirty();
        }
        ModuleWidget::step();
        if (measurePerf)
            debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
    }
    void draw(const DrawArgs& args) override {
        const bool measurePerf = module && isDragonKingDebugEnabled();
        const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
        ModuleWidget::draw(args);
        Chimera* m = dynamic_cast<Chimera*>(module);
        if (!m || !measurePerf) return;
        debug_terminal::drawDebugInstanceId(args.vg, box.size, m->debugMetrics.instanceId);
        debugWidgetMetrics.recordDraw(debug_terminal::elapsedUsSince(drawStart));
        if (debug_terminal::baselineSubmitDue("Chimera", m->debugMetrics.instanceId,
                system::getTime()))
            debug_terminal::submitBaselineMetrics("Chimera", m->debugMetrics.instanceId,
                m->debugMetrics.consumeProcessRange(),
                debugWidgetMetrics.consumeStepRange(),
                debugWidgetMetrics.consumeDrawRange());
    }
    void appendContextMenu(Menu* menu) override {
        Chimera* m = dynamic_cast<Chimera*>(module);
        if (!m) return;
        std::lock_guard<std::recursive_mutex> controlLock(m->controlMutex);
        menu->addChild(createCheckMenuItem("Bandlimited playback (higher CPU)", "",
            [m] { return m->bandlimitedPlayback.load(); },
            [m] { m->bandlimitedPlayback.store(!m->bandlimitedPlayback.load()); }));
        const char* memoryStatus = m->controlActiveHandle ? "Reel ready" :
            (m->ioError.load(std::memory_order_acquire) ? "Reel preparation failed" :
             (m->pendingRequestId || m->awaitingHandle ?
                "Preparing memory - press REC again when ready" :
                "Reel idle - connect audio or press REC"));
        menu->addChild(createMenuLabel(memoryStatus));
        const unsigned requestedRate = m->requestedHostRate.load(std::memory_order_acquire);
        const unsigned currentRate = m->activeHostRate.load(std::memory_order_acquire);
        if (!requestedRate)
            menu->addChild(createMenuLabel("Unsupported host sample rate"));
        else if (requestedRate != currentRate)
            menu->addChild(createMenuLabel("Preparing sample rate"));
        else if (currentRate == 48000)
            menu->addChild(createMenuLabel("48 kHz direct path: 0 bridge frames"));
        else {
            const int inputDelay = m->bridgeInputLatencyHost.load(std::memory_order_acquire);
            const int outputDelay = m->bridgeOutputLatencyHost.load(std::memory_order_acquire);
            menu->addChild(createMenuLabel("Bridge at " + std::to_string(currentRate) +
                " Hz: " + std::to_string(inputDelay + outputDelay) +
                " host frames (" + std::to_string(inputDelay) + " in + " +
                std::to_string(outputDelay) + " out)"));
        }
        std::string saveStatus;
        {
            std::lock_guard<std::mutex> lock(m->persistenceMutex);
            saveStatus = m->committedManifest.empty() ?
                "Reel has no committed patch asset yet" :
                "Reel embedded in saved patch";
        }
        if (m->saveFailure.load(std::memory_order_acquire))
            saveStatus = "Reel save/load failed; check embedded asset";
        menu->addChild(createMenuLabel(saveStatus));
        {
            std::lock_guard<std::mutex> lock(m->ioMessageMutex);
            if (m->ioError.load(std::memory_order_acquire) &&
                !m->lastIoMessage.empty())
                menu->addChild(createMenuLabel("Reel error: " + m->lastIoMessage));
            if (!m->lastIoWarning.empty())
                menu->addChild(createMenuLabel("Reel warning: " + m->lastIoWarning));
        }
        const auto loadWav = [m](bool truncate) {
            osdialog_filters* filters = osdialog_filters_parse("WAV audio:wav");
            char* selected = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
            osdialog_filters_free(filters);
            if (!selected) return;
            const std::string path(selected);
            std::free(selected);
            if (m->publishedValidFrames.load(std::memory_order_acquire) &&
                !osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Replace this Chimera Reel with the selected WAV? The current Reel will remain in the last saved patch, but unsaved recording changes will be replaced."))
                return;
            std::string error;
            if (!m->requestImportWav(path, truncate, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        };
        menu->addChild(createMenuItem("Load Reel WAV…", "", [loadWav] {
            loadWav(false);
        }));
        menu->addChild(createMenuItem("Load Reel WAV (truncate over 174 s)…", "", [loadWav] {
            loadWav(true);
        }));
        menu->addChild(createMenuItem("Export Reel WAV…", "", [m] {
            osdialog_filters* filters = osdialog_filters_parse("WAV audio:wav");
            char* selected = osdialog_file(OSDIALOG_SAVE, nullptr, "chimera-reel.wav", filters);
            osdialog_filters_free(filters);
            if (!selected) return;
            const std::string path(selected);
            std::free(selected);
            const bool exists = rack::system::exists(path);
            if (exists && !osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Overwrite the selected WAV file with this Chimera Reel?")) return;
            std::string error;
            if (!m->exportWav(path, exists, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        if (!m->controlActiveHandle && m->ioError.load(std::memory_order_acquire))
            menu->addChild(createMenuItem("Retry Reel preparation", "", [m] {
                m->ioError.store(false, std::memory_order_release);
                m->prepareRequested.store(true, std::memory_order_release);
            }));
        menu->addChild(createMenuItem("Record Current (stop/cancel if active)", "", [m] {
            m->menuCommand.store(1, std::memory_order_release);
        }));
        menu->addChild(createMenuItem("Record Append (stop/cancel if active)", "", [m] {
            m->menuCommand.store(2, std::memory_order_release);
        }));
        menu->addChild(createMenuItem("Alternate REC (per assignment; stop/cancel if active)", "", [m] {
            m->menuCommand.store(4, std::memory_order_release);
        }));
        menu->addChild(createMenuItem("Stop recording / cancel arm", "", [m] {
            m->menuCommand.store(3, std::memory_order_release);
        }));
        menu->addChild(createMenuItem("Next Splice", "", [m] {
            m->selectionMenuCommands.fetch_or(1u, std::memory_order_release);
        }));
        menu->addChild(createMenuItem("Add Marker", "", [m] {
            m->selectionMenuCommands.fetch_or(2u, std::memory_order_release);
        }));
        auto addReelAction = [m, menu](MenuItem* item) {
            item->disabled = !m->controlActiveHandle ||
                m->recordingOrArmed.load(std::memory_order_acquire);
            menu->addChild(item);
        };
        addReelAction(createMenuItem("Move selected marker to frame…", "", [m] {
            char* entered = osdialog_prompt(OSDIALOG_INFO,
                "New 48 kHz sample-frame position for the selected marker:", "");
            if (!entered) return;
            char* end = nullptr;
            const unsigned long value = std::strtoul(entered, &end, 10);
            const bool valid = end != entered && end && *end == '\0' && value <= UINT32_MAX;
            std::free(entered);
            if (!valid) {
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Enter a valid frame number.");
                return;
            }
            chimera::edit::Request request;
            request.kind = chimera::edit::MoveMarker;
            request.splice = m->publishedRegion.load(std::memory_order_acquire);
            request.frame = std::uint32_t(value);
            std::string error;
            if (!m->requestEdit(request, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Remove selected marker…", "", [m] {
            if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Remove this marker and merge its Splice into the preceding one?")) return;
            chimera::edit::Request request;
            request.kind = chimera::edit::RemoveMarker;
            request.splice = m->publishedRegion.load(std::memory_order_acquire);
            std::string error;
            if (!m->requestEdit(request, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Erase selected Splice audio…", "", [m] {
            if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Replace the selected Splice audio with silence? This cannot be undone.")) return;
            chimera::edit::Request request;
            request.kind = chimera::edit::EraseSplice;
            request.splice = m->publishedRegion.load(std::memory_order_acquire);
            std::string error;
            if (!m->requestEdit(request, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Delete selected Splice…", "", [m] {
            if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Delete and compact the selected Splice? This cannot be undone.")) return;
            chimera::edit::Request request;
            request.kind = chimera::edit::DeleteSplice;
            request.splice = m->publishedRegion.load(std::memory_order_acquire);
            std::string error;
            if (!m->requestEdit(request, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Clear Reel…", "", [m] {
            if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                    "Clear all audio and markers in this Reel? This cannot be undone.")) return;
            chimera::edit::Request request;
            request.kind = chimera::edit::ClearReel;
            std::string error;
            if (!m->requestEdit(request, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Undo marker edit", "", [m] {
            std::string error;
            if (!m->requestUndo(false, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        addReelAction(createMenuItem("Redo marker edit", "", [m] {
            std::string error;
            if (!m->requestUndo(true, error))
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
        }));
        const chimera::recovery::Journal recovery =
            chimera::recovery::inspect(m->recoveryRoot());
        if (recovery.latest) {
            const std::string label = "Recover latest checkpoint (" +
                Chimera::checkpointTime(recovery.latest) + ")…";
            addReelAction(createMenuItem(label, "", [m] {
                if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                        "Replace this Reel with the latest completed recovery checkpoint? Changes after that checkpoint will be lost.")) return;
                std::string error;
                if (!m->requestRecovery(false, error))
                    osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
            }));
        }
        if (recovery.preRecord) {
            const std::string label = "Restore pre-recording checkpoint (" +
                Chimera::checkpointTime(recovery.preRecord) + ")…";
            addReelAction(createMenuItem(label, "", [m] {
                if (!osdialog_message(OSDIALOG_WARNING, OSDIALOG_YES_NO,
                        "Replace this Reel with the last pre-recording checkpoint? Recorded changes since that capture will be lost.")) return;
                std::string error;
                if (!m->requestRecovery(true, error))
                    osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
            }));
        }
        auto addBehavior = [m](Menu* menu) {
        menu->addChild(createSubmenuItem("REC assignment (rsop)", "", [m](Menu* submenu) {
            const char* labels[2] = {"REC: Current / alternate: Append", "REC: Append / alternate: Current"};
            for (int mode = 0; mode < 2; ++mode)
                submenu->addChild(createCheckMenuItem(labels[mode], "",
                    [m, mode] { return m->rsopSetting.load(std::memory_order_acquire) == mode; },
                    [m, mode] { m->rsopSetting.store(mode, std::memory_order_release); }));
        }));
        menu->addChild(createSubmenuItem("Input gain", "", [m](Menu* submenu) {
            const char* labels[4] = {"-3 dB", "0 dB (modular)", "+6 dB", "+12 dB"};
            for (int mode = 0; mode < 4; ++mode)
                submenu->addChild(createCheckMenuItem(labels[mode], "",
                    [m, mode] { return m->inputGainSetting.load(std::memory_order_acquire) == mode; },
                    [m, mode] { m->inputGainSetting.store(mode, std::memory_order_release); }));
        }));
        menu->addChild(createCheckMenuItem("Writer: live input only (inop)", "",
            [m] { return m->inopSetting.load(std::memory_order_acquire); },
            [m] { m->inopSetting.store(!m->inopSetting.load(std::memory_order_relaxed), std::memory_order_release); }));
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
        menu->addChild(createSubmenuItem("Vari-Speed mode (vsop)", "", [m](Menu* submenu) {
            const char* labels[3] = {"Classic rate CV", "Bidirectional 1 V/oct", "Forward-only 1 V/oct"};
            for (int mode = 0; mode < 3; ++mode)
                submenu->addChild(createCheckMenuItem(labels[mode], "",
                    [m, mode] { return m->vsopSetting.load(std::memory_order_acquire) == mode; },
                    [m, mode] { m->vsopSetting.store(mode, std::memory_order_release); }));
        }));
        menu->addChild(createMenuItem("Import options text…", "TXT", [m] {
            osdialog_filters* filters = osdialog_filters_parse("Chimera options:txt");
            char* selected = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
            osdialog_filters_free(filters);
            if (!selected) return;
            const std::string path(selected);
            std::free(selected);
            std::ifstream file;
            if (rack::system::isFile(path)) file.open(path, std::ios::binary | std::ios::ate);
            if (!file.is_open() || !file || file.tellg() < 0 || file.tellg() > 65536) {
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
                    "Cannot read options text, or file exceeds 64 KiB.");
                return;
            }
            const std::size_t size = static_cast<std::size_t>(file.tellg());
            std::string source(size, '\0');
            file.seekg(0);
            if (size && !file.read(&source[0], size)) {
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Cannot read options text.");
                return;
            }
            std::string error;
            std::vector<std::string> warnings;
            if (!m->importOptionsText(source, error, warnings)) {
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
                return;
            }
            if (!warnings.empty()) {
                std::string message = "Unknown options retained for export:\n";
                for (std::size_t i = 0; i < warnings.size() && i < 4; ++i)
                    message += warnings[i] + "\n";
                if (warnings.size() > 4) message += "…";
                osdialog_message(OSDIALOG_INFO, OSDIALOG_OK, message.c_str());
            }
        }));
        menu->addChild(createMenuItem("Export options text…", "TXT", [m] {
            osdialog_filters* filters = osdialog_filters_parse("Chimera options:txt");
            char* selected = osdialog_file(OSDIALOG_SAVE, nullptr,
                "chimera-options.txt", filters);
            osdialog_filters_free(filters);
            if (!selected) return;
            const std::string path(selected);
            std::free(selected);
            std::ofstream file(path, std::ios::binary | std::ios::trunc);
            const std::string source = m->exportOptionsText();
            if (!file || !file.write(source.data(), source.size()) || !file.flush())
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
                    "Cannot write options text.");
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
        };
        menu->addChild(createSubmenuItem("Chimera behavior", "", addBehavior));
    }
};

Model* modelChimera = createModel<Chimera, ChimeraWidget>("Chimera");
