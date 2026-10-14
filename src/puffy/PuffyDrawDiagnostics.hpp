#pragma once

#include <chrono>
#include <cstdint>

struct PuffyDrawMetrics {
	std::uint64_t fishDrawNs = 0u;
	std::uint64_t bodyEnsureNs = 0u;
	std::uint64_t bodyRecolorNs = 0u;
	std::uint64_t bodyUploadNs = 0u;
	std::uint64_t bodyDrawNs = 0u;
	std::uint64_t bodyTransitionDrawNs = 0u;
	std::uint64_t bodyTransitionAtlasPrewarmNs = 0u;
	std::uint64_t finDrawNs = 0u;
	std::uint64_t eyeDrawNs = 0u;
	std::uint64_t transferDrawNs = 0u;
	std::uint64_t transferCurveDrawNs = 0u;
	std::uint64_t transferCurveRebuildNs = 0u;
	std::uint32_t bodyCacheHits = 0u;
	std::uint32_t bodyRecolors = 0u;
	std::uint32_t bodyImageCreates = 0u;
	std::uint32_t bodyImageUpdates = 0u;
	std::uint32_t bodyContextResets = 0u;
	std::uint32_t bodyFallbackDraws = 0u;
	std::uint32_t bodyTransitionDraws = 0u;
	std::uint32_t bodyTransitionAtlasCreates = 0u;
	std::uint32_t bodyTransitionAtlasResets = 0u;
	std::uint32_t bodyTransitionAtlasPrewarms = 0u;
	std::uint32_t transferCurveRebuilds = 0u;
};

PuffyDrawMetrics& puffyDrawMetricsForUiThread();
PuffyDrawMetrics consumePuffyDrawMetrics();
bool isPuffyDrawMeasurementEnabled();

struct PuffyScopedDrawTimer {
	using Clock = std::chrono::steady_clock;
	std::uint64_t* elapsedNs = nullptr;
	Clock::time_point start;

	PuffyScopedDrawTimer(std::uint64_t& elapsedNs, bool enabled)
		: elapsedNs(enabled ? &elapsedNs : nullptr)
		, start(enabled ? Clock::now() : Clock::time_point()) {
	}

	~PuffyScopedDrawTimer() {
		if (elapsedNs) {
			*elapsedNs += std::uint64_t(
				std::chrono::duration_cast<std::chrono::nanoseconds>(
					Clock::now() - start).count());
		}
	}
};
