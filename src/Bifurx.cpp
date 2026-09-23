#include "BifurxModule.hpp"
#include "BifurxPreview.hpp"
#include "BifurxLicense.hpp"

namespace bifurx {

static_assert((kFftSize & (kFftSize - 1)) == 0, "kFftSize must be a power of two for bitmask ring indexing.");

namespace {

constexpr uint64_t kPublishedSlotIndexMask = 0x7u;
constexpr uint32_t kPublishedSlotWriterClaim = 0x80000000u;
constexpr int kSpanShapeLutIntervals = 1024;

struct SpanShapeLut {
	float values[kSpanShapeLutIntervals + 1] = {};

	SpanShapeLut() {
		for (int i = 0; i <= kSpanShapeLutIntervals; ++i) {
			const float x = float(i) / float(kSpanShapeLutIntervals);
			values[i] = std::pow(x, 1.45f);
		}
	}
};

const SpanShapeLut gSpanShapeLut;

inline int publishedSlotFromToken(uint64_t token) {
	return token ? int(token & kPublishedSlotIndexMask) : -1;
}

inline uint64_t makePublishedSlotToken(uint64_t generation, int slot) {
	return (generation << 3u) | uint64_t(slot);
}

template <int SlotCount, typename CopyFn>
bool readPublishedSlot(
	std::atomic<uint64_t>& publishedToken,
	std::atomic<uint32_t> (&readerCounts)[SlotCount],
	CopyFn&& copyFn
) {
	for (int attempt = 0; attempt < SlotCount + 2; ++attempt) {
		const uint64_t tokenBefore = publishedToken.load(std::memory_order_acquire);
		const int slot = publishedSlotFromToken(tokenBefore);
		if (slot < 0 || slot >= SlotCount) {
			return false;
		}
		uint32_t claimState = readerCounts[slot].load(std::memory_order_acquire);
		bool claimed = false;
		while ((claimState & kPublishedSlotWriterClaim) == 0u) {
			if (readerCounts[slot].compare_exchange_weak(
				claimState,
				claimState + 1u,
				std::memory_order_acq_rel,
				std::memory_order_acquire
			)) {
				claimed = true;
				break;
			}
		}
		if (!claimed) {
			continue;
		}
		const uint64_t tokenAfter = publishedToken.load(std::memory_order_acquire);
		if (tokenAfter == tokenBefore) {
			copyFn(slot);
			readerCounts[slot].fetch_sub(1u, std::memory_order_release);
			return true;
		}
		readerCounts[slot].fetch_sub(1u, std::memory_order_release);
	}
	return false;
}

template <int SlotCount>
int findWritableSlot(
	const std::atomic<uint64_t>& publishedToken,
	std::atomic<uint32_t> (&readerCounts)[SlotCount],
	int excludedSlotA = -1,
	int excludedSlotB = -1
) {
	const int publishedSlot = publishedSlotFromToken(publishedToken.load(std::memory_order_relaxed));
	for (int slot = 0; slot < SlotCount; ++slot) {
		if (slot == publishedSlot || slot == excludedSlotA || slot == excludedSlotB) {
			continue;
		}
		uint32_t expected = 0u;
		if (readerCounts[slot].compare_exchange_strong(
			expected,
			kPublishedSlotWriterClaim,
			std::memory_order_acq_rel,
			std::memory_order_acquire
		)) {
			return slot;
		}
	}
	return -1;
}

} // namespace

const char* const kBifurxModeLabels[kBifurxModeCount] = {
	"Low + Low",
	"Low + Band",
	"Notch + Low",
	"Notch + Notch",
	"Low + High",
	"Band + Band",
	"High + Low",
	"High + Notch",
	"Band + High",
	"High + High",
	"Display Only"
};

std::string bifurxUserRootPath() {
	return system::join(leviathanPluginUserRootPath(), "Bifurx");
}

float shapedSpan(float value) {
	const float x = levi_math::clamp01(value);
	const float position = x * float(kSpanShapeLutIntervals);
	const int index = std::min(int(position), kSpanShapeLutIntervals - 1);
	const float fraction = position - float(index);
	return mixf(gSpanShapeLut.values[index], gSpanShapeLut.values[index + 1], fraction);
}

constexpr float kSelfOscResoStart = 0.80f;
constexpr float kSelfOscResoFull = 0.98f;
constexpr float kSelfOscHeatStart = 0.90f;
constexpr float kSelfOscPush = 0.120f;
constexpr float kSelfOscPlateauAmpDamping = 0.045f;
constexpr float kSelfOscPlateauHeatTrim = 1.08f;
constexpr float kSvfSelfOscDampingMin = 0.0005f;

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping, float dampingMin) {
	const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sampleRate);
	const float g = fastTan(kPi * limitedCutoff / sampleRate);
	const float k = clamp(damping, dampingMin, kSvfDampingMax);
	const float a1 = 1.f / (1.f + g * (g + k));
	SvfCoeffs coeffs;
	coeffs.g = g;
	coeffs.k = k;
	coeffs.a1 = a1;
	return coeffs;
}

bool shouldRefreshTitoCoeffs(float cutoff, float cachedCutoff, float damping, float cachedDamping, float sampleRate, float cachedSampleRate, float relativeThreshold, float absoluteThresholdHz) {
	const float dampingThreshold = (relativeThreshold <= 0.f && absoluteThresholdHz <= 0.f) ? 0.f : 1e-5f;
	if (cachedCutoff <= 0.f || std::fabs(sampleRate - cachedSampleRate) > 0.5f || std::fabs(damping - cachedDamping) > dampingThreshold) {
		return true;
	}
	const float thresholdHz = std::max(absoluteThresholdHz, std::fabs(cachedCutoff) * relativeThreshold);
	return std::fabs(cutoff - cachedCutoff) > thresholdHz;
}

void updateTitoCoeffs(
	SvfCoeffs& coeffs,
	float& cachedCutoff,
	float& cachedDamping,
	float& cachedSampleRate,
	float cutoff,
	float damping,
	float sampleRate,
	float relativeThreshold,
	float absoluteThresholdHz,
	float dampingMin = kSvfDampingMin
) {
	if (shouldRefreshTitoCoeffs(cutoff, cachedCutoff, damping, cachedDamping, sampleRate, cachedSampleRate, relativeThreshold, absoluteThresholdHz)) {
		coeffs = makeSvfCoeffs(sampleRate, cutoff, damping, dampingMin);
		cachedCutoff = cutoff;
		cachedDamping = damping;
		cachedSampleRate = sampleRate;
	}
}

SvfOutputs TptSvf::processWithCoeffs(float input, const SvfCoeffs& coeffs) {
	const float v1 = coeffs.a1 * (ic1eq + coeffs.g * (input - ic2eq));
	const float v2 = ic2eq + coeffs.g * v1;

	ic1eq = 2.f * v1 - ic1eq;
	ic2eq = 2.f * v2 - ic2eq;

	SvfOutputs out;
	out.bp = v1;
	out.lp = v2;
	out.hp = input - coeffs.k * v1 - v2;
	out.notch = out.lp + out.hp;
	return out;
}

SvfOutputs TptSvf::processSelfOscWithCoeffs(
	const SvfCoeffs& coeffs,
	float input,
	float oscOnset,
	float oscAmpDamping
) {
	const float m = ic1eq + coeffs.g * (input - ic2eq);
	const float onePlusG2 = 1.f + coeffs.g * coeffs.g;
	float v1 = m / std::max(onePlusG2 + coeffs.g * coeffs.k, 1e-5f);

	const float ampDamping = std::max(oscAmpDamping, 1e-6f);
	const float kEff = coeffs.k - kSelfOscPush * levi_math::clamp01(oscOnset) + ampDamping * v1 * v1;
	v1 = m / std::max(onePlusG2 + coeffs.g * kEff, 1e-5f);

	const float v2 = ic2eq + coeffs.g * v1;
	ic1eq = 2.f * v1 - ic1eq;
	ic2eq = 2.f * v2 - ic2eq;

	SvfOutputs out;
	out.bp = v1;
	out.lp = v2;
	const float outKEff = coeffs.k - kSelfOscPush * levi_math::clamp01(oscOnset) + ampDamping * v1 * v1;
	out.hp = input - outKEff * v1 - v2;
	out.notch = out.lp + out.hp;
	return out;
}

SvfOutputs TptSvf::process(float input, float sampleRate, float cutoff, float damping) {
	return processWithCoeffs(input, makeSvfCoeffs(sampleRate, cutoff, damping));
}

void sanitizeCoreState(TptSvf& core) {
	if (!std::isfinite(core.ic1eq) || !std::isfinite(core.ic2eq)) {
		core.ic1eq = 0.f;
		core.ic2eq = 0.f;
	}
	core.ic1eq = clamp(core.ic1eq, -20.f, 20.f);
	core.ic2eq = clamp(core.ic2eq, -20.f, 20.f);
}

SvfOutputs processCharacterStage(
	TptSvf& core,
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	bool highResonanceSelfOscEnabled,
	const SvfCoeffs* cachedCoeffsOrNull
) {
	(void) stageIndex;
	const SvfCoeffs normalCoeffs = cachedCoeffsOrNull
		? *cachedCoeffsOrNull
		: makeSvfCoeffs(sampleRate, cutoff, damping);
	const CharacterStageState character = prepareCharacterStageState(drive, resoNorm, highResonanceSelfOscEnabled);
	if (!character.selfOscillating) {
		return core.processWithCoeffs(input, normalCoeffs);
	}
	const float selfDamping = mixf(damping, kSvfSelfOscDampingMin, character.oscOnset);
	const SvfCoeffs selfOscCoeffs = makeSvfCoeffs(sampleRate, cutoff, selfDamping, kSvfSelfOscDampingMin);
	return processCharacterStagePrepared(core, input, normalCoeffs, &selfOscCoeffs, character);
}

CharacterStageState prepareCharacterStageState(float drive, float resoNorm, bool highResonanceSelfOscEnabled) {
	CharacterStageState character;
	if (!highResonanceSelfOscEnabled) {
		return character;
	}
	const float r = levi_math::clamp01(resoNorm);
	const float oscNorm = levi_math::smoothstep01((r - kSelfOscResoStart) / (kSelfOscResoFull - kSelfOscResoStart));
	if (oscNorm <= 0.f) {
		return character;
	}
	character.selfOscillating = true;
	character.oscOnset = std::sqrt(oscNorm);
	const float oscHeat = levi_math::smoothstep01((r - kSelfOscHeatStart) / (1.f - kSelfOscHeatStart));
	// Keep the free-running oscillator's level stable across LEVEL. External
	// signals still acquire the full input-stage drive character; this small
	// damping tilt only tightens the oscillator slightly at hot settings.
	const float levelScale = mixf(0.98f, 1.04f, levi_math::clamp01((drive - 1.f) / 2.f));
	// Match nonlinear damping to the growing negative-resistance push so the
	// established onset rises naturally and then settles into a controlled
	// plateau. Heat adds only a small trim instead of the former ~24x increase.
	character.oscAmpDamping = kSelfOscPlateauAmpDamping
		* character.oscOnset
		* mixf(1.f, kSelfOscPlateauHeatTrim, oscHeat)
		* levelScale * levelScale;
	return character;
}

SvfOutputs processCharacterStagePrepared(
	TptSvf& core,
	float input,
	const SvfCoeffs& normalCoeffs,
	const SvfCoeffs* selfOscCoeffsOrNull,
	const CharacterStageState& character
) {
	if (!character.selfOscillating || !selfOscCoeffsOrNull) {
		return core.processWithCoeffs(input, normalCoeffs);
	}
	SvfOutputs out = core.processSelfOscWithCoeffs(
		*selfOscCoeffsOrNull,
		input,
		character.oscOnset,
		character.oscAmpDamping
	);
	if (!std::isfinite(out.lp) || !std::isfinite(out.bp) || !std::isfinite(out.hp) || !std::isfinite(out.notch)) {
		sanitizeCoreState(core);
		return core.processWithCoeffs(input, normalCoeffs);
	}
	return out;
}





float onePoleAlpha(float dt, float tauSeconds) {
	if (tauSeconds <= 0.f) {
		return 1.f;
	}
	return 1.f - fastExp(-std::max(dt, 0.f) / tauSeconds);
}

float logPosition(float hz, float minHz, float maxHz) {
	const float safeHz = clamp(hz, minHz, maxHz);
	return std::log(safeHz / minHz) / std::log(maxHz / minHz);
}

float logFrequencyAt(float x01, float minHz, float maxHz) {
	return minHz * std::pow(maxHz / minHz, levi_math::clamp01(x01));
}

float bifurxFrequencyHzFromParam(float paramValue) {
	// Quantity display/editing is not an audio-rate path. Keep it accurately
	// invertible instead of inheriting the coarse fast-log/exp approximations.
	return kFreqMinHz * std::exp2(kFreqLog2Span * clamp(paramValue, 0.f, 1.f));
}

float bifurxParamFromFrequencyHz(float hz) {
	const float safeHz = clamp(hz, kFreqMinHz, kFreqMaxHz);
	return clamp(std::log2(safeHz / kFreqMinHz) / kFreqLog2Span, 0.f, 1.f);
}

float bifurxSpanSemitonesFromParam(float paramValue) {
	return 96.f * bifurx::shapedSpan(clamp(paramValue, 0.f, 1.f));
}

float bifurxParamFromSpanSemitones(float spanSemitones) {
	const float safeSpan = clamp(spanSemitones, 0.f, 96.f);
	return clamp(std::pow(safeSpan / 96.f, 1.f / 1.45f), 0.f, 1.f);
}

float responseYForDbDisplay(float db, float minDb, float maxDb, float bottomY, float topY) {
	const float clampedDb = clamp(db, minDb, maxDb);
	const float midY = 0.5f * (bottomY + topY);

	if (clampedDb >= 0.f) {
		if (maxDb <= 1e-6f) {
			return midY;
		}
		return rescale(clampedDb, 0.f, maxDb, midY, topY);
	}

	if (minDb >= -1e-6f) {
		return midY;
	}
	return rescale(clampedDb, minDb, 0.f, bottomY, midY);
}

float resoToDamping(float resoNorm) {
	const float r = levi_math::clamp01(resoNorm);
	// Shape resonance in log-Q space so each part of the control travel has an
	// audible effect. The linear term opens the lower half, while r^4 retains
	// the steep approach to self-oscillation near the top. Endpoints remain
	// Q=0.5 (damping=2) and Q=33.33 (damping=0.03).
	constexpr float kLinearLogQ = 2.264f;
	constexpr float kTopLogQ = 1.935705f;
	const float r2 = r * r;
	const float logQOverMin = kLinearLogQ * r + kTopLogQ * r2 * r2;
	return 2.f * fastExp(-logQOverMin);
}

float signedWeight(float balance, bool upperPeak) {
	const float b = clamp(balance, -1.f, 1.f);
	// Slight cubic emphasis: keep midpoint behavior close, push harder near extremes.
	const float shaped = clamp(b + 0.35f * b * b * b, -1.f, 1.f);
	const float sign = upperPeak ? 1.f : -1.f;
	return fastExp(0.82f * sign * shaped);
}

std::complex<double> DisplayBiquad::response(double omega) const {
	const std::complex<double> z1 = std::exp(std::complex<double>(0.f, -omega));
	return response(z1, z1 * z1);
}

std::complex<double> DisplayBiquad::response(std::complex<double> z1, std::complex<double> z2) const {
	const std::complex<double> numerator = b0 + b1 * z1 + b2 * z2;
	const std::complex<double> denominator = 1.0 + a1 * z1 + a2 * z2;
	return numerator / denominator;
}

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
	const float damping = 1.f / std::max(q, 1.f / kSvfDampingMax);
	const SvfCoeffs coeffs = makeSvfCoeffs(sampleRate, cutoff, damping);
	const double g = coeffs.g;
	const double g2 = g * g;
	const double k = coeffs.k;
	const double a = 1.0 / (1.0 + g * (g + k));

	// Exact z-domain transfer functions of TptSvf::processWithCoeffs(). This
	// keeps the nominal gold curve aligned with the production linear core.
	DisplayBiquad biquad;
	biquad.a1 = 2.f * a * (g2 - 1.f);
	biquad.a2 = 2.f * a * (g2 + 1.f) - 1.f;
	switch (type) {
		case 0: // lowpass
			biquad.b0 = a * g2;
			biquad.b1 = 2.f * a * g2;
			biquad.b2 = a * g2;
			break;
		case 1: // bandpass
			biquad.b0 = a * g;
			biquad.b1 = 0.f;
			biquad.b2 = -a * g;
			break;
		case 2: // highpass
			biquad.b0 = 1.f - a * g2 - a * g * k;
			biquad.b1 = -2.f * a;
			biquad.b2 = a * (g2 + g * k + 2.f) - 1.f;
			break;
		default: // notch, exactly LP + HP
			biquad.b0 = a * g2 + (1.f - a * g2 - a * g * k);
			biquad.b1 = 2.f * a * g2 - 2.f * a;
			biquad.b2 = a * g2 + a * (g2 + g * k + 2.f) - 1.f;
			break;
	}
	return biquad;
}

void Bifurx::publishPreviewState(const BifurxPreviewState& state) {
	const int writeIndex = findWritableSlot(previewPublishedToken, previewStateReaders);
	if (writeIndex < 0) {
		return;
	}
	const uint32_t seq = previewPublishSeq.load(std::memory_order_relaxed) + 1u;
	const double publishTime = system::getTime();
	previewStates[writeIndex] = state;
	previewStatePublishTimes[writeIndex] = publishTime;
	previewStateSeqs[writeIndex] = seq;
	previewPublishedToken.store(
		makePublishedSlotToken(++previewPublishGeneration, writeIndex),
		std::memory_order_release
	);
	previewStateReaders[writeIndex].store(0u, std::memory_order_release);
	previewPublishSeq.store(seq, std::memory_order_release);
	lastPreviewState = state;
	hasLastPreviewState = true;
}

void Bifurx::publishLlTelemetryState(const BifurxLlTelemetryState& state) {
	const int writeIndex = findWritableSlot(llTelemetryPublishedToken, llTelemetryStateReaders);
	if (writeIndex < 0) {
		return;
	}
	const uint32_t seq = llTelemetryPublishSeq.load(std::memory_order_relaxed) + 1u;
	llTelemetryStates[writeIndex] = state;
	llTelemetryStateSeqs[writeIndex] = seq;
	llTelemetryPublishedToken.store(
		makePublishedSlotToken(++llTelemetryPublishGeneration, writeIndex),
		std::memory_order_release
	);
	llTelemetryStateReaders[writeIndex].store(0u, std::memory_order_release);
	llTelemetryPublishSeq.store(seq, std::memory_order_release);
}

bool Bifurx::readPreviewState(
	uint32_t lastSeq,
	BifurxPreviewState* state,
	double* publishTime,
	uint32_t* seq
) {
	if (!state || !publishTime || !seq) {
		return false;
	}
	bool changed = false;
	readPublishedSlot(previewPublishedToken, previewStateReaders, [&](int slot) {
		const uint32_t slotSeq = previewStateSeqs[slot];
		if (slotSeq != lastSeq) {
			*state = previewStates[slot];
			*publishTime = previewStatePublishTimes[slot];
			*seq = slotSeq;
			changed = true;
		}
	});
	return changed;
}

bool Bifurx::readLlTelemetryState(
	uint32_t lastSeq,
	BifurxLlTelemetryState* state,
	uint32_t* seq
) {
	if (!state || !seq) {
		return false;
	}
	bool changed = false;
	readPublishedSlot(llTelemetryPublishedToken, llTelemetryStateReaders, [&](int slot) {
		const uint32_t slotSeq = llTelemetryStateSeqs[slot];
		if (slotSeq != lastSeq) {
			*state = llTelemetryStates[slot];
			*seq = slotSeq;
			changed = true;
		}
	});
	return changed;
}

bool Bifurx::copyAnalysisFrame(
	uint32_t lastSeq,
	float* rawInput,
	float* output,
	uint32_t* seq
) {
	if (!rawInput || !output || !seq) {
		return false;
	}
	bool changed = false;
	readPublishedSlot(analysisPublishedToken, analysisFrameReaders, [&](int slot) {
		const uint32_t slotSeq = analysisFrameSeqs[slot];
		if (slotSeq != lastSeq) {
			std::memcpy(rawInput, analysisFrames[slot].rawInput, sizeof(analysisFrames[slot].rawInput));
			std::memcpy(output, analysisFrames[slot].output, sizeof(analysisFrames[slot].output));
			*seq = slotSeq;
			changed = true;
		}
	});
	return changed;
}

void Bifurx::pushAnalysisSample(float rawInputSample, float outputSample) {
	if (analysisVisualSubscribers.load(std::memory_order_acquire) == 0u
		|| (visualWatchdogEnabled.load(std::memory_order_relaxed) && visualLeaseSamples == 0)) {
		if (analysisCaptureSlots[0] >= 0 || analysisCaptureSlots[1] >= 0 || analysisCaptureCountdown != 0) {
			resetAnalysisCapture();
		}
		return;
	}
	if (analysisCaptureCountdown <= 0) {
		int captureIndex = (analysisCaptureSlots[0] < 0) ? 0 : ((analysisCaptureSlots[1] < 0) ? 1 : -1);
		if (captureIndex >= 0) {
			const int slot = findWritableSlot(
				analysisPublishedToken,
				analysisFrameReaders,
				analysisCaptureSlots[0],
				analysisCaptureSlots[1]
			);
			if (slot >= 0) {
				analysisCaptureSlots[captureIndex] = slot;
				analysisCapturePositions[captureIndex] = 0;
			}
		}
		analysisCaptureCountdown = kFftHopSize;
	}

	const float safeRawInput = bifurx::sanitizeFinite(rawInputSample);
	const float safeOutput = bifurx::sanitizeFinite(outputSample);
	for (int captureIndex = 0; captureIndex < 2; ++captureIndex) {
		const int slot = analysisCaptureSlots[captureIndex];
		if (slot < 0) {
			continue;
		}
		const int position = analysisCapturePositions[captureIndex];
		analysisFrames[slot].rawInput[position] = safeRawInput;
		analysisFrames[slot].output[position] = safeOutput;
		const int nextPosition = position + 1;
		analysisCapturePositions[captureIndex] = nextPosition;
		if (nextPosition >= kFftSize) {
			const uint32_t seq = analysisPublishSeq.load(std::memory_order_relaxed) + 1u;
			analysisFrameSeqs[slot] = seq;
			analysisPublishedToken.store(
				makePublishedSlotToken(++analysisPublishGeneration, slot),
				std::memory_order_release
			);
			analysisFrameReaders[slot].store(0u, std::memory_order_release);
			analysisPublishSeq.store(seq, std::memory_order_release);
			analysisCaptureSlots[captureIndex] = -1;
			analysisCapturePositions[captureIndex] = 0;
		}
	}
	analysisCaptureCountdown--;
}
void Bifurx::processBypass(const ProcessArgs& args) {
#if defined(LEVIATHAN_PRO_DRM) && LEVIATHAN_PRO_DRM
	if (!isLicenseVerified()) {
		outputs[OUT_OUTPUT].setChannels(1);
		outputs[OUT_OUTPUT].setVoltage(0.f);
		return;
	}
#endif
	(void)args;
	outputs[OUT_OUTPUT].setChannels(1);
	outputs[OUT_OUTPUT].setVoltage(bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()));
}

void Bifurx::process(const ProcessArgs& args) {
	outputs[OUT_OUTPUT].setChannels(1);
#if defined(LEVIATHAN_PRO_DRM) && LEVIATHAN_PRO_DRM
	if (!isLicenseVerified()) {
		// Clear a previous sample/polyphonic bypass result rather than holding it.
		outputs[OUT_OUTPUT].setChannels(1);
		outputs[OUT_OUTPUT].setVoltage(0.f);
		return;
	}
#endif
	using PerfClock = std::chrono::steady_clock;
	const bool debugEnabled = isDragonKingDebugEnabled();
	const bool measurePerf = debugEnabled && perfMeasureDivider.process();
	const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();

	if (visualWatchdogEnabled.load(std::memory_order_relaxed)) {
		const uint32_t heartbeat = visualHeartbeat.load(std::memory_order_relaxed);
		if (heartbeat != audioVisualHeartbeat) {
			audioVisualHeartbeat = heartbeat;
			visualLeaseSamples = int(args.sampleRate * 0.25f);
		}
		if (visualLeaseSamples > 0) --visualLeaseSamples;
	}
	const bool visualDemand = analysisVisualSubscribers.load(std::memory_order_acquire) > 0
		&& (!visualWatchdogEnabled.load(std::memory_order_relaxed) || visualLeaseSamples > 0);
	const float in = bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()), level = params[LEVEL_PARAM].getValue(), drive = levelDriveGain(level);
	const float tito = clamp(params[TITO_PARAM].getValue(), -1.f, 1.f);
	const float titoAbs = std::fabs(tito);
	const bool titoNeutral = titoAbs < 0.02f;
	const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	// V/Oct is a calibrated pitch input, so follow it directly. Any glide is an
	// explicit patching choice rather than an undocumented module behavior.
	const float voctCv = voctConnected ? clamp(bifurx::sanitizeFinite(inputs[VOCT_INPUT].getVoltage()), -10.f, 10.f) : 0.f;
	const bool fmConnected = inputs[FM_INPUT].isConnected();
	const bool resoCvConnected = inputs[RESO_CV_INPUT].isConnected();
	const bool balanceCvConnected = inputs[BALANCE_CV_INPUT].isConnected();
	const bool spanCvConnected = inputs[SPAN_CV_INPUT].isConnected();
	const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f), fmCv = fmConnected ? clamp(bifurx::sanitizeFinite(inputs[FM_INPUT].getVoltage()), -10.f, 10.f) : 0.f, fm = fmCv * fmAmt;
	const bool slowCvConnected = resoCvConnected || balanceCvConnected || spanCvConnected;
	const bool audioRateControlsActive = voctConnected || fmConnected;
	const bool fastPathEligible = titoNeutral && !voctConnected && !fmConnected && !slowCvConnected;
	int targetControlDivision = 16;
	float titoCoeffRelativeThreshold = kTitoCoeffRelativeUpdateThreshold;
	float titoCoeffAbsoluteThresholdHz = kTitoCoeffAbsoluteUpdateThresholdHz;
	const int modulationQualityModeNow = modulationQualityMode.load(std::memory_order_relaxed);
	if (modulationQualityModeNow != audioQualityMode) {
		audioQualityMode = modulationQualityModeNow;
		controlFastCacheValid = false;
	}
	switch (modulationQualityModeNow) {
		case MOD_QUALITY_HIGH:
			targetControlDivision = slowCvConnected ? 8 : 16;
			titoCoeffRelativeThreshold = 0.5f * kTitoCoeffRelativeUpdateThreshold;
			titoCoeffAbsoluteThresholdHz = 0.5f * kTitoCoeffAbsoluteUpdateThresholdHz;
			break;
		case MOD_QUALITY_EXACT:
			targetControlDivision = 1;
			titoCoeffRelativeThreshold = 0.f;
			titoCoeffAbsoluteThresholdHz = 0.f;
			break;
		case MOD_QUALITY_BALANCED:
		default:
			targetControlDivision = 16;
			break;
	}
	if (targetControlDivision != controlUpdateDivision) {
		controlUpdateDivision = targetControlDivision;
		controlUpdateDivider.setDivision(controlUpdateDivision);
	}
	const bool controlDividerTick = controlUpdateDivider.process();
	const bool initializeControlState = !controlFastCacheValid;
	if (controlDividerTick) {
		if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1); params[MODE_PARAM].setValue(float((currentMode + kBifurxUiModeCount - 1) % kBifurxUiModeCount)); }
		if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1); params[MODE_PARAM].setValue(float((currentMode + 1) % kBifurxUiModeCount)); }
	}
	if (params[MODE_PARAM].getValue() > float(kBifurxUiModeCount - 1)) {
		params[MODE_PARAM].setValue(float(kBifurxUiModeCount - 1));
	}
	const int mode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, kBifurxUiModeCount - 1);
	if (controlDividerTick || initializeControlState) {
		perfSampleRate.store(args.sampleRate, std::memory_order_relaxed);
		perfMode.store(mode, std::memory_order_relaxed);
		perfFastPathEligible.store(fastPathEligible, std::memory_order_relaxed);
		perfPreviewPitchCvConnected.store(voctConnected || fmConnected, std::memory_order_relaxed);
		cachedLowLatencyVisual = lowLatencyVisual.load(std::memory_order_relaxed);
		cachedHighResonanceSelfOscEnabled = highResonanceSelfOscEnabled.load(std::memory_order_relaxed);
		cachedSoftLimitingEnabled = softLimitingEnabled.load(std::memory_order_relaxed);
		const bool nextNonlinearOversamplingEnabled = debugEnabled
			? nonlinearOversamplingEnabled.load(std::memory_order_relaxed)
			: true;
		cachedNonlinearOversamplingEnabled = nextNonlinearOversamplingEnabled;
	}
	const bool forceAudioRateControls = modulationQualityModeNow == MOD_QUALITY_EXACT;
	const bool inspectSlowControls =
		initializeControlState || controlDividerTick || (forceAudioRateControls && slowCvConnected);
	if (std::fabs(previewFilterAlphaSampleRate - args.sampleRate) > 0.5f) { previewFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.05f); previewFilterAlphaSlow = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.20f); previewFilterAlphaSampleRate = args.sampleRate; }
	if (std::fabs(llTelemetryAlphaSampleRate - args.sampleRate) > 0.5f) {
		llTelemetryAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kLlTelemetryTauSeconds);
		llTelemetryAlphaSampleRate = args.sampleRate;
	}

	float freqA0 = cachedFreqA0, freqB0 = cachedFreqB0, dampingA = cachedDampingA, dampingB = cachedDampingB, wA = cachedWA, wB = cachedWB, balance = cachedBalance;
	float resoNorm = cachedResoNorm, balanceNorm = cachedBalanceNorm, spanParamNorm = cachedSpanParamNorm, spanCvNorm = cachedSpanCvNorm, spanAtten = cachedSpanAtten, spanNorm = cachedSpanNorm, spanOct = cachedSpanOct;
	bool slowDerivedStateChanged = false;
	if (inspectSlowControls) {
		const float resoCvNorm = resoCvConnected ? clamp(bifurx::sanitizeFinite(inputs[RESO_CV_INPUT].getVoltage()), 0.f, 8.f) / 8.f : 0.f;
		const float nextResoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f);
		const float balanceCvNorm = balanceCvConnected ? clamp(bifurx::sanitizeFinite(inputs[BALANCE_CV_INPUT].getVoltage()), -5.f, 5.f) / 5.f : 0.f;
		const float nextBalanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f);
		const float nextSpanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f);
		const float nextSpanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f);
		const float nextSpanCvNorm = spanCvConnected ? clamp(bifurx::sanitizeFinite(inputs[SPAN_CV_INPUT].getVoltage()), -10.f, 10.f) / 5.f : 0.f;
		const float nextSpanNorm = clamp(nextSpanParamNorm + 0.5f * nextSpanAtten * nextSpanCvNorm, 0.f, 1.f);
		const bool slowSourcesChanged = initializeControlState
			|| nextResoNorm != cachedResoNorm
			|| nextBalanceNorm != cachedBalanceNorm
			|| nextSpanParamNorm != cachedSpanParamNorm
			|| nextSpanCvNorm != cachedSpanCvNorm
			|| nextSpanAtten != cachedSpanAtten
			|| nextSpanNorm != cachedSpanNorm;
		slowDerivedStateChanged = initializeControlState
			|| nextResoNorm != cachedResoNorm
			|| nextBalanceNorm != cachedBalanceNorm
			|| nextSpanNorm != cachedSpanNorm;
		if (slowSourcesChanged) {
			resoNorm = nextResoNorm;
			balanceNorm = nextBalanceNorm;
			spanParamNorm = nextSpanParamNorm;
			spanAtten = nextSpanAtten;
			spanCvNorm = nextSpanCvNorm;
			spanNorm = nextSpanNorm;
			cachedResoNorm = resoNorm;
			cachedBalanceNorm = balanceNorm;
			cachedSpanParamNorm = spanParamNorm;
			cachedSpanCvNorm = spanCvNorm;
			cachedSpanAtten = spanAtten;
			cachedSpanNorm = spanNorm;
		}
		if (slowDerivedStateChanged) {
			spanOct = 8.f * bifurx::shapedSpan(spanNorm);
			balance = balanceNorm;
			const float baseDamping = resoToDamping(resoNorm);
			dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f);
			dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
			const float lowW = signedWeight(balance, false), highW = signedWeight(balance, true), norm = 2.f / (lowW + highW);
			wA = lowW * norm;
			wB = highW * norm;
			cachedDampingA = dampingA; cachedDampingB = dampingB; cachedWA = wA; cachedWB = wB; cachedBalance = balance; cachedSpanOct = spanOct;
		}
	}
	const bool pitchSourcesChanged = initializeControlState
		|| freqParamNorm != cachedFreqParamNorm
		|| voctCv != cachedVoctCv
		|| fm != cachedFm
		|| std::fabs(args.sampleRate - cachedPitchSampleRate) > 0.5f;
	const bool updatePitchControls = slowDerivedStateChanged || pitchSourcesChanged;
	if (updatePitchControls) {
		const float sr = std::max(args.sampleRate, 1.f);
		const float maxHz = 0.46f * sr;
		if (std::fabs(cachedFrequencyRangeSampleRate - sr) > 0.5f) {
			cachedFrequencyRangeOctaves = std::log2(maxHz / kFreqMinHz);
			cachedFrequencyRangeSampleRate = sr;
		}
		const float availableOctaves = std::max(cachedFrequencyRangeOctaves, 0.f);
		const float effectiveSpanOct = std::min(spanOct, availableOctaves);
		const float halfSpanOct = 0.5f * effectiveSpanOct;
		const float requestedCenterOct = kFreqLog2Span * freqParamNorm + voctCv + fm;
		const float shiftedCenterOct = clamp(requestedCenterOct, halfSpanOct, availableOctaves - halfSpanOct);
		freqA0 = kFreqMinHz * fastExp2(shiftedCenterOct - halfSpanOct);
		freqB0 = kFreqMinHz * fastExp2(shiftedCenterOct + halfSpanOct);
		cachedFreqA0 = freqA0; cachedFreqB0 = freqB0;
		cachedCoeffsA = makeSvfCoeffs(args.sampleRate, freqA0, dampingA);
		cachedCoeffsB = makeSvfCoeffs(args.sampleRate, freqB0, dampingB);
		cachedFreqParamNorm = freqParamNorm;
		cachedVoctCv = voctCv;
		cachedFm = fm;
		cachedPitchSampleRate = args.sampleRate;
		controlFastCacheValid = true;
	}

	// Production always uses the 2x IIR4 path, irrespective of saved debug settings.
	// Keep boundary changes on the existing click-free transition path.
	const int requestedBoundary = !debugEnabled ? 3
		: !cachedNonlinearOversamplingEnabled ? 0
		: boundaryResampling.load(std::memory_order_relaxed);
	const bool primeBoundary = requestedBoundary != transitionSmoother.activeBoundary
		|| (isBifurxDisplayOnlyMode(transitionSmoother.activeMode) && !isBifurxDisplayOnlyMode(mode));
	transitionSmoother.prepare(mode, cachedSoftLimitingEnabled, args.sampleRate, requestedBoundary, primeBoundary);
	const int boundary = transitionSmoother.activeBoundary;
	if (boundary != audioBoundary) {
		nonlinearOversampling.reset();
		legacyOversampling.reset();
		iirOversampling.reset();
		audioBoundary = boundary;
	}
	const float titoModeScale = 1.22f, titoStrength = 2.4f * titoAbs, couplingDepth = titoStrength * titoModeScale * (0.026f + 0.28f * resoNorm * resoNorm);
	const float drivenIn = isBifurxDisplayOnlyMode(transitionSmoother.activeMode) ? in
		: boundary == 3 ? iirOversampling.processInput(in, level)
		: boundary == 2 ? nonlinearOversampling.processInput(in, level)
		: boundary == 1 ? legacyOversampling.processInput(in, level)
		: applyLevelInputStage(in, level);
	const bool highResonanceSelfOscEnabledNow = cachedHighResonanceSelfOscEnabled;
	if (!isBifurxDisplayOnlyMode(transitionSmoother.activeMode) && (!cachedCharacterStateValid
		|| drive != cachedCharacterDrive
		|| resoNorm != cachedCharacterResoNorm
		|| highResonanceSelfOscEnabledNow != cachedCharacterHighResEnabled
	)) {
		cachedCharacterState = prepareCharacterStageState(drive, resoNorm, highResonanceSelfOscEnabledNow);
		cachedCharacterDrive = drive;
		cachedCharacterResoNorm = resoNorm;
		cachedCharacterHighResEnabled = highResonanceSelfOscEnabledNow;
		cachedCharacterStateValid = true;
	}
	const CharacterStageState& character = cachedCharacterState;
	const float oscNorm = character.selfOscillating ? character.oscOnset * character.oscOnset : 0.f;
	const float selfOscSeed = (oscNorm > 0.f) ? (2e-7f + 8e-7f * oscNorm) : 0.f;
	if ((highResonanceSelfOscEnabledNow && oscNorm > 0.f) || controlDividerTick) {
		sanitizeCoreState(coreA);
		sanitizeCoreState(coreB);
	}
	const float excitation = drivenIn + selfOscSeed;
	float cutoffA = freqA0, cutoffB = freqB0;
	const SvfCoeffs* coeffsAForSample = &cachedCoeffsA;
	const SvfCoeffs* coeffsBForSample = &cachedCoeffsB;
	if (!isBifurxDisplayOnlyMode(transitionSmoother.activeMode) && !titoNeutral) {
		const float depthScaled = couplingDepth * 0.2f;
		float modA = 0.f, modB = 0.f;
		if (tito < 0.f) { modA = depthScaled * coreA.ic1eq; modB = depthScaled * coreB.ic1eq; }
		else { modA = depthScaled * coreB.ic1eq; modB = depthScaled * coreA.ic1eq; }
		cutoffA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f)); cutoffB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f));
		updateTitoCoeffs(titoCoeffsA, titoCoeffFreqA, titoCoeffDampingA, titoCoeffSampleRateA, cutoffA, dampingA, args.sampleRate, titoCoeffRelativeThreshold, titoCoeffAbsoluteThresholdHz);
		updateTitoCoeffs(titoCoeffsB, titoCoeffFreqB, titoCoeffDampingB, titoCoeffSampleRateB, cutoffB, dampingB, args.sampleRate, titoCoeffRelativeThreshold, titoCoeffAbsoluteThresholdHz);
		coeffsAForSample = &titoCoeffsA;
		coeffsBForSample = &titoCoeffsB;
	}
	const SvfCoeffs* selfOscCoeffsAForSample = nullptr;
	const SvfCoeffs* selfOscCoeffsBForSample = nullptr;
	if (!isBifurxDisplayOnlyMode(transitionSmoother.activeMode) && character.selfOscillating) {
		const float selfDampingA = mixf(dampingA, kSvfSelfOscDampingMin, character.oscOnset);
		const float selfDampingB = mixf(dampingB, kSvfSelfOscDampingMin, character.oscOnset);
		// Preserve sample-accurate pitch modulation in the otherwise static
		// self-oscillator path. TITO already owns an explicit quality threshold.
		const float selfOscRelativeThreshold = (titoNeutral && audioRateControlsActive) ? 0.f : titoCoeffRelativeThreshold;
		const float selfOscAbsoluteThresholdHz = (titoNeutral && audioRateControlsActive) ? 0.f : titoCoeffAbsoluteThresholdHz;
		updateTitoCoeffs(selfOscCoeffsA, selfOscCoeffFreqA, selfOscCoeffDampingA, selfOscCoeffSampleRateA, cutoffA, selfDampingA, args.sampleRate, selfOscRelativeThreshold, selfOscAbsoluteThresholdHz, kSvfSelfOscDampingMin);
		updateTitoCoeffs(selfOscCoeffsB, selfOscCoeffFreqB, selfOscCoeffDampingB, selfOscCoeffSampleRateB, cutoffB, selfDampingB, args.sampleRate, selfOscRelativeThreshold, selfOscAbsoluteThresholdHz, kSvfSelfOscDampingMin);
		selfOscCoeffsAForSample = &selfOscCoeffsA;
		selfOscCoeffsBForSample = &selfOscCoeffsB;
	}
	float modeOut = 0.f, llExc = 0.f, llA = 0.f, llB = 0.f;
	auto pA = [&](float s) {
		return processCharacterStagePrepared(coreA, s, *coeffsAForSample, selfOscCoeffsAForSample, character);
	};
	auto pB = [&](float s) {
		return processCharacterStagePrepared(coreB, s, *coeffsBForSample, selfOscCoeffsBForSample, character);
	};

	const int audioMode = transitionSmoother.activeMode;
	const bool softLimitingEnabledNow = transitionSmoother.activeSoftLimitingEnabled;
	const bool displayOnlyMode = isBifurxDisplayOnlyMode(audioMode);
	if (displayOnlyMode) {
		modeOut = in;
	}
	else {
		switch (audioMode) {
			case 0: { const SvfOutputs a = pA(excitation), b = pB(a.lp); llExc = excitation; llA = a.lp; llB = b.lp; modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 1: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 2: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB); } break;
			case 3: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 4: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 5: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 6: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB); } break;
			case 7: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB); } break;
			case 8: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(audioMode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 9: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = b.hp; } break;
			default: { const SvfOutputs a = pA(excitation), b = pB(a.lp); modeOut = combineModeResponse<float>(0, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
		}
	}

	// Exponential self-modulation can rectify a zero-mean signal into a large
	// operating-point shift. Apply a slow correction before the limiter, while
	// measuring the final audible output below so limiter asymmetry is included.
	// Neutral TITO and XM retain their existing output exactly.
	const float titoSmDcMix = displayOnlyMode ? 0.f : clamp((-tito - 0.02f) / 0.98f, 0.f, 1.f);
	const float dcCorrectedModeOut = modeOut - titoSmDcMix * titoSmDcCorrection;

	const float targetOut = displayOnlyMode ? in
		: (boundary == 2
			? nonlinearOversampling.processOutput(dcCorrectedModeOut, level, softLimitingEnabledNow)
			: boundary == 3 ? iirOversampling.processOutput(dcCorrectedModeOut, level, softLimitingEnabledNow)
			: boundary == 1 ? legacyOversampling.processOutput(dcCorrectedModeOut, level, softLimitingEnabledNow)
			: applyLevelOutputStage(dcCorrectedModeOut, level, softLimitingEnabledNow));
	const float out = transitionSmoother.apply(targetOut);
	outputs[OUT_OUTPUT].setVoltage(out);
	constexpr float kTitoSmDcServoHz = 1.f;
	const float titoSmDcAlpha = std::min(2.f * kPi * kTitoSmDcServoHz * args.sampleTime, 1.f);
	if (titoSmDcMix > 0.f) {
		titoSmDcCorrection += titoSmDcAlpha * out;
	}
	else {
		titoSmDcCorrection += titoSmDcAlpha * (0.f - titoSmDcCorrection);
	}
	if (!std::isfinite(titoSmDcCorrection)) titoSmDcCorrection = 0.f;
	titoSmDcCorrection = clamp(titoSmDcCorrection, -10.f, 10.f);
	const bool diagnosticsActive = debugEnabled && curveDebugLogging.load(std::memory_order_relaxed);
	const float llAlpha = llTelemetryAlpha;
	if (diagnosticsActive) {
		if (audioMode == 0) { llTelemetryExcitationSq += llAlpha * (llExc * llExc - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (llA * llA - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (llB * llB - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
		else { llTelemetryExcitationSq += llAlpha * (0.f - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (0.f - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (0.f - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	}

	const bool previewDemand = visualDemand || diagnosticsActive;
	if (previewDemand) {
		if (!previewDemandWasActive) {
			hasLastPreviewState = false;
			previewFilterInitialized = false;
			previewTargetMotionInitialized = false;
			previewTargetStillSamples = 0;
			previewSampleAccum = 0;
		}
		const bool pPitchCvConn = voctConnected || fmConnected;
		const bool lowLatencyVisualNow = cachedLowLatencyVisual;
		const int targetFastPreviewDivision = lowLatencyVisualNow ? 64 : bifurx::kPreviewPublishFastDivision;
		const int targetSlowPreviewDivision = lowLatencyVisualNow ? 128 : bifurx::kPreviewPublishSlowDivision;
		if (targetFastPreviewDivision != previewPublishFastDivision) {
			previewPublishFastDivision = targetFastPreviewDivision;
			previewPublishDivider.setDivision(previewPublishFastDivision);
		}
		if (targetSlowPreviewDivision != previewPublishSlowDivision) {
			previewPublishSlowDivision = targetSlowPreviewDivision;
			previewPublishSlowDivider.setDivision(previewPublishSlowDivision);
		}
		const bool perTick = pPitchCvConn ? previewPublishSlowDivider.process() : previewPublishDivider.process();
		previewSampleAccum++;
		const bool shouldUpdatePreviewState = perTick || !hasLastPreviewState;
		if (shouldUpdatePreviewState) {
			const int elapsedSamples = std::max(previewSampleAccum, 1);
			previewSampleAccum = 0;
			const float pTFqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate), pTFqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate), pTQA = 1.f / clamp(dampingA, kSvfDampingMin, kSvfDampingMax), pTQB = 1.f / clamp(dampingB, kSvfDampingMin, kSvfDampingMax), pTBal = balance;
			const float pSmAlpha = pPitchCvConn ? previewFilterAlphaSlow : previewFilterAlpha;
			const float oneMinusAlpha = clamp(1.f - pSmAlpha, 0.f, 1.f);
			if (!previewTargetMotionInitialized) { previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB; previewTargetStillSamples = 0; previewTargetMotionInitialized = true; }
			const float tMAOct = std::fabs(fastLog2(std::max(pTFqA, 1.f)) - fastLog2(std::max(previewPrevTargetFreqA, 1.f)));
			const float tMBOct = std::fabs(fastLog2(std::max(pTFqB, 1.f)) - fastLog2(std::max(previewPrevTargetFreqB, 1.f)));
			const float tMOct = std::max(tMAOct, tMBOct);
			if (tMOct <= kPreviewInstantSettleMotionOctThreshold) {
				const int held = clamp(previewTargetStillSamples, 0, kPreviewInstantSettleHoldSamples);
				previewTargetStillSamples = held + std::min(elapsedSamples, kPreviewInstantSettleHoldSamples - held);
			}
			else previewTargetStillSamples = 0;
			const bool pInstSettle = (previewTargetStillSamples >= kPreviewInstantSettleHoldSamples);
			previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB;
			if (!previewFilterInitialized || pInstSettle) { previewFreqAFiltered = pTFqA; previewFreqBFiltered = pTFqB; previewQAFiltered = pTQA; previewQBFiltered = pTQB; previewBalanceFiltered = pTBal; previewFilterInitialized = true; }
			else { const float a = 1.f - std::pow(oneMinusAlpha, float(elapsedSamples)); previewFreqAFiltered += a * (pTFqA - previewFreqAFiltered); previewFreqBFiltered += a * (pTFqB - previewFreqBFiltered); previewQAFiltered += a * (pTQA - previewQAFiltered); previewQBFiltered += a * (pTQB - previewQBFiltered); previewBalanceFiltered += a * (pTBal - previewBalanceFiltered); }

			BifurxPreviewState pS; pS.sampleRate = args.sampleRate; pS.freqA = previewFreqAFiltered; pS.freqB = previewFreqBFiltered; pS.qA = previewQAFiltered; pS.qB = previewQBFiltered; pS.mode = mode; pS.boundary = boundary; pS.balance = previewBalanceFiltered; pS.balanceTarget = balanceNorm; pS.resoNorm = resoNorm; pS.spanParamNorm = spanParamNorm; pS.spanCvNorm = spanCvNorm; pS.spanAtten = spanAtten; pS.spanNorm = spanNorm; pS.spanOct = spanOct; pS.freqParamNorm = freqParamNorm; pS.voctCv = voctCv;
			if (!hasLastPreviewState || (perTick && previewStatesDiffer(pS, lastPreviewState))) publishPreviewState(pS);
			if (perTick && diagnosticsActive) { BifurxLlTelemetryState llTS; llTS.active = (mode == 0); llTS.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f)); llTS.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f)); llTS.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f)); llTS.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f)); llTS.stageBLpOverALpDb = amplitudeRatioDb(llTS.stageBLpRms, llTS.stageALpRms); llTS.outputOverInputDb = amplitudeRatioDb(llTS.outputRms, llTS.excitationRms); publishLlTelemetryState(llTS); }
		}
	}
	previewDemandWasActive = previewDemand;
	// The measured module-response overlay represents the complete audible
	// transfer, including the optional output safety stage.

	pushAnalysisSample(in, out);

	if (controlDividerTick) {
		lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f)); lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));
		lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f)); lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));
		lights[TITO_SM_LIGHT].setBrightness(std::max(-tito, 0.f));
		lights[TITO_XM_LIGHT].setBrightness(std::max(tito, 0.f));
	}

	if (measurePerf) {
		const uint64_t elapsedNs = uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(PerfClock::now() - perfStart).count());
		debug_terminal::recordAudioProcessTiming(perfAudioProcessRangeMinNs, perfAudioProcessRangeMaxNs, elapsedNs, &perfAudioProcessRangeAverage);
	}
}

} // namespace bifurx
