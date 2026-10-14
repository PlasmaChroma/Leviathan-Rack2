#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace sibyl {
struct PitchOffsets {
    uint8_t fields = 0;
    int32_t steps = 0, periods = 0;
    double cents = 0.;
    bool operator==(const PitchOffsets& b) const {
        return fields==b.fields && steps==b.steps && periods==b.periods && cents==b.cents;
    }
};
inline int pitchOffsetField(const std::string& name) {
    return name=="transposeSteps" ? 0 : name=="transposePeriods" ? 1 : name=="transposeCents" ? 2 : -1;
}
struct NativePitch {
    std::string context;
    int64_t index = 0;
    bool lattice = false;
    double baseV = 0., periodV = 1.;
};
// Control-side pitch mathematics. Playback consumes compiled voltages only.
struct PitchTuning {
    int divisions = 12;
    double periodV = 1.;
    std::vector<double> positionsV; // Empty for equal divisions; otherwise includes unison.
    double lattice(int64_t index) const {
        if (positionsV.empty()) return double(index) * periodV / divisions;
        int64_t q = index / divisions, r = index % divisions;
        if (r < 0) { r += divisions; --q; }
        return double(q) * periodV + positionsV[size_t(r)];
    }
};
struct PitchScale { std::string tuning; std::vector<int> steps; };
struct PitchContext { std::string tuning, scale; double anchorV = 0.; };
struct PitchSystems {
    std::string authored; // Canonical definitions, including metadata and exact ratios.
    std::string defaultContext;
    std::map<std::string, PitchTuning> tunings;
    std::map<std::string, PitchScale> scales;
    std::map<std::string, PitchContext> contexts;
    size_t storageBytes() const {
        // Include capacities and a conservative allowance for each tree node.
        size_t bytes=sizeof(PitchSystems)+authored.capacity()+defaultContext.capacity()+2;
        for (const auto& entry : tunings)
            bytes+=sizeof(entry)+64+entry.first.capacity()+1+entry.second.positionsV.capacity()*sizeof(double);
        for (const auto& entry : scales)
            bytes+=sizeof(entry)+64+entry.first.capacity()+entry.second.tuning.capacity()+2+entry.second.steps.capacity()*sizeof(int);
        for (const auto& entry : contexts)
            bytes+=sizeof(entry)+64+entry.first.capacity()+entry.second.tuning.capacity()+entry.second.scale.capacity()+3;
        return bytes;
    }
};
inline int64_t pitchDegreeIndex(int32_t degree, int divisions, const std::vector<int>& scale) {
    if (scale.empty()) return degree;
    const int64_t size = int64_t(scale.size());
    int64_t q = degree / size, r = degree % size;
    if (r < 0) { r += size; --q; }
    return q * divisions + scale[size_t(r)];
}
inline bool pitchInDomain(double voltage) {
    return std::isfinite(voltage) && voltage >= -10. && voltage <= 10.;
}
} // namespace sibyl
