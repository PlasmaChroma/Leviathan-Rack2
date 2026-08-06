#include "Puffy.hpp"

#include <algorithm>
#include <cmath>

namespace {

std::atomic<std::uint32_t> gPuffyDebugInstanceCounter {1u};
std::atomic<std::uint32_t> gPuffySwarmInstanceCounter {1u};

std::uint32_t makePuffySwarmSeed() {
	std::uint32_t value = gPuffySwarmInstanceCounter.fetch_add(
		1u, std::memory_order_relaxed);
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return value != 0u ? value : 0x6d2b79f5u;
}

} // namespace

Puffy::Puffy() {
	debugMetrics.assignInstanceId(gPuffyDebugInstanceCounter);
	engine.setSwarmSeed(makePuffySwarmSeed());
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(
		CHARACTER_PARAM, 0.f, float(puffy::kCharacterCount - 1), 0.f,
		"Character (negative/linked)",
		{"BLOOM", "SPINE", "FRENZY", "RIPTIDE", "VOID", "SWARM", "TEETH"});
	configParam(PUFF_PARAM, 0.f, 1.f, 0.25f, "Puff", "%", 0.f, 100.f);
	configParam(
		SENSITIVITY_PARAM, -1.f, 1.f, 0.f,
		"Sensitivity", " dB", 0.f, 6.0205999f);
	configParam(
		PUFF_CV_AMOUNT_PARAM, -1.f, 1.f, 0.f,
		"Puff CV amount", "%", 0.f, 100.f);
	configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix", "%", 0.f, 100.f);
	configSwitch(
		POSITIVE_CHARACTER_PARAM, 0.f, float(puffy::kCharacterCount - 1), 0.f,
		"Character (positive)",
		{"BLOOM", "SPINE", "FRENZY", "RIPTIDE", "VOID", "SWARM", "TEETH"});
	configSwitch(
		CHARACTER_LINK_PARAM, 0.f, 1.f, 1.f, "Link polarity characters",
		{"Unlinked", "Linked"});

	configInput(INPUT_L, "Left audio");
	configInput(INPUT_R, "Right audio");
	configInput(PUFF_CV_INPUT, "Puff CV");
	configOutput(OUTPUT_L, "Left audio");
	configOutput(OUTPUT_R, "Right audio");
	configLight(LIMIT_LIGHT, "Limiter gain reduction");
	configLight(CHARACTER_LINK_LIGHT, "Polarity character link");
	configBypass(INPUT_L, OUTPUT_L);
	configBypass(INPUT_R, OUTPUT_R);
}

Puffy::~Puffy() {
	teardownTimer.begin(id);
}

void Puffy::publishVisualState(
	const puffy::Frame& frame,
	bool stereoInputsConnected) {
	const float gainReduction = std::max(
		0.f,
		std::min(
			-20.f * std::log10(std::max(frame.limiterGain, 1e-6f)) / 6.f,
			1.f));
	visualSequence.fetch_add(1u, std::memory_order_acq_rel);
	visualEffectiveAmount.store(frame.effectiveAmount, std::memory_order_relaxed);
	visualWetMix.store(frame.wetMix, std::memory_order_relaxed);
	visualInputActivity.store(frame.inputActivity, std::memory_order_relaxed);
	visualPositiveInputActivity.store(
		frame.positiveInputActivity, std::memory_order_relaxed);
	visualNegativeInputActivity.store(
		frame.negativeInputActivity, std::memory_order_relaxed);
	visualLeftPositiveInputActivity.store(
		frame.leftPositiveInputActivity, std::memory_order_relaxed);
	visualLeftNegativeInputActivity.store(
		frame.leftNegativeInputActivity, std::memory_order_relaxed);
	visualRightPositiveInputActivity.store(
		frame.rightPositiveInputActivity, std::memory_order_relaxed);
	visualRightNegativeInputActivity.store(
		frame.rightNegativeInputActivity, std::memory_order_relaxed);
	visualTransientActivity.store(frame.transientActivity, std::memory_order_relaxed);
	visualGainReduction.store(gainReduction, std::memory_order_relaxed);
	visualNegativeCharacter.store(
		frame.negativeCharacter, std::memory_order_relaxed);
	visualPositiveCharacter.store(
		frame.positiveCharacter, std::memory_order_relaxed);
	visualCharactersLinked.store(
		params[CHARACTER_LINK_PARAM].getValue() > 0.5f,
		std::memory_order_relaxed);
	visualStereoInputsConnected.store(
		stereoInputsConnected, std::memory_order_relaxed);
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
		snapshot.wetMix = visualWetMix.load(std::memory_order_relaxed);
		snapshot.inputActivity =
			visualInputActivity.load(std::memory_order_relaxed);
		snapshot.positiveInputActivity =
			visualPositiveInputActivity.load(std::memory_order_relaxed);
		snapshot.negativeInputActivity =
			visualNegativeInputActivity.load(std::memory_order_relaxed);
		snapshot.leftPositiveInputActivity =
			visualLeftPositiveInputActivity.load(std::memory_order_relaxed);
		snapshot.leftNegativeInputActivity =
			visualLeftNegativeInputActivity.load(std::memory_order_relaxed);
		snapshot.rightPositiveInputActivity =
			visualRightPositiveInputActivity.load(std::memory_order_relaxed);
		snapshot.rightNegativeInputActivity =
			visualRightNegativeInputActivity.load(std::memory_order_relaxed);
		snapshot.transientActivity =
			visualTransientActivity.load(std::memory_order_relaxed);
		snapshot.gainReduction =
			visualGainReduction.load(std::memory_order_relaxed);
		snapshot.negativeCharacter =
			visualNegativeCharacter.load(std::memory_order_relaxed);
		snapshot.positiveCharacter =
			visualPositiveCharacter.load(std::memory_order_relaxed);
		snapshot.charactersLinked =
			visualCharactersLinked.load(std::memory_order_relaxed);
		snapshot.stereoInputsConnected =
			visualStereoInputsConnected.load(std::memory_order_relaxed);
		const std::uint32_t after =
			visualSequence.load(std::memory_order_acquire);
		if (before == after && !(after & 1u)) {
			*state = snapshot;
			return true;
		}
	}
	return false;
}

void Puffy::synchronizeCharacterSelectionFromUi(bool negativeIsSource) {
	if (params[CHARACTER_LINK_PARAM].getValue() <= 0.5f) {
		return;
	}
	const int sourceId = negativeIsSource
		? CHARACTER_PARAM
		: POSITIVE_CHARACTER_PARAM;
	const int destinationId = negativeIsSource
		? POSITIVE_CHARACTER_PARAM
		: CHARACTER_PARAM;
	params[destinationId].setValue(params[sourceId].getValue());
}

void Puffy::process(const ProcessArgs& args) {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto processStart = debug_terminal::debugTimerStart(measurePerf);
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
	const int negativeCharacter = clamp(
		int(std::lround(params[CHARACTER_PARAM].getValue())),
		0,
		puffy::kCharacterCount - 1);
	const bool charactersLinked =
		params[CHARACTER_LINK_PARAM].getValue() > 0.5f;
	if (charactersLinked
		&& params[POSITIVE_CHARACTER_PARAM].getValue()
			!= params[CHARACTER_PARAM].getValue()) {
		params[POSITIVE_CHARACTER_PARAM].setValue(
			params[CHARACTER_PARAM].getValue());
	}
	const int positiveCharacter = charactersLinked
		? negativeCharacter
		: clamp(
			int(std::lround(
				params[POSITIVE_CHARACTER_PARAM].getValue())),
			0,
			puffy::kCharacterCount - 1);
	const puffy::Frame frame = engine.process(
		left,
		right,
		amountTarget,
		negativeCharacter,
		positiveCharacter,
		autoDeflateEnabled.load(std::memory_order_relaxed),
		params[SENSITIVITY_PARAM].getValue(),
		params[MIX_PARAM].getValue(),
		leftConnected && rightConnected);

	outputs[OUTPUT_L].setChannels(1);
	outputs[OUTPUT_R].setChannels(1);
	outputs[OUTPUT_L].setVoltage(frame.left, 0);
	outputs[OUTPUT_R].setVoltage(frame.right, 0);

	visualDivider++;
	if (visualDivider >= visualDivision) {
		visualDivider = 0u;
		publishVisualState(frame, leftConnected && rightConnected);
	}
	lights[LIMIT_LIGHT].setSmoothBrightness(lastGainReduction, args.sampleTime);
	lights[CHARACTER_LINK_LIGHT].setBrightness(charactersLinked ? 1.f : 0.f);
	if (measurePerf) {
		debugMetrics.recordProcess(
			debug_terminal::elapsedNsSince(processStart));
	}
}

void Puffy::onReset(const ResetEvent& event) {
	(void) event;
	engine.reset();
	autoDeflateEnabled.store(false, std::memory_order_relaxed);
	params[CHARACTER_LINK_PARAM].setValue(1.f);
	params[POSITIVE_CHARACTER_PARAM].setValue(
		params[CHARACTER_PARAM].getValue());
	visualDivider = 0u;
	lastGainReduction = 0.f;
	puffy::Frame frame;
	publishVisualState(frame, false);
	lights[LIMIT_LIGHT].setBrightness(0.f);
	lights[CHARACTER_LINK_LIGHT].setBrightness(1.f);
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
	if (params[CHARACTER_LINK_PARAM].getValue() > 0.5f) {
		params[POSITIVE_CHARACTER_PARAM].setValue(
			params[CHARACTER_PARAM].getValue());
	}
	engine.reset();
	visualDivider = 0u;
	lastGainReduction = 0.f;
	puffy::Frame frame;
	publishVisualState(frame, false);
}
