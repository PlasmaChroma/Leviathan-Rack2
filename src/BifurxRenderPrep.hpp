#pragma once

#include "BifurxRenderData.hpp"

namespace bifurx {

// Shared by the visual worker and synchronous renderer; preserves temporal smoothing.
void prepareOverlayTargetsFromSpectra(
	float sampleRate,
	const float* curveBinPos,
	const float* fftOutputFreq,
	const float* fftRawInputFreq,
	bool moduleResponseEnabled,
	bool hasOverlayTarget,
	bool fftScaleDynamic,
	float* overlayTargetModuleDb,
	float* overlayTargetOutputDbfs,
	float* displayTopTargetDbfs
);

void prepareCurveSnapshot(const BifurxUiRenderRequest& request, BifurxUiRenderSnapshot* snapshot);

} // namespace bifurx

