#include "SibylClockEstimator.hpp"

#include <algorithm>
#include <cmath>

namespace sibyl {

ClockAdvance ExternalClockEstimator::process(double sampleTime, bool edge, int ppqn,
		double timeoutMs, OnExternalStop timeoutPolicy, double internalBpm) {
	ClockAdvance result;
	if (!std::isfinite(sampleTime) || sampleTime <= 0.0) return result;
	ppqn = std::max(1, ppqn);
	double quantum = 1.0 / ppqn;
	m_elapsedSinceEdge += sampleTime;

	if (edge) {
		if (m_seenEdge && m_elapsedSinceEdge > 0.0) {
			// The composition BPM range bounds plausible pulse intervals. Clamping
			// prevents one corrupt interval from poisoning free-run indefinitely.
			double fastest = 60.0 / (400.0 * ppqn);
			double slowest = 60.0 / (20.0 * ppqn);
			double measured = std::max(fastest, std::min(m_elapsedSinceEdge, slowest));
			if (!m_hasEstimate) {
				m_intervalSeconds = measured;
				m_hasEstimate = true;
			} else {
				double limited = std::max(m_intervalSeconds * 0.75,
					std::min(measured, m_intervalSeconds * 1.25));
				double relativeChange = std::abs(limited - m_intervalSeconds) / m_intervalSeconds;
				double blend = relativeChange < 0.03 ? 0.125 : 0.35;
				m_intervalSeconds += (limited - m_intervalSeconds) * blend;
			}
		}
		// Predictions never replace or move a real edge. Complete exactly the
		// remaining external quantum at the edge's arrival sample.
		result.beatDelta = std::max(0.0, quantum - m_interpolatedSinceEdge);
		m_musicalPhaseBeats += result.beatDelta;
		if (m_musicalPhaseBeats >= 1024.0) m_musicalPhaseBeats = std::fmod(m_musicalPhaseBeats, 1.0);
		m_elapsedSinceEdge = 0.0;
		m_interpolatedSinceEdge = 0.0;
		m_internalFallbackActive = false;
		m_seenEdge = true;
		return result;
	}

	double timeoutSeconds = std::max(0.001, timeoutMs * 0.001);
	result.timedOut = m_elapsedSinceEdge > timeoutSeconds;
	if (!m_hasEstimate) return result;

	double delta = 0.0;
	if (!result.timedOut) {
		delta = quantum * sampleTime / m_intervalSeconds;
		// Do not predict through the next physical edge. If it arrives late,
		// hold at that boundary until the configured timeout policy takes over.
		delta = std::max(0.0, std::min(delta, quantum - m_interpolatedSinceEdge));
		m_interpolatedSinceEdge += delta;
	} else if (timeoutPolicy == OnExternalStop::FREE_RUN) {
		delta = quantum * sampleTime / m_intervalSeconds;
	} else if (timeoutPolicy == OnExternalStop::INTERNAL) {
		if (m_internalFallbackActive) {
			delta = std::max(20.0, std::min(internalBpm, 400.0)) * sampleTime / 60.0;
		} else {
			// Continue from the learned tempo only as far as the next quarter-note
			// boundary, then hand over without a discontinuous phase jump.
			delta = quantum * sampleTime / m_intervalSeconds;
			double phaseInBeat = std::fmod(m_musicalPhaseBeats, 1.0);
			double remaining = phaseInBeat == 0.0 ? 1.0 : 1.0 - phaseInBeat;
			if (delta >= remaining) {
				delta = remaining;
				m_internalFallbackActive = true;
			}
		}
	}
	result.beatDelta = delta;
	m_musicalPhaseBeats += delta;
	if (m_musicalPhaseBeats >= 1024.0) m_musicalPhaseBeats = std::fmod(m_musicalPhaseBeats, 1.0);
	return result;
}

double ExternalClockEstimator::estimatedBpm(int ppqn, double fallbackBpm) const {
	if (!m_hasEstimate || m_intervalSeconds <= 0.0) return fallbackBpm;
	return 60.0 / (m_intervalSeconds * std::max(1, ppqn));
}

} // namespace sibyl
