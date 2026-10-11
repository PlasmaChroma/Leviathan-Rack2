#pragma once
#include "plugin.hpp"
#include "StrandEngine.hpp"

struct Strand : Module {
    enum InputIds {
        A_INPUT,
        B_INPUT,
        INPUTS_LEN,
        A = A_INPUT,
        B = B_INPUT
    };
    enum OutputIds {
        C_OUTPUT,
        OUTPUTS_LEN,
        C = C_OUTPUT,
        OUT_OUTPUT = C_OUTPUT
    };
    strand::Lane lane;
    double sampleRate = 48000.;
    int connected[2] = {-1, -1};
    Strand() {
        config(0, INPUTS_LEN, OUTPUTS_LEN, 0);
        configInput(A_INPUT, "A");
        configInput(B_INPUT, "B");
        configOutput(C_OUTPUT, "C");
        lane.reset(48000.);
    }
    void onReset(const ResetEvent& e) override {
        Module::onReset(e);
        lane.reset(sampleRate);
        connected[0] = connected[1] = -1;
    }
    void onSampleRateChange(const SampleRateChangeEvent& e) override {
        sampleRate = e.sampleRate;
        lane.reset(sampleRate);
    }
    void process(const ProcessArgs&) override {
        float v[2] = {};
        for (int s = 0; s < 2; ++s) {
            bool isConn = inputs[s].isConnected();
            int connState = isConn ? 1 : 0;
            if (connState != connected[s]) {
                lane.sources[s].reset();
                lane.freqTrackers[s].reset(sampleRate);
                connected[s] = connState;
            }
            if (isConn) {
                v[s] = inputs[s].getVoltage();
            }
        }
        outputs[C_OUTPUT].setChannels(1);
        outputs[C_OUTPUT].setVoltage(lane.process(v[0], v[1]));
    }
    enum PortDisplayId {
        DISP_INPUT_A = 0,
        DISP_INPUT_B = 1,
        DISP_OUTPUT_C = 2
    };

    float getFrequency(int portDisplayIndex) const {
        if (portDisplayIndex == DISP_INPUT_A) return lane.freqTrackers[0].getFrequency();
        if (portDisplayIndex == DISP_INPUT_B) return lane.freqTrackers[1].getFrequency();
        if (portDisplayIndex == DISP_OUTPUT_C) {
            float fa = lane.freqTrackers[0].getFrequency();
            float fb = lane.freqTrackers[1].getFrequency();
            if (fa > 0.05f && fb > 0.05f) return (fa + fb) * 0.5f;
            if (fa > 0.05f) return fa;
            if (fb > 0.05f) return fb;
            return 0.f;
        }
        return 0.f;
    }
};
