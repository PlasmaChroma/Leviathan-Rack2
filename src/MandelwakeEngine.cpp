#include "MandelwakeEngine.hpp"

#include <algorithm>
#include <cstdint>

namespace mandelwake {
namespace {

constexpr OrbitQ28 kCenterXMinQ28 = static_cast<OrbitQ28>(-(std::int64_t {9} << 26));
constexpr OrbitQ28 kCenterXMaxQ28 = static_cast<OrbitQ28>(std::int64_t {1} << 28);
constexpr OrbitQ28 kCenterYMinQ28 = static_cast<OrbitQ28>(-(std::int64_t {3} << 27));
constexpr OrbitQ28 kCenterYMaxQ28 = static_cast<OrbitQ28>(std::int64_t {3} << 27);
constexpr OrbitQ28 kMutationDepthMaxQ28 = static_cast<OrbitQ28>(std::int64_t {1} << 26);

std::uint32_t mapValue(Map map) {
	return static_cast<std::uint32_t>(map);
}

Map sanitizeMap(Map map) {
	return mapValue(map) <= mapValue(Map::BurningShip) ? map : Map::Mandelbrot;
}

OrbitQ28 clampOrbitCandidate(std::int64_t candidate) {
	return static_cast<OrbitQ28>(clamp64(candidate, -kComponentSafetyQ28, kComponentSafetyQ28));
}

} // namespace

Engine::Engine(std::uint64_t seed)
	: baseSeed_(seed) {
	resetAll();
}

int Engine::clampChannel(int channel) {
	return std::max(0, std::min(channel, kMaxChannels - 1));
}

const ChannelState& Engine::channel(int index) const {
	return channels_[static_cast<std::size_t>(clampChannel(index))];
}

bool Engine::setMap(Map newMap) {
	newMap = sanitizeMap(newMap);
	if (map_ == newMap) return false;
	map_ = newMap;
	++mapSerial_;
	resetAll();
	return true;
}

std::uint64_t Engine::reseed() {
	return reseed(map_);
}

std::uint64_t Engine::reseed(Map newMap) {
	newMap = sanitizeMap(newMap);
	if (map_ != newMap) {
		map_ = newMap;
		++mapSerial_;
	}
	const std::uint64_t candidate = mix64(baseSeed_ ^ kDomainReseed);
	baseSeed_ = candidate != baseSeed_ ? candidate : candidate ^ kMixIncrement;
	++reseedSerial_;
	resetAll();
	return baseSeed_;
}

void Engine::resetAll() {
	for (int channelIndex = 0; channelIndex < kMaxChannels; ++channelIndex) {
		resetChannel(channelIndex);
	}
}

void Engine::resetChannel(int channelIndex) {
	channelIndex = clampChannel(channelIndex);
	ChannelState& state = channels_[static_cast<std::size_t>(channelIndex)];
	const std::uint32_t nextResetSerial = state.resetSerial + 1u;
	const std::uint32_t retainedEscapeSerial = state.escapeSerial;
	state = ChannelState {};
	state.resetSerial = nextResetSerial;
	state.escapeSerial = retainedEscapeSerial;

	if (map_ == Map::Julia) {
		const std::uint64_t hashX = orbitHash(
			baseSeed_, kDomainJuliaX, static_cast<std::uint32_t>(channelIndex),
			mapValue(Map::Julia), 0u, 0u);
		const std::uint64_t hashY = orbitHash(
			baseSeed_, kDomainJuliaY, static_cast<std::uint32_t>(channelIndex),
			mapValue(Map::Julia), 0u, 0u);
		state.xQ28 = signedUnitQ30ToQ28(signedUnitQ30(hashX));
		state.yQ28 = signedUnitQ30ToQ28(signedUnitQ30(hashY));
	}

	appendHistory(state, state.xQ28, state.yQ28);
}

void Engine::appendHistory(ChannelState& state, OrbitQ28 xQ28, OrbitQ28 yQ28) {
	state.history[static_cast<std::size_t>(state.historyWriteIndex)] = HistoryPoint(xQ28, yQ28);
	state.historyWriteIndex = static_cast<std::uint16_t>(
		(state.historyWriteIndex + 1u) % kHistoryCapacity);
	if (state.historyCount < kHistoryCapacity) ++state.historyCount;
}

HistoryPoint Engine::historyPointOldestFirst(int channelIndex, int index) const {
	const ChannelState& state = channel(channelIndex);
	if (state.historyCount == 0) return HistoryPoint();
	index = std::max(0, std::min(index, static_cast<int>(state.historyCount) - 1));
	const int oldest = (static_cast<int>(state.historyWriteIndex)
		- static_cast<int>(state.historyCount) + kHistoryCapacity) % kHistoryCapacity;
	const int storageIndex = (oldest + index) % kHistoryCapacity;
	return state.history[static_cast<std::size_t>(storageIndex)];
}

StepOutputs Engine::step(int channelIndex, const StepInputs& rawInputs) {
	channelIndex = clampChannel(channelIndex);
	ChannelState& state = channels_[static_cast<std::size_t>(channelIndex)];
	const std::uint32_t channelValue = static_cast<std::uint32_t>(channelIndex);
	const std::uint32_t currentMap = mapValue(map_);
	const std::uint64_t currentStep = state.stepIndex;

	const OrbitQ28 cXQ28 = static_cast<OrbitQ28>(clamp64(
		rawInputs.cXQ28, kCenterXMinQ28, kCenterXMaxQ28));
	const OrbitQ28 cYQ28 = static_cast<OrbitQ28>(clamp64(
		rawInputs.cYQ28, kCenterYMinQ28, kCenterYMaxQ28));
	const OrbitQ28 mutationDepthQ28 = static_cast<OrbitQ28>(clamp64(
		rawInputs.mutationDepthQ28, 0, kMutationDepthMaxQ28));
	const int iterations = std::max(1, std::min(static_cast<int>(rawInputs.iterations), 32));

	bool escaped = false;
	for (int micro = 0; micro < iterations; ++micro) {
		std::int64_t x = clamp64(state.xQ28, -kComponentSafetyQ28, kComponentSafetyQ28);
		std::int64_t y = clamp64(state.yQ28, -kComponentSafetyQ28, kComponentSafetyQ28);
		if (map_ == Map::BurningShip) {
			if (x < 0) x = -x;
			if (y < 0) y = -y;
		}

		const std::int64_t xSquaredQ28 = divideRoundHalfAway(x * x, kScaleQ28);
		const std::int64_t ySquaredQ28 = divideRoundHalfAway(y * y, kScaleQ28);
		const std::int64_t xyDoubledQ28 = divideRoundHalfAway(2 * x * y, kScaleQ28);

		const std::uint64_t mutationHashX = orbitHash(
			baseSeed_, kDomainMutationX, channelValue, currentMap,
			currentStep, static_cast<std::uint32_t>(micro));
		const std::uint64_t mutationHashY = orbitHash(
			baseSeed_, kDomainMutationY, channelValue, currentMap,
			currentStep, static_cast<std::uint32_t>(micro));
		const std::int64_t mutationXQ28 = divideRoundHalfAway(
			std::int64_t {mutationDepthQ28} * signedUnitQ30(mutationHashX), kOneQ30);
		const std::int64_t mutationYQ28 = divideRoundHalfAway(
			std::int64_t {mutationDepthQ28} * signedUnitQ30(mutationHashY), kOneQ30);

		const OrbitQ28 candidateX = clampOrbitCandidate(
			xSquaredQ28 - ySquaredQ28 + cXQ28 + mutationXQ28);
		const OrbitQ28 candidateY = clampOrbitCandidate(
			xyDoubledQ28 + cYQ28 + mutationYQ28);
		const std::int64_t candidateX64 = candidateX;
		const std::int64_t candidateY64 = candidateY;
		const std::uint64_t candidateRadiusSquared = static_cast<std::uint64_t>(
			candidateX64 * candidateX64 + candidateY64 * candidateY64);

		if (candidateRadiusSquared > kEscapeRadiusSquaredQ56) {
			state.lastPreEscape = HistoryPoint(state.xQ28, state.yQ28);
			state.xQ28 = reentryQ28(orbitHash(
				baseSeed_, kDomainReentryX, channelValue, currentMap,
				currentStep, static_cast<std::uint32_t>(micro)));
			state.yQ28 = reentryQ28(orbitHash(
				baseSeed_, kDomainReentryY, channelValue, currentMap,
				currentStep, static_cast<std::uint32_t>(micro)));
			state.lastReentry = HistoryPoint(state.xQ28, state.yQ28);
			++state.escapeSerial;
			escaped = true;
			break;
		}

		state.xQ28 = candidateX;
		state.yQ28 = candidateY;
	}

	const std::uint32_t finalRadiusQ28 = radiusQ28(state.xQ28, state.yQ28);
	const std::int32_t finalPhaseQ30 = phaseQ30(state.xQ28, state.yQ28);
	const std::uint32_t radiusNormalizedQ16 = static_cast<std::uint32_t>(clamp64(
		divideRoundHalfAway(finalRadiusQ28, std::int64_t {1} << 13), 0, 65536));
	const std::uint32_t edgeQ16 = 16384u + static_cast<std::uint32_t>(
		divideRoundHalfAway(std::int64_t {3} * radiusNormalizedQ16, 4));
	const std::uint32_t densityQ16 = std::min(rawInputs.densityQ16, 65536u);
	const std::uint32_t probabilityQ16 = static_cast<std::uint32_t>(divideRoundHalfAway(
		static_cast<std::int64_t>(densityQ16) * edgeQ16, 65536));
	const std::uint16_t trial = trialQ16(orbitHash(
		baseSeed_, kDomainGate, channelValue, currentMap, currentStep, 0u));
	const bool gate = static_cast<std::uint32_t>(trial) < probabilityQ16;

	appendHistory(state, state.xQ28, state.yQ28);
	++state.stepIndex;

	StepOutputs outputs;
	outputs.xQ28 = state.xQ28;
	outputs.yQ28 = state.yQ28;
	outputs.radiusQ28 = finalRadiusQ28;
	outputs.phaseQ30 = finalPhaseQ30;
	outputs.stepIndex = currentStep;
	outputs.gate = gate;
	outputs.escaped = escaped;
	return outputs;
}

} // namespace mandelwake
