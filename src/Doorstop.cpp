#include "Doorstop.hpp"

Doorstop::Doorstop() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configButton(MANUAL_PARAM, "Manual strike");
	configInput(TRIG_INPUT, "Trigger");
	configInput(VELOCITY_INPUT, "Bipolar velocity");
	configOutput(AUDIO_OUTPUT, "Audio");
	configLight(STRIKE_LIGHT, "Strike");
}

void Doorstop::publishVisualState(const doorstop::Frame& frame) {
	const float maximumDisplacement = engine.getEffectiveTuning().maxDisplacement;
	const float displacement = std::isfinite(frame.displacement)
		? clamp(frame.displacement, -maximumDisplacement, maximumDisplacement)
		: 0.f;
	const float velocity = std::isfinite(frame.velocity) ? clamp(frame.velocity, -1.f, 1.f) : 0.f;
	const float energy = std::isfinite(frame.energy) ? clamp(frame.energy, 0.f, 1.f) : 0.f;
	const float strike = std::isfinite(frame.strikeLight) ? clamp(frame.strikeLight, 0.f, 1.f) : 0.f;
	visualDisplacement.store(displacement, std::memory_order_relaxed);
	visualVelocity.store(velocity, std::memory_order_relaxed);
	visualEnergy.store(energy, std::memory_order_relaxed);
	visualStrike.store(strike, std::memory_order_relaxed);
}

void Doorstop::publishZeroVisualState() {
	visualDisplacement.store(0.f, std::memory_order_relaxed);
	visualVelocity.store(0.f, std::memory_order_relaxed);
	visualEnergy.store(0.f, std::memory_order_relaxed);
	visualStrike.store(0.f, std::memory_order_relaxed);
}

void Doorstop::process(const ProcessArgs& args) {
	bool persistentStateChanged = false;
	if (breakInStatePending.exchange(false, std::memory_order_acquire)) {
		engine.resetMotion();
		engine.setBreakIn(pendingBreakIn.load(std::memory_order_relaxed));
		persistentStateChanged = true;
	}
	engine.setBreakInLocked(breakInLocked.load(std::memory_order_relaxed));
	if (restoreSpringRequested.exchange(false, std::memory_order_acq_rel)) {
		engine.restoreFactoryFresh();
		persistentStateChanged = true;
	}

	const int requestedModel = clamp(
		soundModel.load(std::memory_order_relaxed),
		int(doorstop::SoundModel::Classic),
		int(doorstop::SoundModel::Count) - 1);
	engine.setSoundModel(static_cast<doorstop::SoundModel>(requestedModel));

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
	if (frame.enteredSleep) {
		lights[STRIKE_LIGHT].setBrightness(0.f);
	}
	else {
		lights[STRIKE_LIGHT].setBrightnessSmooth(clamp(frame.strikeLight, 0.f, 1.f), args.sampleTime);
	}

	telemetryDivider = (telemetryDivider + 1u) & 63u;
	if (appliedStrike || frame.enteredSleep || telemetryDivider == 0u) {
		publishVisualState(frame);
	}
	if (persistentStateChanged || appliedStrike) {
		serializedBreakIn.store(engine.getBreakIn(), std::memory_order_relaxed);
	}
}

void Doorstop::onReset(const ResetEvent& e) {
	(void) e;
	params[MANUAL_PARAM].setValue(0.f);
	trigTrigger.reset();
	manualTrigger.reset();
	engine.reset();
	allowVisualOverflow.store(true, std::memory_order_relaxed);
	soundModel.store(int(doorstop::SoundModel::ProbabilisticMix), std::memory_order_relaxed);
	breakInLocked.store(false, std::memory_order_relaxed);
	restoreSpringRequested.store(false, std::memory_order_relaxed);
	serializedBreakIn.store(0.f, std::memory_order_relaxed);
	pendingBreakIn.store(0.f, std::memory_order_relaxed);
	breakInStatePending.store(false, std::memory_order_relaxed);
	pendingManualVelocity.store(0.5f, std::memory_order_relaxed);
	manualVelocityPending.store(false, std::memory_order_relaxed);
	telemetryDivider = 0u;
	lights[STRIKE_LIGHT].setBrightness(0.f);
	publishZeroVisualState();
}

void Doorstop::onSampleRateChange(const SampleRateChangeEvent& e) {
	engine.setSampleRate(e.sampleRate);
}

json_t* Doorstop::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "allowVisualOverflow",
		json_boolean(allowVisualOverflow.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "soundModel",
		json_integer(soundModel.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "breakIn",
		json_real(serializedBreakIn.load(std::memory_order_relaxed)));
	json_object_set_new(rootJ, "breakInLocked",
		json_boolean(breakInLocked.load(std::memory_order_relaxed)));
	return rootJ;
}

void Doorstop::dataFromJson(json_t* rootJ) {
	trigTrigger.reset();
	manualTrigger.reset();
	lights[STRIKE_LIGHT].setBrightness(0.f);
	publishZeroVisualState();
	allowVisualOverflow.store(true, std::memory_order_relaxed);
	soundModel.store(int(doorstop::SoundModel::ProbabilisticMix), std::memory_order_relaxed);
	breakInLocked.store(false, std::memory_order_relaxed);
	restoreSpringRequested.store(false, std::memory_order_relaxed);
	serializedBreakIn.store(0.f, std::memory_order_relaxed);
	pendingBreakIn.store(0.f, std::memory_order_relaxed);
	pendingManualVelocity.store(0.5f, std::memory_order_relaxed);
	manualVelocityPending.store(false, std::memory_order_relaxed);
	if (rootJ) {
		json_t* overflowJ = json_object_get(rootJ, "allowVisualOverflow");
		if (json_is_boolean(overflowJ)) {
			allowVisualOverflow.store(json_boolean_value(overflowJ), std::memory_order_relaxed);
		}
		json_t* soundModelJ = json_object_get(rootJ, "soundModel");
		if (json_is_integer(soundModelJ)) {
			const json_int_t value = json_integer_value(soundModelJ);
			if (value >= int(doorstop::SoundModel::Classic)
				&& value < int(doorstop::SoundModel::Count)) {
				soundModel.store(int(value), std::memory_order_relaxed);
			}
		}
		json_t* breakInJ = json_object_get(rootJ, "breakIn");
		if (json_is_number(breakInJ)) {
			const double value = json_number_value(breakInJ);
			if (std::isfinite(value)) {
				const float restored = clamp(static_cast<float>(value), 0.f, 1.f);
				pendingBreakIn.store(restored, std::memory_order_relaxed);
				serializedBreakIn.store(restored, std::memory_order_relaxed);
			}
		}
		json_t* lockedJ = json_object_get(rootJ, "breakInLocked");
		if (json_is_boolean(lockedJ)) {
			breakInLocked.store(
				json_boolean_value(lockedJ), std::memory_order_relaxed);
		}
	}
	breakInStatePending.store(true, std::memory_order_release);
}
