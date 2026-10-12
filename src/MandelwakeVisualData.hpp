#pragma once

#include "MandelwakeEngine.hpp"

#include <array>
#include <cstdint>

namespace mandelwake {

struct VisualSnapshot {
	std::array<HistoryPoint, kHistoryCapacity> points {};
	std::uint16_t pointCount = 0;
	std::uint8_t selectedChannel = 0;
	std::uint8_t map = 0;
	std::uint32_t escapeSerial = 0;
	std::uint32_t resetSerial = 0;
	std::uint32_t reseedSerial = 0;
	std::uint32_t mapSerial = 0;
	HistoryPoint current {};
	HistoryPoint lastPreEscape {};
	HistoryPoint lastReentry {};
	float mutation = 0.f;
	bool seedLocked = true;
	bool channelActive = true;
	bool compatibilityWarning = false;
};

} // namespace mandelwake
