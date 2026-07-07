#include "../src/IrisWavetable.hpp"
#include "../src/IrisIO.hpp"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <limits>
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
  check("source preview keeps full RGB",
        red.sourcePreviewWidth == iris::kSourcePreviewWidth &&
        red.sourcePreviewHeight == iris::kSourcePreviewHeight &&
        red.sourcePreviewRgb.size() == size_t(iris::kSourcePreviewWidth * iris::kSourcePreviewHeight * 3) &&
        red.sourcePreviewRgb[0] == 255u && red.sourcePreviewRgb[1] == 0u &&
        red.sourcePreviewRgb[2] == 0u);
  settings.imageChannelMode = iris::IMAGE_CHANNEL_RED;
  iris::ImageWavetable redChannel;
  check("red channel interpretation selects red",
        iris::buildWavetableFromRgba(pixels.data(), 1, 1, 4, settings, &redChannel) &&
        redChannel.samples[0] > 0.99f);
  settings.imageChannelMode = iris::IMAGE_CHANNEL_GREEN;
  iris::ImageWavetable greenChannel;
  check("green channel interpretation rejects red",
        iris::buildWavetableFromRgba(pixels.data(), 1, 1, 4, settings, &greenChannel) &&
        greenChannel.samples[0] < -0.99f);
  settings.imageChannelMode = iris::IMAGE_CHANNEL_BLUE;
  iris::ImageWavetable blueChannel;
  check("blue channel interpretation rejects red",
        iris::buildWavetableFromRgba(pixels.data(), 1, 1, 4, settings, &blueChannel) &&
        blueChannel.samples[0] < -0.99f);
  settings.imageChannelMode = iris::IMAGE_CHANNEL_ALL;

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
  check("binary round trip preserves source preview",
        restored.sourcePreviewWidth == horizontal.sourcePreviewWidth &&
        restored.sourcePreviewHeight == horizontal.sourcePreviewHeight &&
        restored.sourcePreviewRgb == horizontal.sourcePreviewRgb);
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

  const iris::ImageWavetable factory = iris::makeDefaultTable();
  check("factory table has full row terrain", factory.rowCount == iris::kDefaultRows);
  check("factory table starts with sine",
        std::fabs(factory.sample(0.125f, 0.f) - std::sin(0.25f * 3.14159265358979323846f)) < 1e-5f);
  check("factory table reaches triangle at first third",
        std::fabs(factory.sample(0.125f, 1.f / 3.f) - 0.5f) < 0.01f);
  check("factory table reaches saw at second third",
        std::fabs(factory.sample(0.125f, 2.f / 3.f) - 0.25f) < 0.01f);
  check("factory table ends with square",
        std::fabs(factory.sample(0.125f, 1.f) - 1.f) < 1e-5f);

  const float nan = std::numeric_limits<float>::quiet_NaN();
  check("non-finite phase is contained", std::isfinite(factory.sample(nan, 0.5f)));
  check("non-finite scan is contained", std::isfinite(factory.sample(0.25f, nan)));
  iris::WavetableOscillator guardedOscillator;
  guardedOscillator.phase = nan;
  const float guardedOut = guardedOscillator.process(factory, nan, nan, nan);
  check("non-finite oscillator state is contained",
        std::isfinite(guardedOut) && std::isfinite(guardedOscillator.phase));
  guardedOscillator.reset(nan);
  check("non-finite reset phase is contained", guardedOscillator.phase == 0.f);

  iris::ImageWavetable shortStride = factory;
  shortStride.stride = shortStride.frameSize;
  shortStride.samples.resize(size_t(shortStride.rowCount) * size_t(shortStride.stride));
  check("table rejects stride without wrap sample", !shortStride.valid());

  iris::ImageWavetable nonFiniteTable = factory;
  nonFiniteTable.samples[10] = nan;
  check("binary writer rejects non-finite samples",
        !iris::saveBinaryTable(binaryPath, nonFiniteTable, &ioError));
  std::remove(binaryPath.c_str());

  iris::ConversionSettings smoothingSettings;
  smoothingSettings.frameSize = 8;
  smoothingSettings.rows = 2;
  smoothingSettings.normalizeMode = iris::NORMALIZE_NONE;
  smoothingSettings.waveSmoothing = 1.f;
  std::vector<uint8_t> jagged(size_t(8 * 2) * 4u, 255u);
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 8; ++x) {
      const uint8_t value = (x & 1) ? 255u : 0u;
      const size_t base = size_t(y * 8 + x) * 4u;
      jagged[base + 0u] = jagged[base + 1u] = jagged[base + 2u] = value;
    }
  }
  iris::ImageWavetable smoothed;
  check("wave smoothing converts jagged image",
        iris::buildWavetableFromRgba(jagged.data(), 8, 2, 4, smoothingSettings, &smoothed));
  float maxAbs = 0.f;
  for (int x = 0; x < smoothed.frameSize; ++x) {
    maxAbs = std::max(maxAbs, std::fabs(smoothed.samples[size_t(x)]));
  }
  check("wave smoothing reduces alternating jagged amplitude", maxAbs < 0.35f);

  std::cout << "Summary: " << (41 - failures) << "/41 passed\n";
  return failures == 0 ? 0 : 1;
}
