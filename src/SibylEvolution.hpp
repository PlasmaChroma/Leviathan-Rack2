#pragma once

#include "SibylTypes.hpp"
#include <algorithm>

namespace sibyl {

// A pass is a scheduled pattern traversal, including automatic scene restarts.
// Explicit transport/adoption restarts reset this small per-channel cursor.
struct EvolutionCursor {
    uint64_t pass = 0;
    int64_t cycle = 0;
    bool seen = false;
    bool newTraversal = false;
    bool rebase = false;

    void observe(int64_t nominalStep, int length) {
        if (length <= 0) return;
        const int64_t nextCycle = nominalStep / length - (nominalStep % length < 0 ? 1 : 0);
        if (seen && !rebase && (newTraversal || nextCycle != cycle)) ++pass;
        seen = true;
        cycle = nextCycle;
        newTraversal = false;
        rebase = false;
    }
};

inline uint64_t evolutionHash(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

inline float evolutionSigned(uint64_t seed, uint64_t pass, int channel, int step, int lane) {
    const uint64_t key = evolutionHash(seed ^ 0x51ed270bULL) ^ evolutionHash(pass)
        ^ evolutionHash(uint64_t(channel) + 0x10000)
        ^ evolutionHash(uint64_t(step) + 0x20000)
        ^ evolutionHash(uint64_t(lane) + 0x30000);
    return float(evolutionHash(key) >> 40) * (2.f / 16777215.f) - 1.f;
}

struct EvolvedExpression {
    float probability = 1.f;
    float gate = 0.f;
    float velocity = 0.f;
    float glideMs = 0.f;
    float mod[3] {};
};

inline EvolvedExpression evolveExpression(const Pattern& pattern, const StepEvent& event,
        const TrackDef& track, uint64_t seed, uint64_t pass, int channel) {
    EvolvedExpression result;
    result.probability = event.hasProbability ? event.probability : 1.f;
    result.gate = event.hasGate ? event.gate : track.defaultGate;
    result.velocity = event.hasVelocity ? event.velocity : track.defaultVelocity;
    result.glideMs = event.glideMs;
    result.mod[0] = event.hasMod ? event.mod : 0.f;
    result.mod[1] = event.hasMod2 ? event.mod2 : 0.f;
    result.mod[2] = event.hasMod3 ? event.mod3 : 0.f;
    if (pass == 0 || !event.evolve || !pattern.evolution.enabled()) return result;
    const auto& amount = pattern.evolution;
    auto vary = [&](float base, float depth, int lane, float low, float high, int step) {
        if (depth == 0.f) return base;
        return std::max(low, std::min(high, base + depth * evolutionSigned(seed, pass, channel, step, lane)));
    };
    result.probability = vary(result.probability, amount.probability, 6, 0.f, 1.f, event.step);
    result.velocity = vary(result.velocity, amount.velocity, 0, 0.f, 1.f, event.step);
    // Keep zero-length authored gates zero; do not turn a silent event into a note.
    if (result.gate > 0.f)
        result.gate = vary(result.gate, amount.gate, 1, .01f, event.ratchets > 1 ? 1.f : 1024.f, event.step);
    result.glideMs = vary(result.glideMs, amount.glideMs, 2, 0.f, 3600000.f, event.step);
    // Each modulation lane shifts coherently over the whole pass, preserving
    // the relative authored contour. Velocity/gate/glide vary per event.
    for (int lane = 0; lane < 3; ++lane)
        result.mod[lane] = vary(result.mod[lane], amount.mod[lane], lane + 3, -10.f, 10.f, 0);
    return result;
}

} // namespace sibyl
