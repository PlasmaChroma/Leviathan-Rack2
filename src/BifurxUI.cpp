#include "Bifurx.hpp"
#include "DebugTerminalTransport.hpp"
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
	uint64_t lastDrawVertexCount = 0;
	
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

	BifurxSpectrumWidget() : BifurxSpectrumBase() {
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
		if (module->curveDebugLogging && !curveDebugRecorder.active) {
			startCurveDebugCapture();
		}
		else if (!module->curveDebugLogging && curveDebugRecorder.active) {
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
		if (module->perfDebugLogging && !perfDebugRecorder.active) {
			startPerfDebugCapture();
		}
		else if (!module->perfDebugLogging && perfDebugRecorder.active) {
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
	const bool perfLoggingActive = module && module->perfDebugLogging;
	const PerfClock::time_point perfStepStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
	Widget::step();
	syncCurveDebugCaptureState();
	syncPerfDebugCaptureState();
	if (!module) return;

	bool dirty = false;
	bool previewUpdated = false;
	bool analysisUpdated = false;

	const bool fftScaleDynamicNow = module->fftScaleDynamic;
	if (fftScaleDynamicNow != lastFftScaleDynamic) {
		lastFftScaleDynamic = fftScaleDynamicNow;
		if (!fftScaleDynamicNow) {
			state.displayTopDbfs = kDisplayTopDbfsCeiling;
			state.displayTopTargetDbfs = kDisplayTopDbfsCeiling;
		}
		dirty = true;
	}
	const bool showModuleResponseOverlayNow = module->showModuleResponseOverlay;
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

	if (isDragonKingDebugEnabled() && module->curveDebugLogging && state.hasPreview) {
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
				const bool anchorMarkerToBottomLane = (state.previewState.mode == 3);
				auto evalPeak = [&](int idx, float targetHz, float* outX, float* outYCurve, float* outYMarker) {
					const auto anchor = displayAnchorForMarker(idx, targetHz, minHz, maxHz);
					const float markerRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
					const float curveIndex = anchor.x01 * float(kCurvePointCount - 1);
					const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
					const float curveDbAtHz = mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0));
					const float yCurve = responseYForDb(curveDbAtHz), markerX = plotX + usableW * anchor.x01;
					const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding, markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
					const float yMarker = anchorMarkerToBottomLane ? (spectrumBottomY - markerRadius - kPeakMarkerBottomLanePadding) : clamp(yCurve, markerMinY, markerMaxY);
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
	
	if (isDragonKingDebugEnabled() && module->renderMode == Bifurx::RENDER_NANOVG) {
		double nowSec = system::getTime();
		uint32_t debugId = module->debugInstanceId;
		double& lastSubmitSec = gDebugTerminalLastSubmitSec[debugId];
		if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kDebugTerminalSubmitIntervalSec) {
			const int filterMode = clamp(int(state.previewState.mode), 0, kBifurxModeCount - 1);
			lastSubmitSec = nowSec;
			debug_terminal::submitBifurxUiMetrics(
				debugId,
				lastDrawMsEma,
				filterMode,
				false, // opengl
				state.lastPreviewSeq,
				state.lastAnalysisSeq,
				lastDrawVertexCount
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
	const bool perfLoggingActive = module && module->perfDebugLogging;
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
	
	std::vector<BifurxCurvePoint> refinedPoints;
	calculateRefinedCurvePoints(&refinedPoints, w, h);

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

	const NVGcolor expectedPurple = nvgRGB(122, 92, 255), expectedCyan = nvgRGB(28, 204, 217), expectedWhite = nvgRGB(206, 210, 216);
	
	BifurxMarkerLayout layout;
	calculateMarkerLayout(&layout, w, h);

	auto drawExpectedGuideStroke = [&](float x, float y, float curveDbVal) {
		const float posAmt = clamp01(curveDbVal / 18.f), negAmt = clamp01(-curveDbVal / 18.f), emph = std::max(posAmt, negAmt);
		NVGcolor tint = expectedWhite; if (posAmt > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posAmt * 1.35f)); if (negAmt > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negAmt * 1.25f));
		tint.a = 0.025f + 0.095f * emph; nvgBeginPath(args.vg); nvgMoveTo(args.vg, x, spectrumBottomY); nvgLineTo(args.vg, x, y); nvgStrokeColor(args.vg, tint); nvgStrokeWidth(args.vg, 1.05f); nvgStroke(args.vg);
	};
	const float markerGuideClearanceX01 = 1.35f / float(kCurvePointCount - 1);
	for (int i = 0; i < kCurvePointCount; i += 3) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		if (std::fabs(x01 - layout.markers[0].x / w) < markerGuideClearanceX01 || std::fabs(x01 - layout.markers[1].x / w) < markerGuideClearanceX01) continue;
		drawExpectedGuideStroke(curveX[i], responseYForDb(state.curveDb[i]), state.curveDb[i]);
	}
	
	for (int i = 0; i < 2; i++) {
		if (!layout.markers[i].visible) continue;
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, spectrumBottomY); nvgLineTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker);
		nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 150)); nvgStrokeWidth(args.vg, 1.05f * 1.45f); nvgStroke(args.vg);
	};
	recordDrawSection(uiDrawExpectedCount, uiDrawExpectedNs);

	if (state.hasOverlay) {
		const bool showModuleResponse = module && module->showModuleResponseOverlay;
		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgD = 0.5f * (state.overlayModuleDb[i] + state.overlayModuleDb[i + 1]);
			const float avgO = 0.5f * (state.overlayOutputDbfs[i] + state.overlayOutputDbfs[i + 1]), energy = clamp01(rescale(avgO, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
			if (energy <= 0.005f) continue;
			const float posA = clamp01(avgD / 18.f), negA = clamp01(-avgD / 18.f);
			NVGcolor tint = expectedWhite; if (posA > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posA * 1.40f)); if (negA > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negA * 1.25f));
			NVGcolor fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energy); fill.a = 1.f;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, curveX[i] - 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i])); nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumYForDbfs(state.overlayOutputDbfs[i + 1]));
			nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumBottomY); nvgLineTo(args.vg, curveX[i] - 0.45f, spectrumBottomY); nvgClosePath(args.vg); nvgFillColor(args.vg, fill); nvgFill(args.vg);
		}
		if (showModuleResponse) {
			nvgBeginPath(args.vg); for (int i = 0; i < kCurvePointCount; ++i) { float y = responseYForDb(state.overlayModuleDb[i]); if (i == 0) nvgMoveTo(args.vg, curveX[i], y); else nvgLineTo(args.vg, curveX[i], y); }
			NVGcolor ml = mixColor(expectedWhite, expectedCyan, 0.35f); ml.a = 0.95f; nvgStrokeWidth(args.vg, 1.4f); nvgStrokeColor(args.vg, ml); nvgStroke(args.vg);
		}
		recordDrawSection(uiDrawOverlayCount, uiDrawOverlayNs);
	}

	nvgBeginPath(args.vg);
	for (int i = 0; i < (int)refinedPoints.size(); ++i) { if (i == 0) nvgMoveTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y); else nvgLineTo(args.vg, w * refinedPoints[i].x01, refinedPoints[i].y); }
	nvgStrokeColor(args.vg, nvgRGBA(255, 248, 208, 244)); nvgLineJoin(args.vg, NVG_ROUND); nvgLineCap(args.vg, NVG_ROUND); nvgStrokeWidth(args.vg, 1.35f); nvgStroke(args.vg);
	lastDrawVertexCount = uint64_t(refinedPoints.size());
	recordDrawSection(uiDrawCurveCount, uiDrawCurveNs);
	nvgRestore(args.vg);

	for (int i = 0; i < 2; ++i) {
		if (!layout.markers[i].visible) continue;
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, layout.markers[i].x, layout.markers[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, layout.markers[i].x, layout.guideYBottom); nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 170)); nvgStrokeWidth(args.vg, 1.1f); nvgStroke(args.vg);
		nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius); nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgCircle(args.vg, layout.markers[i].x, layout.markers[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius); nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth); nvgStroke(args.vg);
	}
	nvgFontSize(args.vg, layout.labelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	for (int i = 0; i < 2; ++i) { if (!layout.markers[i].visible) continue; nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240)); nvgText(args.vg, layout.labelX[i], layout.labelY + 0.75f, layout.markers[i].label, nullptr); nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250)); nvgText(args.vg, layout.labelX[i], layout.labelY, layout.markers[i].label, nullptr); }
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
		nvgResetScissor(args.vg); nvgRestore(args.vg); nvgRestore(args.vg);
	}
};

struct BananutBlack : app::SvgPort {
	BananutBlack() { setSvg(Svg::load(asset::plugin(pluginInstance, "res/BananutBlack.svg"))); }
};

void drawModeStepTriangle(const Widget::DrawArgs& args, const Vec& size, bool pointRight) {
	const float cx = 0.5f * size.x, cy = 0.5f * size.y, hW = 2.8f, hH = 3.3f, off = pointRight ? (hW / 3.f) : (-hW / 3.f);
	nvgBeginPath(args.vg); if (pointRight) { nvgMoveTo(args.vg, cx - hW + off, cy - hH); nvgLineTo(args.vg, cx + hW + off, cy); nvgLineTo(args.vg, cx - hW + off, cy + hH); } else { nvgMoveTo(args.vg, cx + hW + off, cy - hH); nvgLineTo(args.vg, cx - hW + off, cy); nvgLineTo(args.vg, cx + hW + off, cy + hH); }
	nvgClosePath(args.vg); nvgFillColor(args.vg, nvgRGBA(225, 232, 240, 244)); nvgFill(args.vg);
}

struct BifurxModeLeftButton final : TL1105 { void draw(const DrawArgs& args) override { TL1105::draw(args); drawModeStepTriangle(args, box.size, false); } };
struct BifurxModeRightButton final : TL1105 { void draw(const DrawArgs& args) override { TL1105::draw(args); drawModeStepTriangle(args, box.size, true); } };
struct BifurxModeReadoutWidget final : Widget {
	Module* module = nullptr;
	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		int m = module ? clamp(int(std::round(module->params[Bifurx::MODE_PARAM].getValue())), 0, kBifurxModeCount - 1) : 0;
		char label[24]; std::snprintf(label, sizeof(label), "Mode (%d): %s", m + 1, kBifurxModeLabels[m]);
		nvgFontSize(args.vg, std::max(9.5f, box.size.y * 0.72f)); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255)); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE); nvgText(args.vg, 0.5f * box.size.x, 0.5f * box.size.y, label, nullptr);
	}
};

struct BifurxWidget final : ModuleWidget {
	Widget* spectrumNanoVG = nullptr;
	Widget* spectrumOpenGL = nullptr;

	explicit BifurxWidget(Bifurx* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/bifurx.svg");
		try { setPanel(createPanel(panelPath)); }
		catch (const std::exception& e) { setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg"))); box.size = mm2px(Vec(kDefaultPanelWidthMm, kDefaultPanelHeightMm)); }
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0))); addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH))); addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
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

		BifurxModeReadoutWidget* mR = new BifurxModeReadoutWidget(); mR->module = module; mR->box.pos = mm2px(Vec(sRect.pos.x, sRect.pos.y + sRect.size.y + 0.9f)); mR->box.size = mm2px(Vec(sRect.size.x, 4.2f)); addChild(mR);
		Vec mP(13.4f, 22.f), lP(13.4f, 41.f), rP(13.4f, 60.f), fP(35.56f, 46.5f), tP(57.7f, 22.f), sP(57.7f, 41.f), bP(57.7f, 60.f), faP(25.3f, 45.f), saP(45.82f, 45.f);
		Vec iP(7.6f, 112.2f), vP(17.15f, 112.2f), fmP(26.7f, 112.2f), rcP(36.25f, 112.2f), bcP(45.8f, 112.2f), scP(55.35f, 112.2f), oP(64.9f, 112.2f);
		applyPt("MODE_PARAM", &mP); applyPt("LEVEL_PARAM", &lP); applyPt("RESO_PARAM", &rP); applyPt("FREQ_PARAM", &fP); applyPt("TITO_PARAM", &tP); applyPt("SPAN_PARAM", &sP); applyPt("BALANCE_PARAM", &bP); applyPt("FM_AMT_PARAM", &faP); applyPt("SPAN_CV_ATTEN_PARAM", &saP);
		applyPt("IN_INPUT", &iP); applyPt("VOCT_INPUT", &vP); applyPt("FM_INPUT", &fmP); applyPt("RESO_CV_INPUT", &rcP); applyPt("BALANCE_CV_INPUT", &bcP); applyPt("SPAN_CV_INPUT", &scP); applyPt("OUT_OUTPUT", &oP);
		addParam(createParamCentered<BifurxModeLeftButton>(mm2px(mP.plus(Vec(-2.5f, 0.f))), module, Bifurx::MODE_LEFT_PARAM)); addParam(createParamCentered<BifurxModeRightButton>(mm2px(mP.plus(Vec(2.5f, 0.f))), module, Bifurx::MODE_RIGHT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(lP), module, Bifurx::LEVEL_PARAM)); addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(fP), module, Bifurx::FREQ_PARAM)); addParam(createParamCentered<RoundBlackKnob>(mm2px(rP), module, Bifurx::RESO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(bP), module, Bifurx::BALANCE_PARAM)); addParam(createParamCentered<RoundBlackKnob>(mm2px(sP), module, Bifurx::SPAN_PARAM)); addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(faP), module, Bifurx::FM_AMT_PARAM, Bifurx::FM_AMT_POS_LIGHT));
		addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(saP), module, Bifurx::SPAN_CV_ATTEN_PARAM, Bifurx::SPAN_CV_ATTEN_POS_LIGHT)); addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(tP), module, Bifurx::TITO_PARAM));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(tP.plus(Vec(-7.0f, 0.f))), module, Bifurx::TITO_SM_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(tP.plus(Vec(7.0f, 0.f))), module, Bifurx::TITO_XM_LIGHT));
		addInput(createInputCentered<PJ301MPort>(mm2px(iP), module, Bifurx::IN_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(vP), module, Bifurx::VOCT_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(fmP), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(rcP), module, Bifurx::RESO_CV_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(bcP), module, Bifurx::BALANCE_CV_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(scP), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(oP), module, Bifurx::OUT_OUTPUT));
	}

	void step() override {
		ModuleWidget::step();
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
		if (!bifurx) return;

		bool showGL = (bifurx->renderMode == Bifurx::RENDER_OPENGL);
		if (spectrumNanoVG) spectrumNanoVG->setVisible(!showGL);
		if (spectrumOpenGL) spectrumOpenGL->setVisible(showGL);
	}

	void draw(const DrawArgs& args) override {
		ModuleWidget::draw(args);
		Bifurx* bifurx = dynamic_cast<Bifurx*>(module);
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
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu); Bifurx* bifurx = dynamic_cast<Bifurx*>(module); if (!bifurx) return;
		auto setRenderModeWithHistory = [=](Bifurx::RenderMode newMode) {
			if (!bifurx || bifurx->renderMode == newMode) return;
			if (APP && APP->history) {
				history::ModuleChange* h = new history::ModuleChange();
				h->name = "change render engine";
				h->moduleId = bifurx->id;
				h->oldModuleJ = bifurx->toJson();
				bifurx->renderMode = newMode;
				h->newModuleJ = bifurx->toJson();
				APP->history->push(h);
			}
			else {
				bifurx->renderMode = newMode;
			}
		};
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Render Engine", "", [=](Menu* submenu) {
			submenu->addChild(createCheckMenuItem("NanoVG", "", [=]() { return bifurx->renderMode == Bifurx::RENDER_NANOVG; }, [=]() { setRenderModeWithHistory(Bifurx::RENDER_NANOVG); }));
			submenu->addChild(createCheckMenuItem("OpenGL", "", [=]() { return bifurx->renderMode == Bifurx::RENDER_OPENGL; }, [=]() { setRenderModeWithHistory(Bifurx::RENDER_OPENGL); }));
		}));
		menu->addChild(createBoolPtrMenuItem("Dynamic FFT Scale", "", &bifurx->fftScaleDynamic));
		menu->addChild(createBoolPtrMenuItem("Show Module Response", "", &bifurx->showModuleResponseOverlay));
		menu->addChild(createBoolPtrMenuItem("Use GL Shader Renderer", "", &bifurx->useGlShaderRenderer));
		if (isDragonKingDebugEnabled()) {
			menu->addChild(createBoolPtrMenuItem("Log Curve Debug", "", &bifurx->curveDebugLogging));
			menu->addChild(createBoolPtrMenuItem("Log Performance Debug", "", &bifurx->perfDebugLogging));
		}
	}
};

} // namespace bifurx

Model* modelBifurx = createModel<bifurx::Bifurx, bifurx::BifurxWidget>("Bifurx");
