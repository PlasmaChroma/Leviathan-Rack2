#pragma once
#include <cmath>

namespace vessel {

struct StrikeGradientValue {
    double force = 0.0;
    double root = 0.0; // Cached sqrt of nonnegative end compression.
};

// One starting compression per safeguarded solve, many trial end compressions.
// Solvers validate finite d0 >= 0 and k > 0 before constructing this cache.
class StrikeGradient {
public:
    StrikeGradient(double compression, double stiffness) noexcept
        : d0_(compression), root0_(std::sqrt(compression)), k_(stiffness),
          potential0_(0.4*k_*d0_*d0_*root0_) {}

    StrikeGradientValue evaluate(double d1) const noexcept {
        StrikeGradientValue value;
        if (d1 < 0.0) {
            const double gap = d0_-d1;
            value.force = potential0_/gap;
            return value;
        }
        const double a = root0_, b = std::sqrt(d1), sum = a+b;
        if (sum == 0.0) return value;
        // Preserve the original force expression's operation order.
        value.force = 0.4*k_*(d1*d1+d1*b*a+d1*d0_+b*a*d0_+d0_*d0_)/sum;
        value.root = b;
        return value;
    }

    // Reuse the trial force/root only when a Newton step is actually needed.
    // Bracket probes, converged trials and bisection-only iterations skip this.
    double derivative(double d1, const StrikeGradientValue& value) const noexcept {
        if (d1 < 0.0) return value.force/(d0_-d1);
        const double a = root0_, b = value.root, sum = a+b;
        if (sum == 0.0) return 0.0;
        // Analytic derivative of the factored gradient. This also holds at
        // d1 == d0, without subtracting nearly equal forces or a local cutoff.
        return 0.2*k_*((3.0*d1+6.0*a*b+4.0*d0_)*b+2.0*d0_*a)/(sum*sum);
    }
private:
    double d0_, root0_, k_, potential0_;
};

} // namespace vessel
