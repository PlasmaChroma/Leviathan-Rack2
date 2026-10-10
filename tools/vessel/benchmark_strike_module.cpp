// Headless callback timing during impact, including overlapping rubbing.
// Compare preserved before/after executables serially. --trace FILE writes
// untimed audio/energy trajectories with auditing enabled instead of timing.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../../src/Vessel.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel = nullptr;
Model* modelVTune = nullptr;
namespace rack { Context::~Context() {} }

static void row(int bowl, int mallet, int quality, float separation, bool rub,
                double rate, const char* phase, std::vector<double>& values) {
    if (values.empty()) return;
    std::sort(values.begin(), values.end());
    double total = 0;
    for (double v : values) total += v;
    auto percentile = [&](double p) { return values[std::size_t(p*(values.size()-1))]; };
    std::cout << bowl << ',' << mallet << ',' << quality << ',' << separation << ',' << rub << ','
        << rate << ',' << phase << ',' << values.size() << ',' << total/values.size() << ','
        << percentile(.5) << ',' << percentile(.95) << ',' << percentile(.99) << ','
        << percentile(.999) << ',' << values.back() << '\n';
}

int main(int argc, char** argv) {
    const bool tracing = argc == 3 && std::strcmp(argv[1], "--trace") == 0;
    if (argc != 1 && !tracing) return 2;
    std::ofstream trace;
    if (tracing) {
        trace.open(argv[2], std::ios::binary);
        if (!trace) return 2;
    }
    rack::Context context; rack::contextSet(&context);
    int result = 0;
    {
        rack::engine::Engine engine; context.engine = &engine;
        try {
            std::cout << std::setprecision(12);
            if (tracing) std::cout << "bowl,mallet,quality,separation,rub,internal_rate,solver_faults,nonfinite_resets,max_energy_residual,strike_iterations,friction_iterations\n";
            else std::cout << "bowl,mallet,quality,separation,rub,internal_rate,phase,samples,mean_us,median_us,p95_us,p99_us,p999_us,max_us\n";
            for (int bowl : {0, 1}) for (int mallet : {0, 1, 2, 3})
            for (int quality : {1, 2}) for (float separation : {0.f, 33.f}) for (bool rub : {false, true}) {
                Vessel m;
                m.params[Vessel::BOWL_PARAM].setValue(float(bowl));
                m.params[Vessel::MALLET_PARAM].setValue(float(mallet));
                m.params[Vessel::SPEED_PARAM].setValue(.2f);
                m.params[Vessel::BINAURAL_PARAM].setValue(separation);
                m.requestedQuality.store(quality);
                m.inputs[Vessel::ROTATE_INPUT].channels = 1;
                m.inputs[Vessel::ROTATE_INPUT].setVoltage(rub ? 10.f : 0.f);
                m.inputs[Vessel::STRIKE_INPUT].channels = 1;
                m.audio.setAuditEnabled(tracing);
                Module::ProcessArgs args;
                args.sampleRate = 48000; args.sampleTime = 1.f/48000;
                for (int i=0; i<48000; ++i) { args.frame=i; m.process(args); }
                std::vector<double> onset, contact, other;
                if (!tracing) { onset.reserve(32); contact.reserve(32768); other.reserve(32768); }
                for (int i=0; i<32768; ++i) {
                    args.frame = 48000+i;
                    const bool strike = i%1024 == 0;
                    m.inputs[Vessel::STRIKE_INPUT].setVoltage(strike ? 10.f : 0.f);
                    const bool active = m.audio.engine().strikerActive() || m.audio.rightEngine().strikerActive();
                    if (tracing) {
                        // Include soft/hard impacts, release/restart, pitch and width changes.
                        m.params[Vessel::VELOCITY_PARAM].setValue(i < 8192 ? .1f : i < 16384 ? 1.f : .5f);
                        m.inputs[Vessel::ROTATE_INPUT].setVoltage(rub && (i < 16384 || i >= 24576) ? 10.f : 0.f);
                        if (i == 20000) m.params[Vessel::PITCH_PARAM].setValue(.25f);
                        if (i == 24000) m.params[Vessel::WIDTH_PARAM].setValue(.2f);
                        m.process(args);
                        const double values[] = {m.outputs[Vessel::LEFT_OUTPUT].getVoltage(),
                            m.outputs[Vessel::RIGHT_OUTPUT].getVoltage(), m.audio.engine().totalEnergy(),
                            m.audio.rightEngine().totalEnergy()};
                        trace.write(reinterpret_cast<const char*>(values), sizeof(values));
                    } else {
                        const auto start = std::chrono::steady_clock::now();
                        m.process(args);
                        const double us = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e6;
                        (strike ? onset : active ? contact : other).push_back(us);
                    }
                    if (m.visualFault.load()) throw std::runtime_error("strike callback fault");
                }
                if (tracing) {
                    const auto& a = m.audio.engine().ledger();
                    const auto& b = m.audio.rightEngine().ledger();
                    if (a.solverFaults || b.solverFaults || a.nonfiniteResets || b.nonfiniteResets)
                        throw std::runtime_error("strike trace recovery");
                    std::cout << bowl << ',' << mallet << ',' << quality << ',' << separation << ',' << rub << ','
                        << m.audio.internalRate() << ',' << a.solverFaults+b.solverFaults << ','
                        << a.nonfiniteResets+b.nonfiniteResets << ',' << std::max(a.maxStepResidual,b.maxStepResidual)
                        << ',' << std::max(a.maxSolverIterations,b.maxSolverIterations) << ','
                        << std::max(a.maxFrictionIterations,b.maxFrictionIterations) << '\n';
                } else {
                    row(bowl,mallet,quality,separation,rub,m.audio.internalRate(),"onset",onset);
                    row(bowl,mallet,quality,separation,rub,m.audio.internalRate(),"contact",contact);
                    row(bowl,mallet,quality,separation,rub,m.audio.internalRate(),"other",other);
                }
            }
            if (tracing && !trace) throw std::runtime_error("trace write failed");
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; result=1; }
        context.engine = nullptr;
    }
    rack::contextSet(nullptr);
    return result;
}
