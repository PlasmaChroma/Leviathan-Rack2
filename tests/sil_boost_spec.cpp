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

// Exercise the whole chain: the new mono fundamental must survive the other
// adaptive stages, the limiter must still contain peaks, and warm bypass stays dry.
static void lowBandIntegration() {
    Sil m;
    m.inputs[Sil::INPUT_L_INPUT].channels = 1;
    m.inputs[Sil::INPUT_R_INPUT].channels = 1;
    for (float sr : {44100.f, 96000.f}) {
        rack::engine::Module::SampleRateChangeEvent event;
        event.sampleRate = sr; event.sampleTime = 1.f / sr;
        m.onSampleRateChange(event);
        for (bool bypass : {false, true}) {
            m.params[Sil::MASTERING_ENABLED_PARAM].setValue(bypass ? 0.f : 1.f);
            rack::engine::Module::ProcessArgs args;
            args.sampleRate = sr; args.sampleTime = 1.f / sr;
            double midSin = 0, midCos = 0, sideSin = 0, sideCos = 0;
            for (int i = 0; i < int(4 * sr); ++i) {
                args.frame = i;
                const double phase = 2.0 * M_PI * 40.0 * i / sr;
                const float x = 0.05f * std::sin(phase);
                m.inputs[Sil::INPUT_L_INPUT].setVoltage(x);
                m.inputs[Sil::INPUT_R_INPUT].setVoltage(-x);
                m.process(args);
                const float l = m.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
                const float r = m.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage();
                require(std::isfinite(l) && std::isfinite(r), "Nonfinite low-band module output");
                if (i >= int(3 * sr)) {
                    const double mid = 0.5 * (l + r), side = 0.5 * (l - r);
                    midSin += mid * std::sin(phase); midCos += mid * std::cos(phase);
                    sideSin += side * std::sin(phase); sideCos += side * std::cos(phase);
                }
            }
            const double mid = 2 * std::hypot(midSin, midCos) / sr;
            const double side = 2 * std::hypot(sideSin, sideCos) / sr;
            if (bypass) {
                require(mid < 1e-6 && std::fabs(side - 0.05) < 1e-5, "Warm bypass no longer passes dry stereo bass");
            } else {
                require(mid > 0.035 && side < 0.002, "Full chain did not restore a centered bass fundamental");
                require(std::fabs(m.lights[Sil::LOW_RECOVERY_LIGHT].getBrightness() - m.lowBandRecovery.feedbackAmount()) < 0.005f,
                    "Panel feedback does not track affected power");
            }
        }
        m.params[Sil::MASTERING_ENABLED_PARAM].setValue(1.f);
        rack::engine::Module::ProcessArgs args;
        args.sampleRate = sr; args.sampleTime = 1.f / sr;
        for (int i = 0; i < int(2 * sr); ++i) {
            args.frame = i;
            const float x = 20.f * std::sin(2.0 * M_PI * 40 * i / sr);
            m.inputs[Sil::INPUT_L_INPUT].setVoltage(x);
            m.inputs[Sil::INPUT_R_INPUT].setVoltage(-x);
            m.process(args);
            if (i > int(sr)) {
                require(std::fabs(m.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage()) <= m.limiterCeiling + 1e-5f
                    && std::fabs(m.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage()) <= m.limiterCeiling + 1e-5f,
                    "Recovered bass exceeds limiter ceiling");
            }
        }
    }
}

int main() {
    lowBandIntegration();
    for (float sr : {44100.f, 96000.f}) {
        for (float db : {0.f, 3.f, 6.f}) check(sr, .05f, db, false);
        check(sr, 5.f, 6.f, false);
        check(sr, .05f, 6.f, true);
    }
    std::cout << "Sil boost tests passed\n";
}

