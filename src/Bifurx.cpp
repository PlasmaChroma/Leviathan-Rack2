#include "Bifurx.hpp"

namespace bifurx {

const char* const kBifurxCircuitLabels[kBifurxCircuitModeCount] = {"SVF", "Bite", "Vowel", "Erode"};
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
	"High + High"
};

std::string bifurxUserRootPath() {
	return system::join(asset::user(), "Leviathan/Bifurx");
}

float levelDriveGain(float knob) {
	const float x = bifurx::clamp01(knob);
	return 0.06f + 0.95f * x + 3.6f * x * x * x;
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
	return minHz * std::pow(maxHz / minHz, bifurx::clamp01(x01));
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

float softLimitOverlayDeltaDb(float db) {
	constexpr float kneeDb = 18.f;
	constexpr float limitDb = 42.f;
	const float sign = (db < 0.f) ? -1.f : 1.f;
	const float absDb = std::fabs(db);
	if (absDb <= kneeDb) {
		return db;
	}
	const float over = absDb - kneeDb;
	const float compressed = kneeDb + (limitDb - kneeDb) * (1.f - fastExp(-over / (limitDb - kneeDb)));
	return sign * std::min(compressed, limitDb);
}

float softLimitExpectedCurveDb(float db) {
	constexpr float kneeDb = 26.f;
	constexpr float limitDb = 48.f;
	const float sign = (db < 0.f) ? -1.f : 1.f;
	const float absDb = std::fabs(db);
	if (absDb <= kneeDb) {
		return clamp(db, -limitDb, limitDb);
	}
	const float over = absDb - kneeDb;
	const float compressed = kneeDb + (limitDb - kneeDb) * (1.f - fastExp(-over / (limitDb - kneeDb)));
	return sign * std::min(compressed, limitDb);
}

float resoToDamping(float resoNorm) {
	const float r = bifurx::clamp01(resoNorm);
	return 2.f - 1.97f * std::pow(r, 1.18f);
}

float signedWeight(float balance, bool upperPeak) {
	const float sign = upperPeak ? 1.f : -1.f;
	return fastExp(0.82f * sign * clamp(balance, -1.f, 1.f));
}

float cascadeWideMorph(float spanNorm) {
	const float x = bifurx::clamp01((bifurx::clamp01(spanNorm) - 0.03f) / 0.97f);
	return std::pow(x, 0.58f);
}

float highHighSpanCompGain(float wideMorph) {
	const float x = bifurx::clamp01((wideMorph - 0.75f) / 0.25f);
	return 1.f + 0.685f * std::pow(x, 1.1f);
}

float saturateAsym(float x, float positiveDrive, float negativeDrive) {
	const float pos = std::max(positiveDrive, 1e-3f);
	const float neg = std::max(negativeDrive, 1e-3f);
	return (x >= 0.f) ? std::tanh(pos * x) / pos : std::tanh(neg * x) / neg;
}

float foldSoft(float x, float amount) {
	const float a = bifurx::clamp01(amount);
	const float folded = std::tanh(x) + 0.28f * a * std::tanh(2.7f * x) - 0.06f * a * std::tanh(5.4f * x);
	return bifurx::sanitizeFinite(folded);
}

float shapeBiteResonance(float bp, float lp, float hp, float driveNorm, float resoNorm, int stageIndex) {
	const float r = bifurx::clamp01(resoNorm);
	const float stage = clamp(float(stageIndex), 0.f, 1.f);
	const float gain = 1.06f + 0.92f * r + 0.18f * driveNorm + 0.07f * stage;
	const float bias = 0.10f * lp - 0.015f * hp;
	return saturateAsym(gain * bp + bias, 1.38f + 0.24f * r, 0.96f);
}

float shapeErodeResonance(float bp, float lp, float hp, float driveNorm, float resoNorm, int stageIndex) {
	const float r = bifurx::clamp01(resoNorm);
	const float stage = clamp(float(stageIndex), 0.f, 1.f);
	const float core = (0.82f + 0.92f * r + 0.38f * driveNorm + 0.14f * stage) * bp
		+ (0.18f + 0.12f * r) * hp
		- (0.10f + 0.04f * r) * lp;
	return foldSoft(core, 0.58f + 0.28f * r + 0.20f * driveNorm);
}

float previewCharacterDisplayDb(float db, int circuitMode) {
	const int mode = bifurx::clampCircuitMode(circuitMode);
	if (mode != BIFURX_CHARACTER_BITE && mode != BIFURX_CHARACTER_ERODE) {
		return db;
	}
	if (db <= 0.f) {
		return db;
	}
	const float kneeDb = (mode == BIFURX_CHARACTER_BITE) ? 10.f : 8.5f;
	const float limitDb = (mode == BIFURX_CHARACTER_BITE) ? 24.f : 20.f;
	if (db <= kneeDb) {
		return db;
	}
	const float over = db - kneeDb;
	return kneeDb + (limitDb - kneeDb) * (1.f - fastExp(-over / (limitDb - kneeDb)));
}

void applyProbeEnvelopeHint(float* dbValues, int count, int circuitMode) {
	if (!dbValues || count <= 2) {
		return;
	}
	const int mode = bifurx::clampCircuitMode(circuitMode);
	if (mode != BIFURX_CHARACTER_BITE && mode != BIFURX_CHARACTER_ERODE) {
		return;
	}

	const int radius = (mode == BIFURX_CHARACTER_BITE) ? 4 : 6;
	const float blend = (mode == BIFURX_CHARACTER_BITE) ? 0.42f : 0.55f;
	float envelope[kCurvePointCount];
	for (int i = 0; i < count; ++i) {
		float weightedMax = dbValues[i];
		for (int k = -radius; k <= radius; ++k) {
			const int idx = clamp(i + k, 0, count - 1);
			const float dist = std::fabs(float(k)) / float(std::max(radius, 1));
			const float penalty = (mode == BIFURX_CHARACTER_BITE ? 1.6f : 1.2f) * dist;
			weightedMax = std::max(weightedMax, dbValues[idx] - penalty);
		}
		envelope[i] = weightedMax;
	}

	float smoothed[kCurvePointCount];
	for (int i = 0; i < count; ++i) {
		const int left = std::max(0, i - 1);
		const int right = std::min(count - 1, i + 1);
		smoothed[i] = 0.22f * envelope[left] + 0.56f * envelope[i] + 0.22f * envelope[right];
	}

	for (int i = 0; i < count; ++i) {
		dbValues[i] = bifurx::mixf(dbValues[i], smoothed[i], blend);
	}
}

float circuitCutoffScale(int circuitMode) {
	switch (bifurx::clampCircuitMode(circuitMode)) {
		case BIFURX_CHARACTER_BITE:
			return 1.f;
		case BIFURX_CHARACTER_VOWEL:
			return 0.92f;
		case BIFURX_CHARACTER_ERODE:
			return 1.08f;
		default:
			return 1.f;
	}
}

float circuitQScale(float resoNorm, int circuitMode) {
	const float r = bifurx::clamp01(resoNorm);
	switch (bifurx::clampCircuitMode(circuitMode)) {
		case BIFURX_CHARACTER_BITE:
			return 1.18f + 0.95f * r;
		case BIFURX_CHARACTER_VOWEL:
			return 1.20f + 0.45f * r;
		case BIFURX_CHARACTER_ERODE:
			return 0.92f + 0.60f * r;
		default:
			return 1.f;
	}
}

SemanticExportProfile semanticExportProfile(int circuitMode, int stageIndex) {
	const int clampedCircuitMode = bifurx::clampCircuitMode(circuitMode);
	SemanticExportProfile profile;
	switch (clampedCircuitMode) {
		case BIFURX_CHARACTER_BITE:
			profile.lpScale = 0.f;
			profile.bpScale = 6.8f;
			profile.hpScale = 6.2f;
			return profile;
		case BIFURX_CHARACTER_VOWEL:
			profile.lpScale = 0.f;
			profile.bpScale = 6.5f;
			profile.hpScale = 0.f;
			return profile;
		case BIFURX_CHARACTER_ERODE:
			profile.lpScale = 4.4f;
			profile.bpScale = 4.2f;
			profile.hpScale = 3.6f;
			return profile;
		default:
			return profile;
	}
}

SvfOutputs normalizeSemanticOutputs(const SvfOutputs& raw, int circuitMode, int stageIndex) {
	const SemanticExportProfile profile = semanticExportProfile(circuitMode, stageIndex);
	SvfOutputs out;
	out.lp = bifurx::normalizeSemanticComponent(raw.lp, profile.lpScale);
	out.bp = bifurx::normalizeSemanticComponent(raw.bp, profile.bpScale);
	out.hp = bifurx::normalizeSemanticComponent(raw.hp, profile.hpScale);
	out.notch = out.lp + out.hp;
	return out;
}

float modeCircuitSyncCompGain(int mode, int circuitMode, float wideMorph) {
	const int clampedCircuitMode = bifurx::clampCircuitMode(circuitMode);
	if (clampedCircuitMode == BIFURX_CHARACTER_SVF) {
		return 1.f;
	}
	switch (mode) {
		case 0: {
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.03f, 0.99f, 1.02f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 1:
		case 8: {
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.05f, 0.97f, 0.99f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 2:
		case 7: {
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.02f, 0.98f, 1.00f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 3: {
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.00f, 0.98f, 0.96f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 5: {
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.07f, 0.90f, 1.03f};
			return kGainByCircuit[clampedCircuitMode];
		}
		case 9: {
			static const float kBaseByCircuit[kBifurxCircuitModeCount] = {1.f, 1.06f, 0.95f, 1.00f};
			float gain = kBaseByCircuit[clampedCircuitMode];
			const float hiSpan = bifurx::clamp01((wideMorph - 0.72f) / 0.28f);
			if (clampedCircuitMode == BIFURX_CHARACTER_BITE || clampedCircuitMode == BIFURX_CHARACTER_ERODE) {
				gain -= 0.02f * hiSpan;
			}
			else if (clampedCircuitMode == BIFURX_CHARACTER_VOWEL) {
				gain -= 0.02f * hiSpan;
			}
			return gain;
		}
		default:
			static const float kGainByCircuit[kBifurxCircuitModeCount] = {1.f, 1.03f, 0.98f, 1.00f};
			return kGainByCircuit[clampedCircuitMode];
	}
}

NVGcolor mixColor(const NVGcolor& a, const NVGcolor& b, float t) {
	const float clampedT = bifurx::clamp01(t);
	NVGcolor out;
	out.r = bifurx::mixf(a.r, b.r, clampedT);
	out.g = bifurx::mixf(a.g, b.g, clampedT);
	out.b = bifurx::mixf(a.b, b.b, clampedT);
	out.a = bifurx::mixf(a.a, b.a, clampedT);
	return out;
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

SvfCoeffs makeSvfCoeffs(float sampleRate, float cutoff, float damping) {
	const float sr = std::max(sampleRate, 1.f);
	const float limitedCutoff = clamp(cutoff, 4.f, 0.46f * sr);
	const float g = std::tan(kPi * limitedCutoff / sr);
	const float k = clamp(damping, 0.02f, 2.2f);
	const float a1 = 1.f / (1.f + g * (g + k));
	SvfCoeffs coeffs;
	coeffs.g = g;
	coeffs.k = k;
	coeffs.a1 = a1;
	return coeffs;
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
	int characterMode,
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	const SvfCoeffs* cachedCoeffsOrNull
) {
	const int mode = bifurx::clampCircuitMode(characterMode);
	const int stage = clamp(stageIndex, 0, 1);
	const float r = bifurx::clamp01(resoNorm);
	const float driveNorm = bifurx::clamp01((drive - 1.f) / 3.f);
	SvfOutputs raw;

	switch (mode) {
		case BIFURX_CHARACTER_BITE: {
			const float feedbackDamping = clamp(damping * (0.95f - 0.08f * r), 0.02f, 2.2f);
			raw = core.process(input, sampleRate, cutoff, feedbackDamping);
			const float reson = shapeBiteResonance(raw.bp, raw.lp, raw.hp, driveNorm, r, stageIndex);
			const float peakTakeover = clamp(0.24f + 0.50f * r + 0.10f * driveNorm, 0.f, 0.88f);
			raw.lp = 1.03f * raw.lp + (0.045f + 0.035f * peakTakeover) * reson;
			raw.bp = saturateAsym(bifurx::mixf(1.02f * raw.bp, 1.22f * reson, peakTakeover), 1.18f + 0.12f * r, 0.98f);
			raw.hp = saturateAsym(0.98f * raw.hp + 0.010f * reson, 1.04f, 1.00f);
		} break;
		case BIFURX_CHARACTER_VOWEL: {
			const float formantDamping = clamp(damping * (0.94f - 0.05f * stage), 0.02f, 2.2f);
			raw = core.process(input, sampleRate, cutoff, formantDamping);
			const float stageTilt = (stage == 0) ? -1.f : 1.f;
			const float bpGain = 1.18f + 0.60f * r + 0.10f * stage;
			const float lpWarm = (0.92f - 0.04f * r) * raw.lp + (0.11f - 0.02f * stageTilt) * raw.bp;
			const float bpFormant = bpGain * raw.bp + 0.05f * stageTilt * raw.lp - 0.03f * stageTilt * raw.hp;
			const float hpSoft = (0.80f - 0.03f * stage) * raw.hp - 0.07f * raw.bp + 0.03f * stageTilt * raw.lp;
			raw.lp = lpWarm;
			raw.bp = bpFormant;
			raw.hp = hpSoft;
		} break;
		case BIFURX_CHARACTER_ERODE: {
			const float corrodeDamping = clamp(damping * (1.08f + 0.08f * (1.f - r)), 0.02f, 2.2f);
			raw = core.process(input, sampleRate, cutoff, corrodeDamping);
			const float reson = shapeErodeResonance(raw.bp, raw.lp, raw.hp, driveNorm, r, stageIndex);
			const float peakTakeover = clamp(0.24f + 0.42f * r + 0.18f * driveNorm, 0.f, 0.86f);
			raw.lp = 0.90f * raw.lp + (0.025f + 0.025f * peakTakeover) * reson;
			raw.bp = saturateAsym(bifurx::mixf(0.96f * raw.bp, 1.34f * reson, peakTakeover), 1.08f, 0.90f);
			raw.hp = saturateAsym(1.13f * raw.hp + (0.16f + 0.10f * peakTakeover) * reson, 1.20f + 0.10f * r, 0.82f);
		} break;
		case BIFURX_CHARACTER_SVF:
		default:
			if (cachedCoeffsOrNull) {
				raw = core.processWithCoeffs(input, *cachedCoeffsOrNull);
			}
			else {
				raw = core.process(input, sampleRate, cutoff, damping);
			}
			return raw;
	}

	sanitizeCoreState(core);
	raw.lp = bifurx::sanitizeFinite(raw.lp);
	raw.bp = bifurx::sanitizeFinite(raw.bp);
	raw.hp = bifurx::sanitizeFinite(raw.hp);
	raw.notch = bifurx::sanitizeFinite(raw.lp + raw.hp);
	return normalizeSemanticOutputs(raw, mode, stage);
}

std::complex<float> DisplayBiquad::response(float omega) const {
	const std::complex<float> z1 = std::exp(std::complex<float>(0.f, -omega));
	const std::complex<float> z2 = z1 * z1;
	const std::complex<float> numerator = b0 + b1 * z1 + b2 * z2;
	const std::complex<float> denominator = 1.f + a1 * z1 + a2 * z2;
	return numerator / denominator;
}

DisplayBiquad makeDisplayBiquad(float sampleRate, float cutoff, float q, int type) {
	const float sr = std::max(sampleRate, 1.f);
	const float freq = clamp(cutoff, 4.f, 0.46f * sr);
	const float omega = 2.f * kPi * freq / sr;
	const float cosW = std::cos(omega);
	const float sinW = std::sin(omega);
	const float alpha = sinW / (2.f * std::max(q, 0.05f));

	float b0 = 0.f;
	float b1 = 0.f;
	float b2 = 0.f;
	float a0 = 1.f + alpha;
	float a1 = -2.f * cosW;
	float a2 = 1.f - alpha;

	switch (type) {
		case 0: // lowpass
			b0 = 0.5f * (1.f - cosW);
			b1 = 1.f - cosW;
			b2 = 0.5f * (1.f - cosW);
			break;
		case 1: // bandpass
			b0 = alpha;
			b1 = 0.f;
			b2 = -alpha;
			break;
		case 2: // highpass
			b0 = 0.5f * (1.f + cosW);
			b1 = -(1.f + cosW);
			b2 = 0.5f * (1.f + cosW);
			break;
		default: // notch
			b0 = 1.f;
			b1 = -2.f * cosW;
			b2 = 1.f;
			break;
	}

	DisplayBiquad biquad;
	biquad.b0 = b0 / a0;
	biquad.b1 = b1 / a0;
	biquad.b2 = b2 / a0;
	biquad.a1 = a1 / a0;
	biquad.a2 = a2 / a0;
	return biquad;
}

BifurxPreviewCurvePolicy previewCurvePolicyForCharacter(int characterMode) {
	switch (bifurx::clampCircuitMode(characterMode)) {
		case BIFURX_CHARACTER_SVF:
		case BIFURX_CHARACTER_VOWEL:
			return BIFURX_PREVIEW_ANALYTIC;
		case BIFURX_CHARACTER_BITE:
		case BIFURX_CHARACTER_ERODE:
		default:
			return BIFURX_PREVIEW_PROBE_FFT;
	}
}

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) return true;
	if (a.circuitMode != b.circuitMode) return true;
	if (std::fabs(a.sampleRate - b.sampleRate) > 0.5f) return true;
	if (std::fabs(a.balance - b.balance) > 1e-3f) return true;
	if (std::fabs(std::log2(std::max(a.freqA, 1.f) / std::max(b.freqA, 1.f))) > 1e-3f) return true;
	if (std::fabs(std::log2(std::max(a.freqB, 1.f) / std::max(b.freqB, 1.f))) > 1e-3f) return true;
	if (std::fabs(a.qA - b.qA) > 1e-3f) return true;
	if (std::fabs(a.qB - b.qB) > 1e-3f) return true;
	return false;
}

BifurxPreviewModel makePreviewModel(const BifurxPreviewState& state) {
	BifurxPreviewModel model;
	const float freqA = clamp(state.freqA, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float freqB = clamp(state.freqB, 4.f, 0.46f * std::max(state.sampleRate, 1.f));
	const float qA = clamp(state.qA, 0.2f, 18.f);
	const float qB = clamp(state.qB, 0.2f, 18.f);
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
	model.mode = state.mode;
	model.circuitMode = bifurx::clampCircuitMode(state.circuitMode);

	const float lowW = signedWeight(state.balance, false);
	const float highW = signedWeight(state.balance, true);
	const float norm = 2.f / (lowW + highW);
	model.wA = lowW * norm;
	model.wB = highW * norm;
	model.wideMorph = cascadeWideMorph(state.spanNorm);
	return model;
}

std::complex<float> previewModelResponse(const BifurxPreviewModel& model, float hz) {
	const float omega = 2.f * kPi * clamp(hz, 4.f, 0.49f * model.sampleRate) / std::max(model.sampleRate, 1.f);
	const SemanticExportProfile profileA = semanticExportProfile(model.circuitMode, 0);
	const SemanticExportProfile profileB = semanticExportProfile(model.circuitMode, 1);
	std::complex<float> lpA = bifurx::normalizeSemanticComponent(model.lowA.response(omega), profileA.lpScale);
	std::complex<float> bpA = bifurx::normalizeSemanticComponent(model.bandA.response(omega), profileA.bpScale);
	std::complex<float> hpA = bifurx::normalizeSemanticComponent(model.highA.response(omega), profileA.hpScale);
	std::complex<float> lpB = bifurx::normalizeSemanticComponent(model.lowB.response(omega), profileB.lpScale);
	std::complex<float> bpB = bifurx::normalizeSemanticComponent(model.bandB.response(omega), profileB.bpScale);
	std::complex<float> hpB = bifurx::normalizeSemanticComponent(model.highB.response(omega), profileB.hpScale);
	if (model.circuitMode == BIFURX_CHARACTER_VOWEL) {
		const std::complex<float> rawLpA = lpA, rawBpA = bpA, rawHpA = hpA, rawLpB = lpB, rawBpB = bpB, rawHpB = hpB;
		const float stageATilt = -1.f, stageBTilt = 1.f;
		lpA = (0.92f - 0.04f * model.wideMorph) * rawLpA + (0.13f - 0.02f * stageATilt) * rawBpA;
		bpA = (1.18f + 0.55f * model.wideMorph) * rawBpA + 0.05f * stageATilt * rawLpA - 0.03f * stageATilt * rawHpA;
		hpA = 0.80f * rawHpA - 0.07f * rawBpA + 0.03f * stageATilt * rawLpA;
		lpB = (0.92f - 0.04f * model.wideMorph) * rawLpB + (0.13f - 0.02f * stageBTilt) * rawBpB;
		bpB = (1.25f + 0.62f * model.wideMorph) * rawBpB + 0.05f * stageBTilt * rawLpB - 0.03f * stageBTilt * rawHpB;
		hpB = 0.77f * rawHpB - 0.07f * rawBpB + 0.03f * stageBTilt * rawLpB;
	}
	const std::complex<float> ntA = lpA + hpA, ntB = lpB + hpB, cascadeLp = lpB * lpA, cascadeNotch = ntB * ntA, cascadeHpToLp = lpB * hpA, cascadeHpToHp = hpB * hpA;
	return combineModeResponse<std::complex<float>>(model.mode, lpA, bpA, hpA, ntA, lpB, bpB, hpB, ntB, cascadeLp, cascadeNotch, cascadeHpToLp, cascadeHpToHp, model.wA, model.wB, model.wideMorph, model.circuitMode);
}

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return previewCharacterDisplayDb(20.f * std::log10(std::max(mag, 1e-5f)), model.circuitMode);
}

float previewProbeStimulusSample(const BifurxPreviewState& state, int sampleIndex) {
	if (sampleIndex < 0) return 0.f;
	const int circuitMode = bifurx::clampCircuitMode(state.circuitMode);
	if (circuitMode == BIFURX_CHARACTER_BITE || circuitMode == BIFURX_CHARACTER_ERODE) {
		if (sampleIndex >= kPreviewProbeBurstLength) return 0.f;
		const float t = float(sampleIndex) / float(std::max(kPreviewProbeBurstLength - 1, 1)), window = 0.5f - 0.5f * std::cos(2.f * kPi * t), burstCycles = (circuitMode == BIFURX_CHARACTER_ERODE) ? 2.25f : 1.5f, oscillation = std::sin(2.f * kPi * burstCycles * t), polarity = (sampleIndex & 1) ? -1.f : 1.f;
		return 0.09f * window * oscillation + 0.015f * window * polarity;
	}
	return (sampleIndex == 0) ? kPreviewProbeImpulseAmplitude : 0.f;
}

SvfOutputs processProbeStage(BifurxProbeEngineState& state, int stageIndex, int circuitMode, float input, float sampleRate, float cutoff, float damping, float drive, float resoNorm) {
	TptSvf& core = (stageIndex == 0) ? state.svfA : state.svfB;
	return processCharacterStage(core, circuitMode, stageIndex, input, sampleRate, cutoff, damping, drive, resoNorm, nullptr);
}

void simulatePreviewProbeImpulseResponse(const BifurxPreviewState& state, float* inputBuffer, float* outputBuffer, int sampleCount) {
	if (!inputBuffer || !outputBuffer || sampleCount <= 0) return;
	BifurxProbeEngineState engine;
	const float sampleRate = std::max(state.sampleRate, 1.f), freqA = clamp(state.freqA, kFreqMinHz, 0.46f * sampleRate), freqB = clamp(state.freqB, kFreqMinHz, 0.46f * sampleRate), dampingA = clamp(1.f / std::max(state.qA, 0.05f), 0.02f, 2.2f), dampingB = clamp(1.f / std::max(state.qB, 0.05f), 0.02f, 2.2f), lowW = signedWeight(state.balance, false), highW = signedWeight(state.balance, true), norm = 2.f / (lowW + highW), wA = lowW * norm, wB = highW * norm, wideMorph = cascadeWideMorph(state.spanNorm), drive = levelDriveGain(kPreviewProbeLevelKnob);
	const int mode = clamp(state.mode, 0, kBifurxModeCount - 1), circuitMode = bifurx::clampCircuitMode(state.circuitMode);
	for (int i = 0; i < sampleCount; ++i) {
		const float rawIn = previewProbeStimulusSample(state, i), excitation = 5.f * bifurx::softClip(0.2f * rawIn * drive);
		const SvfOutputs a = processProbeStage(engine, 0, circuitMode, excitation, sampleRate, freqA, dampingA, drive, state.resoNorm);
		SvfOutputs b; float modeOut = 0.f;
		switch (mode) {
			case 0: b = processProbeStage(engine, 1, circuitMode, a.lp, sampleRate, freqB, dampingB, drive, state.resoNorm); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, wA, wB, wideMorph, circuitMode); break;
			case 1:
			case 2:
			case 4:
			case 5:
			case 7:
			case 8: b = processProbeStage(engine, 1, circuitMode, excitation, sampleRate, freqB, dampingB, drive, state.resoNorm); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, wideMorph, circuitMode); break;
			case 3: b = processProbeStage(engine, 1, circuitMode, a.notch, sampleRate, freqB, dampingB, drive, state.resoNorm); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, wA, wB, wideMorph, circuitMode); break;
			case 6: b = processProbeStage(engine, 1, circuitMode, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, wA, wB, wideMorph, circuitMode); break;
			case 9:
			default: b = processProbeStage(engine, 1, circuitMode, a.hp, sampleRate, freqB, dampingB, drive, state.resoNorm); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.hp, wA, wB, wideMorph, circuitMode); break;
		}
		inputBuffer[i] = excitation; outputBuffer[i] = bifurx::sanitizeFinite(5.5f * bifurx::softClip(bifurx::sanitizeFinite(modeOut) / 5.5f));
	}
}

Bifurx::Bifurx() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(MODE_PARAM, 0.f, 9.f, 0.f, "Mode", {kBifurxModeLabels[0], kBifurxModeLabels[1], kBifurxModeLabels[2], kBifurxModeLabels[3], kBifurxModeLabels[4], kBifurxModeLabels[5], kBifurxModeLabels[6], kBifurxModeLabels[7], kBifurxModeLabels[8], kBifurxModeLabels[9]});
	configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level"); configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency"); configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance"); configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance"); configParam(SPAN_PARAM, 0.f, 1.f, 0.5f, "Span"); configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount"); configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator"); configSwitch(TITO_PARAM, 0.f, 2.f, 1.f, "TITO", {"XM", "Clean", "SM"}); configButton(MODE_LEFT_PARAM, "Mode previous"); configButton(MODE_RIGHT_PARAM, "Mode next"); configButton(FILTER_CIRCUIT_PARAM, "Filter circuit next");
	configInput(IN_INPUT, "Signal In"); configInput(VOCT_INPUT, "V/Oct"); configInput(FM_INPUT, "FM"); configInput(RESO_CV_INPUT, "Resonance CV"); configInput(BALANCE_CV_INPUT, "Balance CV"); configInput(SPAN_CV_INPUT, "Span CV"); configOutput(OUT_OUTPUT, "Signal Out"); configBypass(IN_INPUT, OUT_OUTPUT);
	paramQuantities[MODE_PARAM]->snapEnabled = true; paramQuantities[TITO_PARAM]->snapEnabled = true;
	previewPublishDivider.setDivision(kPreviewPublishFastDivision); previewPublishSlowDivider.setDivision(kPreviewPublishSlowDivision); controlUpdateDivider.setDivision(16); perfMeasureDivider.setDivision(64);
}

void Bifurx::resetCircuitStates() { coreA.ic1eq = 0.f; coreA.ic2eq = 0.f; coreB.ic1eq = 0.f; coreB.ic2eq = 0.f; llTelemetryExcitationSq = 0.f; llTelemetryStageALpSq = 0.f; llTelemetryStageBLpSq = 0.f; llTelemetryOutputSq = 0.f; voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
void Bifurx::setFilterCircuitMode(int newMode) { const int clampedMode = bifurx::clampCircuitMode(newMode); const bool changed = (filterCircuitMode != clampedMode) || (activeCircuitMode != clampedMode); filterCircuitMode = clampedMode; activeCircuitMode = clampedMode; params[FILTER_CIRCUIT_PARAM].setValue(0.f); if (changed) resetCircuitStates(); }
json_t* Bifurx::dataToJson() { json_t* root = Module::dataToJson(); json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic)); json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging)); json_object_set_new(root, "perfDebugLogging", json_boolean(perfDebugLogging)); json_object_set_new(root, "filterCircuitMode", json_integer(bifurx::clampCircuitMode(filterCircuitMode))); return root; }
void Bifurx::dataFromJson(json_t* root) { Module::dataFromJson(root); json_t* fftScaleDynamicJ = json_object_get(root, "fftScaleDynamic"); if (fftScaleDynamicJ) fftScaleDynamic = json_is_true(fftScaleDynamicJ); json_t* curveDebugLoggingJ = json_object_get(root, "curveDebugLogging"); if (curveDebugLoggingJ) curveDebugLogging = json_is_true(curveDebugLoggingJ); json_t* perfDebugLoggingJ = json_object_get(root, "perfDebugLogging"); if (perfDebugLoggingJ) perfDebugLogging = json_is_true(perfDebugLoggingJ); json_t* filterCircuitModeJ = json_object_get(root, "filterCircuitMode"); if (filterCircuitModeJ) setFilterCircuitMode(int(json_integer_value(filterCircuitModeJ))); else setFilterCircuitMode(filterCircuitMode); }
void Bifurx::resetPerfStats() { perfAudioSampledCount.store(0, std::memory_order_release); perfAudioProcessNs.store(0, std::memory_order_release); perfAudioControlsNs.store(0, std::memory_order_release); perfAudioCoreNs.store(0, std::memory_order_release); perfAudioPreviewNs.store(0, std::memory_order_release); perfAudioAnalysisNs.store(0, std::memory_order_release); perfAudioProcessMaxNs.store(0, std::memory_order_release); }
void Bifurx::publishPreviewState(const BifurxPreviewState& state) { int writeIndex = 1 - previewPublishedIndex.load(std::memory_order_relaxed); previewStates[writeIndex] = state; previewPublishedIndex.store(writeIndex, std::memory_order_release); previewPublishSeq.fetch_add(1, std::memory_order_release); lastPreviewState = state; hasLastPreviewState = true; }
void Bifurx::publishLlTelemetryState(const BifurxLlTelemetryState& state) { const int writeIndex = 1 - llTelemetryPublishedIndex.load(std::memory_order_relaxed); llTelemetryStates[writeIndex] = state; llTelemetryPublishedIndex.store(writeIndex, std::memory_order_release); llTelemetryPublishSeq.fetch_add(1, std::memory_order_release); }
void Bifurx::publishAnalysisFrame() { const int writeIndex = 1 - analysisPublishedIndex.load(std::memory_order_relaxed), start = analysisWritePos, firstCount = kFftSize - start, secondCount = start; std::memcpy(analysisFrames[writeIndex].rawInput, analysisRawInputHistory + start, size_t(firstCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].rawInput + firstCount, analysisRawInputHistory, size_t(secondCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].output, analysisOutputHistory + start, size_t(firstCount) * sizeof(float)); std::memcpy(analysisFrames[writeIndex].output + firstCount, analysisOutputHistory, size_t(secondCount) * sizeof(float)); analysisPublishedIndex.store(writeIndex, std::memory_order_release); analysisPublishSeq.fetch_add(1, std::memory_order_release); }
void Bifurx::pushAnalysisSample(float rawInputSample, float outputSample) { analysisRawInputHistory[analysisWritePos] = bifurx::sanitizeFinite(rawInputSample); analysisOutputHistory[analysisWritePos] = bifurx::sanitizeFinite(outputSample); analysisWritePos = (analysisWritePos + 1) % kFftSize; if (analysisFilled < kFftSize) analysisFilled++; if (analysisFilled == kFftSize) { analysisHopCounter++; if (!analysisPublishedOnce || analysisHopCounter >= kFftHopSize) { analysisHopCounter = 0; publishAnalysisFrame(); analysisPublishedOnce = true; } } }
void Bifurx::onSampleRateChange(const SampleRateChangeEvent& e) { controlFastCacheValid = false; voctCvFilterInitialized = false; previewFilterInitialized = false; }

void Bifurx::process(const ProcessArgs& args) {
	using PerfClock = std::chrono::steady_clock;
	const bool measurePerf = perfDebugLogging && perfMeasureDivider.process();
	const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
	PerfClock::time_point perfCoreStart, perfPreviewStart, perfAnalysisStart;

	int effectiveCircuitMode = kBifurxTuneSvfOnly ? 0 : activeCircuitMode;
	sanitizeCoreState(coreA); sanitizeCoreState(coreB);

	if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9); params[MODE_PARAM].setValue(float((currentMode + 9) % 10)); }
	if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9); params[MODE_PARAM].setValue(float((currentMode + 1) % 10)); }
	if (!kBifurxTuneSvfOnly && filterCircuitTrigger.process(params[FILTER_CIRCUIT_PARAM].getValue())) setFilterCircuitMode((bifurx::clampCircuitMode(filterCircuitMode) + 1) % kBifurxCircuitModeCount);

	const float in = bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()), level = params[LEVEL_PARAM].getValue(), drive = levelDriveGain(level);
	const int mode = int(std::round(params[MODE_PARAM].getValue())), tito = int(std::round(params[TITO_PARAM].getValue()));
	const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	const float voctCvRaw = voctConnected ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
	if (std::fabs(voctCvFilterSampleRate - args.sampleRate) > 0.5f) { voctCvFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kVoctSmoothingTauSeconds); voctCvFilterSampleRate = args.sampleRate; }
	float voctCv = 0.f;
	if (voctConnected) { if (!voctCvFilterInitialized) { voctCvFiltered = voctCvRaw; voctCvFilterInitialized = true; } else voctCvFiltered += voctCvFilterAlpha * (voctCvRaw - voctCvFiltered); voctCv = (std::fabs(voctCvFiltered) < kVoctDeadbandVolts) ? 0.f : voctCvFiltered; }
	else { voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
	const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f), fmCv = inputs[FM_INPUT].isConnected() ? clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) : 0.f, fm = fmCv * fmAmt, resoCvNorm = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f, resoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f), balanceCvNorm = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f, balanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f), spanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f), spanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f), spanCvNorm = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f, spanNorm = clamp(spanParamNorm + 0.5f * spanAtten * spanCvNorm, 0.f, 1.f), spanOct = 8.f * bifurx::shapedSpan(spanNorm), spanWideMorph = cascadeWideMorph(spanNorm);
	const bool fastPathEligible = (effectiveCircuitMode == BIFURX_CHARACTER_SVF) && (tito == 1) && !voctConnected && !inputs[FM_INPUT].isConnected() && !inputs[RESO_CV_INPUT].isConnected() && !inputs[BALANCE_CV_INPUT].isConnected() && !inputs[SPAN_CV_INPUT].isConnected();
	perfSampleRate.store(args.sampleRate, std::memory_order_relaxed); perfMode.store(mode, std::memory_order_relaxed); perfCircuitMode.store(effectiveCircuitMode, std::memory_order_relaxed); perfFastPathEligible.store(fastPathEligible, std::memory_order_relaxed);
	const bool updateFastControls = !controlFastCacheValid || !fastPathEligible || controlUpdateDivider.process();
	if (std::fabs(previewFilterAlphaSampleRate - args.sampleRate) > 0.5f) { previewFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.05f); previewFilterAlphaSlow = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.20f); previewFilterAlphaSampleRate = args.sampleRate; }

	float freqA0 = cachedFreqA0, freqB0 = cachedFreqB0, dampingA = cachedDampingA, dampingB = cachedDampingB, wA = cachedWA, wB = cachedWB, balance = cachedBalance;
	if (updateFastControls) {
		balance = balanceNorm; const float centerHz = kFreqMinHz * fastExp2(kFreqLog2Span * freqParamNorm) * fastExp2(voctCv + fm), sr = std::max(args.sampleRate, 1.f);
		auto computeCircuitFreqs = [&](int cMode, float *fAOut, float *fBOut) { const float cScale = circuitCutoffScale(cMode), safeCenterHz = clamp(centerHz * cScale, kFreqMinHz, 0.46f * sr), maxShiftUp = std::max(0.f, std::log2((0.46f * sr) / safeCenterHz)), maxShiftDown = std::max(0.f, std::log2(safeCenterHz / kFreqMinHz)), maxSymShift = std::min(maxShiftUp, maxShiftDown), halfSpanOct = std::min(0.5f * spanOct, maxSymShift); if (fAOut) *fAOut = clamp(safeCenterHz * fastExp2(-halfSpanOct), kFreqMinHz, 0.46f * sr); if (fBOut) *fBOut = clamp(safeCenterHz * fastExp2(halfSpanOct), kFreqMinHz, 0.46f * sr); };
		const int sEM = bifurx::clampCircuitMode(effectiveCircuitMode); const float qScale = circuitQScale(resoNorm, sEM); computeCircuitFreqs(sEM, &freqA0, &freqB0); const float baseDamping = resoToDamping(resoNorm) / std::max(qScale, 1e-4f);
		dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f); dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
		const float lowW = signedWeight(balance, false), highW = signedWeight(balance, true), norm = 2.f / (lowW + highW); wA = lowW * norm; wB = highW * norm;
		cachedDampingA = dampingA; cachedDampingB = dampingB; cachedWA = wA; cachedWB = wB; cachedFreqA0 = freqA0; cachedFreqB0 = freqB0; cachedBalance = balance; cachedCoeffsA = makeSvfCoeffs(args.sampleRate, freqA0, dampingA); cachedCoeffsB = makeSvfCoeffs(args.sampleRate, freqB0, dampingB); controlFastCacheValid = true;
	}

	auto titoCharacterScale = [&](int cMode) { switch (bifurx::clampCircuitMode(cMode)) { case BIFURX_CHARACTER_BITE: return 0.92f; case BIFURX_CHARACTER_VOWEL: return 1.05f; case BIFURX_CHARACTER_ERODE: return 0.78f; default: return 1.f; } };
	const float titoModeScale = (tito == 0) ? 1.22f : ((tito == 2) ? 1.10f : 0.f), couplingDepth = titoCharacterScale(effectiveCircuitMode) * titoModeScale * (0.026f + 0.28f * resoNorm * resoNorm), drivenIn = 5.f * bifurx::softClip(0.2f * in * drive), excitation = drivenIn + (resoNorm > 0.985f ? 1e-6f : 0.f);
	float cutoffA = freqA0, cutoffB = freqB0;
	if (!fastPathEligible) { float modA = 0.f, modB = 0.f; if (tito == 2) { modA = couplingDepth * coreA.ic1eq / 5.f; modB = couplingDepth * coreB.ic1eq / 5.f; } else if (tito == 0) { modA = couplingDepth * coreB.ic1eq / 5.f; modB = couplingDepth * coreA.ic1eq / 5.f; } cutoffA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f)); cutoffB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f)); }
	if (measurePerf) perfCoreStart = PerfClock::now();
	float modeOut = 0.f, llExc = 0.f, llA = 0.f, llB = 0.f;
	auto pA = [&](float s) { return processCharacterStage(coreA, effectiveCircuitMode, 0, s, args.sampleRate, cutoffA, dampingA, drive, resoNorm, fastPathEligible ? &cachedCoeffsA : nullptr); };
	auto pB = [&](float s) { return processCharacterStage(coreB, effectiveCircuitMode, 1, s, args.sampleRate, cutoffB, dampingB, drive, resoNorm, fastPathEligible ? &cachedCoeffsB : nullptr); };

	switch (mode) {
		case 0: { const SvfOutputs a = pA(excitation), b = pB(a.lp); llExc = excitation; llA = a.lp; llB = b.lp; modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 1: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 2: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 3: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 4: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 5: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 6: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 7: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		case 8: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
		default: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.hp, wA, wB, spanWideMorph, effectiveCircuitMode); } break;
	}

	const float out = bifurx::sanitizeFinite(5.5f * bifurx::softClip(bifurx::sanitizeFinite(modeOut) / 5.5f));
	outputs[OUT_OUTPUT].setChannels(1); outputs[OUT_OUTPUT].setVoltage(out);
	const float llAlpha = onePoleAlpha(args.sampleTime, kLlTelemetryTauSeconds);
	if (mode == 0) { llTelemetryExcitationSq += llAlpha * (llExc * llExc - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (llA * llA - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (llB * llB - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	else { llTelemetryExcitationSq += llAlpha * (0.f - llTelemetryExcitationSq); llTelemetryStageALpSq += llAlpha * (0.f - llTelemetryStageALpSq); llTelemetryStageBLpSq += llAlpha * (0.f - llTelemetryStageBLpSq); llTelemetryOutputSq += llAlpha * (out * out - llTelemetryOutputSq); }
	if (measurePerf) perfPreviewStart = PerfClock::now();

	const float pTFqA = clamp(freqA0, 4.f, 0.46f * args.sampleRate), pTFqB = clamp(freqB0, 4.f, 0.46f * args.sampleRate), pTQA = 1.f / std::max(dampingA, 0.05f), pTQB = 1.f / std::max(dampingB, 0.05f), pTBal = balance;
	const bool pPitchCvConn = voctConnected || inputs[FM_INPUT].isConnected();
	perfPreviewPitchCvConnected.store(pPitchCvConn, std::memory_order_relaxed);
	const float pSmAlpha = pPitchCvConn ? previewFilterAlphaSlow : previewFilterAlpha;
	if (!previewTargetMotionInitialized) { previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB; previewTargetStillSamples = 0; previewTargetMotionInitialized = true; }
	const float tMAOct = std::fabs(std::log2(std::max(pTFqA, 1.f) / std::max(previewPrevTargetFreqA, 1.f))), tMBOct = std::fabs(std::log2(std::max(pTFqB, 1.f) / std::max(previewPrevTargetFreqB, 1.f))), tMOct = std::max(tMAOct, tMBOct);
	if (tMOct <= kPreviewInstantSettleMotionOctThreshold) previewTargetStillSamples++; else previewTargetStillSamples = 0;
	const bool pInstSettle = (previewTargetStillSamples >= kPreviewInstantSettleHoldSamples);
	previewPrevTargetFreqA = pTFqA; previewPrevTargetFreqB = pTFqB;
	if (!previewFilterInitialized || pInstSettle) { previewFreqAFiltered = pTFqA; previewFreqBFiltered = pTFqB; previewQAFiltered = pTQA; previewQBFiltered = pTQB; previewBalanceFiltered = pTBal; previewFilterInitialized = true; }
	else { const float a = pSmAlpha; previewFreqAFiltered += a * (pTFqA - previewFreqAFiltered); previewFreqBFiltered += a * (pTFqB - previewFreqBFiltered); previewQAFiltered += a * (pTQA - previewQAFiltered); previewQBFiltered += a * (pTQB - previewQBFiltered); previewBalanceFiltered += a * (pTBal - previewBalanceFiltered); }

	BifurxPreviewState pS; pS.sampleRate = args.sampleRate; pS.freqA = previewFreqAFiltered; pS.freqB = previewFreqBFiltered; pS.qA = previewQAFiltered; pS.qB = previewQBFiltered; pS.mode = mode; pS.circuitMode = effectiveCircuitMode; pS.balance = previewBalanceFiltered; pS.balanceTarget = balanceNorm; pS.resoNorm = resoNorm; pS.spanParamNorm = spanParamNorm; pS.spanCvNorm = spanCvNorm; pS.spanAtten = spanAtten; pS.spanNorm = spanNorm; pS.spanOct = spanOct; pS.freqParamNorm = freqParamNorm; pS.voctCv = voctCv;
	if (previewAdaptiveCooldown > 0) previewAdaptiveCooldown--;
	bool adpTick = false;
	if (hasLastPreviewState && previewAdaptiveCooldown <= 0) {
		const float fMA = std::fabs(std::log2(std::max(pS.freqA, 1.f) / std::max(lastPreviewState.freqA, 1.f))), fMB = std::fabs(std::log2(std::max(pS.freqB, 1.f) / std::max(lastPreviewState.freqB, 1.f))), sMO = std::fabs(pS.spanOct - lastPreviewState.spanOct), qMA = std::fabs(pS.qA - lastPreviewState.qA), qMB = std::fabs(pS.qB - lastPreviewState.qB), bM = std::fabs(pS.balance - lastPreviewState.balance);
		if (fMA > kPreviewAdaptiveOctaveThreshold || fMB > kPreviewAdaptiveOctaveThreshold || sMO > kPreviewAdaptiveSpanOctThreshold || qMA > kPreviewAdaptiveQThreshold || qMB > kPreviewAdaptiveQThreshold || bM > kPreviewAdaptiveBalanceThreshold) { adpTick = true; previewAdaptiveCooldown = kPreviewAdaptiveCooldownSamples; }
	}
	const bool perTick = pPitchCvConn ? previewPublishSlowDivider.process() : previewPublishDivider.process();
	if (!hasLastPreviewState || ((perTick || adpTick) && previewStatesDiffer(pS, lastPreviewState))) publishPreviewState(pS);
	if (perTick || adpTick) { BifurxLlTelemetryState llTS; llTS.active = (mode == 0); llTS.circuitMode = effectiveCircuitMode; llTS.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f)); llTS.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f)); llTS.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f)); llTS.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f)); llTS.stageBLpOverALpDb = amplitudeRatioDb(llTS.stageBLpRms, llTS.stageALpRms); llTS.outputOverInputDb = amplitudeRatioDb(llTS.outputRms, llTS.excitationRms); publishLlTelemetryState(llTS); }
	if (measurePerf) perfAnalysisStart = PerfClock::now();
	pushAnalysisSample(in, out);

	lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f)); lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));
	lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f)); lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));
	const int cML = bifurx::clampCircuitMode(filterCircuitMode);
	lights[FILTER_CIRCUIT_TL_LIGHT].setBrightness(cML == 0 ? 1.f : 0.f); lights[FILTER_CIRCUIT_TR_LIGHT].setBrightness(0.f);
	lights[FILTER_CIRCUIT_BR_LIGHT].setBrightness(0.f); lights[FILTER_CIRCUIT_BL_LIGHT].setBrightness(0.f);

	if (measurePerf) {
		const PerfClock::time_point pE = PerfClock::now();
		const uint64_t cNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfCoreStart - perfStart).count();
		const uint64_t crNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfPreviewStart - perfCoreStart).count();
		const uint64_t prNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(perfAnalysisStart - perfPreviewStart).count();
		const uint64_t aNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfAnalysisStart).count(), pNS = (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(pE - perfStart).count();
		perfAudioSampledCount.fetch_add(1, std::memory_order_relaxed); perfAudioProcessNs.fetch_add(pNS, std::memory_order_relaxed);
		perfAudioControlsNs.fetch_add(cNS, std::memory_order_relaxed); perfAudioCoreNs.fetch_add(crNS, std::memory_order_relaxed);
		perfAudioPreviewNs.fetch_add(prNS, std::memory_order_relaxed); perfAudioAnalysisNs.fetch_add(aNS, std::memory_order_relaxed);
		uint64_t pM = perfAudioProcessMaxNs.load(std::memory_order_relaxed);
		while (pNS > pM && !perfAudioProcessMaxNs.compare_exchange_weak(pM, pNS, std::memory_order_relaxed));
	}
}

} // namespace bifurx
