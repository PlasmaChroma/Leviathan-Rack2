#pragma once

#include "BifurxDisplay.hpp"
#include "BifurxDebug.hpp"
#include "DebugTerminalTransport.hpp"

#include <fstream>
#include <ctime>
#include <string>
#include <vector>

namespace bifurx {

struct BifurxSpectrumWidget final : Widget, BifurxSpectrumBase {
	using CurveDebugRecorder = BifurxCurveDebugRecorder;
	using PerfDebugRecorder = BifurxPerfDebugRecorder;

	widget::FramebufferWidget* framebuffer = nullptr;
	float curveX[kCurvePointCount] {};
	float curveY[kCurvePointCount] {};
	float bottomY = 0.f;
	float cachedCurveXPlotX = NAN;
	float cachedCurveXUsableW = NAN;
	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;
	bool lastShowModuleResponseOverlay = false;
	int lastColorScheme = -1;
	bool lastThreeColorFftGradient = false;
	double lastCurveDebugLogTimeSec = -1.0;
	uint64_t lastDrawNs = 0;
	float lastDrawMsEma = 0.f;
	float lastStepMsEma = 0.f;
	uint64_t lastDrawVertexCount = 0;
	
	BifurxLlTelemetryState llTelemetryState;
	bool hasLlTelemetry = false;
	uint32_t lastLlTelemetrySeq = 0;
	uint32_t lastRecordedPreviewSeq = 0, lastRecordedAnalysisSeq = 0;
	
	CurveDebugRecorder curveDebugRecorder;
	PerfDebugRecorder perfDebugRecorder;
	uint64_t uiStepCount = 0;
	uint64_t uiStepNs = 0;
	uint64_t uiStepMaxNs = 0;
	uint64_t uiDrawCount = 0;
	uint64_t uiDrawNs = 0;
	uint64_t uiDrawMaxNs = 0;
	uint64_t uiCurveUpdateCount = 0;
	uint64_t uiCurveUpdateNs = 0;
	uint64_t uiOverlayUpdateCount = 0;
	uint64_t uiOverlayUpdateNs = 0;
	uint64_t uiDrawSetupCount = 0;
	uint64_t uiDrawSetupNs = 0;
	uint64_t uiDrawBackgroundCount = 0;
	uint64_t uiDrawBackgroundNs = 0;
	uint64_t uiDrawExpectedCount = 0;
	uint64_t uiDrawExpectedNs = 0;
	uint64_t uiDrawOverlayCount = 0;
	uint64_t uiDrawOverlayNs = 0;
	uint64_t uiDrawCurveCount = 0;
	uint64_t uiDrawCurveNs = 0;
	uint64_t uiDrawMarkersCount = 0;
	uint64_t uiDrawMarkersNs = 0;
	std::vector<BifurxCurvePoint> refinedPoints;

	BifurxSpectrumWidget() : BifurxSpectrumBase() {
		const size_t refinedPointReserve = size_t(kCurvePointCount) + 8;
		refinedPoints.reserve(refinedPointReserve);
	}

	~BifurxSpectrumWidget() override {
		stopCurveDebugCapture();
		stopPerfDebugCapture();
	}

	void syncCurveDebugCaptureState();
	void syncPerfDebugCaptureState();
	void startCurveDebugCapture();
	void stopCurveDebugCapture();
	void startPerfDebugCapture();
	void stopPerfDebugCapture();
	void logCurveDebugSample(const BifurxPreviewState& state, const BifurxLlTelemetryState& llTelemetry,
		float peakAX, float peakAYCurve, float peakAYMarker, float peakBX, float peakBYCurve,
		float peakBYMarker, float uiFrameMs, uint32_t previewSeq, bool previewUpdated,
		uint32_t analysisSeq, bool analysisUpdated);
	void logPerfDebugSample(const debug_terminal::TimingRangeUs& process,
		const debug_terminal::TimingRangeUs& step, const debug_terminal::TimingRangeUs& draw,
		const BifurxSpectrumBase* active, bool gl);	void recordCurveDebug(BifurxSpectrumBase& source);

	void updateCurveXCache(float plotX, float usableW) {
		if (std::fabs(cachedCurveXPlotX - plotX) < 1e-4f && std::fabs(cachedCurveXUsableW - usableW) < 1e-4f) return;
		cachedCurveXPlotX = plotX;
		cachedCurveXUsableW = usableW;
		for (int i = 0; i < kCurvePointCount; i++) {
			curveX[i] = plotX + usableW * (float(i) / float(kCurvePointCount - 1));
		}
	}

	float getTopLabelReservedWidth(const DrawArgs& args, float fontSize);
	void step() override;
	void draw(const DrawArgs& args) override;
};

} // namespace bifurx
