#pragma once

#include "MandelwakeTables.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace mandelwake {

using OrbitQ28 = std::int32_t;

constexpr int kFractionBits = 28;
constexpr std::int64_t kScaleQ28 = std::int64_t {1} << kFractionBits;
constexpr std::int64_t kComponentSafetyQ28 = std::int64_t {4} << kFractionBits;
constexpr std::uint64_t kEscapeRadiusSquaredQ56 = std::uint64_t {1} << 58;
constexpr std::int64_t kOneQ30 = std::int64_t {1} << 30;

constexpr std::uint64_t kMixIncrement = UINT64_C(0x9E3779B97F4A7C15);
constexpr std::uint64_t kMixMultiplier1 = UINT64_C(0xBF58476D1CE4E5B9);
constexpr std::uint64_t kMixMultiplier2 = UINT64_C(0x94D049BB133111EB);

constexpr std::uint64_t kDomainViewportX = UINT64_C(0x4D575F5649455758);
constexpr std::uint64_t kDomainViewportY = UINT64_C(0x4D575F5649455759);
constexpr std::uint64_t kDomainJuliaX = UINT64_C(0x4D575F4A554C4958);
constexpr std::uint64_t kDomainJuliaY = UINT64_C(0x4D575F4A554C4959);
constexpr std::uint64_t kDomainMutationX = UINT64_C(0x4D55544154455F58);
constexpr std::uint64_t kDomainMutationY = UINT64_C(0x4D55544154455F59);
constexpr std::uint64_t kDomainReentryX = UINT64_C(0x5245454E54525F58);
constexpr std::uint64_t kDomainReentryY = UINT64_C(0x5245454E54525F59);
constexpr std::uint64_t kDomainGate = UINT64_C(0x4D575F474154455F);
constexpr std::uint64_t kDomainReseed = UINT64_C(0x4D575F5245534544);

inline constexpr std::int64_t clamp64(
	std::int64_t value, std::int64_t low, std::int64_t high) {
	return value < low ? low : (value > high ? high : value);
}

inline std::uint64_t unsignedMagnitude(std::int64_t value) {
	if (value >= 0) return static_cast<std::uint64_t>(value);
	return static_cast<std::uint64_t>(-(value + 1)) + 1u;
}

inline std::int64_t divideRoundHalfAway(std::int64_t numerator, std::int64_t positiveDivisor) {
	if (positiveDivisor <= 0) return 0;
	const bool negative = numerator < 0;
	const std::uint64_t magnitude = unsignedMagnitude(numerator);
	const std::uint64_t divisor = static_cast<std::uint64_t>(positiveDivisor);
	std::uint64_t quotient = magnitude / divisor;
	const std::uint64_t remainder = magnitude % divisor;
	if (remainder * 2u >= divisor) ++quotient;
	if (!negative) {
		if (quotient > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max())) {
			return std::numeric_limits<std::int64_t>::max();
		}
		return static_cast<std::int64_t>(quotient);
	}
	const std::uint64_t minMagnitude = std::uint64_t {1} << 63;
	if (quotient >= minMagnitude) return std::numeric_limits<std::int64_t>::min();
	return -static_cast<std::int64_t>(quotient);
}

inline std::int64_t multiplyQ28(std::int64_t aQ28, std::int64_t bQ28) {
	return divideRoundHalfAway(aQ28 * bQ28, kScaleQ28);
}

inline std::uint64_t mix64(std::uint64_t value) {
	value += kMixIncrement;
	value = (value ^ (value >> 30)) * kMixMultiplier1;
	value = (value ^ (value >> 27)) * kMixMultiplier2;
	return value ^ (value >> 31);
}

inline std::uint64_t orbitHash(
	std::uint64_t baseSeed,
	std::uint64_t domainTag,
	std::uint32_t channel,
	std::uint32_t map,
	std::uint64_t step,
	std::uint32_t microIteration) {
	std::uint64_t hash = mix64(baseSeed ^ domainTag);
	hash = mix64(hash ^ std::uint64_t {channel});
	hash = mix64(hash ^ std::uint64_t {map});
	hash = mix64(hash ^ step);
	return mix64(hash ^ std::uint64_t {microIteration});
}

inline std::int64_t signedUnitQ30(std::uint64_t hash) {
	return static_cast<std::int64_t>(hash >> 33) - kOneQ30;
}

inline std::uint16_t trialQ16(std::uint64_t hash) {
	return static_cast<std::uint16_t>(hash >> 48);
}

inline OrbitQ28 reentryQ28(std::uint64_t hash) {
	return static_cast<OrbitQ28>(static_cast<std::int64_t>(hash >> 38) - (std::int64_t {1} << 25));
}

inline OrbitQ28 signedUnitQ30ToQ28(std::int64_t valueQ30) {
	return static_cast<OrbitQ28>(divideRoundHalfAway(valueQ30, 4));
}

inline std::int64_t safeAbsQ28(OrbitQ28 value) {
	const std::int64_t widened = value;
	return widened < 0 ? -widened : widened;
}

inline std::uint32_t integerSqrt64(std::uint64_t value) {
	std::uint64_t remainder = 0;
	std::uint64_t root = 0;
	for (int group = 0; group < 32; ++group) {
		root <<= 1;
		remainder = (remainder << 2) | (value >> 62);
		value <<= 2;
		const std::uint64_t candidate = (root << 1) | 1u;
		if (remainder >= candidate) {
			remainder -= candidate;
			++root;
		}
	}
	return static_cast<std::uint32_t>(root);
}

inline std::uint32_t radiusQ28(OrbitQ28 xQ28, OrbitQ28 yQ28) {
	const std::int64_t x = xQ28;
	const std::int64_t y = yQ28;
	return integerSqrt64(static_cast<std::uint64_t>(x * x + y * y));
}

inline std::int32_t phaseQ30(OrbitQ28 xQ28, OrbitQ28 yQ28) {
	if (xQ28 == 0 && yQ28 == 0) return 0;
	const std::uint64_t absX = static_cast<std::uint64_t>(safeAbsQ28(xQ28));
	const std::uint64_t absY = static_cast<std::uint64_t>(safeAbsQ28(yQ28));
	const std::uint64_t smaller = std::min(absX, absY);
	const std::uint64_t larger = std::max(absX, absY);
	const std::size_t index = static_cast<std::size_t>((smaller << 12) / larger);
	const std::int64_t angle = kPhaseAtanQ30[index];
	const std::int64_t base = absX >= absY ? angle : (kOneQ30 / 2 - angle);
	if (xQ28 >= 0 && yQ28 >= 0) return static_cast<std::int32_t>(base);
	if (xQ28 < 0 && yQ28 >= 0) return static_cast<std::int32_t>(kOneQ30 - base);
	if (xQ28 < 0 && yQ28 < 0) return static_cast<std::int32_t>(-kOneQ30 + base);
	return static_cast<std::int32_t>(-base);
}

inline std::int32_t zoomScaleQ28(int tableIndex) {
	tableIndex = std::max(0, std::min(tableIndex, kZoomTableSize - 1));
	return kZoomScaleQ28[static_cast<std::size_t>(tableIndex)];
}

inline std::uint32_t rateMicroHz(int pitchIndex) {
	pitchIndex = std::max(kRateTableMinIndex, std::min(pitchIndex, kRateTableMaxIndex));
	return kRateMicroHz[static_cast<std::size_t>(pitchIndex - kRateTableMinIndex)];
}

} // namespace mandelwake
