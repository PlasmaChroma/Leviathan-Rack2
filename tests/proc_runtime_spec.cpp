#include "../src/plugin.hpp"

#include <atomic>
#include <cmath>
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

#define PROC_HEADLESS_TEST 1
#include "../src/Proc.cpp"

namespace {

struct TestResult {
	std::string name;
	bool pass = false;
	std::string detail;
};

struct TraceHealth {
	bool finite = true;
	bool bounded = true;
	float minValue = INFINITY;
	float maxValue = -INFINITY;
	uint64_t samples = 0u;

	void observe(float value) {
		finite = finite && std::isfinite(value);
		bounded = bounded && std::fabs(value) <= 1e6f;
		if (std::isfinite(value)) {
			minValue = std::min(minValue, value);
			maxValue = std::max(maxValue, value);
		}
		++samples;
	}

	bool hasMeaningfulActivity() const {
		return finite && bounded && samples > 0u && maxValue - minValue > 0.1f;
	}
};

TraceHealth* activeTraceHealth = nullptr;

uint64_t hashFloat(uint64_t hash, float value) {
	if (activeTraceHealth) activeTraceHealth->observe(value);
	uint32_t bits = 0u;
	std::memcpy(&bits, &value, sizeof(bits));
	for (int byte = 0; byte < 4; ++byte) {
		hash ^= uint8_t(bits >> (byte * 8));
		hash *= 1099511628211ull;
	}
	return hash;
}

void connectInput(Proc& module, int inputId) {
	module.inputs[inputId].channels = 1;
}

void connectOutput(Proc& module, int outputId) {
	module.outputs[outputId].channels = 1;
}

Module::ProcessArgs processArgs(float sampleRate = 48000.f) {
	Module::ProcessArgs args;
	args.sampleRate = sampleRate;
	args.sampleTime = 1.f / sampleRate;
	return args;
}

bool nearlyEqual(float actual, float expected, float tolerance = 1e-4f) {
	return std::fabs(actual - expected) <= tolerance;
}

TestResult releasedSchemaIsStable() {
	const bool pass = Proc::CYCLE_PARAM == 0
		&& Proc::AMP_PARAM == 4
		&& Proc::PARAMS_LEN == 5
		&& Proc::SIGNAL_INPUT == 0
		&& Proc::FALL_CV_INPUT == 5
		&& Proc::INPUTS_LEN == 6
		&& Proc::EOR_OUTPUT == 0
		&& Proc::NEG_OUTPUT == 3
		&& Proc::OUTPUTS_LEN == 4
		&& Proc::CYCLE_LIGHT == 0
		&& Proc::NEG_LIGHT == 4
		&& Proc::LIGHTS_LEN == 5;
	return {"released Proc parameter/port/light schema remains append-only", pass,
		pass ? "" : "Proc enum IDs or counts changed"};
}

TestResult persistedSettingsRoundTrip() {
	Proc source;
	source.channel.cycleLatched = true;
	source.bandlimitedGateOutputs.store(true, std::memory_order_relaxed);
	source.bandlimitedSignalOutputs.store(false, std::memory_order_relaxed);
	source.requestTimingUpdateDiv(16);
	source.timingInterpolate.store(false, std::memory_order_relaxed);
	source.previewTracerEnabled.store(false, std::memory_order_relaxed);
	source.previewTracerCacheMode.store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed);
	json_t* state = source.dataToJson();
	Proc restored;
	restored.dataFromJson(state);
	json_decref(state);
	const bool pass = restored.channel.cycleLatched
		&& restored.bandlimitedGateOutputs.load(std::memory_order_relaxed)
		&& !restored.bandlimitedSignalOutputs.load(std::memory_order_relaxed)
		&& restored.requestedTimingUpdateDiv.load(std::memory_order_relaxed) == 16
		&& !restored.timingInterpolate.load(std::memory_order_relaxed)
		&& !restored.previewTracerEnabled.load(std::memory_order_relaxed)
		&& restored.previewTracerCacheMode.load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE;
	return {"persisted Proc settings round-trip", pass,
		pass ? "" : "serialized Proc settings did not round-trip exactly"};
}

TestResult legacyCycleStateStillLoads() {
	json_t* state = json_object();
	json_object_set_new(state, "ch1CycleLatched", json_true());
	Proc module;
	module.dataFromJson(state);
	json_decref(state);
	return {"legacy Proc cycle key still loads", module.channel.cycleLatched,
		module.channel.cycleLatched ? "" : "ch1CycleLatched compatibility was lost"};
}

TestResult previewSnapshotsStayCoherent() {
	Proc module;
	std::atomic<bool> ready {false};
	std::atomic<bool> done {false};
	bool coherent = true;
	std::thread writer([&]() {
		for (uint32_t i = 1u; i <= 250000u; ++i) {
			const float token = float(i);
			const bool alternate = (i & 1u) != 0u;
			module.publishPreviewState(module.previewState, token, token + 0.5f, -token, alternate);
			const float dotToken = alternate ? 0.75f : 0.25f;
			module.publishPreviewDot(module.previewState, alternate, dotToken, dotToken);
			if (i == 1u) ready.store(true, std::memory_order_release);
		}
		done.store(true, std::memory_order_release);
	});
	while (!ready.load(std::memory_order_acquire)) std::this_thread::yield();
	uint32_t previousVersion = 0u;
	do {
		float rise = 0.f;
		float fall = 0.f;
		float curve = 0.f;
		float dotX = 0.f;
		float dotY = 0.f;
		bool dotVisible = false;
		bool interactive = false;
		uint32_t version = 0u;
		module.getPreviewState(rise, fall, curve, dotX, dotY, dotVisible, interactive, version);
		const bool alternate = (uint32_t(rise) & 1u) != 0u;
		if (fall != rise + 0.5f || curve != -rise || interactive != alternate
			|| version < previousVersion || dotX != dotY
			|| !((dotX == 0.75f && dotVisible) || (dotX == 0.25f && !dotVisible))) {
			coherent = false;
			break;
		}
		previousVersion = version;
	} while (!done.load(std::memory_order_acquire));
	writer.join();
	return {"Proc preview curve and dot publications remain coherent", coherent,
		coherent ? "" : "reader observed fields from mixed Proc publications"};
}

TestResult slewModeTracksSignalWhileIdle() {
	Proc module;
	connectInput(module, Proc::SIGNAL_INPUT);
	connectOutput(module, Proc::MAIN_OUTPUT);
	connectOutput(module, Proc::NEG_OUTPUT);
	module.params[Proc::RISE_PARAM].setValue(0.f);
	module.params[Proc::FALL_PARAM].setValue(0.f);
	module.params[Proc::SHAPE_PARAM].setValue(Proc::LINEAR_SHAPE);
	Module::ProcessArgs args = processArgs();

	module.inputs[Proc::SIGNAL_INPUT].setVoltage(6.f);
	for (int frame = 0; frame < 1024; ++frame) {
		args.frame = frame;
		module.process(args);
	}
	const bool roseToTarget = module.channel.phase == Proc::CHANNEL_IDLE
		&& nearlyEqual(module.channel.out, 6.f)
		&& nearlyEqual(module.outputs[Proc::MAIN_OUTPUT].getVoltage(), 6.f)
		&& nearlyEqual(module.outputs[Proc::NEG_OUTPUT].getVoltage(), -6.f);

	module.inputs[Proc::SIGNAL_INPUT].setVoltage(2.f);
	for (int frame = 1024; frame < 2048; ++frame) {
		args.frame = frame;
		module.process(args);
	}
	const bool fellToTarget = module.channel.phase == Proc::CHANNEL_IDLE
		&& nearlyEqual(module.channel.out, 2.f)
		&& nearlyEqual(module.outputs[Proc::MAIN_OUTPUT].getVoltage(), 2.f)
		&& nearlyEqual(module.outputs[Proc::NEG_OUTPUT].getVoltage(), -2.f);
	const bool pass = roseToTarget && fellToTarget;
	return {"Proc idle mode remains a bidirectional shaped slew", pass,
		pass ? "" : "Proc slew mode failed to settle or invert its signal output"};
}

TestResult triggerAndRetriggerContractIsStable() {
	Proc module;
	connectInput(module, Proc::TRIGGER_INPUT);
	connectOutput(module, Proc::MAIN_OUTPUT);
	module.params[Proc::RISE_PARAM].setValue(0.25f);
	module.params[Proc::FALL_PARAM].setValue(0.25f);
	module.params[Proc::SHAPE_PARAM].setValue(Proc::LINEAR_SHAPE);
	Module::ProcessArgs args = processArgs();
	int frame = 0;
	auto processVoltage = [&](float voltage) {
		module.inputs[Proc::TRIGGER_INPUT].setVoltage(voltage);
		args.frame = frame++;
		module.process(args);
	};

	processVoltage(0.f);
	processVoltage(10.f);
	const bool initialTriggerAccepted = module.channel.phase == Proc::CHANNEL_RISE;
	processVoltage(0.f);
	for (int i = 0; i < 32 && module.channel.phase == Proc::CHANNEL_RISE; ++i) {
		processVoltage(0.f);
	}
	const float phaseBeforeRiseRetrigger = module.channel.phasePos;
	processVoltage(10.f);
	const bool riseRetriggerIgnored = module.channel.phase == Proc::CHANNEL_RISE
		&& module.channel.phasePos > phaseBeforeRiseRetrigger;

	processVoltage(0.f);
	for (int i = 0; i < 1000000 && module.channel.phase != Proc::CHANNEL_FALL; ++i) {
		processVoltage(0.f);
	}
	for (int i = 0; i < 32 && module.channel.phase == Proc::CHANNEL_FALL; ++i) {
		processVoltage(0.f);
	}
	const float outputBeforeFallRetrigger = module.channel.out;
	processVoltage(10.f);
	const bool fallRetriggerAccepted = outputBeforeFallRetrigger > 0.f
		&& module.channel.phase == Proc::CHANNEL_RISE
		&& module.channel.out < outputBeforeFallRetrigger;
	const bool pass = initialTriggerAccepted && riseRetriggerIgnored && fallRetriggerAccepted;
	return {"Proc trigger ignores RISE and restarts from FALL", pass,
		pass ? "" : "Proc trigger phase/rearm behavior changed"};
}

TestResult haltFreezesAndResumesActiveCycle() {
	Proc module;
	connectInput(module, Proc::HALT_INPUT);
	connectOutput(module, Proc::MAIN_OUTPUT);
	module.channel.cycleLatched = true;
	module.params[Proc::RISE_PARAM].setValue(0.1f);
	module.params[Proc::FALL_PARAM].setValue(0.1f);
	module.params[Proc::SHAPE_PARAM].setValue(Proc::LINEAR_SHAPE);
	Module::ProcessArgs args = processArgs();
	for (int frame = 0; frame < 64; ++frame) {
		args.frame = frame;
		module.process(args);
	}
	module.inputs[Proc::HALT_INPUT].setVoltage(10.f);
	args.frame = 64;
	module.process(args);
	const Proc::ChannelPhase heldPhase = module.channel.phase;
	const float heldPhasePos = module.channel.phasePos;
	const float heldOut = module.channel.out;
	const float heldRendered = module.outputs[Proc::MAIN_OUTPUT].getVoltage();
	for (int frame = 65; frame < 321; ++frame) {
		args.frame = frame;
		module.process(args);
	}
	const bool heldExactly = module.channel.phase == heldPhase
		&& module.channel.phasePos == heldPhasePos
		&& module.channel.out == heldOut
		&& module.outputs[Proc::MAIN_OUTPUT].getVoltage() == heldRendered;
	module.inputs[Proc::HALT_INPUT].setVoltage(0.f);
	args.frame = 321;
	module.process(args);
	const bool resumed = module.channel.phasePos != heldPhasePos || module.channel.phase != heldPhase;
	const bool pass = heldPhase != Proc::CHANNEL_IDLE && heldExactly && resumed;
	return {"Proc HALT freezes and resumes the active cycle", pass,
		pass ? "" : "HALT no longer holds phase/output exactly or failed to resume"};
}

uint64_t renderModulatedTrace(int timingDiv, bool interpolate) {
	Proc module;
	module.bandlimitedSignalOutputs.store(false, std::memory_order_relaxed);
	module.bandlimitedGateOutputs.store(false, std::memory_order_relaxed);
	module.requestTimingUpdateDiv(timingDiv);
	module.timingInterpolate.store(interpolate, std::memory_order_relaxed);
	module.channel.cycleLatched = true;
	for (int inputId = 0; inputId < Proc::INPUTS_LEN; ++inputId) {
		module.inputs[inputId].channels = 1;
	}
	for (int outputId = 0; outputId < Proc::OUTPUTS_LEN; ++outputId) {
		module.outputs[outputId].channels = 1;
	}
	Module::ProcessArgs args;
	args.sampleRate = 48000.f;
	args.sampleTime = 1.f / args.sampleRate;
	uint64_t hash = 1469598103934665603ull;
	for (int frame = 0; frame < 131072; ++frame) {
		args.frame = frame;
		module.params[Proc::RISE_PARAM].setValue(float((frame * 17 + 19) % 1001) * 0.001f);
		module.params[Proc::FALL_PARAM].setValue(float((frame * 29 + 31) % 1001) * 0.001f);
		module.params[Proc::SHAPE_PARAM].setValue(float((frame / 97) % 101) * 0.01f);
		module.params[Proc::AMP_PARAM].setValue(2.f + float((frame / 257) % 801) * 0.01f);
		module.inputs[Proc::RISE_CV_INPUT].setVoltage(float((frame * 67) % 2001 - 1000) * 0.007f);
		module.inputs[Proc::FALL_CV_INPUT].setVoltage(float((frame * 71) % 2001 - 1000) * 0.007f);
		module.inputs[Proc::BOTH_CV_INPUT].setVoltage(float((frame * 73) % 2001 - 1000) * 0.007f);
		module.inputs[Proc::SIGNAL_INPUT].setVoltage(float((frame % 257) - 128) * 0.03125f);
		module.inputs[Proc::HALT_INPUT].setVoltage(((frame / 8192) & 1) ? 6.f : 0.f);
		module.process(args);
		for (int outputId = 0; outputId < Proc::OUTPUTS_LEN; ++outputId) {
			hash = hashFloat(hash, module.outputs[outputId].getVoltage());
		}
		hash = hashFloat(hash, module.channel.activeRiseTime);
		hash = hashFloat(hash, module.channel.activeFallTime);
		hash = hashFloat(hash, module.channel.phasePos);
		hash = hashFloat(hash, module.channel.out);
		hash = hashFloat(hash, module.channel.signalOutputGain);
	}
	return hash;
}

TestResult modulatedTraceSatisfiesContract() {
	TraceHealth health;
	activeTraceHealth = &health;
	const uint64_t div1Diagnostic = renderModulatedTrace(1, true);
	const uint64_t div8Diagnostic = renderModulatedTrace(8, true);
	activeTraceHealth = nullptr;
	const bool pass = health.hasMeaningfulActivity() && div1Diagnostic != div8Diagnostic;
	return {"Proc modulated stress trace remains finite, bounded, and active", pass,
		pass ? "" : "trace lost activity, exceeded bounds, or timing modes became indistinguishable"};
}

uint64_t renderBandlimitedSampleRateTrace(float sampleRate) {
	Proc module;
	module.bandlimitedSignalOutputs.store(true, std::memory_order_relaxed);
	module.bandlimitedGateOutputs.store(true, std::memory_order_relaxed);
	module.channel.cycleLatched = true;
	for (int inputId = 0; inputId < Proc::INPUTS_LEN; ++inputId) {
		connectInput(module, inputId);
	}
	for (int outputId = 0; outputId < Proc::OUTPUTS_LEN; ++outputId) {
		connectOutput(module, outputId);
	}
	Module::ProcessArgs args = processArgs(sampleRate);
	uint64_t hash = 1469598103934665603ull;
	for (int frame = 0; frame < 32768; ++frame) {
		args.frame = frame;
		module.params[Proc::RISE_PARAM].setValue(0.03f + float((frame / 2048) % 5) * 0.01f);
		module.params[Proc::FALL_PARAM].setValue(0.06f + float((frame / 1536) % 5) * 0.01f);
		module.params[Proc::SHAPE_PARAM].setValue(float((frame / 1024) % 9) * 0.125f);
		module.params[Proc::AMP_PARAM].setValue(2.f + float((frame / 256) % 9));
		module.inputs[Proc::SIGNAL_INPUT].setVoltage(float((frame % 257) - 128) * 0.046875f);
		module.inputs[Proc::BOTH_CV_INPUT].setVoltage(float((frame * 17) % 401 - 200) * 0.0125f);
		module.inputs[Proc::HALT_INPUT].setVoltage(((frame / 4096) & 3) == 1 ? 10.f : 0.f);
		module.inputs[Proc::TRIGGER_INPUT].setVoltage((frame % 5003) == 0 ? 10.f : 0.f);
		module.process(args);
		for (int outputId = 0; outputId < Proc::OUTPUTS_LEN; ++outputId) {
			hash = hashFloat(hash, module.outputs[outputId].getVoltage());
		}
		hash = hashFloat(hash, module.channel.out);
		hash = hashFloat(hash, module.channel.phasePos);
		hash = hashFloat(hash, module.channel.signalOutputGain);
	}
	return hash;
}

TestResult bandlimitedMultiRateTraceSatisfiesContract() {
	TraceHealth health;
	activeTraceHealth = &health;
	const uint64_t diagnostic44100 = renderBandlimitedSampleRateTrace(44100.f);
	const uint64_t diagnostic96000 = renderBandlimitedSampleRateTrace(96000.f);
	const uint64_t diagnostic192000 = renderBandlimitedSampleRateTrace(192000.f);
	activeTraceHealth = nullptr;
	const bool ratesDiffer = diagnostic44100 != diagnostic96000 && diagnostic96000 != diagnostic192000;
	const bool pass = health.hasMeaningfulActivity() && ratesDiffer;
	return {"Proc bandlimited stress traces remain finite and active across sample rates", pass,
		pass ? "" : "trace lost activity, exceeded bounds, or sample-rate scenarios collapsed"};
}

} // namespace

int main() {
	const std::vector<TestResult> results {
		releasedSchemaIsStable(),
		persistedSettingsRoundTrip(),
		legacyCycleStateStillLoads(),
		previewSnapshotsStayCoherent(),
		slewModeTracksSignalWhileIdle(),
		triggerAndRetriggerContractIsStable(),
		haltFreezesAndResumesActiveCycle(),
		modulatedTraceSatisfiesContract(),
		bandlimitedMultiRateTraceSatisfiesContract(),
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
