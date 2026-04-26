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
	int available = int(crownstepModule->history.size());
	if (available > 0 && requested >= available) {
		return "Full";
	}
	return std::to_string(requested);
}
