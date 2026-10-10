#pragma once
#include <cstdint>

// Offline work counts only. No timers, atomics, logging, or production storage.
// Define consistently for the profiling executable, never for normal timing.
namespace vessel {
struct FrictionWork {
    std::uint64_t solves=0, converged=0, zeroLoad=0, lawEvaluations=0;
    std::uint64_t expEvaluations=0, tanhEvaluations=0, newton=0, fallback=0;
    std::uint64_t gaussianCore=0, gaussianTail=0, tanhLookups=0;
};
struct ContactWork {
    FrictionWork rub, coupled;
    std::uint64_t coupledSolves=0, outerTrials=0, bracketExpansions=0;
    std::uint64_t outerNewton=0, outerFallback=0;
    unsigned coupledDepth=0;
    FrictionWork& friction() noexcept { return coupledDepth ? coupled : rub; }
};
#if defined(VESSEL_PROFILE_CONTACT_WORK)
inline ContactWork& contactWork() noexcept {
    static thread_local ContactWork work;
    return work;
}
struct ContactWorkScope {
    ContactWorkScope() noexcept { ++contactWork().coupledSolves; ++contactWork().coupledDepth; }
    ~ContactWorkScope() { --contactWork().coupledDepth; }
};
#define VESSEL_WORK(expression) expression
#else
#define VESSEL_WORK(expression) ((void)0)
#endif
} // namespace vessel
