#include "../src/vessel/StrikeGradient.hpp"
#include "../src/vessel/ModalBank.hpp"
#include "../src/vessel/SeedProfiles.hpp"
#include "vessel_contact_reference.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

namespace {
using namespace vessel;
void require(bool okay, const char* message) { if (!okay) throw std::runtime_error(message); }

// Independent derivative of the energy quotient in extended precision.
// Close endpoints use the integral's binomial expansion, avoiding cancellation
// in the oracle itself. This does not use the production rational factorization.
long double derivativeOracle(long double a, long double b, long double k) {
    if (a == b) return .75L*k*std::sqrt(a);
    if (a > 0 && std::abs(b-a) < .125L*a) {
        const long double r = (b-a)/a;
        long double term = 1, sum = .5L;
        for (int n=1; n<24; ++n) {
            term *= (.5L-(n-1))*r/n;
            sum += term/(n+2);
        }
        return 1.5L*k*std::sqrt(a)*sum;
    }
    auto potential = [&](long double d) { return d > 0 ? .4L*k*d*d*std::sqrt(d) : 0.L; };
    const long double gradient = (potential(b)-potential(a))/(b-a);
    const long double force = b > 0 ? k*b*std::sqrt(b) : 0.L;
    return (force-gradient)/(b-a);
}

void gradientChecks() {
    std::mt19937_64 random(314159);
    std::uniform_real_distribution<double> exponent(-18, -1);
    double worst = 0;
    unsigned cases = 0;
    auto check = [&](double a, double b, double k) {
        const StrikeGradient gradient(a,k);
        const auto value = gradient.evaluate(b);
        const double derivative = gradient.derivative(b,value);
        require(value.force == strikeDiscreteGradient(a,b,k), "cached strike force differs from reference");
        const long double expected = derivativeOracle(a,b,k);
        const double error = double(std::abs(derivative-expected)/std::max(1e-290L,std::abs(expected)));
        worst = std::max(worst,error);
        require(std::isfinite(derivative) && derivative >= 0 && error < 5e-14,
                "strike derivative disagrees with independent oracle");
        ++cases;
    };
    for (const auto& mallet : seedMallets) {
        const double k = mallet.stiffness;
        check(0,0,k); check(0,1e-10,k); check(0,-1e-10,k); check(1e-10,0,k);
        for (int i=0; i<1000; ++i) {
            const double a = std::pow(10.,exponent(random)), b = std::pow(10.,exponent(random));
            for (double next : {b,-b,a,std::nextafter(a,0.),std::nextafter(a,1.),a*(1+1e-10),a*(1-1e-10)})
                check(a,next,k);
        }
    }
    std::cout << "[PASS] " << cases << " gradients; exact cached forces, max relative derivative error=" << worst << '\n';
}

void solverChecks() {
    std::mt19937_64 random(198212);
    std::uniform_real_distribution<double> unit(-1,1);
    double worstStrike = 0, worstDecoupled = 0;
    unsigned long long oldIterations = 0, newIterations = 0;
    for (unsigned i=0; i<4000; ++i) {
        const auto& striker = seedMallets[i%4];
        const auto& rubber = seedMallets[(i/4)%4];
        const double rate = i%3 == 0 ? 96000 : i%3 == 1 ? 192000 : 384000;
        ModalBank bank;
        require(bank.configure(seedBowls[(i/16)%2],261.625565,1,1,rate), "strike fixture setup");
        const auto radial = bank.radialPort(pi*unit(random),striker.patchWidth,true);
        const auto tangent = bank.tangentialPort(pi*unit(random),rubber.patchWidth);
        const double yss = bank.admittance(radial,radial);
        const double ytt = bank.admittance(tangent,tangent), h = 1/rate;
        const double d0 = i%8 == 0 ? 0 : 1e-5*(1+unit(random));
        const double velocity = unit(random), vs = .2*unit(random), vt = .2*unit(random);
        const double speed = .4*unit(random), load = 7.5*(1+unit(random));
        const auto oldStrike = vessel_contact_reference::solveStrike(d0,velocity,vs,yss,h,striker);
        const auto newStrike = vessel::solveStrike(d0,velocity,vs,yss,h,striker);
        // With zero cross-admittance, the unchanged nested solver is a second
        // independent route to the same isolated strike root (rub may remain).
        const auto decoupled = vessel::solveContacts(d0,velocity,vs,vt,yss,0,ytt,h,striker,rubber,speed,load,0);
        require(oldStrike.converged && newStrike.converged && decoupled.converged,
                "strike/coupled convergence regression");
        const double strikeError = std::abs(newStrike.force-oldStrike.force)/std::max(1.,std::abs(oldStrike.force));
        const double decoupledError = std::abs(newStrike.force-decoupled.strike.force)/std::max(1.,std::abs(newStrike.force));
        worstStrike = std::max(worstStrike,strikeError); worstDecoupled = std::max(worstDecoupled,decoupledError);
        require(strikeError < 1e-9 && decoupledError < 1e-9, "strike solution differs beyond solver tolerance allowance");
        oldIterations += oldStrike.iterations;
        newIterations += newStrike.iterations;
    }
    const auto& m = seedMallets[0];
    const double nan = std::numeric_limits<double>::quiet_NaN(), inf = std::numeric_limits<double>::infinity();
    for (double bad : {-1.,nan,inf}) {
        require(!vessel::solveStrike(bad,1,0,.001,1./192000,m).converged, "invalid compression accepted");
        require(!vessel::solveContacts(bad,1,0,0,.001,0,.001,1./192000,m,m,.2,1).converged, "invalid coupled compression accepted");
    }
    require(!vessel::solveContacts(0,1,0,0,.001,1,.001,1./192000,m,m,.2,1).converged,
            "invalid admittance certificate accepted");
    std::cout << "[PASS] 4000 isolated reference/decoupled comparisons, all mallet pairs; max scaled force differences="
        << worstStrike << '/' << worstDecoupled << "; isolated iterations=" << oldIterations << " -> " << newIterations << '\n';
}
}
int main() {
    try { std::cout << std::setprecision(12); gradientChecks(); solverChecks(); }
    catch (const std::exception& e) { std::cerr << "[FAIL] " << e.what() << '\n'; return 1; }
}
