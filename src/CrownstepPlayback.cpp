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
		float boardValueIndex = boardValueIndexForMove(move);
		if (melodicBiasEnabled && sequenceIndex > 0) {
			const Move& previousMove = moveHistory[size_t(sequenceIndex - 1)];
			float previousBoardValueIndex = boardValueIndexForMove(previousMove);
			boardValueIndex = applyMelodicBiasToBoardValueIndex(previousBoardValueIndex, boardValueIndex, move);
		}
		return mapPitchFromBoardValueIndex(boardValueIndex, move.isKing);
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
		playhead = 0;
		eocGateHigh = false;
		return;
	}

	playhead = clamp(playhead, 0, std::max(length - 1, 0));
	displayedStep = playhead + 1;
	int sequenceIndex = activeStartIndex() + playhead;
	const Step& step = history[size_t(sequenceIndex)];
	heldPitch = pitchForSequenceIndex(sequenceIndex);
	heldAccent = step.accent;
	heldMod = step.mod * 10.f;
	modOutputVolts = heldMod;

	playhead++;
	if (playhead >= length) {
		playhead = 0;
		if (length > 1) {
			eocGateHigh = true;
		}
	}
}

void Crownstep::process(const ProcessArgs& args) {
	transportTimeSeconds += args.sampleTime;
	if (transportTimeSeconds >= 4096.0) {
		transportTimeSeconds = std::fmod(transportTimeSeconds, 4096.0);
	}

	if (newGameTrigger.process(params[NEW_GAME_PARAM].getValue())) {
		startNewGame();
	}
	if (debugAddMovesTrigger.process(params[DEBUG_ADD_MOVES_PARAM].getValue())) {
		appendDebugRandomMoves(10);
	}

	if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
		playhead = 0;
		displayedStep = 0;
		eocGateHigh = false;
		eocActivityPulseRequests.store(0, std::memory_order_relaxed);
		eocActivityPulseQueued = 0;
		eocActivityPulseRemainingSeconds = 0.f;
	}

	if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
		// Hold EoC high until the next clock edge, then clear before
		// advancing so non-wrap steps read low.
		eocGateHigh = false;
		emitStepAtClockEdge();
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

	int requestedActivityPulses = eocActivityPulseRequests.exchange(0, std::memory_order_relaxed);
	if (requestedActivityPulses > 0) {
		eocActivityPulseQueued += requestedActivityPulses;
	}
	bool sequenceLengthOneMode = (currentSequenceCap() == 1);
	if (!sequenceLengthOneMode) {
		eocActivityPulseQueued = 0;
		eocActivityPulseRemainingSeconds = 0.f;
	}
	else {
		if (eocActivityPulseRemainingSeconds <= 0.f && eocActivityPulseQueued > 0) {
			eocActivityPulseRemainingSeconds = EOC_ACTIVITY_PULSE_SECONDS;
			--eocActivityPulseQueued;
		}
		if (eocActivityPulseRemainingSeconds > 0.f) {
			eocActivityPulseRemainingSeconds = std::max(0.f, eocActivityPulseRemainingSeconds - args.sampleTime);
		}
	}
	bool eocOutputHigh = sequenceLengthOneMode ? (eocActivityPulseRemainingSeconds > 0.f) : eocGateHigh;

	outputs[PITCH_OUTPUT].setVoltage(heldPitch);
	outputs[ACCENT_OUTPUT].setVoltage(heldAccent);
	outputs[MOD_OUTPUT].setVoltage(modOutputVolts);
	outputs[EOC_OUTPUT].setVoltage(eocOutputHigh ? 10.f : 0.f);

	bool humanLedOn = false;
	bool aiLedOn = false;
	if (gameOver) {
		humanLedOn = (winnerSide == humanSide());
		aiLedOn = (winnerSide == aiSide());
	}
	else {
		humanLedOn = (turnSide == humanSide());
		aiLedOn = (turnSide == aiSide());
	}

	lights[HUMAN_TURN_LIGHT].setBrightness(humanLedOn ? 1.f : 0.f);
	lights[AI_TURN_LIGHT].setBrightness(aiLedOn ? 1.f : 0.f);
}
