#include "Doorstop.hpp"

#include <string>

namespace {

std::atomic<std::uint32_t> gDoorstopDebugInstanceCounter {1u};

const char* engineModeName(doorstop::EngineMode mode) {
	switch (mode) {
		case doorstop::EngineMode::ReferenceV1: return "referenceV1";
		case doorstop::EngineMode::ReferenceV2: return "referenceV2";
		case doorstop::EngineMode::ReferenceV3: return "referenceV3";
		case doorstop::EngineMode::Legacy: return "legacy";
		default: return "referenceV1";
	}
}

const char* soundModelName(doorstop::SoundModel model) {
	switch (model) {
		case doorstop::SoundModel::Classic: return "classic";
		case doorstop::SoundModel::CoupledBody: return "coupledBody";
		case doorstop::SoundModel::CoilContact: return "coilContact";
		case doorstop::SoundModel::DispersiveSpring: return "dispersiveSpring";
		case doorstop::SoundModel::ProbabilisticMix: return "probabilisticMix";
		default: return "probabilisticMix";
	}
}

const char* helicalTuningName(doorstop::HelicalTuningVariant variant) {
	switch (variant) {
		case doorstop::HelicalTuningVariant::BoingProbe: return "boingProbe";
		case doorstop::HelicalTuningVariant::DarkBoing: return "darkBoing";
		case doorstop::HelicalTuningVariant::DeepSwing: return "deepSwing";
		default: return "boingProbe";
	}
}

bool parseEngineMode(json_t* value, doorstop::EngineMode* mode) {
	if (!json_is_string(value) || !mode) return false;
	const std::string name = json_string_value(value);
	if (name == "referenceV1") {
		*mode = doorstop::EngineMode::ReferenceV1;
		return true;
	}
	if (name == "legacy") {
		*mode = doorstop::EngineMode::Legacy;
		return true;
	}
	if (name == "referenceV2") {
		*mode = doorstop::EngineMode::ReferenceV2;
		return true;
	}
	if (name == "referenceV3") {
		*mode = doorstop::EngineMode::ReferenceV3;
		return true;
	}
	return false;
}

bool parseSoundModel(json_t* value, doorstop::SoundModel* model) {
	if (!json_is_string(value) || !model) return false;
	const std::string name = json_string_value(value);
	if (name == "classic") *model = doorstop::SoundModel::Classic;
	else if (name == "coupledBody") *model = doorstop::SoundModel::CoupledBody;
	else if (name == "coilContact") *model = doorstop::SoundModel::CoilContact;
	else if (name == "dispersiveSpring") *model = doorstop::SoundModel::DispersiveSpring;
	else if (name == "probabilisticMix") *model = doorstop::SoundModel::ProbabilisticMix;
	else return false;
	return true;
}

bool parseHelicalTuning(json_t* value,
	doorstop::HelicalTuningVariant* variant) {
	if (!json_is_string(value) || !variant) return false;
	const std::string name = json_string_value(value);
	if (name == "boingProbe") {
		*variant = doorstop::HelicalTuningVariant::BoingProbe;
	}
	else if (name == "darkBoing") {
		*variant = doorstop::HelicalTuningVariant::DarkBoing;
	}
	else if (name == "deepSwing") {
		*variant = doorstop::HelicalTuningVariant::DeepSwing;
	}
	else return false;
	return true;
}

} // namespace

Doorstop::Doorstop() {
	debugMetrics.assignInstanceId(gDoorstopDebugInstanceCounter);
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configButton(MANUAL_PARAM, "Manual strike");
	configInput(TRIG_INPUT, "Trigger");
	configInput(VELOCITY_INPUT, "Bipolar velocity (bipolar +/-10V)");
	configOutput(AUDIO_OUTPUT, "Audio");

	std::uint32_t initialSeed = random::u32();
	if (!initialSeed) initialSeed = 1u;
	specimenSeed.store(initialSeed, std::memory_order_relaxed);
	pendingSpecimenSeed.store(initialSeed, std::memory_order_relaxed);
	engine.setSpecimenSeed(initialSeed);
	const float initialSampleRate = (APP && APP->engine) ? APP->engine->getSampleRate() : 44100.f;
	engine.setSampleRate(initialSampleRate);
}

void Doorstop::publishVisualState(const doorstop::Frame& frame) {
	const float maximumDisplacement = engine.getVisualMaximumDisplacement();
	const float displacement = std::isfinite(frame.displacement)
		? clamp(frame.displacement, -maximumDisplacement, maximumDisplacement)
		: 0.f;
	const float velocity = std::isfinite(frame.velocity) ? clamp(frame.velocity, -1.f, 1.f) : 0.f;
	const float physicalEnergy = std::isfinite(frame.energy)
		? clamp(frame.energy, 0.f, 1.f) : 0.f;
	const float perceptualActivity = std::isfinite(frame.visualActivity)
		? clamp(frame.visualActivity, 0.f, 1.f) : 0.f;
	const float strike = std::isfinite(frame.strikeLight) ? clamp(frame.strikeLight, 0.f, 1.f) : 0.f;
	visualDisplacement.store(displacement, std::memory_order_relaxed);
	visualVelocity.store(velocity, std::memory_order_relaxed);
	visualEnergy.store(
		std::max(physicalEnergy, perceptualActivity), std::memory_order_relaxed);
	visualStrike.store(strike, std::memory_order_relaxed);
}

void Doorstop::publishZeroVisualState() {
	visualDisplacement.store(0.f, std::memory_order_relaxed);
	visualVelocity.store(0.f, std::memory_order_relaxed);
	visualEnergy.store(0.f, std::memory_order_relaxed);
	visualStrike.store(0.f, std::memory_order_relaxed);
}

void Doorstop::process(const ProcessArgs& args) {
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto processStart = debug_terminal::debugTimerStart(measurePerf);
	bool persistentStateChanged = false;
	if (specimenStatePending.exchange(false, std::memory_order_acquire)) {
		const std::uint32_t loadedSeed =
			pendingSpecimenSeed.load(std::memory_order_relaxed);
		engine.setSpecimenSeed(loadedSeed);
		specimenSeed.store(engine.getSpecimenSeed(), std::memory_order_relaxed);
	}
	if (breakInStatePending.exchange(false, std::memory_order_acquire)) {
		engine.resetMotion();
		engine.setBreakIn(pendingBreakIn.load(std::memory_order_relaxed));
		persistentStateChanged = true;
	}
	if (newSpecimenRequested.exchange(false, std::memory_order_acq_rel)) {
		const std::uint32_t requestedSeed =
			pendingSpecimenSeed.load(std::memory_order_relaxed);
		engine.setSpecimenSeed(requestedSeed);
		specimenSeed.store(engine.getSpecimenSeed(), std::memory_order_relaxed);
		engine.restoreFactoryFresh();
		persistentStateChanged = true;
	}
	engine.setBreakInLocked(breakInLocked.load(std::memory_order_relaxed));
	if (restoreSpringRequested.exchange(false, std::memory_order_acq_rel)) {
		engine.restoreFactoryFresh();
		persistentStateChanged = true;
	}

	const int requestedEngineMode = clamp(
		engineMode.load(std::memory_order_acquire),
		int(doorstop::EngineMode::ReferenceV1),
		int(doorstop::EngineMode::Count) - 1);
	engine.setEngineMode(static_cast<doorstop::EngineMode>(requestedEngineMode));
	const int requestedModel = clamp(
		soundModel.load(std::memory_order_relaxed),
		int(doorstop::SoundModel::Classic),
		int(doorstop::SoundModel::Count) - 1);
	engine.setSoundModel(static_cast<doorstop::SoundModel>(requestedModel));
	const int requestedV3Tuning = clamp(
		referenceV3Tuning.load(std::memory_order_relaxed),
		int(doorstop::HelicalTuningVariant::BoingProbe),
		int(doorstop::HelicalTuningVariant::Count) - 1);
	engine.setReferenceV3TuningVariant(
		static_cast<doorstop::HelicalTuningVariant>(requestedV3Tuning));
	const bool externalStrike = trigTrigger.process(inputs[TRIG_INPUT].getVoltage(0), 0.1f, 1.f);
	const bool manualStrike = manualTrigger.process(params[MANUAL_PARAM].getValue(), 0.f, 1.f);
	bool appliedStrike = false;

	if (externalStrike) {
		const float velocityVoltage = inputs[VELOCITY_INPUT].isConnected()
			? inputs[VELOCITY_INPUT].getVoltage(0)
			: 5.f;
		const float normalizedVelocity = std::isfinite(velocityVoltage)
			? clamp(velocityVoltage / 10.f, -1.f, 1.f)
			: 0.f;
		if (normalizedVelocity != 0.f) {
			engine.strike(normalizedVelocity);
			appliedStrike = true;
		}
	}
	if (manualStrike) {
		float manualVelocity = 0.5f;
		if (manualVelocityPending.exchange(false, std::memory_order_acquire)) {
			manualVelocity = pendingManualVelocity.load(std::memory_order_relaxed);
		}
		engine.strike(clamp(manualVelocity, 0.1f, 1.f));
		appliedStrike = true;
	}

	const doorstop::Frame frame = engine.process(args.sampleTime);
	outputs[AUDIO_OUTPUT].setChannels(1);
	outputs[AUDIO_OUTPUT].setVoltage(frame.outputVolts);

	telemetryDivider = (telemetryDivider + 1u) & 63u;
	if (appliedStrike || frame.enteredSleep || telemetryDivider == 0u) {
		publishVisualState(frame);
	}
	if (persistentStateChanged || appliedStrike) {
		serializedBreakIn.store(engine.getBreakIn(), std::memory_order_relaxed);
	}
	if (appliedStrike) {
		visualLastStrikeModel.store(
			int(engine.getLastStrikeModel()), std::memory_order_relaxed);
	}
	if (measurePerf) {
		debugMetrics.recordProcess(debug_terminal::elapsedNsSince(processStart));
	}
}

void Doorstop::onReset(const ResetEvent& e) {
	(void) e;
	params[MANUAL_PARAM].setValue(0.f);
	trigTrigger.reset();
	manualTrigger.reset();
	engine.reset();
	allowVisualOverflow.store(true, std::memory_order_relaxed);
	engineMode.store(int(doorstop::EngineMode::ReferenceV1), std::memory_order_relaxed);
	soundModel.store(int(doorstop::SoundModel::ProbabilisticMix), std::memory_order_relaxed);
	referenceV3Tuning.store(
		int(doorstop::HelicalTuningVariant::BoingProbe), std::memory_order_relaxed);
	specimenStatePending.store(false, std::memory_order_relaxed);
	newSpecimenRequested.store(false, std::memory_order_relaxed);
	pendingSpecimenSeed.store(specimenSeed.load(std::memory_order_relaxed), std::memory_order_relaxed);
	breakInLocked.store(false, std::memory_order_relaxed);
	restoreSpringRequested.store(false, std::memory_order_relaxed);
	serializedBreakIn.store(0.f, std::memory_order_relaxed);
	pendingBreakIn.store(0.f, std::memory_order_relaxed);
	breakInStatePending.store(false, std::memory_order_relaxed);
	pendingManualVelocity.store(0.5f, std::memory_order_relaxed);
	manualVelocityPending.store(false, std::memory_order_relaxed);
	visualLastStrikeModel.store(
		int(doorstop::SoundModel::Classic), std::memory_order_relaxed);
	telemetryDivider = 0u;
	publishZeroVisualState();
}

void Doorstop::onSampleRateChange(const SampleRateChangeEvent& e) {
	engine.setSampleRate(e.sampleRate);
}

json_t* Doorstop::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "schema", json_integer(2));
	json_object_set_new(rootJ, "allowVisualOverflow",
		json_boolean(allowVisualOverflow.load(std::memory_order_relaxed)));
	const auto savedMode = static_cast<doorstop::EngineMode>(clamp(
		engineMode.load(std::memory_order_relaxed),
		int(doorstop::EngineMode::ReferenceV1),
		int(doorstop::EngineMode::Count) - 1));
	json_object_set_new(rootJ, "engineMode", json_string(engineModeName(savedMode)));
	const auto savedModel = static_cast<doorstop::SoundModel>(clamp(
		soundModel.load(std::memory_order_relaxed),
		int(doorstop::SoundModel::Classic),
		int(doorstop::SoundModel::Count) - 1));
	json_object_set_new(rootJ, "legacySoundModel",
		json_string(soundModelName(savedModel)));
	json_object_set_new(rootJ, "soundModel",
		json_integer(int(savedModel)));
	const auto savedV3Tuning = static_cast<doorstop::HelicalTuningVariant>(clamp(
		referenceV3Tuning.load(std::memory_order_relaxed),
		int(doorstop::HelicalTuningVariant::BoingProbe),
		int(doorstop::HelicalTuningVariant::Count) - 1));
	json_object_set_new(rootJ, "referenceV3Tuning",
		json_string(helicalTuningName(savedV3Tuning)));
	json_object_set_new(rootJ, "specimenSeed",
		json_integer(specimenSeed.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "breakIn",
		json_real(serializedBreakIn.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "breakInLocked",
		json_boolean(breakInLocked.load(std::memory_order_relaxed)));
	return rootJ;
}

void Doorstop::dataFromJson(json_t* rootJ) {
	trigTrigger.reset();
	manualTrigger.reset();
	publishZeroVisualState();

	bool loadedOverflow = true;
	doorstop::EngineMode loadedEngineMode = doorstop::EngineMode::Legacy;
	int loadedModel = int(doorstop::SoundModel::ProbabilisticMix);
	doorstop::HelicalTuningVariant loadedV3Tuning =
		doorstop::HelicalTuningVariant::BoingProbe;
	float loadedBreakIn = 0.f;
	bool loadedLocked = false;
	std::uint32_t loadedSeed = specimenSeed.load(std::memory_order_relaxed);

	if (rootJ) {
		json_t* overflowJ = json_object_get(rootJ, "allowVisualOverflow");
		if (json_is_boolean(overflowJ)) {
			loadedOverflow = json_boolean_value(overflowJ);
		}

		json_t* soundModelJ = json_object_get(rootJ, "soundModel");
		if (json_is_integer(soundModelJ)) {
			const json_int_t value = json_integer_value(soundModelJ);
			if (value >= int(doorstop::SoundModel::Classic)
				&& value < int(doorstop::SoundModel::Count)) {
				loadedModel = int(value);
			}
		}
		doorstop::EngineMode parsedMode;
		if (parseEngineMode(json_object_get(rootJ, "engineMode"), &parsedMode)) {
			loadedEngineMode = parsedMode;
		}
		doorstop::SoundModel parsedModel;
		if (parseSoundModel(json_object_get(rootJ, "legacySoundModel"), &parsedModel)) {
			loadedModel = int(parsedModel);
		}
		parseHelicalTuning(
			json_object_get(rootJ, "referenceV3Tuning"), &loadedV3Tuning);
		json_t* seedJ = json_object_get(rootJ, "specimenSeed");
		if (json_is_integer(seedJ)) {
			const json_int_t value = json_integer_value(seedJ);
			if (value > 0 && std::uint64_t(value) <= 0xffffffffull) {
				loadedSeed = std::uint32_t(value);
			}
		}

		json_t* breakInJ = json_object_get(rootJ, "breakIn");
		if (json_is_number(breakInJ)) {
			const double value = json_number_value(breakInJ);
			if (std::isfinite(value)) {
				loadedBreakIn = clamp(static_cast<float>(value), 0.f, 1.f);
			}
		}

		json_t* lockedJ = json_object_get(rootJ, "breakInLocked");
		if (json_is_boolean(lockedJ)) {
			loadedLocked = json_boolean_value(lockedJ);
		}
	}

	allowVisualOverflow.store(loadedOverflow, std::memory_order_relaxed);
	engineMode.store(int(loadedEngineMode), std::memory_order_relaxed);
	soundModel.store(loadedModel, std::memory_order_relaxed);
	referenceV3Tuning.store(int(loadedV3Tuning), std::memory_order_relaxed);
	specimenSeed.store(loadedSeed, std::memory_order_relaxed);
	pendingSpecimenSeed.store(loadedSeed, std::memory_order_relaxed);
	specimenStatePending.store(true, std::memory_order_release);
	newSpecimenRequested.store(false, std::memory_order_relaxed);
	breakInLocked.store(loadedLocked, std::memory_order_relaxed);
	restoreSpringRequested.store(false, std::memory_order_relaxed);
	serializedBreakIn.store(loadedBreakIn, std::memory_order_relaxed);
	pendingBreakIn.store(loadedBreakIn, std::memory_order_relaxed);
	pendingManualVelocity.store(0.5f, std::memory_order_relaxed);
	manualVelocityPending.store(false, std::memory_order_relaxed);
	visualLastStrikeModel.store(
		int(doorstop::SoundModel::Classic), std::memory_order_relaxed);

	breakInStatePending.store(true, std::memory_order_release);
}
