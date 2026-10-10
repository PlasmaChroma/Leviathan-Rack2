#include "StrandEngine.hpp"
#include <cassert>
#include <iostream>
#include <limits>
#include <memory>

float wave(int t, int period, float amplitude) {
    return amplitude * std::sin(float(t) * 6.28318530718f / period + .37f);
}
int main() {
    auto lane = std::unique_ptr<strand::Lane>(new strand::Lane);
    lane->reset(48000.);
    for (int t = 0; t < 400; ++t) lane->process(wave(t, 13, 2.f), wave(t, 19, 4.f));
    for (int s = 0; s < 2; ++s) {
        auto& source = lane->sources[s];
        assert(source.latest >= 0);
        auto& c = source.cycles[source.latest];
        assert(std::abs(c.duration - (s == 0 ? 13. : 19.)) < .001);
        assert(c.at(0.) == 0.f && c.at(c.duration) == 0.f);
        assert(c.first > 0. && c.first < 1.);
    }
    // Frozen publications must alternate with original fractional durations.
    for (auto& source : lane->sources) source.writing = -1;
    double total = 0.; int changes = 0;
    for (int t = 0; t < 32000; ++t) {
        int prior = lane->playing;
        float result = lane->process(0.f, 0.f);
        assert(std::isfinite(result) && std::abs(result) <= 4.01f);
        total += 1.;
        if (lane->playing != prior) ++changes;
    }
    assert(changes >= 1999 && changes <= 2001);
    assert(total == 32000.);
    // Compare many fractional handoffs against an independent timeline.
    lane->reset(48000.);
    for (int i = 0; i < 2; ++i) {
        auto& c = lane->sources[i].cycles[0];
        c.count = 3; c.first = .25; c.duration = i == 0 ? 3.25 : 4.5;
        c.samples[0] = 1.f; c.samples[1] = -1.f; c.samples[2] = -.5f;
        lane->sources[i].latest = 0;
    }
    for (int t = 1; t <= 31000; ++t) {
        lane->process(0.f, 0.f);
        double phase = std::fmod(double(t), 7.75);
        int expected = phase < 3.25 ? 0 : 1;
        assert(lane->playing == expected);
        assert(std::abs(lane->position - (expected == 0 ? phase : phase - 3.25)) < 1e-9);
    }
    // Selected storage stays untouched through capture refreshes.
    lane->reset(48000.);
    for (int t = 0; t < 120; ++t) lane->process(wave(t, 13, 2.f), wave(t, 19, 4.f));
    int sourceId = lane->playing;
    auto& source = lane->sources[sourceId];
    int pinned = source.pinned;
    float saved = source.cycles[pinned].samples[0];
    for (int t = 120; t < 125 && lane->playing == sourceId; ++t) {
        lane->process(wave(t, 4, 8.f), wave(t, 4, 9.f));
        assert(source.cycles[pinned].samples[0] == saved);
    }
    // Mono normalization can run identical independent lanes deterministically.
    auto other = std::unique_ptr<strand::Lane>(new strand::Lane);
    lane->reset(44100.); other->reset(44100.);
    for (int t = 0; t < 4000; ++t)
        assert(lane->process(wave(t, 23, 2.f), wave(t, 17, 3.f)) == other->process(wave(t, 23, 2.f), wave(t, 17, 3.f)));
    lane->reset(48000.);
    for (int t = 0; t < 6000; ++t) assert(lane->process(1.f, 0.f) == 0.f);
    assert(lane->sources[0].latest == -1);
    lane->process(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity());
    assert(lane->playing == -1);
    // Zero plateaus need a negative excursion and an intervening fall.
    lane->reset(48000.);
    const float plateau[] = {-1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f};
    for (float x : plateau) lane->process(x, 0.f);
    assert(lane->sources[0].latest >= 0);
    assert(lane->sources[0].cycles[lane->sources[0].latest].duration == 7.);
    lane->reset(48000.);
    lane->process(-1.f, 0.f); lane->process(1.f, 0.f);
    for (int t = 0; t < 5000; ++t) lane->process(1.f, 0.f);
    assert(lane->sources[0].writing >= 0 && lane->sources[0].latest >= 0);
    const auto& forced = lane->sources[0].cycles[lane->sources[0].latest];
    assert(forced.duration == 4800. && forced.forcedEnd);
    assert(forced.at(forced.duration) == 0.f);
    assert(forced.at(forced.duration - 12.) < forced.at(forced.duration - 24.));
    // Sub-audio sources refresh and switch, rather than retaining an old cycle.
    lane->reset(48000.);
    int forcedCount = 0, switches = 0, prior = -1;
    for (int t = 0; t < 48000; ++t) {
        float result = lane->process(wave(t, 20000, 2.f), wave(t, 24000, 4.f));
        assert(std::isfinite(result) && std::abs(result) <= 4.01f);
        if (lane->playing != prior) { ++switches; prior = lane->playing; }
        for (auto& source : lane->sources) if (source.latest >= 0) {
            const auto& c = source.cycles[source.latest];
            assert(c.duration <= 4800.);
            if (c.forcedEnd) ++forcedCount;
        }
    }
    assert(forcedCount > 0 && switches >= 6);
    std::cout << "Strand engine tests passed\n";
}
