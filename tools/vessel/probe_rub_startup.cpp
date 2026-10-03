// Offline investigation; never modifies production profiles or engine state rules.
// Build with VesselEngine, ModalBank, FrictionContact, StrikeContact, ContactSolver.
#include "vessel/VesselEngine.hpp"
#include "vessel/SeedProfiles.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

using namespace vessel;

static void checkLaw() {
    for (const auto& m : seedMallets) for (double slip : {-1.,-.4,-.1,-.001,-.00001,0.,.00001,.001,.1,.4,1.}) {
        const auto f = frictionValue(slip,15,m);
        const double epsilon = 1e-8;
        const double difference = (frictionValue(slip+epsilon,15,m).force
            - frictionValue(slip-epsilon,15,m).force)/(2*epsilon);
        if (f.force*slip < 0 || f.derivative < -frictionNegativeSlopeBound(15,m)-1e-10
            || std::abs(f.derivative-difference) > 1e-4*std::max(1.,std::abs(difference)))
            throw std::runtime_error("friction derivative/dissipation/bound check failed");
    }
}

static void run(const char* group, int bowl, int material, double speed,
                double pressure = 2.5, double weakeningScale = 1,
                bool radial = false, double seconds = 12, double rate = 192000,
                bool primed = false) {
    auto mallet = seedMallets[material];
    mallet.weakeningVelocity *= weakeningScale;
    EngineSettings settings;
    settings.prescribedRadialLoad = radial;
    VesselEngine engine;
    if (!engine.configure(seedBowls[bowl], mallet, settings, rate))
        throw std::runtime_error("configuration rejected");
    engine.setAuditEnabled(true);
    if (primed) {
        engine.setRotation(true, .4, pressure);
        for (int i = 0; i < int(8*rate); ++i) engine.step();
    }
    const double initialEnergy = engine.totalEnergy();
    const auto initialLedger = engine.ledger();
    engine.setRotation(true, speed, pressure);
    double onset = -1, energySum = 0, kineticSum = 0, velocitySqSum = 0;
    double peak = 0, maxChi = 0;
    const int samples = int(seconds*rate), lastSecond = samples-int(rate);
    for (int i = 0; i < samples; ++i) {
        const auto frame = engine.step();
        if (frame.fault) throw std::runtime_error("solver fault");
        const double energy = engine.bowl().energy();
        if (onset < 0 && energy >= .001) onset = (i+1)/rate;
        peak = std::max(peak, energy);
        maxChi = std::max(maxChi, frame.uniquenessNumber);
        if (i >= lastSecond) {
            energySum += energy;
            for (std::size_t j = 0; j < engine.bowl().size(); ++j) {
                const double y = engine.bowl().state(j).y;
                kineticSum += .5*y*y;
            }
            velocitySqSum += frame.leftVelocity*frame.leftVelocity;
        }
    }
    const auto& l = engine.ledger();
    const double hand = l.handWork-initialLedger.handWork;
    const double loss = l.frictionLoss-initialLedger.frictionLoss;
    const double radialWork = l.radialWork-initialLedger.radialWork;
    const double damping = l.modalLoss-initialLedger.modalLoss;
    const double residual = engine.totalEnergy()-initialEnergy+damping+loss-hand-radialWork;
    const double U = 2*pi*seedBowls[bowl].rimRadius*speed;
    const double slope = frictionValue(U, pressure, mallet).derivative;
    std::printf("%s,%d,%d,%.3g,%.3g,%.3g,%d,%.0f,%.0f,%d,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%llu,%llu\n",
        group,bowl,material,speed,pressure,weakeningScale,radial,seconds,rate,primed,
        U,slope,onset,energySum/rate,kineticSum/rate,std::sqrt(velocitySqSum/rate),peak,
        hand,loss,radialWork,residual,maxChi,
        (unsigned long long)l.solverFaults,(unsigned long long)l.speedCaps);
    std::fflush(stdout);
    if (std::abs(residual) > 1e-7 || l.solverFaults || l.speedCaps || l.nonfiniteResets)
        throw std::runtime_error("energy or safety audit failed");
}

int main(int argc, char** argv) {
    const char* group = argc > 1 ? argv[1] : "baseline";
    std::puts("group,bowl,mallet,speed_rps,pressure_N,vc_scale,radial,seconds,rate,primed,hand_mps,friction_slope,onset_1mJ_s,late_energy_J,late_kinetic_J,pickup_rms_mps,peak_energy_J,hand_work_J,friction_loss_J,radial_work_J,balance_residual_J,max_chi,faults,caps");
    try {
        checkLaw();
        if (!std::strcmp(group,"baseline")) {
            for (int bowl : {0,1}) for (int m : {0,1,2,3})
                for (double speed : {.2,.4,.8,1.2,2.0}) run(group,bowl,m,speed);
        } else if (!std::strcmp(group,"contact")) {
            for (int m : {0,1}) for (double speed : {.4,.8,2.0}) {
                run(group,0,m,speed,2.5,1,true);
                run(group,0,m,speed,7.5);
                run(group,0,m,speed,15);
                run(group,0,m,speed,2.5,2);
                run(group,0,m,speed,2.5,4);
            }
        } else if (!std::strcmp(group,"joint")) {
            for (double speed : {.2,.4,.8,2.0}) {
                run(group,0,1,speed,7.5,2);
                run(group,0,1,speed,7.5,4);
                run(group,0,1,speed,15,4);
            }
        } else if (!std::strcmp(group,"law")) {
            for (int m : {0,1}) for (double speed : {.4,.8,2.0})
                for (double pressure : {2.5,7.5}) run(group,0,m,speed,pressure);
        } else if (!std::strcmp(group,"convergence")) {
            for (double rate : {96000.,384000.})
                for (double speed : {.4,.8,2.0}) run(group,0,1,speed,2.5,1,false,12,rate);
            for (double speed : {.8,2.0}) run(group,0,1,speed,2.5,1,false,60);
            run(group,0,1,2.0,2.5,1,false,12,192000,true);
        } else throw std::runtime_error("unknown group");
    } catch (const std::exception& error) {
        std::fprintf(stderr,"FAIL: %s\n",error.what()); return 1;
    }
}
