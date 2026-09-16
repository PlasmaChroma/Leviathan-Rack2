#pragma once

#include <algorithm>

namespace bifurx {

constexpr float kBifurxTransitionSeconds = 0.003f;

inline float bifurxSmoothstep(float x) {
	const float t = std::max(0.f, std::min(1.f, x));
	return t * t * (3.f - 2.f * t);
}

// Fades the current discrete signal path to zero, adopts the latest requested
// path at the silent midpoint, then fades it back in. The steady-state path is
// one comparison and no extra filter processing.
struct BifurxTransitionSmoother {
	enum Phase {
		STABLE,
		FADE_OUT,
		WARM_UP,
		FADE_IN
	};

	int activeMode = 0;
	int requestedMode = 0;
	int activeBoundary = 0;
	int requestedBoundary = 0;
	int warmupSamples = 0;
	int requestedWarmupSamples = 0;
	bool activeSoftLimitingEnabled = true;
	bool requestedSoftLimitingEnabled = true;
	Phase phase = STABLE;
	int phasePosition = 0;
	int halfTransitionSamples = 1;
	bool initialized = false;

	void reset() {
		activeMode = 0;
		requestedMode = 0;
		activeBoundary = requestedBoundary = 0;
		activeSoftLimitingEnabled = true;
		requestedSoftLimitingEnabled = true;
		phase = STABLE;
		phasePosition = 0;
		halfTransitionSamples = 1;
		initialized = false;
	}

	void prepare(int mode, bool softLimitingEnabled, float sampleRate, int boundary = 0, bool primeBoundary = false) {
		requestedMode = mode;
		requestedBoundary = boundary;
		requestedWarmupSamples = boundary == 2 ? (primeBoundary ? 64 : 32) : (boundary == 1 && primeBoundary ? 16 : 0);
		requestedSoftLimitingEnabled = softLimitingEnabled;
		if (!initialized) {
			initialized = true;
			activeMode = mode;
			activeBoundary = boundary;
			activeSoftLimitingEnabled = softLimitingEnabled;
			return;
		}
		if (phase == STABLE && (
			mode != activeMode || boundary != activeBoundary || softLimitingEnabled != activeSoftLimitingEnabled
		)) {
			halfTransitionSamples = std::max(
				1,
				int(std::max(sampleRate, 1.f) * (0.5f * kBifurxTransitionSeconds) + 0.5f)
			);
			phase = FADE_OUT;
			phasePosition = 0;
		}
	}

	float apply(float target) {
		// The wide FIR's output boundary delays the topology switch by 31
		// samples. Keep its settling edge in the silent part of the transition.
		if (phase == WARM_UP) {
			if (++phasePosition >= warmupSamples) { phasePosition = 0; phase = FADE_IN; }
			return 0.f;
		}
		if (phase == STABLE) {
			return target;
		}

		const float progress = float(phasePosition) / float(halfTransitionSamples);
		const float shaped = bifurxSmoothstep(progress);
		const float gain = phase == FADE_OUT ? 1.f - shaped : shaped;
		const float output = gain * target;
		phasePosition++;
		if (phasePosition > halfTransitionSamples) {
			phasePosition = 0;
			if (phase == FADE_OUT) {
				warmupSamples = requestedWarmupSamples;
				activeMode = requestedMode;
				activeBoundary = requestedBoundary;
				activeSoftLimitingEnabled = requestedSoftLimitingEnabled;
				phase = warmupSamples > 0 ? WARM_UP : FADE_IN;
			}
			else {
				phase = STABLE;
			}
		}
		return output;
	}

	bool isStable() const {
		return phase == STABLE;
	}
};

} // namespace bifurx
