#include "SibylTiming.hpp"

#include <algorithm>

namespace sibyl {

bool resolutionSupportsSwing(const std::string& resolution) {
	return !resolution.empty() && resolution.back() != 'd' && resolution.back() != 't';
}

int wrappedStep(int64_t nominalStep, int patternLength) {
	if (patternLength <= 0) return 0;
	int step = static_cast<int>(nominalStep % patternLength);
	return step < 0 ? step + patternLength : step;
}

const StepEvent* eventAtStep(const Pattern& pattern, int step) {
	if (step < 0 || step >= pattern.length) return nullptr;
	if (pattern.eventIndexByStep.size() == static_cast<size_t>(pattern.length)) {
		int index = pattern.eventIndexByStep[step];
		if (index >= 0 && index < static_cast<int>(pattern.steps.size())) return &pattern.steps[index];
		return nullptr;
	}
	// Manually constructed test/host patterns may not have passed through the
	// compiler. Preserve correctness off the hot production path.
	for (const StepEvent& event : pattern.steps) if (event.step == step) return &event;
	return nullptr;
}

double scheduledEventBeat(const Pattern& pattern, int64_t nominalStep,
		const StepEvent& event, double effectiveSwing) {
	double offset = std::max(-0.499999, std::min(static_cast<double>(event.microshift), 0.499999));
	if (resolutionSupportsSwing(pattern.resolutionStr) && wrappedStep(nominalStep, pattern.length) % 2 == 1)
		offset += std::max(0.0, std::min(effectiveSwing, 0.49));
	return (static_cast<double>(nominalStep) + offset) * pattern.resolutionBeats;
}

} // namespace sibyl
