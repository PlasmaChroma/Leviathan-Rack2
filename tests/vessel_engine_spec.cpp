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

// Independent bracket-only oracles deliberately avoid production Newton logic.
double frictionOracle(double delta, double Y, double load, const MalletDescriptor& m) {
    double lo = -load*m.muS, hi = load*m.muS;
    for (int i = 0; i < 100; ++i) {
        const double f = .5*(lo+hi), s = delta-Y*f;
        const double mu = m.muK+(m.muS-m.muK)*std::exp(-s*s/(m.weakeningVelocity*m.weakeningVelocity));
        const double residual = f-load*mu*std::tanh(s/m.regularizationVelocity);
        if (residual > 0) hi = f; else lo = f;
    }
    return .5*(lo+hi);
}

void frictionLawAndOrbit() {
    std::mt19937 rng(1984);
    std::uniform_real_distribution<double> uniform(-1.0, 1.0);
    unsigned worst = 0;
    for (const auto& m : seedMallets) for (int i = 0; i < 2500; ++i) {
        const double load = 7.5*(uniform(rng)+1.0), Y = 1e-4*(uniform(rng)+1.0);
        const double delta = uniform(rng)*1.5;
        const auto s = solveFriction(delta, Y, load, m, uniform(rng)*load*m.muS);
        require(s.converged && s.force*s.slip >= -1e-15, "friction root/passivity");
        require(close(s.force, frictionOracle(delta, Y, load, m), 1e-9, 1e-9), "friction disagrees with independent oracle");
        const auto a = frictionValue(s.slip, load, m), b = frictionValue(-s.slip, load, m);
        require(a.force == -b.force && a.derivative == b.derivative, "friction oddness/derivative parity");
        require(a.derivative >= -frictionNegativeSlopeBound(load, m)-1e-12, "friction slope certificate");
        const double eps = 1e-4*m.regularizationVelocity;
        // Finite differences are meaningful near adhesion; far from it the
        // derivative check uses a larger increment to avoid cancellation.
        const double step = std::abs(s.slip) < 10*m.regularizationVelocity ? eps : 1e-6;
        const double derivative = (frictionValue(s.slip+step, load, m).force-frictionValue(s.slip-step, load, m).force)/(2*step);
        require(close(a.derivative, derivative, 2e-3, 2e-5), "friction analytic derivative");
        worst = std::max(worst, s.iterations);
    }
    require(solveFriction(.3, .01, 0, seedMallets[1]).force == 0.0, "zero load force");
    require(!solveFriction(.1, 1.0, 15, seedMallets[2]).converged, "uncertified friction accepted");
    ModalBank bank;
    bank.configure(seedBowls[0], 261.625565, 1, 1, 192000);
    ContactOrbit orbit;
    double angle = .7;
    orbit.configure(seedBowls[0], bank, seedMallets[1].patchWidth, angle);
    ModalVector t {}, n {};
    for (int i = 0; i < 400000; ++i) {
        const double increment = 2*pi*(i < 200000 ? .4 : -.7)/192000;
        orbit.midpoint(increment, t, n);
        if (i % 997 == 0) {
            const auto expectedT = bank.tangentialPort(angle+.5*increment, seedMallets[1].patchWidth);
            const auto expectedN = bank.radialPort(angle+.5*increment, seedMallets[1].patchWidth, true);
            for (std::size_t j = 0; j < bank.size(); ++j)
                require(close(t[j], expectedT[j], 1e-8, 2e-9) && close(n[j], expectedN[j], 1e-8, 2e-9), "moving midpoint orbit drift");
        }
        orbit.finish(); angle += increment;
    }
    std::cout << "  randomized friction worst iterations=" << worst << '\n';
    pass("10,000 independent friction roots, slope/derivative checks, and reversing midpoint orbit");
}

void simultaneousContactOracle() {
    std::mt19937 rng(9871);
    std::uniform_real_distribution<double> u(-1, 1);
    double worstResidual = 0;
    for (int i = 0; i < 1000; ++i) {
        ModalBank bank;
        const auto& bowl = seedBowls[i%2]; const auto& m = seedMallets[i%4];
        const double rate = i%3 == 0 ? 384000.0 : 192000.0;
        require(bank.configure(bowl, 261.625565, 1, 1, rate), "coupled modal config");
        for (std::size_t j = 0; j < bank.size(); ++j) bank.setState(j, .002*u(rng), .002*u(rng));
        const auto bs = bank.radialPort(pi*u(rng), m.patchWidth, true);
        const auto bt = bank.tangentialPort(pi*u(rng), m.patchWidth);
        const auto free = bank.freeMidpoint();
        const double h = bank.timeStep(), vs = bank.midpointVelocity(bs, free), vt = bank.midpointVelocity(bt, free);
        const double Yss = bank.admittance(bs, bs), Yst = bank.admittance(bs, bt), Ytt = bank.admittance(bt, bt);
        const double d0 = 1e-5*(1+u(rng)), v0 = u(rng), speed = .4*u(rng), load = 7.5*(1+u(rng));
        const auto solution = solveContacts(d0, v0, vs, vt, Yss, Yst, Ytt, h, m, m, speed, load);
        require(solution.converged, "coupled root failed");
        auto residual = [&](double fs) {
            const double ft = frictionOracle(speed-vt-Yst*fs, Ytt, load, m);
            const double d1 = d0+h*(v0-vs-(Yss+h/(2*m.mass))*fs-Yst*ft);
            // Independent energy quotient, using long double to protect a
            // nearly equal compression pair from cancellation.
            auto potential = [&](long double d) { return d > 0 ? .4L*m.stiffness*d*d*std::sqrt(d) : 0.0L; };
            const double elastic = d1 == d0 ? m.stiffness*d0*std::sqrt(d0)
                : double((potential(d1)-potential(d0))/(static_cast<long double>(d1)-d0));
            const double damping = std::max(d0, d1) > 0 ? m.loadingDamping*std::max(0.0, (d1-d0)/h) : 0;
            return fs-elastic-damping;
        };
        double lo = 0, hi = 1;
        while (residual(hi) < 0 && hi < 1e9) hi *= 2;
        for (int k = 0; k < 90; ++k) {
            const double mid = .5*(lo+hi);
            if (residual(mid) > 0) hi = mid; else lo = mid;
        }
        require(close(solution.strike.force, .5*(lo+hi), 1e-8, 1e-8), "coupled force disagrees with independent oracle");
        const double before = bank.energy()+.5*m.mass*v0*v0+strikePotential(d0, m.stiffness);
        ModalVector force {};
        for (std::size_t j = 0; j < bank.size(); ++j) force[j] = bs[j]*solution.strike.force+bt[j]*solution.friction.force;
        const auto audit = bank.commit(free, force, true);
        const double v1 = v0-h*solution.strike.force/m.mass;
        const double after = bank.energy()+.5*m.mass*v1*v1+strikePotential(solution.strike.compression, m.stiffness);
        const double balance = after-before+audit.dampingLoss+h*solution.strike.dampingForce*solution.strike.compressionVelocity
            +h*solution.friction.force*(solution.friction.slip-speed);
        require(std::abs(balance) < 1e-10, "coupled one-step energy identity");
        worstResidual = std::max(worstResidual, std::abs(balance));
    }
    std::cout << "  coupled oracle max energy residual=" << worstResidual << " J\n";
    pass("1,000 randomized simultaneous-contact roots match independent nested oracle and energy identity");
}

double completeLedgerResidual(const VesselEngine& e) {
    const auto& l = e.ledger();
    return e.totalEnergy()+l.modalLoss+l.contactLoss+l.retiredEnergy+l.recoveryLoss+l.frictionLoss
        -l.launchWork-l.speedCapWork-l.handWork-l.radialWork;
}

void rotationControlsAndPassivity() {
    VesselEngine stopped, lifted;
    stopped.setRotation(false, 0, 2.5); stopped.reset(); stopped.setRotation(true, 0, 2.5);
    stopped.setAuditEnabled(true); lifted.setAuditEnabled(true);
    stopped.strike(.5); lifted.strike(.5);
    double previous = stopped.totalEnergy();
    for (int i = 0; i < 192000; ++i) {
        const auto f = stopped.step(); lifted.step();
        require(!f.fault && stopped.totalEnergy() <= previous+1e-11, "stationary rubbing creates energy");
        previous = stopped.totalEnergy();
    }
    require(stopped.ledger().handWork == 0.0 && stopped.ledger().frictionLoss > 0.0, "stationary contact power");
    require(stopped.bowl().energy() < lifted.bowl().energy(), "stopped contact fails to damp ringing bowl");
    require(std::abs(completeLedgerResidual(stopped)) < 1e-9, "stopped-contact cumulative energy");
    VesselEngine noLoad, absent;
    noLoad.setRotation(false, .4, 0); noLoad.reset();
    noLoad.setRotation(true, .4, 0); noLoad.setAuditEnabled(true);
    noLoad.strike(.5); absent.strike(.5);
    for (int i = 0; i < 20000; ++i) {
        const auto f = noLoad.step(); absent.step();
        require(f.frictionForce == 0.0 && noLoad.bowl().energy() == absent.bowl().energy(), "zero-pressure changes mechanics");
    }
    const double beforeRelease = noLoad.rotationAngle();
    noLoad.setRotation(false, .4, 0);
    for (int i = 0; i < 1000; ++i) noLoad.step();
    require(noLoad.contactEngagement() == 0.0 && noLoad.rotationAngle() != beforeRelease, "release orbit freezes before lift");
    const double liftedAngle = noLoad.rotationAngle();
    for (int i = 0; i < 1000; ++i) noLoad.step();
    require(noLoad.rotationAngle() == liftedAngle, "lifted orbit keeps moving");
    noLoad.setRotation(true, -.4, 2.5);
    noLoad.step(); require(noLoad.rotationAngle() != 0.0, "engagement resets angle");
    require(!noLoad.setRotation(true, std::numeric_limits<double>::quiet_NaN(), 2.5), "NaN speed accepted");
    auto invalid = seedMallets[1]; invalid.muS = 1e10;
    const double oldEnergy = noLoad.totalEnergy();
    require(!noLoad.configure(seedBowls[0], invalid, EngineSettings{}, 192000) && noLoad.totalEnergy() == oldEnergy,
        "failed uniqueness certificate mutates engine");
    pass("Stopped-contact damping, zero pressure, gate-release orbit continuity, and transactional certificate rejection");
}

void basicFrequencyTuning() {
    for (const auto& bowl : seedBowls) for (const auto& mallet : seedMallets) {
        VesselEngine e;
        EngineSettings settings; settings.frequency = 220.0;
        require(e.configure(bowl, mallet, settings, 192000), "tuning setup");
        e.setAuditEnabled(true); e.strike(.5);
        require(!e.step().fault && e.strikerActive(), "tuning test needs active impact");
        auto retune = [&](double frequency) {
            std::array<ModalState, maxModes> states {};
            std::array<ModeCoefficients, maxModes> coefficients {};
            for (std::size_t j = 0; j < e.bowl().size(); ++j) {
                states[j] = e.bowl().state(j); coefficients[j] = e.bowl().coefficients(j);
            }
            const double energy = e.totalEnergy(), compression = e.compression(), velocity = e.strikerVelocity();
            const double angle = e.rotationAngle(), engagement = e.contactEngagement();
            const double ratio = frequency/settings.frequency;
            settings.frequency = frequency;
            require(e.configure(bowl, mallet, settings, 192000), "live base-frequency retune rejected");
            require(e.totalEnergy() == energy && e.compression() == compression && e.strikerVelocity() == velocity,
                "base-frequency tuning resets energy/active contact");
            require(e.rotationAngle() == angle && e.contactEngagement() == engagement, "base-frequency tuning resets rubbing trajectory");
            for (std::size_t j = 0; j < e.bowl().size(); ++j) {
                const auto& c = e.bowl().coefficients(j);
                require(e.bowl().state(j).x == states[j].x && e.bowl().state(j).y == states[j].y, "base-frequency tuning clears tail/phase state");
                require(close(c.frequency, ratio*coefficients[j].frequency) && c.mass == coefficients[j].mass
                    && c.t60 == coefficients[j].t60, "base-frequency tuning changes specimen ratios/mass/decay");
            }
            require(close(std::sqrt(e.bowl().coefficients(0).frequency*e.bowl().coefficients(1).frequency), frequency),
                "basic frequency differs from lowest pair center");
        };
        retune(440.0); // During compression, not only between strikes.
        e.setRotation(true, .4, 2.5);
        for (int i = 0; i < 10000; ++i) require(!e.step().fault, "retuned impact/rubbing fault");
        retune(880.0); // During rubbing and an existing ringing tail.
        for (int i = 0; i < 12000; ++i) {
            const auto f = e.step();
            require(!f.fault && close(f.handSpeed, 2*pi*bowl.rimRadius*.4), "retuning changes rubbing speed");
        }
        require(std::abs(completeLedgerResidual(e)) < 1e-9 && e.ledger().solverFaults == 0,
            "live tuning breaks coupled energy accounting");
    }
    pass("Basic-frequency tuning scales all modal pairs and preserves strike/rub state and energy across both specimens/all mallets");
}

void movingRuns() {
    double maxResidual = 0;
    // Several rotations of default singing from rest, then perturb speed,
    // pressure, direction, material, and add strikes without resetting state.
    for (bool radial : {false, true}) {
        VesselEngine e;
        EngineSettings settings; settings.prescribedRadialLoad = radial;
        require(e.configure(seedBowls[0], seedMallets[1], settings, 192000), "moving configure");
        e.setAuditEnabled(true); e.setRotation(true, .4, 2.5);
        VesselEngine mono;
        auto monoSettings = settings; monoSettings.observerSeparation = 0.0;
        require(mono.configure(seedBowls[0], seedMallets[1], monoSettings, 192000), "rub mono configure");
        mono.setRotation(true, .4, 2.5);
        double early = 0, late = 0, lateMin = 1e100, lateMax = 0;
        for (int i = 0; i < 192000*12; ++i) {
            const auto f = e.step(); require(!f.fault, "default sustained rotation fault");
            const auto mf = mono.step();
            require(!mf.fault && mf.leftVelocity == mf.rightVelocity, "rubbing zero-width observer null");
            if (i%1024 == 0) for (std::size_t j = 0; j < e.bowl().size(); ++j)
                require(e.bowl().state(j).x == mono.bowl().state(j).x && e.bowl().state(j).y == mono.bowl().state(j).y,
                    "rubbing observer/audit changes mechanics");
            if (i >= 96000 && i < 192000) early += e.bowl().energy()/96000;
            if (i >= 10*192000) {
                late += e.bowl().energy()/(2*192000);
                lateMin = std::min(lateMin, e.bowl().energy()); lateMax = std::max(lateMax, e.bowl().energy());
            }
        }
        require(late > early*10 && late > 1e-5, "moving contact did not develop sustained oscillation from rest");
        require(lateMax < 20*lateMin && lateMax < .1, "default singing lacks bounded late energy");
        std::cout << "  radial=" << radial << " early/late energy=" << early << '/' << late
                  << " late range=" << lateMin << ".." << lateMax << '\n';
        e.setRotation(true, -.3, 5);
        for (int i = 0; i < 192000; ++i) {
            if (i%48000 == 0) e.strike(.5);
            require(!e.step().fault, "sustained strike/rub transition fault");
        }
        require(e.configure(seedBowls[1], seedMallets[2], settings, 384000), "rubbing material/rate switch");
        for (int i = 0; i < 38400; ++i) require(!e.step().fault, "material/rate rub transition fault");
        e.setRotation(false, -.3, 5);
        for (int i = 0; i < 10000; ++i) require(!e.step().fault, "rub release fault");
        require(e.contactEngagement() == 0 && e.ledger().speedCaps == 0 && e.ledger().solverFaults == 0, "ordinary rub requires protection");
        const double residual = std::abs(completeLedgerResidual(e));
        require(residual < 1e-8 && e.ledger().maxStepResidual < 1e-9, "whole-run strike/rub energy ledger");
        maxResidual = std::max(maxResidual, residual);
    }
    // Removing velocity weakening must remove the self-excited growth while
    // retaining the travelling-force response and engagement transient.
    VesselEngine constantFriction;
    auto constant = seedMallets[1]; constant.muS = constant.muK;
    require(constantFriction.configure(seedBowls[0], constant, EngineSettings{}, 192000), "constant-friction control config");
    constantFriction.setRotation(true, .4, 2.5);
    for (int i = 0; i < 3*192000; ++i) require(!constantFriction.step().fault, "constant-friction control fault");
    require(constantFriction.bowl().energy() < 1e-5, "constant friction generates spurious self-excitation");
    // All seed combinations, reversal, maximum pressure, two rates and
    // periodic strikes exercise the steep adhesion region during startup.
    for (const auto& bowl : seedBowls) for (const auto& m : seedMallets)
        for (double rate : {176400.0, 384000.0}) {
            VesselEngine e; require(e.configure(bowl, m, EngineSettings{}, rate), "rub seed configure");
            e.setAuditEnabled(true); e.setRotation(true, .4, 15);
            for (int i = 0; i < int(rate*.1); ++i) {
                if (i == 100 || i == 1000) e.strike(.5);
                if (i == int(rate*.05)) e.setRotation(true, -.4, 15);
                const auto frame = e.step();
                if (frame.fault) std::cerr << "  fault bowl=" << bowl.stableId << " mallet=" << m.stableId
                    << " rate=" << rate << " frame=" << i << " compression=" << frame.compression
                    << " max strike/friction iterations=" << e.ledger().maxSolverIterations << '/'
                    << e.ledger().maxFrictionIterations << '\n';
                require(!frame.fault, "high-pressure seed/reversal trajectory fault");
            }
            require(std::abs(completeLedgerResidual(e)) < 1e-8, "seed trajectory energy ledger");
        }
    std::cout << "  whole-run max cumulative residual=" << maxResidual << " J\n";
    pass("Several-rotation startup/saturation, radial-load comparison, coupled strikes, and seed/rate/control transitions");
}

void highEnergyDamping() {
    for (double rate : {96000.0, 192000.0}) {
        ModalBank bank;
        require(bank.configure(seedBowls[1], 261.625565, 4, 1, rate), "extra damping setup");
        bank.setState(0, .4, 0); // 80 mJ, without a contact transient.
        const auto baseline = bank;
        const auto port = bank.radialPort(.2, .05);
        const double initial = bank.energy();
        require(bank.setAdditionalDamping(.25), "extra damping accepted");
        require(bank.energy() == initial, "damping update changes energy");
        require(bank.admittance(port, port) <= baseline.admittance(port, port), "damping increases contact admittance");
        require(!bank.setAdditionalDamping(-1) && !bank.setAdditionalDamping(.6)
            && !bank.setAdditionalDamping(std::numeric_limits<double>::quiet_NaN()), "invalid damping accepted");
        auto retuned = bank;
        require(retuned.configure(seedBowls[1], 261.625565, 4, 1, rate), "damping retune");
        auto fast = bank;
        double loss = 0;
        for (int i = 0; i < int(rate); ++i) {
            const auto audit = bank.commit(bank.freeMidpoint(), ModalVector{}, true);
            loss += audit.dampingLoss;
            require(std::abs(audit.residual) < 1e-14, "extra damping step ledger");
            double left = 0, right = 0;
            require(fast.advanceFree(port, port, left, right), "extra damping fast tail");
            retuned.commit(retuned.freeMidpoint(), ModalVector{});
        }
        require(close(initial-bank.energy(), loss, 1e-9, 1e-10), "extra damping cumulative ledger");
        require(bank.energy() == fast.energy() && bank.energy() == retuned.energy(), "extra damping path/retune mismatch");
        require(bank.energy() < .045 && bank.energy() > .04, "extra damping decay magnitude");
        auto restored = baseline;
        require(restored.setAdditionalDamping(.25) && restored.setAdditionalDamping(0), "damping restore");
        auto original = baseline;
        restored.commit(restored.freeMidpoint(), ModalVector{});
        original.commit(original.freeMidpoint(), ModalVector{});
        require(restored.energy() == original.energy(), "zero damping changed linear tail");

        VesselEngine governed, reference;
        EngineSettings settings; settings.decayMultiplier = 4;
        require(governed.configure(seedBowls[1], seedMallets[1], settings, rate)
            && reference.configure(seedBowls[1], seedMallets[1], settings, rate), "governor engines");
        governed.setAuditEnabled(true); reference.setAuditEnabled(true);
        bool crossedKnee = false;
        for (int i = 0; i < int(2*rate); ++i) {
            if (i % int(rate*.005) == 0) { governed.strike(1, 2); reference.strike(1, 2); }
            if (i % int(rate*.001) == 0) {
                crossedKnee = crossedKnee || governed.bowl().energy() > .04;
                governed.updateHighEnergyDamping();
            }
            require(!governed.step().fault && !reference.step().fault, "governed strike trajectory fault");
            if (!crossedKnee) require(governed.totalEnergy() == reference.totalEnergy(), "governor affects sub-knee trajectory");
        }
        require(crossedKnee, "governor fixture never reached knee");
        require(std::abs(completeLedgerResidual(governed)) < 1e-8, "governed contact energy ledger");
        governed.setRotation(false, 0, 0);
        for (int i = 0; i < int(rate*.02); ++i) governed.step();
        require(!governed.strikerActive(), "governor fixture striker did not separate");
        const double before = governed.bowl().energy();
        for (int i = 0; i < int(rate); ++i) {
            if (i % int(rate*.001) == 0) governed.updateHighEnergyDamping();
            require(!governed.step().fault, "governed tail fault");
        }
        require(governed.bowl().energy() < before, "governed tail grew");
        require(std::abs(completeLedgerResidual(governed)) < 1e-8, "governed tail ledger");
        governed.reset(); reference.reset();
        governed.strike(.3); reference.strike(.3);
        for (int i = 0; i < int(rate*.02); ++i) {
            governed.step(); reference.step();
            require(governed.totalEnergy() == reference.totalEnergy(), "reset retained extra damping");
        }
    }
    pass("High-energy damping preserves quiet trajectories, passive contact accounting, retuning and fast tails");
}

int main() {
    try {
        polesAndConfiguration(); modalEnergyAndPorts(); staticCompliance(); elasticGroundImpact(); highEnergyDamping();
        fullCollisions(); transitionsAndRetriggers(); collisionRateConvergence(); observersAndLongTail();
        frictionLawAndOrbit(); simultaneousContactOracle(); rotationControlsAndPassivity(); basicFrequencyTuning(); movingRuns();
        std::cout << "Vessel mechanics: " << passed << " groups PASS\n"; return 0;
    } catch (const std::exception& error) {
        std::cerr << "[FAIL] " << error.what() << '\n'; return 1;
    }
}
