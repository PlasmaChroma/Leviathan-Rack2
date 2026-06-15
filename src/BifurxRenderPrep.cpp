#include "BifurxRenderPrep.hpp"

#include <chrono>

namespace bifurx {

namespace {

struct WorkerOverlayScratch {
	dsp::RealFFT fft;
	alignas(16) float window[kFftSize];
	alignas(16) float fftInputTime[kFftSize];
	alignas(16) float fftOutputTime[kFftSize];
	alignas(16) float fftOutputFreq[2 * kFftSize];
	alignas(16) float fftResponseOutputFreq[2 * kFftSize];
	alignas(16) float fftRawInputFreq[2 * kFftSize];

	WorkerOverlayScratch() : fft(kFftSize) {
		for (int i = 0; i < kFftSize; ++i) {
			window[i] = 0.5f - 0.5f * std::cos(2.f * kPi * float(i) / float(kFftSize - 1));
		}
	}
};

void fillAxisForSampleRate(float sampleRate, float* curveHz, float* curveBinPos, float* cachedAxisSampleRate) {
	const float safeSampleRate = std::max(1000.f, sampleRate);
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
	bool fftScaleDynamic
) {
	if (!fftScaleDynamic) {
		return kDisplayTopDbfsCeiling;
	}

	float framePeakDbfs = kOverlayDbfsFloor;
	for (int i = 0; i < kCurvePointCount; ++i) {
		framePeakDbfs = std::max(framePeakDbfs, overlayTargetOutputDbfs[i]);
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

void prepareOverlayTargetsFromSpectra(
	float sampleRate,
	const float* curveBinPos,
	const float* fftOutputFreq,
	const float* fftResponseOutputFreq,
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
	float binResponseOutputPower[kFftBinCount];
	float binRawInputPower[kFftBinCount];
	float binModuleDeltaDb[kFftBinCount];
	const float amplitudeScale = 4.f / float(kFftSize);
	const float amplitudeScaleSq = amplitudeScale * amplitudeScale;
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		const float binHz = (float(bin) * sampleRate) / float(kFftSize);
		const float subsonicWeight = levi_math::clamp01((binHz - kOverlaySubsonicCutHz) / (kOverlaySubsonicFadeHz - kOverlaySubsonicCutHz));
		const float weightedPowerScale = subsonicWeight * subsonicWeight * amplitudeScaleSq;
		binOutputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftOutputFreq, bin);
		if (moduleResponseEnabled) {
			binResponseOutputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftResponseOutputFreq, bin);
			binRawInputPower[bin] = weightedPowerScale * orderedSpectrumPower(fftRawInputFreq, bin);
		}
	}

	constexpr int kOverlayBandRadius = 2;
	constexpr float kOverlayBandKernel[5] = {0.08f, 0.24f, 0.36f, 0.24f, 0.08f};
	for (int bin = 0; bin < kFftBinCount; ++bin) {
		float outputEnergy = 0.f;
		float responseOutputEnergy = 0.f;
		float rawInputEnergy = 0.f;
		for (int k = -kOverlayBandRadius; k <= kOverlayBandRadius; ++k) {
			const int sampleBin = clamp(bin + k, 0, kFftBinCount - 1);
			const float w = kOverlayBandKernel[k + kOverlayBandRadius];
			outputEnergy += w * binOutputPower[sampleBin];
			if (moduleResponseEnabled) {
				responseOutputEnergy += w * binResponseOutputPower[sampleBin];
				rawInputEnergy += w * binRawInputPower[sampleBin];
			}
		}
		binModuleDeltaDb[bin] = moduleResponseEnabled
			? softLimitOverlayDeltaDb(10.f * std::log10((responseOutputEnergy + 1e-12f) / (rawInputEnergy + 1e-12f)))
			: 0.f;
		outputEnergy += 1e-12f;
		binOutputDbfs[bin] = clamp(10.f * std::log10(outputEnergy / 25.f + 1e-12f), kOverlayDbfsFloor, kOverlayDbfsCeiling);
	}

	float sampledOutputDbfs[kCurvePointCount];
	float sampledModuleDeltaDb[kCurvePointCount];
	for (int i = 0; i < kCurvePointCount; ++i) {
		const float binPos = curveBinPos[i];
		const int binA = std::max(2, int(std::floor(binPos)));
		const int binB = std::min(binA + 1, kFftSize / 2);
		const float frac = binPos - float(binA);
		sampledOutputDbfs[i] = mixf(binOutputDbfs[binA], binOutputDbfs[binB], frac);
		sampledModuleDeltaDb[i] = mixf(binModuleDeltaDb[binA], binModuleDeltaDb[binB], frac);
	}

	float frameSmoothedOutputDbfs[kCurvePointCount];
	const float targetSmoothing = hasOverlayTarget ? 0.45f : 1.f;
	for (int i = 0; i < kCurvePointCount; ++i) {
		const int left = std::max(0, i - 1);
		const int right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothOutputDbfs =
			0.12f * sampledOutputDbfs[left] +
			0.76f * sampledOutputDbfs[i] +
			0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		const float smoothModuleDeltaDb =
			0.12f * sampledModuleDeltaDb[left] +
			0.76f * sampledModuleDeltaDb[i] +
			0.12f * sampledModuleDeltaDb[right];
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
	}

	*displayTopTargetDbfs = computeDisplayTopTargetDbfs(frameSmoothedOutputDbfs, overlayTargetOutputDbfs, fftScaleDynamic);
}

} // namespace

void prepareCurveSnapshot(const BifurxUiRenderRequest& request, BifurxUiRenderSnapshot* snapshot) {
	if (!snapshot) {
		return;
	}
	const auto prepStart = std::chrono::steady_clock::now();
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
		snapshot->curvePrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now() - prepStart).count());
	}
	else {
		snapshot->curvePrepUs = 0.f;
	}

	if (!request.hasAnalysisFrame) {
		return;
	}

	const auto overlayPrepStart = std::chrono::steady_clock::now();
	thread_local WorkerOverlayScratch scratch;
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(request.previewState.mode);
	for (int i = 0; i < kFftSize; ++i) {
		scratch.fftOutputTime[i] = request.analysisOutput[i] * scratch.window[i];
	}
	scratch.fft.rfft(scratch.fftOutputTime, scratch.fftOutputFreq);
	if (!displayOnlyMode) {
		for (int i = 0; i < kFftSize; ++i) {
			scratch.fftOutputTime[i] = request.analysisResponseOutput[i] * scratch.window[i];
		}
		scratch.fft.rfft(scratch.fftOutputTime, scratch.fftResponseOutputFreq);
		for (int i = 0; i < kFftSize; ++i) {
			scratch.fftInputTime[i] = request.analysisRawInput[i] * scratch.window[i];
		}
		scratch.fft.rfft(scratch.fftInputTime, scratch.fftRawInputFreq);
	}

	for (int i = 0; i < kCurvePointCount; ++i) {
		snapshot->overlayTargetModuleDb[i] = request.previousOverlayTargetModuleDb[i];
		snapshot->overlayTargetOutputDbfs[i] = request.previousOverlayTargetOutputDbfs[i];
	}
	prepareOverlayTargetsFromSpectra(
		request.previewState.sampleRate,
		snapshot->curveBinPos,
		scratch.fftOutputFreq,
		scratch.fftResponseOutputFreq,
		scratch.fftRawInputFreq,
		!displayOnlyMode,
		request.hasOverlayTarget,
		request.fftScaleDynamic,
		snapshot->overlayTargetModuleDb,
		snapshot->overlayTargetOutputDbfs,
		&snapshot->displayTopTargetDbfs
	);
	snapshot->hasOverlayTarget = true;
	snapshot->overlayPrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::steady_clock::now() - overlayPrepStart).count());
}

} // namespace bifurx
