#include "Bifurx.hpp"

namespace bifurx {

struct BifurxSpectrumWidget final : Widget {
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

	Bifurx* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize];
	alignas(16) float fftOutputTime[kFftSize];
	alignas(16) float fftInputFreq[2 * kFftSize];
	alignas(16) float fftOutputFreq[2 * kFftSize];
	float curveDb[kCurvePointCount];
	float curveTargetDb[kCurvePointCount];
	float overlayModuleDb[kCurvePointCount];
	float overlayTargetModuleDb[kCurvePointCount];
	float overlayOutputDbfs[kCurvePointCount];
	float overlayTargetOutputDbfs[kCurvePointCount];
	float curveX[kCurvePointCount];
	float curveY[kCurvePointCount];
	float curveHz[kCurvePointCount];
	float curveBinPos[kCurvePointCount];
	float bottomY = 0.f;
	float displayTopDbfs = kDisplayTopDbfsCeiling;
	float displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	float cachedCurveXPlotX = NAN;
	float cachedCurveXUsableW = NAN;
	int cachedTopLabelFontHandle = -1;
	float cachedTopLabelFontSize = NAN;
	float cachedTopLabelReservedWidth = 0.f;
	bool lastFftScaleDynamic = true;
	double lastCurveDebugLogTimeSec = -1.0;
	float cachedAxisSampleRate = 0.f;
	BifurxPreviewState previewState;
	BifurxLlTelemetryState llTelemetryState;
	bool hasPreview = false;
	bool hasLlTelemetry = false;
	bool hasOverlay = false;
	bool hasCurveTarget = false;
	bool hasOverlayTarget = false;
	uint32_t lastPreviewSeq = 0;
	uint32_t lastLlTelemetrySeq = 0;
	uint32_t lastAnalysisSeq = 0;
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

	BifurxSpectrumWidget() : fft(kFftSize) {
		for (int i = 0; i < kFftSize; i++) {
			window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
		}
		for (int i = 0; i < kCurvePointCount; i++) {
			curveDb[i] = kResponseMinDb;
			curveTargetDb[i] = kResponseMinDb;
			overlayModuleDb[i] = 0.f;
			overlayTargetModuleDb[i] = 0.f;
			overlayOutputDbfs[i] = kOverlayDbfsFloor;
			overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
		}
	}

	~BifurxSpectrumWidget() override {
		stopCurveDebugCapture();
		stopPerfDebugCapture();
	}

	void syncCurveDebugCaptureState() {
		if (!module) return;
		if (module->curveDebugLogging && !curveDebugRecorder.active) {
			startCurveDebugCapture();
		}
		else if (!module->curveDebugLogging && curveDebugRecorder.active) {
			stopCurveDebugCapture();
		}
	}

	void syncPerfDebugCaptureState() {
		if (!module) return;
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

	void updateAxisCache() {
		if (std::fabs(cachedAxisSampleRate - previewState.sampleRate) < 0.5f) return;
		cachedAxisSampleRate = previewState.sampleRate;
		const float minHz = 10.f;
		const float maxHz = std::min(20000.f, 0.46f * cachedAxisSampleRate);
		for (int i = 0; i < kCurvePointCount; i++) {
			const float x01 = float(i) / float(kCurvePointCount - 1);
			const float hz = logFrequencyAt(x01, minHz, maxHz);
			curveHz[i] = hz;
			curveBinPos[i] = (hz * float(kFftSize)) / cachedAxisSampleRate;
		}
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
	void updateCurveCache();
	void updateOverlayCache(const BifurxAnalysisFrame& frame);
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
	if (!module) {
		return;
	}

	bool dirty = false;
	bool previewUpdatedThisStep = false;
	bool analysisUpdatedThisStep = false;
	const bool fftScaleDynamicNow = module->fftScaleDynamic;
	if (fftScaleDynamicNow != lastFftScaleDynamic) {
		lastFftScaleDynamic = fftScaleDynamicNow;
		if (!fftScaleDynamicNow) {
			displayTopDbfs = kDisplayTopDbfsCeiling;
			displayTopTargetDbfs = kDisplayTopDbfsCeiling;
		}
		dirty = true;
	}

	const uint32_t previewSeq = module->previewPublishSeq.load(std::memory_order_acquire);
	if (previewSeq != lastPreviewSeq) {
		const int index = module->previewPublishedIndex.load(std::memory_order_acquire);
		previewState = module->previewStates[index];
		hasPreview = true;
		updateAxisCache();
		lastPreviewSeq = previewSeq;
		previewUpdatedThisStep = true;
		const PerfClock::time_point perfCurveStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
		updateCurveCache();
		if (perfLoggingActive) {
			const uint64_t curveUpdateNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfCurveStart).count();
			uiCurveUpdateCount++;
			uiCurveUpdateNs += curveUpdateNs;
		}
		dirty = true;
	}

	const uint32_t llTelemetrySeq = module->llTelemetryPublishSeq.load(std::memory_order_acquire);
	if (llTelemetrySeq != lastLlTelemetrySeq) {
		const int index = module->llTelemetryPublishedIndex.load(std::memory_order_acquire);
		llTelemetryState = module->llTelemetryStates[index];
		hasLlTelemetry = true;
		lastLlTelemetrySeq = llTelemetrySeq;
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != lastAnalysisSeq) {
		const int index = module->analysisPublishedIndex.load(std::memory_order_acquire);
		const PerfClock::time_point perfOverlayStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
		updateOverlayCache(module->analysisFrames[index]);
		if (perfLoggingActive) {
			const uint64_t overlayUpdateNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfOverlayStart).count();
			uiOverlayUpdateCount++;
			uiOverlayUpdateNs += overlayUpdateNs;
		}
		hasOverlay = true;
		lastAnalysisSeq = analysisSeq;
		analysisUpdatedThisStep = true;
		dirty = true;
	}

	if (hasCurveTarget) {
		bool curveAnimating = false;
		float uiFrameSec = 1.f / 60.f;
		if (APP && APP->window) {
			const float frameSec = float(APP->window->getLastFrameDuration());
			if (std::isfinite(frameSec) && frameSec > 0.f) {
				uiFrameSec = clamp(frameSec, 1.f / 240.f, 1.f / 20.f);
			}
		}
		const float curveMaxStepDb = std::max(0.25f, kCurveVisualSlewDbPerSec * uiFrameSec);
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = curveDb[i];
			float delta = curveTargetDb[i] - prev;
			delta = clamp(delta, -curveMaxStepDb, curveMaxStepDb);
			const float next = prev + delta;
			curveDb[i] = next;
			if (std::fabs(next - prev) > 0.01f) {
				curveAnimating = true;
			}
		}
		dirty = dirty || curveAnimating;
	}

	if (hasOverlayTarget) {
		bool overlayAnimating = false;
		const float overlayDbSmoothing = 0.22f;
		const float overlayLevelSmoothing = 0.20f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prevModuleDb = overlayModuleDb[i];
			const float nextModuleDb = mixf(prevModuleDb, overlayTargetModuleDb[i], overlayDbSmoothing);
			overlayModuleDb[i] = nextModuleDb;
			if (std::fabs(nextModuleDb - prevModuleDb) > 0.02f) {
				overlayAnimating = true;
			}

			const float prevLevel = overlayOutputDbfs[i];
			const float nextLevel = mixf(prevLevel, overlayTargetOutputDbfs[i], overlayLevelSmoothing);
			overlayOutputDbfs[i] = nextLevel;
			if (std::fabs(nextLevel - prevLevel) > 0.02f) {
				overlayAnimating = true;
			}
		}

		const float prevTop = displayTopDbfs;
		float topSmoothing = (displayTopTargetDbfs > prevTop) ? 0.22f : 0.10f;
		if (module && module->fftScaleDynamic && displayTopTargetDbfs > prevTop) {
			topSmoothing = 0.70f;
		}
		displayTopDbfs = mixf(prevTop, displayTopTargetDbfs, topSmoothing);
		if (std::fabs(displayTopDbfs - prevTop) > 0.02f) {
			overlayAnimating = true;
		}

		dirty = dirty || overlayAnimating;
	}

	if (module->curveDebugLogging && hasPreview) {
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

			float peakAX = NAN;
			float peakAYCurve = NAN;
			float peakAYMarker = NAN;
			float peakBX = NAN;
			float peakBYCurve = NAN;
			float peakBYMarker = NAN;
			const float w = box.size.x;
			const float h = box.size.y;
			if (w > 0.f && h > 0.f) {
				const BifurxPreviewModel model = makePreviewModel(previewState);
				const float padX = 0.f;
				const float padY = std::max(4.f, h * 0.035f);
				const float plotX = padX;
				const float usableW = std::max(1.f, w - plotX - padX);
				const float minHz = 10.f;
				const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);
				const float labelBandHeight = std::max(5.2f, h * 0.072f);
				const float labelBandTop = h - labelBandHeight;
				const float spectrumTopY = padY * 0.35f;
				const float spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
				auto responseYForDb = [&](float db) {
					return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY);
				};
				const bool anchorMarkerToBottomLane = (previewState.mode == 3);
				auto evalPeak = [&](float targetHz, float* outX, float* outYCurve, float* outYMarker) {
					const float clampedHz = clamp(targetHz, minHz, maxHz);
					const float targetX01 = logPosition(clampedHz, minHz, maxHz);
					const float markerRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
					const float curveIndex = targetX01 * float(kCurvePointCount - 1);
					const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
					const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
					const float t = curveIndex - float(i0);
					const float curveDbAtHz = mixf(curveDb[i0], curveDb[i1], t);
					const float yCurve = responseYForDb(curveDbAtHz);
					const float markerX = plotX + usableW * targetX01;
					const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding;
					const float markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
					const float bottomLaneY = spectrumBottomY - markerRadius - kPeakMarkerBottomLanePadding;
					const float yMarker = anchorMarkerToBottomLane ? bottomLaneY : clamp(yCurve, markerMinY, markerMaxY);
					*outX = clamp(
						markerX,
						plotX + markerRadius + kPeakMarkerEdgePadding,
						plotX + usableW - markerRadius - kPeakMarkerEdgePadding
					);
					*outYCurve = yCurve;
					*outYMarker = yMarker;
				};
				evalPeak(model.markerFreqA, &peakAX, &peakAYCurve, &peakAYMarker);
				evalPeak(model.markerFreqB, &peakBX, &peakBYCurve, &peakBYMarker);
			}

			logCurveDebugSample(
				previewState,
				hasLlTelemetry ? llTelemetryState : BifurxLlTelemetryState{},
				peakAX, peakAYCurve, peakAYMarker,
				peakBX, peakBYCurve, peakBYMarker,
				uiFrameMs,
				previewSeq, previewUpdatedThisStep,
				analysisSeq, analysisUpdatedThisStep
			);
		}
	}
	else if (lastCurveDebugLogTimeSec >= 0.0) {
		lastCurveDebugLogTimeSec = -1.0;
	}

	if (perfDebugRecorder.active) {
		const double nowSec = system::getTime();
		const double minIntervalSec = 0.5;
		if (perfDebugRecorder.lastLogTimeSec < 0.0 || (nowSec - perfDebugRecorder.lastLogTimeSec) >= minIntervalSec) {
			perfDebugRecorder.lastLogTimeSec = nowSec;
			logPerfDebugSample();
		}
	}

	if (dirty && framebuffer) {
		framebuffer->setDirty();
	}
	if (perfLoggingActive) {
		const uint64_t stepNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfStepStart).count();
		uiStepCount++;
		uiStepNs += stepNs;
		uiStepMaxNs = std::max(uiStepMaxNs, stepNs);
	}
}

void BifurxSpectrumWidget::updateCurveCache() {
	if (!hasPreview) return;
	updateAxisCache();
	const BifurxPreviewModel model = makePreviewModel(previewState);
	for (int i = 0; i < kCurvePointCount; i++) {
		const float db = previewModelResponseDb(model, curveHz[i]);
		curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
	}
	if (!hasCurveTarget) {
		for (int i = 0; i < kCurvePointCount; i++) curveDb[i] = curveTargetDb[i];
		hasCurveTarget = true;
	}
}

void BifurxSpectrumWidget::updateOverlayCache(const BifurxAnalysisFrame& frame) {
	if (!hasPreview) return;
	updateAxisCache();
	const float amplitudeScale = 4.f / float(kFftSize);
	for (int i = 0; i < kFftSize; i++) fftOutputTime[i] = frame.output[i] * window[i];
	fft.rfft(fftOutputTime, fftOutputFreq);
	for (int i = 0; i < kFftSize; i++) fftInputTime[i] = frame.rawInput[i] * window[i];
	float fftRawInputFreq[2 * kFftSize];
	fft.rfft(fftInputTime, fftRawInputFreq);
	float binModuleDeltaDb[kFftBinCount];
	float binOutputDbfs[kFftBinCount];
	float binRawInputAmp[kFftBinCount];
	float binOutputAmp[kFftBinCount];
	for (int bin = 0; bin < kFftBinCount; bin++) {
		const float binHz = (float(bin) * previewState.sampleRate) / float(kFftSize);
		const float subsonicWeight = clamp01((binHz - kOverlaySubsonicCutHz) / (kOverlaySubsonicFadeHz - kOverlaySubsonicCutHz));
		const float weightedRawInputAmp = subsonicWeight * amplitudeScale * orderedSpectrumMagnitude(fftRawInputFreq, bin);
		const float weightedOutputAmp = subsonicWeight * amplitudeScale * orderedSpectrumMagnitude(fftOutputFreq, bin);
		binRawInputAmp[bin] = weightedRawInputAmp;
		binOutputAmp[bin] = weightedOutputAmp;
	}
	constexpr int kOverlayBandRadius = 2;
	constexpr float kOverlayBandKernel[5] = {0.08f, 0.24f, 0.36f, 0.24f, 0.08f};
	for (int bin = 0; bin < kFftBinCount; bin++) {
		float rawInputEnergy = 0.f; float outputEnergy = 0.f;
		for (int k = -kOverlayBandRadius; k <= kOverlayBandRadius; k++) {
			const int sampleBin = clamp(bin + k, 0, kFftBinCount - 1);
			const float w = kOverlayBandKernel[k + kOverlayBandRadius];
			rawInputEnergy += w * binRawInputAmp[sampleBin] * binRawInputAmp[sampleBin];
			outputEnergy += w * binOutputAmp[sampleBin] * binOutputAmp[sampleBin];
		}
		const float rawInputAmp = std::sqrt(std::max(0.f, rawInputEnergy));
		const float outputAmp = std::sqrt(std::max(0.f, outputEnergy));
		binModuleDeltaDb[bin] = softLimitOverlayDeltaDb(20.f * std::log10((outputAmp + 1e-6f) / (rawInputAmp + 1e-6f)));
		binOutputDbfs[bin] = clamp(20.f * std::log10(outputAmp / 5.f + 1e-6f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}
	float sampledModuleDeltaDb[kCurvePointCount];
	float sampledOutputDbfs[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; i++) {
		const float binPos = curveBinPos[i];
		const int binA = std::max(2, int(std::floor(binPos)));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPos - float(binA);
		sampledModuleDeltaDb[i] = mixf(binModuleDeltaDb[binA], binModuleDeltaDb[binB], frac);
		sampledOutputDbfs[i] = mixf(binOutputDbfs[binA], binOutputDbfs[binB], frac);
	}
	float framePeakDbfs = kOverlayDbfsFloor;
	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; i++) {
		const int left = std::max(0, i - 1);
		const int right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothModuleDeltaDb = 0.12f * sampledModuleDeltaDb[left] + 0.76f * sampledModuleDeltaDb[i] + 0.12f * sampledModuleDeltaDb[right];
		const float smoothOutputDbfs = 0.12f * sampledOutputDbfs[left] + 0.76f * sampledOutputDbfs[i] + 0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
	}
	if (!hasOverlayTarget) {
		for (int i = 0; i < kCurvePointCount; i++) {
			overlayModuleDb[i] = overlayTargetModuleDb[i];
			overlayOutputDbfs[i] = overlayTargetOutputDbfs[i];
		}
		hasOverlayTarget = true;
	}
	if (module && !module->fftScaleDynamic) {
		displayTopTargetDbfs = kDisplayTopDbfsCeiling;
	}
	else {
		float sortedOutputDbfs[kCurvePointCount];
		for (int i = 0; i < kCurvePointCount; i++) sortedOutputDbfs[i] = frameSmoothedOutputDbfs[i];
		const int p95Index = int(0.95f * float(kCurvePointCount - 1));
		std::nth_element(sortedOutputDbfs, sortedOutputDbfs + p95Index, sortedOutputDbfs + kCurvePointCount);
		const float robustTopRefDbfs = std::max(sortedOutputDbfs[p95Index], framePeakDbfs - 18.f);
		displayTopTargetDbfs = clamp(std::max(robustTopRefDbfs + 6.f, framePeakDbfs + kDisplayPeakHeadroomDb), kDisplayTopDbfsFloor, kDisplayTopDynamicCeilingDbfs);
	}
}

void BifurxSpectrumWidget::draw(const DrawArgs& args) {
	if (!hasPreview) return;
	const float w = box.size.x; const float h = box.size.y;
	if (!(w > 0.f && h > 0.f)) return;
	using PerfClock = std::chrono::steady_clock;
	const bool perfLoggingActive = module && module->perfDebugLogging;
	const PerfClock::time_point perfDrawStart = perfLoggingActive ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfSectionStart = perfDrawStart;
	auto recordDrawSection = [&](uint64_t& count, uint64_t& totalNs) {
		if (!perfLoggingActive) return;
		const uint64_t ns = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfSectionStart).count();
		count++; totalNs += ns; perfSectionStart = PerfClock::now();
	};
	const float padX = 0.f; const float padY = std::max(4.f, h * 0.035f);
	const float plotX = padX; const float usableW = std::max(1.f, w - plotX - padX);
	const float minHz = 10.f; const float maxHz = std::min(20000.f, 0.46f * previewState.sampleRate);
	const float labelBandHeight = std::max(5.2f, h * 0.072f);
	const float labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f;
	const float spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	bottomY = spectrumBottomY;
	const float displayMaxDbfs = displayTopDbfs;
	const float displayMinDbfs = displayMaxDbfs - kDisplayDbfsSpan;
	auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
	const bool anchorMarkerToBottomLane = (previewState.mode == 3);
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
	const BifurxPreviewModel model = makePreviewModel(previewState);
	updateCurveXCache(plotX, usableW);
	for (int i = 0; i < kCurvePointCount; i++) curveY[i] = responseYForDb(curveDb[i]);

	auto displayAnchorForHz = [&](float targetHz) {
		struct DisplayAnchor { float x01 = 0.f; float hz = 0.f; };
		const float clampedHz = clamp(targetHz, minHz, maxHz);
		DisplayAnchor anchor; anchor.x01 = logPosition(clampedHz, minHz, maxHz); anchor.hz = clampedHz;
		const bool valleyAnchorEligible = (previewState.mode == 2) || (previewState.mode == 3) || (previewState.mode == 7);
		if (!valleyAnchorEligible) return anchor;
		const int centerIndex = clamp(int(std::round(anchor.x01 * float(kCurvePointCount - 1))), 0, kCurvePointCount - 1);
		int bestIndex = centerIndex; float bestScore = curveDb[centerIndex];
		for (int i = std::max(0, centerIndex - 18); i <= std::min(kCurvePointCount - 1, centerIndex + 18); ++i) {
			const float score = curveDb[i] + 0.22f * std::fabs(float(i - centerIndex));
			if (score < bestScore) { bestScore = score; bestIndex = i; }
		}
		anchor.x01 = float(bestIndex) / float(kCurvePointCount - 1);
		anchor.hz = logFrequencyAt(anchor.x01, minHz, maxHz);
		return anchor;
	};
	auto curveYAtX01 = [&](float x01) {
		const float curveIndex = clamp(x01, 0.f, 1.f) * float(kCurvePointCount - 1);
		const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1);
		const int i1 = std::min(i0 + 1, kCurvePointCount - 1);
		return responseYForDb(mixf(curveDb[i0], curveDb[i1], curveIndex - float(i0)));
	};

	struct CurveDrawPoint { float x01 = 0.f; float x = 0.f; float y = 0.f; int priority = 0; };
	CurveDrawPoint dedupedCurveDrawPoints[kCurvePointCount + 6]; int dedupedCurveDrawPointCount = 0;
	for (int i = 0; i < kCurvePointCount; ++i) {
		CurveDrawPoint point; point.x01 = float(i) / float(kCurvePointCount - 1); point.x = curveX[i]; point.y = curveY[i];
		dedupedCurveDrawPoints[dedupedCurveDrawPointCount++] = point;
	}
	auto insertCurveDrawPoint = [&](const CurveDrawPoint& point) {
		int insertIndex = dedupedCurveDrawPointCount;
		for (int i = 0; i < dedupedCurveDrawPointCount; ++i) {
			if (std::fabs(point.x01 - dedupedCurveDrawPoints[i].x01) <= 1e-6f) {
				if (point.priority >= dedupedCurveDrawPoints[i].priority) dedupedCurveDrawPoints[i] = point;
				return;
			}
			if (point.x01 < dedupedCurveDrawPoints[i].x01) { insertIndex = i; break; }
		}
		if (dedupedCurveDrawPointCount >= (kCurvePointCount + 6)) return;
		for (int i = dedupedCurveDrawPointCount; i > insertIndex; --i) dedupedCurveDrawPoints[i] = dedupedCurveDrawPoints[i - 1];
		dedupedCurveDrawPoints[insertIndex] = point; dedupedCurveDrawPointCount++;
	};
	auto addCurveRefinementAroundX01 = [&](float targetX01) {
		const float clampedX01 = clamp(targetX01, 0.f, 1.f); const float refineDx = 0.35f / float(kCurvePointCount - 1);
		const float refineX01[3] = { clamp(clampedX01 - refineDx, 0.f, 1.f), clampedX01, clamp(clampedX01 + refineDx, 0.f, 1.f) };
		for (int i = 0; i < 3; ++i) {
			CurveDrawPoint p; p.x01 = refineX01[i]; p.x = plotX + usableW * p.x01; p.y = curveYAtX01(p.x01);
			if (anchorMarkerToBottomLane && i == 1) { p.y = markerBottomLaneY; p.priority = 2; }
			else p.priority = 1;
			insertCurveDrawPoint(p);
		}
	};
	const auto markerAAnchor = displayAnchorForHz(model.markerFreqA);
	const auto markerBAnchor = displayAnchorForHz(model.markerFreqB);
	addCurveRefinementAroundX01(markerAAnchor.x01); addCurveRefinementAroundX01(markerBAnchor.x01);
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
	recordDrawSection(uiDrawBackgroundCount, uiDrawBackgroundNs);

	const NVGcolor expectedPurple = nvgRGB(122, 92, 255), expectedCyan = nvgRGB(28, 204, 217), expectedWhite = nvgRGB(206, 210, 216);
	auto drawExpectedGuideStroke = [&](float x, float y, float curveDbValue) {
		const float posAmount = clamp01(curveDbValue / 18.f), negAmount = clamp01(-curveDbValue / 18.f), emphasis = std::max(posAmount, negAmount);
		NVGcolor tint = expectedWhite; if (posAmount > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posAmount * 1.35f)); if (negAmount > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negAmount * 1.25f));
		tint.a = 0.025f + 0.095f * emphasis; nvgBeginPath(args.vg); nvgMoveTo(args.vg, x, spectrumBottomY); nvgLineTo(args.vg, x, y);
		nvgStrokeColor(args.vg, tint); nvgStrokeWidth(args.vg, 1.05f); nvgStroke(args.vg);
	};
	const float markerGuideClearanceX01 = 1.35f / float(kCurvePointCount - 1);
	for (int i = 0; i < kCurvePointCount; i += 3) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		if (std::fabs(x01 - markerAAnchor.x01) < markerGuideClearanceX01 || std::fabs(x01 - markerBAnchor.x01) < markerGuideClearanceX01) continue;
		drawExpectedGuideStroke(curveX[i], curveY[i], curveDb[i]);
	}
	auto drawExpectedMarkerGuideStroke = [&](float targetHz) {
		const auto anchor = displayAnchorForHz(targetHz); if (anchor.x01 < 0.f || anchor.x01 > 1.f) return;
		const float x = plotX + usableW * anchor.x01; if (x < plotX + markerOuterRadius + kPeakMarkerEdgePadding || x > plotX + usableW - markerOuterRadius - kPeakMarkerEdgePadding) return;
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, x, spectrumBottomY); nvgLineTo(args.vg, x, anchorMarkerToBottomLane ? markerBottomLaneY : curveYAtX01(anchor.x01));
		nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 150)); nvgStrokeWidth(args.vg, 1.05f * 1.45f); nvgStroke(args.vg);
	};
	drawExpectedMarkerGuideStroke(model.markerFreqA); drawExpectedMarkerGuideStroke(model.markerFreqB);
	recordDrawSection(uiDrawExpectedCount, uiDrawExpectedNs);

	if (hasOverlay) {
		for (int i = 0; i < kCurvePointCount - 1; ++i) {
			const float avgDeltaDb = 0.5f * (overlayModuleDb[i] + overlayModuleDb[i + 1]), avgOutputDbfs = 0.5f * (overlayOutputDbfs[i] + overlayOutputDbfs[i + 1]), energyAmount = clamp01(rescale(avgOutputDbfs, displayMinDbfs, displayMaxDbfs, 0.f, 1.f));
			if (energyAmount <= 0.005f) continue;
			const float posAmount = clamp01(avgDeltaDb / 18.f), negAmount = clamp01(-avgDeltaDb / 18.f);
			NVGcolor tint = expectedWhite; if (posAmount > 0.f) tint = mixColor(tint, expectedCyan, clamp01(posAmount * 1.40f)); if (negAmount > 0.f) tint = mixColor(tint, expectedPurple, clamp01(negAmount * 1.25f));
			NVGcolor fill = mixColor(expectedWhite, tint, 0.55f + 0.45f * energyAmount); fill.a = 1.f;
			nvgBeginPath(args.vg); nvgMoveTo(args.vg, curveX[i] - 0.45f, spectrumYForDbfs(overlayOutputDbfs[i])); nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumYForDbfs(overlayOutputDbfs[i + 1]));
			nvgLineTo(args.vg, curveX[i + 1] + 0.45f, spectrumBottomY); nvgLineTo(args.vg, curveX[i] - 0.45f, spectrumBottomY); nvgClosePath(args.vg);
			nvgFillColor(args.vg, fill); nvgFill(args.vg);
		}
		nvgBeginPath(args.vg); for (int i = 0; i < kCurvePointCount; ++i) { float y = responseYForDb(overlayModuleDb[i]); if (i == 0) nvgMoveTo(args.vg, curveX[i], y); else nvgLineTo(args.vg, curveX[i], y); }
		NVGcolor ml = mixColor(expectedWhite, expectedCyan, 0.35f); ml.a = 0.95f; nvgStrokeWidth(args.vg, 1.4f); nvgStrokeColor(args.vg, ml); nvgStroke(args.vg);
		recordDrawSection(uiDrawOverlayCount, uiDrawOverlayNs);
	}

	nvgBeginPath(args.vg);
	for (int i = 0; i < dedupedCurveDrawPointCount; ++i) { if (i == 0) nvgMoveTo(args.vg, dedupedCurveDrawPoints[i].x, dedupedCurveDrawPoints[i].y); else nvgLineTo(args.vg, dedupedCurveDrawPoints[i].x, dedupedCurveDrawPoints[i].y); }
	nvgStrokeColor(args.vg, nvgRGBA(255, 248, 208, 244)); nvgLineJoin(args.vg, NVG_ROUND); nvgLineCap(args.vg, NVG_ROUND); nvgStrokeWidth(args.vg, 1.35f); nvgStroke(args.vg);
	recordDrawSection(uiDrawCurveCount, uiDrawCurveNs);
	nvgRestore(args.vg);

	struct PeakMarker { float x = 0.f; float yCurve = 0.f; float yMarker = 0.f; float hz = 0.f; bool visible = false; char label[16] = {}; };
	auto buildMarkerAtFrequency = [&](float targetHz) {
		PeakMarker marker; const auto anchor = displayAnchorForHz(targetHz); const float safeHz = std::max(anchor.hz, 1e-6f);
		const float markerX = plotX + usableW * anchor.x01; if (markerX < plotX + markerOuterRadius + kPeakMarkerEdgePadding || markerX > plotX + usableW - markerOuterRadius - kPeakMarkerEdgePadding) { marker.visible = false; return marker; }
		marker.x = markerX; marker.yCurve = curveYAtX01(anchor.x01); const float markerMinY = spectrumTopY + markerOuterRadius + kPeakMarkerEdgePadding, markerMaxY = spectrumBottomY - markerOuterRadius - kPeakMarkerEdgePadding;
		marker.yMarker = anchorMarkerToBottomLane ? (spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding) : clamp(marker.yCurve, markerMinY, markerMaxY);
		marker.hz = safeHz; marker.visible = true; formatFrequencyLabel(marker.hz, marker.label, sizeof(marker.label)); return marker;
	};
	PeakMarker peaks[2] = { buildMarkerAtFrequency(model.markerFreqA), buildMarkerAtFrequency(model.markerFreqB) };
	float labelX[2] = { peaks[0].x, peaks[1].x }; const float labelMargin = std::max(18.f, w * 0.08f), minLabelSeparation = std::max(30.f, w * 0.18f), minX = plotX + labelMargin, maxX = plotX + usableW - labelMargin;
	if (peaks[0].visible && peaks[1].visible) {
		const int leftIndex = (labelX[0] <= labelX[1]) ? 0 : 1, rightIndex = 1 - leftIndex;
		float leftX = clamp(labelX[leftIndex], minX, maxX), rightX = clamp(labelX[rightIndex], minX, maxX), needed = std::min(minLabelSeparation, std::max(0.f, maxX - minX)) - (rightX - leftX);
		if (needed > 0.f) { float moveLeft = std::min(0.5f * needed, leftX - minX), moveRight = std::min(0.5f * needed, maxX - rightX); leftX -= moveLeft; rightX += moveRight; needed -= (moveLeft + moveRight); if (needed > 0.f) { float extraLeft = std::min(needed, leftX - minX); leftX -= extraLeft; needed -= extraLeft; } if (needed > 0.f) rightX += std::min(needed, maxX - rightX); }
		labelX[leftIndex] = leftX; labelX[rightIndex] = rightX;
	} else { for (int i = 0; i < 2; ++i) if (peaks[i].visible) labelX[i] = clamp(labelX[i], minX, maxX); }
	const float freqLabelFontSize = std::max(7.f, h * 0.055f), labelTextY = labelBandTop + 0.5f * labelBandHeight, guideYBottom = clamp(labelBandTop + std::min(2.1f, 0.18f * labelBandHeight), labelBandTop + 0.2f, labelTextY - 0.5f * freqLabelFontSize - 0.6f);
	for (int i = 0; i < 2; ++i) {
		if (!peaks[i].visible) continue;
		nvgBeginPath(args.vg); nvgMoveTo(args.vg, peaks[i].x, peaks[i].yMarker + kPeakMarkerFillRadius + 0.45f); nvgLineTo(args.vg, peaks[i].x, guideYBottom); nvgStrokeColor(args.vg, nvgRGBA(252, 236, 176, 170)); nvgStrokeWidth(args.vg, 1.1f); nvgStroke(args.vg);
		nvgBeginPath(args.vg); nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, kPeakMarkerFillRadius); nvgFillColor(args.vg, nvgRGBA(252, 255, 255, 244)); nvgFill(args.vg);
		nvgBeginPath(args.vg); nvgCircle(args.vg, peaks[i].x, peaks[i].yMarker, kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius); nvgStrokeColor(args.vg, nvgRGBA(8, 10, 14, 220)); nvgStrokeWidth(args.vg, kPeakMarkerOutlineStrokeWidth); nvgStroke(args.vg);
	}
	nvgFontSize(args.vg, freqLabelFontSize); nvgFontFaceId(args.vg, APP->window->uiFont->handle); nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	for (int i = 0; i < 2; ++i) { if (!peaks[i].visible) continue; nvgFillColor(args.vg, nvgRGBA(4, 6, 9, 240)); nvgText(args.vg, labelX[i], labelTextY + 0.75f, peaks[i].label, nullptr); nvgFillColor(args.vg, nvgRGBA(241, 246, 252, 250)); nvgText(args.vg, labelX[i], labelTextY, peaks[i].label, nullptr); }
	nvgResetScissor(args.vg); nvgRestore(args.vg);
	recordDrawSection(uiDrawMarkersCount, uiDrawMarkersNs);
	if (perfLoggingActive) {
		const uint64_t drawNs = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfDrawStart).count();
		uiDrawCount++; uiDrawNs += drawNs; uiDrawMaxNs = std::max(uiDrawMaxNs, drawNs);
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
		BifurxSpectrumWidget* s = new BifurxSpectrumWidget(); s->module = module; s->framebuffer = addFb(sRect, s);
		BifurxModeReadoutWidget* mR = new BifurxModeReadoutWidget(); mR->module = module; mR->box.pos = mm2px(Vec(sRect.pos.x, sRect.pos.y + sRect.size.y + 0.9f)); mR->box.size = mm2px(Vec(sRect.size.x, 4.2f)); addChild(mR);
		Vec mP(13.4f, 22.f), lP(13.4f, 41.f), rP(13.4f, 60.f), fP(35.56f, 46.5f), tP(57.7f, 22.f), sP(57.7f, 41.f), bP(57.7f, 60.f), faP(25.3f, 45.f), saP(45.82f, 45.f);
		Vec iP(7.6f, 112.2f), vP(17.15f, 112.2f), fmP(26.7f, 112.2f), rcP(36.25f, 112.2f), bcP(45.8f, 112.2f), scP(55.35f, 112.2f), oP(64.9f, 112.2f);
		applyPt("MODE_PARAM", &mP); applyPt("LEVEL_PARAM", &lP); applyPt("RESO_PARAM", &rP); applyPt("FREQ_PARAM", &fP); applyPt("TITO_PARAM", &tP); applyPt("SPAN_PARAM", &sP); applyPt("BALANCE_PARAM", &bP); applyPt("FM_AMT_PARAM", &faP); applyPt("SPAN_CV_ATTEN_PARAM", &saP);
		applyPt("IN_INPUT", &iP); applyPt("VOCT_INPUT", &vP); applyPt("FM_INPUT", &fmP); applyPt("RESO_CV_INPUT", &rcP); applyPt("BALANCE_CV_INPUT", &bcP); applyPt("SPAN_CV_INPUT", &scP); applyPt("OUT_OUTPUT", &oP);
		addParam(createParamCentered<BifurxModeLeftButton>(mm2px(mP.plus(Vec(-2.5f, 0.f))), module, Bifurx::MODE_LEFT_PARAM)); addParam(createParamCentered<BifurxModeRightButton>(mm2px(mP.plus(Vec(2.5f, 0.f))), module, Bifurx::MODE_RIGHT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(lP), module, Bifurx::LEVEL_PARAM)); addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(fP), module, Bifurx::FREQ_PARAM)); addParam(createParamCentered<RoundBlackKnob>(mm2px(rP), module, Bifurx::RESO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(bP), module, Bifurx::BALANCE_PARAM)); addParam(createParamCentered<RoundBlackKnob>(mm2px(sP), module, Bifurx::SPAN_PARAM)); addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(faP), module, Bifurx::FM_AMT_PARAM, Bifurx::FM_AMT_POS_LIGHT));
		addParam(createLightParamCentered<VCVLightSlider<GreenRedLight>>(mm2px(saP), module, Bifurx::SPAN_CV_ATTEN_PARAM, Bifurx::SPAN_CV_ATTEN_POS_LIGHT)); addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(tP), module, Bifurx::TITO_PARAM));
		addInput(createInputCentered<PJ301MPort>(mm2px(iP), module, Bifurx::IN_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(vP), module, Bifurx::VOCT_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(fmP), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(rcP), module, Bifurx::RESO_CV_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(bcP), module, Bifurx::BALANCE_CV_INPUT)); addInput(createInputCentered<PJ301MPort>(mm2px(scP), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<BananutBlack>(mm2px(oP), module, Bifurx::OUT_OUTPUT));
	}
	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu); Bifurx* bifurx = dynamic_cast<Bifurx*>(module); if (!bifurx) return;
		menu->addChild(new MenuSeparator()); menu->addChild(createBoolPtrMenuItem("Dynamic FFT Scale", "", &bifurx->fftScaleDynamic)); menu->addChild(createBoolPtrMenuItem("Log Curve Debug", "", &bifurx->curveDebugLogging)); menu->addChild(createBoolPtrMenuItem("Log Performance Debug", "", &bifurx->perfDebugLogging));
	}
};

} // namespace bifurx

Model* modelBifurx = createModel<bifurx::Bifurx, bifurx::BifurxWidget>("Bifurx");
