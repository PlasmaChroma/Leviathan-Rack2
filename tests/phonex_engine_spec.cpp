#include "../src/PhonexEngine.hpp"
#include "../src/PhonexFixtures.hpp"
#include "../src/PhonexRom.hpp"
#include "../src/PhonexPronunciation.hpp"
#include "../src/PhonexSequenceCompiler.hpp"
#include "../src/PhonexSequenceMailbox.hpp"

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
	const float longPeriod = static_cast<float>(phonex::kChirp.size() + 4);
	for (std::size_t i = 0; i < phonex::kChirp.size(); ++i)
		tests.expect(chirp.next(longPeriod) == phonex::kChirp[i],
			"chirp sample " + std::to_string(i));
	for (int i = 0; i < 4; ++i)
		tests.expect(chirp.next(longPeriod) == 0.f, "chirp tail silence");
	tests.expect(chirp.next(longPeriod) == phonex::kChirp[0], "chirp period restart");
	chirp.reset();
	for (int i = 0; i < 8; ++i)
		tests.expect(chirp.next(8) == phonex::kChirp[i], "short period truncation");
	tests.expect(chirp.next(8) == phonex::kChirp[0], "short period restart");
	tests.expect(chirp.next(0) == 0.f, "zero period is safe");
	tests.expect(phonex::kChirp[1] == 42.f / 128.f
		&& phonex::kChirp[2] == -44.f / 128.f
		&& phonex::kChirp[40] == 1.f / 128.f,
		"TMS5100 chirp sentinels");
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
	const float midpointEnergy = 0.4f + 0.2f * phonex::tms5100InterpolationMix(0.5f);
	tests.expect(std::abs(interpolated.energy - midpointEnergy) < 1e-5f,
		"matching excitation uses TMS5100 staircase interpolation");
	tests.expect(phonex::tms5100InterpolationMix(0.124f) == 0.f
		&& phonex::tms5100InterpolationMix(0.125f) == 0.125f
		&& std::abs(phonex::tms5100InterpolationMix(0.875f) - 0.905792236f) < 1e-7f,
		"TMS5100 interpolation period boundaries");
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

	phonex::Engine completionEngine;
	completionEngine.setSequence(&sequence);
	controls = {};
	controls.hostSampleRate = 1000.f;
	bool audibleBeforeEox = false;
	int samplesThroughEox = 0;
	for (int i = 0; i < 80; ++i) {
		output = completionEngine.process(controls);
		++samplesThroughEox;
		if (!output.eoxPulse)
			audibleBeforeEox = audibleBeforeEox || std::abs(output.audio) > 1e-5f;
		if (output.eoxPulse)
			break;
	}
	tests.expect(audibleBeforeEox && output.eoxPulse
		&& samplesThroughEox == 80,
		"free-running utterance renders all four 20 ms frames before EOX");
	bool silentAfterEox = true;
	for (int i = 0; i < 100; ++i)
		silentAfterEox = silentAfterEox && completionEngine.process(controls).audio == 0.f;
	tests.expect(silentAfterEox, "completed free-running utterance remains silent");

	phonex::LpcSequence deClickSequence;
	deClickSequence.frameCount = 1;
	deClickSequence.frames[0].energy = 1.f;
	deClickSequence.frames[0].pitchPeriod10k = 80.f;
	deClickSequence.frames[0].excitation = phonex::Excitation::Voiced;
	phonex::Engine deClickEngine;
	deClickEngine.setSequence(&deClickSequence);
	deClickEngine.setReconstructionMode(phonex::ReconstructionMode::Filtered);
	deClickEngine.setOutputStage(phonex::OutputStage::CalibratedLinear);
	phonex::EngineControls deClickControls;
	deClickControls.hostSampleRate = 48000.f;
	deClickControls.externalConnected = true;
	deClickControls.externalExcitation = 5.f;
	float beforeEox = 0.f;
	phonex::EngineOutput deClickOutput;
	for (int sample = 0; sample < 2000 && !deClickOutput.eoxPulse; ++sample) {
		beforeEox = deClickOutput.audio;
		deClickOutput = deClickEngine.process(deClickControls);
	}
	tests.expect(deClickOutput.eoxPulse && beforeEox > 0.5f
		&& std::abs(deClickOutput.audio - beforeEox) < 0.1f,
		"filtered EOX discharges reconstruction state without a hard-zero click");
	for (int sample = 0; sample < 480; ++sample)
		deClickOutput = deClickEngine.process(deClickControls);
	tests.expect(deClickOutput.audio == 0.f,
		"filtered EOX discharge settles to exact silence within 10 ms");
	controls.scrubConnected = true;
	controls.scrubVoltage = 5.f;
	bool audibleScrubEndpoint = false;
	for (int i = 0; i < 20; ++i)
		audibleScrubEndpoint = audibleScrubEndpoint
			|| std::abs(completionEngine.process(controls).audio) > 1e-5f;
	tests.expect(audibleScrubEndpoint,
		"voltage scrub remains an intentionally audible terminal-frame hold");

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
	auto trace = [&](float pitch, float voct, float speed, float rate) {
		phonex::Engine engine;
		engine.setSequence(&sequence);
		engine.setInternalRate(rate);
		phonex::EngineControls controls;
		controls.hostSampleRate = rate;
		controls.pitchOctaves = pitch;
		controls.voct = voct;
		controls.speed = speed;
		std::array<float, 90> samples{};
		for (auto& sample : samples) sample = engine.process(controls).audio;
		return samples;
	};
	const auto base = trace(0.f, 0.f, 0.5f, 10000.f);
	const auto fastTransport = trace(0.f, 0.f, 2.f, 10000.f);
	tests.expect(base == fastTransport, "transport speed does not transpose excitation");
	const auto octave = trace(1.f, 0.f, 0.f, 10000.f);
	tests.expect(std::abs(base[81] - base[1]) < 1e-6f, "10 kHz base pitch period");
	tests.expect(std::abs(octave[41] - octave[1]) < 1e-6f, "PITCH octave halves period");
	const auto voct = trace(0.f, 1.f, 0.f, 10000.f);
	tests.expect(voct == octave, "V/oct tracks one octave at fixed unity gain");
	const auto underclock = trace(0.f, 0.f, 0.f, 8000.f);
	tests.expect(std::abs(underclock[81] - underclock[1]) < 1e-6f,
		"8 kHz retains period ticks and lowers physical pitch");
}

void schedulerAndSynthesisContract(Tests& tests) {
	tests.expect(phonex::applyOutputStage(1.f, phonex::OutputStage::LegacyCurve)
		== 1.f / 0.45f, "legacy output curve remains selectable for comparison");
	tests.expect(phonex::applyOutputStage(1.f, phonex::OutputStage::CalibratedLinear) == 1.1f,
		"calibrated linear output is exact below the safety region");
	tests.expect(phonex::applyOutputStage(1.f, phonex::OutputStage::CalibratedLimited) == 1.1f
		&& phonex::applyOutputStage(4.f, phonex::OutputStage::CalibratedLimited) < 5.f,
		"calibrated limiter is transparent below 4.5 V and bounded above it");
	const auto voiced = phonex::makeVoicedFixture(32);
	phonex::Engine engine;
	engine.setSequence(&voiced);
	phonex::EngineControls controls;
	controls.hostSampleRate = 48000.f;
	controls.speed = 0.f;
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
	replayControls.speed = 0.f;
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
	engine.setReconstructionMode(phonex::ReconstructionMode::RawHold);
	const auto external = engine.process(controls);
	tests.expect(std::abs(external.audio - 0.55f) < 1e-5f,
		"external carrier retains frame envelope and neutral output calibration");

	controls.externalExcitation = std::numeric_limits<float>::quiet_NaN();
	tests.expect(engine.process(controls).audio == 0.f, "nonfinite DSP is contained");
	controls.externalExcitation = 2.5f;
	tests.expect(std::isfinite(engine.process(controls).audio), "DSP resumes after nonfinite input");

	engine.setReconstructionMode(phonex::ReconstructionMode::Filtered);
	const auto filtered = engine.process(controls);
	tests.expect(std::isfinite(filtered.audio), "filtered reconstruction is finite");

	const phonex::LpcSequence forcedSequence = phonex::makeUnvoicedFixture(16);
	phonex::Engine forcedVoiced;
	forcedVoiced.setSequence(&forcedSequence);
	phonex::EngineControls forcedControls;
	forcedControls.hostSampleRate = 10000.f;
	forcedControls.speed = 0.f;
	forcedControls.exciteBlend = 1.f;
	forcedControls.forcedExcitation = phonex::ForcedExcitation::Voiced;
	float forcedPeak = 0.f;
	for (int sample = 0; sample < 256; ++sample)
		forcedPeak = std::max(forcedPeak, std::abs(forcedVoiced.process(forcedControls).audio));
	tests.expect(forcedPeak > 0.05f,
		"forced voiced excitation has a deterministic startup pitch on unvoiced frames");

	phonex::Engine rawNyquist;
	phonex::Engine filteredNyquist;
	rawNyquist.setSequence(&carrier);
	filteredNyquist.setSequence(&carrier);
	filteredNyquist.setReconstructionMode(phonex::ReconstructionMode::Filtered);
	phonex::EngineControls nyquistControls;
	nyquistControls.hostSampleRate = 10000.f;
	nyquistControls.speed = 0.f;
	nyquistControls.externalConnected = true;
	float rawTail = 0.f;
	float filteredTail = 0.f;
	for (int sample = 0; sample < 512; ++sample) {
		nyquistControls.externalExcitation = (sample & 1) ? 2.5f : -2.5f;
		const float raw = rawNyquist.process(nyquistControls).audio;
		const float lowPassed = filteredNyquist.process(nyquistControls).audio;
		if (sample >= 384) {
			rawTail += std::abs(raw);
			filteredTail += std::abs(lowPassed);
		}
	}
	tests.expect(rawTail > 50.f && filteredTail < rawTail * 0.05f,
		"two-pole reconstruction rejects internal-rate Nyquist images");

}

phonex::LpcSequence glitchSequence() {
	phonex::LpcSequence sequence;
	sequence.frameCount = 8;
	for (std::uint16_t frameIndex = 0; frameIndex < sequence.frameCount; ++frameIndex) {
		auto& frame = sequence.frames[frameIndex];
		frame.energy = 0.07f + 0.1f * frameIndex;
		frame.pitchPeriod10k = 51.f + 7.f * frameIndex;
		frame.excitation = frameIndex == 7 ? phonex::Excitation::Silence
			: (frameIndex & 1u) ? phonex::Excitation::Unvoiced : phonex::Excitation::Voiced;
		for (int k = 0; k < phonex::kLpcOrder; ++k)
			frame.reflection[k] = -0.7f + 0.11f * k + 0.013f * frameIndex;
	}
	return sequence;
}

void phase3Contract(Tests& tests) {
	constexpr std::uint32_t seed = 0x1234abcdu;
	const auto sequence = glitchSequence();
	const auto formantZero = phonex::formantShiftReflection(sequence.frames[2].reflection, 0.f);
	const auto formantLow = phonex::formantShiftReflection(sequence.frames[2].reflection, -1.f);
	const auto formantHigh = phonex::formantShiftReflection(sequence.frames[2].reflection, 1.f);
	const auto warpZero = phonex::warpReflectionCoefficients(sequence.frames[2].reflection, 0.f);
	const auto warpHigh = phonex::warpReflectionCoefficients(sequence.frames[2].reflection, 1.f);
	bool formantDistinct = false;
	bool formantStable = true;
	for (int i = 0; i < phonex::kLpcOrder; ++i) {
		formantDistinct = formantDistinct
			|| std::abs(formantLow[i] - formantHigh[i]) > 1e-4f;
		formantStable = formantStable && std::isfinite(formantLow[i])
			&& std::isfinite(formantHigh[i]) && std::abs(formantLow[i]) <= 0.985f
			&& std::abs(formantHigh[i]) <= 0.985f;
	}
	tests.expect(formantZero == sequence.frames[2].reflection,
		"FORMANT zero is an exact coefficient identity");
	tests.expect(warpZero == sequence.frames[2].reflection,
		"WARP zero is an exact coefficient identity");
	tests.expect(warpHigh != sequence.frames[2].reflection,
		"nonzero WARP transforms reflection coefficients");
	tests.expect(formantDistinct && formantStable,
		"FORMANT extremes are distinct, finite, and lattice-stable");
	auto selected = [&](int level, int index) {
		return phonex::selectGlitchedFrame(sequence, index, level, seed);
	};
	tests.expect(selected(0, 3).energy == sequence.frames[3].energy,
		"GLITCH 0 CLEAN exact frame");
	tests.expect(selected(1, 3).energy == sequence.frames[2].energy,
		"GLITCH 1 HOLD-4");
	tests.expect(selected(2, 3).energy == sequence.frames[4].energy,
		"GLITCH 2 SKIP-4");
	tests.expect(selected(3, 2).energy == sequence.frames[3].energy,
		"GLITCH 3 ADDR-X1");
	tests.expect(selected(4, 1).energy == sequence.frames[3].energy,
		"GLITCH 4 ADDR-X2");
	const int offset = (phonex::phonexFrameHash(seed, 3, 5) & 1u) ? 2 : -2;
	tests.expect(selected(5, 3).energy == sequence.frames[3 + offset].energy,
		"GLITCH 5 OFFSET-2");
	tests.expect(selected(6, 0).energy == sequence.frames[3].energy
		&& selected(6, 3).energy == sequence.frames[0].energy,
		"GLITCH 6 REV-4");
	tests.expect(std::abs(selected(7, 2).energy
		- std::round(sequence.frames[2].energy * 3.f) / 3.f) < 1e-6f,
		"GLITCH 7 ENERGY-2BIT");
	tests.expect(selected(8, 2).pitchPeriod10k == 64.f,
		"GLITCH 8 PITCH-8");
	const float fold = (phonex::phonexFrameHash(seed, 2, 9) & 1u) ? 2.f : 0.5f;
	tests.expect(selected(9, 2).pitchPeriod10k == sequence.frames[2].pitchPeriod10k * fold,
		"GLITCH 9 PITCH-FOLD");
	tests.expect(selected(10, 2).reflection[0] == -sequence.frames[2].reflection[0]
		&& selected(10, 2).reflection[1] == sequence.frames[2].reflection[1]
		&& selected(10, 2).reflection[9] == -sequence.frames[2].reflection[9],
		"GLITCH 10 K-SIGN");
	tests.expect(selected(11, 2).reflection[2] == sequence.frames[2].reflection[0]
		&& selected(11, 2).reflection[1] == sequence.frames[2].reflection[9],
		"GLITCH 11 K-ROTATE");
	tests.expect(selected(12, 2).reflection[1] == 0.f
		&& selected(12, 2).reflection[8] == sequence.frames[2].reflection[8],
		"GLITCH 12 K-HOLES");
	tests.expect(std::abs(selected(13, 2).reflection[0]
		- std::round(sequence.frames[2].reflection[0] * 8.f) / 8.f) < 1e-6f,
		"GLITCH 13 K-4BIT");
	tests.expect(selected(14, 1).excitation == phonex::Excitation::Voiced
		&& selected(14, 2).excitation == phonex::Excitation::Voiced
		&& selected(14, 7).excitation == phonex::Excitation::Silence,
		"GLITCH 14 VOICE-FLIP");
	const auto bus = selected(15, 1);
	tests.expect(bus.energy == std::round(sequence.frames[2].energy * 3.f) / 3.f
		&& bus.pitchPeriod10k == 64.f,
		"GLITCH 15 BUS-SCRAMBLE combined recipe");
	tests.expect(selected(3, 0).energy == sequence.frames[1].energy
		&& selected(4, 7).energy == sequence.frames[5].energy,
		"glitch remapping stays in range");

	auto render = [&](std::uint32_t renderSeed, float bend, std::uint8_t glitch,
		float warp) {
		phonex::Engine engine;
		engine.setSequence(&sequence);
		engine.setSeed(renderSeed);
		phonex::EngineControls controls;
		controls.hostSampleRate = 10000.f;
		controls.speed = 0.f;
		controls.bend = bend;
		controls.glitchLevel = glitch;
		controls.warp = warp;
		std::array<float, 2048> trace{};
		for (float& sample : trace)
			sample = engine.process(controls).audio;
		return trace;
	};
	const auto cleanA = render(seed, 0.f, 0, 0.f);
	const auto cleanB = render(seed ^ 0xffffffffu, 0.f, 0, 0.f);
	tests.expect(cleanA == cleanB, "BEND 0 and GLITCH 0 ignore seed exactly");
	const auto bendA = render(seed, 0.92f, 0, 0.f);
	const auto bendB = render(seed, 0.92f, 0, 0.f);
	tests.expect(bendA == bendB, "seeded bend is repeatable");
	tests.expect(bendA != render(seed + 1u, 0.92f, 0, 0.f),
		"bend seed changes starvation trace");
	tests.expect(cleanA != render(seed, 0.f, 0, 0.75f),
		"WARP transforms the LPC response");
	tests.expect(cleanA != render(seed, 0.f, 10, 0.f),
		"nonzero GLITCH changes synthesis");

	phonex::LpcSequence extreme = sequence;
	for (std::uint16_t i = 0; i < extreme.frameCount; ++i) {
		extreme.frames[i].energy = i & 1u ? 1000.f : -1000.f;
		for (float& coefficient : extreme.frames[i].reflection)
			coefficient = i & 1u ? 100.f : -100.f;
	}
	phonex::Engine engine;
	engine.setSequence(&extreme);
	engine.setSeed(seed);
	phonex::EngineControls controls;
	controls.hostSampleRate = 48000.f;
	controls.bend = 1.f;
	controls.warp = 1.f;
	controls.glitchLevel = 15;
	for (int i = 0; i < 48000; ++i) {
		const float sample = engine.process(controls).audio;
		tests.expect(std::isfinite(sample) && std::abs(sample) <= 5.f,
			"extreme phase 3 output remains finite and bounded " + std::to_string(i));
	}
}

void phase4CorpusAndCompilerContract(Tests& tests) {
	phonex::LpcFrame quantizationProbe;
	quantizationProbe.energy = 0.72f;
	quantizationProbe.pitchPeriod10k = 82.f;
	quantizationProbe.excitation = phonex::Excitation::Voiced;
	const phonex::LpcFrame quantized = phonex::quantizeTms5100Frame(quantizationProbe);
	tests.expect(std::abs(quantized.energy - 61.f / 86.f) < 1e-7f
		&& quantized.pitchPeriod10k == 83.f,
		"TMS5100 nonlinear energy and pitch reconstruction");
	tests.expect(quantized.reflection[0] == -21.f / 512.f
		&& quantized.reflection[1] == -14.f / 512.f
		&& quantized.reflection[9] == 1.f / 512.f,
		"TMS5100 coefficient tables reconstruct progressively quantized K values");
	quantizationProbe.excitation = phonex::Excitation::Unvoiced;
	quantizationProbe.pitchPeriod10k = 99.f;
	quantizationProbe.reflection.fill(0.5f);
	const phonex::LpcFrame unvoiced = phonex::quantizeTms5100Frame(quantizationProbe);
	bool upperCoefficientsZero = unvoiced.pitchPeriod10k == 0.f;
	for (int coefficient = 4; coefficient < phonex::kLpcOrder; ++coefficient)
		upperCoefficientsZero = upperCoefficientsZero
			&& unvoiced.reflection[coefficient] == 0.f;
	tests.expect(upperCoefficientsZero,
		"TMS5100 unvoiced reconstruction retains only K1 through K4");

	tests.expect(phonex::kPhoneCount == 40, "frozen 40-phone inventory");
	for (std::size_t i = 0; i < phonex::kPhoneCount; ++i) {
		const auto phone = static_cast<phonex::Phone>(i);
		phonex::Phone roundTrip;
		tests.expect(phonex::findPhone(phonex::phoneSymbol(phone), roundTrip)
			&& roundTrip == phone, "phone symbol round trip " + std::to_string(i));
		const auto& prototype = phonex::phonePrototype(phone);
		tests.expect(prototype.durationFrames > 0 && prototype.anchorCount > 0
			&& prototype.anchorCount <= phonex::kMaxPhoneAnchors,
			"phone prototype shape " + std::to_string(i));
		for (std::uint8_t anchor = 0; anchor < prototype.anchorCount; ++anchor) {
			const auto& frame = prototype.anchors[anchor];
			tests.expect(std::isfinite(frame.energy) && frame.energy >= 0.f && frame.energy <= 1.f,
				"phone anchor energy " + std::to_string(i));
			if (frame.excitation == phonex::Excitation::Voiced)
				tests.expect(frame.pitchPeriod10k > 0.f && std::isfinite(frame.pitchPeriod10k),
					"voiced anchor pitch " + std::to_string(i));
			for (float coefficient : frame.reflection)
				tests.expect(std::isfinite(coefficient) && std::abs(coefficient) <= 0.995f,
					"stable generated coefficient " + std::to_string(i));
			for (float formant : {-1.f, 1.f}) {
				const auto shifted = phonex::formantShiftReflection(frame.reflection, formant);
				for (float coefficient : shifted)
					tests.expect(std::isfinite(coefficient) && std::abs(coefficient) <= 0.985f,
						"stable formant-shifted coefficient " + std::to_string(i));
			}
		}
	}
	for (phonex::Phone stop : {phonex::Phone::B, phonex::Phone::D, phonex::Phone::G}) {
		const auto& prototype = phonex::phonePrototype(stop);
		tests.expect(prototype.anchorCount == 3
			&& prototype.anchors[0].excitation == phonex::Excitation::Silence
			&& prototype.anchors[1].excitation == phonex::Excitation::Unvoiced
			&& prototype.anchors[2].excitation == phonex::Excitation::Voiced,
			"voiced stop has closure, burst, and voiced release");
	}
	const auto& affricate = phonex::phonePrototype(phonex::Phone::JH);
	tests.expect(affricate.anchorCount == 3
		&& affricate.anchors[0].excitation == phonex::Excitation::Silence
		&& affricate.anchors[1].excitation == phonex::Excitation::Unvoiced
		&& affricate.anchors[2].excitation == phonex::Excitation::Voiced,
		"JH has closure, frication, and voiced release");
	tests.expect(phonex::phonePrototype(phonex::Phone::P).durationFrames == 4,
		"P closure remains distinct in an S-P consonant cluster");
	tests.expect(phonex::phonePrototype(phonex::Phone::HH).anchors[0].energy >= 0.27f,
		"HH aspiration has a distinct authored energy floor");
	tests.expect(phonex::dictionaryPronunciation("VOLTAGE") == "V OW1 L T IH JH",
		"voltage uses the audible unstressed IH vowel");

	phonex::PhoneScript script;
	tests.expect(phonex::parseDirectPhonemes("[HH EH1 L OW0]", script)
		== phonex::CompileStatus::Ok && script.count == 4
		&& script.tokens[1].stress == 1, "direct parser accepts brackets and stress");
	tests.expect(phonex::parseDirectPhonemes("HH BOGUS", script)
		== phonex::CompileStatus::BadPhone
		&& std::string(phonex::compileStatusText(phonex::CompileStatus::BadPhone)) == "BAD PHONE",
		"unknown direct phone is rejected deterministically");
	tests.expect(phonex::parseDirectPhonemes("[S1]", script)
		== phonex::CompileStatus::BadPhone, "stress suffix on consonant is rejected");
	tests.expect(phonex::parseDirectPhonemes("[HH EH", script)
		== phonex::CompileStatus::BadPhone, "unclosed direct escape is rejected");

	for (std::uint8_t index = 0; index < phonex::kBundledPhraseCount; ++index) {
		phonex::LpcSequence sequence;
		const auto status = phonex::compileBundledPhrase(index, sequence);
		tests.expect(status == phonex::CompileStatus::Ok && sequence.valid()
			&& sequence.frameCount > 0 && sequence.phraseId == index,
			"bundled phrase compiles " + std::to_string(index));
		for (std::uint16_t frameIndex = 0; frameIndex < sequence.frameCount; ++frameIndex) {
			const auto& frame = sequence.frames[frameIndex];
			tests.expect(std::isfinite(frame.energy) && std::isfinite(frame.pitchPeriod10k),
				"bundled frame is finite " + std::to_string(index));
			for (float coefficient : frame.reflection)
				tests.expect(std::isfinite(coefficient) && std::abs(coefficient) <= 0.995f,
					"bundled frame coefficient is stable " + std::to_string(index));
		}
	}
	tests.expect(phonex::bundledPhraseName(0) == "A"
		&& phonex::bundledPhraseName(25) == "Z"
		&& phonex::bundledPhraseName(36) == "HELLO"
		&& phonex::bundledPhraseName(62) == "LEVIATHAN"
		&& phonex::bundledPhraseName(63) == "PHONEX",
		"frozen bundled ordering sentinels");

	phonex::LpcSequence helloA;
	phonex::LpcSequence helloB;
	const bool helloOk = phonex::compileDirectPhonemes("HH EH L OW1", helloA)
		== phonex::CompileStatus::Ok
		&& phonex::compileDirectPhonemes("HH EH L OW1", helloB)
		== phonex::CompileStatus::Ok
		&& helloA.frameCount == helloB.frameCount;
	bool framesEqual = helloOk;
	for (std::uint16_t i = 0; framesEqual && i < helloA.frameCount; ++i) {
		framesEqual = helloA.frames[i].energy == helloB.frames[i].energy
			&& helloA.frames[i].pitchPeriod10k == helloB.frames[i].pitchPeriod10k
			&& helloA.frames[i].reflection == helloB.frames[i].reflection
			&& helloA.frames[i].excitation == helloB.frames[i].excitation;
	}
	tests.expect(framesEqual, "direct compilation is deterministic");
	phonex::LpcSequence aspirated;
	tests.expect(phonex::compileDirectPhonemes("HH EH", aspirated)
		== phonex::CompileStatus::Ok, "contextual aspiration compiles");
	const phonex::LpcFrame genericHh = phonex::quantizeTms5100Frame(
		phonex::phonePrototype(phonex::Phone::HH).anchors[0]);
	const phonex::LpcFrame vowelEh = phonex::quantizeTms5100Frame(
		phonex::phonePrototype(phonex::Phone::EH).anchors[0]);
	tests.expect(aspirated.frames[0].excitation == phonex::Excitation::Unvoiced
		&& aspirated.frames[0].reflection != genericHh.reflection
		&& std::abs(aspirated.frames[0].reflection[0] - vowelEh.reflection[0])
			< std::abs(genericHh.reflection[0] - vowelEh.reflection[0]),
		"HH aspiration inherits the following vowel tract");
	phonex::LpcSequence voicedFricative;
	tests.expect(phonex::compileDirectPhonemes("V AA", voicedFricative)
		== phonex::CompileStatus::Ok
		&& voicedFricative.frames[0].excitation == phonex::Excitation::Unvoiced
		&& voicedFricative.frames[1].excitation == phonex::Excitation::Voiced,
		"voiced fricative begins with a quiet turbulence frame");
	const auto unstressedEnergy = phonex::phonePrototype(phonex::Phone::EH).anchors[0].energy;
	phonex::LpcSequence stressed;
	tests.expect(phonex::compileDirectPhonemes("EH1", stressed) == phonex::CompileStatus::Ok
		&& stressed.frames[0].energy > unstressedEnergy
		&& stressed.frames[0].pitchPeriod10k
			< phonex::phonePrototype(phonex::Phone::EH).anchors[0].pitchPeriod10k,
		"primary stress raises energy and pitch");

	for (std::uint8_t index : {std::uint8_t{36}, std::uint8_t{38}, std::uint8_t{47},
		std::uint8_t{62}, std::uint8_t{63}}) {
		phonex::LpcSequence utterance;
		phonex::compileBundledPhrase(index, utterance);
		phonex::Engine engine;
		engine.setSequence(&utterance);
		phonex::EngineControls controls;
		controls.hostSampleRate = 48000.f;
		controls.speed = 0.f;
		float peak = 0.f;
		for (std::uint32_t sample = 0; sample < utterance.frameCount * 960u; ++sample)
			peak = std::max(peak, std::abs(engine.process(controls).audio));
		tests.expect(peak > 0.1f, "required direct audition phrase is audible "
			+ std::string(phonex::bundledPhraseName(index).data(),
				phonex::bundledPhraseName(index).size()));
	}
}

bool equalSequenceFrames(const phonex::LpcSequence& a, const phonex::LpcSequence& b) {
	if (a.frameCount != b.frameCount)
		return false;
	for (std::uint16_t i = 0; i < a.frameCount; ++i) {
		if (a.frames[i].energy != b.frames[i].energy
			|| a.frames[i].pitchPeriod10k != b.frames[i].pitchPeriod10k
			|| a.frames[i].reflection != b.frames[i].reflection
			|| a.frames[i].excitation != b.frames[i].excitation)
			return false;
	}
	return true;
}

void phase5PronunciationContract(Tests& tests) {
	auto compile = [&](phonex::StringView text, phonex::LpcSequence& sequence) {
		return phonex::compileText(text, sequence);
	};
	phonex::LpcSequence lower;
	phonex::LpcSequence normalized;
	tests.expect(compile("  hello\t", lower).status == phonex::CompileStatus::Ok
		&& compile("HELLO", normalized).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(lower, normalized),
		"text normalization handles case and whitespace");
	phonex::LpcSequence dictionary;
	phonex::LpcSequence authored;
	tests.expect(compile("machine", dictionary).status == phonex::CompileStatus::Ok
		&& phonex::compileDirectPhonemes("M AH SH IY1 N", authored) == phonex::CompileStatus::Ok
		&& equalSequenceFrames(dictionary, authored),
		"dictionary pronunciation takes precedence");
	tests.expect(compile("oscillator", dictionary).status == phonex::CompileStatus::Ok
		&& phonex::compileDirectPhonemes("AA1 S AH L EY T ER", authored)
			== phonex::CompileStatus::Ok
		&& equalSequenceFrames(dictionary, authored),
		"modular vocabulary uses an authored pronunciation");
	tests.expect(compile("fire truck", dictionary).status == phonex::CompileStatus::Ok
		&& phonex::compileDirectPhonemes("F AY1 ER SIL T R AH1 K", authored)
			== phonex::CompileStatus::Ok
		&& equalSequenceFrames(dictionary, authored),
		"FIRE TRUCK uses exact diphthong and stressed-vowel pronunciations");

	phonex::LpcSequence integer;
	phonex::LpcSequence integerWords;
	tests.expect(compile("125", integer).status == phonex::CompileStatus::Ok
		&& compile("one hundred twenty five", integerWords).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(integer, integerWords), "integer expansion 0..9999");
	phonex::LpcSequence large;
	phonex::LpcSequence digits;
	tests.expect(compile("12583", large).status == phonex::CompileStatus::Ok
		&& compile("one two five eight three", digits).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(large, digits), "large integer digit spelling");
	phonex::LpcSequence decimal;
	phonex::LpcSequence decimalWords;
	tests.expect(compile("-12.4", decimal).status == phonex::CompileStatus::Ok
		&& compile("minus twelve point four", decimalWords).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(decimal, decimalWords), "negative decimal expansion");
	phonex::LpcSequence zero;
	phonex::LpcSequence zeroWord;
	tests.expect(compile("0", zero).status == phonex::CompileStatus::Ok
		&& compile("zero", zeroWord).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(zero, zeroWord), "zero expansion is not dropped");

	phonex::LpcSequence directText;
	tests.expect(compile("hello [R OW1 B AA T]", directText).status
		== phonex::CompileStatus::Ok && directText.frameCount > normalized.frameCount,
		"direct phoneme escape composes with ordinary text");
	phonex::LpcSequence unchanged = normalized;
	tests.expect(compile("[HH NOPE]", unchanged).status == phonex::CompileStatus::BadPhone
		&& equalSequenceFrames(unchanged, normalized),
		"malformed direct escape leaves prior valid output active");

	for (phonex::StringView word : {"cheap", "ship", "thing", "phone", "sing", "queen",
		"back", "what", "seed", "read", "rain", "day", "boat", "coin", "boy",
		"cow", "out", "her", "bird", "burn", "city", "gem", "cats", "walked"}) {
		phonex::LpcSequence ruled;
		tests.expect(compile(word, ruled).status == phonex::CompileStatus::Ok
			&& ruled.frameCount > 0, "G2P ordered rule "
				+ std::string(word.data(), word.size()));
	}
	phonex::LpcSequence fallbackA;
	phonex::LpcSequence fallbackB;
	tests.expect(compile("x'y", fallbackA).status == phonex::CompileStatus::Ok
		&& compile("x'y", fallbackB).status == phonex::CompileStatus::Ok
		&& equalSequenceFrames(fallbackA, fallbackB),
		"unresolved ordinary word falls back to deterministic letter spelling");

	phonex::LpcSequence comma;
	phonex::LpcSequence sentence;
	compile("hello,", comma);
	compile("hello.", sentence);
	tests.expect(comma.frameCount == normalized.frameCount + 6,
		"comma contributes six silence frames");
	tests.expect(sentence.frameCount == normalized.frameCount + 12,
		"sentence ending contributes twelve silence frames");
	phonex::LpcSequence question;
	compile("hello?", question);
	bool contourDiffers = false;
	for (std::uint16_t i = 0; i < question.frameCount && i < sentence.frameCount; ++i) {
		if (question.frames[i].excitation == phonex::Excitation::Voiced
			&& question.frames[i].pitchPeriod10k != sentence.frames[i].pitchPeriod10k) {
			contourDiffers = true;
			break;
		}
	}
	tests.expect(contourDiffers, "question and sentence contours differ");

	std::string oversized(257, 'a');
	phonex::LpcSequence preserved = normalized;
	tests.expect(compile(oversized, preserved).status == phonex::CompileStatus::TextTooLong
		&& equalSequenceFrames(preserved, normalized),
		"source over 256 bytes is rejected without replacing output");
	std::string capacityStress(256, 'w');
	preserved = normalized;
	tests.expect(compile(capacityStress, preserved).status == phonex::CompileStatus::TextTooLong
		&& equalSequenceFrames(preserved, normalized),
		"compiled sequence over fixed capacity is rejected without truncation");
	phonex::LpcSequence unicode;
	const auto unicodeResult = compile("hello \xc3\xa9 robot", unicode);
	tests.expect(unicodeResult.status == phonex::CompileStatus::Ok
		&& unicodeResult.unsupportedUnicode && unicode.frameCount > normalized.frameCount,
		"unsupported Unicode is a nonfatal token boundary");

	phonex::SequenceMailbox mailbox;
	std::uint32_t observedGeneration = 0;
	tests.expect(mailbox.acquire(observedGeneration) == nullptr,
		"mailbox starts unpublished");
	const std::uint32_t generation1 = mailbox.publish(normalized);
	const phonex::LpcSequence* acquired = mailbox.acquire(observedGeneration);
	tests.expect(generation1 == 1 && acquired && observedGeneration == 1
		&& equalSequenceFrames(*acquired, normalized), "mailbox release/acquire publication");
	tests.expect(mailbox.acquire(observedGeneration) == nullptr,
		"mailbox does not reacquire unchanged generation");
	const std::uint32_t generation2 = mailbox.publish(integer);
	acquired = mailbox.acquire(observedGeneration);
	tests.expect(generation2 == 2 && acquired && observedGeneration == 2
		&& equalSequenceFrames(*acquired, integer), "mailbox alternates fixed slots");
	const std::uint32_t generation3 = mailbox.publish(decimal);
	const std::uint32_t generation4 = mailbox.publish(question);
	tests.expect(equalSequenceFrames(*acquired, integer),
		"rapid publications leave the audio-owned sequence immutable");
	acquired = mailbox.acquire(observedGeneration);
	tests.expect(generation3 == 3 && generation4 == 4 && acquired
		&& observedGeneration == 4 && equalSequenceFrames(*acquired, question),
		"rapid publications coalesce without overwriting the audio-owned slot");
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
	phase3Contract(tests);
	phase4CorpusAndCompilerContract(tests);
	phase5PronunciationContract(tests);
	std::cout << "[TEST SUMMARY] checks=" << tests.checks
		<< " failures=" << tests.failures << '\n';
	return tests.failures == 0 ? 0 : 1;
}
