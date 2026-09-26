#pragma once

#include "ChimeraGeneSize.hpp"
#include <cmath>
#include <cstdio>

// Offline benchmark setup. Search the actual firmware mapping, including its
// duration floor, instead of applying the retired logarithmic inverse.
inline float chimeraBenchmarkGene(unsigned frames, double requested = 0.0) {
    int selected = 4095; // Default: shortest finite Gene for this Splice.
    if (requested > 0.0) {
        double error = std::fabs(chimera::geneSize::mapAdc(frames, selected).durationSamples - requested);
        for (int code = 0; code < 4095; ++code) {
            const double candidate = std::fabs(
                chimera::geneSize::mapAdc(frames, code).durationSamples - requested);
            if (candidate < error) { selected = code; error = candidate; }
        }
    }
    const float control = float(selected) / 4095.f;
    const auto mapped = chimera::geneSize::map(frames, control);
    std::printf("gene_adc=%d requested_frames=%.3f mapped_frames=%.3f integer_frames=%u\n",
        selected, requested, mapped.durationSamples, unsigned(std::lround(mapped.durationSamples)));
    return control;
}
