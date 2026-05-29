#include "CrownstepShared.hpp"

std::string CrownstepSeqLengthQuantity::getDisplayValueString() {
	int requested = clamp(int(std::round(getValue())), SEQ_LENGTH_MIN, SEQ_LENGTH_MAX);
	if (requested >= SEQ_LENGTH_MAX) {
		return "Full";
	}
	const Crownstep* crownstepModule = dynamic_cast<const Crownstep*>(module);
	if (!crownstepModule) {
		return std::to_string(requested);
	}
	std::lock_guard<std::recursive_mutex> lock(crownstepModule->sequenceMutex);
	int available = int(crownstepModule->history.size());
	if (available > 0 && requested >= available) {
		return "Full";
	}
	return std::to_string(requested);
}

float CrownstepRangeQuantity::getDisplayValue() {
	const Crownstep* crownstepModule = dynamic_cast<const Crownstep*>(module);
	const int cellCount = crownstepModule ? crownstepModule->boardCellCount() : crownstep::BOARD_SIZE;
	return crownstep::pitchRangeSemitoneSpan(getValue(), cellCount);
}

void CrownstepRangeQuantity::setDisplayValue(float displayValue) {
	const Crownstep* crownstepModule = dynamic_cast<const Crownstep*>(module);
	const int cellCount = crownstepModule ? crownstepModule->boardCellCount() : crownstep::BOARD_SIZE;
	const float denominator = std::max(1.f, float(std::max(0, cellCount - 1)));
	const float multiplier = displayValue / denominator;
	setImmediateValue(crownstep::pitchRangeParamFromMultiplier(multiplier));
}

std::string CrownstepRangeQuantity::getDisplayValueString() {
	return string::f("%.1f st", getDisplayValue());
}
