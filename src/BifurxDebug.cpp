#include "BifurxDebug.hpp"
#include "BifurxSpectrumNanoVG.hpp"
#include "DebugTerminalTransport.hpp"

#include <unordered_map>

namespace bifurx {

bool shouldSubmitBifurxDebugPacket(uint32_t instanceId, double nowSec) {
	static std::unordered_map<uint32_t, double> lastSubmitByInstance;
	double& lastSubmitSec = lastSubmitByInstance[instanceId];
	if (lastSubmitSec > 0.0 &&
		nowSec - lastSubmitSec < debug_terminal::kTimingRangeSubmitIntervalSec) return false;
	lastSubmitSec = nowSec;
	return true;
}

void BifurxSpectrumWidget::syncCurveDebugCaptureState() {
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

void BifurxSpectrumWidget::syncPerfDebugCaptureState() {
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

void BifurxSpectrumWidget::startCurveDebugCapture() {
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

void BifurxSpectrumWidget::stopCurveDebugCapture() {
		if (!curveDebugRecorder.active) return;
		curveDebugRecorder.file.close();
		curveDebugRecorder.active = false;
		DEBUG("Stopped curve debug capture");
	}

void BifurxSpectrumWidget::startPerfDebugCapture() {
		if (perfDebugRecorder.active) return;
		system::createDirectories(bifurxUserRootPath());
		perfDebugRecorder.path = system::join(bifurxUserRootPath(), "perf_debug_" + std::to_string(std::time(nullptr)) + ".csv");
		perfDebugRecorder.file.open(perfDebugRecorder.path);
		if (perfDebugRecorder.file.is_open()) {
			perfDebugRecorder.file << "Process,Step,Draw,ProcessMin,ProcessMax,StepMin,StepMax,DrawMin,DrawMax,CurvePrep,OverlayPrep,Surface,WorkerSubmit,Renderer\n";
			perfDebugRecorder.active = true;
			perfDebugRecorder.startTimeSec = system::getTime();
			perfDebugRecorder.sequence = 0;
			perfDebugRecorder.lastLogTimeSec = -1.0;
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

void BifurxSpectrumWidget::stopPerfDebugCapture() {
		if (!perfDebugRecorder.active) return;
		perfDebugRecorder.file.close();
		perfDebugRecorder.active = false;
		DEBUG("Stopped performance debug capture");
	}

void BifurxSpectrumWidget::logCurveDebugSample(
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

	// One module-owned interval is shared with Debug Terminal. Values are CPU
	// microseconds; Step includes step-time surface rendering, while Draw measures
	// only BifurxWidget::draw(). Surface is also reported separately.
void BifurxSpectrumWidget::logPerfDebugSample(const debug_terminal::TimingRangeUs& process,
		const debug_terminal::TimingRangeUs& step, const debug_terminal::TimingRangeUs& draw,
		const BifurxSpectrumBase* active, bool gl) {
		if (!isDragonKingDebugEnabled() || !perfDebugRecorder.active || !active) return;
		perfDebugRecorder.file << process.average << "," << step.average << "," << draw.average << ","
			<< process.min << "," << process.max << "," << step.min << "," << step.max << ","
			<< draw.min << "," << draw.max << "," << active->lastCurvePrepUs << ","
			<< active->lastOverlayPrepUs << "," << active->lastSurfaceRenderUs << ","
			<< active->renderClient.lastSubmitUs() << "," << (gl ? "OpenGL" : "NanoVG") << "\n";
	}

void BifurxSpectrumWidget::recordCurveDebug(BifurxSpectrumBase& source) {
	if (!module || !isDragonKingDebugEnabled() || !curveDebugRecorder.active) return;
	const auto& state = source.state;
	const bool previewUpdated = state.curvePreviewSeq != lastRecordedPreviewSeq;
	const bool analysisUpdated = state.lastAnalysisSeq != lastRecordedAnalysisSeq;
	lastRecordedPreviewSeq = state.curvePreviewSeq;
	lastRecordedAnalysisSeq = state.lastAnalysisSeq;
	uint32_t llTelemetrySeq = lastLlTelemetrySeq;
	BifurxLlTelemetryState telemetryState;
	if (module->readLlTelemetryState(lastLlTelemetrySeq, &telemetryState, &llTelemetrySeq)) {
		llTelemetryState = telemetryState;
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
				const BifurxPreviewModel& model = source.getOrUpdateModel();
				const float padY = std::max(4.f, h * 0.035f);
				const float plotX = 0.f, usableW = std::max(1.f, w - plotX);
				const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.displayedPreviewState.sampleRate);
				const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
				const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
				auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
				auto evalPeak = [&](int idx, float targetHz, float* outX, float* outYCurve, float* outYMarker) {
					const auto anchor = source.displayAnchorForMarker(idx, targetHz, minHz, maxHz);
					const float markerRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
					const float curveIndex = anchor.x01 * float(kCurvePointCount - 1);
					const int i0 = clamp(int(std::floor(curveIndex)), 0, kCurvePointCount - 1), i1 = std::min(i0 + 1, kCurvePointCount - 1);
					const float curveDbAtHz = mixf(state.curveDb[i0], state.curveDb[i1], curveIndex - float(i0));
					const float yCurve = responseYForDb(curveDbAtHz), markerX = plotX + usableW * anchor.x01;
					const float markerMinY = spectrumTopY + markerRadius + kPeakMarkerEdgePadding, markerMaxY = spectrumBottomY - markerRadius - kPeakMarkerEdgePadding;
					const float yMarker = source.markerPinnedToBottomLane(idx) ? (spectrumBottomY - markerRadius - kPeakMarkerBottomLanePadding) : clamp(yCurve, markerMinY, markerMaxY);
					*outX = clamp(markerX, plotX + markerRadius + kPeakMarkerEdgePadding, plotX + usableW - markerRadius - kPeakMarkerEdgePadding);
					*outYCurve = yCurve; *outYMarker = yMarker;
				};
				evalPeak(0, model.markerFreqA, &peakAX, &peakAYCurve, &peakAYMarker);
				evalPeak(1, model.markerFreqB, &peakBX, &peakBYCurve, &peakBYMarker);
			}

			logCurveDebugSample(state.displayedPreviewState, hasLlTelemetry ? llTelemetryState : BifurxLlTelemetryState{}, peakAX, peakAYCurve, peakAYMarker, peakBX, peakBYCurve, peakBYMarker, uiFrameMs, state.curvePreviewSeq, previewUpdated, state.lastAnalysisSeq, analysisUpdated);
		}
	}
	else if (lastCurveDebugLogTimeSec >= 0.0) {
		lastCurveDebugLogTimeSec = -1.0;
	}

}

} // namespace bifurx
