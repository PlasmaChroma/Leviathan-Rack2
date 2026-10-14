#pragma once
// MP86 finite-state reference helpers. See analysis/MODULATION_SCHEDULER.md.
// Event arbitration and external writers of the counters are not included.
#include <cmath>
#include <cstdint>

namespace mp86_audit {
struct HaloWalk {
    float position=0.1f;
    float increment=0;

    void advance() noexcept {
        position=increment+position;
        if (position>0.35f || position<0.05f) {
            const float boundary=position>0.35f ? 0.35f : 0.05f;
            increment=-increment;
            position=std::fma(increment,2.0f,boundary);
        }
    }
};

struct HaloModulation {
    std::uint32_t random_state=12345;
    HaloWalk a{0.1f,1.61e-6f};  // 969-sample allpass
    HaloWalk b{0.1f,1.24e-6f};  // 803-sample allpass

    // Call only for the corresponding counter expiry; A precedes B if both
    // expire. Unsigned overflow is the firmware's modulo-2^32 arithmetic.
    void perturb(bool second) noexcept {
        random_state=random_state*196314165u+907633515u;
        const float centered=std::fma(float(random_state),0x1p-32f,-0.5f);
        auto& walk=second ? b : a;
        const float candidate=std::fma(centered,2e-6f,walk.increment);
        walk.increment=candidate>3e-6f ? (second ? 2.3e-6f : 1.3e-6f)
                      : candidate < -3e-6f ? (second ? -2.1e-6f : -1.2e-6f)
                      : candidate;
    }
    void advance() noexcept { a.advance(); b.advance(); }
};

struct DelayExpiryCounters {
    // Raw words make wrap well-defined; firmware interprets the decremented
    // words as signed for expiry. External control paths can rewrite them.
    std::uint32_t a=0,b=0;
    void decrement(float ratio) noexcept {
        --a;
        b=ratio==1.0f ? a : b-1u;
    }
    static bool expired(std::uint32_t word) noexcept { return (word&0x80000000u)!=0; }
    // Precondition: finite sum representable by int32_t; ARM exceptional
    // conversion behavior is deliberately outside this helper's scope.
    static std::uint32_t reload(float delay,float timing_offset) noexcept {
        return static_cast<std::uint32_t>(static_cast<std::int32_t>(timing_offset+delay));
    }
};
} // namespace mp86_audit
