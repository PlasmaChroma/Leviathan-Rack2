#include "../src/common/MathHelpers.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

static void need(bool ok, const char* message) {
    if (!ok) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
int main() {
    using namespace levi_math;
    const double inf = std::numeric_limits<double>::infinity();
    const double nan = std::numeric_limits<double>::quiet_NaN();
    double expError = 0, logError = 0, pitchCents = 0;
    for (int i = 0; i <= 200000; ++i) {
        const double x = -1022.0 + 2044.0 * i / 200000.0;
        const double exact = std::exp2(x);
        expError = std::max(expError, std::abs(exp2Audio(x) / exact - 1));
        logError = std::max(logError, std::abs(log2Audio(exact) - std::log2(exact)));
        pitchCents = std::max(pitchCents, std::abs(1200 * std::log2(exp2Pitch(x) / exact)));
        need(exp2AudioChecked(x) == exp2Audio(x), "checked exp preserves fast domain");
        need(exp2PitchChecked(x) == exp2Pitch(x), "checked pitch preserves fast domain");
        need(log2AudioChecked(exact) == log2Audio(exact), "checked log preserves fast domain");
        const double unit = (i + 1.) / 200001.;
        need(powUnitAudioChecked(unit, 1.7093) == powUnitAudio(unit, 1.7093), "checked power preserves fast domain");
    }
    need(expError < 5e-13 && logError < 1e-12 && pitchCents < 0.001, "full-domain accuracy");
    for (int i = -1022; i <= 1022; ++i)
        need(exp2Audio(i) == std::exp2(double(i)), "exact integer octaves");
    for (double x : {-inf, -1074., -1023., 1023., 1024., inf}) {
        need(exp2AudioChecked(x) == std::exp2(x), "exp fallback");
        need(exp2PitchChecked(x) == std::exp2(x), "pitch fallback");
    }
    need(std::isnan(exp2AudioChecked(nan)) && std::isnan(exp2PitchChecked(nan)), "exp NaN");
    need(log2AudioChecked(0) == -inf && std::isnan(log2AudioChecked(-1))
         && std::isnan(log2AudioChecked(nan)) && log2AudioChecked(inf) == inf, "log exceptional inputs");
    const double tiny = std::numeric_limits<double>::denorm_min();
    need(log2AudioChecked(tiny) == std::log2(tiny), "log subnormal");
    need(powUnitAudioChecked(tiny, 1) == tiny && powUnitAudioChecked(0, 0) == 0
         && powUnitAudioChecked(-1, 2) == 0 && powUnitAudioChecked(0.5, inf) == 0
         && std::isnan(powUnitAudioChecked(nan, 2)), "power fallback and legacy zero policy");
    for (float x : {-2.f, -1.75f, -0.1f, 0.f, 0.25f, 1.9f, 2.f}) {
        need(sinCyclesAudioChecked(x) == sinCyclesAudioBounded(x), "sine compatibility");
        need(triangleCyclesAudioChecked(x) == triangleCyclesAudioBounded(x), "triangle compatibility");
    }
    for (float x : {-100.25f, 123.75f, std::numeric_limits<float>::max()}) {
        need(sinCyclesAudioChecked(x) == sinCyclesAudioBounded(wrap01(x)), "sine wrapping");
        need(triangleCyclesAudioChecked(x) == triangleCyclesAudioBounded(wrap01(x)), "triangle wrapping");
    }
    need(std::isnan(sinCyclesAudioChecked(float(inf))) && std::isnan(triangleCyclesAudioChecked(float(nan))), "phase exceptional inputs");
    need(clampf(float(nan), -1, 1) == 1 && clamp01(float(nan)) == 0, "legacy clamp NaN ordering");
    need(!std::signbit(clampf(-0.f, 0.f, 1.f)), "legacy clamp signed zero");
    need(wrap01Fast(-0.25f) == 0.75f && wrap01Fast(1.25f) == 0.25f, "single-cycle wrapping");
    std::cout << "PASS: contracts; exp relative=" << expError << " log absolute=" << logError
              << " pitch cents=" << pitchCents << '\n';
}
