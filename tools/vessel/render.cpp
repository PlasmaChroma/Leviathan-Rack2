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
#include <vector>

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
        double rotateAt = -1.0, releaseAt = -1.0, stopAt = -1.0, speed = .4, pressure = 2.5;
        int rate = 192000;
        EngineSettings settings;
        const char* const bowlNames[] = {"metal", "crystal"};
        const char* const malletNames[] = {"wood", "suede", "silicone", "felt"};
        for (int i = 1; i < argc; ++i) {
            const std::string key = argv[i];
            if (key == "--help") {
                std::cout << "Vessel internal-rate strike/rub reference renderer\n"
                    "--output PATH --report PATH --bowl metal|crystal --mallet wood|suede|silicone|felt\n"
                    "--seconds 0.1..60 --rate 176400..768000 --pitch 20..2000 --velocity 0..1\n"
                    "--retrigger-at SECONDS --width 0..1\n"
                    "--rotate-at SECONDS --release-at SECONDS --stop-at SECONDS\n"
                    "--speed -2..2 --pressure 0..15 --radial-load 0|1\n"
                    "Velocity 0 skips strikes. Stop retains contact; release lifts it.\n"
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
            else if (key == "--rotate-at") rotateAt = numeric(value);
            else if (key == "--release-at") releaseAt = numeric(value);
            else if (key == "--stop-at") stopAt = numeric(value);
            else if (key == "--speed") speed = numeric(value);
            else if (key == "--pressure") pressure = numeric(value);
            else if (key == "--radial-load") {
                if (value != "0" && value != "1") throw std::runtime_error("radial-load must be 0 or 1");
                settings.prescribedRadialLoad = value == "1";
            }
            else if (key == "--width") {
                const double width = numeric(value);
                if (width < 0 || width > 1) throw std::runtime_error("width must be in 0..1");
                settings.observerSeparation = pi/6.0*width;
            }
            else throw std::runtime_error("unknown option "+key);
        }
        if (seconds < .1 || seconds > 60 || velocity < 0 || velocity > 1
            || (retrigger >= 0 && (retrigger <= .05 || retrigger >= seconds))) throw std::runtime_error("invalid render/event range");
        if ((rotateAt >= 0 && rotateAt >= seconds) || (releaseAt >= 0 && (rotateAt < 0 || releaseAt <= rotateAt || releaseAt >= seconds))
            || (stopAt >= 0 && (rotateAt < 0 || stopAt <= rotateAt || stopAt >= seconds))) throw std::runtime_error("invalid rotation event order");
        VesselEngine engine;
        if (!engine.configure(seedBowls[bowlIndex], seedMallets[malletIndex], settings, rate))
            throw std::runtime_error("unsupported engine configuration");
        engine.setAuditEnabled(true);
        if (!engine.setRotation(false, speed, pressure)) throw std::runtime_error("invalid speed/pressure");
        engine.reset(); // Start at the requested speed without a hidden initial speed ramp.
        const double requestedSpeed = speed;
        bool rotationEngaged = false;
        const std::uint32_t frames = std::uint32_t(std::ceil(seconds*rate));
        const std::uint32_t dataBytes = frames*8u;
        const auto firstStrike = std::uint32_t(std::llround(.05*rate));
        const auto nextStrike = retrigger >= 0 ? std::uint32_t(std::llround(retrigger*rate)) : frames;
        const auto rotationFrame = rotateAt >= 0 ? std::uint32_t(std::llround(rotateAt*rate)) : frames;
        const auto releaseFrame = releaseAt >= 0 ? std::uint32_t(std::llround(releaseAt*rate)) : frames;
        const auto stopFrame = stopAt >= 0 ? std::uint32_t(std::llround(stopAt*rate)) : frames;
        std::ofstream wav(output, std::ios::binary);
        if (!wav) throw std::runtime_error("cannot open WAV "+output);
        wav.write("RIFF", 4); u32(wav, 36+dataBytes); wav.write("WAVEfmt ", 8);
        u32(wav, 16); u16(wav, 3); u16(wav, 2); u32(wav, rate);
        u32(wav, rate*8u); u16(wav, 8); u16(wav, 32);
        wav.write("data", 4); u32(wav, dataBytes);
        double peak = 0, peakForce = 0, maxCompression = 0, worstCompliance = 1.0;
        double peakFriction = 0.0, maxEnergy = 0.0, maxDisplacementRatio = 0.0;
        struct Window { double end, energy, rms, hand, loss, radial; };
        std::vector<Window> windows;
        double energySum = 0.0, outputSum = 0.0, lastHand = 0.0, lastLoss = 0.0, lastRadial = 0.0;
        unsigned windowFrames = 0;
        for (std::size_t j = 0; j < engine.bowl().size(); ++j)
            worstCompliance = std::min(worstCompliance, engine.bowl().coefficients(j).staticComplianceRatio);
        const auto started = std::chrono::steady_clock::now();
        for (std::uint32_t i = 0; i < frames; ++i) {
            if (i == firstStrike || i == nextStrike) engine.strike(velocity);
            if (i == rotationFrame) { rotationEngaged = true; engine.setRotation(true, speed, pressure); }
            if (i == stopFrame) { speed = 0.0; engine.setRotation(rotationEngaged, speed, pressure); }
            if (i == releaseFrame) { rotationEngaged = false; engine.setRotation(false, speed, pressure); }
            const auto frame = engine.step();
            if (frame.fault) throw std::runtime_error("mechanical fault while rendering");
            peak = std::max(peak, std::max(std::abs(frame.leftVelocity), std::abs(frame.rightVelocity)));
            peakForce = std::max(peakForce, frame.strikeForce);
            maxCompression = std::max(maxCompression, frame.compression);
            peakFriction = std::max(peakFriction, std::abs(frame.frictionForce));
            maxEnergy = std::max(maxEnergy, engine.bowl().energy());
            double displacement = 0.0;
            for (std::size_t j = 0; j < engine.bowl().size(); ++j) {
                const auto& c = engine.bowl().coefficients(j);
                displacement += std::abs(engine.bowl().state(j).x)*c.inverseRootMass/c.omega;
            }
            maxDisplacementRatio = std::max(maxDisplacementRatio, displacement/seedBowls[bowlIndex].rimRadius);
            energySum += engine.bowl().energy();
            outputSum += .5*(frame.leftVelocity*frame.leftVelocity+frame.rightVelocity*frame.rightVelocity);
            ++windowFrames;
            if (windowFrames == unsigned(rate/2) || i+1 == frames) {
                const auto& l = engine.ledger();
                const double loss = l.modalLoss+l.contactLoss+l.frictionLoss;
                windows.push_back({double(i+1)/rate, energySum/windowFrames, std::sqrt(outputSum/windowFrames),
                    l.handWork-lastHand, loss-lastLoss, l.radialWork-lastRadial});
                lastHand = l.handWork; lastLoss = loss; lastRadial = l.radialWork;
                energySum = outputSum = 0.0; windowFrames = 0;
            }
            sample(wav, frame.leftVelocity); sample(wav, frame.rightVelocity);
        }
        wav.close();
        if (!wav) throw std::runtime_error("WAV write failed");
        const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now()-started).count();
        const auto& ledger = engine.ledger();
        const double residual = engine.totalEnergy()+ledger.modalLoss+ledger.contactLoss
            +ledger.retiredEnergy+ledger.recoveryLoss+ledger.frictionLoss
            -ledger.launchWork-ledger.speedCapWork-ledger.handWork-ledger.radialWork;
        std::ostream* destination = &std::cout;
        std::ofstream json;
        if (!report.empty()) {
            json.open(report);
            if (!json) throw std::runtime_error("cannot open report "+report);
            destination = &json;
        }
        auto& out = *destination;
        out << std::setprecision(17) << "{\n"
            << "  \"scope\": \"internal-rate coupled strike/rub reference; uncalibrated, no Rack adapter\",\n"
            << "  \"bowl\": \"" << seedBowls[bowlIndex].stableId << "\",\n"
            << "  \"mallet\": \"" << seedMallets[malletIndex].stableId << "\",\n"
            << "  \"internal_rate_hz\": " << rate << ",\n"
            << "  \"frames\": " << frames << ",\n"
            << "  \"frequency_hz\": " << settings.frequency << ",\n"
            << "  \"requested_rotation_speed_rps\": " << requestedSpeed << ",\n"
            << "  \"requested_pressure_N\": " << pressure << ",\n"
            << "  \"rotate_at_s\": " << rotateAt << ",\n"
            << "  \"release_at_s\": " << releaseAt << ",\n"
            << "  \"stop_at_s\": " << stopAt << ",\n"
            << "  \"strikes\": " << ledger.strikes << ",\n"
            << "  \"launch_work_J\": " << ledger.launchWork << ",\n"
            << "  \"modal_loss_J\": " << ledger.modalLoss << ",\n"
            << "  \"contact_loss_J\": " << ledger.contactLoss << ",\n"
            << "  \"hand_work_J\": " << ledger.handWork << ",\n"
            << "  \"friction_loss_J\": " << ledger.frictionLoss << ",\n"
            << "  \"radial_work_J\": " << ledger.radialWork << ",\n"
            << "  \"prescribed_radial_load\": " << (settings.prescribedRadialLoad ? "true" : "false") << ",\n"
            << "  \"max_friction_uniqueness\": " << engine.maxFrictionUniqueness() << ",\n"
            << "  \"peak_friction_force_N\": " << peakFriction << ",\n"
            << "  \"max_bowl_energy_J\": " << maxEnergy << ",\n"
            << "  \"max_rim_displacement_bound_over_radius\": " << maxDisplacementRatio << ",\n"
            << "  \"retired_energy_J\": " << ledger.retiredEnergy << ",\n"
            << "  \"remaining_energy_J\": " << engine.totalEnergy() << ",\n"
            << "  \"cumulative_energy_residual_J\": " << residual << ",\n"
            << "  \"max_step_energy_residual_J\": " << ledger.maxStepResidual << ",\n"
            << "  \"peak_observer_velocity_m_per_s\": " << peak << ",\n"
            << "  \"peak_contact_force_N\": " << peakForce << ",\n"
            << "  \"max_indentation_m\": " << maxCompression << ",\n"
            << "  \"worst_mode_static_compliance_ratio\": " << worstCompliance << ",\n"
            << "  \"max_solver_iterations\": " << ledger.maxSolverIterations << ",\n"
            << "  \"max_friction_iterations\": " << ledger.maxFrictionIterations << ",\n"
            << "  \"solver_faults\": " << ledger.solverFaults << ",\n"
            << "  \"speed_caps\": " << ledger.speedCaps << ",\n"
            << "  \"elapsed_render_with_io_seconds\": " << elapsed << ",\n"
            << "  \"windows\": [\n";
        for (std::size_t i = 0; i < windows.size(); ++i) {
            const auto& w = windows[i];
            out << "    {\"end_s\": " << w.end << ", \"mean_bowl_energy_J\": " << w.energy
                << ", \"observer_rms_m_per_s\": " << w.rms << ", \"hand_work_J\": " << w.hand
                << ", \"loss_J\": " << w.loss << ", \"radial_work_J\": " << w.radial << "}"
                << (i+1 < windows.size() ? ",\n" : "\n");
        }
        out << "  ],\n  \"friction_iteration_histogram\": [";
        for (unsigned i = 0; i <= 80; ++i) out << (i ? ", " : "") << ledger.frictionIterationHistogram[i];
        out << "],\n  \"strike_iteration_histogram\": [";
        for (unsigned i = 0; i <= 80; ++i) out << (i ? ", " : "") << ledger.strikeIterationHistogram[i];
        out << "]\n}\n";
        if (!out) throw std::runtime_error("report write failed");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Vessel render: " << error.what() << '\n'; return 1;
    }
}
