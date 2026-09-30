// Offline mechanical diagnostics, not calibration or a Rack CPU certification.
#include "../../src/vessel/VesselEngine.hpp"
#include "../../src/vessel/SeedProfiles.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {
using namespace vessel;
void run(const char* group, const BowlDescriptor& bowl, MalletDescriptor m,
         double rate, double speed, double pressure, double epsilonScale = 1.0,
         bool radial = false, double pitch = 261.625565, bool strikes = false) {
    m.regularizationVelocity *= epsilonScale;
    VesselEngine e;
    EngineSettings settings;
    settings.frequency = pitch; settings.prescribedRadialLoad = radial;
    if (!e.configure(bowl, m, settings, rate)) throw std::runtime_error("characterization configuration rejected");
    e.setAuditEnabled(true);
    e.setRotation(false, speed, pressure); e.reset(); e.setRotation(true, speed, pressure);
    const int count = int(15*rate), lateBegin = int(12*rate), middleBegin = int(9*rate);
    double early = 0, late = 0, lateRms = 0, maxEnergy = 0, displacementRatio = 0;
    double onset = -1.0, beforeHand = 0, beforeLoss = 0, beforeRadial = 0;
    double lateMin = 1e100, lateMax = 0;
    std::array<double, maxPairs> pairEnergy {};
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < count; ++i) {
        if (strikes && (i == int(4*rate) || i == int(8*rate) || i == int(12*rate))) e.strike(.5);
        const auto f = e.step();
        if (f.fault) throw std::runtime_error("mechanical fault in characterization");
        const double energy = e.bowl().energy();
        maxEnergy = std::max(maxEnergy, energy);
        if (onset < 0 && energy >= 1e-4) onset = double(i+1)/rate;
        if (i >= middleBegin && i < lateBegin) early += energy/(3*rate);
        if (i == lateBegin) {
            const auto& l = e.ledger();
            beforeHand = l.handWork; beforeLoss = l.modalLoss+l.contactLoss+l.frictionLoss;
            beforeRadial = l.radialWork;
        }
        if (i >= lateBegin) {
            late += energy/(3*rate);
            lateRms += .5*(f.leftVelocity*f.leftVelocity+f.rightVelocity*f.rightVelocity)/(3*rate);
            lateMin = std::min(lateMin, energy); lateMax = std::max(lateMax, energy);
            for (std::size_t n = 0; n < bowl.pairCount; ++n)
                for (std::size_t side = 0; side < 2; ++side) {
                    const auto& s = e.bowl().state(2*n+side);
                    pairEnergy[n] += .5*(s.x*s.x+s.y*s.y)/(3*rate);
                }
        }
        double displacement = 0;
        for (std::size_t j = 0; j < e.bowl().size(); ++j) {
            const auto& c = e.bowl().coefficients(j);
            displacement += std::abs(e.bowl().state(j).x)*c.inverseRootMass/c.omega;
        }
        displacementRatio = std::max(displacementRatio, displacement/bowl.rimRadius);
    }
    const double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    const auto& l = e.ledger();
    const double residual = e.totalEnergy()+l.modalLoss+l.contactLoss+l.frictionLoss+l.retiredEnergy+l.recoveryLoss
        -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
    const auto dominant = std::size_t(std::max_element(pairEnergy.begin(), pairEnergy.begin()+bowl.pairCount)-pairEnergy.begin());
    std::cout << group << ',' << bowl.stableId << ',' << m.stableId << ',' << rate << ',' << pitch << ',' << speed << ','
        << pressure << ',' << epsilonScale << ',' << radial << ',' << strikes << ',' << onset << ',' << early << ',' << late << ','
        << lateMin << ',' << lateMax << ',' << std::sqrt(lateRms) << ',' << bowl.pairs[dominant].order << ',' << pairEnergy[0]/std::max(1e-30, late) << ','
        << maxEnergy << ',' << displacementRatio << ',' << l.handWork-beforeHand << ','
        << l.modalLoss+l.contactLoss+l.frictionLoss-beforeLoss << ',' << l.radialWork-beforeRadial << ',' << residual << ','
        << l.maxStepResidual << ',' << l.solverFaults << ',' << l.speedCaps << ',' << l.maxFrictionIterations << ','
        << l.maxSolverIterations << ',' << e.maxFrictionUniqueness() << ',' << elapsed << '\n';
    if (l.solverFaults || l.speedCaps || std::abs(residual) > 1e-8)
        throw std::runtime_error("characterization energy/protection gate failed");
}
}

int main() {
    try {
        using namespace vessel;
        std::cout << std::setprecision(17)
            << "group,bowl,mallet,rate_hz,pitch_hz,speed_rps,pressure_N,epsilon_scale,radial_load,strikes,onset_energy_1e-4_s,"
               "mean_energy_9_12_J,mean_energy_12_15_J,late_min_J,late_max_J,late_observer_rms_m_s,dominant_order,fundamental_energy_fraction,"
               "max_energy_J,max_displacement_bound_over_radius,late_hand_work_J,late_loss_J,late_radial_work_J,"
               "cumulative_residual_J,max_step_residual_J,solver_faults,speed_caps,max_friction_iterations,max_strike_iterations,certificate,audited_elapsed_s\n";
        for (const auto& b : seedBowls) for (const auto& m : seedMallets)
            for (double rate : {192000.0, 384000.0, 768000.0}) run("seeds_rates", b, m, rate, .4, 2.5);
        for (double speed : {-.4, .3, .4, .5}) for (double pressure : {2.0, 2.5, 3.0})
            run("default_neighborhood", seedBowls[0], seedMallets[1], 192000, speed, pressure);
        for (double epsilon : {.5, 2.0}) for (double rate : {192000.0, 384000.0, 768000.0})
            run("epsilon_rates", seedBowls[0], seedMallets[1], rate, .4, 2.5, epsilon);
        for (double rate : {192000.0, 384000.0, 768000.0})
            run("radial_rates", seedBowls[0], seedMallets[1], rate, .4, 2.5, 1, true);
        run("coupled", seedBowls[0], seedMallets[1], 192000, .4, 2.5, 1, false, 261.625565, true);
        auto constant = seedMallets[1]; constant.muS = constant.muK;
        run("no_weakening_control", seedBowls[0], constant, 192000, .4, 2.5);
        for (double pitch : {20.0, 100.0, 1000.0, 2000.0})
            run("pitch", seedBowls[0], seedMallets[1], 192000, .4, 2.5, 1, false, pitch);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Vessel characterization: " << error.what() << '\n'; return 1;
    }
}
