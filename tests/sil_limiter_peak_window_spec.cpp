#include "../src/SilLimiterPeakWindow.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <new>

namespace {
bool trackHeap = false;
std::size_t allocations = 0;
std::size_t releases = 0;
}

void* operator new(std::size_t bytes) {
	if (trackHeap) ++allocations;
	if (void* p = std::malloc(bytes ? bytes : 1)) return p;
	throw std::bad_alloc();
}
void* operator new[](std::size_t bytes) { return ::operator new(bytes); }
void operator delete(void* p) noexcept {
	if (trackHeap && p) ++releases;
	std::free(p);
}
void operator delete[](void* p) noexcept { ::operator delete(p); }

namespace {
// Keep the old detector algorithm as a regression reference. A separate raw
// sliding maximum below checks behavior independently of monotone queues.
struct LegacyWindow {
	struct Entry { uint64_t index; float peak; };
	std::deque<Entry> entries;
	uint64_t next = 0;
	float push(float peak, int lookahead) {
		const uint64_t index = next++;
		while (!entries.empty() && entries.back().peak <= peak) entries.pop_back();
		entries.push_back({index, peak});
		const uint64_t minimum = index + 1 > uint64_t(lookahead)
			? index + 1 - uint64_t(lookahead) : 0;
		while (!entries.empty() && entries.front().index < minimum) entries.pop_front();
		return entries.empty() ? peak : entries.front().peak;
	}
	void clear() { entries.clear(); next = 0; }
};

float signal(int pattern, int i, uint32_t& random) {
	switch (pattern) {
		case 0: return float(i); // rising: repeatedly remove the whole queue
		case 1: return float(10000 - i); // falling: maximum occupancy and wrap
		case 2: return 3.f; // equal peaks
		case 3: return i % 521 == 0 ? 20.f : 0.f; // impulse expiry
		case 4: return i % 2 ? 0.f : 10.f;
		default:
			random = random * 1664525u + 1013904223u;
			return float(random >> 16) / 4096.f;
	}
}
}

int main() {
	sil::LimiterPeakWindow window;
	LegacyWindow legacy;
	std::array<float, 512> raw;
	std::size_t checks = 0;
	// Every supported lookahead, many ring wraps, and reset with queued peaks.
	for (int lookahead = 1; lookahead <= 512; ++lookahead) {
		for (int pattern = 0; pattern < 6; ++pattern) {
			window.clear();
			legacy.clear();
			int sinceReset = 0;
			uint32_t random = 0x12345678u;
			for (int i = 0; i < 4096; ++i) {
				if (i == 2301) {
					window.clear();
					legacy.clear();
					sinceReset = 0;
				}
				const float peak = signal(pattern, i, random);
				raw[std::size_t(sinceReset % lookahead)] = peak;
				++sinceReset;
				const float expected = *std::max_element(raw.begin(),
					raw.begin() + std::min(sinceReset, lookahead));
				const float actual = window.push(peak, lookahead);
				if (actual != legacy.push(peak, lookahead) || actual != expected) {
					std::printf("FAIL lookahead=%d pattern=%d sample=%d actual=%g expected=%g\n",
						lookahead, pattern, i, actual, expected);
					return 1;
				}
				++checks;
			}
		}
	}

	volatile float sink = 0.f;
	trackHeap = true;
	// Include construction, cold/warm updates, resets, and destruction.
	{
		sil::LimiterPeakWindow measured;
		for (int i = 0; i < 100000; ++i) {
			if (i % 25000 == 0) measured.clear();
			sink = measured.push(float(200000 - i), 512);
		}
	}
	trackHeap = false;
	(void) sink;
	std::printf("%zu exact peak comparisons passed; 100000 updates: allocations=%zu releases=%zu\n",
		checks, allocations, releases);
	return allocations || releases ? 1 : 0;
}
