#include "CrownstepShared.hpp"

int Crownstep::activeLength() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	return crownstep::activeLength(int(history.size()), currentSequenceCap());
}

int Crownstep::activeStartIndex() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	return crownstep::activeStartIndex(int(history.size()), currentSequenceCap());
}

float Crownstep::pitchForSequenceIndex(int sequenceIndex) {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	if (sequenceIndex < 0) {
		return 0.f;
	}

	// Preferred path: derive pitch from move sequence so interpretation
	// and quantization settings are applied live at playback time.
	if (sequenceIndex < int(moveHistory.size())) {
		const Move& move = moveHistory[size_t(sequenceIndex)];
		return pitchForMove(move);
	}

	// Backward compatibility for older saves that may not contain moveHistory.
	if (sequenceIndex < int(history.size())) {
		return history[size_t(sequenceIndex)].pitch;
	}

	return 0.f;
}

void Crownstep::refreshHeldPitchForCurrentStep() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	int historySize = int(history.size());
	int sequenceCap = currentSequenceCap();
	int length = crownstep::activeLength(historySize, sequenceCap);
	if (length <= 0) {
		heldPitch = NO_SEQUENCE_PITCH_VOLTS;
		return;
	}
	if (displayedStep <= 0) {
		return;
	}
	int shownStep = clamp(displayedStep, 1, length);
	int sequenceIndex = crownstep::activeStartIndex(historySize, sequenceCap) + (shownStep - 1);
	heldPitch = pitchForSequenceIndex(sequenceIndex);
}

void Crownstep::emitStepAtClockEdge() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	int length = activeLength();
	if (length <= 0) {
		displayedStep = 0;
		heldPitch = NO_SEQUENCE_PITCH_VOLTS;
		heldAccent = 0.f;
		heldMod = 0.f;
		modOutputVolts = 0.f;
		modGlideStartVolts = 0.f;
		modGlideTargetVolts = 0.f;
		modGlideDurationSeconds = 0.f;
		modGlideActive = false;
		playhead = 0;
		return;
	}

	playhead = clamp(playhead, 0, std::max(length - 1, 0));
	displayedStep = playhead + 1;
	int sequenceIndex = activeStartIndex() + playhead;
	const Step& step = history[size_t(sequenceIndex)];
	heldPitch = pitchForSequenceIndex(sequenceIndex);
	heldAccent = step.accent;
	heldMod = step.mod * 10.f;
	if (previousClockPeriodSeconds > 0.f) {
		modGlideStartVolts = modOutputVolts;
		modGlideTargetVolts = heldMod;
		modGlideStartSeconds = transportTimeSeconds;
		modGlideDurationSeconds = previousClockPeriodSeconds;
		modGlideActive = std::fabs(modGlideTargetVolts - modGlideStartVolts) > 1e-6f;
		if (!modGlideActive) {
			modOutputVolts = modGlideTargetVolts;
		}
	}
	else {
		// Before we have a valid clock period, snap directly.
		modGlideStartVolts = heldMod;
		modGlideTargetVolts = heldMod;
		modGlideStartSeconds = transportTimeSeconds;
		modGlideDurationSeconds = 0.f;
		modGlideActive = false;
		modOutputVolts = heldMod;
	}

	playhead++;
	if (playhead >= length) {
		playhead = 0;
		eocPulse.trigger(1e-3f);
	}
}

void Crownstep::process(const ProcessArgs& args) {
	transportTimeSeconds += args.sampleTime;

	if (newGameTrigger.process(params[NEW_GAME_PARAM].getValue())) {
		startNewGame();
	}

	if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
		playhead = 0;
		displayedStep = 0;
	}

	bool running = true;
	if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
		if (lastClockEdgeSeconds >= 0.f) {
			previousClockPeriodSeconds = std::max(transportTimeSeconds - lastClockEdgeSeconds, 1e-6f);
		}
		lastClockEdgeSeconds = transportTimeSeconds;
		if (running) {
			emitStepAtClockEdge();
		}
	}

	int effectiveRootWrapped = rootSemitone();
	int effectiveRootLinear = rootSemitoneLinear();
	if (!cachedRootSemitoneValid) {
		cachedRootSemitoneWrapped = effectiveRootWrapped;
		cachedRootSemitoneLinear = effectiveRootLinear;
		cachedRootSemitoneValid = true;
	}
	else if (
		effectiveRootWrapped != cachedRootSemitoneWrapped
		|| effectiveRootLinear != cachedRootSemitoneLinear
	) {
		cachedRootSemitoneWrapped = effectiveRootWrapped;
		cachedRootSemitoneLinear = effectiveRootLinear;
		refreshHeldPitchForCurrentStep();
	}

	if (modGlideActive && modGlideDurationSeconds > 0.f) {
		float t = clamp((transportTimeSeconds - modGlideStartSeconds) / modGlideDurationSeconds, 0.f, 1.f);
		modOutputVolts = modGlideStartVolts + (modGlideTargetVolts - modGlideStartVolts) * t;
		if (t >= 1.f) {
			modGlideActive = false;
		}
	}
	outputs[PITCH_OUTPUT].setVoltage(heldPitch);
	outputs[ACCENT_OUTPUT].setVoltage(heldAccent);
	outputs[MOD_OUTPUT].setVoltage(modOutputVolts);
	outputs[EOC_OUTPUT].setVoltage(eocPulse.process(args.sampleTime) ? 10.f : 0.f);

	lights[RUN_LIGHT].setBrightness(0.f);
	lights[HUMAN_TURN_LIGHT].setBrightness(!gameOver && turnSide == humanSide() ? 1.f : 0.f);
	lights[AI_TURN_LIGHT].setBrightness(!gameOver && turnSide == aiSide() ? 1.f : 0.f);
}

