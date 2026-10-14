// Selected MG204 instruction transcriptions. NOT a complete Morphagene emulator.
// Preconditions: finite inputs, ADC 0..4095, positive splice, normalized rate 0..1.
// Compile without fast-math and with -ffp-contract=off; only explicit fma is fused.
#pragma once
#include "exact_tables.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace mg204 {
inline int morphStage(unsigned adc) {
    return std::min(21, int((float(adc) * 0x1p-12f) * 33.99f));
}
// 0x080276be..0x080276e6 and 0x08028850..0x0802886a.
inline float sosTarget(unsigned adc) {
    const float scaled = float(adc) * 0x1.144dfcp-12f; // 0x398a26fe
    return scaled > 0x1.0c80c8p+0f ? 1.f : std::max(0.f, scaled - 0x1.9018e8p-5f);
}
inline float smooth(float state, float target, float alpha) {
    return std::fma(target-state, alpha, state);
}
// Separate mode result: whole-splice is not a short ordinary Gene.
struct GeneSize { bool wholeSplice; float samples; };
// Envelope configuration only; not the complete voice lifecycle or renderer.
// 0x08028ad4..0x08028ae8, 0x08028924..0x08028990,
// 0x08027b58..0x08027b94. Finite positive duration, after its eight-frame floor.
struct GeneEnvelopeConfig { float edgeSamples, increment; };
inline GeneEnvelopeConfig geneEnvelopeConfig(float samples, float density,
                                             float launchFactor, bool gnsm) {
    float edge = std::min(samples * .5f, 24000.f);
    float increment = edge == 24000.f ? 0x1.5d867cp-15f : 2.f * (1.f / samples);
    if ((edge > 250.f && (density < 1.f || !gnsm)) || launchFactor == 1.f) {
        edge = 250.f;
        increment = 0x1.0624dep-8f; // firmware 0x3b83126f
    }
    return {edge, increment};
}
inline GeneSize ordinaryGene(unsigned adc, int32_t spliceSamples) {
    if (adc <= 199) return {true, float(spliceSamples)};
    float span = float(spliceSamples);
    while (span > 576000.f) span *= .5f;
    const float x = gene_size_exp[1073-(adc>>2)];
    const float square = x*x;
    const float cube = square*x;
    return {false, std::max(8.f, cube*span)};
}
// 0x0802ab6c..0x0802abd8, store at 0x0802acd2..0x0802acd6.
// Returns the CURRENT upper bin when preliminary >= the NEXT lower boundary.
inline float clockQuantize(float preliminary, float unfoldedSplice) {
    if (preliminary <= 8.f) return preliminary;
    constexpr float twoThirds = 0x1.55553ep-1f; // 0x3f2aaa9f, not exact 2/3
    float current = unfoldedSplice * twoThirds;
    if (preliminary >= current) return unfoldedSplice;
    bool threeQuarters = true;
    for (;;) {
        const float next = current * (threeQuarters ? .75f : twoThirds);
        if (preliminary >= next) return current;
        current = next;
        threeQuarters = !threeQuarters;
    }
}
// Already calibrated normalized Vari-Speed control. Analog voltage calibration,
// update holdoffs and the clock multiplier are outside this helper.
inline float rateTarget(float x, unsigned vsop) {
    if (vsop > 1) {
        const int i = std::clamp(int(x*328.f)-16, 0, 295);
        return varispeed_positive[i];
    }
    int i = std::min(399, int(x*400.f));
    i &= vsop == 0 ? ~1 : ~3;
    if (i > 199) return varispeed_magnitude[i-200];
    if (i <= 196) return -varispeed_magnitude[196-i];
    return 0.f;
}
inline uint32_t nextRandom(uint32_t& state) {
    state = state * 0x0bb38435u + 0x3619636bu;
    return state;
}
struct MorphChoice { float stereoCrossmix; int rateIndex; };
// Audited launch path 0x0802842a..0x08028490; same structure in dense path.
// Call at a launch only; this helper does not model other RNG consumers.
inline MorphChoice chooseMorph(float morph, uint32_t& state) {
    constexpr float scale = 0x1p-32f;
    const float test = float(nextRandom(state)) * (scale*.5f);
    float pitchRandom = float(nextRandom(state));
    float pan = 0.f;
    if (morph-.5f >= test) {
        pan = scale * pitchRandom;
        pitchRandom = float(nextRandom(state));
    }
    const float depth = (morph-0.6f) * (scale*9.999f);
    const int index = (morph > 0.6f) ? int(depth*pitchRandom) : 0;
    return {pan,index};
}
struct Stereo { float left, right; };
// Linear crossfeed between original/swapped stereo, not equal-power panning.
inline Stereo crossmix(Stereo x, float p) {
    const float q = 1.f-p;
    return {std::fma(p,x.right,q*x.left),std::fma(q,x.right,p*x.left)};
}
// Sparse branch 0x08028b5a..0x08028c32. Raw kernel DC gain is TWO.
// x[-1],x[0],x[1],x[2],fraction. Do not replace with a normalized cubic.
inline float sparseRead(float a, float b, float c, float d, float t) {
    const float sumOuter = a+d;
    const float sumAdjacent = a+c;
    const float curvature = (sumOuter-b)-c;
    const float slope = c-a;
    const float halfT = t*.5f;
    const float center = std::fma(sumAdjacent,.5f,b);
    const float adjustedSlope = std::fma(curvature,halfT,slope);
    return std::fma(adjustedSlope,t,center);
}
// Dense branch 0x08029f0c..0x08029f6a: linear read, doubled envelope gain.
inline float denseRead(float b, float c, float t, float envelope) {
    return std::fma(c-b,t,b) * (envelope+envelope);
}
inline int gainIndex(float envelopeSum) {
    return std::clamp(int((envelopeSum-1.f)*6.f)+3,0,33);
}
inline float playbackGain(float prior, float envelopeSum) {
    const float targetPart = morph_gain[gainIndex(envelopeSum)]*.01f;
    return std::fma(prior,.99f,targetPart);
}
inline Stereo sosMix(Stereo live, Stereo playback, float sos) {
    return {std::fma(playback.left-live.left,sos,live.left),
            std::fma(playback.right-live.right,sos,live.right)};
}
// Conditional record-side filter 0x08028eac..0x08028ef0.
// Caller must establish the firmware's splice-index equality condition.
struct RecordFilter {
    float previousScaledInput=0, previousOutput=0;
    float process(float input) {
        const float feedback = std::fma(previousOutput,.997f,-previousScaledInput);
        const float scaled = input*.7f;
        const float output = feedback+scaled;
        previousScaledInput=scaled;previousOutput=output;
        return output;
    }
};
} // namespace mg204
