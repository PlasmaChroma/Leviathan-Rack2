// Compile against before/after Vessel sources; compare the binary traces.
// Includes audio and published meter/control state, with changing host rates,
// irregular strikes, moving controls, expander sustain and runtime resets.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "Vessel.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>

bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel = nullptr;
Model* modelVTune = nullptr;
namespace rack { Context::~Context() {} }

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    std::ofstream out(argv[1], std::ios::binary);
    Model tuneModel; modelVTune = &tuneModel;
    rack::Context context; rack::contextSet(&context);
    {
        rack::engine::Engine engine; context.engine = &engine;
        for (int bowl : {0, 1}) for (int quality : {1, 2}) {
            Vessel m;
            Module expander; expander.model = &tuneModel;
            m.rightExpander.module = &expander; expander.leftExpander.module = &m;
            auto* tune = static_cast<vessel_expander::TuneMessage*>(m.rightExpander.consumerMessage);
            *tune = vessel_expander::TuneMessage{}; tune->magic = vessel_expander::kMagic;
            tune->velocity = .2f;
            m.params[Vessel::BOWL_PARAM].setValue(float(bowl));
            m.requestedQuality.store(quality);
            m.inputs[Vessel::STRIKE_INPUT].channels = 1;
            m.inputs[Vessel::ROTATE_INPUT].channels = 1;
            Module::ProcessArgs args;
            args.frame = 0;
            for (float rate : {44100.f, 48000.f, 96000.f, 32000.f}) {
                args.sampleRate = rate; args.sampleTime = 1.f/rate;
                for (int i = 0; i < 16000; ++i, ++args.frame) {
                    if (i == 8000) m.pendingReset.store(true);
                    if (i%431 == 0) {
                        m.params[Vessel::PITCH_PARAM].setValue(float((i/431)%7-3)*.08f);
                        m.params[Vessel::BINAURAL_PARAM].setValue(float((i/431)%34));
                        tune->sustain = float((i/431)%9-4)*.125f;
                    }
                    m.inputs[Vessel::STRIKE_INPUT].setVoltage(i%1373 == 0 ? 10.f : 0.f);
                    m.inputs[Vessel::ROTATE_INPUT].setVoltage(i < 12000 ? 10.f : 0.f);
                    m.process(args);
                    if (m.visualFault.load()) {
                        std::cerr << "capture fault: bowl=" << bowl << " quality=" << quality
                            << " rate=" << rate << " frame=" << i << '\n';
                        throw std::runtime_error("capture fault");
                    }
                    const float values[] = {m.outputs[Vessel::LEFT_OUTPUT].getVoltage(),
                        m.outputs[Vessel::RIGHT_OUTPUT].getVoltage(), m.visualEnergy.load(),
                        m.rawEnergy.load(), m.visualFrequency.load(), m.visualSeparation.load(),
                        m.leftEnergy.load(), m.rightEnergy.load(), m.visualInternalRate.load()};
                    for (float value : values) if (!std::isfinite(value))
                        throw std::runtime_error("nonfinite capture");
                    out.write(reinterpret_cast<const char*>(values), sizeof(values));
                }
            }
        }
        context.engine = nullptr;
    }
    rack::contextSet(nullptr);
    return out.good() ? 0 : 1;
}
