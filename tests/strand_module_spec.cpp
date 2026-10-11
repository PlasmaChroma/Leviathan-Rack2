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
    for (int t = 0; t < 48000; ++t) {
        m.inputs[Strand::A_INPUT].setVoltage(wave(t, 48));
        m.inputs[Strand::B_INPUT].setVoltage(wave(t, 96));
        m.process(args);
        assert(m.outputs[Strand::C_OUTPUT].getChannels() == 1);
        if (std::abs(m.outputs[Strand::C_OUTPUT].getVoltage()) > 0.1f) {
            hadNonZero = true;
        }
    }
    assert(hadNonZero);
    // Frequency statistics: period 48 -> 1000 Hz, period 96 -> 500 Hz, output average -> 750 Hz
    float fa = m.getFrequency(Strand::DISP_INPUT_A);
    float fb = m.getFrequency(Strand::DISP_INPUT_B);
    float fc = m.getFrequency(Strand::DISP_OUTPUT_C);
    assert(std::abs(fa - 1000.f) < 5.f);
    assert(std::abs(fb - 500.f) < 5.f);
    assert(std::abs(fc - 750.f) < 5.f);

    // Single source: disconnect B, keep A. Playback should continue using A, output freq should equal A.
    m.inputs[Strand::B_INPUT].channels = 0;
    bool aOnlyActive = false;
    for (int t = 0; t < 2000; ++t) {
        m.inputs[Strand::A_INPUT].setVoltage(wave(t, 48));
        m.process(args);
        if (std::abs(m.outputs[Strand::C_OUTPUT].getVoltage()) > 0.1f) {
            aOnlyActive = true;
        }
    }
    assert(aOnlyActive);
    assert(m.getFrequency(Strand::DISP_INPUT_B) == 0.f);
    assert(std::abs(m.getFrequency(Strand::DISP_OUTPUT_C) - 1000.f) < 5.f);
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
