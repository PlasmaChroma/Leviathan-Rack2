#pragma once
#include "ChimeraReel.hpp"
#include "ChimeraProfile.hpp"
#include <cmath>

namespace chimera {
// Optional quality reader, prepared off audio. Core-owned live block sums
// accelerate wide filters without a resampled audio copy or COW duplication.
// Partial blocks wrap inside the Splice and read raw audio directly.
class PlaybackReader {
public:
    PlaybackReader() : kernel_(table().values) {}
private:
    struct Table {
    float values[8193];
    Table() {
        for (unsigned i = 0; i <= 8192; ++i) {
            const double x = double(i) / 1024.0;
            const double window = 0.42 + 0.5 * std::cos(profile1::kPi*x/8.0) +
                0.08 * std::cos(profile1::kPi*x/4.0);
            values[i] = float((i ? std::sin(profile1::kPi*0.94*x) /
                (profile1::kPi*x) : 0.94) * window);
        }
        values[8192] = 0.f;
    }
    };
    static const Table& table() { static const Table instance; return instance; }
public:
    static StereoFrame cubic(const Reel& reel, Region r, double coordinate, bool& invalid) {
        const double p = profile1::wrapPosition(coordinate, r);
        const std::int64_t base = static_cast<std::int64_t>(std::floor(p));
        const double t = p - base;
        const StereoFrame a = reel.readActive(profile1::wrapTap(base, -1, r));
        const StereoFrame b = reel.readActive(profile1::wrapTap(base, 0, r));
        const StereoFrame c = reel.readActive(profile1::wrapTap(base, 1, r));
        const StereoFrame d = reel.readActive(profile1::wrapTap(base, 2, r));
        const float left = float(profile1::cubic(a.l,b.l,c.l,d.l,t));
        const float right = float(profile1::cubic(a.r,b.r,c.r,d.r,t));
        if (!profile1::finite(left) || !profile1::finite(right)) invalid = true;
        return {profile1::audio(left), profile1::audio(right)};
    }
    StereoFrame read(const Reel& reel, Region r, double coordinate, double speed, bool& invalid,
                     bool accelerate = true) const {
        if (r.end <= r.begin) return {0, 0};
        if (!profile1::finite(speed)) { invalid = true; speed = 1; }
        const double width = profile1::clamp(std::fabs(speed), 1.0, 512.0);
        if (width <= 1.0) return cubic(reel, r, coordinate, invalid);
        if (accelerate && width >= r.end-r.begin) {
            // A Splice's first non-DC Fourier bin is beyond the stop band.
            // Folding thousands of repeated kernel taps is equivalent to its
            // mean within filter rejection error, without the tiny-loop spike.
            double left = 0, right = 0;
            // Reuse the live zeroth moments for whole aligned blocks. Only
            // the two partial 16-frame edges need individual reads (<=30).
            for (unsigned at = r.begin; at < r.end;) {
                unsigned block = 0;
                for (unsigned size : {256u, 64u, 16u}) {
                    if (at % size == 0 && r.end-at >= size) { block = size; break; }
                }
                if (block) {
                    const auto& sums = reel.playbackMoments(at, block);
                    invalid = invalid || sums.invalid != 0;
                    left += sums.left[0]; right += sums.right[0];
                    at += block;
                    continue;
                }
                const auto value = reel.readActive(at++);
                invalid = invalid || !profile1::finite(value.l) || !profile1::finite(value.r);
                left += profile1::clamp(profile1::audio(value.l), -64.0, 64.0);
                right += profile1::clamp(profile1::audio(value.r), -64.0, 64.0);
            }
            return {float(left/(r.end-r.begin)), float(right/(r.end-r.begin))};
        }
        const double p = profile1::wrapPosition(coordinate, r), radius = 8.0 * width;
        const std::int64_t begin = std::int64_t(std::ceil(p-radius));
        const std::int64_t end = std::int64_t(std::floor(p+radius));
        std::uint32_t at = profile1::wrapTap(begin, 0, r);
        const double scale = 1024.0 / width;
        double left = 0, right = 0, weight = 0;
        for (std::int64_t tap = begin; tap <= end;) {
            unsigned block = 0;
            if (accelerate && width >= 32) {
                for (unsigned size : {256u, 64u, 16u}) {
                    if (size <= width*0.5 && at % size == 0 && at + size <= r.end &&
                        end-tap+1 >= size) { block = size; break; }
                }
            }
            if (block) {
                const double center = (double(tap)+(block-1)*0.5-p)*scale;
                const double half = (block-1)*0.5*scale;
                const double a = gainAt(center-half), b = gainAt(center-half/3),
                    c = gainAt(center+half/3), d = gainAt(center+half);
                const double even = (a+d)*0.5, innerEven = (b+c)*0.5;
                const double odd = (d-a)*0.5, innerOdd = (c-b)*1.5;
                const double coeff[4] = {even-(even-innerEven)*1.125,
                    odd-(odd-innerOdd)*1.125, (even-innerEven)*1.125, (odd-innerOdd)*1.125};
                const auto& sums = reel.playbackMoments(at, block);
                invalid = invalid || sums.invalid != 0;
                for (unsigned j = 0; j < 4; ++j) {
                    left += coeff[j]*sums.left[j]; right += coeff[j]*sums.right[j];
                }
                weight += coeff[0]*block + coeff[2]*block*(block+1.0)/(3.0*(block-1));
                tap += block; at += block;
                if (at == r.end) at = r.begin;
                continue;
            }
            const double gain = gainAt((double(tap)-p)*scale);
            StereoFrame value = reel.readActive(at);
            if (!profile1::finite(value.l) || !profile1::finite(value.r)) invalid = true;
            left += profile1::clamp(profile1::audio(value.l), -64.0, 64.0) * gain;
            right += profile1::clamp(profile1::audio(value.r), -64.0, 64.0) * gain;
            weight += gain;
            ++tap;
            if (++at == r.end) at = r.begin;
        }
        StereoFrame filtered{float(left/weight), float(right/weight)};
        // Avoid a discontinuity when a smoothed rate crosses unity.
        if (width < 1.125) {
            const auto original = cubic(reel, r, coordinate, invalid);
            const float blend = float((width-1.0)*8.0);
            filtered.l = original.l + (filtered.l-original.l)*blend;
            filtered.r = original.r + (filtered.r-original.r)*blend;
        }
        return filtered;
    }
private:
    double gainAt(double index) const {
        index = std::fabs(index);
        const unsigned i = unsigned(index);
        return i >= 8192 ? 0.0 : kernel_[i] + (kernel_[i+1]-kernel_[i])*(index-i);
    }
    const float* kernel_;
};
} // namespace chimera
