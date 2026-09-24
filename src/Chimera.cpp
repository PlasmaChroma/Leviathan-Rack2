#include "plugin.hpp"
#include "ChimeraSlice.hpp"
#include "ChimeraOwnership.hpp"
#include "ChimeraService.hpp"
#include <atomic>
#include <memory>

struct Chimera : Module {
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
    chimera::Reel* reel = nullptr; // Borrowed from the off-audio registry.
    chimera::Slice slice;
    std::shared_ptr<chimera::IoService> service;
    std::shared_ptr<chimera::JobGeneration> generation{new chimera::JobGeneration};
    std::uint64_t nextRequestId = 2, pendingRequestId = 0, retirementRequestId = 0;
    std::uint32_t nextHandle = 2, awaitingHandle = 1, retiringHandle = 0;
    std::unique_ptr<chimera::Reel> retiringPayload; // Control-side until worker accepts it.
    std::uint32_t audioActiveHandle = 0; // Audio callback only.
    bool readyForPrepare = false; // Control dispatcher only.
    std::atomic<bool> ioBusy{false}, ioError{false};
    std::atomic<int> menuCommand{0};
    std::atomic<bool> inopSetting{false};
    bool lastRec = false;
    bool lastRecJack = false;

    Chimera() : slice(nullptr) {
        std::unique_ptr<chimera::Reel> prepared(new chimera::Reel(chimera::kMaxPages, chimera::kMaxPages));
        if (!stores.accept(1, prepared, chimera::StoreBudget::Prepared))
            throw std::bad_alloc();
        reel = stores.lookup(1);
        stores.transition(1, chimera::StoreBudget::Active);
        const chimera::AudioCommand adopt = {1, 1, 0, 1, 1, reel};
        if (!commands.tryPush(adopt)) throw std::bad_alloc();
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
        if (!readyForPrepare || pendingRequestId || awaitingHandle || retiringHandle)
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
    void serviceStep() {
        chimera::AudioCompletion ack{};
        while (completions.tryPop(ack)) {
            if (ack.kind != 1 || ack.moduleGeneration != generation->value.load(std::memory_order_acquire))
                continue;
            if (ack.handle != awaitingHandle) continue;
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
        return root;
    }
    void dataFromJson(json_t* root) override {
        json_t* inop = json_object_get(root, "inop");
        if (json_is_boolean(inop))
            inopSetting.store(json_is_true(inop), std::memory_order_release);
    }

    void process(const ProcessArgs& args) override {
        chimera::AudioCommand transfer{};
        if (commands.tryPop(transfer) && transfer.kind == 1 &&
            transfer.moduleGeneration == generation->value.load(std::memory_order_acquire) &&
            transfer.prepared) {
            const std::uint32_t oldHandle = audioActiveHandle;
            audioActiveHandle = transfer.handle;
            reel = transfer.prepared;
            slice.setReel(transfer.prepared);
            const chimera::AudioCompletion ack = {transfer.moduleGeneration, transfer.requestId,
                                                  1, transfer.handle, oldHandle};
            completions.tryPushCritical(ack);
        }
        const float l = inputs[AUDIO_L_INPUT].isConnected() ?
            chimera::profile1::audio(inputs[AUDIO_L_INPUT].getVoltage()) : 0.f;
        const float r = inputs[AUDIO_R_INPUT].isConnected() ?
            chimera::profile1::audio(inputs[AUDIO_R_INPUT].getVoltage()) : l;
        // The host-rate converter arrives in Phase 6. Do not record 48 kHz
        // Reel frames at an incorrect host rate in this development slice.
        if (args.sampleRate != 48000.f) {
            outputs[AUDIO_L_OUTPUT].setVoltage(l);
            outputs[AUDIO_R_OUTPUT].setVoltage(r);
            lights[ERROR_LIGHT].setBrightness(1.f);
            return;
        }
        slice.setPlay(!inputs[PLAY_INPUT].isConnected() || inputs[PLAY_INPUT].getVoltage() >= 2.5f);
        slice.setInop(inopSetting.load(std::memory_order_relaxed));
        const bool rec = params[REC_PARAM].getValue() > 0.5f;
        const bool recJack = inputs[REC_INPUT].getVoltage() >= 2.5f;
        const int command = menuCommand.exchange(0, std::memory_order_acq_rel);
        if (command == 1 || (rec && !lastRec) || (recJack && !lastRecJack)) {
            if (slice.recordState() != chimera::Slice::Idle) slice.stopRecord();
            else slice.startCurrent();
        }
        else if (command == 2) slice.startAppend();
        else if (command == 3) slice.stopRecord();
        lastRec = rec;
        lastRecJack = recJack;
        chimera::CoreInput in{};
        in.live = chimera::StereoFrame{l, r};
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
        outputs[CV_OUTPUT].setVoltage(0.f);
        outputs[EOSG_OUTPUT].setVoltage(0.f);
        lights[REC_LIGHT].setBrightness(out.recording ? 1.f : 0.f);
        lights[PLAY_LIGHT].setBrightness(inputs[PLAY_INPUT].isConnected() && inputs[PLAY_INPUT].getVoltage() < 2.5f ? 0.f : 1.f);
        lights[CLIP_LIGHT].setBrightness(slice.overloaded() ? 1.f : 0.f);
        lights[IO_BUSY_LIGHT].setBrightness(ioBusy.load(std::memory_order_acquire) ? 1.f : 0.f);
        lights[ERROR_LIGHT].setBrightness(out.full || ioError.load(std::memory_order_acquire) ? 1.f : 0.f);
    }
};

struct ChimeraWidget : ModuleWidget {
    ChimeraWidget(Chimera* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Chimera.svg")));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(25, 42)), module, Chimera::SOS_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(71, 42)), module, Chimera::VARISPEED_PARAM));
        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(117, 42)), module, Chimera::SLIDE_PARAM));
        addParam(createParamCentered<LEDButton>(mm2px(Vec(71, 101)), module, Chimera::REC_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25, 70)), module, Chimera::AUDIO_L_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53, 70)), module, Chimera::AUDIO_R_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(89, 70)), module, Chimera::AUDIO_L_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117, 70)), module, Chimera::AUDIO_R_OUTPUT));
    }
    void step() override {
        if (Chimera* m = dynamic_cast<Chimera*>(module)) m->serviceStep();
        ModuleWidget::step();
    }
    void appendContextMenu(Menu* menu) override {
        Chimera* m = dynamic_cast<Chimera*>(module);
        if (!m) return;
        menu->addChild(createMenuItem("Start Append", "", [m] { m->menuCommand.store(2, std::memory_order_release); }));
        menu->addChild(createMenuItem("Stop recording", "", [m] { m->menuCommand.store(3, std::memory_order_release); }));
        menu->addChild(createMenuItem("Writer: live input only", "", [m] {
            m->inopSetting.store(!m->inopSetting.load(std::memory_order_relaxed), std::memory_order_release);
        }));
    }
};

Model* modelChimera = createModel<Chimera, ChimeraWidget>("Chimera");
