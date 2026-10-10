// Whole audio callback benchmark; keep before/after executables for paired runs.
// Run serially without builds or other benchmarks. Includes controls/telemetry,
// excludes Rack scheduling, GUI and driver. Debug timing is disabled.
// --expander supplies a valid static V.Tune message to measure the linked
// Vessel callback. It excludes V.Tune's own callback and Rack message flips.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../../src/Vessel.hpp"
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdlib>
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

int main(int argc, char** argv) {
    bool expanderLinked = false;
    float speed = .2f;
    float pressure = 2.5f;
    int selectedMallet = -1;
    for (int i=1; i<argc; ++i) {
        if (std::strcmp(argv[i], "--expander") == 0) expanderLinked = true;
        else if (std::strcmp(argv[i], "--speed") == 0 && i+1<argc) {
            char* end = nullptr; speed = std::strtof(argv[++i], &end);
            if (end==argv[i] || *end || !std::isfinite(speed) || speed<0 || speed>2) return 2;
        } else if (std::strcmp(argv[i], "--pressure") == 0 && i+1<argc) {
            char* end = nullptr; pressure = std::strtof(argv[++i], &end);
            if (end==argv[i] || *end || !std::isfinite(pressure) || pressure<0 || pressure>15) return 2;
        } else if (std::strcmp(argv[i], "--mallet") == 0 && i+1<argc) {
            char* end = nullptr; const long value = std::strtol(argv[++i], &end, 10);
            if (end==argv[i] || *end || value<0 || value>3) return 2;
            selectedMallet = int(value);
        } else return 2;
    }
    Model tuneModel;
    modelVTune = &tuneModel;
    rack::Context context; rack::contextSet(&context);
    int result = 0;
    {
        rack::engine::Engine engine; context.engine = &engine;
        try {
            std::cout << "bowl,quality,separation,coupled,mean_us,fingerprint,mallet,speed,pressure\n" << std::setprecision(10);
            for (int bowl : {1, 0}) for (int quality : {1, 2})
            for (float separation : {0.f, 33.f}) for (bool coupled : {false, true}) {
                double elapsed = 0;
                std::uint64_t hash = UINT64_C(14695981039346656037);
                for (int repeat = 0; repeat < 3; ++repeat) {
                    Vessel m;
                    Module expander;
                    expander.model = &tuneModel;
                    if (expanderLinked) {
                        m.rightExpander.module = &expander;
                        expander.leftExpander.module = &m;
                        auto* message = static_cast<vessel_expander::TuneMessage*>(m.rightExpander.consumerMessage);
                        *message = vessel_expander::TuneMessage{};
                        message->magic = vessel_expander::kMagic;
                        message->speed = speed;
                        message->pressure = pressure;
                    }
                    m.params[Vessel::BOWL_PARAM].setValue(float(bowl));
                    m.params[Vessel::MALLET_PARAM].setValue(selectedMallet>=0 ? float(selectedMallet) : bowl ? 0.f : 1.f);
                    m.params[Vessel::SPEED_PARAM].setValue(speed);
                    m.params[Vessel::PRESSURE_PARAM].setValue(pressure);
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
                    << elapsed*1e6/(3*48000) << ',' << hash << ','
                    << (selectedMallet>=0 ? selectedMallet : bowl ? 0 : 1) << ',' << speed << ',' << pressure << std::endl;
            }
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; result = 1; }
        context.engine = nullptr;
    }
    rack::contextSet(nullptr);
    return result;
}
