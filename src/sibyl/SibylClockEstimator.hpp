#pragma once

#include "SibylTypes.hpp"

namespace sibyl {

struct ClockAdvance {
	double beatDelta = 0.0;
	bool timedOut = false;
};

class ExternalClockEstimator {
public:
	ClockAdvance process(double sampleTime, bool edge, int ppqn, double timeoutMs,
		OnExternalStop timeoutPolicy, double internalBpm);
	double estimatedBpm(int ppqn, double fallbackBpm) const;
	bool hasEstimate() const { return m_hasEstimate; }
	double intervalSeconds() const { return m_intervalSeconds; }

private:
	bool m_seenEdge = false;
	bool m_hasEstimate = false;
	double m_elapsedSinceEdge = 0.0;
	double m_interpolatedSinceEdge = 0.0;
	double m_intervalSeconds = 0.0;
	double m_musicalPhaseBeats = 0.0;
	bool m_internalFallbackActive = false;
};

} // namespace sibyl
