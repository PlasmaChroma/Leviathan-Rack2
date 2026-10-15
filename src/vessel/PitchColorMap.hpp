#pragma once
#include <array>
#include <algorithm>
#include <cmath>

namespace vessel_pitch_color {
constexpr int count = 7;
static const float centers[count] = {100.25f, 112.50f, 119.32f, 127.25f, 141.72f, 155.71f, 171.21f};
static const unsigned char colors[count][3] = {
    {255, 56, 72}, {255, 124, 35}, {255, 220, 50}, {40, 224, 110},
    {50, 180, 255}, {80, 90, 245}, {200, 70, 235}
};
static const char* const names[count] = {
    "Root", "Sacral", "Solar plexus", "Heart", "Throat", "Third eye", "Crown"
};
using Weights = std::array<float, count>;

// The bowl's original A4=440 color-derived bands, with octave folding and
// linear interpolation between their geometric centers. No log/pow needed.
inline Weights weights(float frequency) {
    Weights result{};
    if (!std::isfinite(frequency) || frequency <= 0.f) return result;
    float folded = std::max(frequency, 1.f);
    while (folded < 92.09f) folded *= 2.f;
    while (folded >= 184.17f) folded *= .5f;
    int left = 6, right = 0;
    float low = centers[6], high = centers[0] * 2.f;
    if (folded < centers[0]) low = centers[6] * .5f, high = centers[0];
    else for (int i = 0; i < count - 1; ++i) {
        if (folded < centers[i + 1]) {
            left = i; right = i + 1; low = centers[i]; high = centers[i + 1];
            break;
        }
    }
    const float blend = std::max(0.f, std::min(1.f, (folded - low) / (high - low)));
    result[left] = 1.f - blend;
    result[right] = blend;
    return result;
}
}
