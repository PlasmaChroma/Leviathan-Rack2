// Offline deterministic physical-pickup fixtures and production FIR outputs.
// Binary files contain native-endian interleaved doubles; CSV is the manifest.
#include "../../src/vessel/HostRateAdapter.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include <fstream>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    using namespace vessel;
    if (argc != 2) return 2; // Existing output directory.
    try {
        std::cout << std::setprecision(17);
        if (std::strcmp(argv[1], "--benchmark") == 0) {
            StereoSample input[256];
            unsigned random=1234567;
            for (auto& sample : input) {
                random=1664525u*random+1013904223u;
                sample.left=double(random)/4294967296.-.5;
                random=1664525u*random+1013904223u;
                sample.right=double(random)/4294967296.-.5;
            }
            std::cout << "factor,repeat,ns_per_stereo_frame,checksum\n";
            for (unsigned factor : {1u,2u,4u,8u}) for (int repeat=0; repeat<7; ++repeat) {
                StereoDecimator filter; filter.configure(factor);
                StereoSample output;
                for (unsigned i=0; i<16384*factor; ++i) filter.push(input[i%256],output);
                double checksum=0;
                const unsigned frames=262144;
                const auto start=std::chrono::steady_clock::now();
                for (unsigned i=0; i<frames*factor; ++i)
                    if (filter.push(input[i%256],output)) checksum+=output.left+output.right;
                const double ns=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()*1e9/frames;
                std::cout << factor << ',' << repeat << ',' << ns << ',' << checksum << std::endl;
            }
            return 0;
        }
        StereoDecimator coefficients;
        std::ofstream taps(std::string(argv[1])+"/coefficients.txt");
        taps << std::setprecision(17);
        for (unsigned i=0; i<StereoDecimator::taps; ++i) taps << coefficients.coefficient(i) << '\n';
        if (!taps) throw std::runtime_error("coefficient write");
        std::cout << "id,bowl,mallet,host_rate,quality,pitch,rub,factor,frames,max_energy_residual,solver_faults,nonfinite_resets\n";
        unsigned id=0;
        for (int bowl : {0,1}) for (int mallet : {0,1,2,3})
        for (double hostRate : {44100.,48000.,96000.}) for (int quality : {0,1,2})
        for (double pitch : {261.625565,2000.}) for (bool rub : {false,true}) {
            EngineSettings settings; settings.frequency=pitch;
            VesselEngine engine; engine.setAuditEnabled(true);
            unsigned factor=HostRateAdapter::factorForRate(hostRate,ProcessingQuality(quality));
            const unsigned maximum=HostRateAdapter::factorForRate(hostRate);
            while (factor<=maximum && !engine.configure(seedBowls[bowl],seedMallets[mallet],settings,hostRate*factor)) factor*=2;
            if (factor>maximum) throw std::runtime_error("fixture configuration");
            StereoDecimator filter;
            if (!filter.configure(factor)) throw std::runtime_error("filter configuration");
            const std::string stem=std::string(argv[1])+"/"+std::to_string(id);
            std::ofstream raw(stem+".raw",std::ios::binary), output(stem+".out",std::ios::binary);
            // One second covers launches, sustained rub, release, and retrigger.
            const unsigned frames=unsigned(hostRate);
            for (unsigned frame=0; frame<frames; ++frame) {
                if (frame==0 || frame==frames/2) engine.strike(frame==0 ? .1 : 1.);
                engine.setRotation(rub && (frame<3*frames/4),.2,2.5);
                for (unsigned lane=0; lane<factor; ++lane) {
                    const auto physical=engine.step();
                    if (physical.fault) throw std::runtime_error("physical fault");
                    StereoSample input, result;
                    input.left=physical.leftVelocity; input.right=physical.rightVelocity;
                    const double values[]={input.left,input.right};
                    raw.write(reinterpret_cast<const char*>(values),sizeof(values));
                    if (filter.push(input,result)) {
                        const double values[]={result.left,result.right};
                        output.write(reinterpret_cast<const char*>(values),sizeof(values));
                    }
                }
            }
            if (!raw || !output) throw std::runtime_error("capture write");
            const auto& ledger=engine.ledger();
            if (ledger.solverFaults || ledger.nonfiniteResets) throw std::runtime_error("physical recovery");
            std::cout << id++ << ',' << bowl << ',' << mallet << ',' << hostRate << ',' << quality << ','
                << pitch << ',' << rub << ',' << factor << ',' << frames << ',' << ledger.maxStepResidual << ','
                << ledger.solverFaults << ',' << ledger.nonfiniteResets << std::endl;
        }
    } catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
