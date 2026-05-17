#pragma once

#include "Bifurx.hpp"

#include <cstdint>

namespace bifurx {

struct BifurxUiRenderRequest {
	uint64_t displayId = 0;
	uint64_t requestSeq = 0;
	uint32_t previewSeq = 0;
	uint32_t analysisSeq = 0;
	bool skipCurvePrep = false;
	double requestSubmittedAtSec = 0.0;
	double sourcePreviewTimeSec = 0.0;
	BifurxPreviewState previewState;
	bool hasAnalysisFrame = false;
	bool fftScaleDynamic = true;
	bool hasOverlayTarget = false;
	float previousOverlayTargetModuleDb[kCurvePointCount] = {};
	float previousOverlayTargetOutputDbfs[kCurvePointCount] = {};
	alignas(16) float analysisRawInput[kFftSize] = {};
	alignas(16) float analysisOutput[kFftSize] = {};
	alignas(16) float analysisResponseOutput[kFftSize] = {};
};

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
