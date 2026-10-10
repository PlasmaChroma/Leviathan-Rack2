#pragma once
#include "plugin.hpp"
#include "StrandEngine.hpp"

struct Strand : Module {
    enum InputIds { A_L, A_R, B_L, B_R, INPUTS_LEN };
    enum OutputIds { C_L, C_R, OUTPUTS_LEN };
    std::array<strand::Lane, 2> lanes;
    double sampleRate = 48000.;
    int routing[2] = {-1, -1};
    Strand() {
        config(0, INPUTS_LEN, OUTPUTS_LEN, 0);
        configInput(A_L, "A left / mono"); configInput(A_R, "A right / mono");
        configInput(B_L, "B left / mono"); configInput(B_R, "B right / mono");
        configOutput(C_L, "C left"); configOutput(C_R, "C right");
        for (auto& lane : lanes) lane.reset(48000.);
    }
    void onReset(const ResetEvent& e) override {
        Module::onReset(e);
        for (auto& lane : lanes) lane.reset(sampleRate);
        routing[0] = routing[1] = -1;
    }
    void onSampleRateChange(const SampleRateChangeEvent& e) override {
        sampleRate = e.sampleRate;
        for (auto& lane : lanes) lane.reset(sampleRate);
    }
    void process(const ProcessArgs&) override {
        float v[2][2] = {};
        for (int s = 0; s < 2; ++s) {
            int l = s * 2, r = l + 1;
            int mask = int(inputs[l].isConnected()) | (int(inputs[r].isConnected()) << 1);
            if (mask != routing[s]) {
                for (auto& lane : lanes) lane.sources[s].reset();
                routing[s] = mask;
            }
            if (mask == 0) continue;
            v[s][0] = inputs[mask == 2 ? r : l].getVoltage();
            v[s][1] = inputs[mask == 1 ? l : r].getVoltage();
        }
        for (int i = 0; i < 2; ++i) {
            outputs[i].setChannels(1);
            outputs[i].setVoltage(lanes[i].process(v[0][i], v[1][i]));
        }
    }
};
