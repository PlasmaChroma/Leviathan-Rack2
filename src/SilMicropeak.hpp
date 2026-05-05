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

inline Result analyzeChunk(const float* left, const float* right, size_t count, float fullScaleVolts = 5.f) {
	Result result;
	if (!left || !right || count < 96 || fullScaleVolts <= 0.f) {
		return result;
	}

	const float minPeak = 0.58f * fullScaleVolts;
	const float minNeighborDrop = 0.11f * fullScaleVolts;
	const int localRadius = 24;
	const int exclusionRadius = 2;
	int refractory = 0;

	for (size_t i = size_t(localRadius); i + size_t(localRadius) < count; ++i) {
		if (refractory > 0) {
			refractory--;
			continue;
		}

		const float peak = sampleAbsMax(left[i], right[i]);
		if (peak < minPeak) {
			continue;
		}

		const float prev = sampleAbsMax(left[i - 1], right[i - 1]);
		const float next = sampleAbsMax(left[i + 1], right[i + 1]);
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
			const float v = sampleAbsMax(left[index], right[index]);
			localSum += v;
			localMax = std::max(localMax, v);
			localCount++;
		}
		const float localMean = localSum / std::max(localCount, 1);
		const float isolationRatio = peak / std::max(localMean, 1e-4f);
		if (isolationRatio < 6.0f || peak < localMax * 1.65f) {
			continue;
		}

		int width = 1;
		for (size_t j = i; j > 0 && sampleAbsMax(left[j - 1], right[j - 1]) >= peak * 0.5f; --j) {
			width++;
		}
		for (size_t j = i + 1; j < count && sampleAbsMax(left[j], right[j]) >= peak * 0.5f; ++j) {
			width++;
		}
		if (width > 4) {
			continue;
		}

		const float severity = (peak / fullScaleVolts) * std::min(isolationRatio / 12.f, 2.f);
		result.eventCount++;
		result.strongestSeverity = std::max(result.strongestSeverity, severity);
		refractory = localRadius;
	}

	result.detected = result.eventCount >= 2 || result.strongestSeverity >= 1.35f;
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
