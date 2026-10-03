#include "Tiamat/TiamatControls.hpp"
#include "Tiamat/TiamatCorruptTables.hpp"
#include "Tiamat/TiamatRandom.hpp"

#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

using namespace tiamat;

static std::uint32_t bits(float value) {
    std::uint32_t result;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

// Only the exp/pow-backed control fixtures permit two float32 ULPs: their
// oracle explicitly substituted Python libm. Tables, RNG, snap decisions and
// Tone initialization remain exact. This is not an audio-vector tolerance.
static void closeControl(float actual, float expected) {
    const auto a = bits(actual), e = bits(expected);
    if ((a > e ? a - e : e - a) > 2) {
        std::cerr << "control mismatch: " << actual << " != " << expected << '\n';
        assert(false);
    }
}

template <std::size_t N>
static void table(std::istringstream& row, const std::array<float, N>& values) {
    for (float v : values) {
        std::uint32_t expected = 0;
        assert(bool(row >> expected));
        assert(bits(v) == expected);
    }
}

static void fixtures(const char* path) {
    std::ifstream input(path);
    assert(input && "missing Tiamat fixture");
    std::string line;
    unsigned rngCount = 0, tableCount = 0, microCount = 0, timeCount = 0, toneCount = 0;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream row(line);
        std::string kind;
        row >> kind;
        if (kind == "RNG") {
            std::uint64_t seed, finalState;
            unsigned draws;
            assert(bool(row >> seed >> draws >> finalState));
            Random rng(seed), independent(seed);
            for (unsigned i = 0; i < draws; ++i) {
                const auto value = rng.next();
                if (i < 12) {
                    std::uint32_t expected;
                    assert(bool(row >> expected));
                    assert(value == expected);
                }
                assert(independent.unit255() == float(value % 255) / 255.f);
            }
            assert(rng.state() == finalState && independent.state() == finalState);
            rng.restart(seed);
            assert(rng.state() == seed);
            ++rngCount;
        }
        else if (kind == "TABLE") {
            std::string name;
            row >> name;
            if (name == "micro_octave_targets") table(row, octaveTargets);
            else if (name == "external_clock_ratios") table(row, clockRatios);
            else if (name == "macro_bend_rate_table") table(row, bendRates);
            else if (name == "macro_break_silence_table") table(row, breakSilences);
            else if (name == "decimate_bitcrush_table") table(row, decimateBits);
            else if (name == "decimate_downsample_table") table(row, decimateRates);
            else assert(false);
            ++tableCount;
        }
        else if (kind == "MICRO") {
            float knob, volts, expected;
            bool reverse, octave, hit = false;
            assert(bool(row >> knob >> volts >> reverse >> expected >> octave));
            closeControl(microRate(knob, volts, reverse, &hit), expected);
            assert(hit == octave);
            ++microCount;
        }
        else if (kind == "TIME") {
            float x, frequency, ratio;
            assert(bool(row >> x >> frequency >> ratio));
            closeControl(internalFrequency(x), frequency);
            assert(externalRatio(x) == ratio);
            ++timeCount;
        }
        else if (kind == "TONE") {
            std::uint32_t feedforward, feedback;
            assert(bool(row >> feedforward >> feedback));
            const auto tone = outputToneCoefficients();
            assert(bits(tone.feedforward) == feedforward);
            assert(bits(tone.feedback) == feedback);
            assert(tone.feedforward + tone.feedback == 1.f);
            ++toneCount;
        }
        else assert(false && "unknown fixture record");
        std::string extra;
        assert(!(row >> extra) && "trailing fixture fields");
    }
    assert(rngCount == 4 && tableCount == 6 && microCount == 30 && timeCount == 19 && toneCount == 1);
    std::cout << "Fixtures: 4 seeded 256-draw streams, 6 exact tables, " << microCount
              << " Micro maps, " << timeCount << " Time maps, exact Tone coefficients\n";
}

static void boundaries() {
    for (bool second : {false, true}) {
        for (unsigned i = 0; i < octaveTargets.size(); ++i) {
            const float target = octaveTargets[i];
            const float delta = target * (i == 0 ? .15f : (second ? .015f : .025f));
            for (float edge : {target - delta, target + delta}) {
                bool hit = true;
                assert(snapOctave(edge, second, &hit) == edge && !hit);
                const float inside = std::nextafter(edge, target);
                assert(snapOctave(inside, second, &hit) == target && hit);
            }
        }
    }
    for (unsigned i = 0; i < clockRatios.size(); ++i)
        assert(externalRatio((float(i) + .25f) / 8.25f) == clockRatios[i]);
    assert(externalRatio(1.f) == 8.f);
    assert(microRate(.5f, 1.f, false) == 2.f);
    assert(microRate(.5f, -1.f, true) == -.5f);
    assert(microRate(.5f, 5.f, false) == 32.f); // reader clamp is later
    assert(microRate(.5f, -20.f, true) < 0.f);
    assert(std::isfinite(microRate(1.f, std::numeric_limits<float>::max(), false)));
    assert(bufferFrequencyTarget(.5) == coefficientRate / 24000.f);
    assert(bufferFrequencyTarget(100.) == coefficientRate / float(maxCaptureFrames));
    assert(bufferFrequencyTarget(0.) == coefficientRate);
}

static void mappings() {
    PrimaryControls p;
    ControlVoltages cv;
    SecondarySettings settings;
    EffectiveFlags flags;
    ControlMapper mapper;
    auto out = mapper.map(p, cv, settings, Mode::Macro, flags);
    assert(out.repeatsExponent == 0 && out.baseRate == 1 && out.mixTarget == 1);
    assert(out.macroBend == 0 && out.macroBreak == 0 && out.crossfeed == 0);
    assert(std::abs(out.windowSquared - .02f) < 2e-9f);
    for (unsigned i = 0; i <= 8; ++i) {
        // Transport via CV gives exact binary effective endpoints.
        p.repeats = 0;
        cv.repeats = float(i) * .625f;
        mapper.map(p, cv, settings, Mode::Macro, flags);
        out = mapper.map(p, cv, settings, Mode::Macro, flags);
        assert(out.repeatsExponent == i);
    }
    p.repeats = 1; cv.repeats = 0; mapper.reset();
    assert(mapper.map(p, cv, settings, Mode::Macro, flags).repeatsExponent == 0);
    assert(mapper.map(p, cv, settings, Mode::Macro, flags).repeatsExponent == 8);
    p.mix = 0; flags.freeze = true;
    assert(mapper.map(p, cv, settings, Mode::Macro, flags).mixTarget == 1);
    flags.freeze = false;
    assert(mapper.map(p, cv, settings, Mode::Macro, flags).mixTarget == 0);
    p.bend = .5f; cv.bend = 1; settings.bendDepth = 0;
    out = mapper.map(p, cv, settings, Mode::Micro, flags);
    assert(out.baseRate == 2);
    p.brk = 1; flags.brk = true;
    out = mapper.map(p, cv, settings, Mode::Micro, flags);
    assert(out.manualSilence == 1 && out.traverse == 0);
    flags.brk = false;
    out = mapper.map(p, cv, settings, Mode::Micro, flags);
    assert(out.manualSilence == 0 && out.traverse == 1);
    out = mapper.map(p, cv, settings, Mode::Macro, flags);
    assert(out.baseRate == 1 && out.manualSilence == 0 && out.traverse == 0);
    p.time = std::numeric_limits<float>::quiet_NaN();
    cv.bend = std::numeric_limits<float>::infinity();
    out = mapper.map(p, cv, settings, Mode::Micro, flags);
    assert(out.time == 0 && out.baseRate == 1);
    ControlState state;
    assert(!state.macroBend && !state.macroBreak && !state.microReverse && !state.microSilence);
    assert(!state.buttonFreeze && state.seed == 1 && state.effect == Effect::Decimate);
    assert(planeFrames * 2u * sizeof(float) == 28808400u);
}

int main(int argc, char** argv) {
    fixtures(argc > 1 ? argv[1] : "tests/fixtures/tiamat/reference_v1.txt");
    boundaries();
    mappings();
    std::cout << "Tiamat Phase 1 reference tests passed\n";
}
