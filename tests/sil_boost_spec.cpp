#include "../src/plugin.hpp"

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

Plugin* pluginInstance = nullptr;

bool testDragonKingDebugEnabled = false;
bool isDragonKingDebugEnabled() {
	return testDragonKingDebugEnabled;
}

static bool previewOptionsEnabled = true;
bool isDragonKingPreviewWidgetOptionsEnabled() {
	return previewOptionsEnabled;
}

bool isModuleTeardownLoggingEnabled() {
	return false;
}

void refreshDragonKingDebugEnabled() {
}

ModuleTeardownTimer::ModuleTeardownTimer(const char* moduleName)
	: moduleName(moduleName) {
}

void ModuleTeardownTimer::begin(int moduleId) {
	this->moduleId = moduleId;
}

ModuleTeardownTimer::~ModuleTeardownTimer() {
}

#define SIL_HEADLESS_TEST 1
#include "../src/sil/Sil.cpp"

static void require(bool ok, const char* message) {
    if (!ok) { std::cerr << message << "\n"; std::exit(1); }
}

static void check(float sr, float amplitude, float boost, bool bypass) {
    Sil baseline, raised;
    rack::engine::Module::SampleRateChangeEvent event;
    event.sampleRate = sr; event.sampleTime = 1.f / sr;
    baseline.onSampleRateChange(event); raised.onSampleRateChange(event);
    raised.params[Sil::BOOST_PARAM].setValue(boost);
    for (Sil* module : {&baseline, &raised}) {
        module->inputs[Sil::INPUT_L_INPUT].channels = 1;
        module->outputs[Sil::OUTPUT_L_OUTPUT].channels = 1;
        module->outputs[Sil::OUTPUT_R_OUTPUT].channels = 1;
        module->params[Sil::MASTERING_ENABLED_PARAM].setValue(bypass ? 0.f : 1.f);
    }
    double baseEnergy = 0, raisedEnergy = 0;
    float peak = 0;
    rack::engine::Module::ProcessArgs args;
    args.sampleRate = sr; args.sampleTime = 1.f / sr;
    for (int i = 0; i < int(sr * 3); ++i) {
        args.frame = i;
        float input = amplitude * std::sin(2.f * float(M_PI) * 997.f * i / sr);
        baseline.inputs[Sil::INPUT_L_INPUT].setVoltage(input);
        raised.inputs[Sil::INPUT_L_INPUT].setVoltage(input);
        baseline.process(args); raised.process(args);
        const float a = baseline.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
        const float b = raised.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
        require(std::isfinite(b), "Nonfinite output");
        peak = std::max(peak, std::fabs(b));
        if (i > int(sr * 2)) { baseEnergy += a*a; raisedEnergy += b*b; }
    }
    require(baseEnergy > 1e-8, "Silent test output");
    require(std::fabs(baseline.saturator.makeupDb - raised.saturator.makeupDb) < 1e-6f,
        "Adaptive makeup counteracts boost");
    require(std::fabs(baseline.limiterRecentGrDb - raised.limiterRecentGrDb) < 1e-6f,
        "Adaptive feedback depends on boost");
    if (amplitude < 1.f || bypass) {
        double expected = bypass ? 1.0 : std::pow(10.0, boost / 20.0);
        require(std::fabs(std::sqrt(raisedEnergy / baseEnergy) - expected) < .002,
            "Quiet signal or bypass gain incorrect");
    } else {
        require(peak <= raised.limiterCeiling + 1e-5f, "Limiter ceiling exceeded");
        require(raised.limiterGain < 1.f, "Loud signal did not exercise limiter");
    }
}

int main() {
    for (float sr : {44100.f, 96000.f}) {
        for (float db : {0.f, 3.f, 6.f}) check(sr, .05f, db, false);
        check(sr, 5.f, 6.f, false);
        check(sr, .05f, 6.f, true);
    }
    std::cout << "Sil boost tests passed\n";
}

