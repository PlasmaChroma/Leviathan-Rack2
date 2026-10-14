#include "../plugin.hpp"
#include "../DebugTerminalMetrics.hpp"
#include "../PanelSvgUtils.hpp"
#include "../visual/ApertureLight.hpp"
#include "../visual/VisualAssets.hpp"

#include "HydraEngine.hpp"

#include <atomic>
#include <cmath>

// Hydra: granular texture processor built on the Beads DSP by Emilie Gillet /
// Mutable Instruments (MIT). See src/hydra/stmlib/LICENSE and doc/hydra_notes.md.

namespace {

std::atomic<uint32_t> gHydraDebugInstanceCounter {1u};

constexpr float kSeedLatchHoldSeconds = 4.f;
constexpr float kNoInputSynthSeconds = 10.f;
constexpr int kControlDivider = 8;
constexpr float kInputScale = 0.2f;   // +-5 V -> +-1.0
constexpr float kOutputScale = 5.f;

} // namespace

struct Hydra : Module {
	ModuleTeardownTimer teardownTimer {"Hydra"};

	enum ParamId {
		TIME_PARAM,
		SIZE_PARAM,
		SHAPE_PARAM,
		PITCH_PARAM,
		TIME_AMT_PARAM,
		SIZE_AMT_PARAM,
		SHAPE_AMT_PARAM,
		PITCH_AMT_PARAM,
		DENSITY_PARAM,
		FEEDBACK_PARAM,
		DRYWET_PARAM,
		REVERB_PARAM,
		ASSIGN_FB_AMT_PARAM,
		ASSIGN_DW_AMT_PARAM,
		ASSIGN_RV_AMT_PARAM,
		QUALITY_PARAM,
		FREEZE_PARAM,
		SEED_PARAM,
		ASSIGN_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_L_INPUT,
		IN_R_INPUT,
		FREEZE_GATE_INPUT,
		SEED_INPUT,
		DENSITY_CV_INPUT,
		TIME_CV_INPUT,
		SIZE_CV_INPUT,
		SHAPE_CV_INPUT,
		PITCH_CV_INPUT,
		ASSIGN_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		INPUT_LIGHT,
		FREEZE_LIGHT,
		SEED_LIGHT,
		QUALITY_LIGHT_0,
		QUALITY_LIGHT_1,
		QUALITY_LIGHT_2,
		QUALITY_LIGHT_3,
		ASSIGN_LIGHT_0,
		ASSIGN_LIGHT_1,
		ASSIGN_LIGHT_2,
		LIGHTS_LEN
	};

	hydra::Engine engine;
	hydra::Controls controls;

	// Persistent state.
	int quality = hydra::QUALITY_BRIGHT_DIGITAL;
	int assignDest = 0; // 0..2 single destination, 3 = per-knob amounts
	bool seedLatched = true;
	bool gateOutput = false;
	bool autoSynthMode = true;

	// Runtime state.
	dsp::BooleanTrigger qualityTrigger;
	dsp::BooleanTrigger assignTrigger;
	dsp::BooleanTrigger seedTrigger;
	dsp::SchmittTrigger freezeSchmitt;
	dsp::SchmittTrigger seedSchmitt;
	float seedHeldSeconds = 0.f;
	bool seedHeld = false;
	bool seedLatchHandled = false;
	float noInputSeconds = 0.f;
	bool synthMode = false;
	float inputLevel = 0.f;
	int controlCounter = 0;
	float hostSampleRate = 48000.f;

	debug_terminal::BaselineModuleMetrics debugMetrics;

	Hydra() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(TIME_PARAM, 0.f, 1.f, 0.f, "Time");
		configParam(SIZE_PARAM, 0.f, 1.f, 0.35f, "Size");
		configParam(SHAPE_PARAM, 0.f, 1.f, 0.5f, "Shape");
		configParam(PITCH_PARAM, 0.f, 1.f, 0.5f, "Pitch");
		configParam(TIME_AMT_PARAM, 0.f, 1.f, 0.5f, "Time CV / randomize");
		configParam(SIZE_AMT_PARAM, 0.f, 1.f, 0.5f, "Size CV / randomize");
		configParam(SHAPE_AMT_PARAM, 0.f, 1.f, 0.5f, "Shape CV / randomize");
		configParam(PITCH_AMT_PARAM, 0.f, 1.f, 0.5f, "Pitch CV / randomize");
		configParam(DENSITY_PARAM, 0.f, 1.f, 0.5f, "Density");
		configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
		configParam(DRYWET_PARAM, 0.f, 1.f, 0.5f, "Dry/wet");
		configParam(REVERB_PARAM, 0.f, 1.f, 0.f, "Reverb");
		configParam(ASSIGN_FB_AMT_PARAM, 0.f, 1.f, 1.f, "Assign CV to feedback");
		configParam(ASSIGN_DW_AMT_PARAM, 0.f, 1.f, 1.f, "Assign CV to dry/wet");
		configParam(ASSIGN_RV_AMT_PARAM, 0.f, 1.f, 1.f, "Assign CV to reverb");
		configButton(QUALITY_PARAM, "Quality");
		configParam(FREEZE_PARAM, 0.f, 1.f, 0.f, "Freeze");
		configButton(SEED_PARAM, "Seed");
		configButton(ASSIGN_PARAM, "Assign CV destination");

		configInput(IN_L_INPUT, "Audio left");
		configInput(IN_R_INPUT, "Audio right");
		configInput(FREEZE_GATE_INPUT, "Freeze gate");
		configInput(SEED_INPUT, "Seed trigger/gate");
		configInput(DENSITY_CV_INPUT, "Density CV");
		configInput(TIME_CV_INPUT, "Time CV");
		configInput(SIZE_CV_INPUT, "Size CV");
		configInput(SHAPE_CV_INPUT, "Shape CV");
		configInput(PITCH_CV_INPUT, "Pitch CV (1 V/oct)");
		configInput(ASSIGN_CV_INPUT, "Assignable CV");
		configOutput(OUT_L_OUTPUT, "Left (mono sum when right unpatched)");
		configOutput(OUT_R_OUTPUT, "Right / grain trigger");

		hostSampleRate = APP && APP->engine ? APP->engine->getSampleRate() : 48000.f;
		engine.init(hostSampleRate);
		debugMetrics.assignInstanceId(gHydraDebugInstanceCounter);
	}

	~Hydra() override {
		teardownTimer.begin(id);
	}

	void onSampleRateChange(const SampleRateChangeEvent& e) override {
		hostSampleRate = e.sampleRate;
		engine.setHostSampleRate(e.sampleRate);
	}

	void onReset() override {
		quality = hydra::QUALITY_BRIGHT_DIGITAL;
		assignDest = 0;
		seedLatched = true;
		gateOutput = false;
		noInputSeconds = 0.f;
		synthMode = false;
	}

	json_t* dataToJson() override {
		json_t* root = json_object();
		json_object_set_new(root, "quality", json_integer(quality));
		json_object_set_new(root, "assignDest", json_integer(assignDest));
		json_object_set_new(root, "seedLatched", json_boolean(seedLatched));
		json_object_set_new(root, "gateOutput", json_boolean(gateOutput));
		json_object_set_new(root, "autoSynthMode", json_boolean(autoSynthMode));
		return root;
	}

	void dataFromJson(json_t* root) override {
		auto getInt = [&](const char* key, int fallback) {
			json_t* j = json_object_get(root, key);
			return j && json_is_number(j) ? int(json_number_value(j)) : fallback;
		};
		auto getBool = [&](const char* key, bool fallback) {
			json_t* j = json_object_get(root, key);
			return j && json_is_boolean(j) ? json_boolean_value(j) : fallback;
		};
		quality = clamp(getInt("quality", quality), 0, hydra::QUALITY_LAST - 1);
		assignDest = clamp(getInt("assignDest", assignDest), 0, 3);
		seedLatched = getBool("seedLatched", seedLatched);
		gateOutput = getBool("gateOutput", gateOutput);
		autoSynthMode = getBool("autoSynthMode", autoSynthMode);
	}

	void updateControls() {
		controls.time = params[TIME_PARAM].getValue();
		controls.size = params[SIZE_PARAM].getValue();
		controls.shape = params[SHAPE_PARAM].getValue();
		controls.pitch = params[PITCH_PARAM].getValue();
		controls.timeAmount = params[TIME_AMT_PARAM].getValue();
		controls.sizeAmount = params[SIZE_AMT_PARAM].getValue();
		controls.shapeAmount = params[SHAPE_AMT_PARAM].getValue();
		controls.pitchAmount = params[PITCH_AMT_PARAM].getValue();
		controls.density = params[DENSITY_PARAM].getValue();
		controls.feedback = params[FEEDBACK_PARAM].getValue();
		controls.dryWet = params[DRYWET_PARAM].getValue();
		controls.reverb = params[REVERB_PARAM].getValue();

		controls.timeCvPatched = inputs[TIME_CV_INPUT].isConnected();
		controls.sizeCvPatched = inputs[SIZE_CV_INPUT].isConnected();
		controls.shapeCvPatched = inputs[SHAPE_CV_INPUT].isConnected();
		controls.pitchCvPatched = inputs[PITCH_CV_INPUT].isConnected();
		controls.densityCvPatched = inputs[DENSITY_CV_INPUT].isConnected();
		controls.timeCv = inputs[TIME_CV_INPUT].getVoltage();
		controls.sizeCv = inputs[SIZE_CV_INPUT].getVoltage();
		controls.shapeCv = inputs[SHAPE_CV_INPUT].getVoltage();
		controls.pitchCv = inputs[PITCH_CV_INPUT].getVoltage();
		controls.densityCv = inputs[DENSITY_CV_INPUT].getVoltage();
		controls.assignCv = inputs[ASSIGN_CV_INPUT].isConnected() ? inputs[ASSIGN_CV_INPUT].getVoltage() : 0.f;

		if (assignDest == 3) {
			controls.assignAmount[0] = params[ASSIGN_FB_AMT_PARAM].getValue();
			controls.assignAmount[1] = params[ASSIGN_DW_AMT_PARAM].getValue();
			controls.assignAmount[2] = params[ASSIGN_RV_AMT_PARAM].getValue();
		} else {
			for (int i = 0; i < 3; ++i) {
				controls.assignAmount[i] = (i == assignDest) ? 1.f : 0.f;
			}
		}

		controls.seedInputPatched = inputs[SEED_INPUT].isConnected();
		controls.quality = quality;
		controls.mono = inputs[IN_L_INPUT].isConnected() && !inputs[IN_R_INPUT].isConnected();
		controls.gateOutput = gateOutput && outputs[OUT_R_OUTPUT].isConnected();
		controls.synthMode = synthMode;
		engine.setControls(controls);
	}

	void process(const ProcessArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto processStart = debug_terminal::debugTimerStart(measurePerf);

		const bool assignHeld = params[ASSIGN_PARAM].getValue() > 0.5f;
		const bool seedPressed = params[SEED_PARAM].getValue() > 0.5f;

		if (qualityTrigger.process(params[QUALITY_PARAM].getValue() > 0.5f)) {
			quality = (quality + 1) % hydra::QUALITY_LAST;
		}
		if (assignTrigger.process(assignHeld)) {
			assignDest = (assignDest + 1) % 4;
		}

		// SEED button: short press leaves latched mode and acts as a gate;
		// holding it for four seconds re-latches. ASSIGN + SEED toggles the
		// grain-trigger output on R.
		if (seedTrigger.process(seedPressed)) {
			if (assignHeld) {
				gateOutput = !gateOutput;
				seedLatchHandled = true;
			} else {
				seedLatchHandled = false;
				if (seedLatched) {
					seedLatched = false;
					seedLatchHandled = true;
				}
			}
			seedHeldSeconds = 0.f;
			seedHeld = !assignHeld;
		}
		if (!seedPressed) {
			seedHeld = false;
			seedHeldSeconds = 0.f;
		} else if (seedHeld) {
			seedHeldSeconds += args.sampleTime;
			if (!seedLatched && seedHeldSeconds >= kSeedLatchHoldSeconds) {
				seedLatched = true;
				seedHeld = false;
			}
		}
		controls.seedState = seedLatched ? hydra::SEED_LATCHED
			: (seedHeld ? hydra::SEED_ON : hydra::SEED_OFF);

		const bool inL = inputs[IN_L_INPUT].isConnected();
		const bool inR = inputs[IN_R_INPUT].isConnected();
		if (inL || inR) {
			noInputSeconds = 0.f;
			synthMode = false;
		} else if (autoSynthMode) {
			noInputSeconds += args.sampleTime;
			if (noInputSeconds >= kNoInputSynthSeconds) {
				synthMode = true;
			}
		} else {
			synthMode = false;
		}

		if (controlCounter == 0) {
			updateControls();
		}
		controlCounter = (controlCounter + 1) % kControlDivider;

		freezeSchmitt.process(inputs[FREEZE_GATE_INPUT].getVoltage(), 0.1f, 1.f);
		const bool freeze = params[FREEZE_PARAM].getValue() > 0.5f || freezeSchmitt.isHigh();
		bool seedGate = false;
		if (controls.seedInputPatched) {
			seedSchmitt.process(inputs[SEED_INPUT].getVoltage(), 0.1f, 1.f);
			seedGate = seedSchmitt.isHigh();
		}

		float l = inL ? inputs[IN_L_INPUT].getVoltage() * kInputScale : 0.f;
		float r = inR ? inputs[IN_R_INPUT].getVoltage() * kInputScale : (inL ? l : 0.f);
		const float peak = std::fmax(std::fabs(l), std::fabs(r));
		inputLevel += (peak - inputLevel) * (peak > inputLevel ? 0.01f : 0.0002f);

		float outL = 0.f, outR = 0.f;
		engine.process(l, r, freeze, seedGate, &outL, &outR);

		if (outputs[OUT_R_OUTPUT].isConnected()) {
			outputs[OUT_L_OUTPUT].setVoltage(outL * kOutputScale);
			outputs[OUT_R_OUTPUT].setVoltage(outR * kOutputScale);
		} else {
			float sum = outL + outR;
			sum = sum < -1.f ? -1.f : (sum > 1.f ? 1.f : sum);
			outputs[OUT_L_OUTPUT].setVoltage(sum * kOutputScale);
		}

		if (measurePerf) {
			debugMetrics.recordProcess(debug_terminal::elapsedNsSince(processStart));
		}

		const float lightDt = args.sampleTime * kControlDivider;
		if (controlCounter == 0) {
			lights[INPUT_LIGHT].setBrightnessSmooth(clamp(inputLevel * 3.f, 0.f, 1.f), lightDt);
			lights[FREEZE_LIGHT].setBrightness(freeze ? 1.f : 0.f);
			lights[SEED_LIGHT].setBrightness(seedLatched ? 0.35f + 0.25f * std::sin(args.frame * args.sampleTime * 2.f)
				: (seedGate || seedHeld ? 1.f : 0.f));
			for (int i = 0; i < hydra::QUALITY_LAST; ++i) {
				lights[QUALITY_LIGHT_0 + i].setBrightness(i == quality ? 1.f : 0.f);
			}
			for (int i = 0; i < 3; ++i) {
				lights[ASSIGN_LIGHT_0 + i].setBrightness((assignDest == 3 || assignDest == i) ? 1.f : 0.f);
			}
		}
	}
};

struct HydraWidget : ModuleWidget {
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	debug_terminal::UiCycleTimingAccumulator drawLayerTiming;

	void step() override {
		drawLayerTiming.beginCycle(module && isDragonKingDebugEnabled());
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::step();
		if (measurePerf) {
			debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
		}
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		const bool measurePerf = drawLayerTiming.enabled;
		const auto start = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::drawLayer(args, layer);
		if (measurePerf) {
			drawLayerTiming.add(debug_terminal::elapsedUsSince(start));
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::draw(args);
		Hydra* mod = static_cast<Hydra*>(module);
		if (!mod || !measurePerf) {
			return;
		}
		debug_terminal::drawDebugInstanceId(args.vg, box.size, mod->debugMetrics.instanceId);
		debugWidgetMetrics.recordDraw(debug_terminal::elapsedUsSince(drawStart));
		const double nowSec = system::getTime();
		if (debug_terminal::baselineSubmitDue("Hydra", mod->debugMetrics.instanceId, nowSec)) {
			debug_terminal::submitBaselineMetrics(
				"Hydra",
				mod->debugMetrics.instanceId,
				mod->debugMetrics.consumeProcessRange(),
				debugWidgetMetrics.consumeStepRange(),
				debugWidgetMetrics.consumeDrawRange(), drawLayerTiming.consume());
		}
	}

	HydraWidget(Hydra* module) {
		setModule(module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/hydra.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addThemedLabels("res/hydra.labels.svg",
			"res/hydra.theme-text-input.svg", "res/hydra.theme-text-output.svg");
		splitPanel.addCompactLeviathanLogoBranding();

		addChild(createWidget<CyanOrbScrew>(Vec(0.f, 0.f)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto anchor = [&](const char* id, float fx, float fy) {
			Vec mm(fx, fy);
			panel_svg::loadPointFromSvgMm(panelPath, id, &mm);
			return mm2px(mm);
		};
		const float cols[5] = {10.2f, 30.5f, 50.8f, 71.1f, 91.4f};
		const float yBtn = 17.f, yKnob = 33.f, yAmt = 46.f, yCv = 59.f, yMix = 75.f, yAsg = 88.f, yJackA = 101.f, yJackB = 113.f;
		const float yLed = 11.2f;

		auto knob = [&](int param, const char* id, float x, float y) {
			addParam(createParamCentered<DarkTinyClockworkGearKnob>(anchor(id, x, y), module, param));
		};
		auto smallKnob = [&](int param, const char* id, float x, float y) {
			addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(anchor(id, x, y), module, param));
		};
		auto button = [&](int param, const char* id, float x, float y, bool momentary) {
			SmallGoldButton* b = createParamCentered<SmallGoldButton>(anchor(id, x, y), module, param);
			b->momentary = momentary;
			addParam(b);
		};
		auto in = [&](int input, const char* id, float x, float y) {
			addInput(createInputCentered<Magitek2InputJack>(anchor(id, x, y), module, input));
		};
		auto out = [&](int output, const char* id, float x, float y) {
			addOutput(createOutputCentered<Magitek2OutputJack>(anchor(id, x, y), module, output));
		};
		auto light = [&](int lightId, const char* id, float x, float y) {
			addChild(createLightCentered<SmallAperture<GreenApertureLight>>(anchor(id, x, y), module, lightId));
		};

		button(Hydra::QUALITY_PARAM, "QUALITY_BTN", cols[0], yBtn, true);
		button(Hydra::FREEZE_PARAM, "FREEZE_BTN", cols[1], yBtn, false);
		button(Hydra::SEED_PARAM, "SEED_BTN", cols[2], yBtn, true);
		button(Hydra::ASSIGN_PARAM, "ASSIGN_BTN", cols[3], yBtn, true);

		static const char* kQualityLeds[4] = {"QUALITY_LED_0", "QUALITY_LED_1", "QUALITY_LED_2", "QUALITY_LED_3"};
		for (int k = 0; k < 4; ++k) {
			light(Hydra::QUALITY_LIGHT_0 + k, kQualityLeds[k], cols[0] - 5.4f + k * 3.6f, yLed);
		}
		light(Hydra::FREEZE_LIGHT, "FREEZE_LED", cols[1], yLed);
		light(Hydra::SEED_LIGHT, "SEED_LED", cols[2], yLed);
		static const char* kAssignLeds[3] = {"ASSIGN_LED_0", "ASSIGN_LED_1", "ASSIGN_LED_2"};
		for (int k = 0; k < 3; ++k) {
			light(Hydra::ASSIGN_LIGHT_0 + k, kAssignLeds[k], cols[3] - 3.6f + k * 3.6f, yLed);
		}
		light(Hydra::INPUT_LIGHT, "INPUT_LED", cols[4], yBtn);

		knob(Hydra::TIME_PARAM, "TIME_KNOB", cols[0], yKnob);
		knob(Hydra::SIZE_PARAM, "SIZE_KNOB", cols[1], yKnob);
		knob(Hydra::SHAPE_PARAM, "SHAPE_KNOB", cols[2], yKnob);
		knob(Hydra::PITCH_PARAM, "PITCH_KNOB", cols[3], yKnob);
		knob(Hydra::DENSITY_PARAM, "DENSITY_KNOB", cols[4], yKnob);
		smallKnob(Hydra::TIME_AMT_PARAM, "TIME_AMT", cols[0], yAmt);
		smallKnob(Hydra::SIZE_AMT_PARAM, "SIZE_AMT", cols[1], yAmt);
		smallKnob(Hydra::SHAPE_AMT_PARAM, "SHAPE_AMT", cols[2], yAmt);
		smallKnob(Hydra::PITCH_AMT_PARAM, "PITCH_AMT", cols[3], yAmt);
		in(Hydra::TIME_CV_INPUT, "TIME_CV", cols[0], yCv);
		in(Hydra::SIZE_CV_INPUT, "SIZE_CV", cols[1], yCv);
		in(Hydra::SHAPE_CV_INPUT, "SHAPE_CV", cols[2], yCv);
		in(Hydra::PITCH_CV_INPUT, "PITCH_CV", cols[3], yCv);
		in(Hydra::DENSITY_CV_INPUT, "DENSITY_CV", cols[4], yCv);

		knob(Hydra::FEEDBACK_PARAM, "FEEDBACK_KNOB", cols[0], yMix);
		knob(Hydra::DRYWET_PARAM, "DRYWET_KNOB", cols[1], yMix);
		knob(Hydra::REVERB_PARAM, "REVERB_KNOB", cols[2], yMix);
		smallKnob(Hydra::ASSIGN_FB_AMT_PARAM, "ASSIGN_FB_AMT", cols[0], yAsg);
		smallKnob(Hydra::ASSIGN_DW_AMT_PARAM, "ASSIGN_DW_AMT", cols[1], yAsg);
		smallKnob(Hydra::ASSIGN_RV_AMT_PARAM, "ASSIGN_RV_AMT", cols[2], yAsg);

		in(Hydra::IN_L_INPUT, "IN_L_INPUT", cols[0], yJackA);
		in(Hydra::IN_R_INPUT, "IN_R_INPUT", cols[1], yJackA);
		in(Hydra::FREEZE_GATE_INPUT, "FREEZE_GATE_INPUT", cols[2], yJackA);
		in(Hydra::SEED_INPUT, "SEED_IN_INPUT", cols[3], yJackA);
		in(Hydra::ASSIGN_CV_INPUT, "ASSIGN_CV_INPUT", cols[4], yJackA);
		out(Hydra::OUT_L_OUTPUT, "OUT_L_OUTPUT", cols[3], yJackB);
		out(Hydra::OUT_R_OUTPUT, "OUT_R_OUTPUT", cols[4], yJackB);
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Hydra* mod = dynamic_cast<Hydra*>(module);
		if (!mod) {
			return;
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createIndexSubmenuItem(
			"Quality",
			{"Bright digital (48k/16-bit)", "Cold digital (32k/12-bit)", "Sunny tape (24k/12-bit)", "Scorched cassette (24k/8-bit)"},
			[mod]() { return mod->quality; },
			[mod](int index) { mod->quality = clamp(index, 0, hydra::QUALITY_LAST - 1); }));
		menu->addChild(createCheckMenuItem("Latched seed", "",
			[mod]() { return mod->seedLatched; },
			[mod]() { mod->seedLatched = !mod->seedLatched; }));
		menu->addChild(createCheckMenuItem("Grain trigger on R output", "",
			[mod]() { return mod->gateOutput; },
			[mod]() { mod->gateOutput = !mod->gateOutput; }));
		menu->addChild(createCheckMenuItem("Wavetable mode when no input (10 s)", "",
			[mod]() { return mod->autoSynthMode; },
			[mod]() { mod->autoSynthMode = !mod->autoSynthMode; }));
		menu->addChild(createIndexSubmenuItem(
			"Assign CV destination",
			{"Feedback", "Dry/wet", "Reverb", "All (per-knob amounts)"},
			[mod]() { return mod->assignDest; },
			[mod](int index) { mod->assignDest = clamp(index, 0, 3); }));
	}
};

Model* modelHydra = createModel<Hydra, HydraWidget>("Hydra");
