#pragma once

#include "IrisWavetable.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace nautiloid_location {

constexpr size_t kPayloadSize = 12u;
constexpr size_t kEncodedLength = 16u;
constexpr uint8_t kFormatVersion = 1u;
constexpr float kMinZoom = 0.f;
constexpr float kMaxZoom = 4.f;
constexpr double kMinCoordinate = -2.0;
constexpr double kMaxCoordinate = 2.0;

static_assert(iris::kLastBuiltinFractalMode <= 0x0f,
              "Nautiloid location codes reserve four bits for fractal mode");

struct State {
  int mode = iris::FRACTAL_MANDELBROT;
  float zoom = 0.f;
  double centerX = 0.0;
  double centerY = 0.0;
};

struct DecodeResult {
  bool valid = false;
  State state;
  std::string error;
};

State canonicalize(const State& requested);
std::string encode(const State& state);
DecodeResult decode(const std::string& text);
bool isValid(const std::string& text);

} // namespace nautiloid_location
