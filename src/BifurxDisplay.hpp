#pragma once

#include "BifurxModule.hpp"
#include "BifurxPreview.hpp"
#include "BifurxRenderClient.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

namespace bifurx {

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t);
float displayOnlyColorTone(float energy, float shapeControl);
void formatFrequencyLabel(float hz, char* out, size_t outSize);

struct BifurxColors {
	NVGcolor low;
	NVGcolor high;
	NVGcolor white;
	static BifurxColors get(Bifurx::ColorScheme scheme, bool threeColorGradient = false);
};

#if defined(BIFURX_DISPLAY_TEST_HOOKS)
bool synchronousOverlayScratchAllocatedForCurrentThreadForTest();
const void* synchronousOverlayScratchIdentityForTest();
size_t synchronousOverlayScratchBytesForTest();
#endif

struct BifurxSpectrumState {
	float curveHz[kCurvePointCount];
	float curveBinPos[kCurvePointCount];
	float curveDb[kCurvePointCount];
	float curveTargetDb[kCurvePointCount];
	float overlayModuleDb[kCurvePointCount];
	float overlayTargetModuleDb[kCurvePointCount];
	float overlayOutputDbfs[kCurvePointCount];
	float overlayTargetOutputDbfs[kCurvePointCount];
	float displayTopDbfs = kDisplayTopDbfsCeiling;
	float displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	// Keep the dynamic reference even in fixed mode; toggles need no new FFT.
	float dynamicTopTargetDbfs = kDisplayTopDbfsCeiling;
	bool fftScaleDynamic = true;
	float colorShape = 0.f;
	uint64_t curveRevision = 0;
	uint32_t curvePreviewSeq = 0;
	uint32_t overlayAnalysisSeq = 0;
	float cachedAxisSampleRate = 0.f;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastAnalysisSeq = 0;
	double previewPublishTimeSec = 0.0;
	bool hasPreview = false;
	bool hasOverlay = false;
	bool hasCurve = false; // First valid curve has arrived; independent of animation.
	bool hasCurveTarget = false; // Curve interpolation is active.
	bool hasOverlayTarget = false; // Spectrum or scale interpolation is active.
	BifurxPreviewState previewState;
	BifurxPreviewState displayedPreviewState;
};

struct BifurxCurvePoint {
	float x01;
	float y;
	int priority; // 0: regular, 1: refinement, 2: anchor pin
};

struct BifurxMarkerLayout {
	struct Marker {
		float x;
		float yCurve;
		float yMarker;
		float hz;
		bool visible = false;
		char label[16] = {};
	};
	Marker markers[2];
	float labelX[2];
	float labelY;
	float labelFontSize;
	float guideYBottom;
	bool anchorToBottomLane;
};

struct BifurxRenderTickResult {
	bool previewUpdated = false;
	bool analysisUpdated = false;
	bool animationActive = false;
	bool contentChanged = false; // Includes the last step, when animationActive becomes false.
	float curvePrepUs = 0.f;
	float overlayPrepUs = 0.f;
};

struct BifurxSpectrumBase {
	Bifurx* module = nullptr;
	Bifurx* lastBoundModule = nullptr;
	BifurxSpectrumState state;
	BifurxRenderClient renderClient;
	bool presentationActive = true;
	float lastSurfaceRenderUs = 0.f;

	uint32_t lastModelUpdateSeq = 0;
	uint32_t lastAnalysisGeneration = 0;
	mutable BifurxPreviewModel cachedModel;
	float lastCurvePrepUs = 0.f;
	float lastOverlayPrepUs = 0.f;
	struct RefinedCurveTemplatePoint {
		float x01 = 0.f;
		int priority = 0;
		int gridIndex = -1;

		RefinedCurveTemplatePoint() = default;
		RefinedCurveTemplatePoint(float x, int p, int g = -1) : x01(x), priority(p), gridIndex(g) {}
	};
	mutable std::vector<RefinedCurveTemplatePoint> refinedCurveTemplate;
	mutable bool refinedCurveTemplateValid = false;
	mutable float refinedCurveTemplateW = 0.f;
	mutable float refinedCurveTemplateH = 0.f;
	mutable float refinedCurveTemplateSampleRate = 0.f;
	mutable float refinedCurveTemplateAnchorX01[2] = {0.f, 0.f};
	mutable bool refinedCurveTemplateMarkerPinned[2] = {false, false};
	mutable BifurxMarkerLayout cachedMarkerLayout {};
	mutable bool cachedMarkerLayoutValid = false;
	mutable float cachedMarkerLayoutW = 0.f;
	mutable float cachedMarkerLayoutH = 0.f;
	mutable float cachedMarkerLayoutSampleRate = 0.f;
	mutable uint32_t cachedMarkerLayoutPreviewSeq = 0;
	mutable uint64_t cachedMarkerLayoutCurveRevision = 0;
	mutable float cachedMarkerLayoutAnchorX01[2] = {0.f, 0.f};
	mutable bool cachedMarkerLayoutMarkerPinned[2] = {false, false};

	BifurxSpectrumBase() {
		for (int i = 0; i < kCurvePointCount; i++) {
			state.curveDb[i] = kResponseMinDb;
			state.curveTargetDb[i] = kResponseMinDb;
			state.overlayModuleDb[i] = 0.f;
			state.overlayTargetModuleDb[i] = 0.f;
			state.overlayOutputDbfs[i] = kOverlayDbfsFloor;
			state.overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
		}
	}

	virtual ~BifurxSpectrumBase();

	void syncBase();
	bool shouldUseVisualWorker() const;
	int effectiveVisualWorkerMode() const;
	float workerSnapshotAgeMs() const;
	float workerQueueLatencyMs() const;
	bool ensureWorkerRegistration();
	void releaseWorkerRegistration();
	bool submitWorkerCurveRequest();
	bool adoptWorkerCurveSnapshot();
	void initializeStaticPreviewStateIfNeeded();
	void updateAxisCache();
	void updateCurveCache();
	const BifurxPreviewModel& getOrUpdateModel() const;
	bool updateOverlayCache(uint32_t* copiedSeq = nullptr);
	bool updateAnimation(float dt, bool* contentChanged = nullptr);
	BifurxRenderTickResult runRenderTick(float dt);
	virtual void drawNanoVG(const rack::widget::Widget::DrawArgs& args) {}

	int markerAnchorKind(int markerIndex) const {
		switch (state.displayedPreviewState.mode) {
			case 2: return (markerIndex == 0) ? -1 : 1;
			case 3: return -1;
			case 7: return (markerIndex == 1) ? -1 : 1;
			default: return 0;
		}
	}

	bool markerPinnedToBottomLane(int markerIndex) const {
		switch (state.displayedPreviewState.mode) {
			case 2: return markerIndex == 0; // Notch + Low
			case 3: return true;            // Notch + Notch
			case 7: return markerIndex == 1; // High + Notch
			default: return false;
		}
	}

	struct DisplayAnchor { float x01 = 0.f; float hz = 0.f; };
	DisplayAnchor displayAnchorForMarker(int markerIndex, float targetHz, float minHz, float maxHz) const {
		const float clampedHz = clamp(targetHz, minHz, maxHz);
		DisplayAnchor anchor; anchor.x01 = logPosition(clampedHz, minHz, maxHz); anchor.hz = clampedHz;
		(void)markerIndex;
		return anchor;
	}

	float curveYAtX01(float x01, float spectrumBottomY, float spectrumTopY) const {
		auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
		const float curveIndex = clamp(x01, 0.f, 1.f) * float(kCurvePointCount - 1);
		const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
		return responseYForDb(mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0)));
	}

	void calculateMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const;
	void getCachedMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const;
	void calculateRefinedCurvePoints(std::vector<BifurxCurvePoint>* points, float w, float h) const;
};

} // namespace bifurx
