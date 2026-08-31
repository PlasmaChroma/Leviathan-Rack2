#include "../src/PhonexEngine.hpp"
#include "../src/PhonexFixtures.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>

namespace {

struct Tests {
	int checks = 0;
	int failures = 0;
	void expect(bool value, const std::string& name) {
		++checks;
		if (!value) {
			++failures;
			std::cerr << "[FAIL] " << name << '\n';
		}
	}
};

double referenceLattice(
	double excitation,
	const std::array<float, phonex::kLpcOrder>& coefficients,
	std::array<double, phonex::kLpcOrder>& state) {
	std::array<double, phonex::kLpcOrder + 1> u{};
	u[phonex::kLpcOrder] = excitation;
	for (int i = phonex::kLpcOrder - 1; i >= 0; --i) {
		const double k = std::fmax(-0.995, std::fmin(0.995, coefficients[i]));
		u[i] = u[i + 1] - k * state[i];
	}
	for (int i = phonex::kLpcOrder - 1; i >= 1; --i) {
		const double k = std::fmax(-0.995, std::fmin(0.995, coefficients[i - 1]));
		state[i] = state[i - 1] + k * u[i - 1];
	}
	state[0] = u[0];
	return u[0];
}

void typesAndFixtures(Tests& tests) {
	phonex::LpcSequence empty;
	tests.expect(empty.valid() && empty.frame(0) == nullptr, "empty sequence is safe");
	empty.frameCount = static_cast<std::uint16_t>(phonex::kMaxFrames + 1);
	tests.expect(!empty.valid() && empty.frame(0) == nullptr, "malformed count is rejected");
	const auto silence = phonex::makeSilenceFixture();
	const auto voiced = phonex::makeVoicedFixture();
	const auto noise = phonex::makeUnvoicedFixture();
	tests.expect(silence.frameCount == 4 && silence.frames[0].energy == 0.f,
		"silence fixture");
	tests.expect(voiced.frameCount == 8 && voiced.frames[0].excitation == phonex::Excitation::Voiced,
		"voiced fixture");
	tests.expect(noise.frameCount == 8 && noise.frames[0].excitation == phonex::Excitation::Unvoiced,
		"unvoiced fixture");
}

void chirpContract(Tests& tests) {
	phonex::ChirpGenerator chirp;
	for (std::size_t i = 0; i < phonex::kChirp.size(); ++i)
		tests.expect(chirp.next(20) == phonex::kChirp[i], "chirp sample " + std::to_string(i));
	for (int i = 16; i < 20; ++i)
		tests.expect(chirp.next(20) == 0.f, "chirp tail silence");
	tests.expect(chirp.next(20) == phonex::kChirp[0], "chirp period restart");
	chirp.reset();
	for (int i = 0; i < 8; ++i)
		tests.expect(chirp.next(8) == phonex::kChirp[i], "short period truncation");
	tests.expect(chirp.next(8) == phonex::kChirp[0], "short period restart");
	tests.expect(chirp.next(0) == 0.f, "zero period is safe");
}

void lfsrContract(Tests& tests) {
	constexpr const char* expected = "+----+++--++-+----+-----+-++-+-+-++-++-+-+++----++++++--+----+--";
	phonex::NoiseGenerator noise;
	for (int i = 0; i < 64; ++i)
		tests.expect(noise.next() == (expected[i] == '+' ? 1.f : -1.f),
			"LFSR sign " + std::to_string(i));
	noise.reset();
	const auto initial = noise.state();
	for (std::uint32_t i = 0; i < 131071u; ++i)
		noise.next();
	tests.expect(noise.state() == initial, "LFSR maximal period returns to seed");
	noise.next();
	tests.expect(noise.state() != initial, "LFSR does not repeat early after full period");
	noise.reset();
	tests.expect(noise.next() == 1.f, "LFSR retrigger reset");
}

void latticeContract(Tests& tests) {
	const std::array<float, phonex::kLpcOrder> k {{
		1.4f, -1.2f, 0.41f, -0.3f, 0.22f, -0.16f, 0.11f, -0.08f, 0.05f, -0.03f}};
	phonex::LatticeFilter filter;
	std::array<double, phonex::kLpcOrder> referenceState{};
	for (int sample = 0; sample < 128; ++sample) {
		const float input = sample == 0 ? 1.f : 0.f;
		const double expected = referenceLattice(input, k, referenceState);
		const float actual = filter.process(input, k);
		tests.expect(std::isfinite(actual) && std::abs(actual - expected) < 2e-5,
			"lattice impulse sample " + std::to_string(sample));
	}
	filter.reset();
	tests.expect(filter.process(0.f, k) == 0.f, "lattice reset");
}

phonex::LpcSequence transportSequence() {
	phonex::LpcSequence sequence;
	sequence.frameCount = 4;
	for (int i = 0; i < 4; ++i) {
		sequence.frames[i].energy = 0.2f * static_cast<float>(i + 1);
		sequence.frames[i].pitchPeriod10k = 50.f + 10.f * i;
		sequence.frames[i].reflection[0] = 0.1f * i;
		sequence.frames[i].excitation = i < 3 ? phonex::Excitation::Voiced : phonex::Excitation::Unvoiced;
	}
	return sequence;
}

void transportContract(Tests& tests) {
	const auto sequence = transportSequence();
	phonex::Engine engine;
	engine.setSequence(&sequence);
	phonex::EngineControls controls;
	controls.hostSampleRate = 1000.f;
	for (int i = 0; i < 20; ++i) engine.process(controls);
	tests.expect(std::abs(engine.position() - 1.f) < 1e-5f, "20 ms source frame cadence");
	tests.expect(engine.frameIndex() == 1, "forward transport frame");

	controls.speed = 0.f;
	for (int i = 0; i < 100; ++i) engine.process(controls);
	tests.expect(std::abs(engine.position() - 1.f) < 1e-5f, "transport freeze");
	controls.speed = -1.f;
	engine.retrigger(controls.speed);
	for (int i = 0; i < 20; ++i) engine.process(controls);
	tests.expect(std::abs(engine.position() - 2.f) < 1e-5f, "reverse transport");

	controls.scrubConnected = true;
	controls.scrubVoltage = 0.f;
	auto output = engine.process(controls);
	tests.expect(std::abs(output.position - 1.5f) < 1e-5f, "voltage scrub midpoint");
	tests.expect(output.framePulse, "large scrub jump emits frame pulse");
	const auto interpolated = engine.currentFrame();
	tests.expect(std::abs(interpolated.energy - 0.5f) < 1e-5f, "matching excitation interpolates energy");
	controls.scrubVoltage = 5.f;
	engine.process(controls);
	tests.expect(std::abs(engine.currentFrame().energy - 0.8f) < 1e-5f,
		"interpolation endpoint reaches last frame");
	controls.scrubVoltage = 3.333333f;
	engine.process(controls);
	tests.expect(std::abs(engine.currentFrame().energy - 0.6f) < 1e-5f,
		"excitation change suppresses interpolation");

	engine.setTriggerMode(phonex::TriggerMode::AdvanceOneFrame);
	controls.scrubConnected = false;
	controls.speed = 2.f;
	engine.retrigger(controls.speed);
	controls.triggerGate = true;
	output = engine.process(controls);
	tests.expect(output.frameIndex == 1 && output.framePulse, "frame-step rising edge advances once");
	for (int i = 0; i < 20; ++i) output = engine.process(controls);
	tests.expect(output.frameIndex == 1, "held trigger does not repeat frame step");
	controls.triggerGate = false;
	engine.process(controls);
	controls.triggerGate = true;
	output = engine.process(controls);
	tests.expect(output.frameIndex == 2, "second frame-step edge");

	controls.triggerGate = false;
	engine.process(controls);
	controls.triggerGate = true;
	output = engine.process(controls);
	tests.expect(output.frameIndex == 3 && output.eoxPulse, "forward EOX on last frame");
	controls.triggerGate = false;
	output = engine.process(controls);
	tests.expect(!output.eoxPulse, "EOX pulse lasts exactly 1 ms at 1 kHz host rate");

	controls.speed = -1.f;
	engine.retrigger(controls.speed);
	auto stepEdge = [&]() {
		controls.triggerGate = false;
		engine.process(controls);
		controls.triggerGate = true;
		return engine.process(controls);
	};
	stepEdge();
	stepEdge();
	output = stepEdge();
	tests.expect(output.frameIndex == 0 && output.eoxPulse, "reverse EOX on first frame");
	controls.speed = 1.f;
	stepEdge();
	controls.speed = -1.f;
	output = stepEdge();
	tests.expect(output.eoxPulse, "EOX rearms after moving away from boundary");

	phonex::Engine pulseEngine;
	pulseEngine.setSequence(&sequence);
	controls = {};
	controls.hostSampleRate = 48000.f;
	int highSamples = 0;
	for (int i = 0; i < 960 + 60; ++i) {
		output = pulseEngine.process(controls);
		if (output.framePulse) ++highSamples;
	}
	tests.expect(highSamples == 48, "FRAME_CLK pulse is 1 ms at 48 kHz (observed "
		+ std::to_string(highSamples) + ")");
}

void pitchContract(Tests& tests) {
	phonex::LpcSequence sequence;
	sequence.frameCount = 2;
	for (auto& frame : sequence.frames) {
		frame.energy = 1.f;
		frame.pitchPeriod10k = 80.f;
		frame.excitation = phonex::Excitation::Voiced;
	}
	auto trace = [&](float pitch, float voct, float atten, float speed, float rate) {
		phonex::Engine engine;
		engine.setSequence(&sequence);
		engine.setInternalRate(rate);
		phonex::EngineControls controls;
		controls.hostSampleRate = rate;
		controls.pitchOctaves = pitch;
		controls.voct = voct;
		controls.voctAttenuverter = atten;
		controls.speed = speed;
		std::array<float, 90> samples{};
		for (auto& sample : samples) sample = engine.process(controls).audio;
		return samples;
	};
	const auto base = trace(0.f, 0.f, 1.f, 0.5f, 10000.f);
	const auto fastTransport = trace(0.f, 0.f, 1.f, 2.f, 10000.f);
	tests.expect(base == fastTransport, "transport speed does not transpose excitation");
	const auto octave = trace(1.f, 0.f, 1.f, 0.f, 10000.f);
	tests.expect(std::abs(base[81] - base[1]) < 1e-6f, "10 kHz base pitch period");
	tests.expect(std::abs(octave[41] - octave[1]) < 1e-6f, "PITCH octave halves period");
	const auto voct = trace(0.f, 1.f, 1.f, 0.f, 10000.f);
	tests.expect(voct == octave, "V/oct with unity attenuation transposes one octave");
	const auto blockedVoct = trace(0.f, 1.f, 0.f, 0.f, 10000.f);
	tests.expect(blockedVoct == trace(0.f, 0.f, 1.f, 0.f, 10000.f), "V/oct attenuverter zero blocks CV");
	const auto underclock = trace(0.f, 0.f, 1.f, 0.f, 8000.f);
	tests.expect(std::abs(underclock[81] - underclock[1]) < 1e-6f,
		"8 kHz retains period ticks and lowers physical pitch");
}

void schedulerAndSynthesisContract(Tests& tests) {
	const auto voiced = phonex::makeVoicedFixture(32);
	phonex::Engine engine;
	engine.setSequence(&voiced);
	phonex::EngineControls controls;
	controls.hostSampleRate = 48000.f;
	float peak = 0.f;
	for (int i = 0; i < 48000; ++i) {
		const auto output = engine.process(controls);
		peak = std::max(peak, std::abs(output.audio));
		tests.expect(std::isfinite(output.audio) && std::abs(output.audio) <= 5.f,
			"bounded clean output " + std::to_string(i));
	}
	tests.expect(engine.internalTickCount() == 10000, "10 kHz scheduler long-run accuracy");
	tests.expect(peak > 0.1f, "procedural voiced fixture is audible");
	phonex::Engine replay;
	replay.setSequence(&voiced);
	phonex::EngineControls replayControls;
	replayControls.hostSampleRate = 10000.f;
	std::array<float, 32> firstTrace{};
	std::array<float, 32> resetTrace{};
	for (auto& value : firstTrace) value = replay.process(replayControls).audio;
	for (int i = 0; i < 73; ++i) replay.process(replayControls);
	replay.retrigger(1.f);
	for (auto& value : resetTrace) value = replay.process(replayControls).audio;
	tests.expect(firstTrace == resetTrace, "retrigger resets clean synthesis state");

	engine.setInternalRate(8000.f);
	engine.retrigger(1.f);
	const auto before = engine.internalTickCount();
	for (int i = 0; i < 48000; ++i) engine.process(controls);
	tests.expect(engine.internalTickCount() - before == 8000, "8 kHz scheduler long-run accuracy");

	phonex::LpcSequence carrier;
	carrier.frameCount = 1;
	carrier.frames[0].energy = 1.f;
	carrier.frames[0].excitation = phonex::Excitation::Silence;
	engine.setSequence(&carrier);
	engine.setInternalRate(10000.f);
	controls.hostSampleRate = 10000.f;
	controls.externalConnected = true;
	controls.externalExcitation = 2.5f;
	const auto external = engine.process(controls);
	tests.expect(std::abs(external.audio - (10.f / 7.f)) < 1e-5f,
		"external carrier retains frame envelope and output calibration");

	controls.externalExcitation = std::numeric_limits<float>::quiet_NaN();
	tests.expect(engine.process(controls).audio == 0.f, "nonfinite DSP is contained");
	controls.externalExcitation = 2.5f;
	tests.expect(std::isfinite(engine.process(controls).audio), "DSP resumes after nonfinite input");

	engine.setReconstructionMode(phonex::ReconstructionMode::Filtered);
	const auto filtered = engine.process(controls);
	tests.expect(std::isfinite(filtered.audio), "filtered reconstruction is finite");
}

} // namespace

int main() {
	Tests tests;
	typesAndFixtures(tests);
	chirpContract(tests);
	lfsrContract(tests);
	latticeContract(tests);
	transportContract(tests);
	pitchContract(tests);
	schedulerAndSynthesisContract(tests);
	std::cout << "[TEST SUMMARY] checks=" << tests.checks
		<< " failures=" << tests.failures << '\n';
	return tests.failures == 0 ? 0 : 1;
}
