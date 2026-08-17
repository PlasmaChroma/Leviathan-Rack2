#pragma once

#include "Wyrm.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace wyrm_render {

constexpr float kPointEdgeInsetPx = 2.2f;

inline float pointDrawWidth(Vec size) {
	return std::max(1.f, size.x - 2.f * kPointEdgeInsetPx);
}

inline void visualRockClearance(Vec size, float* valueClearance, float* phaseClearance) {
	const float drawWidth = pointDrawWidth(size);
	const float pixelClearance = 0.5f * 4.f + 0.5f * 2.2f + 0.75f;
	if (valueClearance) {
		const float normalized = size.y > 1.f
			? (2.f * pixelClearance / size.y) / std::max(kWyrmRockValueScale, 1e-4f)
			: kWyrmRockClearance;
		*valueClearance = std::max(kWyrmRockClearance, normalized);
	}
	if (phaseClearance) {
		*phaseClearance = drawWidth > 1.f ? pixelClearance / drawWidth : 0.f;
	}
}

inline int nanoVgBodySampleCount(Vec size, int pointCount) {
	const int pixelBudget = std::max(
		128, int(std::ceil(pointDrawWidth(size) * 1.25f)));
	const int pointBudget = std::max(128, pointCount * 2);
	return clamp(std::min(pixelBudget, pointBudget), 128, 512);
}

inline int glBodySampleCount(Vec size, int pointCount, float absoluteZoom, bool shaderPath) {
	const float zoom = std::max(1.f, absoluteZoom);
	const float samplesPerScreenPixel = shaderPath ? 2.05f : 1.75f;
	const int pixelBudget = int(std::ceil(
		pointDrawWidth(size) * zoom * samplesPerScreenPixel));
	const int pointBudget = std::max(128, pointCount);
	return clamp(std::max(pixelBudget, pointBudget), 128, 2048);
}

struct DisplayGeometryCache {
	std::vector<Vec> points;
	std::vector<uint8_t> nearRock;
	bool valid = false;
	uint32_t waveVersion = 0;
	int rockStateIndex = -1;
	int pointCount = -1;
	int sampleCount = -1;
	Vec size = Vec(-1.f, -1.f);
	float slitherPhase = -1.f;
	float slitherAmount = -1.f;

	void invalidate() {
		valid = false;
	}

	bool ensure(Wyrm* module, Vec requestedSize, int requestedSampleCount) {
		if (!module || module->pointCount < 2 || requestedSampleCount < 2) {
			points.clear();
			nearRock.clear();
			valid = false;
			return false;
		}

		const uint32_t requestedWaveVersion =
			module->waveVersion.load(std::memory_order_acquire);
		const int requestedRockStateIndex =
			module->activeRockStateIndex.load(std::memory_order_acquire);
		const int requestedPointCount = clamp(
			module->pointCount, 2, kWyrmPointCountMax);
		const float requestedSlitherAmount = levi_math::clamp01(
			module->displaySlitherAmount.load(std::memory_order_relaxed));
		const float requestedSlitherPhase = requestedSlitherAmount > 1e-5f
			? module->uiSlitherPhase.load(std::memory_order_relaxed)
			: 0.f;

		const bool cacheValid =
			valid
			&& waveVersion == requestedWaveVersion
			&& rockStateIndex == requestedRockStateIndex
			&& pointCount == requestedPointCount
			&& sampleCount == requestedSampleCount
			&& std::fabs(size.x - requestedSize.x) <= 1e-4f
			&& std::fabs(size.y - requestedSize.y) <= 1e-4f
			&& std::fabs(slitherPhase - requestedSlitherPhase) <= 1e-6f
			&& std::fabs(slitherAmount - requestedSlitherAmount) <= 1e-6f;
		if (cacheValid) {
			module->perfBodySampleCacheHits.fetch_add(1u, std::memory_order_relaxed);
			return false;
		}

		module->perfBodySampleCacheMisses.fetch_add(1u, std::memory_order_relaxed);
		std::array<float, kWyrmPointCountMax> authoredPoints {};
		for (int i = 0; i < requestedPointCount; ++i) {
			authoredPoints[i] = module->getWavePoint(i);
		}

		float valueClearance = 0.f;
		float phaseClearance = 0.f;
		visualRockClearance(requestedSize, &valueClearance, &phaseClearance);
		const float drawWidth = pointDrawWidth(requestedSize);
		const float nearRockMargin = 1.5f / float(requestedSampleCount);
		points.resize(size_t(requestedSampleCount));
		nearRock.resize(size_t(requestedSampleCount));
		for (int i = 0; i < requestedSampleCount; ++i) {
			const float phase = (float(i) + 0.5f) / float(requestedSampleCount);
			const float raw = catmullPeriodic(authoredPoints, requestedPointCount, phase);
			const float base = module->resolveAgainstRocks(
				raw, raw, phase, valueClearance, phaseClearance);
			const float slither = requestedSlitherAmount > 1e-5f
				? slitherOffset(phase, requestedSlitherPhase, requestedSlitherAmount)
				: 0.f;
			const float value = module->resolveAgainstRocks(
				base, base + slither, phase, valueClearance, phaseClearance);
			points[size_t(i)] = Vec(
				kPointEdgeInsetPx + phase * drawWidth,
				(0.5f - 0.5f * clamp(value, -1.f, 1.f)) * requestedSize.y);

			bool closeToRock = false;
			for (int rockIndex = 0; rockIndex < module->rockCount; ++rockIndex) {
				const WyrmRock& rock = module->rocks[rockIndex];
				const float radius = rock.radiusPhase + phaseClearance + nearRockMargin;
				if (std::fabs(module->rockDx(phase, rock)) <= radius) {
					closeToRock = true;
					break;
				}
			}
			nearRock[size_t(i)] = closeToRock ? 1u : 0u;
		}

		waveVersion = requestedWaveVersion;
		rockStateIndex = requestedRockStateIndex;
		pointCount = requestedPointCount;
		sampleCount = requestedSampleCount;
		size = requestedSize;
		slitherPhase = requestedSlitherPhase;
		slitherAmount = requestedSlitherAmount;
		valid = true;
		return true;
	}
};

} // namespace wyrm_render
