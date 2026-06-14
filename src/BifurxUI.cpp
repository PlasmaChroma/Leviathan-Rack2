#include "Bifurx.hpp"
#include "DebugTerminalTransport.hpp"
#include "BifurxWorker.hpp"
#include "VisualAssets.hpp"
#include <unordered_map>

namespace bifurx {

static constexpr double kDebugTerminalSubmitIntervalSec = 1.0 / 8.0;
static std::unordered_map<uint32_t, double> gDebugTerminalLastSubmitSec;

struct BifurxSpectrumWidget final : Widget, BifurxSpectrumBase {
	struct CurveDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
	};
	struct PerfDebugRecorder {
		bool active = false;
		std::ofstream file;
		std::string path;
		double startTimeSec = 0.0;
		uint64_t sequence = 0;
		double lastLogTimeSec = -1.0;
		uint64_t lastAudioSampledCount = 0;
		uint64_t lastAudioProcessNs = 0;
		uint64_t lastAudioControlsNs = 0;
		uint64_t lastAudioCoreNs = 0;
		uint64_t lastAudioPreviewNs = 0;
		uint64_t lastAudioAnalysisNs = 0;
		uint64_t lastUiStepCount = 0;
		uint64_t lastUiStepNs = 0;
		uint64_t lastUiDrawCount = 0;
		uint64_t lastUiDrawNs = 0;
		uint64_t lastUiCurveUpdateCount = 0;
		uint64_t lastUiCurveUpdateNs = 0;
		uint64_t lastUiOverlayUpdateCount = 0;
		uint64_t lastUiOverlayUpdateNs = 0;
		uint64_t lastUiDrawSetupCount = 0;
		uint64_t lastUiDrawSetupNs = 0;
		uint64_t lastUiDrawBackgroundCount = 0;
		uint64_t lastUiDrawBackgroundNs = 0;
		uint64_t lastUiDrawExpectedCount = 0;
		uint64_t lastUiDrawExpectedNs = 0;
		uint64_t lastUiDrawOverlayCount = 0;
		uint64_t lastUiDrawOverlayNs = 0;
		uint64_t lastUiDrawCurveCount = 0;
		uint64_t lastUiDrawCurveNs = 0;
		uint64_t lastUiDrawMarkersCount = 0;
		uint64_t lastUiDrawMarkersNs = 0;
	};

	widget::FramebufferWidget* framebuffer = nullptr;
	float curveX[kCurvePointCount];
	float curveY[kCurvePointCount];
	float bottomY = 0.f;
	float cachedCurveXPlotX = NAN;
	float cachedCurveXUsableW = NAN;
	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;
	bool lastFftScaleDynamic = true;
	bool lastShowModuleResponseOverlay = false;
	double lastCurveDebugLogTimeSec = -1.0;
	uint64_t lastDrawNs = 0;
	float lastDrawMsEma = 0.f;
	float lastStepMsEma = 0.f;
	uint64_t lastDrawVertexCount = 0;
	float lastCurvePrepUs = 0.f;
	float lastOverlayPrepUs = 0.f;
	
	BifurxLlTelemetryState llTelemetryState;
	bool hasLlTelemetry = false;
	uint32_t lastLlTelemetrySeq = 0;
	
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

	void syncCurveDebugCaptureState() {
		if (!module) return;
		if (!isDragonKingDebugEnabled()) {
			if (curveDebugRecorder.active) {
				stopCurveDebugCapture();
			}
			return;
		}
		if (module->curveDebugLogging.load(std::memory_order_relaxed) && !curveDebugRecorder.active) {
			startCurveDebugCapture();
		}
		else if (!module->curveDebugLogging.load(std::memory_order_relaxed) && curveDebugRecorder.active) {
			stopCurveDebugCapture();
		}
	}

	void syncPerfDebugCaptureState() {
		if (!module) return;
		if (!isDragonKingDebugEnabled()) {
			if (perfDebugRecorder.active) {
				stopPerfDebugCapture();
			}
			return;
		}
		if (module->perfDebugLogging.load(std::memory_order_relaxed) && !perfDebugRecorder.active) {
			startPerfDebugCapture();
		}
		else if (!module->perfDebugLogging.load(std::memory_order_relaxed) && perfDebugRecorder.active) {
			stopPerfDebugCapture();
		}
	}

	void startCurveDebugCapture() {
		if (curveDebugRecorder.active) return;
		system::createDirectories(bifurxUserRootPath());
		curveDebugRecorder.path = system::join(bifurxUserRootPath(), "curve_debug_" + std::to_string(std::time(nullptr)) + ".csv");
		curveDebugRecorder.file.open(curveDebugRecorder.path);
		if (curveDebugRecorder.file.is_open()) {
			curveDebugRecorder.file << "sequence,previewSeq,analysisSeq,previewUpdated,analysisUpdated,"
				<< "freqA,freqB,qA,qB,balance,resoNorm,spanNorm,"
				<< "excitationRms,stageALpRms,stageBLpRms,outputRms,stageBLpOverALpDb,outputOverInputDb,"
				<< "peakAX,peakAYCurve,peakAYMarker,peakBX,peakBYCurve,peakBYMarker,uiFrameMs\n";
			curveDebugRecorder.active = true;
			curveDebugRecorder.startTimeSec = system::getTime();
			curveDebugRecorder.sequence = 0;
			DEBUG("Started curve debug capture: %s", curveDebugRecorder.path.c_str());
		}
	}

	void stopCurveDebugCapture() {
		if (!curveDebugRecorder.active) return;
		curveDebugRecorder.file.close();
		curveDebugRecorder.active = false;
		DEBUG("Stopped curve debug capture");
	}

	void startPerfDebugCapture() {
		if (perfDebugRecorder.active) return;
		system::createDirectories(bifurxUserRootPath());
		perfDebugRecorder.path = system::join(bifurxUserRootPath(), "perf_debug_" + std::to_string(std::time(nullptr)) + ".csv");
		perfDebugRecorder.file.open(perfDebugRecorder.path);
		if (perfDebugRecorder.file.is_open()) {
			perfDebugRecorder.file << "sequence,mode,fastPath,pitchCvConnected,"
				<< "audioSampleRate,audioSampledCount,"
				<< "audioProcessAvgNs,audioControlsAvgNs,audioCoreAvgNs,audioPreviewAvgNs,audioAnalysisAvgNs,audioProcessMaxNs,"
				<< "uiStepCount,uiStepAvgNs,uiDrawCount,uiDrawAvgNs,"
				<< "uiCurveUpdateCount,uiCurveUpdateAvgNs,uiOverlayUpdateCount,uiOverlayUpdateAvgNs,"
				<< "uiDrawSetupAvgNs,uiDrawBackgroundAvgNs,uiDrawExpectedAvgNs,uiDrawOverlayAvgNs,uiDrawCurveAvgNs,uiDrawMarkersAvgNs\n";
			perfDebugRecorder.active = true;
			perfDebugRecorder.startTimeSec = system::getTime();
			perfDebugRecorder.sequence = 0;
			perfDebugRecorder.lastLogTimeSec = -1.0;
			if (module) module->resetPerfStats();
			uiStepCount = 0; uiStepNs = 0; uiStepMaxNs = 0;
			uiDrawCount = 0; uiDrawNs = 0; uiDrawMaxNs = 0;
			uiCurveUpdateCount = 0; uiCurveUpdateNs = 0;
			uiOverlayUpdateCount = 0; uiOverlayUpdateNs = 0;
			uiDrawSetupCount = 0; uiDrawSetupNs = 0;
			uiDrawBackgroundCount = 0; uiDrawBackgroundNs = 0;
			uiDrawExpectedCount = 0; uiDrawExpectedNs = 0;
			uiDrawOverlayCount = 0; uiDrawOverlayNs = 0;
			uiDrawCurveCount = 0; uiDrawCurveNs = 0;
			uiDrawMarkersCount = 0; uiDrawMarkersNs = 0;
			DEBUG("Started performance debug capture: %s", perfDebugRecorder.path.c_str());
		}
	}

	void stopPerfDebugCapture() {
		if (!perfDebugRecorder.active) return;
		perfDebugRecorder.file.close();
		perfDebugRecorder.active = false;
		DEBUG("Stopped performance debug capture");
	}

	void logCurveDebugSample(
		const BifurxPreviewState& state,
		const BifurxLlTelemetryState& llTelemetry,
		float peakAX,
		float peakAYCurve,
		float peakAYMarker,
		float peakBX,
		float peakBYCurve,
		float peakBYMarker,
		float uiFrameMs,
		uint32_t previewSeq,
		bool previewUpdated,
		uint32_t analysisSeq,
		bool analysisUpdated
	) {
		if (!curveDebugRecorder.active) return;
		curveDebugRecorder.file << curveDebugRecorder.sequence++ << ","
			<< previewSeq << "," << analysisSeq << "," << (previewUpdated ? 1 : 0) << "," << (analysisUpdated ? 1 : 0) << ","
			<< state.freqA << "," << state.freqB << "," << state.qA << "," << state.qB << "," << state.balance << "," << state.resoNorm << "," << state.spanNorm << ","
			<< llTelemetry.excitationRms << "," << llTelemetry.stageALpRms << "," << llTelemetry.stageBLpRms << "," << llTelemetry.outputRms << ","
			<< llTelemetry.stageBLpOverALpDb << "," << llTelemetry.outputOverInputDb << ","
			<< peakAX << "," << peakAYCurve << "," << peakAYMarker << ","
			<< peakBX << "," << peakBYCurve << "," << peakBYMarker << ","
			<< uiFrameMs << "\n";
	}

	void logPerfDebugSample() {
		if (!perfDebugRecorder.active || !module) return;
		const uint64_t audioSampledCount = module->perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
		const double audioScale = (audioSampledCount > 0) ? (1.0 / double(audioSampledCount)) : 0.0;
		const uint64_t audioProcessNs = module->perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
		const uint64_t audioControlsNs = module->perfAudioControlsNs.exchange(0, std::memory_order_acq_rel);
		const uint64_t audioCoreNs = module->perfAudioCoreNs.exchange(0, std::memory_order_acq_rel);
		const uint64_t audioPreviewNs = module->perfAudioPreviewNs.exchange(0, std::memory_order_acq_rel);
		const uint64_t audioAnalysisNs = module->perfAudioAnalysisNs.exchange(0, std::memory_order_acq_rel);
		const uint64_t audioProcessMaxNs = module->perfAudioProcessMaxNs.exchange(0, std::memory_order_acq_rel);

		auto avg = [](uint64_t total, uint64_t count) { return (count > 0) ? (double(total) / double(count)) : 0.0; };

		perfDebugRecorder.file << perfDebugRecorder.sequence++ << ","
			<< module->perfMode.load() << ","
			<< (module->perfFastPathEligible.load() ? 1 : 0) << "," << (module->perfPreviewPitchCvConnected.load() ? 1 : 0) << ","
			<< module->perfSampleRate.load() << "," << audioSampledCount << ","
			<< (double(audioProcessNs) * audioScale) << "," << (double(audioControlsNs) * audioScale) << ","
			<< (double(audioCoreNs) * audioScale) << "," << (double(audioPreviewNs) * audioScale) << ","
			<< (double(audioAnalysisNs) * audioScale) << "," << audioProcessMaxNs << ","
			<< uiStepCount << "," << avg(uiStepNs, uiStepCount) << ","
			<< uiDrawCount << "," << avg(uiDrawNs, uiDrawCount) << ","
			<< uiCurveUpdateCount << "," << avg(uiCurveUpdateNs, uiCurveUpdateCount) << ","
			<< uiOverlayUpdateCount << "," << avg(uiOverlayUpdateNs, uiOverlayUpdateCount) << ","
			<< avg(uiDrawSetupNs, uiDrawSetupCount) << "," << avg(uiDrawBackgroundNs, uiDrawBackgroundCount) << ","
			<< avg(uiDrawExpectedNs, uiDrawExpectedCount) << "," << avg(uiDrawOverlayNs, uiDrawOverlayCount) << ","
			<< avg(uiDrawCurveNs, uiDrawCurveCount) << "," << avg(uiDrawMarkersNs, uiDrawMarkersCount) << "\n";

		uiStepCount = 0; uiStepNs = 0; uiStepMaxNs = 0;
		uiDrawCount = 0; uiDrawNs = 0; uiDrawMaxNs = 0;
		uiCurveUpdateCount = 0; uiCurveUpdateNs = 0;
		uiOverlayUpdateCount = 0; uiOverlayUpdateNs = 0;
		uiDrawSetupCount = 0; uiDrawSetupNs = 0;
		uiDrawBackgroundCount = 0; uiDrawBackgroundNs = 0;
		uiDrawExpectedCount = 0; uiDrawExpectedNs = 0;
		uiDrawOverlayCount = 0; uiDrawOverlayNs = 0;
		uiDrawCurveCount = 0; uiDrawCurveNs = 0;
		uiDrawMarkersCount = 0; uiDrawMarkersNs = 0;
	}

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

float BifurxSpectrumWidget::getTopLabelReservedWidth(const DrawArgs& args, float fontSize) {
	const int fontHandle = (APP && APP->window && APP->window->uiFont) ? APP->window->uiFont->handle : -1;
	if (fontHandle == cachedTopLabelFontHandle &&
		std::isfinite(cachedTopLabelFontSize) &&
		std::fabs(cachedTopLabelFontSize - fontSize) <= 1e-5f &&
		cachedTopLabelReservedWidth > 0.f) {
		return cachedTopLabelReservedWidth;
	}

	auto compactSignedLabel = [](float value, char* out, size_t outSize) {
		std::snprintf(out, outSize, "%+.1f", value);
	};
	auto measureTopLabelWidthForValue = [&](float db) {
		char sampleValue[12];
		compactSignedLabel(db, sampleValue, sizeof(sampleValue));
		char sampleLabel[24];
		std::snprintf(sampleLabel, sizeof(sampleLabel), "%5s dBFS", sampleValue);
		return nvgTextBounds(args.vg, 0.f, 0.f, sampleLabel, nullptr, nullptr);
	};

	float topLabelReservedWidth = 0.f;
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsFloor));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-10.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(-1.f));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDbfsCeiling));
	topLabelReservedWidth = std::max(topLabelReservedWidth, measureTopLabelWidthForValue(kDisplayTopDynamicCeilingDbfs));

	cachedTopLabelFontHandle = fontHandle;
	cachedTopLabelFontSize = fontSize;
	cachedTopLabelReservedWidth = topLabelReservedWidth;
	return topLabelReservedWidth;
}

void BifurxSpectrumWidget::step() {
	using PerfClock = std::chrono::steady_clock;
	const PerfClock::time_point perfStepStart = PerfClock::now();
	const bool perfLoggingActive = module && module->perfDebugLogging.load(std::memory_order_relaxed);
	Widget::step();
	syncCurveDebugCaptureState();
	syncPerfDebugCaptureState();
	if (!module) {
		const bool hadPreview = state.hasPreview;
		initializeStaticPreviewStateIfNeeded();
		if (!hadPreview && framebuffer) {
			framebuffer->dirty = true;
		}
		return;
	}
	if (module->renderMode != Bifurx::RENDER_NANOVG) return;

	bool dirty = false;
	bool previewUpdated = false;
	bool analysisUpdated = false;

	const bool fftScaleDynamicNow = module->fftScaleDynamic.load(std::memory_order_relaxed);
	if (fftScaleDynamicNow != lastFftScaleDynamic) {
		lastFftScaleDynamic = fftScaleDynamicNow;
		if (!fftScaleDynamicNow) {
			state.displayTopDbfs = kDisplayTopDbfsCeiling;
			state.displayTopTargetDbfs = kDisplayTopDbfsCeiling;
		}
		dirty = true;
	}
	const bool showModuleResponseOverlayNow = module->showModuleResponseOverlay.load(std::memory_order_relaxed);
	if (showModuleResponseOverlayNow != lastShowModuleResponseOverlay) {
		lastShowModuleResponseOverlay = showModuleResponseOverlayNow;
		dirty = true;
	}

	float uiFrameSec = 1.f / 60.f;
	if (APP && APP->window) {
		const float frameSec = float(APP->window->getLastFrameDuration());
		if (std::isfinite(frameSec) && frameSec > 0.f) {
			uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
		}
	}

	const BifurxRenderTickResult tick = runRenderTick(uiFrameSec);
	previewUpdated = tick.previewUpdated;
	analysisUpdated = tick.analysisUpdated;
	if (tick.curvePrepUs > 0.f) {
		lastCurvePrepUs = tick.curvePrepUs;
	}
	if (tick.overlayPrepUs > 0.f) {
		lastOverlayPrepUs = tick.overlayPrepUs;
	}
	if (previewUpdated || analysisUpdated || tick.animationActive) {
		dirty = true;
	}

	const uint32_t llTelemetrySeq = module->llTelemetryPublishSeq.load(std::memory_order_acquire);
	if (llTelemetrySeq != lastLlTelemetrySeq) {
		const int index = module->llTelemetryPublishedIndex.load(std::memory_order_acquire);
		llTelemetryState = module->llTelemetryStates[index];
		hasLlTelemetry = true;
		lastLlTelemetrySeq = llTelemetrySeq;
	}

	if (isDragonKingDebugEnabled() && module->curveDebugLogging.load(std::memory_order_relaxed) && state.hasPreview) {
		const double nowSec = system::getTime();
		const double minIntervalSec = 1.0 / 60.0;
		if (lastCurveDebugLogTimeSec < 0.0 || (nowSec - lastCurveDebugLogTimeSec) >= minIntervalSec) {
			lastCurveDebugLogTimeSec = nowSec;

			float uiFrameMs = NAN;
			if (APP && APP->window) {
				const double frameSec = APP->window->getLastFrameDuration();
				if (std::isfinite(frameSec) && frameSec > 0.0) {
					uiFrameMs = float(frameSec * 1000.0);
				}
			}

			float peakAX = NAN, peakAYCurve = NAN, peakAYMarker = NAN;
			float peakBX = NAN, peakBYCurve = NAN, peakBYMarker = NAN;
			const float w = box.size.x, h = box.size.y;
			if (w > 0.f && h > 0.f) {
				const BifurxPreviewModel& model = getOrUpdateModel();
				const float padY = std::max(4.f, h * 0.035f);
				const float plotX = 0.f, usableW = std::max(1.f, w - plotX);
				const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
				const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
				const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
				auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
				auto evalPeak = [&](int idx, float targetHz, float* outX, float* outYCurve, float* outYMarker) {
					const auto anchor = displayAnchorForMarker(idx, targetHz, minHz, maxHz);
					const float markerRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
					const float curveIndex = anchor.x01 * float(kCurvePointCount - 1);
					const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
					const float curveDbAtHz = mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0));
					const float yCurve = responseYForDb(curveDbAtHz), markerX = plotX + usableW * anchor.x01;
					const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding, markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
					const float yMarker = markerPinnedToBottomLane(idx) ? (spectrumBottomY - markerRadius - kPeakMarkerBottomLanePadding) : clamp(yCurve, markerMinY, markerMaxY);
					*outX = clamp(markerX, plotX + markerRadius + kPeakMarkerEdgePadding, plotX + usableW - markerRadius - kPeakMarkerEdgePadding);
					*outYCurve = yCurve; *outYMarker = yMarker;
				};
				evalPeak(0, model.markerFreqA, &peakAX, &peakAYCurve, &peakAYMarker);
				evalPeak(1, model.markerFreqB, &peakBX, &peakBYCurve, &peakBYMarker);
			}

			logCurveDebugSample(state.previewState, hasLlTelemetry ? llTelemetryState : BifurxLlTelemetryState{}, peakAX, peakAYCurve, peakAYMarker, peakBX, peakBYCurve, peakBYMarker, uiFrameMs, state.lastPreviewSeq, previewUpdated, state.lastAnalysisSeq, analysisUpdated);
		}
	}
	else if (lastCurveDebugLogTimeSec >= 0.0) {
		lastCurveDebugLogTimeSec = -1.0;
	}

	if (isDragonKingDebugEnabled() && perfDebugRecorder.active) {
		const double nowSec = system::getTime();
		const double minIntervalSec = 0.5;
		if (perfDebugRecorder.lastLogTimeSec < 0.0 || (nowSec - perfDebugRecorder.lastLogTimeSec) >= minIntervalSec) {
			perfDebugRecorder.lastLogTimeSec = nowSec;
			logPerfDebugSample();
		}
	}

	if (dirty && framebuffer) framebuffer->setDirty();

	const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
		PerfClock::now() - perfStepStart).count()) * 1e-6f;
	lastStepMsEma = (lastStepMsEma > 0.f) ? (lastStepMsEma + (stepMs - lastStepMsEma) * 0.18f) : stepMs;
	
	if (isDragonKingDebugEnabled() && module->renderMode == Bifurx::RENDER_NANOVG) {
		double nowSec = system::getTime();
		uint32_t debugId = module->debugInstanceId;
		double& lastSubmitSec = gDebugTerminalLastSubmitSec[debugId];
		if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
			const uint64_t audioSampledCount = module->perfAudioSampledCount.exchange(0, std::memory_order_acq_rel);
			const uint64_t audioProcessNs = module->perfAudioProcessNs.exchange(0, std::memory_order_acq_rel);
			module->perfAudioControlsNs.store(0, std::memory_order_release);
			module->perfAudioCoreNs.store(0, std::memory_order_release);
			module->perfAudioPreviewNs.store(0, std::memory_order_release);
			module->perfAudioAnalysisNs.store(0, std::memory_order_release);
			module->perfAudioProcessMaxNs.store(0, std::memory_order_release);
			const float audioUs = (audioSampledCount > 0u) ? float(double(audioProcessNs) / double(audioSampledCount) * 0.001) : 0.f;
			const int vwMode = effectiveVisualWorkerMode();
			const float uiSyncMs = std::max(0.f, lastStepMsEma);
			const float uiDrawMs = std::max(0.f, lastDrawMsEma);
			const float uiTotalMs = uiSyncMs + uiDrawMs;
			const float uiLocalPrepMs = (vwMode == Bifurx::VISUAL_WORKER_OFF)
				? 0.001f * (std::max(0.f, lastCurvePrepUs) + std::max(0.f, lastOverlayPrepUs))
				: 0.f;
			lastSubmitSec = nowSec;
			debug_terminal::submitBifurxUiMetrics(
				debugId,
				uiTotalMs,
				uiDrawMs,
				uiSyncMs,
				uiLocalPrepMs,
				false, // opengl
				audioUs,
				lastCurvePrepUs,
				lastOverlayPrepUs,
				vwMode,
				workerSnapshotAgeMs(),
				workerQueueLatencyMs()
			);
		}
	}

	if (perfLoggingActive) {
		const uint64_t stepNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfStepStart).count();
		uiStepCount++; uiStepNs += stepNs; uiStepMaxNs = std::max(uiStepMaxNs, stepNs);
	}
}

void BifurxSpectrumWidget::draw(const DrawArgs& args) {
	if (!state.hasPreview) return;
	const float w = box.size.x, h = box.size.y;
	if (!(w > 0.f && h > 0.f)) return;
	using PerfClock = std::chrono::steady_clock;
	const bool perfLoggingActive = module && module->perfDebugLogging.load(std::memory_order_relaxed);
	const PerfClock::time_point perfDrawStart = PerfClock::now();
	PerfClock::time_point perfSectionStart = perfDrawStart;
	auto recordDrawSection = [&](uint64_t& count, uint64_t& totalNs) {
		if (!perfLoggingActive) return;
		const uint64_t ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfSectionStart).count();
		count++; totalNs += ns; perfSectionStart = PerfClock::now();
	};
	const float padY = std::max(4.f, h * 0.035f), plotX = 0.f, usableW = std::max(1.f, w - plotX);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight, spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	bottomY = spectrumBottomY;
	const float displayMaxDbfs = state.displayTopDbfs, displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
	auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
	updateCurveXCache(plotX, usableW);
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(state.previewState.mode);
	
	if (!displayOnlyMode) {
		calculateRefinedCurvePoints(&refinedPoints, w, h);
	}
	else {
		refinedPoints.clear();
	}

	recordDrawSection(uiDrawSetupCount, uiDrawSetupNs);

	nvgSave(args.vg);
	const float clipInset = 0.8f; nvgScissor(args.vg, clipInset, clipInset, std::max(0.f, w - 2.f * clipInset), std::max(0.f, h - 2.f * clipInset));
	nvgSave(args.vg); nvgScissor(args.vg, plotX, 0.f, usableW, std::max(1.f, spectrumBottomY));
	auto spectrumYForDbfs = [&](float dbfs) { return rescale(clamp(dbfs, displayMinDbfs, displayMaxDbfs), displayMinDbfs, displayMaxDbfs, spectrumBottomY, spectrumTopY); };
	const float topLabelFontSize = std::max(7.f, h * 0.05f);
	nvgFontSize(args.vg, topLabelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
	char topLabel[32]; std::snprintf(topLabel, sizeof(topLabel), "%+5.1f dBFS", displayMaxDbfs);
	const float topLabelReservedWidth = getTopLabelReservedWidth(args, topLabelFontSize);
	nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP); nvgText(args.vg, 1.5f + topLabelReservedWidth, 1.f, topLabel, nullptr);
	const float badgeFontSize = std::max(6.6f, h * 0.045f);
	nvgFontSize(args.vg, badgeFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgText(args.vg, w - 2.2f + 0.5f, 1.6f + 0.5f, "NVG", nullptr);
	nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 230)); nvgText(args.vg, w - 2.2f, 1.6f, "NVG", nullptr);
	recordDrawSection(uiDrawBackgroundCount, uiDrawBackgroundNs);

	const BifurxColors palette = BifurxColors::get(module ? module->colorScheme : Bifurx::SCHEME_DEFAULT);
	const NVGcolor expectedPurple = palette.low;
	const NVGcolor expectedCyan = palette.high;
	const NVGcolor expectedWhite = palette.white;
	
	BifurxMarkerLayout layout;
	if (!displayOnlyMode) {
		getCachedMarkerLayout(&layout, w, h);
	}

	auto drawExpectedGuideStroke = [&](float x, float y, float curveDbVal) {
		const float posAmt = clamp01(curveDbVal / 18.f), negAmt = clamp01(-curveDbVal / 18.f), emph = std::max(posAmt, negAmt);
		NVGcolor tint = expectedWhite; if (posAmt > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posAmt * 1.35f)); if (negAmt > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negAmt * 1.25f));
		tint.a = 0.025f + 0.095f * emph; nvgBeginPath(args.vg); nvgMoveTo(args.vg, x, spectrumBottomY); nvgLineTo(args.vg, x, y); nvgStrokeColor(args.vg, tint); nvgStrokeWidth(args.vg, 1.05f); nvgStroke(args.vg);
	};
	if (!displayOnlyMode) {
		const float markerGuideClearanceX01 = 1.35f / float(kCurvePointCount - 1);
		for (int i = 0; i < kCurvePointCount; i += 3) {
			const float x01 = float(i) / float(kCurvePointCount - 1);
			if (std::fabs(x01 - layout.markers[0].x / w) < markerGuideClearanceX01 || std::fabs(x01 - layout.markers[1].x / w) < markerGuideClearanceX01) continue;
			drawExpectedGuideStroke(curveX[i], responseYForDb(state.curveDb[i]), state.curveDb[i]);
		}
		
		for (int i = 0; i < 2; i++) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY); nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
			nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210)); nvgStrokeWidth(args.vg, 1.9f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY); nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
			nvgStrokeColor(args.vg, nvgRGBA(249, 236, 190, 248)); nvgStrokeWidth(args.vg, 1.25f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom); nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210)); nvgStrokeWidth(args.vg, 1.9f); nvgStroke(args.vg);
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom); nvgStrokeColor(args.vg, nvgRGBA(249, 236, 190, 248)); nvgStrokeWidth(args.vg, 1.25f); nvgStroke(args.vg);
		};
	}
	recordDrawSection(uiDrawExpectedCount, uiDrawExpectedNs);

	if (state.hasOverlay) {
		const bool showModuleResponse = !displayOnlyMode && module && module->showModuleResponseOverlay.load(std::memory_order_relaxed);
		const float displayOnlyShapeControl = module ? clamp(module->params[Bifurx::FM_AMT_PARAM].getValue(), -1.f, 1.f) : 0.f;
		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]);
			const float avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]), energy = clamp01(rescale(avgO, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
			if (energy <= 0.005f) continue;
			NVGcolor fill;
			if (displayOnlyMode) {
				fill = mixColor(expectedPurple, expectedCyan, displayOnlyColorTone(energy, displayOnlyShapeControl));
			}
			else {
				const float posA = clamp01(avgD / 18.f), negA = clamp01(-avgD / 18.f);
				NVGcolor tint = expectedWhite; if (posA > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posA * 1.40f)); if (negA > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negA * 1.25f));
				fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energy);
			}
			fill.a = 1.f;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, curveX[i] - 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i])); nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i + 1]));
			nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumBottomY); nvgLineTo(args.vg, curveX[i] - 0.45f, spectrumBottomY); nvgClosePath(args.vg); nvgFillColor(args.vg, fill); nvgFill(args.vg);
		}
		if (showModuleResponse) {
			nvgBeginPath(args.vg); for (int i = 0; i < kCurvePointCount; ++i) { float y = responseYForDb(state.overlayModuleDb[i]); if (i == 0) nvgMoveTo(args.vg, curveX[i], y); else nvgLineTo(args.vg, curveX[i], y); }
			NVGcolor ml = mixColor(expectedWhite, expectedCyan, 0.35f); ml.a = 0.95f; nvgStrokeWidth(args.vg, 1.4f); nvgStrokeColor(args.vg, ml); nvgStroke(args.vg);
		}
		recordDrawSection(uiDrawOverlayCount, uiDrawOverlayNs);
	}

	auto drawRefinedCurvePath = [&]() {
		nvgBeginPath(args.vg);
		for (int i = 0; i < (int)refinedPoints.size(); ++i) {
			if (i == 0) nvgMoveTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y);
			else nvgLineTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y);
		}
	};
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgLineCap(args.vg, NVG_ROUND);
	if (!displayOnlyMode) {
		drawRefinedCurvePath();
		nvgStrokeColor(args.vg, nvgRGBA(6, 8, 12, 210));
		nvgStrokeWidth(args.vg, 1.9f);
		nvgStroke(args.vg);
		drawRefinedCurvePath();
		nvgStrokeColor(args.vg, nvgRGBA(249, 236, 190, 248));
		nvgStrokeWidth(args.vg, 1.25f);
		nvgStroke(args.vg);
	}
	lastDrawVertexCount = uint64_t(refinedPoints.size());
	recordDrawSection(uiDrawCurveCount, uiDrawCurveNs);
	nvgRestore(args.vg);

	if (!displayOnlyMode) {
		for (int i = 0; i < 2; ++i) {
			if (!layout.markers[i].visible) continue;
			nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius); nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244)); nvgFill(args.vg);
			nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius); nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth); nvgStroke(args.vg);
		}
		nvgFontSize(args.vg, layout.labelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		for (int i = 0; i < 2; ++i) { if (!layout.markers[i].visible) continue; nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240)); nvgText(args.vg, layout.labelX[i], layout.labelY + 0.75f, layout.markers[i].label, nullptr); nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250)); nvgText(args.vg, layout.labelX[i], layout.labelY, layout.markers[i].label, nullptr); }
	}
	nvgResetScissor(args.vg); nvgRestore(args.vg);
	recordDrawSection(uiDrawMarkersCount, uiDrawMarkersNs);
	lastDrawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
	{
		const float drawMs = std::max(0.f, float(double(lastDrawNs) * 1e-6));
		lastDrawMsEma = (lastDrawMsEma > 0.f) ? (lastDrawMsEma + (drawMs - lastDrawMsEma) * 0.18f) : drawMs;
	}
	if (perfLoggingActive) {
		uiDrawCount++; uiDrawNs += lastDrawNs; uiDrawMaxNs = std::max(uiDrawMaxNs, lastDrawNs);
	}
}

struct BifurxSpectrumBackgroundWidget final : Widget {
	Bifurx* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	uint32_t lastPreviewSeq = 0;
	float sampleRate = 48000.f;
	int lastMode = -1;
	float lastDrawWidth = -1.f;
	float lastDrawHeight = -1.f;

	void step() override {
		Widget::step();
		bool dirty = false;
		if (module) {
			const uint32_t previewSeq = module->previewPublishSeq.load(std::memory_order_acquire);
			if (previewSeq != lastPreviewSeq) {
				const int index = module->previewPublishedIndex.load(std::memory_order_acquire);
				const float newSampleRate = std::max(1.f, module->previewStates[index].sampleRate);
				if (std::fabs(newSampleRate - sampleRate) > 0.5f) { sampleRate = newSampleRate; dirty = true; }
				lastPreviewSeq = previewSeq;
			}
			const int mode = clamp(int(std::round(module->params[Bifurx::MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1);
			if (mode != lastMode) { lastMode = mode; dirty = true; }
		}
		if (std::fabs(box.size.x - lastDrawWidth) > 1e-4f || std::fabs(box.size.y - lastDrawHeight) > 1e-4f) { lastDrawWidth = box.size.x; lastDrawHeight = box.size.y; dirty = true; }
		if (dirty && framebuffer) framebuffer->setDirty();
	}

	void draw(const DrawArgs& args) override {
		const float w = box.size.x; const float h = box.size.y;
		if (!(w > 0.f && h > 0.f)) return;
		const float padX = 0.f, padY = std::max(4.f, h * 0.035f), plotX = padX, usableW = std::max(1.f, w - plotX - padX), minHz = 10.f, maxHz = std::min(20000.f, 0.46f * sampleRate), labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight, spectrumBottomY = std::max(padY * 0.35f + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
		nvgSave(args.vg); nvgScissor(args.vg, 0.8f, 0.8f, std::max(0.f, w - 1.6f), std::max(0.f, h - 1.6f));
		nvgBeginPath(args.vg); nvgRect(args.vg, 0.f, 0.f, w, h); nvgFillColor(args.vg, nvgRGBA(7, 10, 14, 26)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgRect(args.vg, 0.f, labelBandTop, w, h - labelBandTop); nvgFillColor(args.vg, nvgRGBA(4, 7, 11, 208)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, 0.f, labelBandTop); nvgLineTo(args.vg, w, labelBandTop); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 20)); nvgStrokeWidth(args.vg, 1.f); nvgStroke(args.vg);
		nvgSave(args.vg); nvgScissor(args.vg, plotX, 0.f, usableW, std::max(1.f, spectrumBottomY));
		for (float dS = 10.f; dS < maxHz; dS *= 10.f) { for (int m = 1; m <= 9; ++m) { float gH = dS * float(m); if (gH >= maxHz) continue; const bool maj = (m == 1); float gX = plotX + usableW * logPosition(gH, minHz, maxHz); nvgBeginPath(args.vg); nvgMoveTo(args.vg, gX, padY * 0.35f); nvgLineTo(args.vg, gX, spectrumBottomY); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, maj ? 34 : 16)); nvgStrokeWidth(args.vg, maj ? 1.f : 0.7f); nvgStroke(args.vg); } }
		nvgBeginPath(args.vg); float y0 = responseYForDbDisplay(0.f, kResponseMinDb, kResponseMaxDb, spectrumBottomY, padY * 0.35f); nvgMoveTo(args.vg, plotX, y0); nvgLineTo(args.vg, plotX + usableW, y0); nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 24)); nvgStrokeWidth(args.vg, 1.2f); nvgStroke(args.vg);
		nvgResetScissor(args.vg); nvgRestore(args.vg);
		if (isBifurxDisplayOnlyMode(lastMode)) {
			const struct { float hz; const char* label; } marks[] = {
				{100.f, "100Hz"},
				{1000.f, "1kHz"},
				{10000.f, "10kHz"}
			};
			nvgFontSize(args.vg, std::max(7.f, h * 0.055f));
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			const float labelY = labelBandTop + 0.52f * labelBandHeight;
			for (const auto& mark : marks) {
				if (mark.hz <= minHz || mark.hz >= maxHz) continue;
				const float x = clamp(plotX + usableW * logPosition(mark.hz, minHz, maxHz), 14.f, w - 14.f);
				nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240));
				nvgText(args.vg, x, labelY + 0.75f, mark.label, nullptr);
				nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250));
				nvgText(args.vg, x, labelY, mark.label, nullptr);
			}
		}
		nvgRestore(args.vg);
	}
};

void drawModeStepTriangle(const Widget::DrawArgs& args, const Vec& size, bool pointRight) {
	const float cx = 0.5f * size.x, cy = 0.5f * size.y, hW = 2.8f, hH = 3.3f, off = pointRight ? (hW / 3.f) : (-hW / 3.f);
	nvgBeginPath(args.vg); if (pointRight) { nvgMoveTo(args.vg, cx - hW + off, cy - hH); nvgLineTo(args.vg, cx + hW + off, cy); nvgLineTo(args.vg, cx - hW + off, cy + hH); } else { nvgMoveTo(args.vg, cx + hW + off, cy - hH); nvgLineTo(args.vg, cx - hW + off, cy); nvgLineTo(args.vg, cx + hW + off, cy + hH); }
	nvgClosePath(args.vg); nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244)); nvgFill(args.vg);
}

struct BifurxModeLeftButton final : TL1105 { void draw(const DrawArgs& args) override { TL1105::draw(args); drawModeStepTriangle(args, box.size, false); } };
struct BifurxModeRightButton final : TL1105 { void draw(const DrawArgs& args) override { TL1105::draw(args); drawModeStepTriangle(args, box.size, true); } };
struct BifurxModeMenuButton final : TL1105 {
	Bifurx* module = nullptr;

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			TL1105::onButton(e);
			return;
		}
		ui::Menu* menu = createMenu();
		menu->box.pos = getAbsoluteOffset(Vec(0.f, box.size.y));
		menu->addChild(createMenuLabel("Filter Mode"));
		for (int mode = 0; mode < kBifurxUiModeCount; ++mode) {
			menu->addChild(createCheckMenuItem(
				kBifurxModeLabels[mode], "",
				[=]() { return int(std::round(module->params[Bifurx::MODE_PARAM].getValue())) == mode; },
				[=]() { module->params[Bifurx::MODE_PARAM].setValue(float(mode)); }
			));
		}
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		TL1105::draw(args);
		const float cx = 0.5f * box.size.x;
		const float cy = 0.5f * box.size.y;
		const float dy = std::max(1.6f, 0.16f * box.size.y);
		const float halfW = std::max(1.9f, 0.22f * box.size.x);
		const float y0 = cy - dy;
		for (int i = 0; i < 3; ++i) {
			const float y = y0 + dy * float(i);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, cx - halfW, y);
			nvgLineTo(args.vg, cx + halfW, y);
			nvgStrokeWidth(args.vg, 1.2f);
			nvgStrokeColor(args.vg, nvgRGBA(225, 232, 240, 244));
			nvgStroke(args.vg);
		}
	}
};

struct BifurxModeReadoutWidget final : Widget {
	Module* module = nullptr;
	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		int m = module ? clamp(int(std::round(module->params[Bifurx::MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1) : 0;
		char label[24]; std::snprintf(label, sizeof(label), "Mode (%d): %s", m + 1, kBifurxModeLabels[m]);
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f)); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255)); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE); nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, label, nullptr);
	}
};

struct BifurxWidget final : ModuleWidget {
	Widget* spectrumNanoVG = nullptr;
	Widget* spectrumOpenGL = nullptr;
	std::shared_ptr<window::Svg> ageSigilSvg;
	bool ageSigilUnlocked = false;

	explicit BifurxWidget(Bifurx* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("Bifurx", module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/bifurx.svg");
		try { setPanel(createPanel(panelPath)); }
		catch (const std::exception& e) { setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg"))); box.size = mm2px(Vec(kDefaultPanelWidthMm, kDefaultPanelHeightMm)); }
		previewBuildTimer.markPanelDone();
		addChild(createWidget<TorxScrew>(Vec(RACK_GRID_WIDTH, 0))); addChild(createWidget<TorxScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<TorxScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH))); addChild(createWidget<TorxScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		auto applyPt = [&](const char* id, Vec* pos) { Vec p; if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) *pos = p; };
		math::Rect sRect(Vec(1.32f, 75.43f), Vec(68.45f, 21.41f)); panel_svg::loadRectFromSvgMm(panelPath, "SPECTRUM", &sRect);
		auto addFb = [&](math::Rect r, Widget* w) { widget::FramebufferWidget* fb = new widget::FramebufferWidget(); fb->box.pos = mm2px(r.pos); fb->box.size = mm2px(r.size); fb->dirtyOnSubpixelChange = false; w->box.size = fb->box.size; fb->addChild(w); addChild(fb); return fb; };
		BifurxSpectrumBackgroundWidget* sBg = new BifurxSpectrumBackgroundWidget(); sBg->module = module; sBg->framebuffer = addFb(sRect, sBg);
		
		BifurxSpectrumWidget* sNano = new BifurxSpectrumWidget(); sNano->module = module; sNano->framebuffer = addFb(sRect, sNano);
		spectrumNanoVG = sNano->framebuffer;
		
		spectrumOpenGL = createGlSpectrumDisplay(module, sRect);
		addChild(spectrumOpenGL);

		bool showGL = (module && module->renderMode == Bifurx::RENDER_OPENGL);
		if (spectrumNanoVG) spectrumNanoVG->setVisible(!showGL);
		if (spectrumOpenGL) spectrumOpenGL->setVisible(showGL);

		try {
			ageSigilSvg = Svg::load(asset::plugin(pluginInstance, "res/icon/Vahdrim'Keth.svg"));
		}
		catch (const std::exception& e) {
			WARN("Bifurx: failed to load age sigil SVG: %s", e.what());
			ageSigilSvg.reset();
		}

		BifurxModeReadoutWidget* mR = new BifurxModeReadoutWidget(); mR->module = module; mR->box.pos = mm2px(Vec(sRect.pos.x, sRect.pos.y + sRect.size.y + 0.9f)); mR->box.size = mm2px(Vec(sRect.size.x, 4.2f)); addChild(mR);
		Vec mP(13.4f, 22.f), mmP(8.9f, 22.f), lP(13.4f, 41.f), rP(13.4f, 60.f), fP(35.56f, 46.5f), tP(57.7f, 22.f), sP(57.7f, 41.f), bP(57.7f, 60.f), faP(25.3f, 45.f), saP(45.82f, 45.f);
		Vec iP(7.6f, 112.2f), vP(17.15f, 112.2f), fmP(26.7f, 112.2f), rcP(36.25f, 112.2f), bcP(45.8f, 112.2f), scP(55.35f, 112.2f), oP(64.9f, 112.2f);
		applyPt("MODE_PARAM", &mP); applyPt("LEVEL_PARAM", &lP); applyPt("RESO_PARAM", &rP); applyPt("FREQ_PARAM", &fP); applyPt("TITO_PARAM", &tP); applyPt("SPAN_PARAM", &sP); applyPt("BALANCE_PARAM", &bP); applyPt("FM_AMT_PARAM", &faP); applyPt("SPAN_CV_ATTEN_PARAM", &saP);
		applyPt("MODE_MENU_BUTTON", &mmP);
		applyPt("IN_INPUT", &iP); applyPt("VOCT_INPUT", &vP); applyPt("FM_INPUT", &fmP); applyPt("RESO_CV_INPUT", &rcP); applyPt("BALANCE_CV_INPUT", &bcP); applyPt("SPAN_CV_INPUT", &scP); applyPt("OUT_OUTPUT", &oP);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();
		auto* modeMenuButton = createParamCentered<BifurxModeMenuButton>(mm2px(mmP), module, Bifurx::MODE_MENU_PARAM);
		modeMenuButton->module = module;
		addParam(modeMenuButton);
		addParam(createParamCentered<BifurxModeLeftButton>(mm2px(mP.plus(Vec(-2.5f, 0.f))), module, Bifurx::MODE_LEFT_PARAM)); addParam(createParamCentered<BifurxModeRightButton>(mm2px(mP.plus(Vec(2.5f, 0.f))), module, Bifurx::MODE_RIGHT_PARAM));
		const Vec freqCenterPx = mm2px(fP);
		addParam(createParamCentered<Eclipse2Knob>(mm2px(lP), module, Bifurx::LEVEL_PARAM)); addParam(createParamCentered<LeviathanHaloKnob2>(freqCenterPx, module, Bifurx::FREQ_PARAM)); addParam(createParamCentered<Eclipse2Knob>(mm2px(rP), module, Bifurx::RESO_PARAM));
		{
			Eclipse2Knob* balanceKnob = createParamCentered<Eclipse2Knob>(mm2px(bP), module, Bifurx::BALANCE_PARAM);
			balanceKnob->setProgressRingBipolar(true);
			addParam(balanceKnob);
		}
		addParam(createParamCentered<Eclipse2Knob>(mm2px(sP), module, Bifurx::SPAN_PARAM)); addParam(createLightParamCentered<LeviathanSlider>(mm2px(faP), module, Bifurx::FM_AMT_PARAM, Bifurx::FM_AMT_POS_LIGHT));
		addParam(createLightParamCentered<LeviathanSlider>(mm2px(saP), module, Bifurx::SPAN_CV_ATTEN_PARAM, Bifurx::SPAN_CV_ATTEN_POS_LIGHT)); addParam(createParamCentered<BipolarTinyClockworkGearKnob>(mm2px(tP), module, Bifurx::TITO_PARAM));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(tP.plus(Vec(-7.0f, 0.f))), module, Bifurx::TITO_SM_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(tP.plus(Vec(7.0f, 0.f))), module, Bifurx::TITO_XM_LIGHT));
		addInput(createInputCentered<MagitekInputJack>(mm2px(iP), module, Bifurx::IN_INPUT)); addInput(createInputCentered<MagitekInputJack>(mm2px(vP), module, Bifurx::VOCT_INPUT)); addInput(createInputCentered<MagitekInputJack>(mm2px(fmP), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<MagitekInputJack>(mm2px(rcP), module, Bifurx::RESO_CV_INPUT)); addInput(createInputCentered<MagitekInputJack>(mm2px(bcP), module, Bifurx::BALANCE_CV_INPUT)); addInput(createInputCentered<MagitekInputJack>(mm2px(scP), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<MagitekOutputJack>(mm2px(oP), module, Bifurx::OUT_OUTPUT));
	}

	void step() override {
		ModuleWidget::step();
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (!bifurx) return;

		bool showGL = (bifurx->renderMode == Bifurx::RENDER_OPENGL);
		if (spectrumNanoVG) spectrumNanoVG->setVisible(!showGL);
		if (spectrumOpenGL) spectrumOpenGL->setVisible(showGL);
		if (!ageSigilUnlocked) {
			const double createdUnixTimeSec = bifurx->createdUnixTimeSec;
			if (std::isfinite(createdUnixTimeSec) && createdUnixTimeSec > 0.0) {
				ageSigilUnlocked = (system::getUnixTime() - createdUnixTimeSec) >= 666.0;
			}
		}
	}

	void draw(const DrawArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		const PerfClock::time_point perfDrawStart = PerfClock::now();
		ModuleWidget::draw(args);
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (bifurx && ageSigilSvg && ageSigilUnlocked) {
				const Vec sigilSize = mm2px(Vec(3.8f, 4.6f));
				const Vec rightSigilCenter = mm2px(Vec(54.8f, 4.47f));
				const Vec leftSigilCenter(box.size.x - rightSigilCenter.x, rightSigilCenter.y);
				const Vec svgSize = ageSigilSvg->getSize();
				if (svgSize.x > 1.f && svgSize.y > 1.f) {
					const float scaleX = sigilSize.x / svgSize.x;
					const float scaleY = sigilSize.y / svgSize.y;
					auto drawSigilAt = [&](const Vec& center) {
						nvgSave(args.vg);
						nvgTranslate(args.vg, center.x, center.y);
						nvgScale(args.vg, scaleX, scaleY);
						nvgTranslate(args.vg, -svgSize.x * 0.5f, -svgSize.y * 0.5f);
						ageSigilSvg->draw(args.vg);
						nvgRestore(args.vg);
					};
					drawSigilAt(leftSigilCenter);
					drawSigilAt(rightSigilCenter);
				}
		}
		if (bifurx && bifurx->renderMode == Bifurx::RENDER_OPENGL && spectrumOpenGL && spectrumOpenGL->visible) {
			nvgSave(args.vg);
			nvgTranslate(args.vg, spectrumOpenGL->box.pos.x, spectrumOpenGL->box.pos.y);
			if (auto* base = dynamic_cast<BifurxSpectrumBase*>(spectrumOpenGL)) {
				base->drawNanoVG(args);
			}
			nvgRestore(args.vg);
		}
		if (bifurx && isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
			char debugIdLabel[32];
			std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", bifurx->debugInstanceId);
			const float x = box.size.x - mm2px(0.9f);
			const float y = mm2px(2.5f);
			nvgSave(args.vg);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 6.8f);
			nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
			nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
			nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
			nvgText(args.vg, x, y, debugIdLabel, nullptr);
			nvgRestore(args.vg);
		}
		if (bifurx) {
			const float drawMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - perfDrawStart).count()) * 1e-6f;
			const float prevMs = bifurx->perfUiRenderMs.load(std::memory_order_relaxed);
			const float emaMs = (prevMs > 0.f) ? (prevMs + (drawMs - prevMs) * 0.18f) : drawMs;
			bifurx->perfUiRenderMs.store(std::max(0.f, emaMs), std::memory_order_relaxed);
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu); Bifurx* bifurx = dynamic_cast<Bifurx*>(module); if (!bifurx) return;
		auto setRenderStateWithHistory = [=](Bifurx::RenderMode newMode, bool newUseShaderRenderer) {
			if (!bifurx || (bifurx->renderMode == newMode && bifurx->useGlShaderRenderer.load(std::memory_order_relaxed) == newUseShaderRenderer)) return;
			if (APP && APP->history) {
				history::ModuleChange* h = new history::ModuleChange();
				h->name = "change render engine";
				h->moduleId = bifurx->id;
				h->oldModuleJ = bifurx->toJson();
				bifurx->renderMode = newMode;
				bifurx->useGlShaderRenderer.store(newUseShaderRenderer, std::memory_order_relaxed);
				h->newModuleJ = bifurx->toJson();
				APP->history->push(h);
			}
			else {
				bifurx->renderMode = newMode;
				bifurx->useGlShaderRenderer.store(newUseShaderRenderer, std::memory_order_relaxed);
			}
		};
		menu->addChild(new MenuSeparator());
			menu->addChild(createSubmenuItem("Modulation Quality", "", [=](Menu* submenu) {
				submenu->addChild(createCheckMenuItem(
					"Balanced", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_BALANCED; },
					[=]() {
						bifurx->modulationQualityMode.store(Bifurx::MOD_QUALITY_BALANCED, std::memory_order_relaxed);
						bifurx->controlFastCacheValid = false;
					}));
				submenu->addChild(createCheckMenuItem(
					"High", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_HIGH; },
					[=]() {
						bifurx->modulationQualityMode.store(Bifurx::MOD_QUALITY_HIGH, std::memory_order_relaxed);
						bifurx->controlFastCacheValid = false;
					}));
				submenu->addChild(createCheckMenuItem(
					"Exact", "",
					[=]() { return bifurx->modulationQualityMode.load(std::memory_order_relaxed) == Bifurx::MOD_QUALITY_EXACT; },
					[=]() {
						bifurx->modulationQualityMode.store(Bifurx::MOD_QUALITY_EXACT, std::memory_order_relaxed);
						bifurx->controlFastCacheValid = false;
					}));
			}));
			menu->addChild(createSubmenuItem("Color Scheme", "", [=](Menu* submenu) {
				auto addSchemeItem = [=](Bifurx::ColorScheme scheme, const std::string& label) {
					submenu->addChild(createCheckMenuItem(
						label, "",
						[=]() { return bifurx->colorScheme == scheme; },
						[=]() { bifurx->colorScheme = scheme; }
					));
				};
				addSchemeItem(Bifurx::SCHEME_DEFAULT, "Default (Purple/Cyan)");
				addSchemeItem(Bifurx::SCHEME_CLASSIC, "Classic (Green/Red)");
				addSchemeItem(Bifurx::SCHEME_MONOCHROME, "Monochrome (Gray/White)");
				addSchemeItem(Bifurx::SCHEME_FIRE, "Fire (Red/Yellow)");
				addSchemeItem(Bifurx::SCHEME_RETRO_AMBER, "Retro Amber");
				addSchemeItem(Bifurx::SCHEME_RETRO_GREEN, "Retro Green");
			}));
			menu->addChild(createSubmenuItem("Render Engine", "", [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem(
				"NanoVG", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_NANOVG; },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_NANOVG, false); }));
			submenu->addChild(createCheckMenuItem(
				"OpenGL", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_OPENGL && !bifurx->useGlShaderRenderer.load(std::memory_order_relaxed); },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_OPENGL, false); }));
			submenu->addChild(createCheckMenuItem(
				"OpenGL SHDR", "",
				[=]() { return bifurx->renderMode == Bifurx::RENDER_OPENGL && bifurx->useGlShaderRenderer.load(std::memory_order_relaxed); },
				[=]() { setRenderStateWithHistory(Bifurx::RENDER_OPENGL, true); }));
			}));
			menu->addChild(createCheckMenuItem("High Resonance Self-Osc", "",
				[=]() { return bifurx->highResonanceSelfOscEnabled.load(std::memory_order_relaxed); },
				[=]() { bifurx->highResonanceSelfOscEnabled.store(!bifurx->highResonanceSelfOscEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Soft Limiting", "",
				[=]() { return bifurx->softLimitingEnabled.load(std::memory_order_relaxed); },
				[=]() { bifurx->softLimitingEnabled.store(!bifurx->softLimitingEnabled.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Dynamic FFT Scale", "",
				[=]() { return bifurx->fftScaleDynamic.load(std::memory_order_relaxed); },
				[=]() { bifurx->fftScaleDynamic.store(!bifurx->fftScaleDynamic.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Show Module Response", "",
				[=]() { return bifurx->showModuleResponseOverlay.load(std::memory_order_relaxed); },
				[=]() { bifurx->showModuleResponseOverlay.store(!bifurx->showModuleResponseOverlay.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Low Latency Offload", "",
				[=]() { return bifurx->lowLatencyVisual.load(std::memory_order_relaxed); },
				[=]() { bifurx->lowLatencyVisual.store(!bifurx->lowLatencyVisual.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem(
				"Disable Visual Offload", "",
				[=]() { return bifurx->visualWorkerMode.load(std::memory_order_relaxed) == Bifurx::VISUAL_WORKER_OFF; },
				[=]() {
					const bool disabledNow = bifurx->visualWorkerMode.load(std::memory_order_relaxed) == Bifurx::VISUAL_WORKER_OFF;
					bifurx->visualWorkerMode.store(
						disabledNow ? Bifurx::VISUAL_WORKER_INHERIT : Bifurx::VISUAL_WORKER_OFF,
						std::memory_order_relaxed
					);
				}
			));
		if (isDragonKingDebugEnabled()) {
			menu->addChild(createCheckMenuItem("Log Curve Debug", "",
				[=]() { return bifurx->curveDebugLogging.load(std::memory_order_relaxed); },
				[=]() { bifurx->curveDebugLogging.store(!bifurx->curveDebugLogging.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
			menu->addChild(createCheckMenuItem("Log Performance Debug", "",
				[=]() { return bifurx->perfDebugLogging.load(std::memory_order_relaxed); },
				[=]() { bifurx->perfDebugLogging.store(!bifurx->perfDebugLogging.load(std::memory_order_relaxed), std::memory_order_relaxed); }));
		}
	}
};

} // namespace bifurx

Model* modelBifurx = createModel<bifurx::Bifurx, bifurx::BifurxWidget>("Bifurx");
