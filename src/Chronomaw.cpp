#include "Chronomaw.hpp"

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

} // namespace

Chronomaw::Chronomaw() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

	configParam(RUN_PARAM, 0.f, 1.f, 0.f, "Run");
	configParam(BPM_PARAM, chronomaw::kMinBpm, chronomaw::kMaxBpm, chronomaw::kDefaultBpm, "BPM");
	configParam(ACTIVE_BANK_PARAM, 0.f, float(chronomaw::kNumBanks - 1), 0.f, "Active bank");
	configParam(LOAD_BANK_PARAM, 0.f, 1.f, 0.f, "Load bank");
	configParam(SAVE_BANK_PARAM, 0.f, 1.f, 0.f, "Save bank");
	configParam(RESET_ALL_PARAM, 0.f, 1.f, 0.f, "Reset all");
	configParam(SELECTED_OUTPUT_PARAM, 1.f, float(chronomaw::kNumOutputs), 1.f, "Selected output");
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
}

void Chronomaw::onReset() {
	state = chronomaw::ModuleState();
	engine.reset();
}

void Chronomaw::process(const ProcessArgs& args) {
	if (runButtonEdge.process(params[RUN_PARAM].getValue())) {
		state.live.running = !state.live.running;
	}
	state.live.bpm = params[BPM_PARAM].getValue();
	state.live.activeBank = clamp(int(std::lround(params[ACTIVE_BANK_PARAM].getValue())), 0, chronomaw::kNumBanks - 1);
	state.ui.selectedOutput = clamp(int(std::lround(params[SELECTED_OUTPUT_PARAM].getValue())) - 1, 0, chronomaw::kNumOutputs - 1);
	state.live.density = chronomaw::DensityMode(clamp(int(std::lround(params[DENSITY_MODE_PARAM].getValue())), 0, 2));

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
	if (resetAllEdge.process(params[RESET_ALL_PARAM].getValue())) {
		onReset();
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

	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		outputs[OUT_1_OUTPUT + i].setVoltage(frameOut.outVolts[size_t(i)]);
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
	json_object_set_new(liveJ, "selectedOutput", json_integer(state.ui.selectedOutput));
	json_object_set_new(liveJ, "selectedTab", json_integer(state.ui.selectedTab));
	json_object_set_new(liveJ, "density", json_integer(int(state.live.density)));
	json_object_set_new(rootJ, "live", liveJ);

	json_t* banksJ = json_array();
	for (int b = 0; b < chronomaw::kNumBanks; ++b) {
		json_t* bankJ = json_object();
		json_object_set_new(bankJ, "bpm", json_real(state.banks[size_t(b)].bpm));
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
		state.ui.selectedOutput = clamp(jsonIntOr(liveJ, "selectedOutput", 0), 0, chronomaw::kNumOutputs - 1);
		state.ui.selectedTab = std::max(0, int(jsonIntOr(liveJ, "selectedTab", 0)));
		state.live.density = chronomaw::DensityMode(clamp(jsonIntOr(liveJ, "density", 0), 0, 2));
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
		}
	}

	params[BPM_PARAM].setValue(state.live.bpm);
	params[ACTIVE_BANK_PARAM].setValue(float(state.live.activeBank));
	params[SELECTED_OUTPUT_PARAM].setValue(float(state.ui.selectedOutput + 1));
	params[DENSITY_MODE_PARAM].setValue(float(int(state.live.density)));
}

