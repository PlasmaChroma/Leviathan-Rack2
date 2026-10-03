// Offline investigation; never modifies production profiles or engine state rules.
// Build with VesselEngine, ModalBank, FrictionContact, StrikeContact, ContactSolver.
#include "vessel/VesselEngine.hpp"
#include "vessel/SeedProfiles.hpp"
#include "vessel/RubIntensity.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>

using namespace vessel;
// Historical timer controller retained only for offline comparisons.
// Contact approach, not an energy envelope: brief hand acceleration, slower load.
// Release resets both approaches; the engine smooths the force release.
struct RubIntensityAttack {
    static constexpr double approachSeconds = 5.;
    static constexpr double speedApproachSeconds = .5;
    double approach = 0;
    double process(double amount, bool engaged, double dt) noexcept {
        approach = engaged && amount > 0 ? std::min(1., approach + dt/approachSeconds) : 0;
        // Smoothstep gives gentle initial contact and arrival at full effort.
        return amount*approach*approach*(3.-2.*approach);
    }
    // Read after process() so speed and pressure share the same contact onset.
    double speedScale() const noexcept {
        const double t = std::min(1., approach*(approachSeconds/speedApproachSeconds));
        return t*t*(3.-2.*t);
    }
};


static std::FILE* trace = nullptr;

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
                bool primed = false, bool intensityApproach = false) {
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
    RubIntensityAttack attack;
    RubIntensityPlayer player;
    double transfer = 0, slidingLoss = 0, slipSq = 0, loadSum = 0, handSum = 0;
    const int traceSamples = int(rate*.1);
    for (int i = 0; i < samples; ++i) {
        if (!std::strncmp(group,"intensity-player",16)) {
            const auto gesture = player.process(pressure/15.,true,engine.bowl().energy(),speed,
                rubIntensityMaximumSpeed(seedBowls[bowl],mallet,.8),1/rate);
            engine.setRotation(true,gesture.speed,gesture.pressure);
        } else if (intensityApproach) {
            const double amount = attack.process(1.,true,1/rate);
            engine.setRotation(true, speed*attack.speedScale(), pressure*amount);
        }
        const auto frame = engine.step();
        transfer += frame.frictionForce*(frame.handSpeed-frame.slip);
        slidingLoss += frame.frictionForce*frame.slip;
        slipSq += frame.slip*frame.slip;
        loadSum += frame.normalLoad; handSum += frame.handSpeed;
        if (trace && (i+1)%traceSamples == 0) {
            std::fprintf(trace,"%d,%d,%.3g,%.3f,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g\n",
                bowl,material,pressure/15.,(i+1)/rate,engine.bowl().energy(),
                handSum/traceSamples,loadSum/traceSamples,transfer/traceSamples,
                slidingLoss/traceSamples,std::sqrt(slipSq/traceSamples));
            transfer = slidingLoss = slipSq = loadSum = handSum = 0;
        }
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
    if (argc > 2) {
        trace = std::fopen(argv[2],"w");
        if (!trace) return 1;
        std::fprintf(trace,"bowl,mallet,intensity,time_s,energy_J,hand_mps,load_N,transfer_W,sliding_loss_W,slip_rms_mps\n");
    }
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
        } else if (!std::strcmp(group,"intensity-player-rates")) {
            for (int bowl : {0,1}) for (double rate : {96000.,384000.})
                run(group,bowl,0,rubIntensityMaximumSpeed(seedBowls[bowl],seedMallets[0],2.),15,1,false,12,rate);
        } else if (!std::strcmp(group,"intensity-player")) {
            for (int bowl : {0,1}) for (int m : {0,1,2,3}) for(double amount : {.5,1.})
                run(group,bowl,m,rubIntensityMaximumSpeed(seedBowls[bowl],seedMallets[m],m == 0 ? 2. : 1.2),15*amount,1,false,20);
        } else if (!std::strcmp(group,"intensity-ramp")) {
            for (int bowl : {0,1}) for (int m : {0,1,2,3})
                run(group,bowl,m,rubIntensityMaximumSpeed(seedBowls[bowl],seedMallets[m],m == 0 ? 2. : 1.2),15,1,false,12,192000,false,true);
        } else if (!std::strcmp(group,"wood-range")) {
            for (int bowl : {0,1}) for (double factor : {1.2,1.6,2.,2.4,3.})
                run(group,bowl,0,factor*seedMallets[0].weakeningVelocity/(2*pi*seedBowls[bowl].rimRadius),15);
        } else if (!std::strcmp(group,"intensity")) {
            for (int bowl : {0,1}) for (int m : {0,1,2,3})
                for (double amount : {.25,.5,1.}) {
                    const auto gesture = rubIntensity(amount,
                        rubIntensityMaximumSpeed(seedBowls[bowl],seedMallets[m]));
                    run(group,bowl,m,gesture.speed,gesture.pressure);
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
    if (trace) std::fclose(trace);
}
