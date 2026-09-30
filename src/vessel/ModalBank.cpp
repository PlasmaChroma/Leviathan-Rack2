#include "ModalBank.hpp"

#include <algorithm>
#include <cmath>

namespace vessel {
namespace {
bool positive(double value) { return std::isfinite(value) && value > 0.0; }
double patch(int order, double width) {
    const double z = 0.5 * order * width;
    return std::abs(z) < 1e-8 ? 1.0 - z * z / 6.0 : std::sin(z) / z;
}
}

bool ModalBank::configure(const BowlDescriptor& bowl, double frequency, double decay,
                          double imperfection, double rate) noexcept {
    if (!positive(rate) || rate < 32000.0 || rate > 4000000.0
        || !positive(frequency) || !positive(decay) || !std::isfinite(imperfection)
        || imperfection < 0.0 || imperfection > 2.0 || bowl.pairCount == 0
        || bowl.pairCount > maxPairs || !positive(bowl.rimRadius)) return false;
    // Topology changes require an explicit state projection. Initial seeds match.
    if (count_ && count_ != bowl.pairCount * 2) return false;
    const double h = 1.0 / rate;
    std::array<ModeCoefficients, maxModes> next {};
    int previousOrder = 1;
    for (std::size_t n = 0; n < bowl.pairCount; ++n) {
        const auto& p = bowl.pairs[n];
        if (p.order <= previousOrder || !positive(p.ratio)
            || !std::isfinite(p.orientation) || !std::isfinite(p.splitCents)
            || p.splitCents < 0.0 || !positive(p.massA) || !positive(p.massB)
            || !positive(p.t60A) || !positive(p.t60B)
            || !std::isfinite(p.radiationA) || p.radiationA < 0.0
            || !std::isfinite(p.radiationB) || p.radiationB < 0.0) return false;
        if (count_ && pairs_[n].order != p.order) return false;
        previousOrder = p.order;
        for (std::size_t side = 0; side < 2; ++side) {
            auto& c = next[2*n + side];
            const double split = std::exp2((side ? 1.0 : -1.0)*imperfection*p.splitCents/2400.0);
            c.frequency = frequency*p.ratio*split;
            c.t60 = std::max(0.1, std::min(120.0, (side ? p.t60B : p.t60A)*decay));
            c.mass = side ? p.massB : p.massA;
            if (!positive(c.frequency) || c.frequency >= 0.40*rate) return false;
            const double targetSigma = log1000/c.t60;
            const double r = std::exp(-targetSigma*h);
            const double theta = 2.0*pi*c.frequency*h;
            const double denominator = 1.0 + 2.0*r*std::cos(theta) + r*r;
            c.sigma = (2.0/h)*(-std::expm1(-2.0*targetSigma*h))/denominator;
            const double nu = (4.0/h)*r*std::sin(theta)/denominator;
            c.omega = std::hypot(c.sigma, nu);
            c.a = 0.5*h*c.omega;
            c.inverseD = 1.0/(1.0 + h*c.sigma + c.a*c.a);
            c.admittanceWeight = 0.5*h*c.inverseD;
            c.inverseRootMass = 1.0/std::sqrt(c.mass);
            const double omega0 = std::hypot(2.0*pi*c.frequency, targetSigma);
            c.staticComplianceRatio = (omega0/c.omega)*(omega0/c.omega);
            if (!positive(c.omega) || !positive(c.inverseD) || !std::isfinite(c.sigma)) return false;
        }
    }
    coefficients_ = next;
    for (std::size_t j = 0; j < bowl.pairCount*2; ++j) {
        hotA_[j] = next[j].a;
        hotInverseD_[j] = next[j].inverseD;
        hotWeight_[j] = next[j].admittanceWeight;
        hotOmega_[j] = next[j].omega;
        hotSigma_[j] = next[j].sigma;
    }
    pairs_ = bowl.pairs;
    count_ = 2*bowl.pairCount;
    h_ = h;
    return true;
}

void ModalBank::clear() noexcept { states_ = {}; }
bool ModalBank::setState(std::size_t i, double x, double y) noexcept {
    if (i >= count_ || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(x*x+y*y)) return false;
    states_[i].x = x;
    states_[i].y = y;
    return true;
}
double ModalBank::energy() const noexcept {
    double e = 0.0;
    for (std::size_t j = 0; j < count_; ++j) e += 0.5*(states_[j].x*states_[j].x + states_[j].y*states_[j].y);
    return e;
}
bool ModalBank::finite() const noexcept {
    for (std::size_t j = 0; j < count_; ++j)
        if (!std::isfinite(states_[j].x) || !std::isfinite(states_[j].y)) return false;
    return true;
}
bool ModalBank::advanceFree(const ModalVector& left, const ModalVector& right,
    double& leftVelocity, double& rightVelocity) noexcept {
    leftVelocity = rightVelocity = 0.0;
    bool valid = true;
    for (std::size_t j = 0; j < count_; ++j) {
        auto& s = states_[j];
        const double mid = (s.y-hotA_[j]*s.x)*hotInverseD_[j]+0.0;
        s.x += h_*hotOmega_[j]*mid;
        s.y = 2.0*mid-s.y;
        valid = valid && std::isfinite(s.x) && std::isfinite(s.y);
        leftVelocity += left[j]*s.y;
        rightVelocity += right[j]*s.y;
    }
    return valid && std::isfinite(leftVelocity) && std::isfinite(rightVelocity);
}
ModalVector ModalBank::radialPort(double angle, double width, bool inward) const noexcept {
    ModalVector b {};
    if (!std::isfinite(angle) || !std::isfinite(width) || width < 0.0 || width > 2.0*pi) return b;
    for (std::size_t n = 0; n < count_/2; ++n) {
        const auto& p = pairs_[n];
        const double beta = p.order*(std::remainder(angle, 2.0*pi)-std::remainder(p.orientation, 2.0*pi));
        const double gain = (inward ? -1.0 : 1.0)*patch(p.order, width);
        b[2*n] = gain*std::cos(beta)*coefficients_[2*n].inverseRootMass;
        b[2*n+1] = gain*std::sin(beta)*coefficients_[2*n+1].inverseRootMass;
    }
    return b;
}
ModalVector ModalBank::tangentialPort(double angle, double width) const noexcept {
    ModalVector b {};
    if (!std::isfinite(angle) || !std::isfinite(width) || width < 0.0 || width > 2.0*pi) return b;
    for (std::size_t n = 0; n < count_/2; ++n) {
        const auto& p = pairs_[n];
        const double beta = p.order*(std::remainder(angle, 2.0*pi)-std::remainder(p.orientation, 2.0*pi));
        const double gain = patch(p.order, width)/p.order;
        b[2*n] = -gain*std::sin(beta)*coefficients_[2*n].inverseRootMass;
        b[2*n+1] = gain*std::cos(beta)*coefficients_[2*n+1].inverseRootMass;
    }
    return b;
}
ModalVector ModalBank::observer(double angle) const noexcept {
    auto b = radialPort(angle, 0.0);
    for (std::size_t n = 0; n < count_/2; ++n) {
        b[2*n] *= pairs_[n].radiationA;
        b[2*n+1] *= pairs_[n].radiationB;
    }
    return b;
}

} // namespace vessel
