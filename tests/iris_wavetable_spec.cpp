#include "../src/IrisWavetable.hpp"
#include "../src/IrisIO.hpp"
#include "../src/NautiloidFractal.hpp"

#define QOI_IMPLEMENTATION
#include "../src/third_party/qoi.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

int failures = 0;
int checks = 0;

void check(const std::string& name, bool condition) {
  ++checks;
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

  std::vector<uint8_t> alphaA(size_t(2 * 2) * 4u, 0u);
  std::vector<uint8_t> alphaB(size_t(2 * 2) * 4u, 0u);
  for (int i = 0; i < 4; ++i) {
    alphaA[size_t(i) * 4u + 0u] = alphaB[size_t(i) * 4u + 0u] = uint8_t(20 + i * 10);
    alphaA[size_t(i) * 4u + 1u] = alphaB[size_t(i) * 4u + 1u] = uint8_t(90 + i * 10);
    alphaA[size_t(i) * 4u + 2u] = alphaB[size_t(i) * 4u + 2u] = uint8_t(150 + i * 10);
    alphaA[size_t(i) * 4u + 3u] = 0u;
    alphaB[size_t(i) * 4u + 3u] = 255u;
  }
  iris::SourceField sourceA;
  iris::SourceField sourceB;
  check("source field import succeeds",
        iris::buildSourceFieldFromRgba8(alphaA.data(), 2, 2, 4, &sourceA));
  check("source field uses canonical dimensions",
        sourceA.width == iris::kCanonicalSourceWidth &&
        sourceA.height == iris::kCanonicalSourceHeight &&
        sourceA.channels == 3 && sourceA.bitDepth == 8 && sourceA.valid());
  check("alpha does not influence source field",
        iris::buildSourceFieldFromRgba8(alphaB.data(), 2, 2, 4, &sourceB) &&
        sourceA.rgb8 == sourceB.rgb8);
  sourceA.sourcePath = "/tmp/source-image.png";
  sourceA.sourceName = "source-image.png";
  sourceA.originalWidth = 2;
  sourceA.originalHeight = 2;
  sourceA.originalChannels = 4;
  const std::string qoiPath = "/tmp/leviathan_iris_source_field_spec.qoi";
  check("source field QOI saves", iris::saveSourceField(qoiPath, sourceA, &ioError));
  iris::SourceField qoiRestored;
  check("source field QOI loads", iris::loadSourceField(qoiPath, &qoiRestored, &ioError));
  check("source field QOI round trip preserves canonical pixels",
        qoiRestored.width == sourceA.width && qoiRestored.height == sourceA.height &&
        qoiRestored.channels == sourceA.channels && qoiRestored.rgb8 == sourceA.rgb8);
  iris::ImageWavetable sourceBuiltA;
  iris::ImageWavetable sourceBuiltB;
  check("source field builds wavetable",
        iris::buildWavetableFromSourceField(sourceA, settings, &sourceBuiltA, &ioError));
  check("QOI-restored source builds same wavetable",
        iris::buildWavetableFromSourceField(qoiRestored, settings, &sourceBuiltB, &ioError) &&
        sourceBuiltA.samples == sourceBuiltB.samples);
  std::remove(qoiPath.c_str());

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

  const uint8_t darkDominantLevels[10] = {0u, 0u, 0u, 0u, 0u, 0u, 0u, 64u, 128u, 255u};
  pixels.resize(10u * 2u * 4u, 255u);
  settings.frameSize = 10;
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 10; ++x) {
      const size_t base = size_t(y * 10 + x) * 4u;
      pixels[base + 0u] = pixels[base + 1u] = pixels[base + 2u] = darkDominantLevels[x];
    }
  }
  iris::ImageWavetable darkDominantBalanced;
  check("balanced normalization rescues a median collapsed onto the dark floor",
        iris::buildWavetableFromRgba(pixels.data(), 10, 2, 4, settings, &darkDominantBalanced) &&
        darkDominantBalanced.samples[0] < -0.9f && darkDominantBalanced.samples[9] > 0.9f);

  const uint8_t brightDominantLevels[10] = {0u, 127u, 191u, 255u, 255u, 255u, 255u, 255u, 255u, 255u};
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 10; ++x) {
      const size_t base = size_t(y * 10 + x) * 4u;
      pixels[base + 0u] = pixels[base + 1u] = pixels[base + 2u] = brightDominantLevels[x];
    }
  }
  iris::ImageWavetable brightDominantBalanced;
  check("balanced normalization rescues a median collapsed onto the bright ceiling",
        iris::buildWavetableFromRgba(pixels.data(), 10, 2, 4, settings, &brightDominantBalanced) &&
        brightDominantBalanced.samples[0] < -0.9f && brightDominantBalanced.samples[9] > 0.9f);

  const uint8_t separatedLevels[2][5] = {
    {0u, 24u, 48u, 72u, 96u},
    {160u, 184u, 208u, 232u, 255u},
  };
  pixels.resize(5u * 2u * 4u, 255u);
  settings.frameSize = 5;
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 5; ++x) {
      const size_t base = size_t(y * 5 + x) * 4u;
      pixels[base + 0u] = pixels[base + 1u] = pixels[base + 2u] =
        separatedLevels[y][x];
    }
  }
  iris::ImageWavetable rowBalanced;
  check("balanced normalization converts separated row ranges",
        iris::buildWavetableFromRgba(pixels.data(), 5, 2, 4, settings, &rowBalanced));
  const size_t secondRowBase = size_t(rowBalanced.stride);
  bool separatedRowsRetainPolarity = true;
  for (int x = 0; x < rowBalanced.frameSize; ++x) {
    separatedRowsRetainPolarity = separatedRowsRetainPolarity &&
      rowBalanced.samples[size_t(x)] < 0.f &&
      rowBalanced.samples[secondRowBase + size_t(x)] > 0.f;
  }
  check("balanced normalization preserves image-wide row relationships",
        separatedRowsRetainPolarity);

  const uint8_t overlappingLevels[2][5] = {
    {0u, 32u, 64u, 96u, 128u},
    {64u, 96u, 128u, 160u, 192u},
  };
  for (int y = 0; y < 2; ++y) {
    for (int x = 0; x < 5; ++x) {
      const size_t base = size_t(y * 5 + x) * 4u;
      pixels[base + 0u] = pixels[base + 1u] = pixels[base + 2u] =
        overlappingLevels[y][x];
    }
  }
  iris::ImageWavetable globallyBalanced;
  check("balanced normalization converts overlapping row ranges",
        iris::buildWavetableFromRgba(pixels.data(), 5, 2, 4, settings, &globallyBalanced));
  check("balanced normalization uses one image-wide transfer function",
        std::fabs(globallyBalanced.samples[2] -
                  globallyBalanced.samples[size_t(globallyBalanced.stride)]) < 1e-5f);

  settings.dcRemove = true;
  iris::ImageWavetable dcBalanced;
  check("DC removal converts overlapping row ranges",
        iris::buildWavetableFromRgba(pixels.data(), 5, 2, 4, settings, &dcBalanced));
  bool everyNonFlatRowHasBothPolarities = true;
  for (int row = 0; row < dcBalanced.rowCount; ++row) {
    const size_t base = size_t(row) * size_t(dcBalanced.stride);
    bool hasNegative = false;
    bool hasPositive = false;
    for (int x = 0; x < dcBalanced.frameSize; ++x) {
      hasNegative = hasNegative || dcBalanced.samples[base + size_t(x)] < -1e-5f;
      hasPositive = hasPositive || dcBalanced.samples[base + size_t(x)] > 1e-5f;
    }
    everyNonFlatRowHasBothPolarities = everyNonFlatRowHasBothPolarities && hasNegative && hasPositive;
  }
  check("DC removal gives every non-flat row both polarities",
        everyNonFlatRowHasBothPolarities);
  settings.dcRemove = false;

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
  smoothingSettings.waveSmoothing = 1.f / iris::kWaveSmoothingCapacityScale;
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

  smoothingSettings.waveSmoothing = iris::kMaxWaveSmoothing;
  iris::ImageWavetable extendedSmoothed;
  check("extended wave smoothing converts jagged image",
        iris::buildWavetableFromRgba(
          jagged.data(), 8, 2, 4, smoothingSettings, &extendedSmoothed));
  float extendedMaxAbs = 0.f;
  for (int x = 0; x < extendedSmoothed.frameSize; ++x) {
    extendedMaxAbs = std::max(
      extendedMaxAbs, std::fabs(extendedSmoothed.samples[size_t(x)]));
  }
  check("expanded wave smoothing maximum exceeds former capacity",
        extendedMaxAbs < maxAbs);

  iris::NautiloidFractalSourceParams nautiloidParams;
  nautiloidParams.mode = iris::FRACTAL_BURNING_SHIP;
  nautiloidParams.zoom = 2.25f;
  nautiloidParams.centerX = -0.18f;
  nautiloidParams.centerY = 0.07f;
  nautiloidParams.generation = 42u;
  iris::SourceField nautiloidSource;
  check("Nautiloid Iris source helper generates canonical source",
        iris::makeNautiloidIrisSource(nautiloidParams, &nautiloidSource, &ioError) &&
        nautiloidSource.valid() &&
        nautiloidSource.sourceName == "Nautiloid: Burning Ship");
  check("Nautiloid Iris source helper stores generator metadata",
        iris::sourceHasNautiloidFractalParams(nautiloidSource) &&
        nautiloidSource.generatorFractalMode == nautiloidParams.mode &&
        std::fabs(nautiloidSource.generatorFractalZoom - nautiloidParams.zoom) < 1e-6f &&
        std::fabs(nautiloidSource.generatorFractalCenterX - nautiloidParams.centerX) < 1e-6f &&
        std::fabs(nautiloidSource.generatorFractalCenterY - nautiloidParams.centerY) < 1e-6f &&
        nautiloidSource.generatorGeneration == nautiloidParams.generation);
  iris::ImageWavetable nautiloidTable;
  check("Nautiloid Iris source converts to wavetable",
        iris::buildWavetableFromSourceField(nautiloidSource, settings, &nautiloidTable, &ioError) &&
        nautiloidTable.valid());

  iris::SourceField multibrotSource;
  iris::SourceField multijuliaSource;
  check("cubic Multibrot generates a source",
        iris::makeBuiltinFractalSourceSized(
          iris::FRACTAL_MULTIBROT, 0.f, 0.0, 0.0,
          64, 32, 1.f, 64, 32, 0, 0, &multibrotSource, &ioError) &&
        multibrotSource.valid());
  check("cubic Multijulia generates a source",
        iris::makeBuiltinFractalSourceSized(
          iris::FRACTAL_MULTIJULIA, 0.f, 0.0, 0.0,
          64, 32, 1.f, 64, 32, 0, 0, &multijuliaSource, &ioError) &&
        multijuliaSource.valid());
  const auto sourceHasVariation = [](const iris::SourceField& source) {
    for (size_t i = 1u; i < source.rgb8.size(); ++i) {
      if (source.rgb8[i] != source.rgb8[0]) return true;
    }
    return false;
  };
  check("cubic Multibrot source has visible variation", sourceHasVariation(multibrotSource));
  check("cubic Multijulia source has visible variation", sourceHasVariation(multijuliaSource));
  size_t multijuliaInteriorPixels = 0u;
  for (size_t i = 0u; i + 2u < multijuliaSource.rgb8.size(); i += 3u) {
    if (multijuliaSource.rgb8[i] == 7u &&
        multijuliaSource.rgb8[i + 1u] == 4u &&
        multijuliaSource.rgb8[i + 2u] == 18u) {
      ++multijuliaInteriorPixels;
    }
  }
  check("cubic Multijulia default exposes boundary detail",
        multijuliaInteriorPixels * 4u < size_t(multijuliaSource.width * multijuliaSource.height));
  check("Multibrot and Multijulia sources are distinct",
        multibrotSource.rgb8 != multijuliaSource.rgb8);

  std::cout << "Summary: " << (checks - failures) << "/" << checks << " passed\n";
  return failures == 0 ? 0 : 1;
}
