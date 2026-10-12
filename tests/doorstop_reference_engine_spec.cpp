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
	bool pass = false;
	std::string detail;
};

Result referenceCrossingsAndSettles() {
	doorstop::ReferenceSpringEngine engine;
	engine.setSampleRate(48000.f);
	engine.setSpecimenSeed(0x12345678u);
	engine.strike(1.f);
	const float dt = 1.f / 48000.f;
	bool finite = true;
	bool slept = false;
	float peak = 0.f;
	float texturePeak = 0.f;
	float junctionForcePeak = 0.f;
	float junctionModalPeak = 0.f;
	float highBandActivity = 0.f;
	float previous = 0.f;
	float lowpass = 0.f;
	float macroUpperLowpass = 0.f;
	float macroLowerLowpass = 0.f;
	float upperBandLowpass = 0.f;
	double analyzedEnergy = 0.0;
	double sub180Energy = 0.0;
	double macroBendEnergy = 0.0;
	double upperBandEnergy = 0.0;
	const float lowpassAlpha = 1.f - std::exp(
		-2.f * 3.14159265358979323846f * 180.f * dt);
	const float macroUpperAlpha = 1.f - std::exp(
		-2.f * 3.14159265358979323846f * 55.f * dt);
	const float macroLowerAlpha = 1.f - std::exp(
		-2.f * 3.14159265358979323846f * 12.f * dt);
	const float upperBandLowpassAlpha = 1.f - std::exp(
		-2.f * 3.14159265358979323846f * 1200.f * dt);
	for (int i = 0; i < 48000 * 20; ++i) {
		const doorstop::Frame frame = engine.process(dt);
		peak = std::max(peak, std::fabs(frame.outputVolts));
		texturePeak = std::max(texturePeak,
			std::fabs(engine.getDiagnostics().dispersiveTexture));
		junctionForcePeak = std::max(junctionForcePeak,
			std::fabs(engine.getDiagnostics().junctionForce));
		junctionModalPeak = std::max(junctionModalPeak,
			std::fabs(engine.getDiagnostics().junctionModalDisplacement));
		highBandActivity += std::fabs(frame.outputVolts - previous);
		previous = frame.outputVolts;
		lowpass += (frame.outputVolts - lowpass) * lowpassAlpha;
		macroUpperLowpass +=
			(frame.outputVolts - macroUpperLowpass) * macroUpperAlpha;
		macroLowerLowpass +=
			(frame.outputVolts - macroLowerLowpass) * macroLowerAlpha;
		upperBandLowpass +=
			(frame.outputVolts - upperBandLowpass) * upperBandLowpassAlpha;
		if (i >= int(0.050f * 48000.f) && i < int(2.f * 48000.f)) {
			analyzedEnergy += double(frame.outputVolts) * double(frame.outputVolts);
			sub180Energy += double(lowpass) * double(lowpass);
			const double macroBend =
				double(macroUpperLowpass - macroLowerLowpass);
			macroBendEnergy += macroBend * macroBend;
			const double upper = double(frame.outputVolts - upperBandLowpass);
			upperBandEnergy += upper * upper;
		}
		finite = finite
			&& std::isfinite(frame.outputVolts)
			&& std::isfinite(frame.displacement)
			&& std::isfinite(frame.velocity)
			&& std::fabs(frame.outputVolts) <= 5.0001f;
		if (frame.enteredSleep) {
			slept = true;
			break;
		}
	}
	const std::uint64_t crossings = engine.getDiagnostics().crossingCount;
	const float sub180Ratio = float(sub180Energy / std::max(analyzedEnergy, 1e-12));
	const float macroBendRatio = float(
		macroBendEnergy / std::max(analyzedEnergy, 1e-12));
	const float upperBandRatio = float(
		upperBandEnergy / std::max(analyzedEnergy, 1e-12));
	// Keep a trace of the approximately 21 Hz physical bend without allowing it
	// to become a separate sub-bass shelf. Most low-body energy now belongs to
	// the resonant bridge above the macro motion.
	const bool balancedLowBody =
		macroBendRatio > 0.004f && macroBendRatio < 0.12f
		&& sub180Ratio > 0.04f && sub180Ratio < 0.30f;
	// The reference contains a persistent 1.2--3 kHz body, but the upper modes
	// must remain well below the lower metallic resonances.
	const bool controlledUpperBody =
		upperBandRatio > 0.08f && upperBandRatio < 0.55f;
	const bool pass = finite && slept && peak > 1.f
		&& highBandActivity > 10.f && crossings >= 8u
		&& texturePeak > 1e-4f
		&& junctionForcePeak > 1.f && junctionForcePeak <= 4320.001f
		&& junctionModalPeak > 1e-4f
		&& balancedLowBody && controlledUpperBody;
	return {"Reference engine balances low body and upper activity, articulates crossings, and settles", pass,
		"peak=" + std::to_string(peak)
			+ " activity=" + std::to_string(highBandActivity)
			+ " texturePeak=" + std::to_string(texturePeak)
			+ " junctionForcePeak=" + std::to_string(junctionForcePeak)
			+ " junctionModalPeak=" + std::to_string(junctionModalPeak)
			+ " crossings=" + std::to_string(crossings)
			+ " sub180Ratio=" + std::to_string(sub180Ratio)
			+ " macroBendRatio=" + std::to_string(macroBendRatio)
			+ " upperBandRatio=" + std::to_string(upperBandRatio)
			+ " slept=" + std::to_string(slept)};
}

Result supportedSampleRatesRemainFinite() {
	const std::array<float, 5> rates {{44100.f, 48000.f, 88200.f, 96000.f, 192000.f}};
	bool pass = true;
	std::string detail;
	for (float rate : rates) {
		doorstop::ReferenceSpringEngine engine;
		engine.setSampleRate(rate);
		engine.setSpecimenSeed(77u);
		engine.strike(1.f);
		const float dt = 1.f / rate;
		bool finite = true;
		bool slept = false;
		float peak = 0.f;
		// The measured reference body is intentionally long-lived. Allow enough
		// time for the longest 8 s T60 mode to cross the conservative sleep
		// threshold after the second strike.
		for (int i = 0; i < int(rate * 20.f); ++i) {
			if (i == int(rate * 0.31f)) engine.strike(-0.8f);
			const doorstop::Frame frame = engine.process(dt);
			peak = std::max(peak, std::fabs(frame.outputVolts));
			finite = finite && std::isfinite(frame.outputVolts)
				&& std::isfinite(frame.displacement)
				&& std::fabs(frame.outputVolts) <= 5.0001f;
			if (frame.enteredSleep) {
				slept = true;
				break;
			}
		}
		const bool ratePass = finite && slept && peak > 0.05f;
		pass = pass && ratePass;
		detail += std::to_string(int(rate)) + (ratePass ? " ok " : " bad ");
	}
	return {"Reference engine is bounded across supported sample rates", pass, detail};
}

Result specimenIsDeterministicAndDistinct() {
	doorstop::ReferenceSpringEngine first;
	doorstop::ReferenceSpringEngine second;
	doorstop::ReferenceSpringEngine different;
	first.setSampleRate(48000.f);
	second.setSampleRate(48000.f);
	different.setSampleRate(48000.f);
	first.setSpecimenSeed(123u);
	second.setSpecimenSeed(123u);
	different.setSpecimenSeed(124u);
	first.strike(0.75f);
	second.strike(0.75f);
	different.strike(0.75f);
	const float dt = 1.f / 48000.f;
	bool identical = true;
	float difference = 0.f;
	for (int i = 0; i < 48000; ++i) {
		const float a = first.process(dt).outputVolts;
		const float b = second.process(dt).outputVolts;
		const float c = different.process(dt).outputVolts;
		identical = identical && a == b;
		difference += std::fabs(a - c);
	}
	return {"Specimen seed is deterministic and produces stable variation",
		identical && difference > 1.f,
		"identical=" + std::to_string(identical)
			+ " difference=" + std::to_string(difference)};
}

float specimenHighFrequencyShare(std::uint32_t seed,
	doorstop::ReferenceSpringProfile profile =
		doorstop::ReferenceSpringProfile::ReferenceV1) {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	const float lowpassAlpha = 1.f - std::exp(
		-2.f * 3.14159265358979323846f * 500.f * dt);
	doorstop::ReferenceSpringEngine engine(profile);
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(seed);
	engine.strike(0.75f);
	float lowpass = 0.f;
	double lowEnergy = 0.0;
	double highEnergy = 0.0;
	for (int i = 0; i < int(2.f * rate); ++i) {
		const float sample = engine.process(dt).outputVolts;
		lowpass += (sample - lowpass) * lowpassAlpha;
		if (i < int(0.020f * rate)) continue;
		const float highpass = sample - lowpass;
		lowEnergy += double(lowpass) * double(lowpass);
		highEnergy += double(highpass) * double(highpass);
	}
	return float(highEnergy / std::max(lowEnergy + highEnergy, 1e-12));
}

Result correlatedSpecimenTraitsSpanDarkToBright() {
	// These fixed seeds land on opposite sides of the correlated brightness
	// trait. Preserve an audible whole-body tilt without returning to the
	// exaggerated upper-mode spread that made bright specimens tinny.
	const float brightShare = specimenHighFrequencyShare(77u);
	const float darkShare = specimenHighFrequencyShare(124u);
	return {"Bounded specimen traits retain dark and bright spring bodies",
		brightShare > darkShare + 0.08f,
		"brightShare=" + std::to_string(brightShare)
			+ " darkShare=" + std::to_string(darkShare)};
}

Result referenceV2IsDarkBoundedAndJunctionFree() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	constexpr std::uint32_t seed = 77u;
	doorstop::ReferenceSpringEngine engine(
		doorstop::ReferenceSpringProfile::DarkRefinedV2);
	engine.setSampleRate(rate);
	engine.setSpecimenSeed(seed);
	engine.strike(0.75f);
	bool finite = true;
	bool slept = false;
	float peak = 0.f;
	float junctionPeak = 0.f;
	for (int i = 0; i < int(20.f * rate); ++i) {
		const doorstop::Frame frame = engine.process(dt);
		peak = std::max(peak, std::fabs(frame.outputVolts));
		junctionPeak = std::max(junctionPeak,
			std::fabs(engine.getDiagnostics().junctionForce));
		finite = finite && std::isfinite(frame.outputVolts)
			&& std::isfinite(frame.displacement)
			&& std::fabs(frame.outputVolts) <= 5.0001f;
		if (frame.enteredSleep) {
			slept = true;
			break;
		}
	}
	const float v1Share = specimenHighFrequencyShare(seed);
	const float v2Share = specimenHighFrequencyShare(
		seed, doorstop::ReferenceSpringProfile::DarkRefinedV2);
	const std::uint64_t crossings = engine.getDiagnostics().crossingCount;
	const bool pass = finite && slept && peak > 0.1f
		&& junctionPeak == 0.f && crossings >= 20u
		&& v2Share + 0.03f < v1Share;
	return {"Reference V2 is dark, bounded, repeatedly articulated, and junction-free",
		pass,
		"peak=" + std::to_string(peak)
			+ " crossings=" + std::to_string(crossings)
			+ " junctionPeak=" + std::to_string(junctionPeak)
			+ " v1Share=" + std::to_string(v1Share)
			+ " v2Share=" + std::to_string(v2Share)
			+ " slept=" + std::to_string(slept)};
}

Result referenceV2RouterUsesExactComputationPath() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::ReferenceSpringEngine direct(
		doorstop::ReferenceSpringProfile::DarkRefinedV2);
	doorstop::DoorstopEngineRouter routed;
	direct.setSampleRate(rate);
	routed.setSampleRate(rate);
	direct.setSpecimenSeed(0x13579bdfu);
	routed.setSpecimenSeed(0x13579bdfu);
	routed.setEngineMode(doorstop::EngineMode::ReferenceV2);
	for (int i = 0; i < int(rate * 0.020f); ++i) {
		routed.process(dt);
	}
	direct.strike(0.75f);
	routed.strike(0.75f);
	bool exact = true;
	for (int i = 0; i < int(rate * 3.f); ++i) {
		const doorstop::Frame a = direct.process(dt);
		const doorstop::Frame b = routed.process(dt);
		exact = exact
			&& a.outputVolts == b.outputVolts
			&& a.displacement == b.displacement
			&& a.velocity == b.velocity
			&& a.energy == b.energy
			&& a.strikeLight == b.strikeLight
			&& a.sleeping == b.sleeping
			&& a.enteredSleep == b.enteredSleep;
		if (!exact) break;
	}
	return {"Steady Reference V2 routing returns its exact engine frame", exact,
		"exact=" + std::to_string(exact)};
}

Result legacyRouterUsesExactComputationPath() {
	constexpr float rate = 48000.f;
	constexpr float dt = 1.f / rate;
	doorstop::Engine direct;
	doorstop::DoorstopEngineRouter routed;
	direct.setSampleRate(rate);
	routed.setSampleRate(rate);
	direct.setSoundModel(doorstop::SoundModel::CoupledBody);
	routed.setSoundModel(doorstop::SoundModel::CoupledBody);
	routed.setEngineMode(doorstop::EngineMode::Legacy);
	for (int i = 0; i < int(rate * 0.020f); ++i) {
		routed.process(dt);
	}
	direct.strike(0.75f);
	routed.strike(0.75f);
	bool exact = true;
	for (int i = 0; i < int(rate * 3.f); ++i) {
		const doorstop::Frame a = direct.process(dt);
		const doorstop::Frame b = routed.process(dt);
		exact = exact
			&& a.outputVolts == b.outputVolts
			&& a.displacement == b.displacement
			&& a.velocity == b.velocity
			&& a.energy == b.energy
			&& a.strikeLight == b.strikeLight
			&& a.sleeping == b.sleeping
			&& a.enteredSleep == b.enteredSleep;
		if (!exact) break;
	}
	return {"Steady Legacy routing returns the unchanged engine frame", exact,
		"exact=" + std::to_string(exact)};
}

Result switchingIsBounded() {
	doorstop::DoorstopEngineRouter engine;
	engine.setSampleRate(48000.f);
	engine.strike(1.f);
	const float dt = 1.f / 48000.f;
	for (int i = 0; i < 2400; ++i) engine.process(dt);
	engine.setSoundModel(doorstop::SoundModel::DispersiveSpring);
	engine.setEngineMode(doorstop::EngineMode::Legacy);
	bool finite = true;
	float peak = 0.f;
	for (int i = 0; i < 1800; ++i) {
		if (i == 200) engine.setEngineMode(doorstop::EngineMode::ReferenceV1);
		if (i == 275) engine.setEngineMode(doorstop::EngineMode::ReferenceV2);
		const float sample = engine.process(dt).outputVolts;
		finite = finite && std::isfinite(sample) && std::fabs(sample) <= 5.0001f;
		peak = std::max(peak, std::fabs(sample));
	}
	return {"Rapid three-engine switching stays finite and bounded",
		finite && engine.getEngineMode() == doorstop::EngineMode::ReferenceV2,
		"peak=" + std::to_string(peak)
			+ " finalMode=" + std::to_string(int(engine.getEngineMode()))};
}

} // namespace

int main() {
	std::vector<Result> results;
	results.push_back(referenceCrossingsAndSettles());
	results.push_back(supportedSampleRatesRemainFinite());
	results.push_back(specimenIsDeterministicAndDistinct());
	results.push_back(correlatedSpecimenTraitsSpanDarkToBright());
	results.push_back(referenceV2IsDarkBoundedAndJunctionFree());
	results.push_back(referenceV2RouterUsesExactComputationPath());
	results.push_back(legacyRouterUsesExactComputationPath());
	results.push_back(switchingIsBounded());

	int failed = 0;
	std::cout << "Doorstop Reference Engine Spec\n";
	std::cout << "------------------------------\n";
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name
			<< " :: " << result.detail << "\n";
		if (!result.pass) ++failed;
	}
	std::cout << "------------------------------\n";
	std::cout << "Summary: " << (results.size() - failed)
		<< "/" << results.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
