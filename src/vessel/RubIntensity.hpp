#pragma once

#include "Types.hpp"
#include <algorithm>

namespace vessel {

// Performance mapping only: leaves the friction law and modal mechanics intact.
// Keep maximum hand slip near the mallet's velocity-weakening region. Calculate
// at the control rate as bowl/material descriptors morph, not per audio sample.
inline double rubIntensityMaximumSpeed(const BowlDescriptor& bowl, const MalletDescriptor& mallet,
                                      double slipScale = 1.2) noexcept {
    return std::min(2.0, slipScale*mallet.weakeningVelocity/(2.0*pi*bowl.rimRadius));
}

struct RubGesture { double speed, pressure; };

// Give quiet playing useful contact effort without a discontinuous minimum.
// UI/CV position stays linear; this shared response has exact zero/full endpoints.
inline double rubIntensityEffort(double amount) noexcept {
    return amount/(.15+.85*amount);
}

// Caller provides a finite normalized amount and cached maximum speed.
inline RubGesture rubIntensity(double amount, double maximumSpeed) noexcept {
    return {amount*maximumSpeed, amount*15.0};
}

// A virtual player's feedback, acting only through existing contact controls.
// Prototype playing policy, not a measured friction/material law. The 1 kHz
// update and 20/40 ms filters avoid following individual vibration cycles.
// Caller supplies finite normalized amount, mechanical energy in joules, and
// cached descriptor-based speeds. No audio gain or meter position is observed.
struct RubIntensityPlayer {
    double elapsed = 0, energy = 0, growth = 0, grip = 1;
    bool initialized = false;
    RubGesture gesture {};
    RubGesture process(double amount, bool engaged, double measuredEnergy,
                       double maximumSpeed, double startSpeed, double dt) noexcept {
        if (!initialized) { energy = std::max(0.,measuredEnergy); initialized = true; }
        elapsed += dt;
        if (elapsed >= .001) {
            const double effort = rubIntensityEffort(amount);
            const double h = elapsed;
            elapsed = 0;
            const double previous = energy;
            energy += h/(.02+h)*(std::max(0.,measuredEnergy)-energy);
            growth += h/(.04+h)*((energy-previous)/h-growth);
            // Move from a useful startup slip toward the sustained range as the
            // bowl responds. 2 mJ is a transition scale, not an energy target.
            const double readiness = energy/(energy+.002);
            // Relax grip when the measured relative growth outpaces effort.
            // This is soft feedback, not a hard energy/rate limiter. The 2 uJ
            // floor permits startup without division by near-zero energy.
            const double allowedGrowth = effort*1.5*(energy+.000002);
            grip = std::max(0.,std::min(1.,grip+h*.5*(allowedGrowth-growth)/(energy+.000002)));
            gesture.speed = effort*(startSpeed+(maximumSpeed-startSpeed)*readiness);
            gesture.pressure = effort*(3.+12.*readiness)*grip;
        }
        if (!engaged || amount <= 0) { gesture = {}; grip = 1; }
        return gesture;
    }
};

} // namespace vessel
