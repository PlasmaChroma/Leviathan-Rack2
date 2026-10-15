#pragma once
#include <cmath>

// Experimental, isolated kernel. Not wired into Vessel. Preserve the existing
// safeguarded Newton/bracket and energy-audit tests when integrating this.
namespace vessel_audit {
struct GradientResult {
    double force = 0.0;
    double derivative = 0.0; // Partial derivative with respect to nextCompression.
};
class StrikeGradientCache {
    double d0_ = 0.0, root0_ = 0.0, k_ = 0.0, potential0_ = 0.0;
public:
    bool prepare(double compression, double stiffness) noexcept {
        if (!std::isfinite(compression) || compression < 0.0 ||
            !std::isfinite(stiffness) || stiffness <= 0.0) return false;
        d0_ = compression;
        root0_ = std::sqrt(compression);
        k_ = stiffness;
        potential0_ = 0.4*k_*d0_*d0_*root0_;
        return std::isfinite(potential0_);
    }
    // Precondition: prepare succeeded and nextCompression is finite.
    GradientResult evaluate(double nextCompression) const noexcept {
        GradientResult out;
        const double d1 = nextCompression;
        if (d1 < 0.0) {
            const double gap = d0_-d1;
            out.force = potential0_/gap;
            out.derivative = potential0_/(gap*gap);
            return out;
        }
        const double a = root0_, b = std::sqrt(d1), sum = a+b;
        if (sum == 0.0) return out;
        // Keep the existing positive-compression force expression's order.
        out.force = 0.4*k_*(d1*d1+d1*b*a+d1*d0_+b*a*d0_+d0_*d0_)/sum;
        // Exact derivative of that discrete gradient; no subtractive cancellation.
        out.derivative = 0.2*k_*((3.0*d1+6.0*a*b+4.0*d0_)*b+2.0*d0_*a)/(sum*sum);
        return out;
    }
};
} // namespace vessel_audit
