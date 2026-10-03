#pragma once
#include <algorithm>
#include <cmath>

namespace vessel {
// Display-only curve: gentle linear toe joining a logarithmic audible range.
// 20 mJ remains full scale; 2 uJ sets the toe, not a silence threshold.
inline double energyMeterPosition(double joules) noexcept {
    return std::min(1., std::log1p(std::max(0.,joules)/.000002)/9.210440366976517);
}

// Called at the visual control rate, never per audio sample. Equal rise/fall
// response avoids favoring peaks during beating and intermittent energy growth.
inline double smoothEnergyMeter(double position, double target, double dt) noexcept {
    return position-std::expm1(-dt/.15)*(target-position);
}
} // namespace vessel
