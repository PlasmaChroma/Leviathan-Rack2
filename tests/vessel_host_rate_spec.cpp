#include "../src/vessel/HostRateAdapter.hpp"
#include "../src/vessel/SeedProfiles.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
using namespace vessel;
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
bool close(double a, double b, double eps = 1e-10) { return std::abs(a-b) <= eps; }

void frequencyResponseAndLatency() {
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
}

int main() {
    try {
        frequencyResponseAndLatency(); hostSchedulingAndMechanics(); lifecycle();
        std::cout << "Vessel host-rate adapter: 3 groups PASS\n"; return 0;
    } catch (const std::exception& error) { std::cerr << "[FAIL] " << error.what() << '\n'; return 1; }
}
