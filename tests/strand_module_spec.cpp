#include "Strand.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

static float wave(int t, int period) { return 3.f * std::sin(float(t) * 6.2831853f / period + .2f); }
int main() {
    Strand m;
    for (auto& output : m.outputs) output.channels = 1;
    assert(m.inputs.size() == 4 && m.outputs.size() == 2 && m.params.empty());
    rack::engine::Module::ProcessArgs args{};
    args.sampleRate = 48000.f; args.sampleTime = 1.f / args.sampleRate;
    m.inputs[Strand::A_L].channels = 1;
    m.inputs[Strand::B_R].channels = 1;
    for (int t = 0; t < 2000; ++t) {
        m.inputs[Strand::A_L].setVoltage(wave(t, 17));
        m.inputs[Strand::B_R].setVoltage(wave(t, 23));
        m.process(args);
        assert(m.outputs[0].getChannels() == 1 && m.outputs[1].getChannels() == 1);
        assert(m.outputs[0].getVoltage() == m.outputs[1].getVoltage());
    }
    m.inputs[Strand::A_R].channels = 1;
    m.inputs[Strand::B_L].channels = 1;
    bool differs = false;
    for (int t = 0; t < 2000; ++t) {
        for (int i = 0; i < 4; ++i) m.inputs[i].setVoltage(wave(t, 13 + i * 4));
        m.process(args);
        differs |= std::abs(m.outputs[0].getVoltage() - m.outputs[1].getVoltage()) > .1f;
    }
    assert(differs);
    // Disconnect both sources: finish pinned playback, then silence.
    for (auto& input : m.inputs) input.channels = 0;
    for (int t = 0; t < 100; ++t) m.process(args);
    assert(m.outputs[0].getVoltage() == 0.f && m.outputs[1].getVoltage() == 0.f);
    rack::engine::Module::SampleRateChangeEvent rate{}; rate.sampleRate = 96000.f;
    m.onSampleRateChange(rate);
    assert(m.lanes[0].limit == 9600. && m.lanes[0].playing == -1);
    rack::engine::Module::ResetEvent reset{}; m.onReset(reset);
    assert(m.lanes[0].limit == 9600. && m.routing[0] == -1);
    std::cout << "Strand Rack module tests passed\n";
}
