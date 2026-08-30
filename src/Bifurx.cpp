#include "Bifurx.hpp"
#include "BifurxRenderData.hpp"
#include "BifurxWorker.hpp"
#include "DebugTerminalTransport.hpp"
#include "UndertowShape.hpp"

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
	return system::join(asset::user(), "Leviathan/Bifurx");
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
	const float levelScale = mixf(0.85f, 1.35f, levi_math::clamp01((drive - 1.f) / 2.f));
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

std::complex<float> DisplayBiquad::response(float omega) const {
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	return response(z1, z1 * z1);
}

std::complex<float> DisplayBiquad::response(std::complex<float> z1, std::complex<float> z2) const {
	const std::complex<float> numerator = b0 + b1 * z1 + b2 * z2;
	const std::complex<float> denominator = 1.f + a1 * z1 + a2 * z2;
	return numerator / denominator;
}

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
	const float damping = 1.f / std::max(q, 1.f / kSvfDampingMax);
	const SvfCoeffs coeffs = makeSvfCoeffs(sampleRate, cutoff, damping);
	const float g = coeffs.g;
	const float g2 = g * g;
	const float k = coeffs.k;
	const float a = coeffs.a1;

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

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) return true;
	if (std::fabs(a.sampleRate - b.sampleRate) > 0.5f) return true;
	if (std::fabs(a.balance - b.balance) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqA, 1.f)) - fastLog2(std::max(b.freqA, 1.f))) > 1e-3f) return true;
	if (std::fabs(fastLog2(std::max(a.freqB, 1.f)) - fastLog2(std::max(b.freqB, 1.f))) > 1e-3f) return true;
	if (std::fabs(a.qA - b.qA) > 1e-3f) return true;
	if (std::fabs(a.qB - b.qB) > 1e-3f) return true;
	if (std::fabs(a.spanNorm - b.spanNorm) > 1e-4f) return true;
	if (std::fabs(a.resoNorm - b.resoNorm) > 1e-4f) return true;
	return false;
}

BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state) {
	BifurxPreviewModel model;
	const float freqA = clamp(state.freqA, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float freqB = clamp(state.freqB, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float qMin = 1.f / kSvfDampingMax;
	const float qMax = 1.f / kSvfDampingMin;
	const float qA = clamp(state.qA, qMin, qMax);
	const float qB = clamp(state.qB, qMin, qMax);
	model.lowA = makeDisplayBiquad(state.sampleRate, freqA, qA, 0);
	model.bandA = makeDisplayBiquad(state.sampleRate, freqA, qA, 1);
	model.highA = makeDisplayBiquad(state.sampleRate, freqA, qA, 2);
	model.notchA = makeDisplayBiquad(state.sampleRate, freqA, qA, 3);
	model.lowB = makeDisplayBiquad(state.sampleRate, freqB, qB, 0);
	model.bandB = makeDisplayBiquad(state.sampleRate, freqB, qB, 1);
	model.highB = makeDisplayBiquad(state.sampleRate, freqB, qB, 2);
	model.notchB = makeDisplayBiquad(state.sampleRate, freqB, qB, 3);
	model.markerFreqA = freqA;
	model.markerFreqB = freqB;
	model.sampleRate = state.sampleRate;
	model.qA = qA;
	model.qB = qB;
	model.resoNorm = state.resoNorm;
	model.mode = state.mode;

	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	model.wA = lowW * norm;
	model.wB = highW * norm;
	return model;
}

std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz) {
	const float omega = 2.f * kPi * clamp(hz, 4.f, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	const std::complex<float> z2 = z1 * z1;

	std::complex<float> lpA = model.lowA.response(z1, z2);
	std::complex<float> bpA = model.bandA.response(z1, z2);
	std::complex<float> hpA = model.highA.response(z1, z2);
	std::complex<float> lpB = model.lowB.response(z1, z2);
	std::complex<float> bpB = model.bandB.response(z1, z2);
	std::complex<float> hpB = model.highB.response(z1, z2);
	const std::complex<float> ntA = lpA + hpA, ntB = lpB + hpB, cascadeLp = lpB * lpA, cascadeNotch = ntB * ntA, cascadeNotchToLow = lpB * ntA, cascadeHpToLp = lpB * hpA, cascadeHighToNotch = ntB * hpA, cascadeHpToHp = hpB * hpA;
	return combineModeResponse<std::complex<float>>(model.mode, lpA, bpA, hpA, ntA, lpB, bpB, hpB, ntB, cascadeLp, cascadeNotch, cascadeNotchToLow, cascadeHpToLp, cascadeHighToNotch, cascadeHpToHp, model.wA, model.wB);
}

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return 20.f * std::log10(std::max(mag, 1e-5f));
}

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex) {
	if (sampleIndex < 0) return 0.f;
	const float sampleRate = std::max(state.sampleRate, 1.f);
	const float phase = 261.63f * float(sampleIndex) / sampleRate;
	const float phase01 = phase - std::floor(phase);
	return 5.f * undertow_shape::thresholdFold(phase01, 0.5f, false, 0.5f, false);
}

namespace {

float undertowPreviewAtan(float x) {
	const float ax = std::fabs(x);
	if (ax <= 1.f) {
		return x * (0.78539816339f + 0.273f * (1.f - ax));
	}
	const float inv = 1.f / ax;
	const float t = inv * (0.78539816339f + 0.273f * (1.f - inv));
	return (x >= 0.f) ? (1.57079632679f - t) : (-1.57079632679f + t);
}

float undertowPreviewAnalogCharacter(float x, float env) {
	const float drive = 1.02f + 0.18f * clamp(env, 0.f, 1.f);
	const float signDrive = x >= 0.f ? (drive + 0.02f) : (drive - 0.02f);
	const float norm = std::max(undertowPreviewAtan(signDrive), 1e-6f);
	return undertowPreviewAtan(x * signDrive) / norm;
}

void generateUndertowBrowserPreview(float* buffer, int sampleCount, float sampleRate) {
	if (!buffer || sampleCount <= 0) return;
	const float sr = std::max(sampleRate, 1.f);
	const float sampleTime = 1.f / sr;
	constexpr float baseFrequencyHz = 261.63f;
	constexpr float linearFmAmount = 0.5f;
	constexpr float shape = 0.5f;
	constexpr float edgeHardness = 0.5f;
	float phase = 0.f;
	float linearFmHpState = 0.f;
	float characterEnv = 0.f;
	bool subHigh = false;
	const float hpCoeff = clamp(1.f - 2.f * kPi * 4.9f * sampleTime, 0.f, 1.f);
	const float attackCoeff = clamp(sampleTime / (0.002f + sampleTime), 0.f, 1.f);
	const float releaseCoeff = clamp(sampleTime / (0.050f + sampleTime), 0.f, 1.f);

	for (int i = 0; i < sampleCount; ++i) {
		// This mirrors the inspected patch: Undertow Sub self-patches its linear-FM
		// input at 50%, while Morph (also 50%) feeds Bifurx.
		const float subVoltage = subHigh ? 5.f : -5.f;
		const float linearFm = subVoltage - linearFmHpState;
		linearFmHpState = subVoltage - hpCoeff * linearFm;
		const float linearBus = linearFm * linearFmAmount * 0.10f;
		const float frequency = clamp(baseFrequencyHz + baseFrequencyHz * linearBus, 8.f, 20000.f);
		phase += frequency * sampleTime;
		if (phase >= 1.f) {
			phase -= std::floor(phase);
			subHigh = !subHigh;
		}

		const float triangle = 4.f * std::fabs(phase - 0.5f) - 1.f;
		const float sine = undertow_shape::triToSine(triangle);
		const float shaped = undertow_shape::thresholdFold(phase, shape, false, edgeHardness, false);
		const float envTarget = 0.5f * (std::fabs(sine) + std::fabs(shaped));
		const float envCoeff = envTarget > characterEnv ? attackCoeff : releaseCoeff;
		characterEnv += (envTarget - characterEnv) * envCoeff;
		buffer[i] = clamp(5.f * undertowPreviewAnalogCharacter(shaped, characterEnv), -5.f, 5.f);
	}
}

} // namespace

SvfOutputs processProbeStage(BifurxProbeEngineState& state, int stageIndex, float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm, bool highResonanceSelfOscEnabled) {
	TptSvf& core = (stageIndex == 0) ? state.svfA : state.svfB;
	return processCharacterStage(core, stageIndex, input, sampleRate, cutoff, damping, drive, resoNorm, highResonanceSelfOscEnabled, nullptr);
}

void simulatePreviewProbeResponse(const BifurxPreviewState& state, float* inputBuffer, float* outputBuffer, int sampleCount) {
	if (!inputBuffer || !outputBuffer || sampleCount <= 0) return;
	BifurxProbeEngineState engine;
	const float sampleRate = std::max(state.sampleRate, 1.f), freqA = clamp(state.freqA, kFreqMinHz, 0.46f * sampleRate), freqB = clamp(state.freqB, kFreqMinHz, 0.46f * sampleRate), dampingA = clamp(1.f / std::max(state.qA, 0.05f), 0.02f, 2.2f), dampingB = clamp(1.f / std::max(state.qB, 0.05f), 0.02f, 2.2f), lowW = signedWeight(state.balance, false), highW = signedWeight(state.balance, true), norm = 2.f / (lowW + highW), wA = lowW * norm, wB = highW * norm, drive = levelDriveGain(kPreviewProbeLevelKnob);
	const int mode = clamp(state.mode, 0, kBifurxModeCount - 1);
	generateUndertowBrowserPreview(inputBuffer, sampleCount, sampleRate);
	for (int i = 0; i < sampleCount; ++i) {
		const float rawIn = inputBuffer[i], excitation = applyLevelInputStage(rawIn, kPreviewProbeLevelKnob);
		const SvfOutputs a = processProbeStage(engine, 0, excitation, sampleRate, freqA, dampingA, drive, state.resoNorm, true);
		SvfOutputs b; float modeOut = 0.f;
		switch (mode) {
			case 0: b = processProbeStage(engine, 1, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 1:
			case 4:
			case 5:
			case 8: b = processProbeStage(engine, 1, excitation, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 2: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB); break;
			case 3: b = processProbeStage(engine, 1, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
			case 6: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB); break;
			case 7: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB); break;
			case 9: b = processProbeStage(engine, 1, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, b.hp, wA, wB); break;
			default: b = processProbeStage(engine, 1, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm, true); modeOut = combineModeResponse<float>(0, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); break;
		}
		inputBuffer[i] = excitation; outputBuffer[i] = applyLevelOutputStage(modeOut, kPreviewProbeLevelKnob);
	}
}

static std::atomic<uint32_t> gBifurxDebugInstanceCounter{1u};

Bifurx::Bifurx() {
	debugInstanceId = gBifurxDebugInstanceCounter.fetch_add(1u, std::memory_order_relaxed);
	createdUnixTimeSec = system::getUnixTime();
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(MODE_PARAM, 0.f, float(kBifurxUiModeCount - 1), 0.f, "Mode", {
		kBifurxModeLabels[0],
		kBifurxModeLabels[1],
		kBifurxModeLabels[2],
		kBifurxModeLabels[3],
		kBifurxModeLabels[4],
		kBifurxModeLabels[5],
		kBifurxModeLabels[6],
		kBifurxModeLabels[7],
		kBifurxModeLabels[8],
		kBifurxModeLabels[9],
		kBifurxModeLabels[10]
	});
	configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level"); configParam<BifurxFreqQuantity>(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency"); configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance"); configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance"); configParam<BifurxSpanQuantity>(SPAN_PARAM, 0.f, 1.f, 0.33f, "Span"); configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount"); configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator"); configParam(TITO_PARAM, -1.f, 1.f, 0.f, "TITO strength"); configButton(MODE_LEFT_PARAM, "Mode previous"); configButton(MODE_RIGHT_PARAM, "Mode next"); configButton(MODE_MENU_PARAM, "Filter mode");
	configInput(IN_INPUT, "Signal In"); configInput(VOCT_INPUT, "V/Oct"); configInput(FM_INPUT, "FM"); configInput(RESO_CV_INPUT, "Resonance CV"); configInput(BALANCE_CV_INPUT, "Balance CV"); configInput(SPAN_CV_INPUT, "Span CV"); configOutput(OUT_OUTPUT, "Signal Out"); configBypass(IN_INPUT, OUT_OUTPUT);
	outputs[OUT_OUTPUT].setChannels(1);
	paramQuantities[MODE_PARAM]->snapEnabled = true;
	previewPublishDivider.setDivision(kPreviewPublishFastDivision); previewPublishSlowDivider.setDivision(kPreviewPublishSlowDivision); controlUpdateDivider.setDivision(controlUpdateDivision); perfMeasureDivider.setDivision(kPerfMeasureDivision);
}

Bifurx::~Bifurx() {
	teardownTimer.begin(id);
}

void Bifurx::resetAnalysisCapture() {
	for (int captureIndex = 0; captureIndex < 2; ++captureIndex) {
		const int slot = analysisCaptureSlots[captureIndex];
		if (slot >= 0 && slot < kAnalysisFrameSlotCount) {
			analysisFrameReaders[slot].store(0u, std::memory_order_release);
		}
	}
	analysisCaptureSlots[0] = -1;
	analysisCaptureSlots[1] = -1;
	analysisCapturePositions[0] = 0;
	analysisCapturePositions[1] = 0;
	analysisCaptureCountdown = 0;
}

void Bifurx::subscribeAnalysisVisual() {
	analysisVisualSubscribers.fetch_add(1u, std::memory_order_release);
}

void Bifurx::unsubscribeAnalysisVisual() {
	uint32_t subscribers = analysisVisualSubscribers.load(std::memory_order_acquire);
	while (subscribers > 0u && !analysisVisualSubscribers.compare_exchange_weak(
		subscribers,
		subscribers - 1u,
		std::memory_order_acq_rel,
		std::memory_order_acquire
	)) {
	}
}

void Bifurx::resetCircuitStates() {
	coreA = TptSvf {};
	coreB = TptSvf {};
	cachedCoeffsA = SvfCoeffs {};
	cachedCoeffsB = SvfCoeffs {};
	titoCoeffsA = SvfCoeffs {};
	titoCoeffsB = SvfCoeffs {};
	titoCoeffFreqA = 0.f;
	titoCoeffFreqB = 0.f;
	titoCoeffDampingA = 0.f;
	titoCoeffDampingB = 0.f;
	titoCoeffSampleRateA = 0.f;
	titoCoeffSampleRateB = 0.f;
	selfOscCoeffsA = SvfCoeffs {};
	selfOscCoeffsB = SvfCoeffs {};
	selfOscCoeffFreqA = 0.f;
	selfOscCoeffFreqB = 0.f;
	selfOscCoeffDampingA = 0.f;
	selfOscCoeffDampingB = 0.f;
	selfOscCoeffSampleRateA = 0.f;
	selfOscCoeffSampleRateB = 0.f;
	cachedFrequencyRangeSampleRate = 0.f;
	cachedFrequencyRangeOctaves = 0.f;
	cachedFreqParamNorm = -1.f;
	cachedVoctCv = 0.f;
	cachedFm = 0.f;
	cachedPitchSampleRate = 0.f;
	cachedCharacterState = CharacterStageState {};
	cachedCharacterDrive = 0.f;
	cachedCharacterResoNorm = 0.f;
	cachedCharacterHighResEnabled = false;
	cachedCharacterStateValid = false;
	controlFastCacheValid = false;

	llTelemetryExcitationSq = 0.f;
	llTelemetryStageALpSq = 0.f;
	llTelemetryStageBLpSq = 0.f;
	llTelemetryOutputSq = 0.f;
	previewFilterInitialized = false;
	previewTargetMotionInitialized = false;
	previewTargetStillSamples = 0;
	previewSampleAccum = 0;
	previewAdaptiveCooldown = 0;
	hasLastPreviewState = false;
	modeLeftTrigger.reset();
	modeRightTrigger.reset();
	previewPublishDivider.reset();
	previewPublishSlowDivider.reset();
	controlUpdateDivider.reset();
	perfMeasureDivider.reset();
	resetAnalysisCapture();
}

void Bifurx::onReset() {
	resetCircuitStates();
}
json_t* Bifurx::dataToJson() {
	json_t* root = json_object();
	json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic.load(std::memory_order_relaxed)));
	json_object_set_new(root, "showModuleResponseOverlay", json_boolean(showModuleResponseOverlay.load(std::memory_order_relaxed)));
	json_object_set_new(root, "colorScheme", json_integer(colorScheme));
	json_object_set_new(root, "threeColorFftGradient", json_boolean(threeColorFftGradient.load(std::memory_order_relaxed)));
	json_object_set_new(root, "legacyVisuals", json_boolean(legacyVisuals.load(std::memory_order_relaxed)));
	json_object_set_new(root, "useGlShaderRenderer", json_boolean(useGlShaderRenderer.load(std::memory_order_relaxed)));
	json_object_set_new(root, "lowLatencyVisual", json_boolean(lowLatencyVisual.load(std::memory_order_relaxed)));
	json_object_set_new(root, "visualWorkerMode", json_integer(visualWorkerMode.load(std::memory_order_relaxed)));
	json_object_set_new(root, "modulationQualityMode", json_integer(modulationQualityMode.load(std::memory_order_relaxed)));
	json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging.load(std::memory_order_relaxed)));
	json_object_set_new(root, "perfDebugLogging", json_boolean(perfDebugLogging.load(std::memory_order_relaxed)));
	json_object_set_new(root, "highResonanceSelfOscEnabled", json_boolean(highResonanceSelfOscEnabled.load(std::memory_order_relaxed)));
	json_object_set_new(root, "softLimitingEnabled", json_boolean(softLimitingEnabled.load(std::memory_order_relaxed)));
	json_object_set_new(root, "renderMode", json_integer(renderMode));
	json_object_set_new(root, "createdUnixTimeSec", json_real(createdUnixTimeSec));
	return root;
}

void Bifurx::dataFromJson(json_t* root) {
	if (!root) {
		return;
	}
	Module::dataFromJson(root);
	json_t* fftScaleDynamicJ = json_object_get(root, "fftScaleDynamic");
	if (fftScaleDynamicJ) {
		fftScaleDynamic.store(json_is_true(fftScaleDynamicJ), std::memory_order_relaxed);
	}
	json_t* showModuleResponseOverlayJ = json_object_get(root, "showModuleResponseOverlay");
	if (showModuleResponseOverlayJ) {
		showModuleResponseOverlay.store(json_is_true(showModuleResponseOverlayJ), std::memory_order_relaxed);
	}
	json_t* colorSchemeJ = json_object_get(root, "colorScheme");
	if (colorSchemeJ) {
		colorScheme = (ColorScheme) clamp(int(json_integer_value(colorSchemeJ)), 0, SCHEME_LEN - 1);
	}
	json_t* threeColorFftGradientJ = json_object_get(root, "threeColorFftGradient");
	if (threeColorFftGradientJ) {
		threeColorFftGradient.store(json_is_true(threeColorFftGradientJ), std::memory_order_relaxed);
	}
	json_t* legacyVisualsJ = json_object_get(root, "legacyVisuals");
	if (legacyVisualsJ) {
		legacyVisuals.store(json_is_true(legacyVisualsJ), std::memory_order_relaxed);
	}
	json_t* useGlShaderRendererJ = json_object_get(root, "useGlShaderRenderer");
	if (useGlShaderRendererJ) {
		useGlShaderRenderer.store(json_is_true(useGlShaderRendererJ), std::memory_order_relaxed);
	}
	json_t* lowLatencyVisualJ = json_object_get(root, "lowLatencyVisual");
	if (lowLatencyVisualJ) {
		lowLatencyVisual.store(json_is_true(lowLatencyVisualJ), std::memory_order_relaxed);
	}
	json_t* visualWorkerModeJ = json_object_get(root, "visualWorkerMode");
	if (visualWorkerModeJ) {
		const int mode = int(json_integer_value(visualWorkerModeJ));
		visualWorkerMode.store(clamp(mode, VISUAL_WORKER_INHERIT, VISUAL_WORKER_ON), std::memory_order_relaxed);
	}
	json_t* modulationQualityModeJ = json_object_get(root, "modulationQualityMode");
	if (modulationQualityModeJ) {
		modulationQualityMode.store(clamp(int(json_integer_value(modulationQualityModeJ)), MOD_QUALITY_BALANCED, MOD_QUALITY_COUNT - 1), std::memory_order_relaxed);
		controlFastCacheValid = false;
	}
	else {
		// Backward compatibility with old two-state control update mode.
		json_t* controlUpdateModeJ = json_object_get(root, "controlUpdateMode");
		if (controlUpdateModeJ) {
			const int legacyMode = int(json_integer_value(controlUpdateModeJ));
			modulationQualityMode.store((legacyMode <= 0) ? MOD_QUALITY_BALANCED : MOD_QUALITY_EXACT, std::memory_order_relaxed);
			controlFastCacheValid = false;
		}
	}
	json_t* curveDebugLoggingJ = json_object_get(root, "curveDebugLogging");
	if (curveDebugLoggingJ) {
		curveDebugLogging.store(json_is_true(curveDebugLoggingJ), std::memory_order_relaxed);
	}
	json_t* perfDebugLoggingJ = json_object_get(root, "perfDebugLogging");
	if (perfDebugLoggingJ) {
		perfDebugLogging.store(json_is_true(perfDebugLoggingJ), std::memory_order_relaxed);
	}
	json_t* highResonanceSelfOscEnabledJ = json_object_get(root, "highResonanceSelfOscEnabled");
	if (highResonanceSelfOscEnabledJ) {
		highResonanceSelfOscEnabled.store(json_is_true(highResonanceSelfOscEnabledJ), std::memory_order_relaxed);
	}
	json_t* softLimitingEnabledJ = json_object_get(root, "softLimitingEnabled");
	if (softLimitingEnabledJ) {
		softLimitingEnabled.store(json_is_true(softLimitingEnabledJ), std::memory_order_relaxed);
	}
	json_t* createdUnixTimeSecJ = json_object_get(root, "createdUnixTimeSec");
	if (createdUnixTimeSecJ && json_is_number(createdUnixTimeSecJ)) {
		const double loadedCreatedUnixTimeSec = json_number_value(createdUnixTimeSecJ);
		if (std::isfinite(loadedCreatedUnixTimeSec) && loadedCreatedUnixTimeSec > 0.0) {
			createdUnixTimeSec = loadedCreatedUnixTimeSec;
		}
	}

	auto decodeRenderMode = [](int rawRenderMode) {
		// Keep compatibility with earlier enum encodings where OpenGL could be 2 (or higher in migrated values).
		switch (rawRenderMode) {
			case RENDER_OPENGL:
			case 2:
			case 5:
			case 6:
				return RENDER_OPENGL;
			default:
				return RENDER_NANOVG;
		}
	};

	bool loadedRenderMode = false;
	json_t* renderModeJ = json_object_get(root, "renderMode");
	if (renderModeJ) {
		renderMode = (RenderMode) decodeRenderMode(int(json_integer_value(renderModeJ)));
		loadedRenderMode = true;
	}
	if (!loadedRenderMode) {
		// Legacy key used by older renderer debug menus.
		json_t* legacyRenderModeJ = json_object_get(root, "debugRenderMode");
		if (legacyRenderModeJ) {
			renderMode = (RenderMode) decodeRenderMode(int(json_integer_value(legacyRenderModeJ)));
		}
	}
	// Legacy key retained for backward patch compatibility.
	// Bifurx is SVF-only, so this key is intentionally ignored if present.
	json_t* legacyFilterCircuitModeJ = json_object_get(root, "filterCircuitMode");
	if (legacyFilterCircuitModeJ) {
		// Intentionally ignored.
	}
}
void Bifurx::resetPerfStats() { perfAudioSampledCount.store(0, std::memory_order_release); perfAudioProcessNs.store(0, std::memory_order_release); perfAudioProcessRangeMinNs.store(std::numeric_limits<uint64_t>::max(), std::memory_order_release); perfAudioProcessRangeMaxNs.store(0, std::memory_order_release); perfAudioControlsNs.store(0, std::memory_order_release); perfAudioCoreNs.store(0, std::memory_order_release); perfAudioPreviewNs.store(0, std::memory_order_release); perfAudioAnalysisNs.store(0, std::memory_order_release); perfAudioProcessMaxNs.store(0, std::memory_order_release); }
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
	if (analysisVisualSubscribers.load(std::memory_order_acquire) == 0u) {
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
void Bifurx::onSampleRateChange(const SampleRateChangeEvent& e) {
	controlFastCacheValid = false;
	cachedFrequencyRangeSampleRate = 0.f;
	cachedPitchSampleRate = 0.f;
	previewFilterInitialized = false;
	previewSampleAccum = 0;
	resetAnalysisCapture();
	const float sampleRate = std::max(e.sampleRate, 1.f);
	llTelemetryAlpha = onePoleAlpha(1.f / sampleRate, kLlTelemetryTauSeconds);
	llTelemetryAlphaSampleRate = sampleRate;
}

float BifurxFreqQuantity::getDisplayValue() {
	return bifurxFrequencyHzFromParam(getValue());
}

void BifurxFreqQuantity::setDisplayValue(float displayValue) {
	setImmediateValue(bifurxParamFromFrequencyHz(displayValue));
}

std::string BifurxFreqQuantity::getDisplayValueString() {
	const float hz = getDisplayValue();
	if (hz >= 1000.f) {
		return string::f("%.2f kHz", hz / 1000.f);
	}
	if (hz < 10.f) {
		return string::f("%.2f Hz", hz);
	}
	return string::f("%.1f Hz", hz);
}

float BifurxSpanQuantity::getDisplayValue() {
	return bifurxSpanSemitonesFromParam(getValue());
}

void BifurxSpanQuantity::setDisplayValue(float displayValue) {
	setImmediateValue(bifurxParamFromSpanSemitones(displayValue));
}

std::string BifurxSpanQuantity::getDisplayValueString() {
	return string::f("%.1f st", getDisplayValue());
}

void Bifurx::process(const ProcessArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	const bool measurePerf = isDragonKingDebugEnabled() && perfMeasureDivider.process();
	const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfCoreStart, perfPreviewStart, perfAnalysisStart;

	const float in = bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()), level = params[LEVEL_PARAM].getValue(), drive = levelDriveGain(level);
	const float tito = clamp(params[TITO_PARAM].getValue(), -1.f, 1.f);
	const float titoAbs = std::fabs(tito);
	const bool titoNeutral = titoAbs < 0.02f;
	const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	// V/Oct is a calibrated pitch input, so follow it directly. Any glide is an
	// explicit patching choice rather than an undocumented module behavior.
	const float voctCv = voctConnected ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
	const bool fmConnected = inputs[FM_INPUT].isConnected();
	const bool resoCvConnected = inputs[RESO_CV_INPUT].isConnected();
	const bool balanceCvConnected = inputs[BALANCE_CV_INPUT].isConnected();
	const bool spanCvConnected = inputs[SPAN_CV_INPUT].isConnected();
	const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f), fmCv = fmConnected ? clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) : 0.f, fm = fmCv * fmAmt;
	const bool slowCvConnected = resoCvConnected || balanceCvConnected || spanCvConnected;
	const bool audioRateControlsActive = voctConnected || fmConnected;
	const bool fastPathEligible = titoNeutral && !voctConnected && !fmConnected && !slowCvConnected;
	int targetControlDivision = 16;
	float titoCoeffRelativeThreshold = kTitoCoeffRelativeUpdateThreshold;
	float titoCoeffAbsoluteThresholdHz = kTitoCoeffAbsoluteUpdateThresholdHz;
	const int modulationQualityModeNow = modulationQualityMode.load(std::memory_order_relaxed);
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
		const float resoCvNorm = resoCvConnected ? clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f : 0.f;
		const float nextResoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f);
		const float balanceCvNorm = balanceCvConnected ? clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f : 0.f;
		const float nextBalanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f);
		const float nextSpanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f);
		const float nextSpanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f);
		const float nextSpanCvNorm = spanCvConnected ? clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f : 0.f;
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

	const float titoModeScale = 1.22f, titoStrength = 2.4f * titoAbs, couplingDepth = titoStrength * titoModeScale * (0.026f + 0.28f * resoNorm * resoNorm);
	const float drivenIn = applyLevelInputStage(in, level);
	const bool highResonanceSelfOscEnabledNow = cachedHighResonanceSelfOscEnabled;
	if (!cachedCharacterStateValid
		|| drive != cachedCharacterDrive
		|| resoNorm != cachedCharacterResoNorm
		|| highResonanceSelfOscEnabledNow != cachedCharacterHighResEnabled
	) {
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
	if (!titoNeutral) {
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
	if (character.selfOscillating) {
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
	if (measurePerf) perfCoreStart = PerfClock::now();
	float modeOut = 0.f, llExc = 0.f, llA = 0.f, llB = 0.f;
	auto pA = [&](float s) {
		return processCharacterStagePrepared(coreA, s, *coeffsAForSample, selfOscCoeffsAForSample, character);
	};
	auto pB = [&](float s) {
		return processCharacterStagePrepared(coreB, s, *coeffsBForSample, selfOscCoeffsBForSample, character);
	};

	const bool displayOnlyMode = isBifurxDisplayOnlyMode(mode);
	if (displayOnlyMode) {
		modeOut = in;
	}
	else {
		switch (mode) {
			case 0: { const SvfOutputs a = pA(excitation), b = pB(a.lp); llExc = excitation; llA = a.lp; llB = b.lp; modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 1: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 2: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, 0.f, 0.f, wA, wB); } break;
			case 3: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 4: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 5: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 6: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.lp, 0.f, 0.f, wA, wB); } break;
			case 7: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, b.notch, 0.f, wA, wB); } break;
			case 8: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
			case 9: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = b.hp; } break;
			default: { const SvfOutputs a = pA(excitation), b = pB(a.lp); modeOut = combineModeResponse<float>(0, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, 0.f, 0.f, wA, wB); } break;
		}
	}

	const bool softLimitingEnabledNow = cachedSoftLimitingEnabled;
	const float out = displayOnlyMode ? in : applyLevelOutputStage(modeOut, level, softLimitingEnabledNow);
	outputs[OUT_OUTPUT].setVoltage(out);
	const float llAlpha = llTelemetryAlpha;
	if (mode == 0) { llTelemetryExcitationSq += llAlpha * (llExc * llExc - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (llA * llA - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (llB * llB - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	else { llTelemetryExcitationSq += llAlpha * (0.f - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (0.f - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (0.f - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	if (measurePerf) perfPreviewStart = PerfClock::now();

	const bool pPitchCvConn = voctConnected || fmConnected;
	if (previewAdaptiveCooldown > 0) previewAdaptiveCooldown--;
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
		const float effectiveAlpha = 1.f - std::pow(oneMinusAlpha, float(elapsedSamples));
		if (!previewTargetMotionInitialized) { previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB; previewTargetStillSamples = 0; previewTargetMotionInitialized = true; }
		const float tMAOct = std::fabs(fastLog2(std::max(pTFqA, 1.f)) - fastLog2(std::max(previewPrevTargetFreqA, 1.f)));
		const float tMBOct = std::fabs(fastLog2(std::max(pTFqB, 1.f)) - fastLog2(std::max(previewPrevTargetFreqB, 1.f)));
		const float tMOct = std::max(tMAOct, tMBOct);
		if (tMOct <= kPreviewInstantSettleMotionOctThreshold) previewTargetStillSamples += elapsedSamples; else previewTargetStillSamples = 0;
		const bool pInstSettle = (previewTargetStillSamples >= kPreviewInstantSettleHoldSamples);
		previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB;
		if (!previewFilterInitialized || pInstSettle) { previewFreqAFiltered = pTFqA; previewFreqBFiltered = pTFqB; previewQAFiltered = pTQA; previewQBFiltered = pTQB; previewBalanceFiltered = pTBal; previewFilterInitialized = true; }
		else { const float a = effectiveAlpha; previewFreqAFiltered += a * (pTFqA - previewFreqAFiltered); previewFreqBFiltered += a * (pTFqB - previewFreqBFiltered); previewQAFiltered += a * (pTQA - previewQAFiltered); previewQBFiltered += a * (pTQB - previewQBFiltered); previewBalanceFiltered += a * (pTBal - previewBalanceFiltered); }

		BifurxPreviewState pS; pS.sampleRate = args.sampleRate; pS.freqA = previewFreqAFiltered; pS.freqB = previewFreqBFiltered; pS.qA = previewQAFiltered; pS.qB = previewQBFiltered; pS.mode = mode; pS.balance = previewBalanceFiltered; pS.balanceTarget = balanceNorm; pS.resoNorm = resoNorm; pS.spanParamNorm = spanParamNorm; pS.spanCvNorm = spanCvNorm; pS.spanAtten = spanAtten; pS.spanNorm = spanNorm; pS.spanOct = spanOct; pS.freqParamNorm = freqParamNorm; pS.voctCv = voctCv;
		bool adpTick = false;
		if (hasLastPreviewState && previewAdaptiveCooldown <= 0 && perTick) {
			const float fMA = std::fabs(fastLog2(std::max(pS.freqA, 1.f)) - fastLog2(std::max(lastPreviewState.freqA, 1.f))), fMB = std::fabs(fastLog2(std::max(pS.freqB, 1.f)) - fastLog2(std::max(lastPreviewState.freqB, 1.f))), sMO = std::fabs(pS.spanOct - lastPreviewState.spanOct), qMA = std::fabs(pS.qA - lastPreviewState.qA), qMB = std::fabs(pS.qB - lastPreviewState.qB), bM = std::fabs(pS.balance - lastPreviewState.balance);
			if (fMA > kPreviewAdaptiveOctaveThreshold || fMB > kPreviewAdaptiveOctaveThreshold || sMO > kPreviewAdaptiveSpanOctThreshold || qMA > kPreviewAdaptiveQThreshold || qMB > kPreviewAdaptiveQThreshold || bM > kPreviewAdaptiveBalanceThreshold) { adpTick = true; previewAdaptiveCooldown = kPreviewAdaptiveCooldownSamples; }
		}
		if (!hasLastPreviewState || ((perTick || adpTick) && previewStatesDiffer(pS, lastPreviewState))) publishPreviewState(pS);
		if (perTick || adpTick) { BifurxLlTelemetryState llTS; llTS.active = (mode == 0); llTS.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f)); llTS.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f)); llTS.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f)); llTS.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f)); llTS.stageBLpOverALpDb = amplitudeRatioDb(llTS.stageBLpRms, llTS.stageALpRms); llTS.outputOverInputDb = amplitudeRatioDb(llTS.outputRms, llTS.excitationRms); publishLlTelemetryState(llTS); }
	}
	if (measurePerf) perfAnalysisStart = PerfClock::now();
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
		const PerfClock::time_point pE = PerfClock::now();
		const uint64_t cNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfCoreStart - perfStart).count();
		const uint64_t crNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfPreviewStart - perfCoreStart).count();
		const uint64_t prNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfAnalysisStart - perfPreviewStart).count();
		const uint64_t aNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfAnalysisStart).count(), pNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfStart).count();
		perfAudioSampledCount.fetch_add(1, std::memory_order_relaxed); perfAudioProcessNs.fetch_add(pNS, std::memory_order_relaxed);
		debug_terminal::recordAudioProcessTiming(perfAudioProcessRangeMinNs, perfAudioProcessRangeMaxNs, pNS);
		perfAudioControlsNs.fetch_add(cNS, std::memory_order_relaxed); perfAudioCoreNs.fetch_add(crNS, std::memory_order_relaxed);
		perfAudioPreviewNs.fetch_add(prNS, std::memory_order_relaxed); perfAudioAnalysisNs.fetch_add(aNS, std::memory_order_relaxed);
		uint64_t pM = perfAudioProcessMaxNs.load(std::memory_order_relaxed);
		while (pNS > pM && !perfAudioProcessMaxNs.compare_exchange_weak(pM, pNS, std::memory_order_relaxed));
	}
}

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

inline float computeDisplayTopTargetDbfs(
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
	for (int i = 0; i < kCurvePointCount; i++) sortedOutputDbfs[i] = frameSmoothedOutputDbfs[i];
	const int p95Index = int(0.95f * float(kCurvePointCount - 1));
	std::nth_element(sortedOutputDbfs, sortedOutputDbfs + p95Index, sortedOutputDbfs + kCurvePointCount);
	const float robustTopRefDbfs = std::max(sortedOutputDbfs[p95Index], framePeakDbfs - 18.f);
	return clamp(
		std::max(robustTopRefDbfs + 6.f, framePeakDbfs + kDisplayPeakHeadroomDb),
		kDisplayTopDbfsFloor,
		kDisplayTopDynamicCeilingDbfs
	);
}

inline void prepareOverlayTargetsFromSpectra(
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
	const float amplitudeScale = 4.f / float(kFftSize);
	const float amplitudeScaleSq = amplitudeScale * amplitudeScale;
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
				responseOutputEnergy += w * binOutputPower[sampleBin];
				rawInputEnergy += w * binRawInputPower[sampleBin];
			}
		}
		binModuleDeltaDb[bin] = moduleResponseEnabled
			? 10.f * std::log10((responseOutputEnergy + 1e-12f) / (rawInputEnergy + 1e-12f))
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
		const int left = std::max(0, i - 1), right = std::min(kCurvePointCount - 1, i + 1);
		const float smoothOutputDbfs = 0.12f * sampledOutputDbfs[left] + 0.76f * sampledOutputDbfs[i] + 0.12f * sampledOutputDbfs[right];
		frameSmoothedOutputDbfs[i] = smoothOutputDbfs;
		const float smoothModuleDeltaDb = 0.12f * sampledModuleDeltaDb[left] + 0.76f * sampledModuleDeltaDb[i] + 0.12f * sampledModuleDeltaDb[right];
		overlayTargetModuleDb[i] = mixf(overlayTargetModuleDb[i], smoothModuleDeltaDb, targetSmoothing);
		overlayTargetOutputDbfs[i] = mixf(overlayTargetOutputDbfs[i], smoothOutputDbfs, targetSmoothing);
	}

	*displayTopTargetDbfs = computeDisplayTopTargetDbfs(frameSmoothedOutputDbfs, overlayTargetOutputDbfs, fftScaleDynamic);
}

} // namespace

void BifurxSpectrumBase::syncBase() {
	if (!module) return;
	const bool useWorkerCurve = shouldUseVisualWorker();
	if (useWorkerCurve) {
		ensureWorkerRegistration();
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
		state.previewState = previewState;
		state.previewPublishTimeSec = previewPublishTimeSec;
		state.hasPreview = true;
		state.lastPreviewSeq = previewSeq;
		if (!useWorkerCurve) {
			updateAxisCache();
			const bool measurePrep = isDragonKingDebugEnabled();
			const auto curvePrepStart = measurePrep ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
			updateCurveCache();
			if (measurePrep) {
				lastCurvePrepUs = float(std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::steady_clock::now() - curvePrepStart).count());
			}
		}
	}

	const uint32_t analysisSeq = module->analysisPublishSeq.load(std::memory_order_acquire);
	if (analysisSeq != state.lastAnalysisSeq) {
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
				state.hasOverlay = true;
			}
		}
		else {
			state.lastAnalysisSeq = analysisSeq;
		}
	}

	if (useWorkerCurve) {
		submitWorkerCurveRequest();
	}
}

BifurxSpectrumBase::~BifurxSpectrumBase() {
	releaseWorkerRegistration();
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
	if (!workerSnapshotCache || workerSnapshotCache->completedAtSec <= 0.0) {
		return 0.f;
	}
	// Report age only when the rendered snapshot is behind the most recent published state.
	const bool previewBehind = workerLastAppliedPreviewSeq < state.lastPreviewSeq;
	const bool analysisBehind = workerLastAppliedAnalysisSeq < state.lastAnalysisSeq;
	if (!previewBehind && !analysisBehind) {
		return 0.f;
	}
	const double ageSec = std::max(0.0, system::getTime() - workerSnapshotCache->completedAtSec);
	return float(ageSec * 1000.0);
}

float BifurxSpectrumBase::workerQueueLatencyMs() const {
	if (!workerSnapshotCache || workerSnapshotCache->requestSubmittedAtSec <= 0.0 || workerSnapshotCache->completedAtSec <= 0.0) {
		return 0.f;
	}
	const double queueSec = std::max(0.0, workerSnapshotCache->completedAtSec - workerSnapshotCache->requestSubmittedAtSec);
	return float(queueSec * 1000.0);
}

void BifurxSpectrumBase::ensureWorkerRegistration() {
	if (workerDisplayId != 0) {
		return;
	}
	BifurxUiRenderService& service = bifurxRenderService();
	service.start();
	workerDisplayId = service.registerDisplay();
}

void BifurxSpectrumBase::releaseWorkerRegistration() {
	if (workerDisplayId != 0) {
		bifurxRenderService().unregisterDisplay(workerDisplayId);
	}
	workerDisplayId = 0;
	workerRequestSeq = 0;
	workerLastAppliedRequestSeq = 0;
	workerLastSubmittedPreviewSeq = 0;
	workerLastAppliedPreviewSeq = 0;
	workerLastSubmittedAnalysisSeq = 0;
	workerLastAppliedAnalysisSeq = 0;
	workerSnapshotCache.reset();
	for (auto& analysisFrame : workerAnalysisFramePool) {
		analysisFrame.reset();
	}
	workerAnalysisFramePoolCursor = 0;
	lastWorkerSubmitUs = 0.f;
}

std::shared_ptr<BifurxUiRenderPayload> BifurxSpectrumBase::acquireWorkerAnalysisFrame() {
	for (size_t attempt = 0; attempt < kWorkerAnalysisFramePoolSize; ++attempt) {
		const size_t index = (workerAnalysisFramePoolCursor + attempt) % kWorkerAnalysisFramePoolSize;
		auto& frame = workerAnalysisFramePool[index];
		if (!frame) {
			frame = std::make_shared<BifurxUiRenderPayload>();
		}
		if (frame.use_count() == 1) {
			workerAnalysisFramePoolCursor = (index + 1) % kWorkerAnalysisFramePoolSize;
			return frame;
		}
	}
	return nullptr;
}

void BifurxSpectrumBase::submitWorkerCurveRequest() {
	if (!module || workerDisplayId == 0 || !state.hasPreview) {
		return;
	}
	if (workerLastSubmittedPreviewSeq == state.lastPreviewSeq &&
		workerLastSubmittedAnalysisSeq == state.lastAnalysisSeq) {
		return;
	}
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto submitStart = measurePerf
		? std::chrono::steady_clock::now()
		: std::chrono::steady_clock::time_point();
	BifurxUiRenderRequest request;
	request.displayId = workerDisplayId;
	request.requestSeq = ++workerRequestSeq;
	request.previewSeq = state.lastPreviewSeq;
	request.analysisSeq = workerLastSubmittedAnalysisSeq;
	request.requestSubmittedAtSec = system::getTime();
	request.sourcePreviewTimeSec = state.previewPublishTimeSec;
	request.previewState = state.previewState;
	request.fftScaleDynamic = module->fftScaleDynamic.load(std::memory_order_relaxed);
	request.showModuleResponseOverlay = module->showModuleResponseOverlay.load(std::memory_order_relaxed);
	const bool analysisChangedSinceSubmit =
		(state.lastAnalysisSeq != 0) && (state.lastAnalysisSeq != workerLastSubmittedAnalysisSeq);
	if (analysisChangedSinceSubmit) {
		std::shared_ptr<BifurxUiRenderPayload> payload = acquireWorkerAnalysisFrame();
		if (payload) {
			uint32_t copiedAnalysisSeq = workerLastSubmittedAnalysisSeq;
			const bool copiedAnalysis = module->copyAnalysisFrame(
				workerLastSubmittedAnalysisSeq,
				payload->analysisFrame.rawInput,
				payload->analysisFrame.output,
				&copiedAnalysisSeq
			);
			if (copiedAnalysis) {
				payload->hasOverlayTarget = state.hasOverlayTarget;
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
	workerLastSubmittedPreviewSeq = state.lastPreviewSeq;
	workerLastSubmittedAnalysisSeq = request.analysisSeq;
	bifurxRenderService().submitLatest(std::move(request));
	if (measurePerf) {
		lastWorkerSubmitUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - submitStart).count()) * 1e-3f;
	}
}

bool BifurxSpectrumBase::adoptWorkerCurveSnapshot() {
	if (workerDisplayId == 0 || workerLastAppliedRequestSeq >= workerRequestSeq) {
		return false;
	}
	workerSnapshotCache = bifurxRenderService().getLatestSnapshot(workerDisplayId);
	if (!workerSnapshotCache) {
		return false;
	}
	if (workerSnapshotCache->requestSeq <= workerLastAppliedRequestSeq) {
		return false;
	}
	if (!workerSnapshotCache->hasCurveTarget) {
		return false;
	}
	// Do not reject snapshots solely for being older than the latest submitted seq.
	// Under heavy load this can cause a module to repeatedly drop usable snapshots
	// and appear permanently behind.
	for (int i = 0; i < kCurvePointCount; ++i) {
		state.curveHz[i] = workerSnapshotCache->curveHz[i];
		state.curveBinPos[i] = workerSnapshotCache->curveBinPos[i];
		state.curveTargetDb[i] = workerSnapshotCache->curveTargetDb[i];
	}
	state.cachedAxisSampleRate = workerSnapshotCache->cachedAxisSampleRate;
	if (!state.hasCurveTarget) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.curveDb[i] = state.curveTargetDb[i];
		}
	}
	state.hasCurveTarget = true;
	// Force marker/layout recompute after worker-provided curve adoption.
	// Without this, first-frame VW spawns can retain stale marker Y positions
	// from pre-adoption cache state until another parameter change occurs.
	cachedMarkerLayoutValid = false;
	refinedCurveTemplateValid = false;
	lastCurvePrepUs = workerSnapshotCache->curvePrepUs;
	if (workerSnapshotCache->hasOverlayTarget &&
		workerSnapshotCache->analysisSeq >= workerLastAppliedAnalysisSeq) {
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.overlayTargetModuleDb[i] = workerSnapshotCache->overlayTargetModuleDb[i];
			state.overlayTargetOutputDbfs[i] = workerSnapshotCache->overlayTargetOutputDbfs[i];
		}
		state.displayTopTargetDbfs = workerSnapshotCache->displayTopTargetDbfs;
		if (!state.hasOverlayTarget) {
			for (int i = 0; i < kCurvePointCount; ++i) {
				state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
				state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
			}
			state.hasOverlayTarget = true;
		}
		state.hasOverlay = true;
		lastOverlayPrepUs = workerSnapshotCache->overlayPrepUs;
		workerLastAppliedAnalysisSeq = workerSnapshotCache->analysisSeq;
	}
	workerLastAppliedRequestSeq = workerSnapshotCache->requestSeq;
	workerLastAppliedPreviewSeq = workerSnapshotCache->previewSeq;
	return true;
}

void BifurxSpectrumBase::initializeStaticPreviewStateIfNeeded() {
	if (state.hasPreview) return;
	BifurxPreviewState preview;
	preview.sampleRate = 48000.f;
	preview.mode = kBrowserPreviewMode;
	const float previewCenterHz = bifurxFrequencyHzFromParam(kBrowserPreviewFrequency);
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
	const BifurxPreviewModel& model = getOrUpdateModel();
	prepareCurveTargets(model, state.curveHz, state.curveTargetDb);
	if (!state.hasCurveTarget) {
		for (int i = 0; i < kCurvePointCount; i++) state.curveDb[i] = state.curveTargetDb[i];
		state.hasCurveTarget = true;
	}
}

const BifurxPreviewModel& BifurxSpectrumBase::getOrUpdateModel() const {
	if (state.lastPreviewSeq != lastModelUpdateSeq) {
		cachedModel = makePreviewModel(state.previewState);
		const_cast<BifurxSpectrumBase*>(this)->lastModelUpdateSeq = state.lastPreviewSeq;
	}
	return cachedModel;
}

bool BifurxSpectrumBase::updateOverlayCache(uint32_t* copiedSeq) {
	if (!state.hasPreview || !module) return false;
	updateAxisCache();
	SynchronousOverlayScratch& scratch = synchronousOverlayScratch();
	uint32_t frameSeq = state.lastAnalysisSeq;
	if (!module->copyAnalysisFrame(
		state.lastAnalysisSeq,
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
	const bool fftScaleDynamic = module ? module->fftScaleDynamic.load(std::memory_order_relaxed) : true;
	prepareOverlayTargetsFromSpectra(
		state.previewState.sampleRate,
		state.curveBinPos,
		scratch.fftOutputFreq,
		scratch.fftRawInputFreq,
		moduleResponseEnabled,
		state.hasOverlayTarget,
		fftScaleDynamic,
		state.overlayTargetModuleDb,
		state.overlayTargetOutputDbfs,
		&state.displayTopTargetDbfs
	);

	if (!state.hasOverlayTarget) {
		for (int i = 0; i < kCurvePointCount; i++) {
			state.overlayModuleDb[i] = state.overlayTargetModuleDb[i];
			state.overlayOutputDbfs[i] = state.overlayTargetOutputDbfs[i];
		}
		state.hasOverlayTarget = true;
	}
	if (copiedSeq) {
		*copiedSeq = frameSeq;
	}
	return true;
}

bool BifurxSpectrumBase::updateAnimation(float dt) {
	bool animationActive = false;
	constexpr float kCurveEpsilonDb = 0.01f;
	constexpr float kOverlayEpsilonDb = 0.02f;
	constexpr float kTopEpsilonDbfs = 0.02f;

	if (state.hasCurveTarget) {
		const float curveMaxStepDb = std::max(0.25f, kCurveVisualSlewDbPerSec * dt);
		float maxCurveResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			const float prev = state.curveDb[i];
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

	if (state.hasOverlayTarget) {
		const float overlayDbSmoothing = 0.22f;
		const float overlayLevelSmoothing = 0.20f;
		float maxOverlayResidualDb = 0.f;
		for (int i = 0; i < kCurvePointCount; ++i) {
			state.overlayModuleDb[i] = mixf(state.overlayModuleDb[i], state.overlayTargetModuleDb[i], overlayDbSmoothing);
			state.overlayOutputDbfs[i] = mixf(state.overlayOutputDbfs[i], state.overlayTargetOutputDbfs[i], overlayLevelSmoothing);
			const float moduleResidual = std::fabs(state.overlayTargetModuleDb[i] - state.overlayModuleDb[i]);
			const float outputResidual = std::fabs(state.overlayTargetOutputDbfs[i] - state.overlayOutputDbfs[i]);
			maxOverlayResidualDb = std::max(maxOverlayResidualDb, std::max(moduleResidual, outputResidual));
		}

		const float prevTop = state.displayTopDbfs;
		float topSmoothing = (state.displayTopTargetDbfs > prevTop) ? 0.22f : 0.10f;
		if (module && module->fftScaleDynamic.load(std::memory_order_relaxed) && state.displayTopTargetDbfs > prevTop) {
			topSmoothing = 0.70f;
		}
		state.displayTopDbfs = mixf(prevTop, state.displayTopTargetDbfs, topSmoothing);
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

	return animationActive;
}

BifurxRenderTickResult BifurxSpectrumBase::runRenderTick(float dt) {
	BifurxRenderTickResult result;
	const uint32_t prevPreviewSeq = state.lastPreviewSeq;
	const uint32_t prevAnalysisSeq = state.lastAnalysisSeq;

	syncBase();
	const bool workerAdopted = adoptWorkerCurveSnapshot();
	result.previewUpdated = (state.lastPreviewSeq != prevPreviewSeq) || workerAdopted;
	result.analysisUpdated = (state.lastAnalysisSeq != prevAnalysisSeq);
	result.animationActive = updateAnimation(dt);
	result.curvePrepUs = (result.previewUpdated || workerAdopted) ? lastCurvePrepUs : 0.f;
	result.overlayPrepUs = result.analysisUpdated ? lastOverlayPrepUs : 0.f;
	return result;
}

void BifurxSpectrumBase::calculateMarkerLayout(BifurxMarkerLayout* layout, float w, float h) const {
	if (!layout) return;
	const float padY = std::max(4.f, h * 0.035f);
	const float labelBandHeight = std::max(5.2f, h * 0.072f), labelBandTop = h - labelBandHeight;
	const float spectrumTopY = padY * 0.35f, spectrumBottomY = std::max(spectrumTopY + 1.f, labelBandTop - std::max(0.05f, h * 0.0008f));
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
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
		const bool allowBottomCurveMarker = state.previewState.mode == 0 || state.previewState.mode == 9;
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
	const float maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
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
	rebuild = rebuild || std::fabs(cachedMarkerLayoutSampleRate - state.previewState.sampleRate) > 0.5f;
	rebuild = rebuild || cachedMarkerLayoutPreviewSeq != state.lastPreviewSeq;
	rebuild = rebuild || state.hasCurveTarget;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutAnchorX01[0] - anchors[0].x01) > 1e-7f;
	rebuild = rebuild || std::fabs(cachedMarkerLayoutAnchorX01[1] - anchors[1].x01) > 1e-7f;
	rebuild = rebuild || cachedMarkerLayoutMarkerPinned[0] != markerPinned[0];
	rebuild = rebuild || cachedMarkerLayoutMarkerPinned[1] != markerPinned[1];

	if (rebuild) {
		calculateMarkerLayout(&cachedMarkerLayout, w, h);
		cachedMarkerLayoutW = w;
		cachedMarkerLayoutH = h;
		cachedMarkerLayoutSampleRate = state.previewState.sampleRate;
		cachedMarkerLayoutPreviewSeq = state.lastPreviewSeq;
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
	const float minHz = 10.f, maxHz = std::min(20000.f, 0.46f * state.previewState.sampleRate);
	const BifurxPreviewModel& model = getOrUpdateModel();
	const float markerOuterRadius = kPeakMarkerFillRadius + kPeakMarkerOutlineExtraRadius + 0.5f * kPeakMarkerOutlineStrokeWidth;
	const float markerBottomLaneY = spectrumBottomY - markerOuterRadius - kPeakMarkerBottomLanePadding;
	const DisplayAnchor anchors[2] = {
		displayAnchorForMarker(0, model.markerFreqA, minHz, maxHz),
		displayAnchorForMarker(1, model.markerFreqB, minHz, maxHz)
	};
	const bool markerPinned[2] = {
		markerPinnedToBottomLane(0),
		markerPinnedToBottomLane(1)
	};

	bool rebuildTemplate = !refinedCurveTemplateValid;
	rebuildTemplate = rebuildTemplate || std::fabs(w - refinedCurveTemplateW) > 1e-4f;
	rebuildTemplate = rebuildTemplate || std::fabs(h - refinedCurveTemplateH) > 1e-4f;
	rebuildTemplate = rebuildTemplate || std::fabs(state.previewState.sampleRate - refinedCurveTemplateSampleRate) > 0.5f;
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
			refinedCurveTemplate.push_back({float(i) / float(kCurvePointCount - 1), 0.f, 0});
		}

		auto addRefinement = [&](const DisplayAnchor& anchor) {
			const float dx = 0.35f / float(kCurvePointCount - 1);
			refinedCurveTemplate.push_back({clamp(anchor.x01 - dx, 0.f, 1.f), 0.f, 1});
			refinedCurveTemplate.push_back({clamp(anchor.x01, 0.f, 1.f), 0.f, 2});
			refinedCurveTemplate.push_back({clamp(anchor.x01 + dx, 0.f, 1.f), 0.f, 1});
		};

		addRefinement(anchors[0]);
		addRefinement(anchors[1]);

		std::sort(refinedCurveTemplate.begin(), refinedCurveTemplate.end(), [](const BifurxCurvePoint& a, const BifurxCurvePoint& b) {
			if (std::fabs(a.x01 - b.x01) > 1e-7f) return a.x01 < b.x01;
			return a.priority > b.priority;
		});
		refinedCurveTemplate.erase(std::unique(refinedCurveTemplate.begin(), refinedCurveTemplate.end(), [](const BifurxCurvePoint& a, const BifurxCurvePoint& b) {
			return std::fabs(a.x01 - b.x01) < 1e-7f;
		}), refinedCurveTemplate.end());

		refinedCurveTemplateW = w;
		refinedCurveTemplateH = h;
		refinedCurveTemplateSampleRate = state.previewState.sampleRate;
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
	points->insert(points->end(), refinedCurveTemplate.begin(), refinedCurveTemplate.end());

	// Evaluate Y coordinates for all final points from live curve data.
	for (auto& p : *points) {
		p.y = curveYAtX01(p.x01, spectrumBottomY, spectrumTopY);
	}
	for (int markerIndex = 0; markerIndex < 2; ++markerIndex) {
		if (!markerPinned[markerIndex]) continue;
		const DisplayAnchor& anchor = anchors[markerIndex];
		for (auto& p : *points) {
			if (p.priority == 2 && std::fabs(p.x01 - anchor.x01) < 1e-7f) {
				p.y = markerBottomLaneY;
			}
		}
	}
}

} // namespace bifurx
