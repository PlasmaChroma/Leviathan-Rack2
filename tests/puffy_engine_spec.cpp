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
	bool voidOdd = true;
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
			const float voidOutput = puffy::Engine::processCharacter(
				puffy::Character::Void, input, amount, dynamics);
			const float voidNegative = puffy::Engine::processCharacter(
				puffy::Character::Void, -input, amount, dynamics);
			finite = finite && std::isfinite(bloom) && std::isfinite(spine)
				&& std::isfinite(frenzy) && std::isfinite(riptide)
				&& std::isfinite(voidOutput);
			bloomOdd = bloomOdd && near(bloom, -bloomNegative, 2e-6f);
			spineOdd = spineOdd && near(spine, -spineNegative, 2e-6f);
			riptideOdd = riptideOdd
				&& near(riptide, -riptideNegative, 2e-6f);
			voidOdd = voidOdd && near(voidOutput, -voidNegative, 2e-6f);
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
			&& riptideOdd && riptideBounded && voidOdd
			&& spineMonotonic && continuous,
		"finite=" + std::to_string(finite)
			+ " bloomOdd=" + std::to_string(bloomOdd)
			+ " spineOdd=" + std::to_string(spineOdd)
			+ " riptideOdd=" + std::to_string(riptideOdd)
			+ " voidOdd=" + std::to_string(voidOdd)
			+ " riptideBounded=" + std::to_string(riptideBounded)
			+ " spineMonotonic=" + std::to_string(spineMonotonic)
			+ " edgeDelta=" + std::to_string(std::fabs(edgeAbove - edgeBelow))
	};
}

Result voidOpensSmoothDeadZone() {
	puffy::DynamicsState dynamics;
	const float unity = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.2f, 0.f, dynamics);
	const float inside = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.29f, 1.f, dynamics);
	const float boundary = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.30f, 1.f, dynamics);
	const float justOutside = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.301f, 1.f, dynamics);
	const float middle = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.50f, 1.f, dynamics);
	const float rail = puffy::Engine::processCharacter(
		puffy::Character::Void, 1.f, 1.f, dynamics);
	const float negativeMiddle = puffy::Engine::processCharacter(
		puffy::Character::Void, -0.50f, 1.f, dynamics);
	return {
		"VOID opens a continuous soft dead zone and reconnects at the rail",
		near(unity, 0.2f, 1e-7f)
			&& near(inside, 0.f, 1e-7f)
			&& near(boundary, 0.f, 1e-7f)
			&& justOutside > 0.f && justOutside < 1e-4f
			&& middle > 0.15f && middle < 0.25f
			&& near(rail, 1.f, 1e-7f)
			&& near(middle, -negativeMiddle, 1e-7f),
		"unity=" + std::to_string(unity)
			+ " deadZone=" + std::to_string(inside)
			+ "/" + std::to_string(boundary)
			+ " outside=" + std::to_string(justOutside)
			+ " middle=" + std::to_string(middle)
			+ " rail=" + std::to_string(rail)
	};
}

Result frenzySinusoidalFold() {
	const auto measure = [](float amount, const puffy::DynamicsState& dynamics) {
		float previous = puffy::Engine::processCharacter(
			puffy::Character::Frenzy, -1.f, amount, dynamics);
		float previousSlope = 0.f;
		int turningPoints = 0;
		float peak = std::fabs(previous);
		for (int i = 1; i <= 800; ++i) {
			const float input = -1.f + 2.f * float(i) / 800.f;
			const float output = puffy::Engine::processCharacter(
				puffy::Character::Frenzy, input, amount, dynamics);
			peak = std::max(peak, std::fabs(output));
			const float slope = output - previous;
			if (std::fabs(slope) > 1e-7f) {
				if (previousSlope * slope < 0.f) {
					turningPoints++;
				}
				previousSlope = slope;
			}
			previous = output;
		}
		return std::make_pair(turningPoints, peak);
	};

	puffy::DynamicsState idle;
	puffy::DynamicsState active;
	active.fast = 1.f;
	active.transient = 1.f;
	const auto lowPuff = measure(0.5f, active);
	const auto highPuff = measure(1.f, active);
	const float idleZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, idle);
	const float activeZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, active);
	const float activePositive = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.8f, 1.f, active);
	const float activeNegative = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, -0.8f, 1.f, active);
	return {
		"FRENZY adds contracted sinusoidal-fold lobes as PUFF rises",
		lowPuff.first >= 4 && highPuff.first >= lowPuff.first + 2
			&& highPuff.second > 0.65f && highPuff.second < 0.68f
			&& near(idleZero, 0.f, 1e-7f)
			&& near(activeZero, 0.f, 1e-7f)
			&& std::fabs(activePositive + activeNegative) > 0.05f,
		"turns=" + std::to_string(lowPuff.first)
			+ "/" + std::to_string(highPuff.first)
			+ " highPeak=" + std::to_string(highPuff.second)
			+ " activePair=" + std::to_string(activeNegative)
			+ "/" + std::to_string(activePositive)
	};
}

Result riptideFractalAnchors() {
	puffy::DynamicsState dynamics;
	int slopeReversals = 0;
	float previous = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.f, 1.f, dynamics);
	float previousDelta = 0.f;
	for (int i = 1; i <= 32; ++i) {
		const float current = puffy::Engine::processCharacter(
			puffy::Character::Riptide, float(i) / 80.f, 1.f, dynamics);
		const float delta = current - previous;
		if (std::fabs(delta) > 1e-6f) {
			if (previousDelta * delta < 0.f) {
				++slopeReversals;
			}
			previousDelta = delta;
		}
		previous = current;
	}

	// At full amount RIPTIDE drives the shaper by 2.5. These inputs therefore
	// pin successive landmarks in the five-level dyadic response.
	const float nearZeroRise = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.0125f, 1.f, dynamics);
	const float firstNotch = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.0375f, 1.f, dynamics);
	const float firstCrest = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.075f, 1.f, dynamics);
	const float centralTrench = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.2375f, 1.f, dynamics);
	const float outerRebound = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.3375f, 1.f, dynamics);
	const float rail = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f, 1.f, dynamics);
	const float beyondRail = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.70f, 1.f, dynamics);
	const float joinBelow = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f - 1e-6f, 1.f, dynamics);
	const float joinAbove = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.40f + 1e-6f, 1.f, dynamics);
	return {
		"RIPTIDE follows the pinned multi-scale folds and joins its outer rail continuously",
		near(nearZeroRise, 0.2135f, 1e-5f)
			&& near(firstNotch, 0.2240f, 1e-5f)
			&& near(firstCrest, 0.5851f, 1e-5f)
			&& near(centralTrench, 0.2240f, 1e-5f)
			&& near(outerRebound, 0.8733f, 1e-5f)
			&& near(rail, 1.f, 1e-6f)
			&& near(beyondRail, 1.f, 1e-6f)
			&& firstCrest > firstNotch
			&& outerRebound > centralTrench
			&& slopeReversals >= 10
			&& std::fabs(joinAbove - joinBelow) < 1e-4f,
		"anchors=" + std::to_string(nearZeroRise)
			+ "/" + std::to_string(firstNotch)
			+ "/" + std::to_string(firstCrest)
			+ "/" + std::to_string(centralTrench)
			+ "/" + std::to_string(outerRebound)
			+ " rail=" + std::to_string(rail)
			+ "/" + std::to_string(beyondRail)
			+ " reversals=" + std::to_string(slopeReversals)
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
	bool identicalStereo = true,
	float wetMix = 1.f) {
	const float sampleRate = engine.getSampleRate();
	const int samples = int(seconds * sampleRate);
	const int skip = int(0.1f * sampleRate);
	ToneStats stats;
	for (int i = 0; i < samples; ++i) {
		const float phase = 2.f * kPi * frequency * float(i) / sampleRate;
		const float left = amplitude * std::sin(phase);
		const float right = identicalStereo ? left : amplitude * 0.5f * std::cos(phase);
		const puffy::Frame frame = engine.process(
			left, right, amount, character, autoDeflate, manualDeflate, wetMix);
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

Result wetDryMixEndpoints() {
	puffy::Engine dry;
	puffy::Engine wet;
	dry.setSampleRate(48000.f);
	wet.setSampleRate(48000.f);
	const ToneStats dryStats = renderTone(
		dry, 0.5f, 1.f, 431.f, 1.f, int(puffy::Character::Frenzy),
		true, 0.f, true, 0.f);
	const ToneStats wetStats = renderTone(
		wet, 0.5f, 1.f, 431.f, 1.f, int(puffy::Character::Frenzy),
		true, 0.f, true, 1.f);
	const float dryRatio = float(std::sqrt(dryStats.outputSq / dryStats.inputSq));
	const float wetRatio = float(std::sqrt(wetStats.outputSq / wetStats.inputSq));
	const float dryDb = 20.f * std::log10(std::max(dryRatio, 1e-9f));
	return {
		"MIX endpoints retain latency-matched dry and fully processed wet signals",
		dryStats.finite && wetStats.finite && std::fabs(dryDb) <= 0.05f
			&& std::fabs(wetRatio - dryRatio) > 0.05f,
		"dryDb=" + std::to_string(dryDb)
			+ " wetRatio=" + std::to_string(wetRatio)
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
		const int character = (i / 137) % puffy::kCharacterCount;
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
		finalCharacter = switchedFrame.negativeCharacter;
	}

	puffy::Engine queued;
	queued.setSampleRate(48000.f);
	for (int i = 0; i < 2000; ++i) {
		int requestedCharacter = i < 500
			? 0
			: (i < 550 ? 1 : (i < 600 ? 2 : (i < 650 ? 3 : 4)));
		const float phase = 2.f * kPi * 431.f * float(i) / 48000.f;
		const puffy::Frame frame = queued.process(
			2.f * std::sin(phase),
			2.f * std::sin(phase),
			0.75f,
			requestedCharacter,
			true,
			0.f);
		finalCharacter = frame.negativeCharacter;
	}

	return {
		"Character transitions stay unity at zero Puff and rapid retargets complete",
		maximumDelta <= 2e-6f && finalCharacter == int(puffy::Character::Void),
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
	for (int character = 0; character < puffy::kCharacterCount; ++character) {
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

Result splitCharactersCreateContinuousAsymmetry() {
	puffy::DynamicsState dynamics;
	const float negativeZero = puffy::Engine::processCharacter(
		puffy::Character::Void, -0.f, 1.f, dynamics);
	const float positiveZero = puffy::Engine::processCharacter(
		puffy::Character::Bloom, 0.f, 1.f, dynamics);

	puffy::Engine linked;
	puffy::Engine split;
	linked.setSampleRate(48000.f);
	split.setSampleRate(48000.f);
	float splitDelta = 0.f;
	for (int i = 0; i < 48000; ++i) {
		const float phase = 2.f * kPi * 101.f * float(i) / 48000.f;
		const float input = 2.f * std::sin(phase);
		const puffy::Frame linkedFrame = linked.process(
			input, input, 1.f,
			int(puffy::Character::Bloom), int(puffy::Character::Bloom),
			false, 0.f);
		const puffy::Frame splitFrame = split.process(
			input, input, 1.f,
			int(puffy::Character::Void), int(puffy::Character::Bloom),
			false, 0.f);
		if (i >= 24000) {
			splitDelta = std::max(
				splitDelta,
				std::fabs(splitFrame.left - linkedFrame.left));
		}
	}
	return {
		"Split characters meet at zero and produce a distinct asymmetric transfer",
		near(negativeZero, positiveZero, 1e-7f)
			&& splitDelta > 0.05f,
		"zero=" + std::to_string(negativeZero)
			+ "/" + std::to_string(positiveZero)
			+ " splitDelta=" + std::to_string(splitDelta)
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
			(i / 333) % puffy::kCharacterCount,
			(i / 271 + 1) % puffy::kCharacterCount,
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
		voidOpensSmoothDeadZone(),
		frenzySinusoidalFold(),
		riptideFractalAnchors(),
		unityAndStereo(),
		linkedLimiter(),
		manualDeflateIsExact(),
		wetDryMixEndpoints(),
		recoveryAndSilence(),
		characterTransitionsAreTransparentAndRetargetable(),
		nonlinearGrowth(),
		splitCharactersCreateContinuousAsymmetry(),
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
