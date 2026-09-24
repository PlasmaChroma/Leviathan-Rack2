// Phase 0 compiler/harness probe. Phase 1 replaces these local PODs with
// Chimera's Rack-independent engine types and real reference-vector checks.
#include <cstdint>
#include <type_traits>

namespace chimera_phase0 {
struct StereoFrame {
    float left;
    float right;
};
struct Region {
    std::uint32_t begin;
    std::uint32_t end;
};
constexpr std::uint32_t kCoreRate = 48000;
constexpr std::uint32_t kMaxFrames = 8352000;
constexpr std::uint32_t kPageFrames = 256;
static_assert(std::is_trivially_copyable<StereoFrame>::value, "frame must be POD");
static_assert(std::is_trivially_copyable<Region>::value, "region must be POD");
static_assert(kMaxFrames / kPageFrames == 32625, "page capacity changed");
}  // namespace chimera_phase0

int main() {
    const chimera_phase0::Region region{0, chimera_phase0::kCoreRate};
    const chimera_phase0::StereoFrame frame{1.f, -1.f};
    return region.end == chimera_phase0::kCoreRate && frame.left == -frame.right ? 0 : 1;
}
