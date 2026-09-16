#include "BifurxRenderPrep.hpp"

#include <chrono>

namespace bifurx {

namespace {

struct WorkerOverlayScratch {
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize] {};
	alignas(16) float fftOutputTime[kFftSize] {};
	alignas(16) float fftOutputFreq[2 * kFftSize] {};
	alignas(16) float fftRawInputFreq[2 * kFftSize] {};

	WorkerOverlayScratch() : fft(kFftSize) {
		for (int i = 0; i < kFftSize; ++i) {
			window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
		}
	}
};

void fillAxisForSampleRate(float sampleRate, float* curveHz, float* curveBinPos, float* cachedAxisSampleRate) {
	const float safeSampleRate = std::max(1000.f, sampleRate);
	if (*cachedAxisSampleRate == safeSampleRate) return;
	*cachedAxisSampleRate = safeSampleRate;
	const float minHz = 10.f;
	const float maxHz = std::min(20000.f, 0.46f * safeSampleRate);
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float x01 = float(i) / float(kCurvePointCount - 1);
		const float hz = logFrequencyAt(x01, minHz, maxHz);
		curveHz[i] = hz;
		curveBinPos[i] = (hz * float(kFftSize)) / safeSampleRate;
	}
}


float computeDisplayTopTargetDbfs(
	const float* frameSmoothedOutputDbfs,
	const float* overlayTargetOutputDbfs,
	bool fftScaleDynamic,
	float previousTopTargetDbfs = kDisplayTopDbfsCeiling
) {
	if (!fftScaleDynamic) {
		return kDisplayTopDbfsCeiling;
	}

	float framePeakDbfs = kOverlayDbfsFloor;
	for (int i = 0; i < kCurvePointCount; ++i) {
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
	}

	if (previousTopTargetDbfs > kDisplayTopDbfsFloor && std::fabs(framePeakDbfs - (previousTopTargetDbfs - 6.f)) < 0.5f) {
		return previousTopTargetDbfs;
	}

	float sortedOutputDbfs[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		sortedOutputDbfs[i] = frameSmoothedOutputDbfs[i];
	}
	const int p95Index = int(0.95f * float(kCurvePointCount - 1));
	std::nth_element(sortedOutputDbfs, sortedOutputDbfs + p95Index, sortedOutputDbfs + kCurvePointCount);
	const float robustTopRefDbfs = std::max(sortedOutputDbfs[p95Index], framePeakDbfs - 18.f);
	return clamp(
		std::max(robustTopRefDbfs + 6.f, framePeakDbfs + kDisplayPeakHeadroomDb),
		kDisplayTopDbfsFloor,
		kDisplayTopDynamicCeilingDbfs
	);
}

} // namespace

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
) {
	float binOutputDbfs[kFftBinCount];
	float binOutputPower[kFftBinCount];
	float binRawInputPower[kFftBinCount];
	float binModuleDeltaDb[kFftBinCount];
	// Tone-equivalent peak power: Hann coherent gain and ENBW (1.5 bins).
	// Sum the main lobe, rather than averaging it with a frequency-dependent loss.
	const float amplitudeScaleSq = 16.f / (1.5f * float(kFftSize) * float(kFftSize - 1));
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float binHz = (float(bin) * sampleRate) / float(kFftSize);
		const float subsonicWeight = levi_math::clamp01((binHz - kOverlaySubsonicCutHz) / (kOverlaySubsonicFadeHz - kOverlaySubsonicCutHz));
		const float weightedPowerScale = subsonicWeight * subsonicWeight * amplitudeScaleSq;
		binOutputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftOutputFreq, bin);
		if (moduleResponseEnabled) {
			binRawInputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftRawInputFreq, bin);
		}
	}

	constexpr int kOverlayBandRadius = 2;

	for (int bin = 0; bin < kFftBinCount; ++bin) {
		float outputEnergy = 0.f;
		float responseOutputEnergy = 0.f;
		float rawInputEnergy = 0.f;
		for (int k = -kOverlayBandRadius; k <= kOverlayBandRadius; ++k) {
			const int sampleBin = bin + k;
			if (sampleBin < 0 || sampleBin >= kFftBinCount) continue;
			const float w = 1.f;
			outputEnergy += w * binOutputPower[sampleBin];
			if (moduleResponseEnabled) {
				responseOutputEnergy += w * binOutputPower[sampleBin];
				rawInputEnergy += w * binRawInputPower[sampleBin];
			}
		}
		binModuleDeltaDb[bin] = moduleResponseEnabled
			? (rawInputEnergy / (rawInputEnergy + 2.5e-7f)) * 10.f * std::log10((responseOutputEnergy + 1e-12f) / (rawInputEnergy + 1e-12f))
			: 0.f;
		outputEnergy += 1e-12f;
		binOutputDbfs[bin] = clamp(10.f * std::log10(outputEnergy * 0.04f + 1e-12f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}

	float sampledOutputDbfs[kCurvePointCount];
	float sampledModuleDeltaDb[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float binPos = clamp(curveBinPos[i], 0.f, float(kFftBinCount - 1));
		const int binA = int(std::floor(binPos));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPos - float(binA);
		// Max-hold within the plotted log-frequency cell preserves narrow tones
		// between sparse high-frequency points. Units are tone peak dBFS, not PSD.
		const int leftBin = clamp(int(std::ceil(i ? 0.5f * (curveBinPos[i-1] + binPos) : binPos)), 0, kFftBinCount - 1);
		const int rightBin = clamp(int(std::floor(i + 1 < kCurvePointCount ? 0.5f * (curveBinPos[i+1] + binPos) : binPos)), 0, kFftBinCount - 1);
		sampledOutputDbfs[i] = std::max(binOutputDbfs[binA], binOutputDbfs[binB]);
		for (int bin = leftBin; bin <= rightBin; ++bin) sampledOutputDbfs[i] = std::max(sampledOutputDbfs[i], binOutputDbfs[bin]);
		sampledModuleDeltaDb[i] = mixf(binModuleDeltaDb[binA], binModuleDeltaDb[binB], frac);
	}

	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; ++i) {
		const int left = std::max(0, i - 1);
		const int right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothOutputDbfs =
			sampledOutputDbfs[i];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		const float smoothModuleDeltaDb =
			0.12f * sampledModuleDeltaDb[left] +
			0.76f * sampledModuleDeltaDb[i] +
			0.12f * sampledModuleDeltaDb[right];
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
	}

	*displayTopTargetDbfs = computeDisplayTopTargetDbfs(frameSmoothedOutputDbfs, overlayTargetOutputDbfs, fftScaleDynamic, *displayTopTargetDbfs);
}

void prepareCurveSnapshot(const BifurxUiRenderRequest& request, BifurxUiRenderSnapshot* snapshot) {
	if (!snapshot) {
		return;
	}
	const bool measurePrep = isDragonKingDebugEnabled();
	const auto prepStart = measurePrep ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
	snapshot->previewState = request.previewState;
	snapshot->displayId = request.displayId;
	snapshot->requestSeq = request.requestSeq;
	snapshot->previewSeq = request.previewSeq;
	snapshot->analysisSeq = request.analysisSeq;
	snapshot->requestSubmittedAtSec = request.requestSubmittedAtSec;
	snapshot->sourcePreviewTimeSec = request.sourcePreviewTimeSec;
	if (!(request.skipCurvePrep && snapshot->hasCurveTarget)) {
		fillAxisForSampleRate(
			request.previewState.sampleRate,
			snapshot->curveHz,
			snapshot->curveBinPos,
			&snapshot->cachedAxisSampleRate
		);

		if (isBifurxDisplayOnlyMode(request.previewState.mode)) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				snapshot->curveTargetDb[i] = 0.f;
			}
		}
		else {
			const BifurxPreviewModel model = makePreviewModel(request.previewState);
			for (int i = 0; i < kCurvePointCount; ++i) {
				const float db = previewModelResponseDb(model, snapshot->curveHz[i]);
				snapshot->curveTargetDb[i] = clamp(db, kResponseMinDb, kResponseMaxDb);
			}
		}
		snapshot->hasCurveTarget = true;
		snapshot->curvePrepUs = measurePrep ? float(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - prepStart).count()) : 0.f;
	}
	else {
		snapshot->curvePrepUs = 0.f;
	}

	if (!request.payload) {
		return;
	}
	const BifurxUiRenderPayload& payload = *request.payload;
	const BifurxAnalysisFrame& analysisFrame = payload.analysisFrame;

	const auto overlayPrepStart = measurePrep ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
	thread_local WorkerOverlayScratch scratch;
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(request.previewState.mode);
	// Normal module rendering uses measured response for the FFT fill colors
	// even when the response line itself is hidden. Browser/display-only views
	// are the only path whose gradient is independent of the input spectrum.
	const bool moduleResponseEnabled = !displayOnlyMode;
	for (int i = 0; i < kFftSize; ++i) {
		scratch.fftOutputTime[i] = analysisFrame.output[i] * scratch.window[i];
	}
	scratch.fft.rfft(scratch.fftOutputTime, scratch.fftOutputFreq);
	if (moduleResponseEnabled) {
		for (int i = 0; i < kFftSize; ++i) {
			scratch.fftInputTime[i] = analysisFrame.rawInput[i] * scratch.window[i];
		}
		scratch.fft.rfft(scratch.fftInputTime, scratch.fftRawInputFreq);
	}

	for (int i = 0; i < kCurvePointCount; ++i) {
		snapshot->overlayTargetModuleDb[i] = payload.previousOverlayTargetModuleDb[i];
		snapshot->overlayTargetOutputDbfs[i] = payload.previousOverlayTargetOutputDbfs[i];
	}
	snapshot->displayTopTargetDbfs = payload.previousDisplayTopTargetDbfs;
	prepareOverlayTargetsFromSpectra(
		request.previewState.sampleRate,
		snapshot->curveBinPos,
		scratch.fftOutputFreq,
		scratch.fftRawInputFreq,
		moduleResponseEnabled,
		payload.hasOverlayTarget,
		request.fftScaleDynamic,
		snapshot->overlayTargetModuleDb,
		snapshot->overlayTargetOutputDbfs,
		&snapshot->displayTopTargetDbfs
	);
	snapshot->hasOverlayTarget = true;
	snapshot->overlayPrepUs = measurePrep ? float(std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - overlayPrepStart).count()) : 0.f;
}

} // namespace bifurx
