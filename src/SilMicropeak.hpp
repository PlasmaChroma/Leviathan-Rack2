#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace sil_micropeak {

struct Result {
	bool detected = false;
	int eventCount = 0;
	float strongestSeverity = 0.f;
};

struct StereoResult {
	Result left;
	Result right;
};

struct Profile {
	float minPeakFullScale;
	float minNeighborDropFullScale;
	float minIsolationRatio;
	float minLocalMaxRatio;
	float minSeverityForSingleEvent;
	int minEvents;
	int localRadius;
	int exclusionRadius;
	int maxHalfPeakWidth;
	float minRoughness;

	Profile()
		: minPeakFullScale(0.30f),
		  minNeighborDropFullScale(0.045f),
		  minIsolationRatio(3.25f),
		  minLocalMaxRatio(1.28f),
		  minSeverityForSingleEvent(0.55f),
		  minEvents(2),
		  localRadius(24),
		  exclusionRadius(2),
		  maxHalfPeakWidth(4),
		  minRoughness(4.5f) {
	}
};

inline Profile makeDebugProfile() {
	Profile profile;
	profile.minPeakFullScale = 0.18f;
	profile.minNeighborDropFullScale = 0.025f;
	profile.minIsolationRatio = 2.75f;
	profile.minLocalMaxRatio = 1.20f;
	profile.minSeverityForSingleEvent = 0.35f;
	profile.minEvents = 1;
	return profile;
}

struct StereoSample {
	float l = 0.f;
	float r = 0.f;

	StereoSample() = default;
	StereoSample(float left, float right) : l(left), r(right) {
	}
};

inline float sampleAbsMax(float l, float r) {
	return std::max(std::fabs(l), std::fabs(r));
}

inline Result analyzeChunkMono(
	const float* channel,
	size_t count,
	float fullScaleVolts = 5.f,
	const Profile& profile = Profile()
) {
	Result result;
	if (!channel || fullScaleVolts <= 0.f) {
		return result;
	}

	const int localRadius = std::max(profile.localRadius, 2);
	const int exclusionRadius = std::max(1, std::min(profile.exclusionRadius, localRadius - 1));
	if (count < size_t(2 * localRadius + 3)) {
		return result;
	}

	const float minPeak = std::max(0.f, profile.minPeakFullScale) * fullScaleVolts;
	const float minNeighborDrop = std::max(0.f, profile.minNeighborDropFullScale) * fullScaleVolts;
	int refractory = 0;

	for (size_t i = size_t(localRadius); i + size_t(localRadius) < count; ++i) {
		if (refractory > 0) {
			refractory--;
			continue;
		}

		const float peak = std::fabs(channel[i]);
		if (peak < minPeak) {
			continue;
		}

		const float prev = std::fabs(channel[i - 1]);
		const float next = std::fabs(channel[i + 1]);
		const float neighborMax = std::max(prev, next);
		if (peak - neighborMax < minNeighborDrop) {
			continue;
		}

		float localSum = 0.f;
		float localMax = 0.f;
		int localCount = 0;
		for (int o = -localRadius; o <= localRadius; ++o) {
			if (std::abs(o) <= exclusionRadius) {
				continue;
			}
			const size_t index = size_t(int(i) + o);
				const float v = std::fabs(channel[index]);
			localSum += v;
			localMax = std::max(localMax, v);
			localCount++;
		}
		const float localMean = localSum / std::max(localCount, 1);
		const float isolationRatio = peak / std::max(localMean, 1e-4f);
		const bool isoPass = isolationRatio >= profile.minIsolationRatio && peak >= localMax * profile.minLocalMaxRatio;

		float d2Sum = 0.f;
		for (int o = -3; o <= 3; ++o) {
			const size_t j = size_t(int(i) + o);
			const float a = std::fabs(channel[j - 1]);
			const float b = std::fabs(channel[j]);
			const float c = std::fabs(channel[j + 1]);
			d2Sum += std::fabs(a - 2.f * b + c);
		}
		const float roughness = d2Sum / std::max(localMean, 1e-4f);
		const bool roughPass = roughness >= profile.minRoughness;
		if (!(isoPass || roughPass)) {
			continue;
		}

		int width = 1;
		for (size_t j = i; j > 0 && std::fabs(channel[j - 1]) >= peak * 0.5f; --j) {
			width++;
		}
		for (size_t j = i + 1; j < count && std::fabs(channel[j]) >= peak * 0.5f; ++j) {
			width++;
		}
		if (width > std::max(1, profile.maxHalfPeakWidth)) {
			continue;
		}

		const float severity =
			(peak / fullScaleVolts) * std::min((0.85f * isolationRatio + 0.15f * roughness) / 12.f, 2.f);
		result.eventCount++;
		result.strongestSeverity = std::max(result.strongestSeverity, severity);
		refractory = localRadius;
	}

	result.detected = result.eventCount >= std::max(1, profile.minEvents) ||
		result.strongestSeverity >= profile.minSeverityForSingleEvent;
	return result;
}

inline Result analyzeChunk(
	const float* left,
	const float* right,
	size_t count,
	float fullScaleVolts = 5.f,
	const Profile& profile = Profile()
) {
	const Result leftResult = analyzeChunkMono(left, count, fullScaleVolts, profile);
	const Result rightResult = analyzeChunkMono(right, count, fullScaleVolts, profile);
	Result merged;
	merged.eventCount = leftResult.eventCount + rightResult.eventCount;
	merged.strongestSeverity = std::max(leftResult.strongestSeverity, rightResult.strongestSeverity);
	merged.detected = leftResult.detected || rightResult.detected;
	return merged;
}

inline StereoResult analyzeChunkStereo(
	const float* left,
	const float* right,
	size_t count,
	float fullScaleVolts = 5.f,
	const Profile& profile = Profile()
) {
	StereoResult result;
	result.left = analyzeChunkMono(left, count, fullScaleVolts, profile);
	result.right = analyzeChunkMono(right, count, fullScaleVolts, profile);
	return result;
}

inline bool isRepairableMicropeak(
	StereoSample previous,
	StereoSample center,
	StereoSample next,
	float fullScaleVolts = 5.f
) {
	if (fullScaleVolts <= 0.f) {
		return false;
	}

	const float peak = sampleAbsMax(center.l, center.r);
	const float neighborMax = std::max(sampleAbsMax(previous.l, previous.r), sampleAbsMax(next.l, next.r));
	const float minPeak = 0.52f * fullScaleVolts;
	const float minNeighborDrop = 0.10f * fullScaleVolts;
	return peak >= minPeak && (peak - neighborMax) >= minNeighborDrop && peak >= neighborMax * 1.55f;
}

inline StereoSample repairMicropeak(StereoSample previous, StereoSample center, StereoSample next, float fullScaleVolts = 5.f) {
	if (!isRepairableMicropeak(previous, center, next, fullScaleVolts)) {
		return center;
	}
	return StereoSample(0.5f * (previous.l + next.l), 0.5f * (previous.r + next.r));
}

class CleanupFilter {
public:
	StereoSample process(StereoSample input, bool active, float fullScaleVolts = 5.f) {
		if (!initialized) {
			older = input;
			center = input;
			initialized = true;
			return input;
		}

		const StereoSample output = active ? repairMicropeak(older, center, input, fullScaleVolts) : center;
		older = center;
		center = input;
		return output;
	}

	void reset() {
		older = {};
		center = {};
		initialized = false;
	}

private:
	StereoSample older;
	StereoSample center;
	bool initialized = false;
};

} // namespace sil_micropeak
