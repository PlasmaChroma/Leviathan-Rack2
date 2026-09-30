#include "../../src/vessel/VesselEngine.hpp"
#include "../../src/vessel/SeedProfiles.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void u16(std::ostream& out, std::uint16_t v) {
    const char bytes[] = {char(v), char(v >> 8)};
    out.write(bytes, 2);
}
void u32(std::ostream& out, std::uint32_t v) {
    const char bytes[] = {char(v), char(v >> 8), char(v >> 16), char(v >> 24)};
    out.write(bytes, 4);
}
void sample(std::ostream& out, double value) {
    const float f = float(value);
    std::uint32_t bits;
    std::memcpy(&bits, &f, sizeof(bits));
    u32(out, bits);
}
double numeric(const std::string& value) {
    std::size_t used = 0;
    const double result = std::stod(value, &used);
    if (used != value.size() || !std::isfinite(result)) throw std::runtime_error("invalid finite number: "+value);
    return result;
}
std::size_t selection(const std::string& value, const char* const* names, std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) if (value == names[i]) return i;
    throw std::runtime_error("unknown profile: "+value);
}
}

int main(int argc, char** argv) {
    try {
        using namespace vessel;
        std::string output = "build/vessel-strike-raw.wav", report;
        std::size_t bowlIndex = 0, malletIndex = 1;
        double seconds = 8.0, velocity = .5, retrigger = -1.0;
        int rate = 192000;
        EngineSettings settings;
        const char* const bowlNames[] = {"metal", "crystal"};
        const char* const malletNames[] = {"wood", "suede", "silicone", "felt"};
        for (int i = 1; i < argc; ++i) {
            const std::string key = argv[i];
            if (key == "--help") {
                std::cout << "Vessel internal-rate mechanical strike renderer\n"
                    "--output PATH --report PATH --bowl metal|crystal --mallet wood|suede|silicone|felt\n"
                    "--seconds 0.1..30 --rate 176400..768000 --pitch 20..2000 --velocity 0..1\n"
                    "--retrigger-at SECONDS --width 0..1\n"
                    "WAV channels are physical virtual-pickup velocities in m/s, before gain/resampling.\n";
                return 0;
            }
            if (++i >= argc) throw std::runtime_error("missing value for "+key);
            const std::string value = argv[i];
            if (key == "--output") output = value;
            else if (key == "--report") report = value;
            else if (key == "--bowl") bowlIndex = selection(value, bowlNames, 2);
            else if (key == "--mallet") malletIndex = selection(value, malletNames, 4);
            else if (key == "--seconds") seconds = numeric(value);
            else if (key == "--rate") {
                const double requested = numeric(value);
                if (requested < 176400 || requested > 768000 || requested != std::floor(requested))
                    throw std::runtime_error("internal rate must be an integer in 176400..768000");
                rate = int(requested);
            }
            else if (key == "--pitch") settings.frequency = numeric(value);
            else if (key == "--velocity") velocity = numeric(value);
            else if (key == "--retrigger-at") retrigger = numeric(value);
            else if (key == "--width") {
                const double width = numeric(value);
                if (width < 0 || width > 1) throw std::runtime_error("width must be in 0..1");
                settings.observerSeparation = pi/6.0*width;
            }
            else throw std::runtime_error("unknown option "+key);
        }
        if (seconds < .1 || seconds > 30 || velocity < 0 || velocity > 1
            || (retrigger >= 0 && (retrigger <= .05 || retrigger >= seconds))) throw std::runtime_error("invalid render/event range");
        VesselEngine engine;
        if (!engine.configure(seedBowls[bowlIndex], seedMallets[malletIndex], settings, rate))
            throw std::runtime_error("unsupported engine configuration");
        engine.setAuditEnabled(true);
        const std::uint32_t frames = std::uint32_t(std::ceil(seconds*rate));
        const std::uint32_t dataBytes = frames*8u;
        const auto firstStrike = std::uint32_t(std::llround(.05*rate));
        const auto nextStrike = retrigger >= 0 ? std::uint32_t(std::llround(retrigger*rate)) : frames;
        std::ofstream wav(output, std::ios::binary);
        if (!wav) throw std::runtime_error("cannot open WAV "+output);
        wav.write("RIFF", 4); u32(wav, 36+dataBytes); wav.write("WAVEfmt ", 8);
        u32(wav, 16); u16(wav, 3); u16(wav, 2); u32(wav, rate);
        u32(wav, rate*8u); u16(wav, 8); u16(wav, 32);
        wav.write("data", 4); u32(wav, dataBytes);
        double peak = 0, peakForce = 0, maxCompression = 0, worstCompliance = 1.0;
        for (std::size_t j = 0; j < engine.bowl().size(); ++j)
            worstCompliance = std::min(worstCompliance, engine.bowl().coefficients(j).staticComplianceRatio);
        const auto started = std::chrono::steady_clock::now();
        for (std::uint32_t i = 0; i < frames; ++i) {
            if (i == firstStrike || i == nextStrike) engine.strike(velocity);
            const auto frame = engine.step();
            if (frame.fault) throw std::runtime_error("mechanical fault while rendering");
            peak = std::max(peak, std::max(std::abs(frame.leftVelocity), std::abs(frame.rightVelocity)));
            peakForce = std::max(peakForce, frame.strikeForce);
            maxCompression = std::max(maxCompression, frame.compression);
            sample(wav, frame.leftVelocity); sample(wav, frame.rightVelocity);
        }
        wav.close();
        if (!wav) throw std::runtime_error("WAV write failed");
        const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now()-started).count();
        const auto& ledger = engine.ledger();
        const double residual = engine.totalEnergy()+ledger.modalLoss+ledger.contactLoss
            +ledger.retiredEnergy+ledger.recoveryLoss-ledger.launchWork-ledger.speedCapWork;
        std::ostream* destination = &std::cout;
        std::ofstream json;
        if (!report.empty()) {
            json.open(report);
            if (!json) throw std::runtime_error("cannot open report "+report);
            destination = &json;
        }
        auto& out = *destination;
        out << std::setprecision(17) << "{\n"
            << "  \"scope\": \"internal-rate strike prototype; no rubbing or Rack adapter\",\n"
            << "  \"bowl\": \"" << seedBowls[bowlIndex].stableId << "\",\n"
            << "  \"mallet\": \"" << seedMallets[malletIndex].stableId << "\",\n"
            << "  \"internal_rate_hz\": " << rate << ",\n"
            << "  \"frames\": " << frames << ",\n"
            << "  \"frequency_hz\": " << settings.frequency << ",\n"
            << "  \"strikes\": " << ledger.strikes << ",\n"
            << "  \"launch_work_J\": " << ledger.launchWork << ",\n"
            << "  \"modal_loss_J\": " << ledger.modalLoss << ",\n"
            << "  \"contact_loss_J\": " << ledger.contactLoss << ",\n"
            << "  \"retired_energy_J\": " << ledger.retiredEnergy << ",\n"
            << "  \"remaining_energy_J\": " << engine.totalEnergy() << ",\n"
            << "  \"cumulative_energy_residual_J\": " << residual << ",\n"
            << "  \"max_step_energy_residual_J\": " << ledger.maxStepResidual << ",\n"
            << "  \"peak_observer_velocity_m_per_s\": " << peak << ",\n"
            << "  \"peak_contact_force_N\": " << peakForce << ",\n"
            << "  \"max_indentation_m\": " << maxCompression << ",\n"
            << "  \"worst_mode_static_compliance_ratio\": " << worstCompliance << ",\n"
            << "  \"max_solver_iterations\": " << ledger.maxSolverIterations << ",\n"
            << "  \"solver_faults\": " << ledger.solverFaults << ",\n"
            << "  \"speed_caps\": " << ledger.speedCaps << ",\n"
            << "  \"elapsed_render_with_io_seconds\": " << elapsed << "\n}\n";
        if (!out) throw std::runtime_error("report write failed");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Vessel render: " << error.what() << '\n'; return 1;
    }
}
