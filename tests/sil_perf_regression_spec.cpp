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

// Deterministic program-like stimulus: low tone, mid tones, a slow amplitude swell and noise.
struct Stimulus {
    uint32_t rng = 12345u;
    float noise() { rng = rng * 1664525u + 1013904223u; return float(int32_t(rng)) * (1.f / 2147483648.f); }
    void sample(int i, float sr, float level, float& l, float& r) {
        const float t = float(i) / sr;
        const float swell = 0.55f + 0.45f * std::sin(2.f * float(M_PI) * 0.7f * t);
        const float tau = 2.f * float(M_PI);
        const float bass = 0.5f * std::sin(tau * 55.f * t);
        const float mid = 0.25f * std::sin(tau * 440.f * t) + 0.18f * std::sin(tau * 1250.f * t + 0.4f);
        const float hi = 0.08f * std::sin(tau * 6200.f * t) + 0.06f * noise();
        const float side = 0.2f * std::sin(tau * 3100.f * t + 1.1f) + 0.1f * noise();
        l = level * swell * (bass + mid + hi + side);
        r = level * swell * (bass + 0.9f * mid + hi - side);
    }
};

static void setup(Sil& m, float sr) {
    rack::engine::Module::SampleRateChangeEvent event;
    event.sampleRate = sr; event.sampleTime = 1.f / sr;
    m.onSampleRateChange(event);
    m.inputs[Sil::INPUT_L_INPUT].channels = 1;
    m.inputs[Sil::INPUT_R_INPUT].channels = 1;
    m.outputs[Sil::OUTPUT_L_OUTPUT].channels = 1;
    m.outputs[Sil::OUTPUT_R_OUTPUT].channels = 1;
    m.params[Sil::MASTERING_ENABLED_PARAM].setValue(1.f);
}

struct Golden { const char* name; float sr; float level; float secs; float rmsMidDb; float rmsSideDb; float peakDb; };

// Reference captured after the isolated-stage detector/dynamics/limiter audit.
// Named detector bands and dynamics now match the intended processing; the
// stage behavior suite independently checks them before these chain references.
// Recaptured for the 1 ms limiter attack ramp (loud rows) and the saturator
// wet fade-in (quiet row).
static const Golden kGolden[] = {
    {"loud48",  48000.f, 12.f, 6.f, 1.542f, -5.863f, 12.479f},
    {"quiet48", 48000.f, 0.6f, 6.f, -12.431f, -19.663f, 3.103f},
    {"loud96",  96000.f, 12.f, 5.f, 2.654f, -4.783f, 12.479f},
};

static void measure(const Golden& g, float& rmsMidDb, float& rmsSideDb, float& peakDb) {
    Sil m; setup(m, g.sr);
    Stimulus st;
    rack::engine::Module::ProcessArgs args; args.sampleRate = g.sr; args.sampleTime = 1.f / g.sr;
    const int n = int(g.sr * g.secs), tail = int(g.sr);
    double midE = 0, sideE = 0; float peak = 0;
    for (int i = 0; i < n; ++i) {
        args.frame = i;
        float l, r; st.sample(i, g.sr, g.level, l, r);
        m.inputs[Sil::INPUT_L_INPUT].setVoltage(l);
        m.inputs[Sil::INPUT_R_INPUT].setVoltage(r);
        m.process(args);
        const float a = m.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
        const float b = m.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage();
        require(std::isfinite(a) && std::isfinite(b), "Nonfinite output");
        peak = std::max(peak, std::max(std::fabs(a), std::fabs(b)));
        if (i >= n - tail) { const double mid = 0.5 * (a + b), side = 0.5 * (a - b); midE += mid * mid; sideE += side * side; }
    }
    rmsMidDb = 10.f * std::log10(float(midE / tail) + 1e-20f);
    rmsSideDb = 10.f * std::log10(float(sideE / tail) + 1e-20f);
    peakDb = 20.f * std::log10(peak + 1e-12f);
}

static void goldenTest(bool print) {
    for (const Golden& g : kGolden) {
        float mid, side, peak; measure(g, mid, side, peak);
        if (print) { std::printf("    {\"%s\", %.0f.f, %.1ff, %.1ff, %.3ff, %.3ff, %.3ff},\n", g.name, g.sr, g.level, g.secs, mid, side, peak); continue; }
        std::printf("  %-8s mid %.3f dB (golden %.3f)  side %.3f dB (golden %.3f)  peak %.3f dB (golden %.3f)\n",
            g.name, mid, g.rmsMidDb, side, g.rmsSideDb, peak, g.peakDb);
        require(std::fabs(mid - g.rmsMidDb) < 0.05f, "Mid RMS drifted from reference");
        require(std::fabs(side - g.rmsSideDb) < 0.05f, "Side RMS drifted from reference");
        require(std::fabs(peak - g.peakDb) < 0.10f, "Peak drifted from reference");
    }
}

// 20 s of exact silence puts the module on the idle path. The output must be exactly zero while
// idle, and the audio after the gap must match the same module with idling disabled.
static void idleTest() {
    const float sr = 48000.f;
    Sil idleM, fullM;
    setup(idleM, sr); setup(fullM, sr);
    fullM.idleEntrySamples = 0xFFFFFFFFu;
    Stimulus a, b;
    rack::engine::Module::ProcessArgs args; args.sampleRate = sr; args.sampleTime = 1.f / sr;
    const int pre = int(sr * 4), gap = int(sr * 20), post = int(sr * 4);
    double errE = 0, refE = 0; bool sawIdle = false;
    for (int i = 0; i < pre + gap + post; ++i) {
        args.frame = i;
        float l = 0.f, r = 0.f, l2 = 0.f, r2 = 0.f;
        const bool play = (i < pre) || (i >= pre + gap);
        if (play) { a.sample(i, sr, 8.f, l, r); b.sample(i, sr, 8.f, l2, r2); }
        idleM.inputs[Sil::INPUT_L_INPUT].setVoltage(l); idleM.inputs[Sil::INPUT_R_INPUT].setVoltage(r);
        fullM.inputs[Sil::INPUT_L_INPUT].setVoltage(l2); fullM.inputs[Sil::INPUT_R_INPUT].setVoltage(r2);
        idleM.process(args); fullM.process(args);
        const float x = idleM.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
        const float y = fullM.outputs[Sil::OUTPUT_L_OUTPUT].getVoltage();
        require(std::isfinite(x), "Nonfinite output");
        if (idleM.idle) {
            sawIdle = true;
            require(x == 0.f && idleM.outputs[Sil::OUTPUT_R_OUTPUT].getVoltage() == 0.f, "Idle output not exactly zero");
        }
        if (i >= pre + gap) { errE += double(x - y) * double(x - y); refE += double(y) * double(y); }
    }
    require(sawIdle, "Module never entered the idle path");
    require(!idleM.idle, "Module did not leave the idle path on audio");
    const double nullDb = 10.0 * std::log10(errE / refE + 1e-30);
    std::printf("  idle wake null: %.1f dB\n", nullDb);
    require(nullDb < -60.0, "Audio after an idle gap differs from the non-idle result");
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--print-golden") { goldenTest(true); return 0; }
    goldenTest(false);
    idleTest();
    std::cout << "Sil perf regression tests passed\n";
}
