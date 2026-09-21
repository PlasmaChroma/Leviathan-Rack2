#pragma once

#include "SibylTuning.hpp"
#include <array>
#include <cstdint>
#include <string>

namespace sibyl {
enum OverrideField {
    TRANSPOSE, VELOCITY_SCALE, VELOCITY_OFFSET, PROBABILITY_SCALE,
    PROBABILITY_OFFSET, GATE_SCALE, GATE_OFFSET, MOD_1_OFFSET, MOD2_OFFSET,
    MOD3_OFFSET, OVERRIDE_COUNT
};
struct OverrideFieldInfo { const char* name; float minimum; float maximum; };
inline const std::array<OverrideFieldInfo, OVERRIDE_COUNT>& overrideFields() {
    static const std::array<OverrideFieldInfo, OVERRIDE_COUNT> fields {{
        {"transposeSemitones", -120, 120}, {"velocityScale", 0, 4},
        {"velocityOffset", -1, 1}, {"probabilityScale", 0, 4},
        {"probabilityOffset", -1, 1}, {"gateScale", 0, 4},
        {"gateOffset", -1024, 1024}, {"modOffset", -20, 20},
        {"mod2Offset", -20, 20}, {"mod3Offset", -20, 20}
    }};
    return fields;
}
inline int overrideFieldIndex(const std::string& name) {
    const auto& fields = overrideFields();
    for (int i = 0; i < OVERRIDE_COUNT; ++i) if (name == fields[i].name) return i;
    return -1;
}
struct AssignmentOverrides {
    PitchOffsets pitchOffsets;
    bool present = false;
    uint16_t fields = 0; // Authored presence is retained through partial edits.
    std::array<float, OVERRIDE_COUNT> values {{0,1,0,1,0,1,0,0,0,0}};
    bool gateTransform() const { return values[GATE_SCALE] != 1.f || values[GATE_OFFSET] != 0.f; }
    bool operator==(const AssignmentOverrides& b) const {
        return pitchOffsets == b.pitchOffsets && present == b.present && fields == b.fields && values == b.values;
    }
    float pitch(float base) const { return values[TRANSPOSE] == 0.f ? base : base + values[TRANSPOSE] / 12.f; }
    float scaled(float base, int scale, int offset) const {
        // Preserve the legacy identity path, including signed zero.
        if (values[scale] != 1.f) base *= values[scale];
        if (values[offset] != 0.f) base += values[offset];
        return base;
    }
};
} // namespace sibyl
