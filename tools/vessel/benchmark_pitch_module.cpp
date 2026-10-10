// Cross-binary pitch-modulation benchmark. The two lanes are identical workloads.
// Individual update timing excludes control writes; stream timing includes the
// control driver, update timers, output checksum and fault checks. No GUI/driver.
#include <context.hpp>
#include <engine/Engine.hpp>
#undef PRIVATE
#include "../../src/Vessel.hpp"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
bool isDragonKingDebugEnabled() { return false; }
Model* modelVessel = nullptr;
Model* modelVTune = nullptr;
namespace rack { Context::~Context() {} }

int main() {
    rack::Context context; rack::contextSet(&context);
    int result=0;
    {
        rack::engine::Engine engine; context.engine=&engine;
        try {
            std::cout << std::setprecision(12)
                << "bowl,quality,separation,rub,repeat,lane,internal_rate,samples,mean_us,median_us,p99_us,max_us,stream_mean_us,checksum\n";
            for (int bowl : {0,1}) for (int quality : {1,2})
            for (float separation : {0.f,33.f}) for (bool rub : {false,true}) {
                Vessel full, cached;
                double fullElapsed=.001, cachedElapsed=.001;
                Module::ProcessArgs args; args.sampleRate=48000; args.sampleTime=1.f/48000;
                for (auto* m : {&full,&cached}) {
                    auto& elapsed=m == &cached ? cachedElapsed : fullElapsed;
                    m->inputs[Vessel::VOCT_INPUT].channels=1;
                    m->params[Vessel::BOWL_PARAM].setValue(float(bowl));
                    m->params[Vessel::MALLET_PARAM].setValue(bowl ? 0.f : 1.f);
                    m->params[Vessel::SPEED_PARAM].setValue(.2f);
                    m->params[Vessel::BINAURAL_PARAM].setValue(separation);
                    m->requestedQuality.store(quality);
                    m->inputs[Vessel::ROTATE_INPUT].channels=1;
                    m->inputs[Vessel::ROTATE_INPUT].setVoltage(rub ? 10.f : 0.f);
                    m->inputs[Vessel::STRIKE_INPUT].channels=1;
                    for (int i=0; i<48000; ++i) {
                        args.frame=i; m->inputs[Vessel::STRIKE_INPUT].setVoltage(i == 0 ? 10.f : 0.f); m->process(args);
                        elapsed += args.sampleTime;
                        if (elapsed >= .001 || i == 0) elapsed=0;
                    }
                }
                unsigned fullWidth=0, cachedWidth=0;
                for (unsigned repeat=0; repeat<6; ++repeat) {
                    double checksums[2] = {};
                    for (unsigned slot=0; slot<2; ++slot) {
                        const bool fast=(repeat%2) ? slot == 0 : slot == 1;
                        auto& m=fast ? cached : full;
                        auto& width=fast ? cachedWidth : fullWidth;
                        auto& elapsed=fast ? cachedElapsed : fullElapsed;
                        std::vector<double> samples; samples.reserve(1024);
                        const auto streamStart=std::chrono::steady_clock::now();
                        for (unsigned i=0; i<32768; ++i) {
                            args.frame=48000+repeat*32768+i;
                            // Mirror the module's 1 kHz boundary without exposing
                            // private state or forcing extra control callbacks.
                            elapsed += args.sampleTime;
                            const bool update=elapsed >= .001;
                            if (update) elapsed=0;
                            if (update) m.inputs[Vessel::VOCT_INPUT].setVoltage((++width%2) ? -.1f : .1f);
                            if (update) {
                                const auto start=std::chrono::steady_clock::now();
                                m.process(args);
                                samples.push_back(std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e6);
                            } else m.process(args);
                            checksums[fast] += m.outputs[Vessel::LEFT_OUTPUT].getVoltage()+m.outputs[Vessel::RIGHT_OUTPUT].getVoltage();
                            if (m.visualFault.load()) throw std::runtime_error("pitch benchmark fault");
                        }
                        const double streamUs=std::chrono::duration<double>(std::chrono::steady_clock::now()-streamStart).count()*1e6/32768;
                        if (samples.empty()) throw std::runtime_error("no pitch updates");
                        std::sort(samples.begin(),samples.end()); double sum=0; for (double v:samples) sum+=v;
                        std::cout << bowl << ',' << quality << ',' << separation << ',' << rub << ',' << repeat << ',' << fast << ','
                            << m.audio.internalRate() << ',' << samples.size() << ',' << sum/samples.size() << ','
                            << samples[samples.size()/2] << ',' << samples[(samples.size()-1)*99/100] << ',' << samples.back()
                            << ',' << streamUs << ',' << checksums[fast] << '\n';
                    }
                    if (checksums[0] != checksums[1]) throw std::runtime_error("pitch benchmark output mismatch");
                }
            }
        } catch (const std::exception& e) { std::cerr << e.what() << '\n'; result=1; }
        context.engine=nullptr;
    }
    rack::contextSet(nullptr); return result;
}
