#include "Chronomaw.hpp"
#include "ChronomawWaveforms.hpp"

namespace {

template <typename T>
static T jsonIntOr(json_t* rootJ, const char* key, T fallback) {
	json_t* valueJ = json_object_get(rootJ, key);
	if (!valueJ || !json_is_integer(valueJ)) {
		return fallback;
	}
	return T(json_integer_value(valueJ));
}

static float jsonFloatOr(json_t* rootJ, const char* key, float fallback) {
	json_t* valueJ = json_object_get(rootJ, key);
	if (!valueJ || !json_is_number(valueJ)) {
		return fallback;
	}
	return float(json_number_value(valueJ));
}

static json_t* outputStateToJson(const chronomaw::OutputState& out) {
	json_t* outJ = json_object();
	json_object_set_new(outJ, "muted", json_boolean(out.muted));
	json_object_set_new(outJ, "waveform", json_integer(int(out.waveform)));
	json_object_set_new(outJ, "modifierMode", json_integer(int(out.modifierMode)));
	json_object_set_new(outJ, "multiplier", json_real(out.multiplier));
	json_object_set_new(outJ, "widthPct", json_real(out.widthPct));
	json_object_set_new(outJ, "levelPct", json_real(out.levelPct));
	json_object_set_new(outJ, "offsetPct", json_real(out.offsetPct));
	json_object_set_new(outJ, "phasePct", json_real(out.phasePct));
	json_object_set_new(outJ, "swingPct", json_real(out.swingPct));
	json_object_set_new(outJ, "skewPct", json_real(out.skewPct));
	json_object_set_new(outJ, "rotatePct", json_real(out.rotatePct));
	json_object_set_new(outJ, "probabilityPct", json_real(out.probabilityPct));
	json_object_set_new(outJ, "invert", json_boolean(out.invert));
	json_object_set_new(outJ, "randomSeed", json_integer(out.randomSeed));
	return outJ;
}

static void outputStateFromJson(json_t* outJ, chronomaw::OutputState* out) {
	if (!out || !outJ || !json_is_object(outJ)) {
		return;
	}
	out->muted = json_is_true(json_object_get(outJ, "muted"));
	out->waveform = chronomaw::waveformFromIndex(jsonIntOr(outJ, "waveform", 0));
	out->multiplier = clamp(jsonFloatOr(outJ, "multiplier", 1.f), 1.f / 16384.f, 192.f);
	const int legacyMode = (out->multiplier < 1.f) ? int(chronomaw::ModifierMode::Div) : int(chronomaw::ModifierMode::Mult);
	out->modifierMode = chronomaw::ModifierMode(clamp(jsonIntOr(outJ, "modifierMode", legacyMode), 0, 2));
	out->widthPct = clamp(jsonFloatOr(outJ, "widthPct", 50.f), 0.f, 100.f);
	out->levelPct = clamp(jsonFloatOr(outJ, "levelPct", 100.f), 0.f, 100.f);
	out->offsetPct = clamp(jsonFloatOr(outJ, "offsetPct", 0.f), -100.f, 100.f);
	out->phasePct = clamp(jsonFloatOr(outJ, "phasePct", 0.f), -100.f, 100.f);
	out->swingPct = clamp(jsonFloatOr(outJ, "swingPct", 0.f), -100.f, 100.f);
	out->skewPct = clamp(jsonFloatOr(outJ, "skewPct", 0.f), -100.f, 100.f);
	out->rotatePct = clamp(jsonFloatOr(outJ, "rotatePct", 0.f), -100.f, 100.f);
	out->probabilityPct = clamp(jsonFloatOr(outJ, "probabilityPct", 100.f), 0.f, 100.f);
	out->invert = json_is_true(json_object_get(outJ, "invert"));
	out->randomSeed = uint32_t(std::max<int64_t>(0, jsonIntOr<int64_t>(outJ, "randomSeed", 0)));
}

static void writeOutputArray(json_t* parentJ, const char* key, const std::array<chronomaw::OutputState, chronomaw::kNumOutputs>& outputs) {
	json_t* outputsJ = json_array();
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		json_array_append_new(outputsJ, outputStateToJson(outputs[size_t(i)]));
	}
	json_object_set_new(parentJ, key, outputsJ);
}

static void readOutputArray(json_t* parentJ, const char* key, std::array<chronomaw::OutputState, chronomaw::kNumOutputs>* outputs) {
	if (!outputs || !parentJ) {
		return;
	}
	json_t* outputsJ = json_object_get(parentJ, key);
	if (!outputsJ || !json_is_array(outputsJ)) {
		return;
	}
	const size_t count = std::min<size_t>(json_array_size(outputsJ), size_t(chronomaw::kNumOutputs));
	for (size_t i = 0; i < count; ++i) {
		outputStateFromJson(json_array_get(outputsJ, i), &(*outputs)[i]);
	}
}

} // namespace

Chronomaw::Chronomaw() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

	configParam(RUN_PARAM, 0.f, 1.f, 0.f, "Run");
	configParam(BPM_PARAM, chronomaw::kMinBpm, chronomaw::kMaxBpm, chronomaw::kDefaultBpm, "BPM");
	configParam(ACTIVE_BANK_PARAM, 0.f, float(chronomaw::kNumBanks - 1), 0.f, "Active bank");
	paramQuantities[ACTIVE_BANK_PARAM]->snapEnabled = true;
	configParam(LOAD_BANK_PARAM, 0.f, 1.f, 0.f, "Load bank");
	configParam(SAVE_BANK_PARAM, 0.f, 1.f, 0.f, "Save bank");
	configParam(SELECTED_OUTPUT_PARAM, 1.f, float(chronomaw::kNumOutputs), 1.f, "Selected output");
	configParam(TIMELINE_ZOOM_PARAM, -1.f, 1.f, 0.f, "Timeline zoom");
	configParam(DENSITY_MODE_PARAM, 0.f, 2.f, 0.f, "Density mode");

	configInput(CLK_INPUT, "Clock");
	configInput(RUN_INPUT, "Run");
	configInput(RESET_INPUT, "Reset");
	configInput(CV_1_INPUT, "CV 1");
	configInput(CV_2_INPUT, "CV 2");
	configInput(CV_3_INPUT, "CV 3");
	configInput(CV_4_INPUT, "CV 4");

	configOutput(OUT_1_OUTPUT, "Output 1");
	configOutput(OUT_2_OUTPUT, "Output 2");
	configOutput(OUT_3_OUTPUT, "Output 3");
	configOutput(OUT_4_OUTPUT, "Output 4");
	configOutput(OUT_5_OUTPUT, "Output 5");
	configOutput(OUT_6_OUTPUT, "Output 6");
	configOutput(OUT_7_OUTPUT, "Output 7");
	configOutput(OUT_8_OUTPUT, "Output 8");

	for (int ch = 0; ch < chronomaw::kNumOutputs; ++ch) {
		for (int i = 0; i < kTimelineHistorySize; ++i) {
			timelineInternalHistory[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineInternalHistoryMin[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineInternalHistoryMax[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistory[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistoryMin[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistoryMax[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
		}
		for (int i = 0; i < kTimelineFutureSize; ++i) {
			timelineFutureOutput[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
		}
	}
}

void Chronomaw::onReset() {
	state = chronomaw::ModuleState();
	engine.reset();
	timelineWritePos.store(0, std::memory_order_relaxed);
	timelinePhaseBeats.store(0.f, std::memory_order_relaxed);
	timelineBpm.store(chronomaw::kDefaultBpm, std::memory_order_relaxed);
	timelineCycleCount.store(0u, std::memory_order_relaxed);
	timelineRunning.store(false, std::memory_order_relaxed);
	timelineCaptureElapsedSec = 0.f;
	timelineAccumSamples = 0;
	for (int ch = 0; ch < chronomaw::kNumOutputs; ++ch) {
		timelineInternalAccum[size_t(ch)] = 0.f;
		timelineOutputAccum[size_t(ch)] = 0.f;
		timelineInternalAccumMin[size_t(ch)] = 0.f;
		timelineInternalAccumMax[size_t(ch)] = 0.f;
		timelineOutputAccumMin[size_t(ch)] = 0.f;
		timelineOutputAccumMax[size_t(ch)] = 0.f;
		timelineTimingPhaseOffsets[size_t(ch)].store(0.f, std::memory_order_relaxed);
		for (int i = 0; i < kTimelineHistorySize; ++i) {
			timelineInternalHistory[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineInternalHistoryMin[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineInternalHistoryMax[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistory[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistoryMin[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
			timelineOutputHistoryMax[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
		}
		for (int i = 0; i < kTimelineFutureSize; ++i) {
			timelineFutureOutput[size_t(ch)][size_t(i)].store(0.f, std::memory_order_relaxed);
		}
	}
}

void Chronomaw::process(const ProcessArgs& args) {
	if (runButtonEdge.process(params[RUN_PARAM].getValue())) {
		state.live.running = !state.live.running;
	}
	state.live.bpm = params[BPM_PARAM].getValue();
	state.live.activeBank = clamp(int(std::lround(params[ACTIVE_BANK_PARAM].getValue())), 0, chronomaw::kNumBanks - 1);
	state.ui.selectedOutput = clamp(int(std::lround(params[SELECTED_OUTPUT_PARAM].getValue())) - 1, 0, chronomaw::kNumOutputs - 1);
	state.live.density = chronomaw::DensityMode::Monitor;
	params[DENSITY_MODE_PARAM].setValue(0.f);

	const int bank = state.live.activeBank;
	if (saveBankEdge.process(params[SAVE_BANK_PARAM].getValue())) {
		state.banks[size_t(bank)].bpm = state.live.bpm;
		state.banks[size_t(bank)].outputs = state.live.outputs;
	}
	if (loadBankEdge.process(params[LOAD_BANK_PARAM].getValue())) {
		state.live.bpm = state.banks[size_t(bank)].bpm;
		state.live.outputs = state.banks[size_t(bank)].outputs;
		params[BPM_PARAM].setValue(state.live.bpm);
	}

	chronomaw::FrameInputs in;
	in.sampleTime = args.sampleTime;
	in.clkConnected = inputs[CLK_INPUT].isConnected();
	in.clkVoltage = inputs[CLK_INPUT].getVoltage();
	in.runConnected = inputs[RUN_INPUT].isConnected();
	in.runVoltage = inputs[RUN_INPUT].getVoltage();
	in.resetConnected = inputs[RESET_INPUT].isConnected();
	in.resetVoltage = inputs[RESET_INPUT].getVoltage();
	engine.process(in, state.live, &frameOut);
	timelinePhaseBeats.store(frameOut.phaseBeats, std::memory_order_relaxed);
	timelineBpm.store(state.live.bpm, std::memory_order_relaxed);
	timelineCycleCount.store(frameOut.cycleCount, std::memory_order_relaxed);
	timelineRunning.store(frameOut.running, std::memory_order_relaxed);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		timelineTimingPhaseOffsets[size_t(i)].store(float(frameOut.timingPhaseOffsets[size_t(i)]), std::memory_order_relaxed);
	}

	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		outputs[OUT_1_OUTPUT + i].setVoltage(frameOut.outVolts[size_t(i)]);
		const float internalV = frameOut.internalVolts[size_t(i)];
		const float outputV = frameOut.outVolts[size_t(i)];
		timelineInternalAccum[size_t(i)] += internalV;
		timelineOutputAccum[size_t(i)] += outputV;
		if (timelineAccumSamples == 0) {
			timelineInternalAccumMin[size_t(i)] = internalV;
			timelineInternalAccumMax[size_t(i)] = internalV;
			timelineOutputAccumMin[size_t(i)] = outputV;
			timelineOutputAccumMax[size_t(i)] = outputV;
		}
		else {
			timelineInternalAccumMin[size_t(i)] = std::min(timelineInternalAccumMin[size_t(i)], internalV);
			timelineInternalAccumMax[size_t(i)] = std::max(timelineInternalAccumMax[size_t(i)], internalV);
			timelineOutputAccumMin[size_t(i)] = std::min(timelineOutputAccumMin[size_t(i)], outputV);
			timelineOutputAccumMax[size_t(i)] = std::max(timelineOutputAccumMax[size_t(i)], outputV);
		}
	}
	timelineAccumSamples++;

	timelineCaptureElapsedSec += args.sampleTime;
	if (timelineCaptureElapsedSec >= kTimelineCaptureIntervalSec) {
		timelineCaptureElapsedSec -= kTimelineCaptureIntervalSec;
		const int writePos = timelineWritePos.load(std::memory_order_relaxed);
		const float invSamples = (timelineAccumSamples > 0) ? (1.f / float(timelineAccumSamples)) : 1.f;
		for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
			timelineInternalHistory[size_t(i)][size_t(writePos)].store(timelineInternalAccum[size_t(i)] * invSamples, std::memory_order_relaxed);
			timelineInternalHistoryMin[size_t(i)][size_t(writePos)].store(timelineInternalAccumMin[size_t(i)], std::memory_order_relaxed);
			timelineInternalHistoryMax[size_t(i)][size_t(writePos)].store(timelineInternalAccumMax[size_t(i)], std::memory_order_relaxed);
			timelineOutputHistory[size_t(i)][size_t(writePos)].store(timelineOutputAccum[size_t(i)] * invSamples, std::memory_order_relaxed);
			timelineOutputHistoryMin[size_t(i)][size_t(writePos)].store(timelineOutputAccumMin[size_t(i)], std::memory_order_relaxed);
			timelineOutputHistoryMax[size_t(i)][size_t(writePos)].store(timelineOutputAccumMax[size_t(i)], std::memory_order_relaxed);
			timelineInternalAccum[size_t(i)] = 0.f;
			timelineOutputAccum[size_t(i)] = 0.f;
			timelineInternalAccumMin[size_t(i)] = 0.f;
			timelineInternalAccumMax[size_t(i)] = 0.f;
			timelineOutputAccumMin[size_t(i)] = 0.f;
			timelineOutputAccumMax[size_t(i)] = 0.f;
		}
		timelineAccumSamples = 0;
		timelineWritePos.store((writePos + 1) % kTimelineHistorySize, std::memory_order_relaxed);
	}

	lights[RUN_LIGHT].setBrightness(state.live.running ? 1.f : 0.f);
	lights[SYNC_LIGHT].setBrightness(inputs[CLK_INPUT].isConnected() ? 0.6f : 0.f);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		float v = frameOut.outVolts[size_t(i)];
		lights[OUT_1_LIGHT + i].setBrightness(clamp(v / chronomaw::kOutputMaxV, 0.f, 1.f));
	}
}

json_t* Chronomaw::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "schemaVersion", json_integer(chronomaw::ModuleState::kSchemaVersion));

	json_t* liveJ = json_object();
	json_object_set_new(liveJ, "bpm", json_real(state.live.bpm));
	json_object_set_new(liveJ, "running", json_boolean(state.live.running));
	json_object_set_new(liveJ, "activeBank", json_integer(state.live.activeBank));
	json_object_set_new(liveJ, "density", json_integer(int(state.live.density)));
	writeOutputArray(liveJ, "outputs", state.live.outputs);
	json_object_set_new(rootJ, "live", liveJ);

	json_t* uiJ = json_object();
	json_object_set_new(uiJ, "selectedOutput", json_integer(state.ui.selectedOutput));
	json_object_set_new(uiJ, "selectedTab", json_integer(state.ui.selectedTab));
	json_object_set_new(rootJ, "ui", uiJ);

	json_t* banksJ = json_array();
	for (int b = 0; b < chronomaw::kNumBanks; ++b) {
		json_t* bankJ = json_object();
		json_object_set_new(bankJ, "bpm", json_real(state.banks[size_t(b)].bpm));
		writeOutputArray(bankJ, "outputs", state.banks[size_t(b)].outputs);
		json_array_append_new(banksJ, bankJ);
	}
	json_object_set_new(rootJ, "banks", banksJ);
	return rootJ;
}

void Chronomaw::dataFromJson(json_t* rootJ) {
	if (!rootJ || !json_is_object(rootJ)) {
		return;
	}
	json_t* liveJ = json_object_get(rootJ, "live");
	if (liveJ && json_is_object(liveJ)) {
		state.live.bpm = clamp(jsonFloatOr(liveJ, "bpm", chronomaw::kDefaultBpm), chronomaw::kMinBpm, chronomaw::kMaxBpm);
		state.live.running = json_is_true(json_object_get(liveJ, "running"));
		state.live.activeBank = clamp(jsonIntOr(liveJ, "activeBank", 0), 0, chronomaw::kNumBanks - 1);
			state.live.density = chronomaw::DensityMode::Monitor;
		readOutputArray(liveJ, "outputs", &state.live.outputs);
		// Backward compatibility with early scaffold JSON where ui lived under "live".
		state.ui.selectedOutput = clamp(jsonIntOr(liveJ, "selectedOutput", state.ui.selectedOutput), 0, chronomaw::kNumOutputs - 1);
		state.ui.selectedTab = std::max(0, int(jsonIntOr(liveJ, "selectedTab", state.ui.selectedTab)));
	}

	json_t* uiJ = json_object_get(rootJ, "ui");
	if (uiJ && json_is_object(uiJ)) {
		state.ui.selectedOutput = clamp(jsonIntOr(uiJ, "selectedOutput", state.ui.selectedOutput), 0, chronomaw::kNumOutputs - 1);
		state.ui.selectedTab = std::max(0, int(jsonIntOr(uiJ, "selectedTab", state.ui.selectedTab)));
	}

	json_t* banksJ = json_object_get(rootJ, "banks");
	if (banksJ && json_is_array(banksJ)) {
		const size_t count = std::min<size_t>(json_array_size(banksJ), size_t(chronomaw::kNumBanks));
		for (size_t b = 0; b < count; ++b) {
			json_t* bankJ = json_array_get(banksJ, b);
			if (!bankJ || !json_is_object(bankJ)) {
				continue;
			}
			state.banks[b].bpm = clamp(jsonFloatOr(bankJ, "bpm", chronomaw::kDefaultBpm), chronomaw::kMinBpm, chronomaw::kMaxBpm);
			readOutputArray(bankJ, "outputs", &state.banks[b].outputs);
		}
	}

	params[BPM_PARAM].setValue(state.live.bpm);
	params[ACTIVE_BANK_PARAM].setValue(float(state.live.activeBank));
	params[SELECTED_OUTPUT_PARAM].setValue(float(state.ui.selectedOutput + 1));
	params[DENSITY_MODE_PARAM].setValue(0.f);
}
