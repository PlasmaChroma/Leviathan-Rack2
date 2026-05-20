#include "CrownstepShared.hpp"

namespace {
struct ActiveRange {
	int start = 0;
	int endExclusive = 0;
	int length = 0;
};

static ActiveRange computeActiveRange(const Crownstep* module, int historySize, int sequenceCap) {
	ActiveRange r;
	if (historySize <= 0) {
		return r;
	}
	if (module && module->sequenceRangeTrimEnabled) {
		int leftTrim = clamp(module->sequenceTrimLeft, 0, historySize - 1);
		int maxRightTrim = std::max(0, historySize - leftTrim - 1);
		int rightTrim = clamp(module->sequenceTrimRight, 0, maxRightTrim);
		r.start = leftTrim;
		r.endExclusive = historySize - rightTrim;
		r.length = std::max(1, r.endExclusive - r.start);
		r.endExclusive = r.start + r.length;
		return r;
	}
	r.length = crownstep::activeLength(historySize, sequenceCap);
	r.start = crownstep::activeStartIndex(historySize, sequenceCap);
	r.endExclusive = r.start + r.length;
	return r;
}
}

int Crownstep::activeLength() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	const ActiveRange range = computeActiveRange(this, int(history.size()), currentSequenceCap());
	return range.length;
}

int Crownstep::activeStartIndex() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	const ActiveRange range = computeActiveRange(this, int(history.size()), currentSequenceCap());
	return range.start;
}

int Crownstep::activeEndIndexExclusive() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	const ActiveRange range = computeActiveRange(this, int(history.size()), currentSequenceCap());
	return range.endExclusive;
}

void Crownstep::setActiveRangeTrimWindow(int startInclusive, int endExclusive) {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	const int historySize = int(history.size());
	if (historySize <= 0) {
		sequenceRangeTrimEnabled = false;
		sequenceTrimLeft = 0;
		sequenceTrimRight = 0;
		return;
	}
	int start = clamp(startInclusive, 0, historySize - 1);
	int end = clamp(endExclusive, start + 1, historySize);
	sequenceRangeTrimEnabled = true;
	sequenceTrimLeft = start;
	sequenceTrimRight = std::max(0, historySize - end);
}

void Crownstep::clearActiveRangeTrimWindow() {
	std::lock_guard<std::recursive_mutex> lock(sequenceMutex);
	sequenceRangeTrimEnabled = false;
	sequenceTrimLeft = 0;
	sequenceTrimRight = 0;
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
	const ActiveRange range = computeActiveRange(this, historySize, sequenceCap);
	int length = range.length;
	if (length <= 0) {
		heldPitch = NO_SEQUENCE_PITCH_VOLTS;
		return;
	}
	if (displayedStep <= 0) {
		return;
	}
	int shownStep = clamp(displayedStep, 1, length);
	int sequenceIndex = range.start + (shownStep - 1);
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
	float effectivePitchRangeParam = params[RANGE_PARAM].getValue();
	if (!cachedRootSemitoneValid) {
		cachedRootSemitoneWrapped = effectiveRootWrapped;
		cachedRootSemitoneLinear = effectiveRootLinear;
		cachedPitchRangeParam = effectivePitchRangeParam;
		cachedRootSemitoneValid = true;
	}
	else if (
		effectiveRootWrapped != cachedRootSemitoneWrapped
		|| effectiveRootLinear != cachedRootSemitoneLinear
		|| std::fabs(effectivePitchRangeParam - cachedPitchRangeParam) > 1e-6f
	) {
		cachedRootSemitoneWrapped = effectiveRootWrapped;
		cachedRootSemitoneLinear = effectiveRootLinear;
		cachedPitchRangeParam = effectivePitchRangeParam;
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
