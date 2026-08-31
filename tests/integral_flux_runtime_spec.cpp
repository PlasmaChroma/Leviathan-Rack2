#include "../src/plugin.hpp"

#include <atomic>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

Plugin* pluginInstance = nullptr;

bool isDragonKingDebugEnabled() {
	return false;
}

bool isDragonKingPreviewWidgetOptionsEnabled() {
	return true;
}

bool isModuleTeardownLoggingEnabled() {
	return false;
}

void refreshDragonKingDebugEnabled() {
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int moduleId) {
	this->moduleId = moduleId;
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

#define INTEGRAL_FLUX_HEADLESS_TEST 1
#include "../src/IntegralFlux.cpp"

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

uint64_t hashFloat(uint64_t hash, float value) {
	uint32_t bits = 0u;
	static_assert(sizeof(bits) == sizeof(value), "float hash assumes 32-bit float");
	std::memcpy(&bits, &value, sizeof(bits));
	for (int byte = 0; byte < 4; ++byte) {
		hash ^= uint8_t(bits >> (byte * 8));
		hash *= 1099511628211ull;
	}
	return hash;
}

void connectInput(IntegralFluxImpl& module, int inputId) {
	module.inputs[inputId].channels = 1;
}

void connectOutput(IntegralFluxImpl& module, int outputId) {
	module.outputs[outputId].channels = 1;
}

TestResult releasedSchemaIsStable() {
	const bool pass = IntegralFlux::ATTENUATE_1_PARAM == 0
		&& IntegralFlux::SHAPE_MODE_4_PARAM == 13
		&& IntegralFlux::PARAMS_LEN == 14
		&& IntegralFlux::INPUT_1_INPUT == 0
		&& IntegralFlux::CH4_CYCLE_CV_INPUT == 13
		&& IntegralFlux::INPUTS_LEN == 14
		&& IntegralFlux::OUT_1_OUTPUT == 0
		&& IntegralFlux::EOC_4_OUTPUT == 10
		&& IntegralFlux::OUTPUTS_LEN == 11
		&& IntegralFlux::CYCLE_1_LED_LIGHT == 0
		&& IntegralFlux::INV_LED_LIGHT == 7
		&& IntegralFlux::LIGHTS_LEN == 8;
	return {"released parameter/port/light schema remains append-only", pass,
		pass ? "" : "Integral Flux enum IDs or counts changed"};
}

TestResult persistedSettingsRoundTrip() {
	IntegralFluxImpl source;
	source.ch1.cycleLatched = true;
	source.ch4.cycleLatched = false;
	source.bandlimitedGateOutputs.store(true, std::memory_order_relaxed);
	source.bandlimitedSignalOutputs.store(false, std::memory_order_relaxed);
	source.requestTimingUpdateDiv(8);
	source.timingInterpolate.store(false, std::memory_order_relaxed);
	source.previewTracerEnabled.store(false, std::memory_order_relaxed);
	source.previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed);
	source.previewRenderMode.store(1, std::memory_order_relaxed);

	json_t* state = source.dataToJson();
	IntegralFluxImpl restored;
	restored.dataFromJson(state);
	json_decref(state);

	const bool pass = restored.ch1.cycleLatched
		&& !restored.ch4.cycleLatched
		&& restored.bandlimitedGateOutputs.load(std::memory_order_relaxed)
		&& !restored.bandlimitedSignalOutputs.load(std::memory_order_relaxed)
		&& restored.requestedTimingUpdateDiv.load(std::memory_order_relaxed) == 8
		&& !restored.timingInterpolate.load(std::memory_order_relaxed)
		&& !restored.previewTracerEnabled.load(std::memory_order_relaxed)
		&& restored.previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE
		&& restored.previewRenderMode.load(std::memory_order_relaxed) == 1;
	return {"persisted Integral Flux settings round-trip", pass,
		pass ? "" : "serialized settings did not round-trip exactly"};
}

TestResult previewSnapshotsStayCoherent() {
	IntegralFluxImpl module;
	std::atomic<bool> ready {false};
	std::atomic<bool> done {false};
	std::atomic<bool> coherent {true};
	std::thread writer([&]() {
		for (uint32_t i = 1u; i <= 250000u; ++i) {
			const float token = float(i);
			const bool alternate = (i & 1u) != 0u;
			module.publishPreviewState(
				module.previewCh1,
				token,
				token + 0.5f,
				-token,
				alternate ? IntegralFlux::FUNCTION_SHAPE_SHARK_FIN : IntegralFlux::FUNCTION_SHAPE_MATHS,
				alternate);
			const float dotToken = alternate ? 0.75f : 0.25f;
			module.publishPreviewDot(module.previewCh1, alternate, dotToken, dotToken);
			if (i == 1u) {
				ready.store(true, std::memory_order_release);
			}
		}
		done.store(true, std::memory_order_release);
	});
	while (!ready.load(std::memory_order_acquire)) {
		std::this_thread::yield();
	}
	uint32_t previousVersion = 0u;
	do {
		float rise = 0.f;
		float fall = 0.f;
		float curve = 0.f;
		float dotX = 0.f;
		float dotY = 0.f;
		bool dotVisible = false;
		bool interactive = false;
		IntegralFlux::FunctionShapeMode mode = IntegralFlux::FUNCTION_SHAPE_MATHS;
		uint32_t version = 0u;
		module.getPreviewState(1, rise, fall, curve, dotX, dotY, dotVisible, mode, interactive, version);
		const uint32_t token = uint32_t(rise);
		const bool alternate = (token & 1u) != 0u;
		const bool curveCoherent = fall == rise + 0.5f
			&& curve == -rise
			&& interactive == alternate
			&& mode == (alternate ? IntegralFlux::FUNCTION_SHAPE_SHARK_FIN : IntegralFlux::FUNCTION_SHAPE_MATHS)
			&& version >= previousVersion;
		const bool dotCoherent = dotX == dotY
			&& ((dotX == 0.75f && dotVisible) || (dotX == 0.25f && !dotVisible));
		if (!curveCoherent || !dotCoherent) {
			coherent.store(false, std::memory_order_relaxed);
			break;
		}
		previousVersion = version;
	} while (!done.load(std::memory_order_acquire));
	writer.join();
	return {"preview curve and dot publications remain coherent", coherent.load(std::memory_order_relaxed),
		coherent.load(std::memory_order_relaxed) ? "" : "reader observed fields from mixed publications"};
}

uint64_t renderModulatedTrace(int timingDiv, bool interpolate) {
	IntegralFluxImpl module;
	module.bandlimitedSignalOutputs.store(false, std::memory_order_relaxed);
	module.bandlimitedGateOutputs.store(false, std::memory_order_relaxed);
	module.requestTimingUpdateDiv(timingDiv);
	module.timingInterpolate.store(interpolate, std::memory_order_relaxed);
	module.ch1.cycleLatched = true;
	module.ch4.cycleLatched = true;

	for (int inputId : {
		IntegralFlux::INPUT_1_INPUT,
		IntegralFlux::INPUT_4_INPUT,
		IntegralFlux::CH1_RISE_CV_INPUT,
		IntegralFlux::CH1_FALL_CV_INPUT,
		IntegralFlux::CH1_BOTH_CV_INPUT,
		IntegralFlux::CH4_RISE_CV_INPUT,
		IntegralFlux::CH4_FALL_CV_INPUT,
		IntegralFlux::CH4_BOTH_CV_INPUT
	}) {
		connectInput(module, inputId);
	}
	for (int outputId = 0; outputId < IntegralFlux::OUTPUTS_LEN; ++outputId) {
		connectOutput(module, outputId);
	}

	Module::ProcessArgs args;
	args.sampleRate = 48000.f;
	args.sampleTime = 1.f / args.sampleRate;
	uint64_t hash = 1469598103934665603ull;
	for (int frame = 0; frame < 131072; ++frame) {
		args.frame = frame;
		const float rise1 = float((frame * 17 + 19) % 1001) * (1.f / 1000.f);
		const float fall1 = float((frame * 29 + 31) % 1001) * (1.f / 1000.f);
		const float rise4 = float((frame * 43 + 47) % 1001) * (1.f / 1000.f);
		const float fall4 = float((frame * 59 + 61) % 1001) * (1.f / 1000.f);
		module.params[IntegralFlux::RISE_1_PARAM].setValue(rise1);
		module.params[IntegralFlux::FALL_1_PARAM].setValue(fall1);
		module.params[IntegralFlux::RISE_4_PARAM].setValue(rise4);
		module.params[IntegralFlux::FALL_4_PARAM].setValue(fall4);
		module.params[IntegralFlux::LIN_LOG_1_PARAM].setValue(float((frame / 97) % 101) * 0.01f);
		module.params[IntegralFlux::LIN_LOG_4_PARAM].setValue(float((frame / 131) % 101) * 0.01f);
		module.params[IntegralFlux::SHAPE_MODE_1_PARAM].setValue(((frame / 4096) & 1) ? 1.f : 0.f);
		module.params[IntegralFlux::SHAPE_MODE_4_PARAM].setValue(((frame / 6144) & 1) ? 0.f : 1.f);
		module.inputs[IntegralFlux::CH1_RISE_CV_INPUT].setVoltage(float((frame * 67) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::CH1_FALL_CV_INPUT].setVoltage(float((frame * 71) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::CH1_BOTH_CV_INPUT].setVoltage(float((frame * 73) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::CH4_RISE_CV_INPUT].setVoltage(float((frame * 79) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::CH4_FALL_CV_INPUT].setVoltage(float((frame * 83) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::CH4_BOTH_CV_INPUT].setVoltage(float((frame * 89) % 2001 - 1000) * 0.007f);
		module.inputs[IntegralFlux::INPUT_1_INPUT].setVoltage(float((frame % 257) - 128) * 0.03125f);
		module.inputs[IntegralFlux::INPUT_4_INPUT].setVoltage(float((frame % 193) - 96) * -0.041666667f);
		module.process(args);

		for (int outputId = 0; outputId < IntegralFlux::OUTPUTS_LEN; ++outputId) {
			hash = hashFloat(hash, module.outputs[outputId].getVoltage());
		}
		hash = hashFloat(hash, module.ch1.activeRiseTime);
		hash = hashFloat(hash, module.ch1.activeFallTime);
		hash = hashFloat(hash, module.ch1.phasePos);
		hash = hashFloat(hash, module.ch4.activeRiseTime);
		hash = hashFloat(hash, module.ch4.activeFallTime);
		hash = hashFloat(hash, module.ch4.phasePos);
	}
	return hash;
}

TestResult modulatedTraceMatchesReleasedBaseline() {
	const uint64_t div1Hash = renderModulatedTrace(1, true);
	const uint64_t div8Hash = renderModulatedTrace(8, true);
	// Filled from the pre-refactor native Windows implementation. These hashes
	// intentionally include engine stage state as well as every output voltage.
	constexpr uint64_t expectedDiv1Hash = 8388581365298595617ull;
	constexpr uint64_t expectedDiv8Hash = 1427530046052129200ull;
	const bool pass = div1Hash == expectedDiv1Hash && div8Hash == expectedDiv8Hash;
	return {"modulated audio/state trace remains bit-identical", pass,
		"actual /1=" + std::to_string(div1Hash) + ", /8=" + std::to_string(div8Hash)};
}

} // namespace

int main() {
	const std::vector<TestResult> results {
		releasedSchemaIsStable(),
		persistedSettingsRoundTrip(),
		previewSnapshotsStayCoherent(),
		modulatedTraceMatchesReleasedBaseline(),
	};
	bool allPassed = true;
	for (const TestResult& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name;
		if (!result.detail.empty()) {
			std::cout << " :: " << result.detail;
		}
		std::cout << '\n';
		allPassed = allPassed && result.pass;
	}
	return allPassed ? 0 : 1;
}
