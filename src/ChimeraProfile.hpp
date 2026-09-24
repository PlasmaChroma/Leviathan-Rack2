#pragma once

#include "ChimeraTypes.hpp"
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

namespace chimera {
namespace profile1 {

static const double kPi = 3.14159265358979323846264338327950288;
static const double kStopEpsilon = 0.0001;
static const std::uint32_t kDefaultSeed = 0x6D2B79F5u;

// Bit checks survive a plugin translation unit compiled with fast-math.
inline bool finite(float x) {
    std::uint32_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    return (bits & 0x7f800000u) != 0x7f800000u;
}
inline bool finite(double x) {
    std::uint64_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    return (bits & 0x7ff0000000000000ull) != 0x7ff0000000000000ull;
}
inline double clamp(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
inline double clamp01(double x) { return clamp(x, 0.0, 1.0); }
inline double controlVoltage(float x) { return finite(x) ? clamp(x, -24.0, 24.0) : 0.0; }
inline float audio(float x) { return finite(x) ? x : 0.f; }
inline std::uint32_t roundNonnegative(double x) {
    if (!finite(x) || x <= 0.0) return 0;
    if (x >= static_cast<double>(std::numeric_limits<std::uint32_t>::max()))
        return std::numeric_limits<std::uint32_t>::max();
    return static_cast<std::uint32_t>(std::floor(x + 0.5));
}

struct FiniteHold {
    float value;
    bool observed;
    std::uint64_t invalidCount;
    explicit FiniteHold(float initial = 0.f) : value(initial), observed(false), invalidCount(0) {}
    float take(float candidate) {
        if (finite(candidate)) { value = candidate; observed = true; }
        else ++invalidCount;
        return value;
    }
};

inline double sos(double knob, bool patched, double cv) {
    return clamp01(clamp01(knob) * (patched ? clamp(cv, -24.0, 24.0) / 8.0 : 1.0));
}
inline double additive8(double knob, double att, double cv) {
    return clamp01(knob + clamp(att, -1.0, 1.0) * clamp(cv, -24.0, 24.0) / 8.0);
}
inline double additive5(double knob, double cv) {
    return clamp01(knob + clamp(cv, -24.0, 24.0) / 5.0);
}
inline double classicRateFromCoordinate(double x) {
    x = clamp(x, -1.0, 1.0);
    const double magnitude = std::fabs(x);
    if (magnitude <= kStopEpsilon) return 0.0;
    // Rack parameters are float32. Snap their nominal 1x anchor so a full
    // traversal at the default 5/6 knob does not gain a frame over time.
    if (std::fabs(magnitude - 2.0/3.0) < 1e-7) return x < 0.0 ? -1.0 : 1.0;
    const double q = (magnitude - kStopEpsilon) / (1.0 - kStopEpsilon);
    const double qUnity = (2.0 / 3.0 - kStopEpsilon) / (1.0 - kStopEpsilon);
    const double gamma = std::log(0.5) / std::log(qUnity);
    return (x < 0.0 ? -1.0 : 1.0) * 2.0 * std::pow(q, gamma);
}
inline double classicRate(double knob, double att = 0.0, double cv = 0.0) {
    return classicRateFromCoordinate(2.0 * clamp01(knob) - 1.0 +
        clamp(att, -1.0, 1.0) * clamp(cv, -24.0, 24.0) / 4.0);
}
inline double forwardBaseRate(double knob) {
    const double k = clamp01(knob);
    if (k <= kStopEpsilon) return 0.0;
    const double q = (k - kStopEpsilon) / (1.0 - kStopEpsilon);
    const double qUnity = (0.75 - kStopEpsilon) / (1.0 - kStopEpsilon);
    return 2.0 * std::pow(q, std::log(0.5) / std::log(qUnity));
}
inline double pitchRate(int mode, double knob, double att, double cv) {
    const double base = mode == 2 ? forwardBaseRate(knob) : classicRate(knob);
    const double pitchVolts = clamp(clamp(att, -1.0, 1.0) * clamp(cv, -24.0, 24.0), -8.0, 8.0);
    return clamp(base * std::exp2(pitchVolts), -32.0, 32.0);
}
inline double rate(int mode, double knob, double att, double cv) {
    return mode == 0 ? classicRate(knob, att, cv) : pitchRate(mode, knob, att, cv);
}

struct OnePole {
    double value;
    double alpha;
    bool initialized;
    OnePole() : value(0), alpha(1), initialized(false) {}
    void setTau(double seconds) {
        alpha = seconds <= 0.0 ? 1.0 : 1.0 - std::exp(-1.0 / (seconds * kCoreRate));
    }
    double step(double target) {
        if (!initialized) { value = target; initialized = true; }
        else {
            value += alpha * (target - value);
            if (std::fabs(target - value) < 1e-7) value = target;
        }
        return value;
    }
};

struct LinearFade {
    double value, start, target;
    std::uint32_t length, progress;
    LinearFade() : value(0), start(0), target(0), length(0), progress(0) {}
    void initialize(double first) { value = start = target = first; length = progress = 0; }
    void begin(double next, std::uint32_t frames) {
        start = value; target = next; length = frames; progress = 0;
        if (!frames) value = target;
    }
    double step() {
        if (progress < length) {
            ++progress;
            value = start + (target - start) * (static_cast<double>(progress) / length);
        }
        return value;
    }
};

struct GeneMode {
    bool full;
    GeneMode() : full(true) {}
    bool observe(double normalized) {
        if (full && normalized >= 0.0002) full = false;
        else if (!full && normalized <= 0.0001) full = true;
        return full;
    }
};

inline std::uint32_t finiteGeneFrames(std::uint32_t regionLength, double normalized) {
    if (!regionLength) return 0;
    const std::uint32_t minimum = regionLength < 16 ? regionLength : 16;
    const double x = std::exp(std::log(static_cast<double>(regionLength)) + clamp01(normalized) *
                              (std::log(static_cast<double>(minimum)) - std::log(static_cast<double>(regionLength))));
    const std::uint32_t n = roundNonnegative(x);
    return n < 1 ? 1 : (n > regionLength ? regionLength : n);
}

inline std::uint32_t windowEdge(std::uint32_t n, bool smooth) {
    if (n <= 2) return 0;
    const std::uint32_t shortEdge = 96;
    const std::uint32_t wanted = smooth ?
        (roundNonnegative(0.20 * n) > shortEdge ? roundNonnegative(0.20 * n) : shortEdge) : shortEdge;
    const std::uint32_t cap = (n - 1) / 2;
    return wanted < cap ? wanted : cap;
}
inline double window(std::uint32_t n, std::uint32_t age, bool smooth) {
    if (!n || age >= n) return 0.0;
    const std::uint32_t edge = windowEdge(n, smooth);
    if (!edge) return 1.0;
    const std::uint32_t distance = age < n - 1 - age ? age : n - 1 - age;
    const double phase = clamp01(static_cast<double>(distance) / edge);
    return 0.5 - 0.5 * std::cos(kPi * phase);
}

inline double morphDensity(double morph) {
    const double m = clamp01(morph);
    if (m <= 1.0 / 6.0) return 0.9 + 0.6 * m;
    if (m <= 0.5) return 1.0 + 3.0 * (m - 1.0 / 6.0);
    if (m <= 5.0 / 6.0) return 2.0 + 3.0 * (m - 0.5);
    return 3.0 + 6.0 * (m - 5.0 / 6.0);
}
inline double unityBlend(double density, bool smooth) {
    return smooth ? 0.0 : clamp01(1.0 - std::fabs(density - 1.0) / 0.025);
}
inline double effectiveWindow(double base, double density, bool smooth) {
    const double blend = unityBlend(density, smooth);
    return (1.0 - blend) * base + blend;
}

inline double cubic(double a, double b, double c, double d, double t) {
    return b + 0.5 * t * (c - a + t * (2*a - 5*b + 4*c - d + t * (3*(b-c) + d - a)));
}
inline double wrapPosition(double p, Region region) {
    const double begin = region.begin;
    const double length = static_cast<double>(region.end - region.begin);
    if (!length || !finite(p)) return begin;
    if (p >= begin && p < region.end) return p;
    const double relative = p - begin;
    const double result = relative - std::floor(relative / length) * length;
    return begin + (result >= length ? 0.0 : result);
}
inline std::uint32_t wrapTap(std::int64_t base, int offset, Region region) {
    const std::int64_t length = region.end - region.begin;
    if (length <= 0) return region.begin;
    std::int64_t relative = (base - region.begin + offset) % length;
    if (relative < 0) relative += length;
    return region.begin + static_cast<std::uint32_t>(relative);
}
inline StereoFrame readCubic(const StereoFrame* reel, Region region, double coordinate) {
    if (!reel || region.end <= region.begin) return StereoFrame{0.f, 0.f};
    const double wrapped = wrapPosition(coordinate, region);
    const std::int64_t base = static_cast<std::int64_t>(std::floor(wrapped));
    const double t = wrapped - base;
    const StereoFrame a = reel[wrapTap(base, -1, region)];
    const StereoFrame b = reel[wrapTap(base, 0, region)];
    const StereoFrame c = reel[wrapTap(base, 1, region)];
    const StereoFrame d = reel[wrapTap(base, 2, region)];
    return StereoFrame{static_cast<float>(cubic(a.l,b.l,c.l,d.l,t)),
                       static_cast<float>(cubic(a.r,b.r,c.r,d.r,t))};
}

struct Xorshift32 {
    std::uint32_t state;
    explicit Xorshift32(std::uint32_t seed = kDefaultSeed) : state(seed ? seed : kDefaultSeed) {}
    std::uint32_t next() {
        state ^= state << 13; state ^= state >> 17; state ^= state << 5;
        return state;
    }
    double uniform() { return static_cast<double>(next() >> 8) / 16777216.0; }
};
struct OnsetChoice { bool chord; double pan; std::uint8_t slot; double ratio; };
inline OnsetChoice chooseOnset(Xorshift32& random, std::uint8_t slot,
                               double morph, const double ratios[3]) {
    const double high = clamp01((clamp01(morph) - 5.0/6.0) * 6.0);
    const double activation = random.uniform();
    const double panDraw = random.uniform();
    const bool chord = slot != 0 && activation < high;
    return OnsetChoice{chord, (2.0 * panDraw - 1.0) * high,
                       static_cast<std::uint8_t>(slot % kMusicalVoices),
                       chord ? ratios[(slot - 1) % 3] : 1.0};
}
inline void stereoBalance(double pan, double& left, double& right) {
    const double angle = kPi * (clamp(pan, -1.0, 1.0) + 1.0) / 4.0;
    left = std::sqrt(2.0) * std::cos(angle);
    right = std::sqrt(2.0) * std::sin(angle);
}
inline std::uint64_t finiteCompletionFrame(std::uint64_t born, std::uint32_t duration) {
    return born + duration;
}

} // namespace profile1
} // namespace chimera
