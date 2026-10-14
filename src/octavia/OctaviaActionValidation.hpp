#pragma once

#include <cmath>
#include <cstddef>
#include <string>

namespace octavia {

constexpr float kMaxAbsRackHp = 1000000.f;
constexpr int kMaxAbsRackRow = 100000;
constexpr std::size_t kMaxBulkChanges = 1024;
constexpr std::size_t kMaxModuleStateBytes = 1024 * 1024;
constexpr std::size_t kMaxSemanticRequestBytes = 1024 * 1024;

inline std::string validateRackPosition(float hp, int row) {
	if (!std::isfinite(hp)) return "hp must be finite";
	if (std::fabs(hp) > kMaxAbsRackHp)
		return "hp exceeds the safe coordinate limit of +/-1000000";
	if (row < -kMaxAbsRackRow || row > kMaxAbsRackRow)
		return "row exceeds the safe coordinate limit of +/-100000";
	return {};
}

inline std::string validateParameterValue(float value) {
	return std::isfinite(value) ? std::string() : "parameter value must be finite";
}

} // namespace octavia
