#pragma once

#include <array>

namespace tiamat {

// Original float32 lookup order, including both leading zeros. Amount mapping
// and persistent Decimator state belong to Phase 4, not the control mapper.
constexpr std::array<float, 17> decimateBits {{
    0.f, 0.f, .58f, .60f, .17f, .68f, .69f, .63f, .43f,
    .64f, .49f, .52f, .54f, .87f, .74f, .77f, .80f
}};
constexpr std::array<float, 17> decimateRates {{
    0.f, 0.f, .33f, .51f, .47f, .49f, .42f, .82f, .28f,
    .73f, .64f, .26f, .52f, .17f, .22f, .96f, .60f
}};

} // namespace tiamat
