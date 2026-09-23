#include "BifurxModule.hpp"

namespace bifurx {

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
	analysisPublishedToken.store(0, std::memory_order_release);
	analysisGeneration.fetch_add(1, std::memory_order_release);
	coreA = TptSvf {};
	coreB = TptSvf {};
	nonlinearOversampling.reset();
	legacyOversampling.reset();
	iirOversampling.reset();
	transitionSmoother.reset();
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
	titoSmDcCorrection = 0.f;
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
	json_object_set_new(root, "boundaryResampling", json_integer(boundaryResampling.load(std::memory_order_relaxed)));
	json_object_set_new(root, "legacyBoundaryResampling", json_boolean(boundaryResampling.load(std::memory_order_relaxed) == 1));
	json_object_set_new(root, "softLimitingEnabled", json_boolean(softLimitingEnabled.load(std::memory_order_relaxed)));
	json_object_set_new(root, "nonlinearOversamplingEnabled", json_boolean(nonlinearOversamplingEnabled.load(std::memory_order_relaxed)));
	json_object_set_new(root, "renderMode", json_integer(renderMode));
	json_object_set_new(root, "createdUnixTimeSec", json_real(createdUnixTimeSec));
	return root;
}

void Bifurx::dataFromJson(json_t* root) {
	if (!root) {
		return;
	}
	Module::dataFromJson(root);
	const json_t* boundaryJ = json_object_get(root, "boundaryResampling");
	const int savedBoundary = json_is_integer(boundaryJ) ? int(json_integer_value(boundaryJ))
		: json_is_true(json_object_get(root, "legacyBoundaryResampling")) ? 1 : 2;
	boundaryResampling.store(savedBoundary >= 1 && savedBoundary <= 3 ? savedBoundary : 2, std::memory_order_relaxed);
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
	}
	else {
		// Backward compatibility with old two-state control update mode.
		json_t* controlUpdateModeJ = json_object_get(root, "controlUpdateMode");
		if (controlUpdateModeJ) {
			const int legacyMode = int(json_integer_value(controlUpdateModeJ));
			modulationQualityMode.store((legacyMode <= 0) ? MOD_QUALITY_BALANCED : MOD_QUALITY_EXACT, std::memory_order_relaxed);
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
	json_t* nonlinearOversamplingEnabledJ = json_object_get(root, "nonlinearOversamplingEnabled");
	if (nonlinearOversamplingEnabledJ) {
		nonlinearOversamplingEnabled.store(json_is_true(nonlinearOversamplingEnabledJ), std::memory_order_relaxed);
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
void Bifurx::onSampleRateChange(const SampleRateChangeEvent& e) {
	analysisPublishedToken.store(0, std::memory_order_release);
	analysisGeneration.fetch_add(1, std::memory_order_release);
	hasLastPreviewState = false;
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

} // namespace bifurx
