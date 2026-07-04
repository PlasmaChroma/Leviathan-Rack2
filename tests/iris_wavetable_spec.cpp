#include "../src/IrisWavetable.hpp"
#include "../src/IrisIO.hpp"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void check(const std::string& name, bool condition) {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << name << "\n";
  if (!condition) ++failures;
}

std::vector<uint8_t> solid(int width, int height, uint8_t value) {
  std::vector<uint8_t> pixels(size_t(width * height) * 4u, 255u);
  for (int i = 0; i < width * height; ++i) {
    pixels[size_t(i) * 4u + 0u] = value;
    pixels[size_t(i) * 4u + 1u] = value;
    pixels[size_t(i) * 4u + 2u] = value;
  }
  return pixels;
}

} // namespace

int main() {
  iris::ConversionSettings settings;
  settings.frameSize = 8;
  settings.rows = 2;

  iris::ImageWavetable black;
  std::vector<uint8_t> pixels = solid(2, 2, 0u);
  check("black image converts", iris::buildWavetableFromRgba(pixels.data(), 2, 2, 4, settings, &black));
  check("black maps to -1", std::fabs(black.samples[0] + 1.f) < 1e-6f);

  iris::ImageWavetable white;
  pixels = solid(2, 2, 255u);
  check("white image converts", iris::buildWavetableFromRgba(pixels.data(), 2, 2, 4, settings, &white));
  check("white maps to +1", std::fabs(white.samples[0] - 1.f) < 1e-6f);

  settings.normalizeMode = iris::NORMALIZE_NONE;
  iris::ImageWavetable gray;
  pixels = solid(1, 1, 128u);
  check("gray image converts", iris::buildWavetableFromRgba(pixels.data(), 1, 1, 4, settings, &gray));
  check("50 percent gray maps near zero", std::fabs(gray.samples[0]) < 0.01f);

  pixels.resize(2u * 2u * 4u, 255u);
  for (int y = 0; y < 2; ++y) {
    const size_t left = size_t(y * 2) * 4u;
    pixels[left + 0u] = pixels[left + 1u] = pixels[left + 2u] = 0u;
  }
  iris::ImageWavetable horizontal;
  check("horizontal gradient converts",
        iris::buildWavetableFromRgba(pixels.data(), 2, 2, 4, settings, &horizontal));
  check("horizontal gradient rises across phase",
        horizontal.sample(0.05f, 0.f) < horizontal.sample(0.45f, 0.f));

  pixels = solid(2, 2, 0u);
  for (int x = 0; x < 2; ++x) {
    const size_t bottom = size_t(2 + x) * 4u;
    pixels[bottom + 0u] = pixels[bottom + 1u] = pixels[bottom + 2u] = 255u;
  }
  iris::ImageWavetable vertical;
  check("vertical gradient converts",
        iris::buildWavetableFromRgba(pixels.data(), 2, 2, 4, settings, &vertical));
  const float top = vertical.sample(0.f, 0.f);
  const float bottom = vertical.sample(0.f, 1.f);
  check("vertical scan follows image rows", top < bottom);
  check("vertical interpolation is linear",
        std::fabs(vertical.sample(0.f, 0.5f) - 0.5f * (top + bottom)) < 1e-5f);

  iris::ImageWavetable manual;
  manual.frameSize = 4;
  manual.stride = 5;
  manual.rowCount = 2;
  manual.samples = {0.f, 1.f, 0.f, -1.f, 0.f, 1.f, 0.f, -1.f, 0.f, 1.f};
  check("horizontal interpolation", std::fabs(manual.sample(0.125f, 0.f) - 0.5f) < 1e-6f);
  check("phase wrap interpolation", std::fabs(manual.sample(0.875f, 0.f) + 0.5f) < 1e-6f);

  iris::WavetableOscillator oscillator;
  oscillator.process(manual, 1.f, 0.25f, 0.f);
  check("oscillator phase advances", std::fabs(oscillator.phase - 0.25f) < 1e-6f);
  oscillator.reset(0.9f);
  oscillator.process(manual, 1.f, 0.25f, 0.f);
  check("oscillator phase wraps", std::fabs(oscillator.phase - 0.15f) < 1e-5f);

  settings.invert = true;
  iris::ImageWavetable inverted;
  pixels = solid(2, 2, 255u);
  check("inverted image converts",
        iris::buildWavetableFromRgba(pixels.data(), 2, 2, 4, settings, &inverted));
  check("inversion flips polarity", inverted.samples[0] < -0.99f);

  settings.invert = false;
  settings.normalizeMode = iris::NORMALIZE_NONE;
  pixels = solid(1, 1, 0u);
  pixels[0] = 255u;
  iris::ImageWavetable red;
  check("RGBA red image converts",
        iris::buildWavetableFromRgba(pixels.data(), 1, 1, 4, settings, &red));
  check("RGBA channels use resized pixel data", std::fabs(red.samples[0] - (2.f * 0.299f - 1.f)) < 0.01f);

  settings.trimMode = iris::TRIM_AGGRESSIVE;
  pixels = solid(2, 4, 128u);
  iris::ImageWavetable flat;
  check("flat-row image converts",
        iris::buildWavetableFromRgba(pixels.data(), 2, 4, 4, settings, &flat));
  check("trimming preserves at least two rows", flat.rowCount >= 2);
  check("wrap sample duplicates row start",
        std::fabs(flat.samples[size_t(flat.frameSize)] - flat.samples[0]) < 1e-7f);

  const std::string binaryPath = "/tmp/leviathan_iris_wavetable_spec.bin";
  std::string ioError;
  check("binary table saves", iris::saveBinaryTable(binaryPath, horizontal, &ioError));
  iris::ImageWavetable restored;
  check("binary table loads", iris::loadBinaryTable(binaryPath, &restored, &ioError));
  check("binary round trip preserves samples",
        restored.samples.size() == horizontal.samples.size() &&
        std::fabs(restored.samples[3] - horizontal.samples[3]) < 1e-7f);
  std::remove(binaryPath.c_str());

  settings.frameSize = 5;
  settings.rows = 2;
  settings.trimMode = iris::TRIM_OFF;
  settings.normalizeMode = iris::NORMALIZE_BALANCED;
  pixels.resize(5u * 2u * 4u, 255u);
  const uint8_t levels[5] = {0u, 32u, 96u, 160u, 255u};
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 5; ++x) {
      const size_t base = size_t(y * 5 + x) * 4u;
      pixels[base + 0u] = pixels[base + 1u] = pixels[base + 2u] = levels[x];
    }
  }
  iris::ImageWavetable balanced;
  check("balanced normalization converts",
        iris::buildWavetableFromRgba(pixels.data(), 5, 2, 4, settings, &balanced));
  check("balanced normalization maps median to zero", std::fabs(balanced.samples[2]) < 1e-5f);
  check("balanced normalization retains both polarities",
        balanced.samples[0] < -0.9f && balanced.samples[4] > 0.9f);

  std::cout << "Summary: " << (28 - failures) << "/28 passed\n";
  return failures == 0 ? 0 : 1;
}
