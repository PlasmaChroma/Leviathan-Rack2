#pragma once

#include "../plugin.hpp"
#include "ThemePersistence.hpp"

#include <cmath>
#include <cstdint>

namespace leviathan {
namespace theme {

// Theme publications can invalidate cached artwork throughout the rack. Give
// every module instance a stable phase so those invalidations are distributed
// across several UI frames. Multiple visual layers owned by the same module
// receive the same phase and therefore continue to update together.
class ThemeUiPoller {
public:
	void setOwner(const void* owner) {
		std::uintptr_t hash = reinterpret_cast<std::uintptr_t>(owner);
		hash ^= hash >> 16u;
		hash *= std::uintptr_t(0x7feb352du);
		hash ^= hash >> 15u;
		hash *= std::uintptr_t(0x846ca68bu);
		hash ^= hash >> 16u;
		slot_ = uint32_t(hash % slotCount());
		nextPollAt_ = -1.0;
	}

	bool shouldPoll() {
		const double now = system::getTime();
		if (!std::isfinite(now)) {
			persistence::refreshExternalThemeIfChanged();
			return true;
		}

		const double period = pollPeriodSec();
		if (nextPollAt_ < 0.0) {
			const double windowStart = std::floor(now / period) * period;
			nextPollAt_ = windowStart + double(slot_) * period / double(slotCount());
			if (nextPollAt_ <= now) {
				nextPollAt_ += period;
			}
			return false;
		}
		if (now < nextPollAt_) {
			return false;
		}

		const double periodsElapsed = std::floor((now - nextPollAt_) / period) + 1.0;
		nextPollAt_ += periodsElapsed * period;
		persistence::refreshExternalThemeIfChanged();
		return true;
	}

private:
	static uint32_t slotCount() {
		return 12u;
	}

	static double pollPeriodSec() {
		return 0.24;
	}

	uint32_t slot_ = 0u;
	double nextPollAt_ = -1.0;
};

} // namespace theme
} // namespace leviathan
