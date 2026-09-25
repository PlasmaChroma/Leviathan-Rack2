#include "ChimeraProfile.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
int main() {
    double expRelative = 0, logAbsolute = 0, powCents = 0, geneError = 0, pitchCents = 0, combinedCents = 0;
    for (unsigned i = 0; i <= 200000; ++i) {
        const double t = i/200000.0;
        const double x = -24+48*t, exact = std::exp2(x);
        expRelative = std::max(expRelative, std::fabs(levi_math::exp2Audio(x)/exact-1));
        pitchCents = std::max(pitchCents, std::fabs(1200*std::log2(levi_math::exp2Pitch(x)/exact)));
        const double positive = std::exp2(-70+94*t);
        logAbsolute = std::max(logAbsolute, std::fabs(levi_math::log2Audio(positive)-std::log2(positive)));
        const double q = 1e-15+(1-1e-15)*t;
        for (double gamma : {std::log(0.5)/std::log((2.0/3-0.0001)/0.9999),
                             std::log(0.5)/std::log((0.75-0.0001)/0.9999)}) {
            const double approximate = levi_math::powUnitAudio(q, gamma);
            powCents = std::max(powCents, std::fabs(1200*std::log2(approximate/std::pow(q, gamma))));
            combinedCents = std::max(combinedCents, std::fabs(1200*std::log2(
                approximate*levi_math::exp2Pitch(x)/(std::pow(q, gamma)*exact))));
        }
        for (unsigned length : {17u, 480u, 48000u, chimera::kMaxReelFrames}) {
            const double logLength = levi_math::log2Audio(length);
            const double approximate = levi_math::exp2Audio(logLength+t*(4-logLength));
            const double reference = std::exp(std::log(double(length))+t*(std::log(16.0)-std::log(double(length))));
            geneError = std::max(geneError, std::fabs(approximate-reference));
        }
    }
    std::printf("exp2 relative=%g log2 absolute=%g rate cents=%g Gene frames=%g\n",
        expRelative, logAbsolute, powCents, geneError);
    need(expRelative < 5e-13 && logAbsolute < 1e-12 && powCents < 0.001 && pitchCents < 0.001 && combinedCents < 0.001 && geneError < 1e-5,
         "fast scalar math preserves pitch and integer-frame timing within bounded error");
    std::printf("pitch cents=%g combined rate cents=%g\n", pitchCents, combinedCents);
    for (int octave = -24; octave <= 24; ++octave)
        need(levi_math::exp2Audio(octave) == std::exp2(octave), "integer octaves remain exact");
    volatile double sink = 0;
    for (bool fast : {false, true}) {
        const auto start = std::chrono::steady_clock::now();
        for (unsigned i = 0; i < 2000000; ++i) {
            const double x = (i%100000+1)/100001.0;
            sink += fast ? levi_math::powUnitAudio(x, 1.7093)*levi_math::exp2Pitch(x*16-8) :
                std::pow(x, 1.7093)*std::exp2(x*16-8);
        }
        std::printf("rate mapping %s: %.3f ms\n", fast ? "fast" : "libm",
            1000*std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count());
    }
    std::printf("PASS: scalar audio math sweep; sink=%g\n", double(sink));
}
