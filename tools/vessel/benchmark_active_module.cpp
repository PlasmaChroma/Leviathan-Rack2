// Whole audio callback benchmark; keep before/after executables for paired runs.
// Run serially without builds or other benchmarks. Includes controls/telemetry,
// excludes Rack scheduling, GUI and driver. Debug timing is disabled.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../../src/Vessel.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>

bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel = nullptr;
Model* modelVTune = nullptr;
namespace rack { Context::~Context() {} }

static void hashFloat(std::uint64_t& hash, float value) {
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    hash = (hash ^ bits) * UINT64_C(1099511628211);
}

int main() {
    rack::Context context; rack::contextSet(&context);
    int result = 0;
    {
        rack::engine::Engine engine; context.engine = &engine;
        try {
            std::cout << "bowl,quality,separation,coupled,mean_us,fingerprint\n" << std::setprecision(10);
            for (int bowl : {1, 0}) for (int quality : {1, 2})
            for (float separation : {0.f, 33.f}) for (bool coupled : {false, true}) {
                double elapsed = 0;
                std::uint64_t hash = UINT64_C(14695981039346656037);
                for (int repeat = 0; repeat < 3; ++repeat) {
                    Vessel m;
                    m.params[Vessel::BOWL_PARAM].setValue(float(bowl));
                    m.params[Vessel::MALLET_PARAM].setValue(bowl ? 0.f : 1.f);
                    m.params[Vessel::SPEED_PARAM].setValue(.2f);
                    m.params[Vessel::BINAURAL_PARAM].setValue(separation);
                    m.requestedQuality.store(quality);
                    m.inputs[Vessel::ROTATE_INPUT].channels = 1;
                    m.inputs[Vessel::ROTATE_INPUT].setVoltage(10.f);
                    m.inputs[Vessel::STRIKE_INPUT].channels = 1;
                    Module::ProcessArgs args;
                    args.sampleRate = 48000; args.sampleTime = 1.f/48000;
                    for (int i = 0; i < 48000; ++i) { args.frame = i; m.process(args); }
                    const auto start = std::chrono::steady_clock::now();
                    for (int i = 0; i < 48000; ++i) {
                        args.frame = 48000+i;
                        m.inputs[Vessel::STRIKE_INPUT].setVoltage(coupled && i % 12000 == 0 ? 10.f : 0.f);
                        m.process(args);
                    }
                    elapsed += std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
                    if (m.visualFault.load()) throw std::runtime_error("benchmark fault");
                    // Separate untimed trace: output/energy fingerprint across release,
                    // strike re-entry and pitch modulation, not just a final sample.
                    for (int i = 0; i < 24000; ++i) {
                        args.frame = 96000+i;
                        m.inputs[Vessel::ROTATE_INPUT].setVoltage(i < 8000 ? 0.f : 10.f);
                        m.inputs[Vessel::STRIKE_INPUT].setVoltage(i == 12000 ? 10.f : 0.f);
                        if (i == 16000) m.params[Vessel::PITCH_PARAM].setValue(.25f);
                        m.process(args);
                        if (m.visualFault.load()) throw std::runtime_error("trace fault");
                        hashFloat(hash, m.outputs[Vessel::LEFT_OUTPUT].getVoltage());
                        hashFloat(hash, m.outputs[Vessel::RIGHT_OUTPUT].getVoltage());
                        hashFloat(hash, m.rawEnergy.load());
                    }
                }
                std::cout << bowl << ',' << quality << ',' << separation << ',' << coupled << ','
                    << elapsed*1e6/(3*48000) << ',' << hash << std::endl;
            }
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; result = 1; }
        context.engine = nullptr;
    }
    rack::contextSet(nullptr);
    return result;
}
