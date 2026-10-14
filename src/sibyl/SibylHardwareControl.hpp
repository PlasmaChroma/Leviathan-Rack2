#pragma once

#include "SibylAdoption.hpp"

namespace sibyl {

// Rack's default SchmittTrigger thresholds. Physical RUN uses the same 1 V
// high threshold as the trigger inputs, while its low state is level-sensitive.
constexpr float kHardwareSchmittLowVolts = 0.1f;
constexpr float kHardwareSchmittHighVolts = 1.0f;
constexpr float kSceneCvRangeVolts = 10.0f;
constexpr float kSceneCvHysteresisFraction = 0.05f;

// Equal-width 0-10 V scene selection with a hysteresis band equal to 5% of
// one scene bin on each side of the currently selected bin.
int sceneIndexFromCv(float voltage, int sceneCount, int currentIndex);

// RESET waits for the next detected external edge in external mode, or the
// next quarter-note boundary in internal mode.
bool hardwareResetBoundaryReached(bool externalClockConnected, bool externalClockEdge,
		const BoundaryState& boundary);

inline bool hardwareSceneBoundaryReached(ApplyAt applyAt, const BoundaryState& boundary) {
	return adoptionBoundaryReached(applyAt, boundary);
}

inline bool hardwareRunFallingEdge(bool wasRunning, bool isRunning) {
	return wasRunning && !isRunning;
}

// Per-assignment phase mode overrides the destination scene default.
PhaseMode destinationTrackPhaseMode(const Scene& scene, const std::string& trackId);

} // namespace sibyl
