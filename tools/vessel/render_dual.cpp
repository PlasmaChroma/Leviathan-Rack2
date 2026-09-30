// Audition the causal production-rate path rather than the offline converter.
#include "../../src/vessel/DualBowlAdapter.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void u16(std::ostream& out, std::uint16_t n) { const char b[] = {char(n), char(n>>8)}; out.write(b, 2); }
void u32(std::ostream& out, std::uint32_t n) { const char b[] = {char(n), char(n>>8), char(n>>16), char(n>>24)}; out.write(b, 4); }
void sample(std::ostream& out, double v) { float f = float(v); std::uint32_t bits; std::memcpy(&bits, &f, 4); u32(out, bits); }
double numeric(const char* text) {
    std::size_t used = 0; const std::string s(text); const double value = std::stod(s, &used);
    if (used != s.size() || !std::isfinite(value)) throw std::runtime_error("invalid finite number");
    return value;
}
}
int main(int argc, char** argv) {
    try {
        using namespace vessel;
        if (argc < 3 || argc > 7) {
            std::cerr << "Usage: vessel_render_dual OUTPUT.wav HOST_RATE [PITCH_HZ [SECONDS [rub|coupled|transition [SEPARATION_HZ]]]]\n"
                "Two-bowl Metal/Suede audition; float stereo WAV in pickup m/s, no normalization.\n";
            return 1;
        }
        const double requestedRate = numeric(argv[2]);
        if (!HostRateAdapter::factorForRate(requestedRate) || requestedRate != std::floor(requestedRate))
            throw std::runtime_error("host rate must be an integer in 32000..192000");
        const unsigned rate = unsigned(requestedRate);
        EngineSettings settings;
        if (argc >= 4) settings.frequency = numeric(argv[3]);
        const double seconds = argc >= 5 ? numeric(argv[4]) : 15;
        if (seconds < .1 || seconds > 60) throw std::runtime_error("seconds must be in 0.1..60");
        const std::string mode = argc >= 6 ? argv[5] : "rub";
        if (mode != "rub" && mode != "coupled" && mode != "transition") throw std::runtime_error("mode must be rub, coupled or transition");
        const double separation = argc == 7 ? numeric(argv[6]) : 10;
        DualBowlAdapter host;
        if (!host.configure(seedBowls[0], seedMallets[1], settings, separation, rate)) throw std::runtime_error("unsupported configuration");
        host.setAuditEnabled(true);
        const std::uint32_t frames = std::uint32_t(std::ceil(seconds*rate));
        std::ofstream wav(argv[1], std::ios::binary);
        if (!wav) throw std::runtime_error("cannot open WAV");
        wav.write("RIFF", 4); u32(wav, 36+frames*8); wav.write("WAVEfmt ", 8);
        u32(wav, 16); u16(wav, 3); u16(wav, 2); u32(wav, rate);
        u32(wav, rate*8); u16(wav, 8); u16(wav, 32); wav.write("data", 4); u32(wav, frames*8);
        double peak = 0;
        HostControls controls;
        for (std::uint32_t i = 0; i < frames; ++i) {
            if (mode == "transition") {
                double delta = -1;
                if (i == 3*rate || i == 9*rate || i == 13*rate) delta = 0;
                if (i == 6*rate) delta = 10;
                if (i == 11*rate) delta = 33;
                if (delta >= 0 && !host.configure(seedBowls[0], seedMallets[1], settings, delta, rate))
                    throw std::runtime_error("transition configuration failed");
            }
            controls.rotate = i >= std::uint32_t(std::llround(.05*rate)) && (mode == "rub" || i < 12*rate);
            controls.strikeEvent = mode != "rub" && (i == std::uint32_t(std::llround(.05*rate)) || i == 8*rate);
            const auto frame = host.process(controls);
            if (frame.fault) throw std::runtime_error("host-rate mechanical/output fault");
            peak = std::max(peak, std::max(std::abs(frame.audio.left), std::abs(frame.audio.right)));
            sample(wav, frame.audio.left); sample(wav, frame.audio.right);
        }
        wav.close(); if (!wav) throw std::runtime_error("WAV write failed");
        const auto& e = host.engine(); const auto& l = e.ledger();
        const double residual = e.totalEnergy()+l.modalLoss+l.contactLoss+l.frictionLoss+l.retiredEnergy+l.recoveryLoss
            -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
        const auto& right = host.rightEngine(); const auto& rl = right.ledger();
        const double rightResidual = right.totalEnergy()+rl.modalLoss+rl.contactLoss+rl.frictionLoss+rl.retiredEnergy+rl.recoveryLoss
            -rl.launchWork-rl.speedCapWork-rl.handWork-rl.radialWork;
        std::cout << std::setprecision(17) << "{\n  \"scope\": \"causal dual-bowl Metal/Suede audition, uncalibrated\",\n"
            << "  \"mode\": \"" << mode << "\",\n  \"host_rate_hz\": " << rate
            << ",\n  \"internal_rate_hz\": " << host.internalRate() << ",\n  \"pitch_hz\": " << settings.frequency
            << ",\n  \"filter_latency_seconds\": " << host.latencySeconds()
            << ",\n  \"peak_pickup_velocity_m_s\": " << peak << ",\n  \"strikes\": " << l.strikes
            << ",\n  \"solver_faults\": " << l.solverFaults << ",\n  \"speed_caps\": " << l.speedCaps
            << ",\n  \"cumulative_energy_residual_J\": " << residual
            << ",\n  \"separation_hz\": " << host.separationHz()
            << ",\n  \"left_frequency_hz\": " << host.engine().settings().frequency
            << ",\n  \"right_frequency_hz\": " << right.settings().frequency
            << ",\n  \"left_energy_J\": " << e.bowl().energy()
            << ",\n  \"right_energy_J\": " << right.bowl().energy()
            << ",\n  \"right_strikes\": " << rl.strikes
            << ",\n  \"right_solver_faults\": " << rl.solverFaults
            << ",\n  \"right_speed_caps\": " << rl.speedCaps
            << ",\n  \"right_cumulative_energy_residual_J\": " << rightResidual << "\n}\n";
        return 0;
    } catch (const std::exception& error) { std::cerr << "Vessel dual render: " << error.what() << '\n'; return 1; }
}
