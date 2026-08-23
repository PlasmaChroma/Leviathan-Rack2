#pragma once

#include "Wyrm.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace wyrm_render {

constexpr float kPointEdgeInsetPx = 2.2f;

struct BodyLayerMaterial {
	float widthPx;
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct BodyMaterial {
	std::array<BodyLayerMaterial, 3> layers;
	float edgeSoftness;

	BodyMaterial()
		: layers {{
			BodyLayerMaterial {4.0f, 74, 54, 24, 205},
			BodyLayerMaterial {2.6f, 167, 132, 72, 230},
			BodyLayerMaterial {1.15f, 246, 215, 136, 225},
		}}
		, edgeSoftness(0.205f) {
	}
};

inline const BodyMaterial& bodyMaterial() {
	static const BodyMaterial material;
	return material;
}

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
	// Keep authored geometry stable across Rack zoom transitions. The backing
	// framebuffer already follows display scale, so multiplying sample density
	// by absolute zoom only causes geometry/texture churn and can force Wyrm's
	// specialized body shader to be synchronously recompiled mid-transition.
	(void) absoluteZoom;
	const float samplesPerScreenPixel = shaderPath ? 1.35f : 1.20f;
	const int pixelBudget = int(std::ceil(
		pointDrawWidth(size) * samplesPerScreenPixel));
	const int pointBudget = std::max(128, pointCount * 2);
	return clamp(std::min(pixelBudget, pointBudget), 128, 512);
}

enum class DisplayGeometryRequirement {
	PointsOnly,
	PointsAndNearRock,
};

struct DisplayGeometryCache {
	std::vector<Vec> points;
	std::vector<uint8_t> nearRock;
	uint64_t revision = 0;
	bool valid = false;
	bool nearRockValid = false;
	uint32_t waveVersion = 0;
	int rockStateIndex = -1;
	int pointCount = -1;
	int sampleCount = -1;
	Vec size = Vec(-1.f, -1.f);
	float slitherPhase = -1.f;
	float slitherAmount = -1.f;

	void invalidate() {
		valid = false;
		nearRockValid = false;
	}

	bool ensure(Wyrm* module, Vec requestedSize, int requestedSampleCount,
	            DisplayGeometryRequirement requirement) {
		if (!module || module->pointCount < 2 || requestedSampleCount < 2) {
			points.clear();
			nearRock.clear();
			valid = false;
			nearRockValid = false;
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
		if (cacheValid && (requirement == DisplayGeometryRequirement::PointsOnly || nearRockValid)) {
			module->perfBodySampleCacheHits.fetch_add(1u, std::memory_order_relaxed);
			return false;
		}
		if (cacheValid) {
			const float nearRockMargin = 1.5f / float(requestedSampleCount);
			float phaseClearance = 0.f;
			visualRockClearance(requestedSize, nullptr, &phaseClearance);
			nearRock.assign(size_t(requestedSampleCount), 0u);
			for (int i = 0; i < requestedSampleCount; ++i) {
				const float phase = (float(i) + 0.5f) / float(requestedSampleCount);
				for (int rockIndex = 0; rockIndex < module->rockCount; ++rockIndex) {
					const WyrmRock& rock = module->rocks[rockIndex];
					const float radius = rock.radiusPhase + phaseClearance + nearRockMargin;
					if (std::fabs(module->rockDx(phase, rock)) <= radius) {
						nearRock[size_t(i)] = 1u;
						break;
					}
				}
			}
			nearRockValid = true;
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
		points.resize(size_t(requestedSampleCount));
		nearRock.clear();
		nearRockValid = false;
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
		}

		waveVersion = requestedWaveVersion;
		rockStateIndex = requestedRockStateIndex;
		pointCount = requestedPointCount;
		sampleCount = requestedSampleCount;
		size = requestedSize;
		slitherPhase = requestedSlitherPhase;
		slitherAmount = requestedSlitherAmount;
		++revision;
		if (revision == 0) {
			revision = 1;
		}
		valid = true;
		if (requirement == DisplayGeometryRequirement::PointsAndNearRock) {
			ensure(module, requestedSize, requestedSampleCount, requirement);
		}
		return true;
	}
};

} // namespace wyrm_render
