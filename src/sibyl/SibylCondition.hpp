#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace sibyl {

// Exact in JSON/double telemetry; counters saturate rather than wrap.
constexpr int64_t kConditionOrdinalMax = (int64_t(1) << 52) - 1;
enum class ConditionScope { PATTERN_PASS, SCENE_REPEAT, ARRANGEMENT_LOOP };
enum class ConditionTestKind { EVERY, FIRST, LAST };
struct ConditionTest {
    ConditionScope scope = ConditionScope::PATTERN_PASS;
    ConditionTestKind kind = ConditionTestKind::EVERY;
    int every = 1;
    int offset = 1;
    bool operator==(const ConditionTest& b) const {
        return scope == b.scope && kind == b.kind && every == b.every && offset == b.offset;
    }
};
struct Condition {
    bool present = false;
    uint8_t count = 0;
    std::array<ConditionTest, 8> tests {};
    bool operator==(const Condition& b) const {
        if (present != b.present || count != b.count) return false;
        for (unsigned i = 0; i < count; ++i) if (!(tests[i] == b.tests[i])) return false;
        return true;
    }
};
inline int64_t conditionOrdinal(int64_t value) {
    return std::max(int64_t(1), std::min(kConditionOrdinalMax, value));
}
inline int64_t conditionCycle(double phase, double duration) {
    if (!(duration > 0.) || !std::isfinite(phase)) return 0;
    return static_cast<int64_t>(std::max(-double(kConditionOrdinalMax),
        std::min(double(kConditionOrdinalMax), std::floor(phase / duration))));
}
// Phase coordinates may be rebased by adoption independently of the ordinal.
// Ordinary samples only compare to the cached next boundary; no event scan.
struct ConditionCursor {
    int64_t pass = 1;
    int64_t cycle = 0;
    double nextBoundary = 0.;
    void rebase(double phase, double duration, int64_t ordinal) {
        pass = conditionOrdinal(ordinal);
        cycle = conditionCycle(phase, duration);
        nextBoundary = (double(cycle) + 1.) * duration;
    }
    void advance(double phase, double duration) {
        if (phase < nextBoundary) return;
        const int64_t next = conditionCycle(phase, duration);
        pass = conditionOrdinal(pass + std::max(int64_t(0), next - cycle));
        cycle = next;
        nextBoundary = (double(cycle) + 1.) * duration;
    }
    int64_t eventPass(int64_t nominalStep, int length) const {
        const int64_t eventCycle = nominalStep / length - (nominalStep % length < 0 ? 1 : 0);
        return conditionOrdinal(pass + std::max(-kConditionOrdinalMax,
            std::min(kConditionOrdinalMax, eventCycle)) - cycle);
    }
};
inline bool conditionEligible(const Condition& condition, int64_t patternPass,
        int64_t sceneRepeat, int64_t sceneRepeats, int64_t arrangementLoop) {
    for (unsigned i = 0; i < condition.count; ++i) {
        const auto& test = condition.tests[i];
        int64_t ordinal = test.scope == ConditionScope::PATTERN_PASS ? patternPass :
            test.scope == ConditionScope::SCENE_REPEAT ? sceneRepeat : arrangementLoop;
        if (test.kind == ConditionTestKind::FIRST) { if (ordinal != 1) return false; }
        else if (test.kind == ConditionTestKind::LAST) { if (ordinal != sceneRepeats) return false; }
        else if ((ordinal - test.offset) % test.every != 0) return false;
    }
    return true;
}
} // namespace sibyl
