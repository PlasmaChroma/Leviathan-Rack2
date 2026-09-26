#include "ChimeraGrains.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>

static void need(bool ok, const char* message) {
    if (!ok) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}
static bool near(float a, float b) {
    return std::fabs(a - b) <= std::max(0.001f, std::fabs(b) * 0.000002f);
}
int main() {
    using namespace chimera;
    using namespace chimera::geneSize;
    need(mapAdc(480000, 199).wholeSpliceMode &&
         mapAdc(8352000, 199).durationSamples == 8352000,
         "ADC 199 traverses the entire unfolded splice");
    need(!mapAdc(480000, 200).wholeSpliceMode &&
         mapAdc(480000, 200).durationSamples == 480000,
         "ADC 200 begins ordinary mapping at the base span, without an 8-second cap");
    need(near(mapAdc(480000, 4095).durationSamples, 480000.f * 0.00264940236f) &&
         near(mapAdc(480000, 4095).durationSamples, 1271.713f),
         "10-second splice reaches 26.494 milliseconds at full CW");
    need(foldedBase(576000) == 576000 && foldedBase(576001) == 288000.5f &&
         mapAdc(576001, 200).durationSamples == 288000.5f,
         "12-second threshold preserves the firmware folding discontinuity");
    need(foldedBase(2304001) == 288000.125f, "repeated power-of-two folding");
    need(adc(-1.f) == 0 && adc(2.f) == 4095 && adc(199.f/4095.f) == 199 &&
         adc(200.f/4095.f) == 200, "normalized ADC conversion and endpoints");
    for (int code = 200; code <= 4095; ++code) {
        need(mapAdc(3, code).durationSamples >= 8.f, "eight output-sample floor");
        need(mapAdc(480000, code).durationSamples ==
             mapAdc(480000, code & ~3).durationSamples, "four-code LUT plateaus");
        const float expected = 480000.f * std::exp2(float(150 - 3*(code/4)) / 341.f);
        need(near(mapAdc(480000, code).durationSamples, expected), "firmware curve across every ordinary code");
    }
    need(quantize(800.f, 1200) == 1200.f && quantize(799.f, 1200) == 600.f &&
         near(quantize(599.f, 1200), 400.f) && quantize(399.f, 1200) == 300.f &&
         near(quantize(299.f, 1200), 200.f) && quantize(199.f, 1200) == 150.f,
         "clock quantizes to the binary/ternary subdivision family");
    need(mapAdc(1152000, 200, true).durationSamples == 576000.f,
         "clock quantization uses original unfolded splice length");
    const std::uint32_t twoThirdBits = 0x3f2aaa9fU;
    float twoThirds;
    std::memcpy(&twoThirds, &twoThirdBits, sizeof(twoThirds));
    need(quantize(twoThirds * 1200.f, 1200) == 1200.f &&
         quantize(std::nextafter(twoThirds * 1200.f, 0.f), 1200) == 600.f,
         "clock threshold uses the exact firmware float constant");

    Reel reel(40, 40);
    for (unsigned i = 0; i < 10000; ++i) need(reel.write(i, {1, 1}, i), "source fixture");
    CoreOutput c{};
    c.gene = 1564.f/4095.f; // Exactly one eighth before output-frame rounding.
    c.morph = 1.f/6.f;
    for (float rate : {0.5f, 1.f, 2.f, -0.5f, -1.f, -2.f}) {
        Grains grains;
        c.rate = rate;
        const unsigned duration = 1250;
        double position = 0;
        for (unsigned i = 0; i <= duration; ++i) {
            auto out = grains.step(reel, {0, 10000}, c);
            need(out.primaryBoundary == (i == duration), "output duration is independent of speed and direction");
            if (i == duration - 1) position = out.primaryPosition;
        }
        const double expected = profile1::wrapPosition((rate < 0 ? 9999.0 : 0.0) +
            rate * duration, {0, 10000});
        need(std::fabs(position - expected) < 1e-5, "source distance equals signed increment times output duration");
        for (unsigned i = 0; i <= 625; ++i) {
            auto out = grains.step(reel, {0, 5000}, c);
            need(out.primaryBoundary == (i == 625), "splice length change remaps without knob movement");
        }
    }
    for (float rate : {1.f, -1.f}) {
        Grains grains;
        c.rate = rate;
        c.gene = 199.f / 4095.f;
        for (unsigned i = 0; i <= 10000; ++i) {
            auto out = grains.step(reel, {0, 10000}, c, false, true);
            need(out.primaryBoundary == (i == 10000), "whole-splice mode traverses and wraps the complete region");
        }
    }
    c.rate = 1.f;
    c.gene = 1712.f / 4095.f;
    for (int state = 0; state < 3; ++state) {
        Grains grains;
        const bool valid = state == 1;
        const unsigned duration = profile1::finiteGeneFrames(10000, c.gene, valid);
        need(duration == (valid ? 833u : 998u), "clock integration duration fixture");
        const Grains::ClockDrive clock(1, false, state ? 2400 : 0, state == 2);
        for (unsigned i = 0; i <= duration; ++i) {
            const auto out = grains.step(reel, {0, 10000}, c, false, false, false, 0, clock);
            need(out.primaryBoundary == (i == duration),
                 "only a valid non-waiting external clock quantizes ordinary duration");
        }
    }
    std::puts("PASS: MG204 Gene mapping, endpoints, folding, plateaus, clock subdivisions and speed-independent duration");
}
