#include "BifurxDisplay.hpp"
#include "MathHelpers.hpp"
#include "BifurxRenderData.hpp"
#include "BifurxRenderPrep.hpp"
#include "BifurxWorker.hpp"

namespace bifurx {

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t) {
	const float clampedT = levi_math::clamp01(t);
	NVGcolor out;
	out.r = bifurx::mixf(a.r, b.r, clampedT);
	out.g = bifurx::mixf(a.g, b.g, clampedT);
	out.b = bifurx::mixf(a.b, b.b, clampedT);
	out.a = bifurx::mixf(a.a, b.a, clampedT);
	return out;
}

float displayOnlyColorTone(float energy, float shapeControl) {
	const float e = levi_math::clamp01(energy);
	const float ctl = clamp(shapeControl, -1.f, 1.f);
	if (ctl < 0.f) {
		// Cool side: blend linear -> squared to delay hot color.
		const float sq = e * e;
		return bifurx::mixf(e, sq, -ctl);
	}
	// Hot side: blend linear -> fast-rising polynomial.
	const float hot = e * (2.f - e);
	return bifurx::mixf(e, hot, ctl);
}

BifurxColors BifurxColors::get(Bifurx::ColorScheme scheme, bool threeColorGradient) {
	BifurxColors palette;
	switch (scheme) {
		case Bifurx::SCHEME_CLASSIC:
			palette = {nvgRGBA(0x00, 0xff, 0x00, 0xff), nvgRGBA(0xff, 0x00, 0x00, 0xff), nvgRGBA(0xce, 0xd2, 0xd8, 0xff)};
			break;
		case Bifurx::SCHEME_MONOCHROME:
			palette = {nvgRGBA(0x40, 0x40, 0x40, 0xff), nvgRGBA(0xff, 0xff, 0xff, 0xff), nvgRGBA(0xce, 0xd2, 0xd8, 0xff)};
			break;
		case Bifurx::SCHEME_FIRE:
			palette = {nvgRGBA(0x80, 0x00, 0x00, 0xff), nvgRGBA(0xff, 0xff, 0x00, 0xff), nvgRGBA(0xce, 0xd2, 0xd8, 0xff)};
			break;
		case Bifurx::SCHEME_RETRO_AMBER:
			palette = {nvgRGBA(0x5a, 0x2f, 0x00, 0xff), nvgRGBA(0xff, 0xb8, 0x3d, 0xff), nvgRGBA(0xff, 0xd8, 0x8a, 0xff)};
			break;
		case Bifurx::SCHEME_RETRO_GREEN:
			palette = {nvgRGBA(0x0b, 0x3d, 0x22, 0xff), nvgRGBA(0x49, 0xff, 0x8f, 0xff), nvgRGBA(0xb7, 0xff, 0xcc, 0xff)};
			break;
		case Bifurx::SCHEME_DEFAULT:
		default:
			palette = {nvgRGBA(0x7a, 0x5c, 0xff, 0xff), nvgRGBA(0x1c, 0xcc, 0xd9, 0xff), nvgRGBA(0xce, 0xd2, 0xd8, 0xff)};
			break;
	}
	if (!threeColorGradient) {
		palette.white = mixColor(palette.low, palette.high, 0.5f);
	}
	return palette;
}

void formatFrequencyLabel(float hz, char* out, size_t outSize) {
	const float safeHz = std::max(hz, 0.f);
	if (safeHz >= 1000.f) {
		if (safeHz >= 10000.f) {
			std::snprintf(out, outSize, "%.1fkHz", safeHz / 1000.f);
		}
		else {
			std::snprintf(out, outSize, "%.2fkHz", safeHz / 1000.f);
		}
		return;
	}
	if (safeHz >= 100.f) {
		std::snprintf(out, outSize, "%.0fHz", safeHz);
		return;
	}
	if (safeHz >= 10.f) {
		std::snprintf(out, outSize, "%.1fHz", safeHz);
		return;
	}
	std::snprintf(out, outSize, "%.2fHz", safeHz);
}

namespace {
struct SynchronousOverlayScratch {
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize] {};
	alignas(16) float fftOutputTime[kFftSize] {};
	alignas(16) float fftOutputFreq[2 * kFftSize] {};
	alignas(16) float fftRawInputFreq[2 * kFftSize] {};

	SynchronousOverlayScratch() : fft(kFftSize) {
		for (int i = 0; i < kFftSize; ++i) {
			window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
		}
	}
};

inline std::unique_ptr<SynchronousOverlayScratch>& synchronousOverlayScratchSlot() {
	thread_local std::unique_ptr<SynchronousOverlayScratch> scratch;
	return scratch;
}

inline bool synchronousOverlayScratchAllocatedForCurrentThread() {
	return bool(synchronousOverlayScratchSlot());
}

SynchronousOverlayScratch& synchronousOverlayScratch() {
	auto& scratch = synchronousOverlayScratchSlot();
	if (!scratch) {
		scratch.reset(new SynchronousOverlayScratch());
	}
	return *scratch;
}

} // namespace

#if defined(BIFURX_DISPLAY_TEST_HOOKS)
bool synchronousOverlayScratchAllocatedForCurrentThreadForTest() {
	return synchronousOverlayScratchAllocatedForCurrentThread();
}

const void* synchronousOverlayScratchIdentityForTest() {
	return &synchronousOverlayScratch();
}

size_t synchronousOverlayScratchBytesForTest() {
	return sizeof(SynchronousOverlayScratch);
}
#endif

namespace {

inline void prepareCurveTargets(const BifurxPreviewModel& model, const float* curveHz, float* curveTargetDb) {
	if (isBifurxDisplayOnlyMode(model.mode)) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			curveTargetDb[i] = 0.f;
		}
		return;
	}
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float db = previewModelResponseDb(model, curveHz[i]);
		curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
	}
}

} // namespace

void BifurxSpectrumBase::syncBase() {
	if (module != lastBoundModule) {
		releaseWorkerRegistration();
		lastBoundModule = module;
		lastAnalysisGeneration = 0;
		lastModelUpdateSeq = 0;
		cachedMarkerLayoutValid = false;
		refinedCurveTemplateValid = false;
		state.hasPreview = state.hasCurve = state.hasCurveTarget = false;
		state.hasOverlay = state.hasOverlayTarget = false;
		state.lastPreviewSeq = state.curvePreviewSeq = 0;
		state.lastAnalysisSeq = state.overlayAnalysisSeq = 0;
		state.cachedAxisSampleRate = 0.f;
	}
	if (!module) return;
	if (!presentationActive) {
		releaseWorkerRegistration();
		return;
	}
	const uint32_t generation = module->analysisGeneration.load(std::memory_order_acquire);
	if (generation != lastAnalysisGeneration) {
		lastAnalysisGeneration = generation;
		releaseWorkerRegistration();
		state.hasOverlay = state.hasOverlayTarget = false;
		state.lastAnalysisSeq = state.overlayAnalysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i] = kOverlayDbfsFloor;
			state.overlayModuleDb[i] = state.overlayTargetModuleDb[i] = 0.f;
		}
	}
	bool useWorkerCurve = shouldUseVisualWorker();
	if (useWorkerCurve) {
		useWorkerCurve = ensureWorkerRegistration();
	}
	else {
		releaseWorkerRegistration();
	}
	BifurxPreviewState previewState;
	double previewPublishTimeSec = 0.0;
	uint32_t previewSeq = 0;
	if (module->readPreviewState(
		state.lastPreviewSeq,
		&previewState,
		&previewPublishTimeSec,
		&previewSeq
	)) {
		if (state.hasPreview &&
			std::fabs(previewState.sampleRate - state.previewState.sampleRate) > 0.5f) {
			releaseWorkerRegistration();
			useWorkerCurve = false;
			state.hasCurve = state.hasCurveTarget = false;
			state.hasOverlay = state.hasOverlayTarget = false;
			state.overlayAnalysisSeq = 0;
			state.cachedAxisSampleRate = 0.f;
			cachedMarkerLayoutValid = false;
			refinedCurveTemplateValid = false;
		}
		state.previewState = previewState;
		state.previewPublishTimeSec = previewPublishTimeSec;
		state.hasPreview = true;
		state.lastPreviewSeq = previewSeq;
	}
	// Also catch up if worker mode was disabled with a preview still in flight.
	if (!useWorkerCurve && state.hasPreview
		&& (!state.hasCurve || state.curvePreviewSeq != state.lastPreviewSeq)) {
		updateAxisCache();
		const bool measurePrep = isDragonKingDebugEnabled();
		const auto curvePrepStart = measurePrep ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
		updateCurveCache();
		if (measurePrep) {
			lastCurvePrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - curvePrepStart).count());
		}
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != state.lastAnalysisSeq
		|| (!useWorkerCurve && analysisSeq != state.overlayAnalysisSeq)) {
		if (!useWorkerCurve) {
			const bool measurePrep = isDragonKingDebugEnabled();
			const auto overlayPrepStart = measurePrep ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
			uint32_t copiedAnalysisSeq = state.lastAnalysisSeq;
			const bool copiedAnalysis = updateOverlayCache(&copiedAnalysisSeq);
			if (measurePrep) {
				lastOverlayPrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::steady_clock::now() - overlayPrepStart).count());
			}
			if (copiedAnalysis) {
				state.lastAnalysisSeq = copiedAnalysisSeq;
				state.overlayAnalysisSeq = copiedAnalysisSeq;
				state.hasOverlay = true;
			}
		}
		else {
			state.lastAnalysisSeq = analysisSeq;
		}
	}

	if (useWorkerCurve) {
		if (!submitWorkerCurveRequest()) {
			// Admission can fail during plugin shutdown or a service stop. An active
			// display still needs the latest published targets on this UI tick.
			releaseWorkerRegistration();
			if (state.hasPreview && (!state.hasCurve || state.curvePreviewSeq != state.lastPreviewSeq)) {
				updateAxisCache();
				updateCurveCache();
			}
			if (state.hasPreview && state.overlayAnalysisSeq != state.lastAnalysisSeq) {
				uint32_t copiedAnalysisSeq = state.overlayAnalysisSeq;
				if (updateOverlayCache(&copiedAnalysisSeq)) {
					state.lastAnalysisSeq = copiedAnalysisSeq;
					state.overlayAnalysisSeq = copiedAnalysisSeq;
					state.hasOverlay = true;
				}
			}
		}
	}
}

BifurxSpectrumBase::~BifurxSpectrumBase() {
}

bool BifurxSpectrumBase::shouldUseVisualWorker() const {
	return effectiveVisualWorkerMode() != Bifurx::VISUAL_WORKER_OFF;
}

int BifurxSpectrumBase::effectiveVisualWorkerMode() const {
	if (!module) {
		return Bifurx::VISUAL_WORKER_OFF;
	}
	int mode = module->visualWorkerMode.load(std::memory_order_relaxed);
	if (mode == Bifurx::VISUAL_WORKER_INHERIT) {
		mode = getBifurxVisualWorkerDefaultMode();
	}
	mode = clamp(mode, Bifurx::VISUAL_WORKER_OFF, Bifurx::VISUAL_WORKER_ON);
	if (mode == Bifurx::VISUAL_WORKER_OFF) {
		return Bifurx::VISUAL_WORKER_OFF;
	}
	if (mode == Bifurx::VISUAL_WORKER_ON) {
		return Bifurx::VISUAL_WORKER_ON;
	}
	// AUTO mode: keep it conservative for MVP.
	if (module->renderMode == Bifurx::RENDER_OPENGL && module->useGlShaderRenderer.load(std::memory_order_relaxed)) {
		return (state.hasOverlay || module->showModuleResponseOverlay.load(std::memory_order_relaxed))
			? Bifurx::VISUAL_WORKER_AUTO
			: Bifurx::VISUAL_WORKER_OFF;
	}
	return (module->renderMode == Bifurx::RENDER_NANOVG)
		? Bifurx::VISUAL_WORKER_AUTO
		: Bifurx::VISUAL_WORKER_OFF;
}

float BifurxSpectrumBase::workerSnapshotAgeMs() const {
	const auto& workerSnapshotCache = renderClient.snapshot();
	if (!workerSnapshotCache || workerSnapshotCache->completedAtSec <= 0.0) {
		return 0.f;
	}
	// Report age only when the rendered snapshot is behind the most recent published state.
	const bool previewBehind = renderClient.lastAppliedPreviewSeq() != state.lastPreviewSeq;
	const bool analysisBehind = renderClient.lastAppliedAnalysisSeq() != state.lastAnalysisSeq;
	if (!previewBehind && !analysisBehind) {
		return 0.f;
	}
	const double ageSec = std::max(0.0, system::getTime() - workerSnapshotCache->completedAtSec);
	return float(ageSec * 1000.0);
}

float BifurxSpectrumBase::workerQueueLatencyMs() const {
	const auto& workerSnapshotCache = renderClient.snapshot();
	if (!workerSnapshotCache || workerSnapshotCache->requestSubmittedAtSec <= 0.0 || workerSnapshotCache->completedAtSec <= 0.0) {
		return 0.f;
	}
	const double queueSec = std::max(0.0, workerSnapshotCache->completedAtSec - workerSnapshotCache->requestSubmittedAtSec);
	return float(queueSec * 1000.0);
}

bool BifurxSpectrumBase::ensureWorkerRegistration() {
	return renderClient.ensureRegistered();
}

void BifurxSpectrumBase::releaseWorkerRegistration() {
	renderClient.release();
}

bool BifurxSpectrumBase::submitWorkerCurveRequest() {
	if (!module || renderClient.displayId() == 0 || !state.hasPreview) {
		return true;
	}
	if (renderClient.lastSubmittedPreviewSeq() == state.lastPreviewSeq &&
		renderClient.lastSubmittedAnalysisSeq() == state.lastAnalysisSeq) {
		return true;
	}
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto submitStart = measurePerf
		? std::chrono::steady_clock::now()
		: std::chrono::steady_clock::time_point();
	BifurxUiRenderRequest request;
	request.previewSeq = state.lastPreviewSeq;
	request.analysisSeq = renderClient.lastSubmittedAnalysisSeq();
	request.requestSubmittedAtSec = system::getTime();
	request.sourcePreviewTimeSec = state.previewPublishTimeSec;
	request.previewState = state.previewState;
	// Always retain the dynamic reference so scale toggles work without new audio.
	request.fftScaleDynamic = true;
	request.showModuleResponseOverlay = module->showModuleResponseOverlay.load(std::memory_order_relaxed);
	const bool analysisChangedSinceSubmit =
		(module->analysisPublishedToken.load(std::memory_order_acquire) != 0) && (state.lastAnalysisSeq != renderClient.lastSubmittedAnalysisSeq());
	if (analysisChangedSinceSubmit) {
		std::shared_ptr<BifurxUiRenderPayload> payload = renderClient.tryAcquirePayload();
		if (payload) {
			uint32_t copiedAnalysisSeq = renderClient.lastSubmittedAnalysisSeq();
			const bool copiedAnalysis = module->copyAnalysisFrame(
				renderClient.lastSubmittedAnalysisSeq(),
				payload->analysisFrame.rawInput,
				payload->analysisFrame.output,
				&copiedAnalysisSeq
			);
			if (copiedAnalysis) {
				payload->hasOverlayTarget = state.hasOverlay;
				payload->previousDisplayTopTargetDbfs = state.dynamicTopTargetDbfs;
				std::memcpy(
					payload->previousOverlayTargetModuleDb,
					state.overlayTargetModuleDb,
					sizeof(payload->previousOverlayTargetModuleDb)
				);
				std::memcpy(
					payload->previousOverlayTargetOutputDbfs,
					state.overlayTargetOutputDbfs,
					sizeof(payload->previousOverlayTargetOutputDbfs)
				);
				request.payload = std::move(payload);
				request.analysisSeq = copiedAnalysisSeq;
			}
		}
	}
	return renderClient.submit(std::move(request), measurePerf ? &submitStart : nullptr);
}

bool BifurxSpectrumBase::adoptWorkerCurveSnapshot() {
	if (!presentationActive) {
		return false;
	}
	const auto workerSnapshotCache = renderClient.pollLatest();
	if (!workerSnapshotCache) {
		return false;
	}
	if (!workerSnapshotCache->hasCurveTarget) {
		return false;
	}
	if (std::fabs(workerSnapshotCache->previewState.sampleRate - state.previewState.sampleRate) > 0.5f) {
		return false;
	}
	// Do not reject snapshots solely for being older than the latest submitted seq.
	// Under heavy load this can cause a module to repeatedly drop usable snapshots
	// and appear permanently behind.
	// Analysis-only snapshots carry the same curve. Keep its animation and
	// layout caches instead of re-adopting identical data on every FFT frame.
	if (!state.hasCurve || workerSnapshotCache->previewSeq != state.curvePreviewSeq) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.curveHz[i] = workerSnapshotCache->curveHz[i];
			state.curveBinPos[i] = workerSnapshotCache->curveBinPos[i];
			state.curveTargetDb[i] = workerSnapshotCache->curveTargetDb[i];
		}
		state.cachedAxisSampleRate = workerSnapshotCache->cachedAxisSampleRate;
		if (!state.hasCurve) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.curveDb[i] = state.curveTargetDb[i];
			}
		}
		if (!state.hasCurve) ++state.curveRevision;
		state.displayedPreviewState = workerSnapshotCache->previewState;
		state.curvePreviewSeq = workerSnapshotCache->previewSeq;
		state.hasCurve = true;
		state.hasCurveTarget = true;
		cachedMarkerLayoutValid = false;
		refinedCurveTemplateValid = false;
	}
	lastCurvePrepUs = workerSnapshotCache->curvePrepUs;
	if (workerSnapshotCache->hasOverlayTarget &&
		(!state.hasOverlay || workerSnapshotCache->analysisSeq != renderClient.lastAppliedAnalysisSeq())) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.overlayTargetModuleDb[i] = workerSnapshotCache->overlayTargetModuleDb[i];
			state.overlayTargetOutputDbfs[i] = workerSnapshotCache->overlayTargetOutputDbfs[i];
		}
		state.dynamicTopTargetDbfs = workerSnapshotCache->displayTopTargetDbfs;
		if (!state.hasOverlay) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
				state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
			}
		}
		state.hasOverlayTarget = true;
		state.hasOverlay = true;
		lastOverlayPrepUs = workerSnapshotCache->overlayPrepUs;
		renderClient.acknowledgeAnalysis(workerSnapshotCache->analysisSeq);
		state.overlayAnalysisSeq = workerSnapshotCache->analysisSeq;
	}
	renderClient.acknowledge(workerSnapshotCache);
	return true;
}

void BifurxSpectrumBase::initializeStaticPreviewStateIfNeeded() {
	if (state.hasPreview) return;
	BifurxPreviewState preview;
	preview.sampleRate = 48000.f;
	preview.mode = kBrowserPreviewMode;
	const float previewCenterHz = kFreqMinHz * std::exp2(kFreqLog2Span * clamp(kBrowserPreviewFrequency, 0.f, 1.f));
	constexpr float previewSpanNorm = kBrowserPreviewSpan;
	preview.spanOct = 8.f * bifurx::shapedSpan(previewSpanNorm);
	preview.freqA = previewCenterHz * fastExp2(-0.5f * preview.spanOct);
	preview.freqB = previewCenterHz * fastExp2(0.5f * preview.spanOct);
	const float baseDamping = resoToDamping(kBrowserPreviewResonance);
	preview.qA = 1.f / clamp(baseDamping * fastExp(0.48f * kBrowserPreviewBalance), kSvfDampingMin, kSvfDampingMax);
	preview.qB = 1.f / clamp(baseDamping * fastExp(-0.48f * kBrowserPreviewBalance), kSvfDampingMin, kSvfDampingMax);
	preview.balance = kBrowserPreviewBalance;
	preview.balanceTarget = preview.balance;
	preview.resoNorm = kBrowserPreviewResonance;
	preview.spanParamNorm = previewSpanNorm;
	preview.spanCvNorm = 0.f;
	preview.spanAtten = 0.f;
	preview.spanNorm = previewSpanNorm;
	preview.freqParamNorm = kBrowserPreviewFrequency;
	preview.voctCv = 0.f;

	state.previewState = preview;
	state.lastPreviewSeq = 1u;
	state.cachedAxisSampleRate = 0.f;
	state.hasPreview = true;
	updateAxisCache();
	updateCurveCache();
	for (int i = 0; i < kCurvePointCount; ++i) {
		state.curveDb[i] = state.curveTargetDb[i];
	}
	state.hasCurveTarget = false;

	// The module browser has no live engine input. Recreate the authored Undertow
	// Morph scene, run it through the real filter, and feed both signals through
	// the same FFT preparation used by an instantiated module.
	SynchronousOverlayScratch& scratch = synchronousOverlayScratch();
	simulatePreviewProbeResponse(preview, scratch.fftInputTime, scratch.fftOutputTime, kFftSize);
	for (int i = 0; i < kFftSize; ++i) {
		scratch.fftInputTime[i] *= scratch.window[i];
		scratch.fftOutputTime[i] *= scratch.window[i];
	}
	scratch.fft.rfft(scratch.fftInputTime, scratch.fftRawInputFreq);
	scratch.fft.rfft(scratch.fftOutputTime, scratch.fftOutputFreq);
	prepareOverlayTargetsFromSpectra(
		preview.sampleRate,
		state.curveBinPos,
		scratch.fftOutputFreq,
		scratch.fftRawInputFreq,
		true,
		false,
		true,
		state.overlayTargetModuleDb,
		state.overlayTargetOutputDbfs,
		&state.displayTopTargetDbfs
	);
	for (int i = 0; i < kCurvePointCount; ++i) {
		state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
		state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
	}
	state.hasOverlay = true;
	state.hasOverlayTarget = false;
	state.displayTopDbfs = state.displayTopTargetDbfs;
	state.dynamicTopTargetDbfs = state.displayTopTargetDbfs;
}

void BifurxSpectrumBase::updateAxisCache() {
	if (std::fabs(state.cachedAxisSampleRate - state.previewState.sampleRate) < 0.5f) return;
	state.cachedAxisSampleRate = state.previewState.sampleRate;
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * state.cachedAxisSampleRate);
	for (int i = 0; i < kCurvePointCount; i++) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		state.curveHz[i] = hz;
		state.curveBinPos[i] = (hz * float(kFftSize)) / state.cachedAxisSampleRate;
	}
}

void BifurxSpectrumBase::updateCurveCache() {
	if (!state.hasPreview) return;
	updateAxisCache();
	state.displayedPreviewState = state.previewState;
	state.curvePreviewSeq = state.lastPreviewSeq;
	const BifurxPreviewModel& model = getOrUpdateModel();
	prepareCurveTargets(model, state.curveHz, state.curveTargetDb);
	if (!state.hasCurve) {
		for (int i = 0; i < kCurvePointCount; i++) state.curveDb[i] = state.curveTargetDb[i];
		++state.curveRevision;
	}
	state.curvePreviewSeq = state.lastPreviewSeq;
	state.hasCurve = true;
	state.hasCurveTarget = true;
}

const BifurxPreviewModel& BifurxSpectrumBase::getOrUpdateModel() const {
	if (state.curvePreviewSeq != lastModelUpdateSeq) {
		cachedModel = makePreviewModel(state.displayedPreviewState);
		const_cast<BifurxSpectrumBase*>(this)->lastModelUpdateSeq = state.curvePreviewSeq;
	}
	return cachedModel;
}

bool BifurxSpectrumBase::updateOverlayCache(uint32_t* copiedSeq) {
	if (!state.hasPreview || !module) return false;
	updateAxisCache();
	SynchronousOverlayScratch& scratch = synchronousOverlayScratch();
	uint32_t frameSeq = state.overlayAnalysisSeq;
	if (!module->copyAnalysisFrame(
		state.overlayAnalysisSeq,
		scratch.fftInputTime,
		scratch.fftOutputTime,
		&frameSeq
	)) {
		return false;
	}
	for (int i = 0; i < kFftSize; i++) {
		scratch.fftOutputTime[i] *= scratch.window[i];
	}
	scratch.fft.rfft(scratch.fftOutputTime, scratch.fftOutputFreq);
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(state.previewState.mode);
	// The measured response drives both the optional response line and the
	// normal spectrum fill's low/high color tint. Keep it alive when the line
	// is hidden; only display-only previews use an energy-only gradient.
	const bool moduleResponseEnabled = !displayOnlyMode;
	if (moduleResponseEnabled) {
		for (int i = 0; i < kFftSize; i++) {
			scratch.fftInputTime[i] *= scratch.window[i];
		}
		scratch.fft.rfft(scratch.fftInputTime, scratch.fftRawInputFreq);
	}
	prepareOverlayTargetsFromSpectra(
		state.previewState.sampleRate,
		state.curveBinPos,
		scratch.fftOutputFreq,
		scratch.fftRawInputFreq,
		moduleResponseEnabled,
		state.hasOverlay,
		true,
		state.overlayTargetModuleDb,
		state.overlayTargetOutputDbfs,
		&state.dynamicTopTargetDbfs
	);

	if (!state.hasOverlay) {
		for (int i = 0; i < kCurvePointCount; i++) {
			state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
			state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
		}
	}
	state.hasOverlayTarget = true;
	if (copiedSeq) {
		*copiedSeq = frameSeq;
	}
	return true;
}

bool BifurxSpectrumBase::updateAnimation(float dt, bool* contentChanged) {
	bool animationActive = false;
	bool changed = false;
	bool curveChanged = false;
	constexpr float kCurveEpsilonDb = 0.01f;
	constexpr float kOverlayEpsilonDb = 0.02f;
	constexpr float kTopEpsilonDbfs = 0.02f;

	if (state.hasCurveTarget) {
		const float curveMaxStepDb = std::max(0.25f, kCurveVisualSlewDbPerSec * dt);
		float maxCurveResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = state.curveDb[i];
			curveChanged |= prev != state.curveTargetDb[i];
			float delta = state.curveTargetDb[i] - prev;
			delta = clamp(delta, -curveMaxStepDb, curveMaxStepDb);
			state.curveDb[i] = prev + delta;
			maxCurveResidualDb = std::max(maxCurveResidualDb, std::fabs(state.curveTargetDb[i] - state.curveDb[i]));
		}
		if (maxCurveResidualDb <= kCurveEpsilonDb) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.curveDb[i] = state.curveTargetDb[i];
			}
			state.hasCurveTarget = false;
		}
		else {
			animationActive = true;
		}
	}

	if (curveChanged) ++state.curveRevision;
	changed |= curveChanged;
	if (state.hasOverlayTarget) {
		const float overlayDbSmoothing = 1.f - std::pow(0.78f, std::max(0.f, dt) * 60.f);
		const float overlayLevelSmoothing = 1.f - std::pow(0.80f, std::max(0.f, dt) * 60.f);
		float maxOverlayResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			changed |= state.overlayModuleDb[i] != state.overlayTargetModuleDb[i]
				|| state.overlayOutputDbfs[i] != state.overlayTargetOutputDbfs[i];
			state.overlayModuleDb[i] = mixf(state.overlayModuleDb[i], state.overlayTargetModuleDb[i], overlayDbSmoothing);
			state.overlayOutputDbfs[i] = mixf(state.overlayOutputDbfs[i], state.overlayTargetOutputDbfs[i], overlayLevelSmoothing);
			const float moduleResidual = std::fabs(state.overlayTargetModuleDb[i] - state.overlayModuleDb[i]);
			const float outputResidual = std::fabs(state.overlayTargetOutputDbfs[i] - state.overlayOutputDbfs[i]);
			maxOverlayResidualDb = std::max(maxOverlayResidualDb, std::max(moduleResidual, outputResidual));
		}

		const float prevTop = state.displayTopDbfs;
		changed |= prevTop != state.displayTopTargetDbfs;
		float topSmoothing = (state.displayTopTargetDbfs > prevTop) ? 0.22f : 0.10f;
		if (module && module->fftScaleDynamic.load(std::memory_order_relaxed) && state.displayTopTargetDbfs > prevTop) {
			topSmoothing = 0.70f;
		}
		state.displayTopDbfs = mixf(prevTop, state.displayTopTargetDbfs, 1.f - std::pow(1.f - topSmoothing, std::max(0.f, dt) * 60.f));
		const float topResidualDbfs = std::fabs(state.displayTopTargetDbfs - state.displayTopDbfs);

		if (maxOverlayResidualDb <= kOverlayEpsilonDb && topResidualDbfs <= kTopEpsilonDbfs) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
				state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
			}
			state.displayTopDbfs = state.displayTopTargetDbfs;
			state.hasOverlayTarget = false;
		}
		else {
			animationActive = true;
		}
	}

	if (contentChanged) *contentChanged = changed;
	return animationActive;
}

BifurxRenderTickResult BifurxSpectrumBase::runRenderTick(float dt) {
	BifurxRenderTickResult result;
	if (module && !presentationActive) {
		releaseWorkerRegistration();
		return result;
	}
	const uint32_t prevPreviewSeq = state.lastPreviewSeq;
	const uint32_t prevAnalysisSeq = state.lastAnalysisSeq;
	const uint32_t prevOverlaySeq = state.overlayAnalysisSeq;
	const uint64_t prevCurveRevision = state.curveRevision;

	syncBase();
	const bool workerAdopted = adoptWorkerCurveSnapshot();
	result.previewUpdated = (state.lastPreviewSeq != prevPreviewSeq) || workerAdopted;
	result.analysisUpdated = (state.lastAnalysisSeq != prevAnalysisSeq) || state.overlayAnalysisSeq != prevOverlaySeq;
	const bool dynamic = module ? module->fftScaleDynamic.load(std::memory_order_relaxed) : true;
	const float colorShape = module ? module->params[Bifurx::FM_AMT_PARAM].getValue() : 0.f;
	const bool shapeChanged = colorShape != state.colorShape;
	state.colorShape = colorShape;
	const bool scaleChanged = dynamic != state.fftScaleDynamic;
	state.fftScaleDynamic = dynamic;
	state.displayTopTargetDbfs = dynamic ? state.dynamicTopTargetDbfs : kDisplayTopDbfsCeiling;
	if (scaleChanged && !dynamic) state.displayTopDbfs = kDisplayTopDbfsCeiling;
	if (state.hasOverlay && state.displayTopDbfs != state.displayTopTargetDbfs) state.hasOverlayTarget = true;
	bool animationChanged = false;
	result.animationActive = updateAnimation(dt, &animationChanged);
	result.contentChanged = result.previewUpdated || result.analysisUpdated || workerAdopted || scaleChanged || shapeChanged || animationChanged
		|| state.curveRevision != prevCurveRevision;
	result.curvePrepUs = (result.previewUpdated || workerAdopted) ? lastCurvePrepUs : 0.f;
	result.overlayPrepUs = result.analysisUpdated ? lastOverlayPrepUs : 0.f;
	return result;
}

void BifurxSpectrumBase::calculateMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const {
	if (!layout) return;
	const float padY = std::max(4.f, h * 0.035f);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.displayedPreviewState.sampleRate);
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
	const BifurxPreviewModel& model = getOrUpdateModel();

	layout->anchorToBottomLane = markerPinnedToBottomLane(0) || markerPinnedToBottomLane(1);
	layout->markers[0].visible = false; layout->markers[1].visible = false;

	auto populateMarker = [&](int mIdx, float targetHz) {
		auto& m = layout->markers[mIdx];
		const auto anchor = displayAnchorForMarker(mIdx, targetHz, minHz, maxHz);
		const float mX = w * anchor.x01;
		if (mX < markerOuterRadius + kPeakMarkerEdgePadding || mX > w - markerOuterRadius - kPeakMarkerEdgePadding) {
			return;
		}
		m.x = mX;
		m.yCurve = curveYAtX01(anchor.x01, spectrumBottomY, spectrumTopY);
		const float mMinY = spectrumTopY + markerOuterRadius + kPeakMarkerEdgePadding, mMaxY = spectrumBottomY - markerOuterRadius - kPeakMarkerEdgePadding;
		const bool allowBottomCurveMarker = state.displayedPreviewState.mode == 0 || state.displayedPreviewState.mode == 9;
		m.yMarker = markerPinnedToBottomLane(mIdx)
			? markerBottomLaneY
			: (allowBottomCurveMarker && m.yCurve > mMaxY)
				? std::min(m.yCurve, spectrumBottomY)
				: clamp(m.yCurve, mMinY, mMaxY);
		m.hz = std::max(anchor.hz, 1e-6f);
		m.visible = true;
		formatFrequencyLabel(m.hz, m.label, sizeof(m.label));
	};

	populateMarker(0, model.markerFreqA);
	populateMarker(1, model.markerFreqB);

	layout->labelX[0] = layout->markers[0].x;
	layout->labelX[1] = layout->markers[1].x;
	const float labelMargin = std::max(18.f, w * 0.08f), minLabelSeparation = std::max(30.f, w * 0.18f), minX = labelMargin, maxX = w - labelMargin;
	if (layout->markers[0].visible && layout->markers[1].visible) {
		const int leftIndex = (layout->labelX[0] <= layout->labelX[1]) ? 0 : 1, rightIndex = 1 - leftIndex;
		float leftX = clamp(layout->labelX[leftIndex], minX, maxX), rightX = clamp(layout->labelX[rightIndex], minX, maxX), needed = std::min(minLabelSeparation, std::max(0.f, maxX - minX)) - (rightX - leftX);
		if (needed > 0.f) {
			float moveLeft = std::min(0.5f * needed, leftX - minX), moveRight = std::min(0.5f * needed, maxX - rightX);
			leftX -= moveLeft; rightX += moveRight; needed -= (moveLeft + moveRight);
			if (needed > 0.f) { float extraLeft = std::min(needed, leftX - minX); leftX -= extraLeft; needed -= extraLeft; }
			if (needed > 0.f) rightX += std::min(needed, maxX - rightX);
		}
		layout->labelX[leftIndex] = leftX; layout->labelX[rightIndex] = rightX;
	} else {
		for (int i = 0; i < 2; ++i) if (layout->markers[i].visible) layout->labelX[i] = clamp(layout->labelX[i], minX, maxX);
	}

	layout->labelFontSize = std::max(7.f, h * 0.055f);
	layout->labelY = labelBandTop + 0.5f * labelBandHeight;
	layout->guideYBottom = clamp(labelBandTop + std::min(2.1f, 0.18f * labelBandHeight), labelBandTop + 0.2f, layout->labelY - 0.5f * layout->labelFontSize - 0.6f);
}

void BifurxSpectrumBase::getCachedMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const {
	if (!layout) return;
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * state.displayedPreviewState.sampleRate);
	const BifurxPreviewModel& model = getOrUpdateModel();
	const DisplayAnchor anchors[2] = {
		displayAnchorForMarker(0, model.markerFreqA, minHz, maxHz),
		displayAnchorForMarker(1, model.markerFreqB, minHz, maxHz)
	};
	const bool markerPinned[2] = {
		markerPinnedToBottomLane(0),
		markerPinnedToBottomLane(1)
	};

	bool rebuild = !cachedMarkerLayoutValid;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutW - w) > 1e-4f;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutH - h) > 1e-4f;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutSampleRate - state.displayedPreviewState.sampleRate) > 0.5f;
	rebuild = rebuild || cachedMarkerLayoutPreviewSeq != state.curvePreviewSeq;
	rebuild = rebuild || cachedMarkerLayoutCurveRevision != state.curveRevision;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutAnchorX01[0] - anchors[0].x01) > 1e-7f;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutAnchorX01[1] - anchors[1].x01) > 1e-7f;
	rebuild = rebuild || cachedMarkerLayoutMarkerPinned[0] != markerPinned[0];
	rebuild = rebuild || cachedMarkerLayoutMarkerPinned[1] != markerPinned[1];

	if (rebuild) {
		calculateMarkerLayout(&cachedMarkerLayout, w, h);
		cachedMarkerLayoutW = w;
		cachedMarkerLayoutH = h;
		cachedMarkerLayoutSampleRate = state.displayedPreviewState.sampleRate;
		cachedMarkerLayoutPreviewSeq = state.curvePreviewSeq;
		cachedMarkerLayoutCurveRevision = state.curveRevision;
		cachedMarkerLayoutAnchorX01[0] = anchors[0].x01;
		cachedMarkerLayoutAnchorX01[1] = anchors[1].x01;
		cachedMarkerLayoutMarkerPinned[0] = markerPinned[0];
		cachedMarkerLayoutMarkerPinned[1] = markerPinned[1];
		cachedMarkerLayoutValid = true;
	}

	*layout = cachedMarkerLayout;
}

void BifurxSpectrumBase::calculateRefinedCurvePoints(std::vector<BifurxCurvePoint>* points, float w, float h) const {
	if (!points) return;
	const size_t refinedPointReserve = size_t(kCurvePointCount) + 6;
	const float padY = std::max(4.f, h * 0.035f);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.displayedPreviewState.sampleRate);
	const BifurxPreviewModel& model = getOrUpdateModel();
	const DisplayAnchor anchors[2] = {
		displayAnchorForMarker(0, model.markerFreqA, minHz, maxHz),
		displayAnchorForMarker(1, model.markerFreqB, minHz, maxHz)
	};
	const bool markerPinned[2] = {
		markerPinnedToBottomLane(0) && model.markerFreqA >= minHz && model.markerFreqA <= maxHz,
		markerPinnedToBottomLane(1) && model.markerFreqB >= minHz && model.markerFreqB <= maxHz
	};

	bool rebuildTemplate = !refinedCurveTemplateValid;
	rebuildTemplate = rebuildTemplate || std::fabs(w - refinedCurveTemplateW) > 1e-4f;
	rebuildTemplate = rebuildTemplate || std::fabs(h - refinedCurveTemplateH) > 1e-4f;
	rebuildTemplate = rebuildTemplate || std::fabs(state.displayedPreviewState.sampleRate - refinedCurveTemplateSampleRate) > 0.5f;
	rebuildTemplate = rebuildTemplate || std::fabs(anchors[0].x01 - refinedCurveTemplateAnchorX01[0]) > 1e-7f;
	rebuildTemplate = rebuildTemplate || std::fabs(anchors[1].x01 - refinedCurveTemplateAnchorX01[1]) > 1e-7f;
	rebuildTemplate = rebuildTemplate || markerPinned[0] != refinedCurveTemplateMarkerPinned[0];
	rebuildTemplate = rebuildTemplate || markerPinned[1] != refinedCurveTemplateMarkerPinned[1];

	if (rebuildTemplate) {
		refinedCurveTemplate.clear();
		if (refinedCurveTemplate.capacity() < refinedPointReserve) {
			refinedCurveTemplate.reserve(refinedPointReserve);
		}

		// Initial grid points
		for (int i = 0; i < kCurvePointCount; ++i) {
			refinedCurveTemplate.push_back({float(i) / float(kCurvePointCount - 1), 0, i});
		}

		auto addRefinement = [&](const DisplayAnchor& anchor, bool notch) {
			const float dx = 0.35f / float(kCurvePointCount - 1);
			refinedCurveTemplate.push_back({clamp(anchor.x01 - dx, 0.f, 1.f), 1, -1});
			refinedCurveTemplate.push_back({clamp(anchor.x01, 0.f, 1.f), notch ? 3 : 2, -1});
			refinedCurveTemplate.push_back({clamp(anchor.x01 + dx, 0.f, 1.f), 1, -1});
		};

		addRefinement(anchors[0], markerPinned[0]);
		addRefinement(anchors[1], markerPinned[1]);

		std::sort(refinedCurveTemplate.begin(), refinedCurveTemplate.end(), [](const RefinedCurveTemplatePoint& a, const RefinedCurveTemplatePoint& b) {
			if (std::fabs(a.x01 - b.x01) > 1e-7f) return a.x01 < b.x01;
			return a.priority > b.priority;
		});
		refinedCurveTemplate.erase(std::unique(refinedCurveTemplate.begin(), refinedCurveTemplate.end(), [](const RefinedCurveTemplatePoint& a, const RefinedCurveTemplatePoint& b) {
			return std::fabs(a.x01 - b.x01) < 1e-7f;
		}), refinedCurveTemplate.end());

		refinedCurveTemplateW = w;
		refinedCurveTemplateH = h;
		refinedCurveTemplateSampleRate = state.displayedPreviewState.sampleRate;
		refinedCurveTemplateAnchorX01[0] = anchors[0].x01;
		refinedCurveTemplateAnchorX01[1] = anchors[1].x01;
		refinedCurveTemplateMarkerPinned[0] = markerPinned[0];
		refinedCurveTemplateMarkerPinned[1] = markerPinned[1];
		refinedCurveTemplateValid = true;
	}

	points->clear();
	if (points->capacity() < refinedCurveTemplate.size()) {
		points->reserve(refinedCurveTemplate.size());
	}

	// Notch nulls fall between the sampled bins. Preserve their exact center
	// at the floor instead of interpolating a shallow minimum across the null.
	// Surviving regular grid points evaluate directly from state.curveDb without interpolation.
	// Non-grid refinement points continue to interpolate along the animated curve.
	auto responseYForDb = [&](float db) { return responseYForDbDisplay(db, kResponseMinDb, kResponseMaxDb, spectrumBottomY, spectrumTopY); };
	for (const auto& pt : refinedCurveTemplate) {
		float y = 0.f;
		if (pt.priority == 3) {
			y = spectrumBottomY;
		} else if (pt.gridIndex >= 0) {
			y = responseYForDb(state.curveDb[pt.gridIndex]);
		} else {
			y = curveYAtX01(pt.x01, spectrumBottomY, spectrumTopY);
		}
		points->push_back({pt.x01, y, pt.priority});
	}
}

} // namespace bifurx
