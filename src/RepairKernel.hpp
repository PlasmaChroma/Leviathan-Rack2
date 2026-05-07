#pragma once

#include <algorithm>
#include <cmath>

namespace sil {
namespace repair {

struct CandidateConfig {
	float minPeakFullScale = 0.52f;
	float minNeighborDropFullScale = 0.10f;
	float minNeighborRatio = 1.55f;
	float maxNeighborShare = 0.65f;
	float minIsolationRatio = 3.0f;
};

struct Window5 {
	float prev2 = 0.f;
	float prev1 = 0.f;
	float center = 0.f;
	float next1 = 0.f;
	float next2 = 0.f;
};

struct RepairDecision {
	bool candidate = false;
	float repaired = 0.f;
	float depth = 0.f;
};

inline bool detectCandidate(const Window5& w, const CandidateConfig& cfg, float audioFullScale) {
	const float peak = std::fabs(w.center);
	const float near = std::max(std::fabs(w.prev1), std::fabs(w.next1));
	const float guard = std::max(std::fabs(w.prev2), std::fabs(w.next2));
	const float localMean = 0.25f * (std::fabs(w.prev2) + std::fabs(w.prev1) + std::fabs(w.next1) + std::fabs(w.next2));
	const float minPeak = cfg.minPeakFullScale * audioFullScale;
	const float minDrop = cfg.minNeighborDropFullScale * audioFullScale;
	if (peak < minPeak) {
		return false;
	}
	if ((peak - near) < minDrop) {
		return false;
	}
	if (peak < near * cfg.minNeighborRatio) {
		return false;
	}
	if (near > peak * cfg.maxNeighborShare || guard > peak * cfg.maxNeighborShare) {
		return false;
	}
	const float isolation = peak / std::max(localMean, 1e-4f);
	return isolation >= cfg.minIsolationRatio;
}

inline RepairDecision repairCenterLinear(const Window5& w, bool candidate) {
	RepairDecision d;
	d.candidate = candidate;
	d.repaired = candidate ? (0.5f * (w.prev1 + w.next1)) : w.center;
	if (candidate) {
		d.depth = std::fabs(w.center - d.repaired) / std::max(std::fabs(w.center), 1e-4f);
	}
	return d;
}

} // namespace repair
} // namespace sil
