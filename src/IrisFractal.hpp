#pragma once

#include "IrisWavetable.hpp"

#include <algorithm>
#include <cmath>

namespace iris {

namespace detail {

inline uint8_t fractalByte(float x) {
  x = clamp01(x);
  return uint8_t(std::round(x * 255.f));
}

inline void hsvToRgb(float h, float s, float v, uint8_t* r, uint8_t* g, uint8_t* b) {
  h = h - std::floor(h);
  s = clamp01(s);
  v = clamp01(v);
  const float c = v * s;
  const float hp = h * 6.f;
  const float x = c * (1.f - std::fabs(std::fmod(hp, 2.f) - 1.f));
  float rr = 0.f;
  float gg = 0.f;
  float bb = 0.f;
  if (hp < 1.f) {
    rr = c; gg = x;
  } else if (hp < 2.f) {
    rr = x; gg = c;
  } else if (hp < 3.f) {
    gg = c; bb = x;
  } else if (hp < 4.f) {
    gg = x; bb = c;
  } else if (hp < 5.f) {
    rr = x; bb = c;
  } else {
    rr = c; bb = x;
  }
  const float m = v - c;
  *r = fractalByte(rr + m);
  *g = fractalByte(gg + m);
  *b = fractalByte(bb + m);
}

inline void writePalette(float t, float orbit, size_t base, std::vector<uint8_t>* rgb8) {
  t = clamp01(t);
  orbit = clamp01(orbit);
  uint8_t r = 0u;
  uint8_t g = 0u;
  uint8_t b = 0u;
  hsvToRgb(0.64f + 1.35f * t + 0.08f * orbit, 0.72f + 0.20f * orbit,
           0.18f + 0.82f * std::sqrt(t), &r, &g, &b);
  (*rgb8)[base + 0u] = r;
  (*rgb8)[base + 1u] = g;
  (*rgb8)[base + 2u] = b;
}

inline void writeSetInterior(size_t base, std::vector<uint8_t>* rgb8) {
  (*rgb8)[base + 0u] = 7u;
  (*rgb8)[base + 1u] = 4u;
  (*rgb8)[base + 2u] = 18u;
}

} // namespace detail

inline bool makeBuiltinFractalSource(
  int mode,
  float zoom,
  float centerX,
  float centerY,
  SourceField* out,
  std::string* error = nullptr) {
  if (!out) {
    if (error) *error = "Missing fractal output";
    return false;
  }
  if (!isBuiltinFractalMode(mode)) {
    if (error) *error = "Unknown built-in fractal mode";
    return false;
  }

  SourceField source;
  source.width = kCanonicalSourceWidth;
  source.height = kCanonicalSourceHeight;
  source.channels = kCanonicalSourceChannels;
  source.bitDepth = kCanonicalSourceBitDepth;
  source.originalWidth = source.width;
  source.originalHeight = source.height;
  source.originalChannels = source.channels;
  source.sourceName = std::string("Fractal: ") + builtinFractalName(mode);
  source.rgb8.assign(size_t(source.width) * size_t(source.height) * 3u, 0u);

  zoom = clamp(zoom, 0.f, 1.f);
  const double zoomScale = std::pow(0.05, double(zoom));
  const double panX = clamp(centerX, -2.f, 2.f);
  const double panY = clamp(centerY, -2.f, 2.f);
  const int maxIter = (mode == FRACTAL_NEWTON || mode == FRACTAL_NOVA)
    ? 36
    : (mode == FRACTAL_EYE_OF_THE_WORLD ? 360 : 140);
  for (int y = 0; y < source.height; ++y) {
    const float ny = (float(y) + 0.5f) / float(source.height) * 2.f - 1.f;
    for (int x = 0; x < source.width; ++x) {
      const float nx = (float(x) + 0.5f) / float(source.width) * 2.f - 1.f;
      const size_t base = (size_t(y) * size_t(source.width) + size_t(x)) * 3u;

      if (mode == FRACTAL_NEWTON || mode == FRACTAL_NOVA) {
        double zr = panX + nx * (mode == FRACTAL_NOVA ? 2.0 : 2.45) * zoomScale;
        double zi = panY + ny * (mode == FRACTAL_NOVA ? 0.86 : 0.98) * zoomScale;
        const double cr = -0.52;
        const double ci = 0.38;
        int iter = 0;
        for (; iter < maxIter; ++iter) {
          const double zr2 = zr * zr;
          const double zi2 = zi * zi;
          const double denom = 3.0 * ((zr2 - zi2) * (zr2 - zi2) + 4.0 * zr2 * zi2);
          if (denom < 1e-14) break;
          double nr = (2.0 * zr * (zr2 + zi2) + (zr2 - zi2)) / denom;
          double ni = (2.0 * zi * (zr2 + zi2) - 2.0 * zr * zi) / denom;
          if (mode == FRACTAL_NOVA) {
            nr += cr;
            ni += ci;
          }
          const double dr = nr - zr;
          const double di = ni - zi;
          zr = nr;
          zi = ni;
          if (dr * dr + di * di < 1e-12) break;
          if (zr * zr + zi * zi > 64.0) break;
        }
        const double d0 = (zr - 1.0) * (zr - 1.0) + zi * zi;
        const double d1 = (zr + 0.5) * (zr + 0.5) + (zi - 0.86602540378) * (zi - 0.86602540378);
        const double d2 = (zr + 0.5) * (zr + 0.5) + (zi + 0.86602540378) * (zi + 0.86602540378);
        const int root = d0 < d1 && d0 < d2 ? 0 : (d1 < d2 ? 1 : 2);
        const float t = 1.f - float(iter) / float(maxIter);
        const float v = 0.20f + 0.80f * std::sqrt(clamp01(t));
        const float phase = mode == FRACTAL_NOVA ? 0.18f : 0.f;
        source.rgb8[base + 0u] = detail::fractalByte((root == 0 ? 0.95f : 0.20f + phase) * v);
        source.rgb8[base + 1u] = detail::fractalByte((root == 1 ? 0.90f : 0.30f + phase) * v);
        source.rgb8[base + 2u] = detail::fractalByte((root == 2 ? 1.00f : 0.46f + phase) * v);
        continue;
      }

      double cr = 0.0;
      double ci = 0.0;
      double zr = 0.0;
      double zi = 0.0;
      double pr = 0.0;
      double pi = 0.0;
      if (mode == FRACTAL_MANDELBROT) {
        cr = -0.72 + panX + double(nx) * 1.62 * zoomScale;
        ci = 0.03 + panY + double(ny) * 0.86 * zoomScale;
      } else if (mode == FRACTAL_EYE_OF_THE_WORLD) {
        cr = -0.743643887037151 + panX + double(nx) * 0.0075 * zoomScale;
        ci = 0.131825904205330 + panY + double(ny) * 0.00395 * zoomScale;
      } else if (mode == FRACTAL_JULIA) {
        cr = -0.74543;
        ci = 0.11301;
        zr = panX + double(nx) * 1.58 * zoomScale;
        zi = panY + double(ny) * 0.72 * zoomScale;
      } else if (mode == FRACTAL_PHOENIX_JULIA) {
        cr = -0.42;
        ci = 0.08;
        zr = panX + double(nx) * 1.62 * zoomScale;
        zi = panY + double(ny) * 0.74 * zoomScale;
      } else if (mode == FRACTAL_BURNING_SHIP) {
        cr = -1.76 + panX + double(nx) * 0.42 * zoomScale;
        ci = -0.045 + panY + double(ny) * 0.145 * zoomScale;
      } else if (mode == FRACTAL_CELTIC) {
        cr = -0.25 + panX + double(nx) * 1.62 * zoomScale;
        ci = 0.02 + panY + double(ny) * 0.88 * zoomScale;
      } else if (mode == FRACTAL_SPIDER) {
        cr = -0.52 + panX + double(nx) * 1.56 * zoomScale;
        ci = panY + double(ny) * 0.84 * zoomScale;
      } else {
        cr = -0.12 + panX + double(nx) * 1.68 * zoomScale;
        ci = panY + double(ny) * 0.90 * zoomScale;
      }

      double minOrbit = 1e9;
      int iter = 0;
      double mag2 = 0.0;
      for (; iter < maxIter; ++iter) {
        if (mode == FRACTAL_BURNING_SHIP) {
          zr = std::fabs(zr);
          zi = std::fabs(zi);
        }
        const double zr2 = zr * zr;
        const double zi2 = zi * zi;
        minOrbit = std::min(minOrbit, zr2 + zi2);
        if (mode == FRACTAL_PHOENIX_JULIA) {
          const double nextR = zr2 - zi2 + cr + 0.48 * pr;
          const double nextI = 2.0 * zr * zi + ci + 0.48 * pi;
          pr = zr;
          pi = zi;
          zr = nextR;
          zi = nextI;
        } else if (mode == FRACTAL_TRICORN) {
          const double nextR = zr2 - zi2 + cr;
          const double nextI = -2.0 * zr * zi + ci;
          zr = nextR;
          zi = nextI;
        } else if (mode == FRACTAL_CELTIC) {
          const double nextR = std::fabs(zr2 - zi2) + cr;
          const double nextI = 2.0 * zr * zi + ci;
          zr = nextR;
          zi = nextI;
        } else if (mode == FRACTAL_SPIDER) {
          const double nextR = zr2 - zi2 + cr;
          const double nextI = 2.0 * zr * zi + ci;
          cr = 0.5 * cr + nextR;
          ci = 0.5 * ci + nextI;
          zr = nextR;
          zi = nextI;
        } else {
          zi = 2.0 * zr * zi + ci;
          zr = zr2 - zi2 + cr;
        }
        mag2 = zr * zr + zi * zi;
        if (mag2 > 16.0) break;
      }
      if (iter >= maxIter) {
        detail::writeSetInterior(base, &source.rgb8);
      } else {
        const float smooth = float(iter) + 1.f -
          float(std::log(std::log(std::sqrt(std::max(mag2, 1.000001)))) / std::log(2.0));
        const float t = clamp01(smooth / float(maxIter));
        const float orbit = 1.f - clamp01(float(std::sqrt(std::min(minOrbit, 4.0)) * 0.5));
        detail::writePalette(t, orbit, base, &source.rgb8);
      }
    }
  }

  if (!source.valid()) {
    if (error) *error = "Generated fractal source is invalid";
    return false;
  }
  *out = std::move(source);
  return true;
}

} // namespace iris
