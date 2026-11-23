#pragma once
#include "SilLevelMeter.hpp"
#include <cstdint>

namespace sil {
// UI requests are transient. Odd generations enable capture, and the next odd
// generation starts a fresh measurement. Only the audio thread owns trim/state.
struct Audition {
    std::atomic<uint32_t> request{0};
    std::atomic<bool> held{false};
    uint32_t generation = 0;
    bool capturing = false;
    int blockFrames = 1, fadeFrames = 1, remaining = 0;
    int frames = 0, accepted = 0;
    double filter[4][4]{}, energy[2]{}, total[2]{};
    float dryGain = 1.f, wetGain = 1.f, dryTarget = 1.f, wetTarget = 1.f;
    float dryStep = 0.f, wetStep = 0.f;

    void configure(float rate) {
        blockFrames = std::max(1, int(std::round(rate * .1f)));
        fadeFrames = std::max(1, int(std::round(rate * .01f)));
        generation = 0;
        capturing = false;
        held.store(false, std::memory_order_relaxed);
        dryGain = wetGain = dryTarget = wetTarget = 1.f;
        remaining = 0;
    }
    void fadeTo(float dry, float wet) {
        dryTarget = dry;
        wetTarget = wet;
        remaining = fadeFrames;
        dryStep = (dry - dryGain) / remaining;
        wetStep = (wet - wetGain) / remaining;
    }
    void poll() {
        auto next = request.load(std::memory_order_relaxed);
        if (next == generation) return;
        generation = next;
        capturing = (next & 1u) != 0;
        held.store(false, std::memory_order_relaxed);
        fadeTo(1.f, 1.f);
        if (capturing) {
            for (auto& channel : filter) for (double& v : channel) v = 0.;
            energy[0] = energy[1] = total[0] = total[1] = 0.;
            frames = accepted = 0;
        }
    }
    bool active() const { return (generation & 1u) || remaining; }
    void measure(const StereoLevelMeter& meter, float dl, float dr, float wl, float wr) {
        const float x[] = {dl, dr, wl, wr};
        for (int i = 0; i < 4; ++i) {
            double y = meter.weighted(.2 * x[i], filter[i]);
            energy[i / 2] += y * y;
        }
        if (++frames < blockFrames) return;
        // Ignore silent or effectively one-sided windows. Accumulate stereo
        // power over two seconds of usable program, then freeze the comparison.
        if (std::min(energy[0], energy[1]) > frames * 1e-7) {
            total[0] += energy[0];
            total[1] += energy[1];
            if (++accepted == 20) {
                double ratio = std::sqrt(total[0] / total[1]);
                fadeTo(float(std::min(1., 1. / ratio)), float(std::min(1., ratio)));
                capturing = false;
                held.store(true, std::memory_order_relaxed);
            }
        }
        frames = 0;
        energy[0] = energy[1] = 0.;
    }
    void advanceFade() {
        if (remaining && --remaining == 0) {
            dryGain = dryTarget;
            wetGain = wetTarget;
        } else if (remaining) {
            dryGain += dryStep;
            wetGain += wetStep;
        }
    }
};
}
