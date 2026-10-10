#include "../src/vessel/HostRateAdapter.hpp"
#include "../src/vessel/SeedProfiles.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <random>
#include <cstring>

namespace {
using namespace vessel;
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
bool close(double a, double b, double eps = 1e-10) { return std::abs(a-b) <= eps; }

// Original branch-wrapped scalar kernel, independent of mirrored storage/SIMD.
void originalFirEquivalence() {
    constexpr unsigned taps = StereoDecimator::taps, center = (taps-1)/2;
    struct Stage { std::array<StereoSample,taps> samples {}; unsigned pos=0,phase=0; };
    std::mt19937 rng(71241); std::uniform_real_distribution<double> values(-10,10);
    double maxNormalizedError = 0.0;
    for (double scale : {1e-100, 1e-6, 1.0, 1e6, 1e100})
    for (unsigned factor : {1u,2u,4u,8u}) {
        StereoDecimator fast; require(fast.configure(factor), "optimized FIR setup");
        std::array<Stage,3> stages {};
        const unsigned count=factor==8?3:factor==4?2:factor==2?1:0;
        double coefficientNorm = 0.0;
        for (unsigned i=0; i<StereoDecimator::taps; ++i)
            coefficientNorm += std::abs(fast.coefficient(i));
        // Absolute, input-scaled bound remains meaningful at cancellation/nulls.
        // Allow 64 eps per stage, amplified by the cascade's coefficient L1 norm.
        const double tolerance = 64*std::numeric_limits<double>::epsilon()*10*scale
            *count*std::pow(coefficientNorm, count);
        for (unsigned sample=0;sample<16000;++sample) {
            StereoSample input,actual;
            if (sample < 4000) { input.left=values(rng); input.right=values(rng); }
            else if (sample < 6000) { input.left=10; input.right=-3; }
            else if (sample < 8000) { input.left=sample%2 ? 10 : -10; input.right=-input.left; }
            else if (sample < 10000) { input.left=sample%257 == 0 ? 10 : 0; }
            else if (sample < 12000) {
                input.left=10*std::cos(.49*pi*sample);
                input.right=10*std::sin(.4*pi*sample);
            }
            else if (sample < 14000) {
                input.left=sample%2 ? 10 : -10+1e-12;
                input.right=values(rng)*1e-12;
            }
            input.left *= scale; input.right *= scale;
            bool ready=true;auto expected=input;
            for (unsigned stage=0;stage<count;++stage) {
                auto& s=stages[stage];s.samples[s.pos]=expected;const unsigned newest=s.pos;
                if(++s.pos==taps)s.pos=0;
                s.phase^=1;
                if(s.phase) {ready=false;break;}
                expected={};unsigned a=newest,b=s.pos;
                for (unsigned i=0;i<center;++i) {
                    expected.left+=fast.coefficient(i)*(s.samples[a].left+s.samples[b].left);
                    expected.right+=fast.coefficient(i)*(s.samples[a].right+s.samples[b].right);
                    a=a==0?taps-1:a-1;if(++b==taps)b=0;
                }
                expected.left+=fast.coefficient(center)*s.samples[a].left;
                expected.right+=fast.coefficient(center)*s.samples[a].right;
            }
            require(fast.push(input,actual)==ready, "optimized FIR cadence differs");
            if (ready) {
                const double error = std::max(std::abs(actual.left-expected.left),
                                              std::abs(actual.right-expected.right));
                require(std::isfinite(actual.left) && std::isfinite(actual.right), "nonfinite FIR output");
                maxNormalizedError = std::max(maxNormalizedError, error/(10*scale));
#if defined(VESSEL_SERIAL_FIR) || defined(VESSEL_SCALAR_FIR) || !defined(__SSE2__)
                require(actual.left==expected.left && actual.right==expected.right,
                        "strict FIR differs from original scalar kernel");
#else
                require(error <= tolerance, "four-accumulator FIR exceeds rounding bound");
#endif
            }
        }
        (void)tolerance; // Strict builds use exact equality instead.
    }
    std::cout << "  FIR maximum error/input peak=" << maxNormalizedError << '\n';
}

void frequencyResponseAndLatency() {
    originalFirEquivalence();
    StereoDecimator d;
    double passMin = 1e100, passMax = 0, stopMax = 0;
    for (int k = 0; k <= 20000; ++k) {
        const double f = .5*k/20000;
        double real = 0, imaginary = 0;
        for (unsigned n = 0; n < StereoDecimator::taps; ++n) {
            real += d.coefficient(n)*std::cos(2*pi*f*n);
            imaginary -= d.coefficient(n)*std::sin(2*pi*f*n);
        }
        const double amplitude = std::hypot(real, imaginary);
        if (f <= .2) { passMin = std::min(passMin, amplitude); passMax = std::max(passMax, amplitude); }
        if (f >= .25) stopMax = std::max(stopMax, amplitude);
    }
    const double stopDb = 20*std::log10(stopMax), passDb = 20*std::log10(passMax/passMin);
    require(stopDb < -70 && passDb < .01, "decimator response violates declared pass/stop bands");
    std::cout << "  stage stopband=" << stopDb << " dB passband span=" << passDb << " dB\n";
    for (unsigned factor : {1u, 2u, 4u, 8u}) {
        require(d.configure(factor), "valid decimation rejected");
        double sum = 0, moment = 0;
        for (int host = 0; host < 512; ++host) for (unsigned lane = 0; lane < factor; ++lane) {
            StereoSample input, output;
            if (host == 0 && lane == 0) input.left = input.right = 1;
            const bool emitted = d.push(input, output);
            require(emitted == (lane+1 == factor), "decimator output phase/event cadence");
            if (emitted) {
                require(output.left == output.right, "stereo impulse mismatch");
                sum += output.left; moment += host*output.left;
            }
        }
        require(close(sum*factor, 1, 1e-4), "impulse/DC gain");
        require(close(moment/sum, d.latencyHostSamples(), .005), "measured impulse latency disagrees with declared host tags");
        require(!d.configure(3) && d.factor() == factor, "invalid factor mutates decimator");
        d.reset();
        for (unsigned i = 0; i < 1024*factor; ++i) {
            StereoSample zero, output;
            if (d.push(zero, output)) require(output.left == 0 && output.right == 0, "reset leaves filter tail");
        }
        if (factor == 1) continue;
        double worstFold = 0;
        for (double cyclesPerHost : {.51, .55, .8, 1.1, 1.7, 2.9, 3.7}) {
            if (cyclesPerHost >= .5*factor) continue;
            d.reset(); double square = 0; unsigned measured = 0;
            for (unsigned i = 0; i < 8192*factor; ++i) {
                StereoSample input, output;
                input.left = std::cos(2*pi*cyclesPerHost*i/factor+.23);
                input.right = -input.left;
                if (d.push(input, output) && i >= 1024*factor) {
                    require(output.right == -output.left, "stereo decimation leaks channels");
                    square += output.left*output.left; ++measured;
                }
            }
            worstFold = std::max(worstFold, std::sqrt(2*square/measured));
        }
        require(worstFold < std::pow(10.0, -70.0/20), "cascade folded-tone rejection");
        std::cout << "  factor=" << factor << " tagged latency=" << d.latencyHostSamples()
                  << " host samples folded-tone rejection=" << 20*std::log10(worstFold) << " dB\n";
    }
    std::cout << "[PASS] Measured FIR response, cascade folded tones, stereo isolation, impulse latency and history reset\n";
}

void hostSchedulingAndMechanics() {
    for (double rate : {32000.0, 44100.0, 48000.0, 88200.0, 96000.0, 176400.0, 192000.0}) {
        HostRateAdapter host;
        VesselEngine reference;
        EngineSettings settings;
        settings.observerSeparation = 0.0;
        require(host.configure(seedBowls[0], seedMallets[1], settings, rate), "supported host rate rejected");
        require(host.internalRate() >= 176400 && host.factor() <= 8, "wrong internal rate policy");
        require(reference.configure(seedBowls[0], seedMallets[1], settings, host.internalRate()), "reference configure");
        host.setAuditEnabled(true); reference.setAuditEnabled(true);
        HostControls controls; controls.rotate = true;
        for (int i = 0; i < 20000; ++i) {
            controls.strikeEvent = i == 0 || i == 10000;
            if (controls.strikeEvent) reference.strike(controls.velocity);
            reference.setRotation(controls.rotate, controls.speed, controls.pressure);
            const auto frame = host.process(controls);
            for (unsigned lane = 0; lane < host.factor(); ++lane) require(!reference.step().fault, "reference fault");
            require(!frame.fault && frame.audio.left == frame.audio.right && frame.bowlEnergy == reference.bowl().energy(),
                "rate conversion changes energy or stereo null");
            if (i%257 == 0) for (std::size_t j = 0; j < reference.bowl().size(); ++j)
                require(reference.bowl().state(j).x == host.engine().bowl().state(j).x
                    && reference.bowl().state(j).y == host.engine().bowl().state(j).y, "host adapter changes mechanical trajectory");
        }
        require(host.engine().ledger().strikes == 2, "strike repeated across oversample lanes");
        require(host.engine().ledger().handWork == reference.ledger().handWork, "host resampling changes drive accounting");
        std::cout << "  host=" << rate << " internal=" << host.internalRate() << " latency=" << host.latencySeconds()*1000 << " ms\n";
    }
    std::cout << "[PASS] Seven host rates match internal reference bit-for-bit, events fire once, energy stays independent of audio filtering\n";
}

void lifecycle() {
    HostRateAdapter host;
    HostControls controls;
    require(host.process(controls).fault, "unconfigured adapter processes audio");
    EngineSettings settings;
    require(host.configure(seedBowls[1], seedMallets[2], settings, 48000), "lifecycle configure");
    controls.strikeEvent = true;
    require(!host.process(controls).fault && host.engine().strikerActive(), "lifecycle active strike");
    controls.strikeEvent = false; controls.rotate = true;
    const double energy = host.engine().totalEnergy(), compression = host.engine().compression();
    require(host.configure(seedBowls[1], seedMallets[2], settings, 44100), "active rate change");
    require(host.engine().totalEnergy() == energy && host.engine().compression() == compression,
        "host rate change discards mechanics/contact state");
    const double oldRate = host.hostRate();
    require(!host.configure(seedBowls[1], seedMallets[2], settings, std::numeric_limits<double>::quiet_NaN())
        && host.hostRate() == oldRate && host.engine().totalEnergy() == energy, "invalid host rate mutates state");
    auto invalid = seedMallets[2]; invalid.muS = 1e10;
    require(!host.configure(seedBowls[1], invalid, settings, 96000) && host.hostRate() == oldRate,
        "failed descriptor/rate transaction mutates adapter");
    for (int i = 0; i < 10000; ++i) require(!host.process(controls).fault, "active rate transition fault");
    const auto before = host.process(controls);
    require(host.configure(seedBowls[1], seedMallets[2], settings, 96000), "audible rate transition configure");
    const auto after = host.process(controls);
    require(close(after.audio.left, before.audio.left*(1.0-1.0/(.005*96000)), 1e-7)
        && close(after.audio.right, before.audio.right*(1.0-1.0/(.005*96000)), 1e-7),
        "rate change fails to blend from last finite output");
    controls.speed = std::numeric_limits<double>::infinity(); controls.pressure = std::numeric_limits<double>::quiet_NaN();
    require(!host.process(controls).fault, "nonfinite host controls not sanitized");
    host.reset(); controls = HostControls{};
    for (int i = 0; i < 1000; ++i) {
        const auto frame = host.process(controls);
        require(!frame.fault && frame.audio.left == 0 && frame.audio.right == 0 && frame.bowlEnergy == 0,
            "host reset leaves mechanics/audio history");
    }
    require(HostRateAdapter::factorForRate(31000) == 0 && HostRateAdapter::factorForRate(193000) == 0,
        "undeclared host rate accepted");
    std::cout << "[PASS] Transactional host/descriptor changes, active compression continuity, input sanitation and complete reset\n";
}
void qualityPolicies() {
    for (double rate : {32000.,44100.,48000.,88200.,96000.,176400.,192000.})
    for (auto quality : {ProcessingQuality::Economy, ProcessingQuality::Balanced, ProcessingQuality::Reference}) {
        HostRateAdapter host; EngineSettings settings;
        require(host.configure(seedBowls[0], seedMallets[1], settings, rate, quality), "quality setup");
        const unsigned factor = HostRateAdapter::factorForRate(rate, quality);
        require(host.factor() == factor, "C4 unexpectedly falls back");
        VesselEngine reference; StereoDecimator filter;
        require(reference.configure(seedBowls[0], seedMallets[1], settings, rate*factor)
            && filter.configure(factor), "quality oracle setup");
        HostControls c; c.rotate = true;
        for (int i = 0; i < 2048; ++i) {
            c.strikeEvent = i == 0;
            reference.setRotation(c.rotate, c.speed, c.pressure);
            if (c.strikeEvent) reference.strike(c.velocity);
            StereoSample expected;
            for (unsigned j = 0; j < factor; ++j) {
                const auto f = reference.step();
                StereoSample sample; sample.left = f.leftVelocity; sample.right = f.rightVelocity;
                filter.push(sample, expected);
            }
            const auto actual = host.process(c);
            require(!actual.fault && actual.audio.left == expected.left && actual.audio.right == expected.right,
                "quality cadence/audio differs from internal-rate oracle");
        }
    }
    HostRateAdapter h; EngineSettings s; s.frequency = 2000;
    require(h.configure(seedBowls[1], seedMallets[1], s, 48000, ProcessingQuality::Economy)
        && h.internalRate() == 192000, "high crystal must fall back to Reference");
    s.frequency = 261.625565;
    for (auto q : {ProcessingQuality::Economy, ProcessingQuality::Balanced, ProcessingQuality::Reference}) {
        require(h.configure(seedBowls[0], seedMallets[1], s, 48000, q), "same-host quality switch");
        require(h.factor() == HostRateAdapter::factorForRate(48000, q), "same-host factor not applied");
    }
    std::cout << "[PASS] Three quality policies at seven host rates match internal oracles; high-pitch fallback and same-host factor changes\n";
}

void freeTailEquivalence() {
    for (double rate : {32000.,44100.,48000.,88200.,96000.,176400.,192000.})
    for (auto quality : {ProcessingQuality::Economy,ProcessingQuality::Balanced,ProcessingQuality::Reference}) {
        HostRateAdapter fast, reference; EngineSettings s;
        reference.setFastTailEnabled(false);
        require(fast.configure(seedBowls[0],seedMallets[1],s,rate,quality)
            && reference.configure(seedBowls[0],seedMallets[1],s,rate,quality), "tail setup");
        HostControls c;
        for (int i=0;i<10000;++i) {
            c.rotate=i<2000 || (i>=7000 && i<8000);
            c.strikeEvent=i==0 || i==5000;
            c.speed=i<6000?.4:-.4; c.pressure=i<6000?2.5:1.2;
            if(i==4000 || i==6500) {
                s.observerSeparation=i==4000?0:pi/6;
                s.frequency=i==4000?330:261.625565;
                require(fast.configure(seedBowls[0],seedMallets[1],s,rate,quality)
                    && reference.configure(seedBowls[0],seedMallets[1],s,rate,quality), "tail reconfigure");
            }
            const auto a=fast.process(c), b=reference.process(c);
            require(!a.fault && !b.fault && std::memcmp(&a.audio,&b.audio,sizeof(a.audio))==0
                && std::memcmp(&a.bowlEnergy,&b.bowlEnergy,sizeof(a.bowlEnergy))==0, "fast tail differs from full-step filtered oracle");
            if(i%64==0)for(std::size_t j=0;j<fast.engine().bowl().size();++j)
                require(std::memcmp(&fast.engine().bowl().state(j),&reference.engine().bowl().state(j),sizeof(ModalState))==0, "fast tail changes mechanical state");
        }
    }
    std::cout << "[PASS] Fused tails match full-step states and filtered audio through strike/rotation re-entry, tuning and width at seven rates/three qualities\n";
}

}

int main() {
    try {
        frequencyResponseAndLatency(); hostSchedulingAndMechanics(); lifecycle(); qualityPolicies(); freeTailEquivalence();
        std::cout << "Vessel host-rate adapter: 5 groups PASS\n"; return 0;
    } catch (const std::exception& error) { std::cerr << "[FAIL] " << error.what() << '\n'; return 1; }
}
