#pragma once

#include "Bifurx.hpp"

#include <cstdint>
#include <memory>

namespace bifurx {

struct BifurxUiRenderPayload {
	BifurxAnalysisFrame analysisFrame;
	bool hasOverlayTarget = false;
	float previousOverlayTargetModuleDb[kCurvePointCount] = {};
	float previousOverlayTargetOutputDbfs[kCurvePointCount] = {};
};

struct BifurxUiRenderRequest {
	uint64_t displayId = 0;
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;
	bool skipCurvePrep = false;
	double requestSubmittedAtSec = 0.0;
	double sourcePreviewTimeSec = 0.0;
	BifurxPreviewState previewState;
	bool fftScaleDynamic = true;
	bool showModuleResponseOverlay = false;
	// Immutable lease on a bounded per-display pool slot. The only bulk copy is
	// from the audio module's published frame into this slot; request coalescing
	// and worker pickup move/copy only this shared ownership handle.
	std::shared_ptr<const BifurxUiRenderPayload> payload;
};

static_assert(sizeof(BifurxUiRenderRequest) < 256, "Bifurx worker requests must not embed FFT or curve payload arrays.");

struct BifurxUiRenderSnapshot {
	uint64_t displayId = 0;
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;
	double requestSubmittedAtSec = 0.0;
	double sourcePreviewTimeSec = 0.0;
	double completedAtSec = 0.0;
	float cachedAxisSampleRate = 0.f;
	float curveHz[kCurvePointCount] = {};
	float curveBinPos[kCurvePointCount] = {};
	float curveTargetDb[kCurvePointCount] = {};
	float overlayTargetModuleDb[kCurvePointCount] = {};
	float overlayTargetOutputDbfs[kCurvePointCount] = {};
	float displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	float curvePrepUs = 0.f;
	float overlayPrepUs = 0.f;
	bool hasCurveTarget = false;
	bool hasOverlayTarget = false;
};

} // namespace bifurx
