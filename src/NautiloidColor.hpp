#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace nautiloid_color {

enum Mode {
  PRISM,
  ABYSS,
  EMBER,
  AMETHYST,
  EMERALD,
  INVERTED,
  MODES_LEN
};

struct Rgb {
  float r;
  float g;
  float b;
};

inline int normalize(int mode) {
  return std::max(int(PRISM), std::min(mode, int(MODES_LEN) - 1));
}

inline const char* name(int mode) {
  switch (normalize(mode)) {
    case ABYSS: return "Abyss";
    case EMBER: return "Ember";
    case AMETHYST: return "Amethyst";
    case EMERALD: return "Emerald";
    case INVERTED: return "Inverted";
    case PRISM:
    default: return "Prism";
  }
}

inline Rgb stop(int mode, int index) {
  static constexpr Rgb palettes[4][3] = {
    {{2.f, 7.f, 18.f}, {5.f, 100.f, 172.f}, {144.f, 247.f, 255.f}},
    {{18.f, 3.f, 2.f}, {202.f, 42.f, 8.f}, {255.f, 231.f, 104.f}},
    {{11.f, 3.f, 22.f}, {126.f, 25.f, 194.f}, {255.f, 171.f, 246.f}},
    {{2.f, 15.f, 10.f}, {0.f, 168.f, 107.f}, {166.f, 255.f, 207.f}}
  };
  const int palette = std::max(0, std::min(normalize(mode) - int(ABYSS), 3));
  return palettes[palette][std::max(0, std::min(index, 2))];
}

inline void applyPixel(int mode, uint8_t r, uint8_t g, uint8_t b,
                       uint8_t* outR, uint8_t* outG, uint8_t* outB) {
  mode = normalize(mode);
  if (mode == PRISM) {
    *outR = r;
    *outG = g;
    *outB = b;
    return;
  }
  if (mode == INVERTED) {
    // Preserve Prism's near-black set interior while reversing the escaped
    // exterior colors. Without this exception the fractal's bulb turns white.
    const bool setInterior = r == 7u && g == 4u && b == 18u;
    *outR = setInterior ? r : uint8_t(255u - r);
    *outG = setInterior ? g : uint8_t(255u - g);
    *outB = setInterior ? b : uint8_t(255u - b);
    return;
  }
  // The canonical palette's maximum channel is its HSV value component. This
  // retains escape-band structure while replacing the hue scheme.
  const float value = float(std::max(r, std::max(g, b))) / 255.f;
  constexpr float split = 0.52f;
  const int lowIndex = value < split ? 0 : 1;
  const float t = value < split ? value / split : (value - split) / (1.f - split);
  const Rgb low = stop(mode, lowIndex);
  const Rgb high = stop(mode, lowIndex + 1);
  *outR = uint8_t(std::lround(low.r + (high.r - low.r) * t));
  *outG = uint8_t(std::lround(low.g + (high.g - low.g) * t));
  *outB = uint8_t(std::lround(low.b + (high.b - low.b) * t));
}

inline void applyRgb8(int mode, std::vector<uint8_t>* rgb8) {
  if (!rgb8 || normalize(mode) == PRISM) return;
  for (size_t i = 0; i + 2u < rgb8->size(); i += 3u) {
    applyPixel(mode, (*rgb8)[i], (*rgb8)[i + 1u], (*rgb8)[i + 2u],
      &(*rgb8)[i], &(*rgb8)[i + 1u], &(*rgb8)[i + 2u]);
  }
}

} // namespace nautiloid_color
