#include "../src/Wyrm.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

bool isDragonKingDebugEnabled() {
	return false;
}

bool isModuleTeardownLoggingEnabled() {
	return false;
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* name) : moduleName(name) {
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

void ModuleTeardownTimer::begin(int) {
}

namespace {

constexpr float kSampleRate = 48000.f;

void process(Wyrm& module, int samples = 1) {
	Module::ProcessArgs args;
	args.sampleRate = kSampleRate;
	args.sampleTime = 1.f / args.sampleRate;
	for (int i = 0; i < samples; ++i) {
		args.frame = i;
		module.process(args);
	}
}

void configureFastEnvelope(Wyrm& module, int channels = 1) {
	module.params[Wyrm::ENV_MODE_PARAM].setValue(1.f);
	module.params[Wyrm::FREQ_PARAM].setValue(1.f);
	module.inputs[Wyrm::VOCT_INPUT].channels = channels;
	for (int c = 0; c < channels; ++c) {
		module.inputs[Wyrm::VOCT_INPUT].setVoltage(0.f, c);
	}
	process(module);
}

bool oneShotTraversesAndReturnsToZero() {
	Wyrm module;
	configureFastEnvelope(module);
	const bool idleIsZero = std::fabs(module.outputs[Wyrm::RAW_OUTPUT].getVoltage()) < 1e-6f;

	module.inputs[Wyrm::VOCT_INPUT].setVoltage(10.f);
	process(module);
	float minimum = 10.f;
	float maximum = 0.f;
	for (int i = 0; i < 240; ++i) {
		process(module);
		const float voltage = module.outputs[Wyrm::RAW_OUTPUT].getVoltage();
		minimum = std::min(minimum, voltage);
		maximum = std::max(maximum, voltage);
	}
	const bool traversedUnipolarRange = minimum >= -1e-5f && maximum > 9.5f
		&& maximum <= 10.00001f && module.envelopeRunning[0];
	process(module, 300);
	const bool completed = !module.envelopeRunning[0]
		&& std::fabs(module.outputs[Wyrm::RAW_OUTPUT].getVoltage()) < 1e-6f
		&& std::fabs(module.outputs[Wyrm::OUT_OUTPUT].getVoltage()) < 1e-6f;
	const float durationMs = module.displayEnvelopeTimeMs.load(std::memory_order_relaxed);
	return idleIsZero && traversedUnipolarRange && completed
		&& durationMs > 9.9f && durationMs < 10.1f;
}

bool fallingThenRisingEdgeRetriggers() {
	Wyrm module;
	configureFastEnvelope(module);
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(10.f);
	process(module, 120);
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(0.f);
	process(module);
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(5.f);
	process(module);
	return module.envelopeRunning[0] && module.phase[0] > 0.f && module.phase[0] < 0.01f;
}

bool polyphonicTriggersRemainIndependent() {
	Wyrm module;
	configureFastEnvelope(module, 2);
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(10.f, 0);
	process(module, 8);
	const bool firstOnly = module.envelopeRunning[0] && !module.envelopeRunning[1]
		&& std::fabs(module.outputs[Wyrm::RAW_OUTPUT].getVoltage(1)) < 1e-6f;
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(0.f, 0);
	module.inputs[Wyrm::VOCT_INPUT].setVoltage(10.f, 1);
	process(module);
	return firstOnly && module.envelopeRunning[0] && module.envelopeRunning[1];
}

} // namespace

int main() {
	const bool oneShot = oneShotTraversesAndReturnsToZero();
	const bool retrigger = fallingThenRisingEdgeRetriggers();
	const bool polyphony = polyphonicTriggersRemainIndependent();
	std::cout << (oneShot ? "[PASS] " : "[FAIL] ") << "ENV traverses 0-10 V once and returns to 0 V\n";
	std::cout << (retrigger ? "[PASS] " : "[FAIL] ") << "ENV retriggers on a new rising edge\n";
	std::cout << (polyphony ? "[PASS] " : "[FAIL] ") << "ENV trigger voices remain polyphonically independent\n";
	const int passed = int(oneShot) + int(retrigger) + int(polyphony);
	std::cout << "[SUMMARY] wyrm_envelope_spec: " << passed << "/3 passed\n";
	return passed == 3 ? 0 : 1;
}
