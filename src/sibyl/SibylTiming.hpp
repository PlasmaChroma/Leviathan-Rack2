#pragma once

#include "SibylTypes.hpp"

#include <cstdint>

namespace sibyl {

bool resolutionSupportsSwing(const std::string& resolution);
int wrappedStep(int64_t nominalStep, int patternLength);
const StepEvent* eventAtStep(const Pattern& pattern, int step);

// nominalStep is an unwrapped integer grid position, allowing negative
// microshift at step zero to appear before the next loop boundary naturally.
double scheduledEventBeat(const Pattern& pattern, int64_t nominalStep,
	const StepEvent& event, double effectiveSwing);

inline bool scheduledEventCrossed(double previousBeat, double currentBeat, double scheduledBeat) {
	return (previousBeat < scheduledBeat && scheduledBeat <= currentBeat) ||
		(previousBeat == 0.0 && scheduledBeat == 0.0 && currentBeat > 0.0);
}

} // namespace sibyl
