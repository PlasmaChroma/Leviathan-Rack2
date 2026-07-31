#include "../src/PuffyEngine.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <new>
#include <string>
#include <vector>

namespace {
bool gTrackAllocations = false;
std::size_t gAllocationCount = 0;
}

void* operator new(std::size_t size) {
	if (gTrackAllocations) {
		gAllocationCount++;
	}
	if (void* memory = std::malloc(size)) {
		return memory;
	}
	throw std::bad_alloc();
}

void* operator new[](std::size_t size) {
	return ::operator new(size);
}

void operator delete(void* memory) noexcept {
	std::free(memory);
}

void operator delete[](void* memory) noexcept {
	std::free(memory);
}

void operator delete(void* memory, std::size_t) noexcept {
	std::free(memory);
}

void operator delete[](void* memory, std::size_t) noexcept {
	std::free(memory);
}

namespace {

constexpr float kPi = 3.14159265358979323846f;

struct Result {
	std::string name;
	bool pass = false;
	std::string detail;
};

bool near(float actual, float expected, float tolerance) {
	return std::fabs(actual - expected) <= tolerance;
}

Result characterCurves() {
	puffy::DynamicsState dynamics;
	dynamics.fast = 0.7f;
	dynamics.transient = 0.4f;
	bool finite = true;
	bool bloomOdd = true;
	bool spineOdd = true;
	bool frenzyBounded = true;
	bool riptideOdd = true;
	bool riptideBounded = true;
	float previousSpine = -10.f;
	bool spineMonotonic = true;
	for (int ai = 0; ai <= 20; ++ai) {
		const float amount = float(ai) / 20.f;
		for (int xi = -400; xi <= 400; ++xi) {
			const float input = float(xi) / 100.f;
			const float bloom = puffy::Engine::processCharacter(
				puffy::Character::Bloom, input, amount, dynamics);
			const float bloomNegative = puffy::Engine::processCharacter(
				puffy::Character::Bloom, -input, amount, dynamics);
			const float spine = puffy::Engine::processCharacter(
				puffy::Character::Spine, input, amount, dynamics);
			const float spineNegative = puffy::Engine::processCharacter(
				puffy::Character::Spine, -input, amount, dynamics);
			const float frenzy = puffy::Engine::processCharacter(
				puffy::Character::Frenzy, input, amount, dynamics);
			const float riptide = puffy::Engine::processCharacter(
				puffy::Character::Riptide, input, amount, dynamics);
			const float riptideNegative = puffy::Engine::processCharacter(
				puffy::Character::Riptide, -input, amount, dynamics);
			finite = finite && std::isfinite(bloom) && std::isfinite(spine)
				&& std::isfinite(frenzy) && std::isfinite(riptide);
			bloomOdd = bloomOdd && near(bloom, -bloomNegative, 2e-6f);
			spineOdd = spineOdd && near(spine, -spineNegative, 2e-6f);
			riptideOdd = riptideOdd
				&& near(riptide, -riptideNegative, 2e-6f);
			frenzyBounded = frenzyBounded
				&& frenzy >= std::min(input, -1.25f) - 1e-5f
				&& frenzy <= std::max(input, 1.25f) + 1e-5f;
			riptideBounded = riptideBounded
				&& riptide >= std::min(input, -1.f) - 1e-5f
				&& riptide <= std::max(input, 1.f) + 1e-5f;
			if (ai == 20) {
				spineMonotonic = spineMonotonic && spine >= previousSpine - 1e-6f;
				previousSpine = spine;
			}
		}
	}
	const float edgeBelow = puffy::Engine::processCharacter(
		puffy::Character::Spine, 0.1f - 1e-6f, 1.f, dynamics);
	const float edgeAbove = puffy::Engine::processCharacter(
		puffy::Character::Spine, 0.1f + 1e-6f, 1.f, dynamics);
	const bool continuous = std::fabs(edgeAbove - edgeBelow) < 1e-4f;
	return {
		"Character curves are finite, bounded, symmetric where required, and continuous",
		finite && bloomOdd && spineOdd && frenzyBounded
			&& riptideOdd && riptideBounded && spineMonotonic && continuous,
		"finite=" + std::to_string(finite)
			+ " bloomOdd=" + std::to_string(bloomOdd)
			+ " spineOdd=" + std::to_string(spineOdd)
			+ " riptideOdd=" + std::to_string(riptideOdd)
			+ " riptideBounded=" + std::to_string(riptideBounded)
			+ " spineMonotonic=" + std::to_string(spineMonotonic)
			+ " edgeDelta=" + std::to_string(std::fabs(edgeAbove - edgeBelow))
	};
}

Result frenzyPolynomialLobes() {
	const auto countTurningPoints = [](const puffy::DynamicsState& dynamics) {
		float previous = puffy::Engine::processCharacter(
			puffy::Character::Frenzy, -1.f, 1.f, dynamics);
		float previousSlope = 0.f;
		int turningPoints = 0;
		for (int i = 1; i <= 800; ++i) {
			const float input = -1.f + 2.f * float(i) / 800.f;
			const float output = puffy::Engine::processCharacter(
				puffy::Character::Frenzy, input, 1.f, dynamics);
			const float slope = output - previous;
			if (std::fabs(slope) > 1e-7f) {
				if (previousSlope * slope < 0.f) {
					turningPoints++;
				}
				previousSlope = slope;
			}
			previous = output;
		}
		return turningPoints;
	};

	puffy::DynamicsState idle;
	puffy::DynamicsState active;
	active.fast = 1.f;
	active.transient = 1.f;
	const int idleTurns = countTurningPoints(idle);
	const int activeTurns = countTurningPoints(active);
	const float idleZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, idle);
	const float activeZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, active);
	const float activePositive = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.8f, 1.f, active);
	const float activeNegative = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, -0.8f, 1.f, active);
	return {
		"FRENZY has broad input-reactive polynomial lobes with anchored zero",
		idleTurns >= 2 && activeTurns >= 4
			&& near(idleZero, 0.f, 1e-7f)
			&& near(activeZero, 0.f, 1e-7f)
			&& std::fabs(activePositive + activeNegative) > 0.1f,
		"turns=" + std::to_string(idleTurns)
			+ "/" + std::to_string(activeTurns)
			+ " activePair=" + std::to_string(activeNegative)
			+ "/" + std::to_string(activePositive)
	};
}

Result riptideFractalAnchors() {
	puffy::DynamicsState dynamics;
	// At full amount RIPTIDE drives the shaper by 2.5, so these inputs land
	// exactly on four successive tent-curve landmarks.
	const float eighth = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.05f, 1.f, dynamics);
	const float quarter = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.10f, 1.f, dynamics);
	const float half = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.20f, 1.f, dynamics);
	const float threeQuarter = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.30f, 1.f, dynamics);
	const float outerJoin = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f, 1.f, dynamics);
	const float outerNotch = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.625f, 1.f, dynamics);
	const float outerRebound = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.70f, 1.f, dynamics);
	const float outerCrest = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 1.f, 1.f, dynamics);
	const float joinBelow = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f - 1e-6f, 1.f, dynamics);
	const float joinAbove = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f + 1e-6f, 1.f, dynamics);
	return {
		"RIPTIDE retains inner landmarks and adds a continuous outer fractal crest",
		near(eighth, 0.3182f, 1e-5f)
			&& near(quarter, 0.4516f, 1e-5f)
			&& near(half, 0.6248f, 1e-5f)
			&& near(threeQuarter, 0.8172f, 1e-5f)
			&& near(outerJoin, 1.f, 1e-6f)
			&& near(outerNotch, 0.6976f, 1e-5f)
			&& near(outerRebound, 0.7816f, 1e-5f)
			&& near(outerCrest, 1.f, 1e-6f)
			&& std::fabs(joinAbove - joinBelow) < 1e-4f,
		"anchors=" + std::to_string(eighth)
			+ "/" + std::to_string(quarter)
			+ "/" + std::to_string(half)
			+ "/" + std::to_string(threeQuarter)
			+ " outer=" + std::to_string(outerJoin)
			+ "/" + std::to_string(outerNotch)
			+ "/" + std::to_string(outerRebound)
			+ "/" + std::to_string(outerCrest)
	};
}

struct ToneStats {
	double inputSq = 0.0;
	double outputSq = 0.0;
	float peakLeft = 0.f;
	float peakRight = 0.f;
	float maxStereoDelta = 0.f;
	bool finite = true;
};

ToneStats renderTone(
	puffy::Engine& engine,
	float seconds,
	float amplitude,
	float frequency,
	float amount,
	int character,
	bool autoDeflate,
	float manualDeflate,
	bool identicalStereo = true) {
	const float sampleRate = engine.getSampleRate();
	const int samples = int(seconds * sampleRate);
	const int skip = int(0.1f * sampleRate);
	ToneStats stats;
	for (int i = 0; i < samples; ++i) {
		const float phase = 2.f * kPi * frequency * float(i) / sampleRate;
		const float left = amplitude * std::sin(phase);
		const float right = identicalStereo ? left : amplitude * 0.5f * std::cos(phase);
		const puffy::Frame frame = engine.process(
			left, right, amount, character, autoDeflate, manualDeflate);
		stats.finite = stats.finite && std::isfinite(frame.left)
			&& std::isfinite(frame.right);
		stats.peakLeft = std::max(stats.peakLeft, std::fabs(frame.left));
		stats.peakRight = std::max(stats.peakRight, std::fabs(frame.right));
		stats.maxStereoDelta =
			std::max(stats.maxStereoDelta, std::fabs(frame.left - frame.right));
		if (i >= skip) {
			stats.inputSq += double(left) * double(left);
			stats.outputSq += double(frame.left) * double(frame.left);
		}
	}
	return stats;
}

Result unityAndStereo() {
	puffy::Engine engine;
	engine.setSampleRate(48000.f);
	const ToneStats stats = renderTone(
		engine, 0.5f, 1.f, 1000.f, 0.f, 0, false, 0.f);
	const float ratio = float(std::sqrt(stats.outputSq / stats.inputSq));
	const float db = 20.f * std::log10(std::max(ratio, 1e-9f));
	return {
		"Amount zero is unity and identical stereo remains identical",
		stats.finite && std::fabs(db) <= 0.05f && stats.maxStereoDelta <= 1e-6f,
		"gainDb=" + std::to_string(db)
			+ " stereoDelta=" + std::to_string(stats.maxStereoDelta)
	};
}

Result linkedLimiter() {
	puffy::Engine engine;
	engine.setSampleRate(48000.f);
	float maximum = 0.f;
	float ratioError = 0.f;
	for (int i = 0; i < 48000; ++i) {
		const float phase = 2.f * kPi * 997.f * float(i) / 48000.f;
		const float left = 18.f * std::sin(phase);
		const float right = 4.5f * std::sin(phase);
		const puffy::Frame frame = engine.process(left, right, 0.f, 0, false, 0.f);
		maximum = std::max(maximum, std::max(std::fabs(frame.left), std::fabs(frame.right)));
		if (i > 1000 && std::fabs(frame.left) > 1e-4f) {
			ratioError = std::max(
				ratioError, std::fabs(frame.right / frame.left - 0.25f));
		}
	}
	return {
		"LIVE limiter holds 5 V and applies one linked stereo gain",
		maximum <= 5.00001f && ratioError < 2e-4f,
		"peak=" + std::to_string(maximum)
			+ " ratioError=" + std::to_string(ratioError)
	};
}

Result manualDeflateIsExact() {
	puffy::Engine dry;
	puffy::Engine trimmed;
	dry.setSampleRate(48000.f);
	trimmed.setSampleRate(48000.f);
	const ToneStats dryStats = renderTone(
		dry, 0.5f, 1.f, 431.f, 0.5f, 0, false, 0.f);
	const ToneStats trimStats = renderTone(
		trimmed, 0.5f, 1.f, 431.f, 0.5f, 0, false, 0.5f);
	const float ratio = float(std::sqrt(trimStats.outputSq / dryStats.outputSq));
	const float expected = std::pow(10.f, -6.f / 20.f);
	return {
		"DEFLATE is an exact post-limiter dB trim",
		near(ratio, expected, 2e-5f),
		"ratio=" + std::to_string(ratio)
			+ " expected=" + std::to_string(expected)
	};
}

Result recoveryAndSilence() {
	puffy::Engine engine;
	engine.setSampleRate(96000.f);
	for (int i = 0; i < 2000; ++i) {
		engine.process(8.f, -6.f, 1.f, 2, true, 0.f);
	}
	const puffy::Frame invalid = engine.process(
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::infinity(),
		1.f, 2, true, 0.f);
	bool finite = std::isfinite(invalid.left) && std::isfinite(invalid.right);
	float tail = 0.f;
	for (int i = 0; i < 192000; ++i) {
		const int character = (i / 137) % 4;
		const puffy::Frame frame = engine.process(0.f, 0.f, 0.75f, character, true, 0.f);
		finite = finite && std::isfinite(frame.left) && std::isfinite(frame.right);
		if (i > 96000) {
			tail = std::max(tail, std::max(std::fabs(frame.left), std::fabs(frame.right)));
		}
	}
	return {
		"Non-finite recovery and rapid character changes settle to silence",
		finite && tail < 1e-6f,
		"finite=" + std::to_string(finite) + " tail=" + std::to_string(tail)
	};
}

Result characterTransitionsAreTransparentAndRetargetable() {
	puffy::Engine reference;
	puffy::Engine switched;
	reference.setSampleRate(48000.f);
	switched.setSampleRate(48000.f);
	float maximumDelta = 0.f;
	int finalCharacter = 0;
	for (int i = 0; i < 4000; ++i) {
		const float phase = 2.f * kPi * 997.f * float(i) / 48000.f;
		const float input = std::sin(phase);
		int requestedCharacter = 0;
		if (i >= 1000 && i < 1050) {
			requestedCharacter = 1;
		}
		else if (i >= 1050 && i < 1100) {
			requestedCharacter = 2;
		}
		else if (i >= 1100 && i < 1150) {
			requestedCharacter = 3;
		}
		const puffy::Frame referenceFrame = reference.process(
			input, input, 0.f, 0, false, 0.f);
		const puffy::Frame switchedFrame = switched.process(
			input, input, 0.f, requestedCharacter, false, 0.f);
		maximumDelta = std::max(
			maximumDelta,
			std::max(
				std::fabs(switchedFrame.left - referenceFrame.left),
				std::fabs(switchedFrame.right - referenceFrame.right)));
		finalCharacter = switchedFrame.character;
	}

	puffy::Engine queued;
	queued.setSampleRate(48000.f);
	for (int i = 0; i < 2000; ++i) {
		int requestedCharacter =
			i < 500 ? 0 : (i < 550 ? 1 : (i < 600 ? 2 : 3));
		const float phase = 2.f * kPi * 431.f * float(i) / 48000.f;
		const puffy::Frame frame = queued.process(
			2.f * std::sin(phase),
			2.f * std::sin(phase),
			0.75f,
			requestedCharacter,
			true,
			0.f);
		finalCharacter = frame.character;
	}

	return {
		"Character transitions stay unity at zero Puff and rapid retargets complete",
		maximumDelta <= 2e-6f && finalCharacter == int(puffy::Character::Riptide),
		"unityDelta=" + std::to_string(maximumDelta)
			+ " finalCharacter=" + std::to_string(finalCharacter)
	};
}

Result nonlinearGrowth() {
	bool pass = true;
	std::string detail;
	puffy::DynamicsState dynamics;
	dynamics.fast = 0.7f;
	dynamics.transient = 0.4f;
	for (int character = 0; character < 4; ++character) {
		float lowDeviation = 0.f;
		float highDeviation = 0.f;
		for (int i = -200; i <= 200; ++i) {
			const float input = float(i) / 200.f;
			const float low = puffy::Engine::processCharacter(
				static_cast<puffy::Character>(character), input, 0.2f, dynamics);
			const float high = puffy::Engine::processCharacter(
				static_cast<puffy::Character>(character), input, 1.f, dynamics);
			lowDeviation += std::fabs(low - input);
			highDeviation += std::fabs(high - input);
		}
		lowDeviation /= 401.f;
		highDeviation /= 401.f;
		pass = pass && highDeviation > lowDeviation + 0.01f;
		detail += " c" + std::to_string(character)
			+ "=" + std::to_string(lowDeviation)
			+ "/" + std::to_string(highDeviation);
	}
	return {
		"Every character's transfer deviates further from unity as PUFF rises",
		pass,
		detail
	};
}

Result bipolarInputActivityTracksStereoExcursions() {
	puffy::Engine engine;
	engine.setSampleRate(48000.f);
	puffy::Frame frame;
	for (int i = 0; i < 48000; ++i) {
		frame = engine.process(2.5f, -4.f, 0.f, 0, false, 0.f);
	}
	return {
		"Preview activity independently tracks positive and negative stereo excursions",
		near(frame.positiveInputActivity, 0.5f, 2e-3f)
			&& near(frame.negativeInputActivity, 0.8f, 2e-3f),
		"positive=" + std::to_string(frame.positiveInputActivity)
			+ " negative=" + std::to_string(frame.negativeInputActivity)
	};
}

Result realtimePathDoesNotAllocate() {
	puffy::Engine engine;
	engine.setSampleRate(192000.f);
	gAllocationCount = 0;
	gTrackAllocations = true;
	float peak = 0.f;
	for (int i = 0; i < 30000; ++i) {
		const float phase = 2.f * kPi * 997.f * float(i) / 192000.f;
		const puffy::Frame frame = engine.process(
			7.f * std::sin(phase),
			5.f * std::cos(phase),
			float(i % 1000) / 999.f,
			(i / 333) % 4,
			(i & 1) != 0,
			float(i % 101) / 100.f);
		peak = std::max(peak, std::max(std::fabs(frame.left), std::fabs(frame.right)));
	}
	gTrackAllocations = false;
	const std::size_t allocations = gAllocationCount;
	return {
		"Realtime engine path performs no dynamic allocation",
		allocations == 0 && std::isfinite(peak),
		"allocations=" + std::to_string(allocations)
			+ " peak=" + std::to_string(peak)
	};
}

} // namespace

int main() {
	const std::vector<Result> results = {
		characterCurves(),
		frenzyPolynomialLobes(),
		riptideFractalAnchors(),
		unityAndStereo(),
		linkedLimiter(),
		manualDeflateIsExact(),
		recoveryAndSilence(),
		characterTransitionsAreTransparentAndRetargetable(),
		nonlinearGrowth(),
		bipolarInputActivityTracksStereoExcursions(),
		realtimePathDoesNotAllocate()
	};
	int failures = 0;
	for (const Result& result : results) {
		std::cout << (result.pass ? "[PASS] " : "[FAIL] ") << result.name;
		if (!result.detail.empty()) {
			std::cout << " :: " << result.detail;
		}
		std::cout << '\n';
		if (!result.pass) {
			failures++;
		}
	}
	std::cout << "[SUMMARY] puffy_engine_spec: "
		<< (results.size() - std::size_t(failures)) << "/" << results.size()
		<< " passed\n";
	return failures == 0 ? 0 : 1;
}
