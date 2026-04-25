#include "Bifurx.hpp"

namespace bifurx {

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
	int stageIndex,
	float input,
	float sampleRate,
	float cutoff,
	float damping,
	float drive,
	float resoNorm,
	const SvfCoeffs* cachedCoeffsOrNull
) {
	(void) stageIndex;
	(void) drive;
	(void) resoNorm;
	SvfOutputs raw;
	if (cachedCoeffsOrNull) {
		raw = core.processWithCoeffs(input, *cachedCoeffsOrNull);
	}
	else {
		raw = core.process(input, sampleRate, cutoff, damping);
	}
	return raw;
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

bool previewStatesDiffer(const BifurxPreviewState& a, const BifurxPreviewState& b) {
	if (a.mode != b.mode) return true;
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
	const std::complex<float> lpA = model.lowA.response(omega);
	const std::complex<float> bpA = model.bandA.response(omega);
	const std::complex<float> hpA = model.highA.response(omega);
	const std::complex<float> lpB = model.lowB.response(omega);
	const std::complex<float> bpB = model.bandB.response(omega);
	const std::complex<float> hpB = model.highB.response(omega);
	const std::complex<float> ntA = lpA + hpA, ntB = lpB + hpB, cascadeLp = lpB * lpA, cascadeNotch = ntB * ntA, cascadeHpToLp = lpB * hpA, cascadeHpToHp = hpB * hpA;
	return combineModeResponse<std::complex<float>>(model.mode, lpA, bpA, hpA, ntA, lpB, bpB, hpB, ntB, cascadeLp, cascadeNotch, cascadeHpToLp, cascadeHpToHp, model.wA, model.wB, model.wideMorph);
}

float previewModelResponseDb(const BifurxPreviewModel& model, float hz) {
	const float mag = std::abs(previewModelResponse(model, hz));
	return 20.f * std::log10(std::max(mag, 1e-5f));
}

Bifurx::Bifurx() {
	config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
	configSwitch(MODE_PARAM, 0.f, 9.f, 0.f, "Mode", {kBifurxModeLabels[0], kBifurxModeLabels[1], kBifurxModeLabels[2], kBifurxModeLabels[3], kBifurxModeLabels[4], kBifurxModeLabels[5], kBifurxModeLabels[6], kBifurxModeLabels[7], kBifurxModeLabels[8], kBifurxModeLabels[9]});
	configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level"); configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency"); configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance"); configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance"); configParam(SPAN_PARAM, 0.f, 1.f, 0.5f, "Span"); configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount"); configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator"); configParam(TITO_PARAM, -1.f, 1.f, 0.f, "TITO strength"); configButton(MODE_LEFT_PARAM, "Mode previous"); configButton(MODE_RIGHT_PARAM, "Mode next");
	configInput(IN_INPUT, "Signal In"); configInput(VOCT_INPUT, "V/Oct"); configInput(FM_INPUT, "FM"); configInput(RESO_CV_INPUT, "Resonance CV"); configInput(BALANCE_CV_INPUT, "Balance CV"); configInput(SPAN_CV_INPUT, "Span CV"); configOutput(OUT_OUTPUT, "Signal Out"); configBypass(IN_INPUT, OUT_OUTPUT);
	paramQuantities[MODE_PARAM]->snapEnabled = true;
	previewPublishDivider.setDivision(kPreviewPublishFastDivision); previewPublishSlowDivider.setDivision(kPreviewPublishSlowDivision); controlUpdateDivider.setDivision(16); perfMeasureDivider.setDivision(64);
}

void Bifurx::resetCircuitStates() { coreA.ic1eq = 0.f; coreA.ic2eq = 0.f; coreB.ic1eq = 0.f; coreB.ic2eq = 0.f; llTelemetryExcitationSq = 0.f; llTelemetryStageALpSq = 0.f; llTelemetryStageBLpSq = 0.f; llTelemetryOutputSq = 0.f; voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
json_t* Bifurx::dataToJson() { json_t* root = Module::dataToJson(); json_object_set_new(root, "fftScaleDynamic", json_boolean(fftScaleDynamic)); json_object_set_new(root, "curveDebugLogging", json_boolean(curveDebugLogging)); json_object_set_new(root, "perfDebugLogging", json_boolean(perfDebugLogging)); return root; }
void Bifurx::dataFromJson(json_t* root) { Module::dataFromJson(root); json_t* fftScaleDynamicJ = json_object_get(root, "fftScaleDynamic"); if (fftScaleDynamicJ) fftScaleDynamic = json_is_true(fftScaleDynamicJ); json_t* curveDebugLoggingJ = json_object_get(root, "curveDebugLogging"); if (curveDebugLoggingJ) curveDebugLogging = json_is_true(curveDebugLoggingJ); json_t* perfDebugLoggingJ = json_object_get(root, "perfDebugLogging"); if (perfDebugLoggingJ) perfDebugLogging = json_is_true(perfDebugLoggingJ); }
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

	sanitizeCoreState(coreA); sanitizeCoreState(coreB);

	if (modeLeftTrigger.process(params[MODE_LEFT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9); params[MODE_PARAM].setValue(float((currentMode + 9) % 10)); }
	if (modeRightTrigger.process(params[MODE_RIGHT_PARAM].getValue())) { const int currentMode = clamp(int(std::round(params[MODE_PARAM].getValue())), 0, 9); params[MODE_PARAM].setValue(float((currentMode + 1) % 10)); }

	const float in = bifurx::sanitizeFinite(inputs[IN_INPUT].getVoltage()), level = params[LEVEL_PARAM].getValue(), drive = levelDriveGain(level);
	const int mode = int(std::round(params[MODE_PARAM].getValue()));
	const float tito = clamp(params[TITO_PARAM].getValue(), -1.f, 1.f);
	const float titoAbs = std::fabs(tito);
	const bool titoNeutral = titoAbs < 0.02f;
	const float freqParamNorm = clamp(params[FREQ_PARAM].getValue(), 0.f, 1.f);
	const bool voctConnected = inputs[VOCT_INPUT].isConnected();
	const float voctCvRaw = voctConnected ? clamp(inputs[VOCT_INPUT].getVoltage(), -10.f, 10.f) : 0.f;
	if (std::fabs(voctCvFilterSampleRate - args.sampleRate) > 0.5f) { voctCvFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), kVoctSmoothingTauSeconds); voctCvFilterSampleRate = args.sampleRate; }
	float voctCv = 0.f;
	if (voctConnected) { if (!voctCvFilterInitialized) { voctCvFiltered = voctCvRaw; voctCvFilterInitialized = true; } else voctCvFiltered += voctCvFilterAlpha * (voctCvRaw - voctCvFiltered); voctCv = (std::fabs(voctCvFiltered) < kVoctDeadbandVolts) ? 0.f : voctCvFiltered; }
	else { voctCvFiltered = 0.f; voctCvFilterInitialized = false; }
	const float fmAmt = clamp(params[FM_AMT_PARAM].getValue(), -1.f, 1.f), fmCv = inputs[FM_INPUT].isConnected() ? clamp(inputs[FM_INPUT].getVoltage(), -10.f, 10.f) : 0.f, fm = fmCv * fmAmt, resoCvNorm = clamp(inputs[RESO_CV_INPUT].getVoltage(), 0.f, 8.f) / 8.f, resoNorm = clamp(params[RESO_PARAM].getValue() + resoCvNorm, 0.f, 1.f), balanceCvNorm = clamp(inputs[BALANCE_CV_INPUT].getVoltage(), -5.f, 5.f) / 5.f, balanceNorm = clamp(params[BALANCE_PARAM].getValue() + balanceCvNorm, -1.f, 1.f), spanParamNorm = clamp(params[SPAN_PARAM].getValue(), 0.f, 1.f), spanAtten = clamp(params[SPAN_CV_ATTEN_PARAM].getValue(), -1.f, 1.f), spanCvNorm = clamp(inputs[SPAN_CV_INPUT].getVoltage(), -10.f, 10.f) / 5.f, spanNorm = clamp(spanParamNorm + 0.5f * spanAtten * spanCvNorm, 0.f, 1.f), spanOct = 8.f * bifurx::shapedSpan(spanNorm), spanWideMorph = cascadeWideMorph(spanNorm);
	const bool fastPathEligible = titoNeutral && !voctConnected && !inputs[FM_INPUT].isConnected() && !inputs[RESO_CV_INPUT].isConnected() && !inputs[BALANCE_CV_INPUT].isConnected() && !inputs[SPAN_CV_INPUT].isConnected();
	perfSampleRate.store(args.sampleRate, std::memory_order_relaxed); perfMode.store(mode, std::memory_order_relaxed); perfFastPathEligible.store(fastPathEligible, std::memory_order_relaxed);
	const bool updateFastControls = !controlFastCacheValid || !fastPathEligible || controlUpdateDivider.process();
	if (std::fabs(previewFilterAlphaSampleRate - args.sampleRate) > 0.5f) { previewFilterAlpha = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.05f); previewFilterAlphaSlow = onePoleAlpha(1.f / std::max(args.sampleRate, 1.f), 0.20f); previewFilterAlphaSampleRate = args.sampleRate; }

	float freqA0 = cachedFreqA0, freqB0 = cachedFreqB0, dampingA = cachedDampingA, dampingB = cachedDampingB, wA = cachedWA, wB = cachedWB, balance = cachedBalance;
	if (updateFastControls) {
		balance = balanceNorm; const float centerHz = kFreqMinHz * fastExp2(kFreqLog2Span * freqParamNorm) * fastExp2(voctCv + fm), sr = std::max(args.sampleRate, 1.f);
		const float safeCenterHz = clamp(centerHz, kFreqMinHz, 0.46f * sr), maxShiftUp = std::max(0.f, std::log2((0.46f * sr) / safeCenterHz)), maxShiftDown = std::max(0.f, std::log2(safeCenterHz / kFreqMinHz)), maxSymShift = std::min(maxShiftUp, maxShiftDown), halfSpanOct = std::min(0.5f * spanOct, maxSymShift);
		freqA0 = clamp(safeCenterHz * fastExp2(-halfSpanOct), kFreqMinHz, 0.46f * sr); freqB0 = clamp(safeCenterHz * fastExp2(halfSpanOct), kFreqMinHz, 0.46f * sr); const float baseDamping = resoToDamping(resoNorm);
		dampingA = clamp(baseDamping * fastExp(0.48f * balance), 0.02f, 2.2f); dampingB = clamp(baseDamping * fastExp(-0.48f * balance), 0.02f, 2.2f);
		const float lowW = signedWeight(balance, false), highW = signedWeight(balance, true), norm = 2.f / (lowW + highW); wA = lowW * norm; wB = highW * norm;
		cachedDampingA = dampingA; cachedDampingB = dampingB; cachedWA = wA; cachedWB = wB; cachedFreqA0 = freqA0; cachedFreqB0 = freqB0; cachedBalance = balance; cachedCoeffsA = makeSvfCoeffs(args.sampleRate, freqA0, dampingA); cachedCoeffsB = makeSvfCoeffs(args.sampleRate, freqB0, dampingB); controlFastCacheValid = true;
	}

	const float titoModeScale = 1.22f;
	const float titoStrength = 2.f * titoAbs;
	const float couplingDepth = titoStrength * titoModeScale * (0.026f + 0.28f * resoNorm * resoNorm);
	const float drivenIn = 5.f * bifurx::softClip(0.2f * in * drive), excitation = drivenIn + (resoNorm > 0.985f ? 1e-6f : 0.f);
	float cutoffA = freqA0, cutoffB = freqB0;
	if (!fastPathEligible) { float modA = 0.f, modB = 0.f; if (tito < 0.f) { modA = couplingDepth * coreA.ic1eq / 5.f; modB = couplingDepth * coreB.ic1eq / 5.f; } else if (tito > 0.f) { modA = couplingDepth * coreB.ic1eq / 5.f; modB = couplingDepth * coreA.ic1eq / 5.f; } cutoffA = freqA0 * fastExp2(clamp(modA, -2.5f, 2.5f)); cutoffB = freqB0 * fastExp2(clamp(modB, -2.5f, 2.5f)); }
	if (measurePerf) perfCoreStart = PerfClock::now();
	float modeOut = 0.f, llExc = 0.f, llA = 0.f, llB = 0.f;
	auto pA = [&](float s) { return processCharacterStage(coreA, 0, s, args.sampleRate, cutoffA, dampingA, drive, resoNorm, fastPathEligible ? &cachedCoeffsA : nullptr); };
	auto pB = [&](float s) { return processCharacterStage(coreB, 1, s, args.sampleRate, cutoffB, dampingB, drive, resoNorm, fastPathEligible ? &cachedCoeffsB : nullptr); };

	switch (mode) {
		case 0: { const SvfOutputs a = pA(excitation), b = pB(a.lp); llExc = excitation; llA = a.lp; llB = b.lp; modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, b.lp, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 1: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 2: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 3: { const SvfOutputs a = pA(excitation), b = pB(a.notch); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, b.notch, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 4: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 5: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 6: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, b.lp, 0.f, wA, wB, spanWideMorph); } break;
		case 7: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		case 8: { const SvfOutputs a = pA(excitation), b = pB(excitation); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, 0.f, wA, wB, spanWideMorph); } break;
		default: { const SvfOutputs a = pA(excitation), b = pB(a.hp); modeOut = combineModeResponse<float>(mode, a.lp, a.bp, a.hp, a.notch, b.lp, b.bp, b.hp, b.notch, 0.f, 0.f, 0.f, b.hp, wA, wB, spanWideMorph); } break;
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

	BifurxPreviewState pS; pS.sampleRate = args.sampleRate; pS.freqA = previewFreqAFiltered; pS.freqB = previewFreqBFiltered; pS.qA = previewQAFiltered; pS.qB = previewQBFiltered; pS.mode = mode; pS.balance = previewBalanceFiltered; pS.balanceTarget = balanceNorm; pS.resoNorm = resoNorm; pS.spanParamNorm = spanParamNorm; pS.spanCvNorm = spanCvNorm; pS.spanAtten = spanAtten; pS.spanNorm = spanNorm; pS.spanOct = spanOct; pS.freqParamNorm = freqParamNorm; pS.voctCv = voctCv;
	if (previewAdaptiveCooldown > 0) previewAdaptiveCooldown--;
	bool adpTick = false;
	if (hasLastPreviewState && previewAdaptiveCooldown <= 0) {
		const float fMA = std::fabs(std::log2(std::max(pS.freqA, 1.f) / std::max(lastPreviewState.freqA, 1.f))), fMB = std::fabs(std::log2(std::max(pS.freqB, 1.f) / std::max(lastPreviewState.freqB, 1.f))), sMO = std::fabs(pS.spanOct - lastPreviewState.spanOct), qMA = std::fabs(pS.qA - lastPreviewState.qA), qMB = std::fabs(pS.qB - lastPreviewState.qB), bM = std::fabs(pS.balance - lastPreviewState.balance);
		if (fMA > kPreviewAdaptiveOctaveThreshold || fMB > kPreviewAdaptiveOctaveThreshold || sMO > kPreviewAdaptiveSpanOctThreshold || qMA > kPreviewAdaptiveQThreshold || qMB > kPreviewAdaptiveQThreshold || bM > kPreviewAdaptiveBalanceThreshold) { adpTick = true; previewAdaptiveCooldown = kPreviewAdaptiveCooldownSamples; }
	}
	const bool perTick = pPitchCvConn ? previewPublishSlowDivider.process() : previewPublishDivider.process();
	if (!hasLastPreviewState || ((perTick || adpTick) && previewStatesDiffer(pS, lastPreviewState))) publishPreviewState(pS);
	if (perTick || adpTick) { BifurxLlTelemetryState llTS; llTS.active = (mode == 0); llTS.excitationRms = std::sqrt(std::max(llTelemetryExcitationSq, 0.f)); llTS.stageALpRms = std::sqrt(std::max(llTelemetryStageALpSq, 0.f)); llTS.stageBLpRms = std::sqrt(std::max(llTelemetryStageBLpSq, 0.f)); llTS.outputRms = std::sqrt(std::max(llTelemetryOutputSq, 0.f)); llTS.stageBLpOverALpDb = amplitudeRatioDb(llTS.stageBLpRms, llTS.stageALpRms); llTS.outputOverInputDb = amplitudeRatioDb(llTS.outputRms, llTS.excitationRms); publishLlTelemetryState(llTS); }
	if (measurePerf) perfAnalysisStart = PerfClock::now();
	pushAnalysisSample(in, out);

	lights[FM_AMT_POS_LIGHT].setBrightness(std::max(fmAmt, 0.f)); lights[FM_AMT_NEG_LIGHT].setBrightness(std::max(-fmAmt, 0.f));
	lights[SPAN_CV_ATTEN_POS_LIGHT].setBrightness(std::max(spanAtten, 0.f)); lights[SPAN_CV_ATTEN_NEG_LIGHT].setBrightness(std::max(-spanAtten, 0.f));

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
