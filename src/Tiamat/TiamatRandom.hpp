#pragma once

#include <cstdint>

namespace tiamat {

// One shared Macro/outer-clock/Dropout stream per engine. Vinyl's distinct
// generators will be owned by its effect state, not by this stream.
class Random {
public:
    explicit Random(std::uint64_t seed = 1) noexcept : state_(seed) {}
    void restart(std::uint64_t seed) noexcept { state_ = seed; }
    std::uint64_t state() const noexcept { return state_; }
    std::uint32_t next() noexcept {
        state_ = state_ * UINT64_C(6364136223846793005) + UINT64_C(1);
        return static_cast<std::uint32_t>(state_ >> 32) & UINT32_C(0x7fffffff);
    }
    float unit255() noexcept { return float(next() % 255) / 255.f; }
private:
    std::uint64_t state_;
};

} // namespace tiamat
