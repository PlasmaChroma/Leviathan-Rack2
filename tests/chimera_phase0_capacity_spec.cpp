// Worst supported host-rate bounds before committing the bridge layout.
#include <array>
#include <cstdint>
#include <cstdio>

namespace {
constexpr std::size_t kJacks = 13;
constexpr std::size_t kHostsPerCoreAt768k = 16;
constexpr std::size_t kInputDelayAt768k = 640; // Measured Speex quality 5.
constexpr std::size_t kEventsPerCoreLimit = 256;
constexpr std::size_t kHistoryCapacity = 16384;
constexpr std::uint64_t kStoreAudioBytes = 2ull * 66816000ull; // Active + COW reserve.
constexpr std::uint64_t kAudioBudget = 256ull * 1024ull * 1024ull;
static_assert(kJacks * kHostsPerCoreAt768k == 208, "core event burst changed");
static_assert(kJacks * kHostsPerCoreAt768k < kEventsPerCoreLimit, "core event cap exceeded");
static_assert(kJacks * kInputDelayAt768k == 8320, "history delay bound changed");
static_assert(kJacks * (kInputDelayAt768k + 1) < kHistoryCapacity,
              "history ring too small for enqueue-before-consume ordering");
static_assert(2 * kStoreAudioBytes <= kAudioBudget, "active plus prepared exceeds budget");
static_assert(3 * kStoreAudioBytes > kAudioBudget, "leased retired store must block preparation");

struct Event { std::uint32_t hostFrame; std::uint8_t jack; };
}

int main() {
    std::array<Event, kHistoryCapacity> ring{};
    std::size_t read = 0, write = 0, maximum = 0;
    // Every jack changes connection/edge state on every host frame. The bridge
    // keeps each encoded jack record until the delayed input can be mapped.
    for (std::uint32_t host = 0; host < 10000; ++host) {
        for (std::size_t jack = 0; jack < kJacks; ++jack) {
            if (write - read == kHistoryCapacity) return 1;
            ring[write % kHistoryCapacity] = {host, static_cast<std::uint8_t>(jack)};
            ++write;
        }
        if (write - read > maximum) maximum = write - read;
        if (host >= kInputDelayAt768k) {
            for (std::size_t jack = 0; jack < kJacks; ++jack) {
                const Event e = ring[read % kHistoryCapacity];
                if (e.hostFrame != host - kInputDelayAt768k || e.jack != jack) return 2;
                ++read;
            }
        }
    }
    if (maximum != kJacks * (kInputDelayAt768k + 1)) return 3;
    std::printf("PASS: 768 kHz delayed events peak=%zu/%zu; two resident stores=%llu/%llu bytes\n",
        maximum, kHistoryCapacity,
        static_cast<unsigned long long>(2 * kStoreAudioBytes),
        static_cast<unsigned long long>(kAudioBudget));
}
