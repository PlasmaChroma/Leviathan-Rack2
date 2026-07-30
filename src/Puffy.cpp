#include "Puffy.hpp"

#include <algorithm>
#include <cmath>

Puffy::Puffy() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(
		CHARACTER_PARAM, 0.f, 2.f, 0.f, "Character",
		{"BLOOM", "SPINE", "FRENZY"});
	configParam(PUFF_PARAM, 0.f, 1.f, 0.25f, "Puff", "%", 0.f, 100.f);
	configParam(DEFLATE_PARAM, 0.f, 1.f, 0.f, "Deflate", " dB", 0.f, -12.f);
	configParam(
		PUFF_CV_AMOUNT_PARAM, -1.f, 1.f, 0.f,
		"Puff CV amount", "%", 0.f, 100.f);

	configInput(INPUT_L, "Left audio");
	configInput(INPUT_R, "Right audio");
	configInput(PUFF_CV_INPUT, "Puff CV");
	configOutput(OUTPUT_L, "Left audio");
	configOutput(OUTPUT_R, "Right audio");
	configLight(LIMIT_LIGHT, "Limiter gain reduction");
	configBypass(INPUT_L, OUTPUT_L);
	configBypass(INPUT_R, OUTPUT_R);
}

Puffy::~Puffy() {
	teardownTimer.begin(id);
}

void Puffy::publishVisualState(const puffy::Frame& frame) {
	const float gainReduction = std::max(
		0.f,
		std::min(
			-20.f * std::log10(std::max(frame.limiterGain, 1e-6f)) / 6.f,
			1.f));
	visualSequence.fetch_add(1u, std::memory_order_acq_rel);
	visualEffectiveAmount.store(frame.effectiveAmount, std::memory_order_relaxed);
	visualInputActivity.store(frame.inputActivity, std::memory_order_relaxed);
	visualTransientActivity.store(frame.transientActivity, std::memory_order_relaxed);
	visualGainReduction.store(gainReduction, std::memory_order_relaxed);
	visualCharacter.store(frame.character, std::memory_order_relaxed);
	visualSequence.fetch_add(1u, std::memory_order_release);
	lastGainReduction = gainReduction;
}

bool Puffy::readVisualState(PuffyVisualState* state) const {
	if (!state) {
		return false;
	}
	for (int attempt = 0; attempt < 4; ++attempt) {
		const std::uint32_t before =
			visualSequence.load(std::memory_order_acquire);
		if (before & 1u) {
			continue;
		}
		PuffyVisualState snapshot;
		snapshot.effectiveAmount =
			visualEffectiveAmount.load(std::memory_order_relaxed);
		snapshot.inputActivity =
			visualInputActivity.load(std::memory_order_relaxed);
		snapshot.transientActivity =
			visualTransientActivity.load(std::memory_order_relaxed);
		snapshot.gainReduction =
			visualGainReduction.load(std::memory_order_relaxed);
		snapshot.character = visualCharacter.load(std::memory_order_relaxed);
		const std::uint32_t after =
			visualSequence.load(std::memory_order_acquire);
		if (before == after && !(after & 1u)) {
			*state = snapshot;
			return true;
		}
	}
	return false;
}

void Puffy::process(const ProcessArgs& args) {
	const bool leftConnected = inputs[INPUT_L].isConnected();
	const bool rightConnected = inputs[INPUT_R].isConnected();
	float left = 0.f;
	float right = 0.f;
	if (leftConnected && rightConnected) {
		left = inputs[INPUT_L].getVoltage(0);
		right = inputs[INPUT_R].getVoltage(0);
	}
	else if (leftConnected) {
		left = inputs[INPUT_L].getVoltage(0);
		right = left;
	}
	else if (rightConnected) {
		right = inputs[INPUT_R].getVoltage(0);
		left = right;
	}

	const float puffCv = inputs[PUFF_CV_INPUT].isConnected()
		? inputs[PUFF_CV_INPUT].getVoltage(0)
		: 0.f;
	const float amountTarget = clamp(
		params[PUFF_PARAM].getValue()
			+ params[PUFF_CV_AMOUNT_PARAM].getValue() * puffCv / 10.f,
		0.f,
		1.f);
	const int character = clamp(
		int(std::lround(params[CHARACTER_PARAM].getValue())), 0, 2);
	const puffy::Frame frame = engine.process(
		left,
		right,
		amountTarget,
		character,
		autoDeflateEnabled.load(std::memory_order_relaxed),
		params[DEFLATE_PARAM].getValue());

	outputs[OUTPUT_L].setChannels(1);
	outputs[OUTPUT_R].setChannels(1);
	outputs[OUTPUT_L].setVoltage(frame.left, 0);
	outputs[OUTPUT_R].setVoltage(frame.right, 0);

	visualDivider++;
	if (visualDivider >= visualDivision) {
		visualDivider = 0u;
		publishVisualState(frame);
	}
	lights[LIMIT_LIGHT].setSmoothBrightness(lastGainReduction, args.sampleTime);
}

void Puffy::onReset(const ResetEvent& event) {
	(void) event;
	engine.reset();
	autoDeflateEnabled.store(true, std::memory_order_relaxed);
	visualDivider = 0u;
	lastGainReduction = 0.f;
	puffy::Frame frame;
	publishVisualState(frame);
	lights[LIMIT_LIGHT].setBrightness(0.f);
}

void Puffy::onSampleRateChange(const SampleRateChangeEvent& event) {
	engine.setSampleRate(event.sampleRate);
	visualDivision = std::max(
		std::uint32_t(1u),
		std::uint32_t(std::lround(event.sampleRate / 240.f)));
	visualDivider = 0u;
}

json_t* Puffy::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "schemaVersion", json_integer(1));
	json_object_set_new(root, "oversampling", json_integer(4));
	json_object_set_new(root, "limiterMode", json_string("live"));
	json_object_set_new(
		root,
		"autoDeflate",
		json_boolean(autoDeflateEnabled.load(std::memory_order_relaxed)));
	return root;
}

void Puffy::dataFromJson(json_t* root) {
	bool loadedAutoDeflate = true;
	if (root) {
		json_t* autoDeflate = json_object_get(root, "autoDeflate");
		if (json_is_boolean(autoDeflate)) {
			loadedAutoDeflate = json_boolean_value(autoDeflate);
		}
	}
	autoDeflateEnabled.store(loadedAutoDeflate, std::memory_order_relaxed);
	engine.reset();
	visualDivider = 0u;
	lastGainReduction = 0.f;
	puffy::Frame frame;
	publishVisualState(frame);
}
