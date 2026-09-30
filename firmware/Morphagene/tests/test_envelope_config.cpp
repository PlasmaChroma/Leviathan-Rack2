#include "../reconstruction/audited_components.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
static void need(bool value, const char* why) {
    if (!value) throw std::runtime_error(why);
}
static unsigned bits(float value) {
    unsigned result; std::memcpy(&result, &value, sizeof(result)); return result;
}
int main() try {
    using mg204::geneEnvelopeConfig;
    const auto classic = geneEnvelopeConfig(4800.f, 2.f, .5f, false);
    need(classic.edgeSamples == 250.f && bits(classic.increment) == 0x3b83126fu,
         "classic overlap chooses firmware 250-frame edge");
    const auto smooth = geneEnvelopeConfig(4800.f, 2.f, .5f, true);
    need(smooth.edgeSamples == 2400.f && smooth.increment == 2.f * (1.f / 4800.f),
         "smooth overlap retains half-duration edge");
    need(geneEnvelopeConfig(4800.f, 1.f, 1.f, true).edgeSamples == 250.f,
         "seamless launch overrides smooth mode");
    need(geneEnvelopeConfig(4800.f, .5f, 2.f, true).edgeSamples == 250.f,
         "density below one overrides smooth mode");
    need(geneEnvelopeConfig(400.f, 2.f, .5f, false).edgeSamples == 200.f,
         "short overlapping gene retains half-duration edge even in classic mode");
    need(geneEnvelopeConfig(400.f, 1.f, 1.f, false).edgeSamples == 250.f,
         "seamless launch overrides short half-duration edge");
    const auto capped = geneEnvelopeConfig(96000.f, 2.f, .5f, true);
    need(capped.edgeSamples == 24000.f && bits(capped.increment) == 0x382ec33eu,
         "smooth envelope cap retains exact firmware reciprocal");
    std::cout << "PASS: MG204 envelope configuration branch transcription (not hardware oracle)\n";
} catch (const std::exception& error) {
    std::cerr << "FAIL: " << error.what() << '\n'; return 1;
}
