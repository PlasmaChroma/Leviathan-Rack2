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
	const bool idleIsZero = std::fabs(module.outputs[Wyrm::RAW_OUTPUT].getVoltage()) < 1e-6f
		&& !module.displayEnvelopeRunning.load(std::memory_order_relaxed);

	module.inputs[Wyrm::VOCT_INPUT].setVoltage(10.f);
	process(module);
	const bool displayBecameActive = module.displayEnvelopeRunning.load(std::memory_order_relaxed);
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
		&& !module.displayEnvelopeRunning.load(std::memory_order_relaxed)
		&& std::fabs(module.outputs[Wyrm::RAW_OUTPUT].getVoltage()) < 1e-6f
		&& std::fabs(module.outputs[Wyrm::OUT_OUTPUT].getVoltage()) < 1e-6f;
	const float durationMs = module.displayEnvelopeTimeMs.load(std::memory_order_relaxed);
	return idleIsZero && displayBecameActive && traversedUnipolarRange && completed
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

bool interpolationStaysInsideAuthoredSegments() {
	std::array<float, kWyrmPointCountMax> points {};
	points[0] = -1.f;
	points[1] = 0.25f;
	points[2] = 0.30f;
	points[3] = 0.40f;
	for (int sample = 0; sample <= 64; ++sample) {
		const float phase = (1.f + float(sample) / 64.f) / 4.f;
		const float value = catmullPeriodic(points, 4, phase);
		if (value < points[1] - 1e-6f || value > points[2] + 1e-6f) {
			return false;
		}
	}
	return true;
}

bool octaveModeQuantizesOscillatorAndEnvelopeRates() {
	Wyrm module;
	module.params[Wyrm::FREQ_PARAM].setValue(0.61f);
	module.params[Wyrm::COARSE_STEP_MODE_PARAM].setValue(1.f);
	process(module);
	const float oscillatorHz = module.displayFrequencyHz.load(std::memory_order_relaxed);
	const float oscillatorOctave = std::log2(oscillatorHz / dsp::FREQ_C4);
	const bool oscillatorIsStepped = std::fabs(oscillatorOctave - std::round(oscillatorOctave)) < 1e-4f;

	module.params[Wyrm::ENV_MODE_PARAM].setValue(1.f);
	process(module);
	const float envelopeMs = module.displayEnvelopeTimeMs.load(std::memory_order_relaxed);
	const float envelopeRate = 1000.f / envelopeMs;
	const float envelopeOctave = std::log2(envelopeRate / dsp::FREQ_C4);
	const bool envelopeIsStepped = std::fabs(envelopeOctave - std::round(envelopeOctave)) < 1e-4f;
	return oscillatorIsStepped && envelopeIsStepped;
}

bool enteringEnvelopeModeLoadsFastAttackSlowReleaseShape() {
	Wyrm module;
	module.setFactoryShape(SHAPE_SQUARE);
	module.params[Wyrm::ENV_MODE_PARAM].setValue(1.f);
	process(module);
	const int last = module.pointCount - 1;
	int peakIndex = 0;
	float peak = -1.f;
	for (int i = 0; i <= last; ++i) {
		const float value = module.getWavePoint(i);
		if (value > peak) {
			peak = value;
			peakIndex = i;
		}
	}
	const float attackFraction = float(peakIndex) / float(last);
	const int attackMidpoint = peakIndex / 2;
	const float attackMidpointUnipolar = 0.5f * (module.getWavePoint(attackMidpoint) + 1.f);
	return std::fabs(module.getWavePoint(0) + 1.f) < 1e-6f
		&& std::fabs(module.getWavePoint(last) + 1.f) < 1e-6f
		&& peak > 0.999f
		&& peakIndex > 0
		&& attackFraction > 0.14f
		&& attackFraction < 0.16f
		&& attackMidpointUnipolar > 0.55f
		&& (last - peakIndex) > 4 * peakIndex;
}

} // namespace

int main() {
	const bool oneShot = oneShotTraversesAndReturnsToZero();
	const bool retrigger = fallingThenRisingEdgeRetriggers();
	const bool polyphony = polyphonicTriggersRemainIndependent();
	const bool boundedInterpolation = interpolationStaysInsideAuthoredSegments();
	const bool octaveStepping = octaveModeQuantizesOscillatorAndEnvelopeRates();
	const bool arShape = enteringEnvelopeModeLoadsFastAttackSlowReleaseShape();
	std::cout << (oneShot ? "[PASS] " : "[FAIL] ") << "ENV traverses 0-10 V once and returns to 0 V\n";
	std::cout << (retrigger ? "[PASS] " : "[FAIL] ") << "ENV retriggers on a new rising edge\n";
	std::cout << (polyphony ? "[PASS] " : "[FAIL] ") << "ENV trigger voices remain polyphonically independent\n";
	std::cout << (boundedInterpolation ? "[PASS] " : "[FAIL] ") << "wave interpolation stays within authored segment endpoints\n";
	std::cout << (octaveStepping ? "[PASS] " : "[FAIL] ") << "OCT mode quantizes oscillator and envelope rates\n";
	std::cout << (arShape ? "[PASS] " : "[FAIL] ") << "entering ENV loads a fast-attack slow-release shape\n";
	const int passed = int(oneShot) + int(retrigger) + int(polyphony) + int(boundedInterpolation) + int(octaveStepping) + int(arShape);
	std::cout << "[SUMMARY] wyrm_envelope_spec: " << passed << "/6 passed\n";
	return passed == 6 ? 0 : 1;
}
