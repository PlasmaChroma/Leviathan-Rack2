#include "../src/DoorstopEngine.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

struct Result {
	std::string name;
	bool pass = false;
	std::string detail;
};

struct StrikeStats {
	float peakVolts = 0.f;
	float peakDisplacement = 0.f;
	float peakLight = 0.f;
	float activeSeconds = 0.f;
	bool slept = false;
	bool finite = true;
};

bool closeEnough(float actual, float expected, float tolerance = 1e-6f) {
	return std::fabs(actual - expected) <= tolerance;
}

StrikeStats renderStrike(float sampleRate, float velocity, float maxSeconds = 10.f) {
	doorstop::Engine engine;
	engine.setSampleRate(sampleRate);
	engine.setSoundModel(doorstop::SoundModel::Classic);
	engine.strike(velocity);
	StrikeStats stats;
	const float dt = 1.f / sampleRate;
	const int samples = int(maxSeconds * sampleRate);
	for (int i = 0; i < samples; ++i) {
		const doorstop::Frame frame = engine.process(dt);
		stats.peakVolts = std::max(stats.peakVolts, std::fabs(frame.outputVolts));
		stats.peakDisplacement = std::max(stats.peakDisplacement, std::fabs(frame.displacement));
		stats.peakLight = std::max(stats.peakLight, frame.strikeLight);
		stats.finite = stats.finite
			&& std::isfinite(frame.outputVolts)
			&& std::isfinite(frame.displacement)
			&& std::isfinite(frame.velocity)
			&& std::isfinite(frame.energy)
			&& std::isfinite(frame.strikeLight);
		if (frame.enteredSleep) {
			stats.activeSeconds = float(i + 1) * dt;
			stats.slept = true;
			break;
		}
	}
	return stats;
}

Result velocityCurve() {
	const float neg = doorstop::Engine::shapeMagnitude(-1.f);
	const float zero = doorstop::Engine::shapeMagnitude(0.f);
	const float medium = doorstop::Engine::shapeMagnitude(0.5f);
	const float maximum = doorstop::Engine::shapeMagnitude(1.f);
	const float over = doorstop::Engine::shapeMagnitude(2.f);
	const bool pass = neg == 0.f && zero == 0.f
		&& std::fabs(medium - 0.3f) < 1e-6f
		&& maximum == 1.f && over == 1.f;
	return {"Magnitude curve clamps and shapes once", pass,
		"neg=" + std::to_string(neg) + " mid=" + std::to_string(medium)
		+ " max=" + std::to_string(maximum)};
}

Result manualVerticalVelocity() {
	const float above = doorstop::Engine::manualVelocityFromVerticalPosition(-1.f);
	const float top = doorstop::Engine::manualVelocityFromVerticalPosition(0.f);
	const float middle = doorstop::Engine::manualVelocityFromVerticalPosition(0.5f);
	const float bottom = doorstop::Engine::manualVelocityFromVerticalPosition(1.f);
	const float below = doorstop::Engine::manualVelocityFromVerticalPosition(2.f);
	const bool pass = above == 1.f && top == 1.f
		&& std::fabs(middle - 0.55f) < 1e-6f
		&& std::fabs(bottom - 0.10f) < 1e-6f && below == bottom;
	return {"Manual strike height maps top-to-bottom velocity", pass,
		"top=" + std::to_string(top) + " middle=" + std::to_string(middle)
			+ " bottom=" + std::to_string(bottom)};
}

Result breakInProgression() {
	doorstop::Engine full;
	const bool fresh = full.getBreakIn() == 0.f
		&& !full.isBreakInLocked();
	full.strike(0.f);
	full.strike(std::numeric_limits<float>::quiet_NaN());
	full.strike(std::numeric_limits<float>::infinity());
	const bool invalidIgnored = full.getBreakIn() == 0.f;
	for (int i = 0; i < 1000; ++i) {
		full.strike((i & 1) ? -1.f : 1.f);
	}
	const bool reachedFull = closeEnough(full.getBreakIn(), 1.f, 2e-5f);
	full.strike(1.f);
	const bool capped = full.getBreakIn() == 1.f;

	doorstop::Engine positive;
	doorstop::Engine negative;
	positive.strike(0.5f);
	negative.strike(-0.5f);
	const float expectedMedium = std::pow(
		doorstop::Engine::shapeMagnitude(0.5f), 1.8f) / 1000.f;
	const bool magnitudeSymmetric =
		closeEnough(positive.getBreakIn(), expectedMedium, 1e-8f)
		&& closeEnough(negative.getBreakIn(), expectedMedium, 1e-8f);
	const float beforeRateChange = positive.getBreakIn();
	positive.setSampleRate(96000.f);
	for (int i = 0; i < 100; ++i) {
		positive.process(1.f / 96000.f);
	}
	const bool timeIndependent = positive.getBreakIn() == beforeRateChange;

	const bool pass = fresh && invalidIgnored && reachedFull && capped
		&& magnitudeSymmetric && timeIndependent;
	return {"Break-in dose is deterministic, symmetric, capped, and event-driven",
		pass,
		"full=" + std::to_string(full.getBreakIn())
			+ " medium=" + std::to_string(positive.getBreakIn())
			+ " expected=" + std::to_string(expectedMedium)};
}

Result breakInControlsAndResetSemantics() {
	doorstop::Engine engine;
	engine.setBreakIn(0.4f);
	engine.setBreakInLocked(true);
	engine.strike(1.f);
	const bool locked = closeEnough(engine.getBreakIn(), 0.4f)
		&& engine.isBreakInLocked();

	engine.resetMotion();
	const bool motionResetPreserved = engine.isSleeping()
		&& closeEnough(engine.getBreakIn(), 0.4f)
		&& engine.isBreakInLocked();

	engine.setBreakIn(2.f);
	const bool explicitSetWhileLocked = engine.getBreakIn() == 1.f;
	engine.setBreakIn(std::numeric_limits<float>::quiet_NaN());
	const bool invalidSetIgnored = engine.getBreakIn() == 1.f;
	engine.restoreFactoryFresh();
	const bool restoredButLocked = engine.getBreakIn() == 0.f
		&& engine.isBreakInLocked() && engine.isSleeping();

	engine.setBreakIn(0.6f);
	engine.reset();
	const bool fullReset = engine.getBreakIn() == 0.f
		&& !engine.isBreakInLocked() && engine.isSleeping();

	const bool pass = locked && motionResetPreserved && explicitSetWhileLocked
		&& invalidSetIgnored && restoredButLocked && fullReset;
	return {"Lock, motion reset, factory restoration, and full reset are distinct",
		pass,
		"locked=" + std::to_string(locked)
			+ " motionReset=" + std::to_string(motionResetPreserved)
			+ " restored=" + std::to_string(restoredButLocked)
			+ " reset=" + std::to_string(fullReset)};
}

Result breakInEffectiveEndpoints() {
	doorstop::Engine engine;
	const doorstop::Tuning factory = engine.getTuning();
	const doorstop::EffectiveTuning fresh = engine.getEffectiveTuning();
	engine.setBreakIn(1.f);
	const doorstop::EffectiveTuning worn = engine.getEffectiveTuning();

	bool pass = closeEnough(fresh.baseFrequencyHz, factory.baseFrequencyHz)
		&& closeEnough(fresh.dampingRatio, factory.dampingRatio)
		&& closeEnough(fresh.nonlinearStiffness, factory.nonlinearStiffness)
		&& closeEnough(fresh.maxDisplacement, factory.maxDisplacement)
		&& closeEnough(worn.baseFrequencyHz, factory.baseFrequencyHz * 0.84f)
		&& closeEnough(worn.dampingRatio, factory.dampingRatio * 0.65f)
		&& closeEnough(worn.nonlinearStiffness, factory.nonlinearStiffness * 0.72f)
		&& closeEnough(worn.maxDisplacement, factory.maxDisplacement * 1.15f);
	const float frequencyScales[] = {0.96f, 0.94f, 0.92f, 0.90f};
	const float decayScales[] = {1.25f, 1.22f, 1.18f, 1.12f};
	for (int i = 0; i < doorstop::MODE_COUNT; ++i) {
		pass = pass
			&& closeEnough(fresh.modeFrequenciesHz[i], factory.modeFrequenciesHz[i])
			&& closeEnough(fresh.modeDecayT60Seconds[i], factory.modeDecayT60Seconds[i])
			&& closeEnough(
				worn.modeFrequenciesHz[i],
				factory.modeFrequenciesHz[i] * frequencyScales[i])
			&& closeEnough(
				worn.modeDecayT60Seconds[i],
				factory.modeDecayT60Seconds[i] * decayScales[i]);
	}
	return {"Fresh and fully broken-in coefficients match documented endpoints",
		pass,
		"frequency=" + std::to_string(worn.baseFrequencyHz)
			+ " damping=" + std::to_string(worn.dampingRatio)
			+ " displacement=" + std::to_string(worn.maxDisplacement)};
}

Result zeroStrikeSleeps() {
	doorstop::Engine engine;
	engine.strike(std::numeric_limits<float>::quiet_NaN());
	engine.strike(std::numeric_limits<float>::infinity());
	engine.strike(0.f);
	const doorstop::Frame frame = engine.process(1.f / 48000.f);
	const bool pass = engine.isSleeping() && frame.sleeping && frame.outputVolts == 0.f
		&& frame.displacement == 0.f && frame.strikeLight == 0.f;
	return {"Zero strike is a complete no-op", pass,
		"sleeping=" + std::to_string(engine.isSleeping())};
}

Result bipolarStrikeDirection() {
	doorstop::Engine positive;
	doorstop::Engine negative;
	positive.strike(0.5f);
	negative.strike(-0.5f);
	const float dt = 1.f / 48000.f;
	const doorstop::Frame positiveFrame = positive.process(dt);
	const doorstop::Frame negativeFrame = negative.process(dt);
	const bool pass = positiveFrame.displacement > 0.f
		&& negativeFrame.displacement < 0.f
		&& positiveFrame.velocity > 0.f
		&& negativeFrame.velocity < 0.f
		&& std::fabs(positiveFrame.displacement + negativeFrame.displacement) < 1e-6f
		&& std::fabs(positiveFrame.velocity + negativeFrame.velocity) < 1e-6f
		&& std::fabs(positiveFrame.energy - negativeFrame.energy) < 1e-6f
		&& std::fabs(positiveFrame.strikeLight - negativeFrame.strikeLight) < 1e-6f;
	return {"Bipolar strikes reverse motion without changing strength", pass,
		"positive=" + std::to_string(positiveFrame.displacement)
		+ " negative=" + std::to_string(negativeFrame.displacement)};
}

Result strikeScaling() {
	const StrikeStats soft = renderStrike(48000.f, 0.1f);
	const StrikeStats medium = renderStrike(48000.f, 0.5f);
	const StrikeStats hard = renderStrike(48000.f, 1.f);
	const bool pass = soft.finite && medium.finite && hard.finite
		&& soft.slept && medium.slept && hard.slept
		&& soft.peakVolts < medium.peakVolts
		&& medium.peakVolts < hard.peakVolts
		&& soft.peakDisplacement < medium.peakDisplacement
		&& medium.peakDisplacement < hard.peakDisplacement
		&& hard.peakDisplacement > medium.peakDisplacement * 2.f
		&& soft.activeSeconds < medium.activeSeconds
		&& medium.activeSeconds < hard.activeSeconds
		&& medium.peakVolts >= 2.0f && medium.peakVolts <= 4.6f
		&& hard.peakVolts <= 5.0001f;
	return {"Strike strength scales level, motion, and decay", pass,
		"softV=" + std::to_string(soft.peakVolts)
		+ " mediumV=" + std::to_string(medium.peakVolts)
		+ " hardV=" + std::to_string(hard.peakVolts)
		+ " displacement=" + std::to_string(soft.peakDisplacement) + "/"
		+ std::to_string(medium.peakDisplacement) + "/" + std::to_string(hard.peakDisplacement)
		+ " times=" + std::to_string(soft.activeSeconds) + "/"
		+ std::to_string(medium.activeSeconds) + "/" + std::to_string(hard.activeSeconds)};
}

Result sampleRateStability() {
	const float rates[] = {44100.f, 48000.f, 88200.f, 96000.f, 192000.f};
	bool pass = true;
	std::string detail;
	for (float rate : rates) {
		const StrikeStats stats = renderStrike(rate, 1.f);
		const bool ok = stats.finite && stats.slept && stats.peakVolts <= 5.0001f;
		pass = pass && ok;
		detail += std::to_string(int(rate)) + "Hz=" + (ok ? "ok" : "bad") + " ";
	}
	return {"Single strike is finite at supported sample rates", pass, detail};
}

Result coupledModelStability() {
	doorstop::Engine engine;
	engine.setSampleRate(48000.f);
	engine.setSoundModel(doorstop::SoundModel::CoupledBody);
	engine.strike(1.f);
	float peak = 0.f;
	bool finite = true;
	bool slept = false;
	const float dt = 1.f / 48000.f;
	for (int i = 0; i < 48000 * 10; ++i) {
		const doorstop::Frame frame = engine.process(dt);
		peak = std::max(peak, std::fabs(frame.outputVolts));
		finite = finite && std::isfinite(frame.outputVolts)
			&& std::isfinite(frame.displacement)
			&& std::isfinite(frame.velocity);
		if (frame.enteredSleep) {
			slept = true;
			break;
		}
	}
	return {"Coupled body model remains finite and settles", finite && slept && peak <= 5.0001f,
		"peak=" + std::to_string(peak) + " slept=" + std::to_string(slept)};
}

Result coilContactModel() {
	doorstop::Engine coupled;
	doorstop::Engine contact;
	coupled.setSampleRate(48000.f);
	contact.setSampleRate(48000.f);
	coupled.setSoundModel(doorstop::SoundModel::CoupledBody);
	contact.setSoundModel(doorstop::SoundModel::CoilContact);
	coupled.strike(1.f);
	contact.strike(1.f);
	const float dt = 1.f / 48000.f;
	float difference = 0.f;
	bool finite = true;
	bool slept = false;
	for (int i = 0; i < 48000 * 10; ++i) {
		const doorstop::Frame coupledFrame = coupled.process(dt);
		const doorstop::Frame contactFrame = contact.process(dt);
		if (i < 48000 * 2) {
			difference += std::fabs(contactFrame.outputVolts - coupledFrame.outputVolts);
		}
		finite = finite && std::isfinite(contactFrame.outputVolts)
			&& std::fabs(contactFrame.outputVolts) <= 5.0001f;
		if (contactFrame.enteredSleep) {
			slept = true;
			break;
		}
	}
	doorstop::Engine mediumCoupled;
	doorstop::Engine mediumContact;
	mediumCoupled.setSampleRate(48000.f);
	mediumContact.setSampleRate(48000.f);
	mediumCoupled.setSoundModel(doorstop::SoundModel::CoupledBody);
	mediumContact.setSoundModel(doorstop::SoundModel::CoilContact);
	mediumCoupled.strike(0.5f);
	mediumContact.strike(0.5f);
	float mediumDifference = 0.f;
	for (int i = 0; i < 48000 * 2; ++i) {
		mediumDifference += std::fabs(
			mediumContact.process(dt).outputVolts
				- mediumCoupled.process(dt).outputVolts);
	}
	return {"Coil contact adds bounded collision behavior and settles",
		finite && slept && difference > 1.f && mediumDifference < 1e-4f,
		"hardDifference=" + std::to_string(difference)
			+ " mediumDifference=" + std::to_string(mediumDifference)
			+ " slept=" + std::to_string(slept)};
}

Result dispersiveSpringModel() {
	const float rates[] = {44100.f, 48000.f, 96000.f, 192000.f};
	bool pass = true;
	std::string detail;
	for (float rate : rates) {
		doorstop::Engine engine;
		engine.setSampleRate(rate);
		engine.setSoundModel(doorstop::SoundModel::DispersiveSpring);
		engine.strike(1.f);
		const float dt = 1.f / rate;
		float delayedPeak = 0.f;
		bool finite = true;
		bool slept = false;
		for (int i = 0; i < int(rate * 10.f); ++i) {
			const int retriggerPeriod = std::max(1, int(rate * 0.073f));
			if (i > 0 && i < int(rate * 2.f) && i % retriggerPeriod == 0) {
				engine.strike(((i / retriggerPeriod) & 1) ? -1.f : 1.f);
			}
			const doorstop::Frame frame = engine.process(dt);
			const float seconds = float(i) * dt;
			if (seconds > 0.020f && seconds < 0.50f) {
				delayedPeak = std::max(delayedPeak, std::fabs(frame.outputVolts));
			}
			finite = finite && std::isfinite(frame.outputVolts)
				&& std::fabs(frame.outputVolts) <= 5.0001f;
			if (frame.enteredSleep) {
				slept = true;
				break;
			}
		}
		const bool ok = finite && slept && delayedPeak > 0.25f;
		pass = pass && ok;
		detail += std::to_string(int(rate)) + "HzPeak="
			+ std::to_string(delayedPeak) + (ok ? " ok " : " bad ");
	}
	return {"Dispersive spring produces bounded delayed reflections and settles", pass, detail};
}

Result probabilisticMixModel() {
	doorstop::Engine engine;
	engine.setSampleRate(48000.f);
	engine.setSoundModel(doorstop::SoundModel::ProbabilisticMix);
	std::array<bool, 4> seen {{false, false, false, false}};
	bool finite = true;
	const float dt = 1.f / 48000.f;
	for (int strike = 0; strike < 64; ++strike) {
		engine.strike((strike & 1) ? -0.5f : 0.5f);
		const int selected = int(engine.getLastStrikeModel());
		if (selected >= 0 && selected < int(seen.size())) {
			seen[selected] = true;
		}
		for (int i = 0; i < 256; ++i) {
			const doorstop::Frame frame = engine.process(dt);
			finite = finite && std::isfinite(frame.outputVolts)
				&& std::fabs(frame.outputVolts) <= 5.0001f;
		}
	}
	const bool selectedAll = std::all_of(seen.begin(), seen.end(), [](bool value) {
		return value;
	});
	return {"Probabilistic mix selects all four bounded excitation models",
		engine.getSoundModel() == doorstop::SoundModel::ProbabilisticMix
			&& selectedAll && finite,
		"seen=" + std::to_string(seen[0]) + std::to_string(seen[1])
			+ std::to_string(seen[2]) + std::to_string(seen[3])};
}

Result abusiveRetriggerStability() {
	const float rates[] = {44100.f, 96000.f, 192000.f};
	bool pass = true;
	std::string detail;
	for (float rate : rates) {
		doorstop::Engine engine;
		engine.setSampleRate(rate);
		std::uint32_t rng = 0x8f31a26du;
		float peak = 0.f;
		const float dt = 1.f / rate;
		const int samples = int(rate * 5.f);
		for (int i = 0; i < samples; ++i) {
			rng ^= rng << 13;
			rng ^= rng >> 17;
			rng ^= rng << 5;
			if ((rng & 0x1ffu) == 0u) {
				const float randomVelocity = 2.f * float((rng >> 9) & 0xffffu) / 65535.f - 1.f;
				engine.strike(randomVelocity);
			}
			const doorstop::Frame frame = engine.process(dt);
			peak = std::max(peak, std::fabs(frame.outputVolts));
			pass = pass && std::isfinite(frame.outputVolts)
				&& std::isfinite(frame.displacement)
				&& std::fabs(frame.displacement)
					<= engine.getEffectiveTuning().maxDisplacement + 1e-5f
				&& peak <= 5.0001f;
		}
		detail += std::to_string(int(rate)) + "HzPeak=" + std::to_string(peak) + " ";
	}
	return {"Random abusive retriggers remain bounded", pass, detail};
}

} // namespace

int main() {
	std::vector<Result> results;
	results.push_back(velocityCurve());
	results.push_back(manualVerticalVelocity());
	results.push_back(breakInProgression());
	results.push_back(breakInControlsAndResetSemantics());
	results.push_back(breakInEffectiveEndpoints());
	results.push_back(zeroStrikeSleeps());
	results.push_back(bipolarStrikeDirection());
	results.push_back(strikeScaling());
	results.push_back(sampleRateStability());
	results.push_back(coupledModelStability());
	results.push_back(coilContactModel());
	results.push_back(dispersiveSpringModel());
	results.push_back(probabilisticMixModel());
	results.push_back(abusiveRetriggerStability());

	int failed = 0;
	std::cout << "Doorstop Engine Spec\n";
	std::cout << "--------------------\n";
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name
			<< " :: " << result.detail << "\n";
		if (!result.pass) failed++;
	}
	std::cout << "--------------------\n";
	std::cout << "Summary: " << (results.size() - failed) << "/" << results.size() << " passed\n";
	return failed == 0 ? 0 : 1;
}
