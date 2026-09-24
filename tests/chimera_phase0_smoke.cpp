// Phase 0 harness now compiles Phase 1's real Rack-independent type contract.
#include "ChimeraTypes.hpp"
#include <type_traits>

static_assert(std::is_trivially_copyable<chimera::StereoFrame>::value, "frame must be POD");
static_assert(std::is_trivially_copyable<chimera::Region>::value, "region must be POD");
static_assert(std::is_trivially_copyable<chimera::CoreInput>::value, "input must be POD");
static_assert(std::is_trivially_copyable<chimera::CoreOutput>::value, "output must be POD");

int main() {
    const chimera::Region region{0, chimera::kCoreRate};
    const chimera::StereoFrame frame{1.f, -1.f};
    return region.end == chimera::kCoreRate && frame.l == -frame.r ? 0 : 1;
}
