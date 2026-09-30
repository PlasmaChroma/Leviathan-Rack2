#include "../src/vessel/VesselEngine.hpp"
#include "../src/vessel/SeedProfiles.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>

namespace {
using namespace vessel;
int passed = 0;
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
bool close(double a, double b, double relative = 1e-10, double absolute = 1e-14) {
    return std::abs(a-b) <= absolute+relative*std::max(std::abs(a), std::abs(b));
}
void pass(const char* name) { ++passed; std::cout << "[PASS] " << name << '\n'; }

void polesAndConfiguration() {
    ModalBank bank;
    for (double rate : {176400.0, 192000.0, 352800.0, 384000.0, 768000.0})
        for (const auto& profile : seedBowls) for (double frequency : {20.0, 261.625565, 2000.0})
            for (double decay : {0.25, 1.0, 4.0}) {
                require(bank.configure(profile, frequency, decay, 1.0, rate), "valid profile rejected");
                for (std::size_t j = 0; j < bank.size(); ++j) {
                    const auto& c = bank.coefficients(j);
                    const double D = 1.0/c.inverseD;
                    const double A00 = 1.0-2.0*c.a*c.a/D, A01 = 2.0*c.a/D;
                    const double A10 = -A01, A11 = 2.0/D-1.0;
                    const double r = std::exp(-log1000/(rate*c.t60));
                    require(close(A00+A11, 2.0*r*std::cos(2.0*pi*c.frequency/rate)), "pole trace");
                    require(close(A00*A11-A01*A10, r*r), "pole determinant");
                }
            }
    bank.setState(0, 0.123, -0.456);
    const double energy = bank.energy(), f = bank.coefficients(0).frequency;
    require(!bank.configure(seedBowls[0], 100.0, 1.0, 1.0, 100.0), "bad rate accepted");
    require(!bank.configure(seedBowls[0], std::numeric_limits<double>::quiet_NaN(), 1.0, 1.0, 192000.0), "NaN pitch accepted");
    auto invalid = seedBowls[0]; invalid.pairs[2].massA = 0.0;
    require(!bank.configure(invalid, 100.0, 1.0, 1.0, 192000.0), "zero mass accepted");
    require(bank.energy() == energy && bank.coefficients(0).frequency == f, "failed setup mutated state");
    require(bank.configure(seedBowls[0], 100.0, 1.0, 0.0, 192000.0), "retune rejected");
    require(bank.energy() == energy, "retune changed normalized energy");
    require(bank.size() == 14 && bank.coefficients(0).frequency == bank.coefficients(1).frequency, "degenerate B mode lost");
    pass("Exact modal poles across profiles/rates/pitches; transactional validation and retuning");
}

void modalEnergyAndPorts() {
    ModalBank bank;
    require(bank.configure(seedBowls[0], 261.625565, 1.0, 1.0, 192000.0), "modal setup");
    std::mt19937 rng(9302026);
    std::uniform_real_distribution<double> random(-1.0, 1.0);
    double largestResidual = 0.0;
    for (int i = 0; i < 10000; ++i) {
        ModalVector force {};
        for (std::size_t j = 0; j < bank.size(); ++j) {
            bank.setState(j, random(rng), random(rng)); force[j] = random(rng)*30.0;
        }
        const auto audit = bank.commit(bank.freeMidpoint(), force, true);
        largestResidual = std::max(largestResidual, std::abs(audit.residual));
        require(std::abs(audit.residual) < 1e-12, "collocated energy identity");
        const auto tail = bank.commit(bank.freeMidpoint(), ModalVector{}, true);
        require(tail.energyAfter <= tail.energyBefore+1e-14, "unforced energy grew");
    }
    const auto radial = bank.radialPort(.3, .07), inward = bank.radialPort(.3, .07, true);
    const auto tangential = bank.tangentialPort(.4, .07);
    const double Yss = bank.admittance(inward, inward), Ytt = bank.admittance(tangential, tangential);
    const double Yst = bank.admittance(inward, tangential);
    require(close(Yst, bank.admittance(tangential, inward)) && Yss*Ytt >= Yst*Yst-1e-20, "port PSD/reciprocity");
    for (std::size_t j = 0; j < bank.size(); ++j) require(radial[j] == -inward[j], "inward radial sign");
    const auto point = bank.radialPort(.3, 0.0), wide = bank.radialPort(.3, .2);
    for (std::size_t j = 0; j < bank.size(); ++j) require(std::abs(wide[j]) <= std::abs(point[j])+1e-14, "patch spatial attenuation");
    bank.clear(); bank.commit(bank.freeMidpoint(), ModalVector{}, true);
    require(bank.energy() == 0.0 && bank.velocity(radial) == 0.0, "rest generates motion");
    std::cout << "  max modal residual=" << largestResidual << " J\n";
    pass("10,000 randomized modal energy steps; reciprocal ports, patch coupling, and exact silence");
}

void staticCompliance() {
    ModalBank bank;
    require(bank.configure(seedBowls[1], 2000.0, 1.0, 1.0, 192000.0), "compliance setup");
    ModalVector force {};
    for (std::size_t j = 0; j < bank.size(); ++j) {
        const auto& c = bank.coefficients(j);
        force[j] = c.inverseRootMass;
        bank.setState(j, force[j]/c.omega, 0.0);
    }
    const double before = bank.energy();
    for (int i = 0; i < 100; ++i) bank.commit(bank.freeMidpoint(), force);
    require(close(bank.energy(), before), "static equilibrium moved");
    for (std::size_t j = 0; j < bank.size(); ++j) {
        const auto& c = bank.coefficients(j);
        const double q = bank.state(j).x*c.inverseRootMass/c.omega;
        const double continuousCompliance = 1.0/(c.mass*(std::pow(2*pi*c.frequency, 2)+std::pow(log1000/c.t60, 2)));
        require(close(q/continuousCompliance, c.staticComplianceRatio), "forced static compliance diagnostic");
    }
    pass("Forced static equilibrium and prewarped compliance are explicitly measured");
}

void elasticGroundImpact() {
    auto mallet = seedMallets[0]; mallet.loadingDamping = 0.0;
    const double velocity0 = .5, energy0 = .5*mallet.mass*velocity0*velocity0;
    const double dMax = std::pow(2.5*energy0/mallet.stiffness, 1.0/2.5);
    const double beta = std::tgamma(.4)*std::tgamma(.5)/std::tgamma(.9);
    const double exactDuration = 2.0*dMax/velocity0*.4*beta;
    double previousError = 1.0;
    for (double rate : {192000.0, 384000.0, 768000.0}) {
        const double h = 1.0/rate;
        double d = 0.0, v = velocity0, peak = 0.0, duration = 0.0;
        bool separated = false;
        for (int i = 0; i < 10000; ++i) {
            const double previousD = d;
            const auto s = solveStrike(d, v, 0.0, 0.0, h, mallet);
            require(s.converged, "elastic ground solver fault");
            d = s.compression; v -= h*s.force/mallet.mass;
            peak = std::max(peak, d); duration += h;
            const double e = .5*mallet.mass*v*v + strikePotential(d, mallet.stiffness);
            require(close(e, energy0, 2e-8, 2e-12), "elastic ground energy");
            if (d <= 0.0 && v < 0.0) {
                // Interpolate the zero-overlap event for comparison rather
                // than letting endpoint sample quantization conceal convergence.
                duration -= h*(-d)/(previousD-d);
                separated = true; break;
            }
        }
        require(separated, "elastic collision did not separate");
        require(close(v, -velocity0, 2e-8), "elastic restitution differs from one");
        require(std::abs(peak/dMax-1.0) < .004, "elastic maximum indentation");
        const double error = std::abs(duration/exactDuration-1.0);
        require(error < .002 && error < previousError, "elastic contact duration convergence");
        previousError = error;
        std::cout << "  elastic ground rate=" << rate << " duration=" << duration
                  << " exact=" << exactDuration << " relative error=" << error << '\n';
    }
    pass("Elastic 3/2-power impact matches independent energy, indentation, restitution, and contact-time solution");
}

void fullCollisions() {
    double largestResidual = 0.0;
    unsigned maxIterations = 0;
    std::size_t cases = 0;
    for (const auto& bowl : seedBowls) for (const auto& mallet : seedMallets)
        for (double rate : {176400.0, 192000.0, 384000.0}) for (int u = 1; u <= 16; ++u) {
            VesselEngine engine;
            require(engine.configure(bowl, mallet, EngineSettings{}, rate), "collision profile setup");
            engine.setAuditEnabled(true);
            const double velocity = double(u)/16;
            require(engine.strike(velocity), "launch rejected");
            const double launched = .5*mallet.mass*velocity*velocity;
            double peakForce = 0.0;
            bool separated = false;
            for (int i = 0; i < int(rate*.05); ++i) {
                const auto frame = engine.step();
                require(!frame.fault, "full collision solver fault");
                peakForce = std::max(peakForce, frame.strikeForce);
                if (frame.separated) separated = true;
                require(engine.totalEnergy() <= launched+1e-10, "collision creates energy");
            }
            const auto& ledger = engine.ledger();
            const double residual = engine.totalEnergy()+ledger.modalLoss+ledger.contactLoss+ledger.retiredEnergy-launched;
            require(separated && !engine.strikerActive() && peakForce > 0.0 && engine.bowl().energy() > 0.0,
                    "impact did not transfer energy and separate");
            require(std::abs(residual) < 1e-9 && ledger.maxStepResidual < 1e-10, "complete collision energy ledger");
            require(ledger.solverFaults == 0 && ledger.speedCaps == 0, "ordinary strike used recovery/cap");
            largestResidual = std::max(largestResidual, std::abs(residual));
            maxIterations = std::max(maxIterations, ledger.maxSolverIterations); ++cases;
        }
    std::cout << "  collisions=" << cases << " max cumulative residual=" << largestResidual
              << " J max solver iterations=" << maxIterations << '\n';
    pass("384 complete bowl/mallet/velocity/rate trajectories include separation and energy retirement");
}

void transitionsAndRetriggers() {
    VesselEngine engine; engine.setAuditEnabled(true);
    require(!engine.strike(0.0) && !engine.strike(std::numeric_limits<double>::quiet_NaN()), "invalid strike creates contact");
    require(engine.strike(.5), "transition launch");
    const auto first = engine.step();
    require(engine.strikerActive() && engine.compression() > 0.0 && !first.fault, "contact not compressing");
    const double beforeZero = engine.totalEnergy();
    require(!engine.strike(0.0) && engine.totalEnergy() == beforeZero, "zero retrigger changes state");
    VesselEngine control = engine;
    auto settings = engine.settings(); settings.strikeAngle = 1.0;
    require(engine.configure(seedBowls[0], seedMallets[0], settings, 192000.0), "active material switch failed");
    require(engine.totalEnergy() == control.totalEnergy(), "active material switch changes spring/kinetic energy");
    for (int i = 0; i < 2000; ++i) {
        const auto a = engine.step(), b = control.step();
        require(!a.fault && a.strikeForce == b.strikeForce && engine.totalEnergy() == control.totalEnergy(), "active mallet did not latch");
    }
    const double bowlBefore = engine.bowl().energy(); engine.strike(.7);
    require(engine.bowl().energy() == bowlBefore, "retrigger clears ringing tail");
    for (int i = 0; i < 8; ++i) engine.strike(1.0);
    require(engine.ledger().speedCaps > 0 && std::abs(engine.strikerVelocity()) <= 4.0, "speed cap policy");
    require(engine.configure(seedBowls[0], seedMallets[0], settings, 384000.0), "active sample-rate change");
    for (int i = 0; i < 12000; ++i) require(!engine.step().fault, "retrigger/rate-change trajectory failed");
    const auto& ledger = engine.ledger();
    const double residual = engine.totalEnergy()+ledger.modalLoss+ledger.contactLoss
        +ledger.retiredEnergy+ledger.recoveryLoss-ledger.launchWork-ledger.speedCapWork;
    require(std::abs(residual) < 1e-8, "event/cap/transition cumulative ledger");
    pass("Material/angle latching, zero events, ringing retriggers, speed caps, and active rate changes");
}

void collisionRateConvergence() {
    struct Metrics { double energy, impulse, peakForce; };
    auto render = [](const BowlDescriptor& bowl, const MalletDescriptor& mallet, double rate) {
        VesselEngine engine;
        require(engine.configure(bowl, mallet, EngineSettings{}, rate), "convergence configuration");
        engine.strike(.5);
        double impulse = 0.0, peakForce = 0.0;
        for (int i = 0; i < int(rate*.05); ++i) {
            const auto frame = engine.step();
            require(!frame.fault, "convergence contact fault");
            impulse += frame.strikeForce/rate;
            peakForce = std::max(peakForce, frame.strikeForce);
            if (frame.separated) return Metrics{engine.bowl().energy(), impulse, peakForce};
        }
        throw std::runtime_error("convergence collision did not separate");
    };
    double largestEnergyError = 0.0, largestImpulseError = 0.0, largestForceError = 0.0;
    for (const auto& bowl : seedBowls) for (const auto& mallet : seedMallets) {
        const auto coarse = render(bowl, mallet, 192000.0);
        const auto medium = render(bowl, mallet, 384000.0);
        const auto reference = render(bowl, mallet, 768000.0);
        const double energyError = std::abs(coarse.energy/reference.energy-1.0);
        const double impulseError = std::abs(coarse.impulse/reference.impulse-1.0);
        const double forceError = std::abs(coarse.peakForce/reference.peakForce-1.0);
        require(energyError < .01 && impulseError < .005 && forceError < .01,
                "default strike differs excessively from finer-rate reference");
        require(std::abs(medium.impulse/reference.impulse-1.0) < impulseError+1e-8,
                "collision impulse fails to converge");
        largestEnergyError = std::max(largestEnergyError, energyError);
        largestImpulseError = std::max(largestImpulseError, impulseError);
        largestForceError = std::max(largestForceError, forceError);
    }
    std::cout << "  C4 192k versus 768k: largest relative energy/impulse/peak-force error="
              << largestEnergyError << '/' << largestImpulseError << '/' << largestForceError << '\n';
    pass("Default-pitch half-velocity impacts converge across 192/384/768 kHz for all eight seed pairs");
}

void observersAndLongTail() {
    VesselEngine stereo, mono;
    auto settings = mono.settings(); settings.observerSeparation = 0.0;
    require(mono.configure(seedBowls[0], seedMallets[1], settings, 192000.0), "mono setup");
    stereo.strike(.5); mono.strike(.5);
    for (int i = 0; i < 10000; ++i) {
        stereo.step(); const auto frame = mono.step();
        require(frame.leftVelocity == frame.rightVelocity, "zero-width observers do not null");
        for (std::size_t j = 0; j < stereo.bowl().size(); ++j)
            require(stereo.bowl().state(j).x == mono.bowl().state(j).x
                    && stereo.bowl().state(j).y == mono.bowl().state(j).y, "observer changes mechanics");
    }
    double previous = mono.bowl().energy(); const double initial = previous;
    require(!mono.strikerActive(), "striker still active before tail test");
    for (int i = 0; i < 120*192000; ++i) {
        const auto frame = mono.step();
        if (i % 4096 == 0) {
            const double e = mono.bowl().energy();
            require(e <= previous+1e-16 && std::isfinite(frame.leftVelocity), "free tail energy grew/nonfinite"); previous = e;
        }
    }
    require(mono.bowl().energy() < initial*1e-20, "long tail does not decay");
    pass("Stereo observation is mechanically invariant; zero width nulls; 120-second tail decays safely");
}
}

int main() {
    try {
        polesAndConfiguration(); modalEnergyAndPorts(); staticCompliance(); elasticGroundImpact();
        fullCollisions(); transitionsAndRetriggers(); collisionRateConvergence(); observersAndLongTail();
        std::cout << "Vessel mechanical foundation: " << passed << " groups PASS\n"; return 0;
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n'; return 1;
    }
}
