#include "Strand.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

static float wave(int t, int period) { return 3.f * std::sin(float(t) * 6.2831853f / period + .2f); }
int main() {
    Strand m;
    for (auto& output : m.outputs) output.channels = 1;
    assert(m.inputs.size() == 2 && m.outputs.size() == 1 && m.params.empty());
    rack::engine::Module::ProcessArgs args{};
    args.sampleRate = 48000.f; args.sampleTime = 1.f / args.sampleRate;
    m.inputs[Strand::A_INPUT].channels = 1;
    m.inputs[Strand::B_INPUT].channels = 1;
    bool hadNonZero = false;
    for (int t = 0; t < 2000; ++t) {
        m.inputs[Strand::A_INPUT].setVoltage(wave(t, 17));
        m.inputs[Strand::B_INPUT].setVoltage(wave(t, 23));
        m.process(args);
        assert(m.outputs[Strand::C_OUTPUT].getChannels() == 1);
        if (std::abs(m.outputs[Strand::C_OUTPUT].getVoltage()) > 0.1f) {
            hadNonZero = true;
        }
    }
    assert(hadNonZero);
    // Single source: disconnect B, keep A. Playback should continue using A.
    m.inputs[Strand::B_INPUT].channels = 0;
    bool aOnlyActive = false;
    for (int t = 0; t < 2000; ++t) {
        m.inputs[Strand::A_INPUT].setVoltage(wave(t, 17));
        m.process(args);
        if (std::abs(m.outputs[Strand::C_OUTPUT].getVoltage()) > 0.1f) {
            aOnlyActive = true;
        }
    }
    assert(aOnlyActive);
    // Disconnect both sources: finish pinned playback, then silence.
    for (auto& input : m.inputs) input.channels = 0;
    for (int t = 0; t < 100; ++t) m.process(args);
    assert(m.outputs[Strand::C_OUTPUT].getVoltage() == 0.f);
    rack::engine::Module::SampleRateChangeEvent rate{}; rate.sampleRate = 96000.f;
    m.onSampleRateChange(rate);
    assert(m.lane.limit == 9600. && m.lane.playing == -1);
    rack::engine::Module::ResetEvent reset{}; m.onReset(reset);
    assert(m.lane.limit == 9600. && m.connected[0] == -1);
    std::cout << "Strand Rack module tests passed\n";
}
