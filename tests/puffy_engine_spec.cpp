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
	bool teethBounded = true;
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
			const float teeth = puffy::Engine::processCharacter(
				puffy::Character::Teeth, input, amount, dynamics);
			const float riptide = puffy::Engine::processCharacter(
				puffy::Character::Riptide, input, amount, dynamics);
			const float riptideNegative = puffy::Engine::processCharacter(
				puffy::Character::Riptide, -input, amount, dynamics);
			const float voidOutput = puffy::Engine::processCharacter(
				puffy::Character::Void, input, amount, dynamics);
			const float voidNegative = puffy::Engine::processCharacter(
				puffy::Character::Void, -input, amount, dynamics);
			finite = finite && std::isfinite(bloom) && std::isfinite(spine)
				&& std::isfinite(frenzy) && std::isfinite(teeth)
				&& std::isfinite(riptide)
				&& std::isfinite(voidOutput);
			bloomOdd = bloomOdd && near(bloom, -bloomNegative, 2e-6f);
			spineOdd = spineOdd && near(spine, -spineNegative, 2e-6f);
			riptideOdd = riptideOdd
				&& near(riptide, -riptideNegative, 2e-6f);
			voidOdd = voidOdd && near(voidOutput, -voidNegative, 2e-6f);
			frenzyBounded = frenzyBounded
				&& frenzy >= std::min(input, -1.25f) - 1e-5f
				&& frenzy <= std::max(input, 1.25f) + 1e-5f;
			teethBounded = teethBounded
				&& teeth >= std::min(input, -1.f) - 1e-5f
				&& teeth <= std::max(input, 1.f) + 1e-5f;
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
		"Character curves are finite, bounded, symmetric where required, and smooth modes remain continuous",
		finite && bloomOdd && spineOdd && frenzyBounded && teethBounded
			&& riptideOdd && riptideBounded && voidOdd
			&& spineMonotonic && continuous,
		"finite=" + std::to_string(finite)
			+ " bloomOdd=" + std::to_string(bloomOdd)
			+ " spineOdd=" + std::to_string(spineOdd)
			+ " riptideOdd=" + std::to_string(riptideOdd)
			+ " voidOdd=" + std::to_string(voidOdd)
			+ " teethBounded=" + std::to_string(teethBounded)
			+ " riptideBounded=" + std::to_string(riptideBounded)
			+ " spineMonotonic=" + std::to_string(spineMonotonic)
			+ " edgeDelta=" + std::to_string(std::fabs(edgeAbove - edgeBelow))
	};
}

Result swarmKernelInvariants() {
	puffy::DynamicsState dynamics;
	bool finite = true;
	bool identity = true;
	bool silent = true;
	bool odd = true;
	bool bounded = true;
	for (int ai = 0; ai <= 20; ++ai) {
		const float amount = float(ai) / 20.f;
		for (int xi = -160; xi <= 160; ++xi) {
			const float input = float(xi) / 40.f;
			for (int ci = -20; ci <= 20; ++ci) {
				const float chaos = float(ci) / 20.f;
				const float output = puffy::Engine::processCharacter(
					puffy::Character::Swarm, input, amount, dynamics, chaos);
				const float negative = puffy::Engine::processCharacter(
					puffy::Character::Swarm, -input, amount, dynamics, chaos);
				finite = finite && std::isfinite(output);
				identity = identity && (ai != 0 || output == input);
				silent = silent && (input != 0.f || output == 0.f);
				odd = odd && near(output, -negative, 2e-6f);
				bounded = bounded
					&& output >= std::min(input, -1.f) - 1e-6f
					&& output <= std::max(input, 1.f) + 1e-6f;
			}
		}
	}
	return {
		"SWARM kernel preserves identity, silence, odd symmetry, and bounds",
		finite && identity && silent && odd && bounded,
		"finite=" + std::to_string(finite)
			+ " identity=" + std::to_string(identity)
			+ " silent=" + std::to_string(silent)
			+ " odd=" + std::to_string(odd)
			+ " bounded=" + std::to_string(bounded)
	};
}

Result swarmRailTransitionIsContinuous() {
	puffy::DynamicsState dynamics;
	float maximumDelta = 0.f;
	bool continuous = true;
	constexpr float inputEpsilon = 1e-5f;
	for (int ai = 1; ai <= 20; ++ai) {
		const float amount = float(ai) / 20.f;
		const float amount2 = amount * amount;
		const float drive = 1.f + 4.f * amount2;
		const float scatter = 0.05f * amount + 0.55f * amount2;
		for (int ci = -20; ci <= 20; ++ci) {
			const float chaos = float(ci) / 20.f;
			const float localDriveScale = std::max(
				0.35f, 1.f + scatter * chaos);
			const float railInput = 1.f / (drive * localDriveScale);
			const float below = puffy::Engine::processCharacter(
				puffy::Character::Swarm,
				railInput - inputEpsilon,
				amount,
				dynamics,
				chaos);
			const float above = puffy::Engine::processCharacter(
				puffy::Character::Swarm,
				railInput + inputEpsilon,
				amount,
				dynamics,
				chaos);
			const float delta = std::fabs(above - below);
			maximumDelta = std::max(maximumDelta, delta);
			continuous = continuous && delta < 1e-3f;
		}
	}
	return {
		"SWARM rail scatter enters continuously across its moving threshold",
		continuous,
		"maximumDelta=" + std::to_string(maximumDelta)
	};
}

Result swarmChaosInfluence() {
	puffy::DynamicsState dynamics;
	auto spreadAt = [&](float amount, float input) {
		float low = std::numeric_limits<float>::max();
		float high = -std::numeric_limits<float>::max();
		for (int i = -32; i <= 32; ++i) {
			const float output = puffy::Engine::processCharacter(
				puffy::Character::Swarm, input, amount, dynamics,
				float(i) / 32.f);
			low = std::min(low, output);
			high = std::max(high, output);
		}
		return high - low;
	};
	const float lowSpread = spreadAt(0.25f, 0.45f);
	const float highSpread = spreadAt(1.f, 0.45f);
	const float silentSpread = spreadAt(1.f, 0.f);
	return {
		"SWARM chaos gains influence with PUFF without creating a noise floor",
		highSpread > lowSpread && highSpread > 0.05f && silentSpread == 0.f,
		"spread=" + std::to_string(lowSpread) + "/"
			+ std::to_string(highSpread)
			+ " silent=" + std::to_string(silentSpread)
	};
}

Result swarmRailAttraction() {
	puffy::DynamicsState dynamics;
	std::uint32_t state = 0x31415926u;
	float lowMean = 0.f;
	float mediumMean = 0.f;
	float highMean = 0.f;
	bool symmetric = true;
	constexpr int sampleCount = 4096;
	for (int i = 0; i < sampleCount; ++i) {
		state ^= state << 13;
		state ^= state >> 17;
		state ^= state << 5;
		const float chaos = float(state >> 8) * (2.f / 16777216.f) - 1.f;
		const float low = puffy::Engine::processCharacter(
			puffy::Character::Swarm, 0.45f, 0.2f, dynamics, chaos);
		const float medium = puffy::Engine::processCharacter(
			puffy::Character::Swarm, 0.45f, 0.6f, dynamics, chaos);
		const float high = puffy::Engine::processCharacter(
			puffy::Character::Swarm, 0.45f, 1.f, dynamics, chaos);
		const float negative = puffy::Engine::processCharacter(
			puffy::Character::Swarm, -0.45f, 1.f, dynamics, chaos);
		lowMean += std::fabs(low);
		mediumMean += std::fabs(medium);
		highMean += std::fabs(high);
		symmetric = symmetric && near(high, -negative, 2e-6f);
	}
	lowMean /= float(sampleCount);
	mediumMean /= float(sampleCount);
	highMean /= float(sampleCount);
	return {
		"SWARM density increasingly attracts active samples toward both rails",
		lowMean < mediumMean && mediumMean < highMean
			&& highMean - lowMean > 0.15f && symmetric,
		"mean=" + std::to_string(lowMean) + "/"
			+ std::to_string(mediumMean) + "/"
			+ std::to_string(highMean)
			+ " symmetric=" + std::to_string(symmetric)
	};
}

Result swarmCenterlineFollowsBloom() {
	puffy::DynamicsState dynamics;
	double bloomDistance = 0.0;
	double spineDistance = 0.0;
	for (int ai = 1; ai <= 20; ++ai) {
		const float amount = float(ai) / 20.f;
		for (int xi = -100; xi <= 100; ++xi) {
			const float input = float(xi) / 100.f;
			const float swarm = puffy::Engine::processCharacter(
				puffy::Character::Swarm, input, amount, dynamics, 0.f);
			const float bloom = puffy::Engine::processCharacter(
				puffy::Character::Bloom, input, amount, dynamics);
			const float spine = puffy::Engine::processCharacter(
				puffy::Character::Spine, input, amount, dynamics);
			bloomDistance += std::fabs(double(swarm - bloom));
			spineDistance += std::fabs(double(swarm - spine));
		}
	}
	return {
		"SWARM's deterministic centerline follows BLOOM more closely than SPINE",
		bloomDistance < spineDistance,
		"distance bloom=" + std::to_string(bloomDistance)
			+ " spine=" + std::to_string(spineDistance)
	};
}

Result swarmSeedingAndStereo() {
	puffy::Engine first;
	puffy::Engine second;
	puffy::Engine different;
	first.setSwarmSeed(0x12345678u);
	second.setSwarmSeed(0x12345678u);
	different.setSwarmSeed(0x87654321u);
	bool identical = true;
	bool diverged = false;
	bool stereo = true;
	for (int i = 0; i < 2000; ++i) {
		const float input = 2.25f * std::sin(2.f * kPi * 431.f * float(i) / 48000.f);
		const puffy::Frame a = first.process(
			input, input, 0.8f, int(puffy::Character::Swarm), false, 0.f);
		const puffy::Frame b = second.process(
			input, input, 0.8f, int(puffy::Character::Swarm), false, 0.f);
		const puffy::Frame c = different.process(
			input, input, 0.8f, int(puffy::Character::Swarm), false, 0.f);
		identical = identical && a.left == b.left && a.right == b.right;
		diverged = diverged || std::fabs(a.left - c.left) > 1e-6f;
		stereo = stereo && std::fabs(a.left - a.right) <= 1e-6f;
	}
	first.setSwarmSeed(0x12345678u);
	second.setSwarmSeed(0x12345678u);
	bool replayed = true;
	for (int i = 0; i < 1000; ++i) {
		const float input = 1.75f * std::sin(2.f * kPi * 673.f * float(i) / 48000.f);
		const puffy::Frame a = first.process(
			input, input, 0.7f, int(puffy::Character::Swarm), false, 0.f);
		const puffy::Frame b = second.process(
			input, input, 0.7f, int(puffy::Character::Swarm), false, 0.f);
		replayed = replayed && a.left == b.left && a.right == b.right;
	}
	first.reset();
	second.reset();
	for (int i = 0; i < 1000; ++i) {
		const float input = 1.25f * std::sin(2.f * kPi * 739.f * float(i) / 48000.f);
		const puffy::Frame a = first.process(
			input, input, 0.6f, int(puffy::Character::Swarm), false, 0.f);
		const puffy::Frame b = second.process(
			input, input, 0.6f, int(puffy::Character::Swarm), false, 0.f);
		replayed = replayed && a.left == b.left && a.right == b.right;
	}
	return {
		"SWARM seeding is deterministic and shared stereo remains identical",
		identical && diverged && stereo && replayed,
		"identical=" + std::to_string(identical)
			+ " diverged=" + std::to_string(diverged)
			+ " stereo=" + std::to_string(stereo)
			+ " replayed=" + std::to_string(replayed)
	};
}

Result voidBecomesSteppedQuantizer() {
	puffy::DynamicsState dynamics;
	const float unity = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.23f, 0.f, dynamics);
	const float deadZone = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.049f, 1.f, dynamics);
	const float firstTreadStart = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.051f, 1.f, dynamics);
	const float firstTreadEnd = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.099f, 1.f, dynamics);
	const float secondTread = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.101f, 1.f, dynamics);
	const float halfPuff = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.37f, 0.5f, dynamics);
	const float halfPuffTreadStart = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.365f, 0.5f, dynamics);
	const float halfPuffTreadEnd = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.369f, 0.5f, dynamics);
	const float rail = puffy::Engine::processCharacter(
		puffy::Character::Void, 1.f, 1.f, dynamics);
	const float riptideRateRail = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.40f, 1.f, dynamics);
	const float beforeRiptideRateRail = puffy::Engine::processCharacter(
		puffy::Character::Void, 0.40f - 1e-6f, 1.f, dynamics);
	bool railTracksRiptide = true;
	for (int amountIndex = 1; amountIndex <= 4; ++amountIndex) {
		const float amount = 0.25f * float(amountIndex);
		const float amountSq = amount * amount;
		const float threshold = 1.f / (1.f + 1.5f * amountSq);
		const float stepMix =
			1.f - (1.f - amount) * (1.f - amount) * (1.f - amount);
		const float expectedRail = threshold + (1.f - threshold) * stepMix;
		const float atRail = puffy::Engine::processCharacter(
			puffy::Character::Void, threshold, amount, dynamics);
		const float beforeRail = puffy::Engine::processCharacter(
			puffy::Character::Void,
			threshold - 0.0625f * amountSq * threshold,
			amount,
			dynamics);
		railTracksRiptide = railTracksRiptide
			&& near(atRail, expectedRail, 1e-6f)
			&& beforeRail < atRail;
	}
	const float negativeSecondTread = puffy::Engine::processCharacter(
		puffy::Character::Void, -0.101f, 1.f, dynamics);
	return {
		"VOID widens its quantizer treads and moves its rail inward at RIPTIDE's rate",
		near(unity, 0.23f, 1e-7f)
			&& near(deadZone, 0.f, 1e-7f)
			&& near(firstTreadStart, 0.125f, 1e-6f)
			&& near(firstTreadEnd, 0.125f, 1e-6f)
			&& near(secondTread, 0.25f, 1e-6f)
			&& near(halfPuff, 0.48375f, 1e-6f)
			&& near(halfPuffTreadEnd - halfPuffTreadStart, 0.0005f, 1e-6f)
			&& near(rail, 1.f, 1e-7f)
			&& near(riptideRateRail, 1.f, 1e-7f)
			&& beforeRiptideRateRail < 1.f
			&& railTracksRiptide
			&& near(secondTread, -negativeSecondTread, 1e-7f),
		"unity=" + std::to_string(unity)
			+ " deadZone=" + std::to_string(deadZone)
			+ " tread1=" + std::to_string(firstTreadStart)
			+ "/" + std::to_string(firstTreadEnd)
			+ " tread2=" + std::to_string(secondTread)
			+ " half=" + std::to_string(halfPuff)
			+ " halfTreadSlope="
			+ std::to_string(halfPuffTreadEnd - halfPuffTreadStart)
			+ " rail=" + std::to_string(beforeRiptideRateRail)
			+ "/" + std::to_string(riptideRateRail)
	};
}

Result frenzySinusoidalFold() {
	struct FoldMeasure {
		int turningPoints = 0;
		float positivePeak = 0.f;
		float negativePeak = 0.f;
		int railSamples = 0;
	};
	const auto measure = [](float amount, const puffy::DynamicsState& dynamics) {
		FoldMeasure result;
		float previous = puffy::Engine::processCharacter(
			puffy::Character::Frenzy, -1.f, amount, dynamics);
		float previousSlope = 0.f;
		result.negativePeak = std::max(result.negativePeak, -previous);
		for (int i = 1; i <= 800; ++i) {
			const float input = -1.f + 2.f * float(i) / 800.f;
			const float output = puffy::Engine::processCharacter(
				puffy::Character::Frenzy, input, amount, dynamics);
			result.positivePeak = std::max(result.positivePeak, output);
			result.negativePeak = std::max(result.negativePeak, -output);
			if (i < 800 && std::fabs(output) >= 0.9999f) {
				result.railSamples++;
			}
			const float slope = output - previous;
			if (std::fabs(slope) > 1e-7f) {
				if (previousSlope * slope < 0.f) {
					result.turningPoints++;
				}
				previousSlope = slope;
			}
			previous = output;
		}
		return result;
	};

	puffy::DynamicsState idle;
	puffy::DynamicsState active;
	active.fast = 1.f;
	active.transient = 1.f;
	const auto lowPuff = measure(0.5f, active);
	const auto highPuff = measure(1.f, active);
	const auto highPuffIdle = measure(1.f, idle);
	const float idleZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, idle);
	const float activeZero = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.f, 1.f, active);
	const float activePositive = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 0.8f, 1.f, active);
	const float activeNegative = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, -0.8f, 1.f, active);
	const float negativeEdge = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, -1.f, 1.f, active);
	const float positiveEdge = puffy::Engine::processCharacter(
		puffy::Character::Frenzy, 1.f, 1.f, active);
	double positiveMean = 0.0;
	double negativeMean = 0.0;
	int clampSamples = 0;
	for (int i = 1; i <= 400; ++i) {
		const float input = float(i) / 400.f;
		positiveMean += puffy::Engine::processCharacter(
			puffy::Character::Frenzy, input, 1.f, active);
		negativeMean += puffy::Engine::processCharacter(
			puffy::Character::Frenzy, -input, 1.f, active);
	}
	positiveMean /= 400.0;
	negativeMean /= 400.0;
	for (int amountIndex = 1; amountIndex <= 20; ++amountIndex) {
		const float amount = float(amountIndex) / 20.f;
		for (int dynamicsIndex = 0; dynamicsIndex < 4; ++dynamicsIndex) {
			puffy::DynamicsState sweepDynamics;
			sweepDynamics.fast = (dynamicsIndex & 1) ? 1.f : 0.f;
			sweepDynamics.transient = (dynamicsIndex & 2) ? 1.f : 0.f;
			for (int inputIndex = -999; inputIndex <= 999; ++inputIndex) {
				const float input = float(inputIndex) / 1000.f;
				const float output = puffy::Engine::processCharacter(
					puffy::Character::Frenzy, input, amount, sweepDynamics);
				const float clampedTarget = std::copysign(1.f, input);
				const float clampedOutput = input + (clampedTarget - input) * amount;
				if (output == clampedOutput) {
					clampSamples++;
				}
			}
		}
	}
	return {
		"FRENZY bias slopes let lobe extrema kiss the rails without saturating",
		lowPuff.turningPoints >= 4
			&& highPuff.turningPoints >= lowPuff.turningPoints + 2
			&& highPuff.positivePeak > 0.997f && highPuff.positivePeak < 0.9995f
			&& highPuff.negativePeak > 0.997f && highPuff.negativePeak < 0.9995f
			&& highPuffIdle.positivePeak > 0.997f && highPuffIdle.positivePeak < 0.9995f
			&& highPuffIdle.negativePeak > 0.997f && highPuffIdle.negativePeak < 0.9995f
			&& highPuff.railSamples == 0 && highPuffIdle.railSamples == 0
			&& clampSamples == 0
			&& near(idleZero, 0.f, 1e-7f)
			&& near(activeZero, 0.f, 1e-7f)
			&& near(negativeEdge, -0.75f, 1e-5f)
			&& near(positiveEdge, 0.75f, 1e-5f)
			&& positiveMean > 0.25
			&& negativeMean < -0.20
			&& std::fabs(activePositive + activeNegative) > 0.05f,
		"turns=" + std::to_string(lowPuff.turningPoints)
			+ "/" + std::to_string(highPuff.turningPoints)
			+ " activePeaks=" + std::to_string(highPuff.negativePeak)
			+ "/" + std::to_string(highPuff.positivePeak)
			+ " idlePeaks=" + std::to_string(highPuffIdle.negativePeak)
			+ "/" + std::to_string(highPuffIdle.positivePeak)
			+ " railSamples=" + std::to_string(highPuff.railSamples)
			+ "/" + std::to_string(highPuffIdle.railSamples)
			+ " clampSamples=" + std::to_string(clampSamples)
			+ " activePair=" + std::to_string(activeNegative)
			+ "/" + std::to_string(activePositive)
			+ " edges=" + std::to_string(negativeEdge)
			+ "/" + std::to_string(positiveEdge)
			+ " means=" + std::to_string(negativeMean)
			+ "/" + std::to_string(positiveMean)
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
	const float firstRailNotch = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.45f, 1.f, dynamics);
	const float firstRailReturn = puffy::Engine::processCharacter(
		puffy::Character::Riptide, 0.50f, 1.f, dynamics);
	const float negativeRailNotch = puffy::Engine::processCharacter(
		puffy::Character::Riptide, -0.45f, 1.f, dynamics);
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
			&& near(firstRailNotch, 0.94f, 1e-6f)
			&& near(firstRailReturn, 1.f, 1e-6f)
			&& near(negativeRailNotch, -firstRailNotch, 1e-6f)
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
			+ " teeth=" + std::to_string(firstRailNotch)
			+ "/" + std::to_string(firstRailReturn)
			+ " reversals=" + std::to_string(slopeReversals)
	};
}

struct ToneStats {
	double inputSq = 0.0;
	double outputSq = 0.0;
	double errorSq = 0.0;
	float peakLeft = 0.f;
	float peakRight = 0.f;
	float peakPositiveActivity = 0.f;
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
	float sensitivity,
	bool identicalStereo = true,
	float wetMix = 1.f,
	float settleSeconds = 0.1f) {
	const float sampleRate = engine.getSampleRate();
	const int samples = int(seconds * sampleRate);
	const int skip = int(settleSeconds * sampleRate);
	ToneStats stats;
	for (int i = 0; i < samples; ++i) {
		const float phase = 2.f * kPi * frequency * float(i) / sampleRate;
		const float left = amplitude * std::sin(phase);
		const float right = identicalStereo ? left : amplitude * 0.5f * std::cos(phase);
		const puffy::Frame frame = engine.process(
			left, right, amount, character, autoDeflate, sensitivity, wetMix);
		stats.finite = stats.finite && std::isfinite(frame.left)
			&& std::isfinite(frame.right);
		stats.peakLeft = std::max(stats.peakLeft, std::fabs(frame.left));
		stats.peakRight = std::max(stats.peakRight, std::fabs(frame.right));
		stats.peakPositiveActivity = std::max(
			stats.peakPositiveActivity, frame.positiveInputActivity);
		stats.maxStereoDelta =
			std::max(stats.maxStereoDelta, std::fabs(frame.left - frame.right));
		if (i >= skip) {
			stats.inputSq += double(left) * double(left);
			stats.outputSq += double(frame.left) * double(frame.left);
			const double error = double(frame.left) - double(left);
			stats.errorSq += error * error;
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

Result dynamicAutoDeflateTracksProgramEnergy() {
	puffy::Engine dryReference;
	puffy::Engine adaptive;
	puffy::Engine voidReference;
	puffy::Engine voidAdaptive;
	puffy::Engine partialAdaptive;
	for (puffy::Engine* engine : {
		&dryReference, &adaptive, &voidReference, &voidAdaptive,
		&partialAdaptive}) {
		engine->setSampleRate(48000.f);
	}
	const ToneStats spineWithoutAuto = renderTone(
		dryReference, 4.f, 1.f, 431.f, 1.f,
		int(puffy::Character::Spine), false, 0.f, true, 1.f, 2.f);
	const ToneStats spineWithAuto = renderTone(
		adaptive, 4.f, 1.f, 431.f, 1.f,
		int(puffy::Character::Spine), true, 0.f, true, 1.f, 2.f);
	const ToneStats voidWithoutAuto = renderTone(
		voidReference, 4.f, 1.f, 431.f, 1.f,
		int(puffy::Character::Void), false, 0.f, true, 1.f, 2.f);
	const ToneStats voidWithAuto = renderTone(
		voidAdaptive, 4.f, 1.f, 431.f, 1.f,
		int(puffy::Character::Void), true, 0.f, true, 1.f, 2.f);
	const ToneStats partialWithAuto = renderTone(
		partialAdaptive, 4.f, 1.f, 431.f, 1.f,
		int(puffy::Character::Spine), true, 0.f, true, 0.5f, 2.f);
	const float spineRawRatio = float(std::sqrt(
		spineWithoutAuto.outputSq / spineWithoutAuto.inputSq));
	const float spineAutoRatio = float(std::sqrt(
		spineWithAuto.outputSq / spineWithAuto.inputSq));
	const float voidRawRatio = float(std::sqrt(
		voidWithoutAuto.outputSq / voidWithoutAuto.inputSq));
	const float voidAutoRatio = float(std::sqrt(
		voidWithAuto.outputSq / voidWithAuto.inputSq));
	const float partialAutoRatio = float(std::sqrt(
		partialWithAuto.outputSq / partialWithAuto.inputSq));
	return {
		"AUTO dynamically matches program energy with downward-only linked gain",
		spineWithoutAuto.finite && spineWithAuto.finite
			&& voidWithoutAuto.finite && voidWithAuto.finite
			&& spineRawRatio > 1.2f
			&& spineAutoRatio < spineRawRatio - 0.15f
			&& spineAutoRatio > 0.95f && spineAutoRatio < 1.10f
			&& voidAutoRatio <= voidRawRatio + 1e-5f
			&& partialAutoRatio > 0.95f && partialAutoRatio < 1.10f
			&& spineWithAuto.maxStereoDelta <= 1e-6f,
		"spine=" + std::to_string(spineRawRatio)
			+ "/" + std::to_string(spineAutoRatio)
			+ " void=" + std::to_string(voidRawRatio)
			+ "/" + std::to_string(voidAutoRatio)
			+ " partial=" + std::to_string(partialAutoRatio)
			+ " stereoDelta="
			+ std::to_string(spineWithAuto.maxStereoDelta)
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

Result limiterModes() {
	puffy::Engine soft;
	puffy::Engine off;
	soft.setSampleRate(48000.f);
	off.setSampleRate(48000.f);
	soft.setLimiterMode(puffy::LimiterMode::Soft);
	off.setLimiterMode(puffy::LimiterMode::Off);
	float softPeak = 0.f;
	float offPeak = 0.f;
	float softRatioError = 0.f;
	for (int i = 0; i < 48000; ++i) {
		const float phase = 2.f * kPi * 997.f * float(i) / 48000.f;
		const float left = 8.f * std::sin(phase);
		const float right = 2.f * std::sin(phase);
		const puffy::Frame softFrame = soft.process(
			left, right, 0.f, 0, false, 0.f);
		const puffy::Frame offFrame = off.process(
			left, right, 0.f, 0, false, 0.f);
		softPeak = std::max(softPeak, std::fabs(softFrame.left));
		offPeak = std::max(offPeak, std::fabs(offFrame.left));
		if (i > 1000 && std::fabs(softFrame.left) > 1e-4f) {
			softRatioError = std::max(
				softRatioError,
				std::fabs(softFrame.right / softFrame.left - 0.25f));
		}
	}
	return {
		"SOFT stays below 5 V with linked gain while OFF permits overrange",
		softPeak > 4.f && softPeak <= 5.00001f
			&& offPeak > 7.f && softRatioError < 2e-4f,
		"softPeak=" + std::to_string(softPeak)
			+ " offPeak=" + std::to_string(offPeak)
			+ " ratioError=" + std::to_string(softRatioError)
	};
}

Result sensitivityChangesInputProjectionAndLevel() {
	puffy::Engine low;
	puffy::Engine center;
	puffy::Engine high;
	puffy::Engine lowUnity;
	puffy::Engine highUnity;
	low.setSampleRate(48000.f);
	center.setSampleRate(48000.f);
	high.setSampleRate(48000.f);
	lowUnity.setSampleRate(48000.f);
	highUnity.setSampleRate(48000.f);
	const ToneStats lowStats = renderTone(
		low, 0.5f, 2.f, 431.f, 1.f, int(puffy::Character::Bloom),
		false, -1.f);
	const ToneStats centerStats = renderTone(
		center, 0.5f, 2.f, 431.f, 1.f, int(puffy::Character::Bloom),
		false, 0.f);
	const ToneStats highStats = renderTone(
		high, 0.5f, 2.f, 431.f, 1.f, int(puffy::Character::Bloom),
		false, 1.f);
	const ToneStats lowUnityStats = renderTone(
		lowUnity, 0.5f, 1.f, 431.f, 0.f, int(puffy::Character::Bloom),
		false, -1.f);
	const ToneStats highUnityStats = renderTone(
		highUnity, 0.5f, 1.f, 431.f, 0.f, int(puffy::Character::Bloom),
		false, 1.f);
	const float lowError = float(std::sqrt(lowStats.errorSq / lowStats.inputSq));
	const float centerError = float(
		std::sqrt(centerStats.errorSq / centerStats.inputSq));
	const float highError = float(std::sqrt(highStats.errorSq / highStats.inputSq));
	const float lowCleanRatio = float(
		std::sqrt(lowUnityStats.outputSq / lowUnityStats.inputSq));
	const float highCleanRatio = float(
		std::sqrt(highUnityStats.outputSq / highUnityStats.inputSq));
	return {
		"SENSITIVITY changes waveshaper projection and pre-shaper level",
		lowStats.finite && centerStats.finite && highStats.finite
			&& lowStats.peakPositiveActivity < centerStats.peakPositiveActivity
			&& centerStats.peakPositiveActivity < highStats.peakPositiveActivity
			&& near(
				2.f * lowStats.peakPositiveActivity,
				centerStats.peakPositiveActivity,
				0.002f)
			&& near(
				2.f * centerStats.peakPositiveActivity,
				highStats.peakPositiveActivity,
				0.002f)
			&& highError != centerError && centerError != lowError
			&& near(lowCleanRatio, 0.5f, 0.002f)
			&& near(highCleanRatio, 2.f, 0.002f),
		"projection=" + std::to_string(lowStats.peakPositiveActivity)
			+ "/" + std::to_string(centerStats.peakPositiveActivity)
			+ "/" + std::to_string(highStats.peakPositiveActivity)
			+ " error=" + std::to_string(lowError)
			+ "/" + std::to_string(centerError)
			+ "/" + std::to_string(highError)
			+ " cleanGain=" + std::to_string(lowCleanRatio)
			+ "/" + std::to_string(highCleanRatio)
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
		frame = engine.process(2.5f, -4.f, 0.f, 0, false, 0.f, 1.f, true);
	}
	return {
		"Preview activity tracks combined and per-channel polarity excursions",
		near(frame.positiveInputActivity, 0.5f, 2e-3f)
			&& near(frame.negativeInputActivity, 0.8f, 2e-3f)
			&& near(frame.leftPositiveInputActivity, 0.5f, 2e-3f)
			&& near(frame.leftNegativeInputActivity, 0.f, 2e-3f)
			&& near(frame.rightPositiveInputActivity, 0.f, 2e-3f)
			&& near(frame.rightNegativeInputActivity, 0.8f, 2e-3f),
		"positive=" + std::to_string(frame.positiveInputActivity)
			+ " negative=" + std::to_string(frame.negativeInputActivity)
			+ " left=" + std::to_string(frame.leftNegativeInputActivity)
			+ "/" + std::to_string(frame.leftPositiveInputActivity)
			+ " right=" + std::to_string(frame.rightNegativeInputActivity)
			+ "/" + std::to_string(frame.rightPositiveInputActivity)
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
		swarmKernelInvariants(),
		swarmRailTransitionIsContinuous(),
		swarmChaosInfluence(),
		swarmRailAttraction(),
		swarmCenterlineFollowsBloom(),
		swarmSeedingAndStereo(),
		voidBecomesSteppedQuantizer(),
		frenzySinusoidalFold(),
		riptideFractalAnchors(),
		unityAndStereo(),
		dynamicAutoDeflateTracksProgramEnergy(),
		linkedLimiter(),
		limiterModes(),
		sensitivityChangesInputProjectionAndLevel(),
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
