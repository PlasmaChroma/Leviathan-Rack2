#pragma once
#include "MathHelpers.hpp"

namespace maths_function {
// Calibrated BOTH response shared by Integral Flux, Proc and Pelagic.
// The caller supplies its fast exp2 implementation, keeping this Rack-free.
template <class Exp2>
float bothHzFromCv(float volts, Exp2 exp2) {
    const float x=1.10815030f*(volts-4.15514297f);
    const float r=exp2(x);
    return 1.93157058f+986.84629918f*(r/(1.f+r));
}
template <class Exp2>
float bothTimeScaleFromCv(float volts, Exp2 exp2) {
    const float frequency=bothHzFromCv(levi_math::softLimit(volts,8.f),exp2);
    static const float neutralHz=bothHzFromCv(-.05f,exp2);
    return levi_math::clampf(neutralHz/std::max(frequency,1e-6f),1.f/64.f,64.f);
}
}
