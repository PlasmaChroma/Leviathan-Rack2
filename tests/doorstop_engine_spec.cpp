#include "../src/DoorstopEngine.hpp"

#include <algorithm>
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

struct StrikeStats {
	float peakVolts = 0.f;
	float peakDisplacement = 0.f;
	float peakLight = 0.f;
	float activeSeconds = 0.f;
	bool slept = false;
	bool finite = true;
};

StrikeStats renderStrike(float sampleRate, float velocity, float maxSeconds = 10.f) {
	doorstop::Engine engine;
	engine.setSampleRate(sampleRate);
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
	const float neg = doorstop::Engine::shapeStrike(-1.f);
	const float zero = doorstop::Engine::shapeStrike(0.f);
	const float medium = doorstop::Engine::shapeStrike(0.5f);
	const float maximum = doorstop::Engine::shapeStrike(1.f);
	const float over = doorstop::Engine::shapeStrike(2.f);
	const bool pass = neg == 0.f && zero == 0.f
		&& std::fabs(medium - 0.3f) < 1e-6f
		&& maximum == 1.f && over == 1.f;
	return {"Velocity curve clamps and shapes once", pass,
		"neg=" + std::to_string(neg) + " mid=" + std::to_string(medium)
		+ " max=" + std::to_string(maximum)};
}

Result zeroStrikeSleeps() {
	doorstop::Engine engine;
	engine.strike(0.f);
	const doorstop::Frame frame = engine.process(1.f / 48000.f);
	const bool pass = engine.isSleeping() && frame.sleeping && frame.outputVolts == 0.f
		&& frame.displacement == 0.f && frame.strikeLight == 0.f;
	return {"Zero strike is a complete no-op", pass,
		"sleeping=" + std::to_string(engine.isSleeping())};
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
		&& soft.activeSeconds < medium.activeSeconds
		&& medium.activeSeconds < hard.activeSeconds
		&& medium.peakVolts >= 2.0f && medium.peakVolts <= 4.6f
		&& hard.peakVolts <= 5.0001f;
	return {"Strike strength scales level, motion, and decay", pass,
		"softV=" + std::to_string(soft.peakVolts)
		+ " mediumV=" + std::to_string(medium.peakVolts)
		+ " hardV=" + std::to_string(hard.peakVolts)
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
				engine.strike(float((rng >> 9) & 0xffffu) / 65535.f);
			}
			const doorstop::Frame frame = engine.process(dt);
			peak = std::max(peak, std::fabs(frame.outputVolts));
			pass = pass && std::isfinite(frame.outputVolts)
				&& std::isfinite(frame.displacement)
				&& std::fabs(frame.displacement) <= engine.getTuning().maxDisplacement + 1e-5f
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
	results.push_back(zeroStrikeSleeps());
	results.push_back(strikeScaling());
	results.push_back(sampleRateStability());
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
