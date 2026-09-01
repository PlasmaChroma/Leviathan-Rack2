#pragma once

#include "PhonexTypes.hpp"

#include <array>
#include <atomic>
#include <cstdint>

namespace phonex {

class SequenceMailbox {
public:
	SequenceMailbox() {
		for (auto& state : slotState_)
			state.store(0u, std::memory_order_relaxed);
	}

	std::uint32_t publish(const LpcSequence& sequence) {
		const std::uint32_t current = publication_.load(std::memory_order_relaxed);
		const std::uint32_t nextGeneration = (current >> 1) + 1u;
		const std::uint32_t preferred = (current ^ 1u) & 1u;
		for (;;) {
			for (std::uint32_t attempt = 0; attempt < 2u; ++attempt) {
				const std::uint32_t index = preferred ^ attempt;
				std::uint32_t expected = 0u;
				if (!slotState_[index].compare_exchange_strong(expected, kWriter,
					std::memory_order_acquire, std::memory_order_relaxed))
					continue;
				slots_[index] = sequence;
				slots_[index].generation = nextGeneration;
				slotState_[index].store(0u, std::memory_order_release);
				publication_.store((nextGeneration << 1) | index, std::memory_order_release);
				return nextGeneration;
			}
			// Both slots can be reserved only for the few instructions in which
			// the audio reader pins a replacement before releasing its old slot.
		}
	}

	const LpcSequence* acquire(std::uint32_t& observedGeneration) {
		const std::uint32_t publication = publication_.load(std::memory_order_acquire);
		const std::uint32_t generation = publication >> 1;
		if (generation == 0 || generation == observedGeneration)
			return nullptr;
		const std::uint32_t index = publication & 1u;
		std::uint32_t expected = 0u;
		if (!slotState_[index].compare_exchange_strong(expected, 1u,
			std::memory_order_acquire, std::memory_order_relaxed))
			return nullptr;
		if (publication_.load(std::memory_order_acquire) != publication) {
			slotState_[index].fetch_sub(1u, std::memory_order_release);
			return nullptr;
		}
		const int previous = activeIndex_;
		activeIndex_ = static_cast<int>(index);
		observedGeneration = generation;
		if (previous >= 0 && previous != activeIndex_)
			slotState_[static_cast<std::size_t>(previous)].fetch_sub(
				1u, std::memory_order_release);
		return &slots_[index];
	}

private:
	static constexpr std::uint32_t kWriter = 0x80000000u;
	std::array<LpcSequence, 2> slots_{};
	std::array<std::atomic<std::uint32_t>, 2> slotState_;
	std::atomic<std::uint32_t> publication_{0};
	int activeIndex_ = -1;
};

} // namespace phonex
