#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace sil {

// Monotone maximum queue for the limiter detector. Storage belongs to the
// module: updates and resets never allocate or release heap memory.
class LimiterPeakWindow {
public:
	enum { kMaximumLookaheadSamples = 512 };

	void clear() {
		head = 0;
		count = 0;
		nextSampleIndex = 0;
	}

	// Sil clears the window whenever its lookahead changes.
	float push(float peak, int lookaheadSamples) {
		assert(lookaheadSamples >= 1 && lookaheadSamples <= kMaximumLookaheadSamples);
		const uint64_t sampleIndex = nextSampleIndex++;
		while (count && entries[slot(count - 1)].peak <= peak) {
			--count;
		}
		assert(count < kCapacity);
		Entry& entry = entries[slot(count++)];
		entry.sampleIndex = sampleIndex;
		entry.peak = peak;
		const uint64_t maxAge = uint64_t(lookaheadSamples);
		const uint64_t minValidIndex = sampleIndex + 1u > maxAge
			? sampleIndex + 1u - maxAge : 0u;
		while (count && entries[head].sampleIndex < minValidIndex) {
			head = head + 1 == kCapacity ? 0 : head + 1;
			--count;
		}
		return count ? entries[head].peak : peak;
	}

private:
	// Preserve insertion-before-expiry order, including one transient entry
	// beyond the full lookahead on a strictly decreasing peak stream.
	static constexpr std::size_t kCapacity = kMaximumLookaheadSamples + 1;
	struct Entry {
		uint64_t sampleIndex;
		float peak;
	};
	std::array<Entry, kCapacity> entries;
	std::size_t head = 0;
	std::size_t count = 0;
	uint64_t nextSampleIndex = 0;

	std::size_t slot(std::size_t offset) const {
		const std::size_t index = head + offset;
		return index >= kCapacity ? index - kCapacity : index;
	}
};

} // namespace sil
