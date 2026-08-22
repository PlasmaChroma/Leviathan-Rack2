#include "SibylHardwareControl.hpp"

#include <algorithm>
#include <cmath>

namespace sibyl {

int sceneIndexFromCv(float voltage, int sceneCount, int currentIndex) {
	if (sceneCount <= 0) return 0;
	const float clamped = std::max(0.0f, std::min(voltage, kSceneCvRangeVolts));
	const float binWidth = kSceneCvRangeVolts / sceneCount;
	if (currentIndex >= 0 && currentIndex < sceneCount) {
		const float margin = binWidth * kSceneCvHysteresisFraction;
		const float lower = currentIndex * binWidth - margin;
		const float upper = (currentIndex + 1) * binWidth + margin;
		if (clamped >= lower && clamped <= upper) return currentIndex;
	}
	return std::min(sceneCount - 1, static_cast<int>(std::floor(clamped / binWidth)));
}

bool hardwareResetBoundaryReached(bool externalClockConnected, bool externalClockEdge,
		const BoundaryState& boundary) {
	return externalClockConnected ? externalClockEdge : boundary.beat;
}

PhaseMode destinationTrackPhaseMode(const Scene& scene, const std::string& trackId) {
	auto assignment = scene.tracks.find(trackId);
	if (assignment != scene.tracks.end() && assignment->second.hasPhaseModeOverride)
		return assignment->second.phaseModeOverride;
	return scene.phaseMode;
}

} // namespace sibyl
