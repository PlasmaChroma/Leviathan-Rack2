#include "../src/HelicalContinuumEngine.hpp"
#include "../src/DoorstopEngineRouter.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Result {
	std::string name;
	bool pass;
	std::string detail;
};

Result zeroStateRemainsZero() {
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(48000.f);
	bool exact = true;
	for (int i = 0; i < 48000; ++i) {
		const doorstop::Frame frame = engine.process(1.f / 48000.f);
		exact = exact && frame.outputVolts == 0.f && frame.displacement == 0.f
			&& frame.velocity == 0.f && frame.energy == 0.f
			&& frame.visualActivity == 0.f && frame.sleeping;
	}
	return {"V3 zero state remains exactly silent", exact,
		"exact=" + std::to_string(exact)};
}

Result supportedRatesAreFiniteAndPairedMotionIsVisible() {
	bool pass = true;
	float largestPeak = 0.f;
	int smallestCrossings = 1000000;
	for (float rate : {44100.f, 48000.f, 96000.f, 192000.f}) {
		doorstop::HelicalContinuumEngine engine;
		engine.setSampleRate(rate);
		engine.setSpecimenSeed(77u);
		engine.strike(0.75f);
		float peak = 0.f;
		float previous = 0.f;
		int crossings = 0;
		for (int i = 0; i < int(2.f * rate); ++i) {
			const doorstop::Frame frame = engine.process(1.f / rate);
			peak = std::max(peak, std::fabs(frame.outputVolts));
			if (i > int(0.020f * rate)
				&& ((previous < 0.f && frame.displacement >= 0.f)
					|| (previous > 0.f && frame.displacement <= 0.f))) ++crossings;
			previous = frame.displacement;
			pass = pass && std::isfinite(frame.outputVolts)
				&& std::isfinite(frame.displacement)
				&& std::fabs(frame.outputVolts) <= 5.0001f;
		}
		largestPeak = std::max(largestPeak, peak);
		smallestCrossings = std::min(smallestCrossings, crossings);
		pass = pass && peak > 0.01f && crossings >= 60 && crossings <= 110;
	}
	return {"V3 is finite across sample rates and retains the low paired motion",
		pass, "largestPeak=" + std::to_string(largestPeak)
			+ " smallestCrossings=" + std::to_string(smallestCrossings)};
}

Result specimenPopulationIsDeterministicAndDistinct() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine a;
	doorstop::HelicalContinuumEngine b;
	doorstop::HelicalContinuumEngine c;
	for (doorstop::HelicalContinuumEngine* engine : {&a, &b, &c}) {
		engine->setSampleRate(rate);
	}
	a.setSpecimenSeed(0x12345678u);
	b.setSpecimenSeed(0x12345678u);
	c.setSpecimenSeed(0x87654321u);
	a.strike(0.7f);
	b.strike(0.7f);
	c.strike(0.7f);
	bool exact = true;
	double distinct = 0.0;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const float sa = a.process(dt).outputVolts;
		const float sb = b.process(dt).outputVolts;
		const float sc = c.process(dt).outputVolts;
		exact = exact && sa == sb;
		distinct += std::fabs(sa - sc);
	}
	return {"V3 specimen coefficients are deterministic and distinct",
		exact && distinct > 10.0,
		"exact=" + std::to_string(exact)
			+ " distinct=" + std::to_string(distinct)};
}

float peakMeterEnergy(float velocity) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(1u);
	engine.strike(velocity);
	float peak = 0.f;
	for (int i = 0; i < int(rate); ++i) {
		peak = std::max(peak, engine.process(dt).energy);
	}
	return peak;
}

Result v3EnergyMeterHasUsefulStrikeRange() {
	const float gentle = peakMeterEnergy(0.25f);
	const float medium = peakMeterEnergy(0.50f);
	const float strong = peakMeterEnergy(0.80f);
	const bool pass = gentle >= 0.05f && gentle <= 0.20f
		&& medium >= gentle * 2.f && medium <= 0.60f
		&& strong >= medium * 1.5f && strong <= 0.95f;
	return {"V3 energy meter distinguishes gentle, medium, and strong strikes",
		pass, "gentle=" + std::to_string(gentle)
			+ " medium=" + std::to_string(medium)
			+ " strong=" + std::to_string(strong)};
}

struct StrikeResponse {
	float peakEnergy = 0.f;
	float peakDisplacement = 0.f;
	float peakOutput = 0.f;
	float rms400ms = 0.f;
};

StrikeResponse measureStrikeResponse(
	doorstop::HelicalTuningVariant tuning, float velocity) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(1u);
	engine.setTuningVariant(tuning);
	engine.strike(velocity);
	StrikeResponse response;
	double sum400ms = 0.0;
	for (int i = 0; i < int(rate); ++i) {
		const doorstop::Frame frame = engine.process(dt);
		response.peakEnergy = std::max(response.peakEnergy, frame.energy);
		response.peakDisplacement = std::max(
			response.peakDisplacement, std::fabs(frame.displacement));
		response.peakOutput = std::max(
			response.peakOutput, std::fabs(frame.outputVolts));
		if (i < int(0.4f * rate))
			sum400ms += double(frame.outputVolts) * frame.outputVolts;
	}
	response.rms400ms = std::sqrt(float(sum400ms / (0.4f * rate)));
	return response;
}

StrikeResponse measureStrikeFromState(
		doorstop::HelicalContinuumEngine engine, float velocity) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	engine.strike(velocity);
	StrikeResponse response;
	double sum = 0.0;
	for (int i = 0; i < int(0.4f * rate); ++i) {
		const doorstop::Frame frame = engine.process(dt);
		response.peakOutput = std::max(
			response.peakOutput, std::fabs(frame.outputVolts));
		sum += double(frame.outputVolts) * frame.outputVolts;
	}
	response.rms400ms = std::sqrt(float(sum / (0.4f * rate)));
	return response;
}

Result velocityResponseStaysMonotonicThroughMaximum() {
	bool pass = true;
	float smallestEnergyStep = 1.f;
	float smallestDisplacementStep = 1.f;
	float smallestOutputStep = 1.f;
	for (doorstop::HelicalTuningVariant tuning : {
		doorstop::HelicalTuningVariant::BoingProbe,
		doorstop::HelicalTuningVariant::DarkBoing,
		doorstop::HelicalTuningVariant::DeepSwing,
		doorstop::HelicalTuningVariant::DeepContinuum}) {
		StrikeResponse previous = measureStrikeResponse(tuning, 0.80f);
		for (float velocity : {0.85f, 0.90f, 0.95f, 1.00f}) {
			const StrikeResponse current = measureStrikeResponse(tuning, velocity);
			const float energyStep = current.peakEnergy - previous.peakEnergy;
			const float displacementStep =
				current.peakDisplacement - previous.peakDisplacement;
			const float outputStep = current.peakOutput - previous.peakOutput;
			smallestEnergyStep = std::min(smallestEnergyStep, energyStep);
			smallestDisplacementStep = std::min(
				smallestDisplacementStep, displacementStep);
			smallestOutputStep = std::min(smallestOutputStep, outputStep);
			// Energy and displacement telemetry are bounded at 1 and 2, while
			// output peak remains unsaturated in this strike range.
			pass = pass && energyStep >= 0.f && displacementStep >= 0.f
				&& outputStep > 0.f;
			previous = current;
		}
	}
	return {"V3 velocity response remains monotonic from 80% through maximum",
		pass, "smallestEnergyStep=" + std::to_string(smallestEnergyStep)
			+ " smallestDisplacementStep="
			+ std::to_string(smallestDisplacementStep)
			+ " smallestOutputStep=" + std::to_string(smallestOutputStep)};
}

Result deepSwingHardRetriggerRemainsLouderAcrossPhase() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	bool pass = true;
	float smallestPeakMargin = 100.f;
	float smallestRmsMargin = 100.f;
	for (std::uint32_t seed : {1u, 3076668551u}) {
		doorstop::HelicalContinuumEngine base;
		base.setSampleRate(rate);
		base.setSpecimenSeed(seed);
		base.setTuningVariant(doorstop::HelicalTuningVariant::DeepSwing);
		base.strike(0.7f);
		for (int phase = 0; phase <= 20; ++phase) {
			doorstop::HelicalContinuumEngine state = base;
			for (int i = 0; i < phase * 1200; ++i) state.process(dt);
			const StrikeResponse medium = measureStrikeFromState(state, 0.5f);
			const StrikeResponse hard = measureStrikeFromState(state, 1.f);
			const float peakMargin = hard.peakOutput - medium.peakOutput;
			const float rmsMargin = hard.rms400ms - medium.rms400ms;
			smallestPeakMargin = std::min(smallestPeakMargin, peakMargin);
			smallestRmsMargin = std::min(smallestRmsMargin, rmsMargin);
			pass = pass && peakMargin > 0.f && rmsMargin > 0.f;
		}
	}
	return {"Deep Swing 10 V retrigger stays louder than 5 V across retained phases",
		pass, "smallestPeakMargin=" + std::to_string(smallestPeakMargin)
			+ " smallestRmsMargin=" + std::to_string(smallestRmsMargin)};
}

StrikeResponse measurePeriodicRetrigger(
		doorstop::HelicalTuningVariant tuning,
		float velocity, float intervalSeconds) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(3076668551u);
	engine.setBreakIn(0.0239455067f);
	engine.setTuningVariant(tuning);
	const int intervalFrames = int(intervalSeconds * rate);
	StrikeResponse response;
	double sum = 0.0;
	for (int frame = 0; frame < 6 * intervalFrames; ++frame) {
		if (frame % intervalFrames == 0) engine.strike(velocity);
		const float output = engine.process(dt).outputVolts;
		if (frame >= 5 * intervalFrames) {
			response.peakOutput = std::max(response.peakOutput, std::fabs(output));
			sum += double(output) * output;
		}
	}
	response.rms400ms = std::sqrt(float(sum / intervalFrames));
	return response;
}

Result deepSwingPeriodicMaximumBeatsMidrange() {
	bool pass = true;
	float smallestPeakRatio = 100.f;
	float smallestRmsRatio = 100.f;
	for (float interval : {0.5f, 1.f, 2.f}) {
		const StrikeResponse hard = measurePeriodicRetrigger(
			doorstop::HelicalTuningVariant::DeepSwing, 1.f, interval);
		for (float mediumVelocity : {0.5f, 0.55f}) {
			const StrikeResponse medium = measurePeriodicRetrigger(
				doorstop::HelicalTuningVariant::DeepSwing,
				mediumVelocity, interval);
			const float peakRatio = hard.peakOutput
				/ std::max(medium.peakOutput, 1e-6f);
			const float rmsRatio = hard.rms400ms
				/ std::max(medium.rms400ms, 1e-6f);
			smallestPeakRatio = std::min(smallestPeakRatio, peakRatio);
			smallestRmsRatio = std::min(smallestRmsRatio, rmsRatio);
			pass = pass && peakRatio > 1.f && rmsRatio > 1.f;
		}
	}
	return {"Deep Swing periodic 10 V strikes stay louder than the midrange",
		pass, "smallestPeakRatio=" + std::to_string(smallestPeakRatio)
			+ " smallestRmsRatio=" + std::to_string(smallestRmsRatio)};
}

Result deepContinuumPeriodicMaximumBeatsMidrange() {
	bool pass = true;
	float smallestPeakRatio = 100.f;
	float smallestRmsRatio = 100.f;
	for (float interval : {0.5f, 1.f, 2.f}) {
		const StrikeResponse hard = measurePeriodicRetrigger(
			doorstop::HelicalTuningVariant::DeepContinuum, 1.f, interval);
		for (float mediumVelocity : {0.5f, 0.55f}) {
			const StrikeResponse medium = measurePeriodicRetrigger(
				doorstop::HelicalTuningVariant::DeepContinuum,
				mediumVelocity, interval);
			const float peakRatio = hard.peakOutput
				/ std::max(medium.peakOutput, 1e-6f);
			const float rmsRatio = hard.rms400ms
				/ std::max(medium.rms400ms, 1e-6f);
			smallestPeakRatio = std::min(smallestPeakRatio, peakRatio);
			smallestRmsRatio = std::min(smallestRmsRatio, rmsRatio);
			pass = pass && peakRatio > 1.f && rmsRatio > 1.f;
		}
	}
	return {"Deep Continuum periodic 10 V strikes stay above the midrange",
		pass, "smallestPeakRatio=" + std::to_string(smallestPeakRatio)
			+ " smallestRmsRatio=" + std::to_string(smallestRmsRatio)};
}

struct BodyBellEnergy {
	double body = 0.0;
	double bell = 0.0;
};

BodyBellEnergy measureBodyBellEnergy(
		doorstop::HelicalTuningVariant tuning,
		float velocity, std::uint32_t seed) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	const float low250Coefficient = 1.f
		- std::exp(-2.f * 3.14159265358979323846f * 250.f / rate);
	const float low400Coefficient = 1.f
		- std::exp(-2.f * 3.14159265358979323846f * 400.f / rate);
	const float low1500Coefficient = 1.f
		- std::exp(-2.f * 3.14159265358979323846f * 1500.f / rate);
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(seed);
	engine.setTuningVariant(tuning);
	engine.strike(velocity);
	float low250 = 0.f;
	float low400 = 0.f;
	float low1500 = 0.f;
	BodyBellEnergy energy;
	for (int i = 0; i < int(rate); ++i) {
		const float output = engine.process(dt).outputVolts;
		low250 += low250Coefficient * (output - low250);
		low400 += low400Coefficient * (output - low400);
		low1500 += low1500Coefficient * (output - low1500);
		const float bell = low1500 - low400;
		energy.body += double(low250) * low250;
		energy.bell += double(bell) * bell;
	}
	return energy;
}

Result deepSwingHardStrikeKeepsItsLowBody() {
	bool pass = true;
	float smallestBodyGain = 1e30f;
	float smallestRatioGain = 1e30f;
	for (std::uint32_t seed : {1u, 3076668551u}) {
		const BodyBellEnergy medium = measureBodyBellEnergy(
			doorstop::HelicalTuningVariant::DeepSwing, 0.5f, seed);
		const BodyBellEnergy hard = measureBodyBellEnergy(
			doorstop::HelicalTuningVariant::DeepSwing, 1.f, seed);
		const float bodyGain = float(hard.body / std::max(medium.body, 1e-12));
		const float mediumRatio = float(medium.body / std::max(medium.bell, 1e-12));
		const float hardRatio = float(hard.body / std::max(hard.bell, 1e-12));
		const float ratioGain = hardRatio / std::max(mediumRatio, 1e-12f);
		smallestBodyGain = std::min(smallestBodyGain, bodyGain);
		smallestRatioGain = std::min(smallestRatioGain, ratioGain);
		pass = pass && bodyGain > 1.f && ratioGain > 1.f;
	}
	return {"Deep Swing hard strikes favor low body instead of becoming a bell",
		pass, "smallestBodyGain=" + std::to_string(smallestBodyGain)
			+ " smallestBodyToBellGain=" + std::to_string(smallestRatioGain)};
}

Result deepContinuumIsAReactionBodyRevision() {
	bool pass = true;
	float smallestBodyGain = 1e30f;
	float smallestBodyRatio = 1e30f;
	for (std::uint32_t seed : {1u, 3076668551u}) {
		const BodyBellEnergy deepSwing = measureBodyBellEnergy(
			doorstop::HelicalTuningVariant::DeepSwing, 1.f, seed);
		const BodyBellEnergy continuum = measureBodyBellEnergy(
			doorstop::HelicalTuningVariant::DeepContinuum, 1.f, seed);
		const float bodyGain = float(
			continuum.body / std::max(deepSwing.body, 1e-12));
		const float deepRatio = float(
			deepSwing.body / std::max(deepSwing.bell, 1e-12));
		const float continuumRatio = float(
			continuum.body / std::max(continuum.bell, 1e-12));
		const float ratioGain = continuumRatio / std::max(deepRatio, 1e-12f);
		smallestBodyGain = std::min(smallestBodyGain, bodyGain);
		smallestBodyRatio = std::min(smallestBodyRatio, ratioGain);
		pass = pass && bodyGain > 0.65f && ratioGain > 1.f;
	}
	doorstop::HelicalContinuumEngine deepSwing;
	doorstop::HelicalContinuumEngine continuum;
	deepSwing.setSampleRate(48000.f);
	continuum.setSampleRate(48000.f);
	deepSwing.setSpecimenSeed(77u);
	continuum.setSpecimenSeed(77u);
	deepSwing.setTuningVariant(doorstop::HelicalTuningVariant::DeepSwing);
	continuum.setTuningVariant(doorstop::HelicalTuningVariant::DeepContinuum);
	deepSwing.strike(1.f);
	continuum.strike(1.f);
	double signatureDelta = 0.0;
	for (int i = 0; i < 48000; ++i) {
		signatureDelta += std::fabs(
			deepSwing.process(1.f / 48000.f).outputVolts
			- continuum.process(1.f / 48000.f).outputVolts);
	}
	const bool distinct = signatureDelta > 100.0;
	return {"Deep Continuum is a distinct reaction-body revision",
		pass && distinct,
		"bodyGain=" + std::to_string(smallestBodyGain)
			+ " bodyToBellGain=" + std::to_string(smallestBodyRatio)
			+ " signatureDelta=" + std::to_string(signatureDelta)};
}

Result deepSwingAudibleTailMatchesVisualSettling() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	bool pass = true;
	float largestAudibleTail = 0.f;
	float largestAudibleVisual = 0.f;
	float largestAudibleActivity = 0.f;
	float largestSettledPeak = 0.f;
	float largestSettledVisual = 0.f;
	for (std::uint32_t seed : {1u, 3076668551u}) {
		doorstop::HelicalContinuumEngine engine;
		engine.setSampleRate(rate);
		engine.setSpecimenSeed(seed);
		engine.setTuningVariant(doorstop::HelicalTuningVariant::DeepSwing);
		engine.strike(1.f);
		for (int i = 0; i < int(4.f * rate); ++i) {
			const doorstop::Frame frame = engine.process(dt);
			if (i >= int(2.f * rate) && i < int(3.f * rate)) {
				largestAudibleTail = std::max(
					largestAudibleTail, std::fabs(frame.outputVolts));
				largestAudibleVisual = std::max(
					largestAudibleVisual, std::fabs(frame.displacement));
				largestAudibleActivity = std::max(
					largestAudibleActivity, frame.visualActivity);
			}
			else if (i >= int(3.f * rate)) {
				largestSettledPeak = std::max(
					largestSettledPeak, std::fabs(frame.outputVolts));
				largestSettledVisual = std::max(
					largestSettledVisual, std::fabs(frame.displacement));
			}
		}
	}
	pass = largestAudibleTail > 0.02f && largestAudibleVisual > 0.04f
		&& largestAudibleActivity > 0.02f
		&& largestSettledPeak < 0.02f && largestSettledVisual < 0.01f;
	return {"Deep Swing audible tail remains visible until both settle",
		pass, "audibleTail=" + std::to_string(largestAudibleTail)
			+ " audibleVisual=" + std::to_string(largestAudibleVisual)
			+ " audibleActivity=" + std::to_string(largestAudibleActivity)
			+ " settledPeak=" + std::to_string(largestSettledPeak)
			+ " settledVisual=" + std::to_string(largestSettledVisual)};
}

struct TuningTrace {
	int displacementCrossings = 0;
	int initialCrossings = 0;
	float peakDisplacement = 0.f;
	double signature = 0.0;
};

TuningTrace renderTuningTrace(
	doorstop::HelicalTuningVariant tuning, float velocity = 0.8f) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine engine;
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(77u);
	engine.setTuningVariant(tuning);
	engine.strike(velocity);
	TuningTrace trace;
	float previous = 0.f;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const doorstop::Frame frame = engine.process(dt);
		if (i > int(0.020f * rate)
			&& ((previous < 0.f && frame.displacement >= 0.f)
				|| (previous > 0.f && frame.displacement <= 0.f))) {
			++trace.displacementCrossings;
			if (i < int(0.60f * rate)) ++trace.initialCrossings;
		}
		previous = frame.displacement;
		trace.peakDisplacement = std::max(
			trace.peakDisplacement, std::fabs(frame.displacement));
		trace.signature += double(i + 1) * frame.outputVolts;
	}
	return trace;
}

Result deepSwingTargetsHardStrikeDeformation() {
	const TuningTrace gentleDark = renderTuningTrace(
		doorstop::HelicalTuningVariant::DarkBoing, 0.50f);
	const TuningTrace gentleDeep = renderTuningTrace(
		doorstop::HelicalTuningVariant::DeepSwing, 0.50f);
	const TuningTrace hardDark = renderTuningTrace(
		doorstop::HelicalTuningVariant::DarkBoing, 1.f);
	const TuningTrace hardDeep = renderTuningTrace(
		doorstop::HelicalTuningVariant::DeepSwing, 1.f);
	const bool gentleUnchanged = gentleDark.signature == gentleDeep.signature
		&& gentleDark.peakDisplacement == gentleDeep.peakDisplacement;
	const bool hardSlower = hardDeep.initialCrossings < hardDark.initialCrossings;
	const bool hardWider = hardDeep.peakDisplacement
		> 1.25f * hardDark.peakDisplacement;
	return {"Deep swing softens and widens only hard initial strikes",
		gentleUnchanged && hardSlower && hardWider,
		"gentleExact=" + std::to_string(gentleUnchanged)
			+ " initialCrossings=" + std::to_string(hardDark.initialCrossings)
			+ "/" + std::to_string(hardDeep.initialCrossings)
			+ " peakDisplacement=" + std::to_string(hardDark.peakDisplacement)
			+ "/" + std::to_string(hardDeep.peakDisplacement)};
}

Result darkTuningIsLowerAndWaveformDistinct() {
	const TuningTrace reference = renderTuningTrace(
		doorstop::HelicalTuningVariant::BoingProbe);
	const TuningTrace dark = renderTuningTrace(
		doorstop::HelicalTuningVariant::DarkBoing);
	const bool lower = dark.displacementCrossings
		< int(0.90f * float(reference.displacementCrossings))
		&& dark.displacementCrossings
		> int(0.70f * float(reference.displacementCrossings));
	const bool distinct = std::fabs(reference.signature - dark.signature) > 100.0;
	return {"Dark V3 tuning is lower and waveform-distinct", lower && distinct,
		"crossings=" + std::to_string(reference.displacementCrossings)
			+ "/" + std::to_string(dark.displacementCrossings)
			+ " signatureDelta="
			+ std::to_string(std::fabs(reference.signature - dark.signature))};
}

Result routerReturnsExactV3Frames() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::HelicalContinuumEngine direct;
	doorstop::DoorstopEngineRouter routed;
	direct.setSampleRate(rate);
	routed.setSampleRate(rate);
	direct.setSpecimenSeed(7331u);
	routed.setSpecimenSeed(7331u);
	routed.setEngineMode(doorstop::EngineMode::ReferenceV3);
	for (int i = 0; i < int(0.020f * rate); ++i) routed.process(dt);
	direct.strike(0.8f);
	routed.strike(0.8f);
	bool exact = true;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const doorstop::Frame a = direct.process(dt);
		const doorstop::Frame b = routed.process(dt);
		exact = exact && a.outputVolts == b.outputVolts
			&& a.displacement == b.displacement && a.velocity == b.velocity
			&& a.energy == b.energy && a.visualActivity == b.visualActivity
			&& a.strikeLight == b.strikeLight
			&& a.sleeping == b.sleeping && a.enteredSleep == b.enteredSleep;
		if (!exact) break;
	}
	return {"Steady V3 routing returns the exact engine frame", exact,
		"exact=" + std::to_string(exact)};
}

} // namespace

int main() {
	const std::vector<Result> results {
		zeroStateRemainsZero(),
		supportedRatesAreFiniteAndPairedMotionIsVisible(),
		specimenPopulationIsDeterministicAndDistinct(),
		v3EnergyMeterHasUsefulStrikeRange(),
		velocityResponseStaysMonotonicThroughMaximum(),
		deepSwingHardRetriggerRemainsLouderAcrossPhase(),
		deepSwingPeriodicMaximumBeatsMidrange(),
		deepContinuumPeriodicMaximumBeatsMidrange(),
		deepSwingHardStrikeKeepsItsLowBody(),
		deepContinuumIsAReactionBodyRevision(),
		deepSwingAudibleTailMatchesVisualSettling(),
		darkTuningIsLowerAndWaveformDistinct(),
		deepSwingTargetsHardStrikeDeformation(),
		routerReturnsExactV3Frames(),
	};
	int failed = 0;
	std::cout << "Doorstop Helical Engine Spec\n-----------------------------\n";
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name
			<< " :: " << result.detail << "\n";
		if (!result.pass) ++failed;
	}
	std::cout << "-----------------------------\nSummary: "
		<< (results.size() - failed) << "/" << results.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
