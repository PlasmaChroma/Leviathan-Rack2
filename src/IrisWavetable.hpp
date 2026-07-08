#pragma once

#include "IrisSourceField.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace iris {

constexpr int kFrameSize = 1024;
constexpr int kDefaultRows = 256;
constexpr int kMaxRows = 256;
static_assert(kCanonicalSourceWidth == kFrameSize, "Iris source width must match frame size");
static_assert(kCanonicalSourceHeight == kDefaultRows, "Iris source height must match default rows");
constexpr int kSourcePreviewWidth = 256;
constexpr int kSourcePreviewHeight = 128;

enum NormalizeMode {
  NORMALIZE_NONE = 0,
  NORMALIZE_GLOBAL = 1,
  NORMALIZE_PER_ROW = 2,
  NORMALIZE_BALANCED = 3,
};

enum RowOrder {
  ROW_TOP_TO_BOTTOM = 0,
  ROW_BOTTOM_TO_TOP = 1,
};

enum TrimMode {
  TRIM_OFF = 0,
  TRIM_GENTLE = 1,
  TRIM_MEDIUM = 2,
  TRIM_AGGRESSIVE = 3,
};

enum ImageChannelMode {
  IMAGE_CHANNEL_ALL = 0,
  IMAGE_CHANNEL_RED = 1,
  IMAGE_CHANNEL_GREEN = 2,
  IMAGE_CHANNEL_BLUE = 3,
};

enum SourceKind {
  SOURCE_IMAGE = 0,
  SOURCE_BUILTIN_FRACTAL = 1,
};

enum BuiltinFractalMode {
  FRACTAL_NONE = 0,
  FRACTAL_MANDELBROT = 1,
  FRACTAL_MANDELBROT_SEAHORSE = 2,
  FRACTAL_MANDELBROT_SPIRAL = 3,
  FRACTAL_JULIA = 4,
  FRACTAL_PHOENIX_JULIA = 5,
  FRACTAL_BURNING_SHIP = 7,
  FRACTAL_CELTIC = 8,
  FRACTAL_BUFFALO = 9,
  FRACTAL_TRICORN = 10,
  FRACTAL_SPIDER = 11,
  FRACTAL_NEWTON = 12,
  FRACTAL_NOVA = 13,
};

constexpr int kBuiltinFractalVersion = 1;
constexpr int kFirstBuiltinFractalMode = FRACTAL_MANDELBROT;
constexpr int kLastBuiltinFractalMode = FRACTAL_NOVA;

inline bool isBuiltinFractalMode(int mode) {
  switch (mode) {
    case FRACTAL_MANDELBROT:
    case FRACTAL_MANDELBROT_SEAHORSE:
    case FRACTAL_MANDELBROT_SPIRAL:
    case FRACTAL_JULIA:
    case FRACTAL_PHOENIX_JULIA:
    case FRACTAL_BURNING_SHIP:
    case FRACTAL_CELTIC:
    case FRACTAL_BUFFALO:
    case FRACTAL_TRICORN:
    case FRACTAL_SPIDER:
    case FRACTAL_NEWTON:
    case FRACTAL_NOVA:
      return true;
    default:
      return false;
  }
}

inline const char* builtinFractalName(int mode) {
  switch (mode) {
    case FRACTAL_MANDELBROT: return "Mandelbrot";
    case FRACTAL_MANDELBROT_SEAHORSE: return "Mandelbrot Seahorse";
    case FRACTAL_MANDELBROT_SPIRAL: return "Mandelbrot Spiral";
    case FRACTAL_JULIA: return "Julia Lattice";
    case FRACTAL_PHOENIX_JULIA: return "Phoenix Julia";
    case FRACTAL_BURNING_SHIP: return "Burning Ship";
    case FRACTAL_CELTIC: return "Celtic";
    case FRACTAL_BUFFALO: return "Buffalo";
    case FRACTAL_TRICORN: return "Tricorn";
    case FRACTAL_SPIDER: return "Spider";
    case FRACTAL_NEWTON: return "Newton Roots";
    case FRACTAL_NOVA: return "Nova";
    case FRACTAL_NONE:
    default: return "Image";
  }
}

struct ConversionSettings {
  int frameSize = kFrameSize;
  int rows = kDefaultRows;
  NormalizeMode normalizeMode = NORMALIZE_BALANCED;
  RowOrder rowOrder = ROW_TOP_TO_BOTTOM;
  TrimMode trimMode = TRIM_OFF;
  ImageChannelMode imageChannelMode = IMAGE_CHANNEL_ALL;
  float seamSmoothing = 0.f;
  float waveSmoothing = 0.f;
  bool dcRemove = false;
  bool invert = false;
  float contrast = 1.f;
  float brightness = 0.f;
  float gamma = 1.f;
};

struct ImageWavetable {
  int frameSize = kFrameSize;
  int rowCount = 0;
  int stride = kFrameSize + 1;
  std::vector<float> samples;
  std::string sourcePath;
  std::string sourceName;
  int sourceWidth = 0;
  int sourceHeight = 0;
  int sourceChannels = 0;
  int sourcePreviewWidth = 0;
  int sourcePreviewHeight = 0;
  std::vector<uint8_t> sourcePreviewRgb;
  float globalMin = 0.f;
  float globalMax = 0.f;
  float globalRms = 0.f;

  bool valid() const {
    return frameSize >= 2 && rowCount >= 1 && stride >= frameSize + 1 &&
           samples.size() == size_t(rowCount) * size_t(stride);
  }

  float sample(float phase, float scan) const {
    if (!valid()) {
      return 0.f;
    }
    if (!std::isfinite(phase)) phase = 0.f;
    if (!std::isfinite(scan)) scan = 0.f;
    if (phase < 0.f || phase >= 1.f) {
      phase -= std::floor(phase);
    }
    scan = std::max(0.f, std::min(scan, 1.f));
    const float x = phase * float(frameSize);
    const int x0Raw = int(x);
    float xFrac = x - float(x0Raw);
    int x0 = x0Raw;
    // Rare rounding guard for phase values that land exactly on the wrap sample.
    if (x0 >= frameSize) {
      x0 = 0;
      xFrac = 0.f;
    }
    const float rowPos = scan * float(rowCount - 1);
    const int row0 = int(rowPos);
    const int row1 = std::min(row0 + 1, rowCount - 1);
    const float rowFrac = rowPos - float(row0);
    const size_t base0 = size_t(row0) * size_t(stride) + size_t(x0);
    const size_t base1 = size_t(row1) * size_t(stride) + size_t(x0);
    const float a = samples[base0] + (samples[base0 + 1u] - samples[base0]) * xFrac;
    const float b = samples[base1] + (samples[base1 + 1u] - samples[base1]) * xFrac;
    const float value = a + (b - a) * rowFrac;
    return std::isfinite(value) ? value : 0.f;
  }
};

struct WavetableOscillator {
  float phase = 0.f;

  void reset(float newPhase = 0.f) {
    if (!std::isfinite(newPhase)) newPhase = 0.f;
    phase = newPhase - std::floor(newPhase);
  }

  void softSync() {
    if (!std::isfinite(phase)) {
      phase = 0.f;
      return;
    }
    phase = 1.f - phase;
    phase -= std::floor(phase);
  }

  float process(const ImageWavetable& table, float frequency, float sampleTime, float scan) {
    if (!std::isfinite(phase)) phase = 0.f;
    if (!std::isfinite(frequency) || frequency < 0.f) frequency = 0.f;
    if (!std::isfinite(sampleTime) || sampleTime < 0.f) sampleTime = 0.f;
    const float out = table.sample(phase, scan);
    phase += frequency * sampleTime;
    if (!std::isfinite(phase)) phase = 0.f;
    if (phase < 0.f || phase >= 1.f) {
      phase -= std::floor(phase);
    }
    return std::isfinite(out) ? out : 0.f;
  }
};

inline float clamp01(float x) {
  if (!std::isfinite(x)) return 0.f;
  return std::max(0.f, std::min(x, 1.f));
}

inline float trimThreshold(TrimMode mode) {
  switch (mode) {
    case TRIM_GENTLE: return 0.015f;
    case TRIM_MEDIUM: return 0.04f;
    case TRIM_AGGRESSIVE: return 0.10f;
    default: return 0.f;
  }
}

inline void smoothRowsCyclic(std::vector<std::vector<float> >* rows, float smoothing) {
  if (!rows || rows->empty()) {
    return;
  }
  constexpr int passes = 6;
  const float amount = 0.44f * clamp01(smoothing);
  if (!(amount > 0.f)) {
    return;
  }

  for (size_t row = 0; row < rows->size(); ++row) {
    std::vector<float>& values = (*rows)[row];
    if (values.size() < 3u) {
      continue;
    }
    std::vector<float> scratch(values.size(), 0.f);
    for (int pass = 0; pass < passes; ++pass) {
      const size_t last = values.size() - 1u;
      for (size_t x = 0; x < values.size(); ++x) {
        const float left = values[x == 0u ? last : x - 1u];
        const float right = values[x == last ? 0u : x + 1u];
        const float neighborAverage = 0.5f * (left + right);
        scratch[x] = values[x] + (neighborAverage - values[x]) * amount;
      }
      values.swap(scratch);
    }
  }
}

inline void updateStatistics(ImageWavetable* table) {
  if (!table || !table->valid()) {
    return;
  }
  float minValue = table->samples[0];
  float maxValue = table->samples[0];
  double sumSquares = 0.0;
  size_t count = 0u;
  for (int row = 0; row < table->rowCount; ++row) {
    const size_t base = size_t(row) * size_t(table->stride);
    for (int x = 0; x < table->frameSize; ++x) {
      const float value = table->samples[base + size_t(x)];
      minValue = std::min(minValue, value);
      maxValue = std::max(maxValue, value);
      sumSquares += double(value) * double(value);
      ++count;
    }
  }
  table->globalMin = minValue;
  table->globalMax = maxValue;
  table->globalRms = count ? float(std::sqrt(sumSquares / double(count))) : 0.f;
}

inline void buildSourcePreviewFromSourceField(const SourceField& source, ImageWavetable* table) {
  if (!source.valid() || !table) {
    return;
  }
  const int previewWidth = kSourcePreviewWidth;
  const int previewHeight = kSourcePreviewHeight;
  table->sourcePreviewWidth = previewWidth;
  table->sourcePreviewHeight = previewHeight;
  table->sourcePreviewRgb.assign(size_t(previewWidth * previewHeight * 3), 0u);
  for (int y = 0; y < previewHeight; ++y) {
    const float sourceY = (float(y) + 0.5f) * float(source.height) / float(previewHeight) - 0.5f;
    const int y0 = std::max(0, std::min(int(std::floor(sourceY)), source.height - 1));
    const int y1 = std::min(y0 + 1, source.height - 1);
    const float fy = clamp01(sourceY - float(y0));
    for (int x = 0; x < previewWidth; ++x) {
      const float sourceX =
        (float(x) + 0.5f) * float(source.width) / float(previewWidth) - 0.5f;
      const int x0 = std::max(0, std::min(int(std::floor(sourceX)), source.width - 1));
      const int x1 = std::min(x0 + 1, source.width - 1);
      const float fx = clamp01(sourceX - float(x0));
      const size_t outBase = (size_t(y) * size_t(previewWidth) + size_t(x)) * 3u;
      for (int c = 0; c < 3; ++c) {
        const auto component = [&](int px, int py) {
          const size_t base = (size_t(py) * size_t(source.width) + size_t(px)) * 3u;
          return float(source.rgb8[base + size_t(c)]);
        };
        const float top = component(x0, y0) + (component(x1, y0) - component(x0, y0)) * fx;
        const float bottom = component(x0, y1) + (component(x1, y1) - component(x0, y1)) * fx;
        const float value = std::max(0.f, std::min(top + (bottom - top) * fy, 255.f));
        table->sourcePreviewRgb[outBase + size_t(c)] =
          uint8_t(std::round(value));
      }
    }
  }
}

inline bool buildWavetableFromSourceField(const SourceField& source,
                                          const ConversionSettings& requested,
                                          ImageWavetable* out, std::string* error = nullptr) {
  if (!source.valid() || !out) {
    if (error) *error = "Invalid source field";
    return false;
  }
  ConversionSettings settings = requested;
  settings.frameSize = std::max(2, std::min(settings.frameSize, 16384));
  settings.rows = std::max(2, std::min(settings.rows, kMaxRows));
  settings.contrast = std::isfinite(settings.contrast) ? std::max(settings.contrast, 0.f) : 1.f;
  settings.brightness = std::isfinite(settings.brightness) ? settings.brightness : 0.f;
  settings.gamma = std::isfinite(settings.gamma) ? std::max(settings.gamma, 0.01f) : 1.f;
  settings.seamSmoothing = clamp01(settings.seamSmoothing);
  settings.waveSmoothing = clamp01(settings.waveSmoothing);

  std::vector<std::vector<float> > rows(size_t(settings.rows),
                                        std::vector<float>(size_t(settings.frameSize), 0.f));
  for (int row = 0; row < settings.rows; ++row) {
    const int outputRow = settings.rowOrder == ROW_BOTTOM_TO_TOP ? settings.rows - 1 - row : row;
    const float sourceY = (float(row) + 0.5f) * float(source.height) / float(settings.rows) - 0.5f;
    const int y0 = std::max(0, std::min(int(std::floor(sourceY)), source.height - 1));
    const int y1 = std::min(y0 + 1, source.height - 1);
    const float fy = clamp01(sourceY - float(y0));
    for (int x = 0; x < settings.frameSize; ++x) {
      const float sourceX = (float(x) + 0.5f) * float(source.width) / float(settings.frameSize) - 0.5f;
      const int x0 = std::max(0, std::min(int(std::floor(sourceX)), source.width - 1));
      const int x1 = std::min(x0 + 1, source.width - 1);
      const float fx = clamp01(sourceX - float(x0));
      const auto channelValue = [&](int px, int py) {
        const size_t base = (size_t(py) * size_t(source.width) + size_t(px)) * 3u;
        switch (settings.imageChannelMode) {
          case IMAGE_CHANNEL_RED: return float(source.rgb8[base]) / 255.f;
          case IMAGE_CHANNEL_GREEN: return float(source.rgb8[base + 1u]) / 255.f;
          case IMAGE_CHANNEL_BLUE: return float(source.rgb8[base + 2u]) / 255.f;
          case IMAGE_CHANNEL_ALL:
          default:
            return (0.299f * float(source.rgb8[base]) + 0.587f * float(source.rgb8[base + 1u]) +
                    0.114f * float(source.rgb8[base + 2u])) / 255.f;
        }
      };
      const float top =
        channelValue(x0, y0) + (channelValue(x1, y0) - channelValue(x0, y0)) * fx;
      const float bottom =
        channelValue(x0, y1) + (channelValue(x1, y1) - channelValue(x0, y1)) * fx;
      float gray = top + (bottom - top) * fy;
      gray = clamp01((gray - 0.5f) * settings.contrast + 0.5f + settings.brightness);
      gray = std::pow(gray, settings.gamma);
      rows[size_t(outputRow)][size_t(x)] = (gray * 2.f - 1.f) * (settings.invert ? -1.f : 1.f);
    }
  }

  if (settings.normalizeMode == NORMALIZE_BALANCED) {
    std::vector<float> distribution;
    distribution.reserve(size_t(settings.rows) * size_t(settings.frameSize));
    for (size_t row = 0; row < rows.size(); ++row) {
      distribution.insert(distribution.end(), rows[row].begin(), rows[row].end());
    }
    std::sort(distribution.begin(), distribution.end());
    const auto percentile = [&](float p) {
      const float position = p * float(distribution.size() - 1u);
      const size_t lower = size_t(position);
      const size_t upper = std::min(lower + 1u, distribution.size() - 1u);
      const float fraction = position - float(lower);
      return distribution[lower] + (distribution[upper] - distribution[lower]) * fraction;
    };
    const float low = percentile(0.01f);
    const float midpoint = percentile(0.50f);
    const float high = percentile(0.99f);
    if (high - low > 1e-6f) {
      const float negativeRange = std::max(midpoint - low, 1e-6f);
      const float positiveRange = std::max(high - midpoint, 1e-6f);
      for (size_t row = 0; row < rows.size(); ++row) {
        for (size_t x = 0; x < rows[row].size(); ++x) {
          const float centered = rows[row][x] - midpoint;
          rows[row][x] = centered < 0.f ? centered / negativeRange : centered / positiveRange;
        }
      }
    }
  } else if (settings.normalizeMode == NORMALIZE_GLOBAL) {
    float minValue = rows[0][0];
    float maxValue = rows[0][0];
    for (size_t row = 0; row < rows.size(); ++row) {
      for (size_t x = 0; x < rows[row].size(); ++x) {
        minValue = std::min(minValue, rows[row][x]);
        maxValue = std::max(maxValue, rows[row][x]);
      }
    }
    const float center = 0.5f * (minValue + maxValue);
    const float radius = 0.5f * (maxValue - minValue);
    if (radius > 1e-6f) {
      for (size_t row = 0; row < rows.size(); ++row) {
        for (size_t x = 0; x < rows[row].size(); ++x) {
          rows[row][x] = (rows[row][x] - center) / radius;
        }
      }
    }
  } else if (settings.normalizeMode == NORMALIZE_PER_ROW) {
    for (size_t row = 0; row < rows.size(); ++row) {
      const std::pair<std::vector<float>::iterator, std::vector<float>::iterator> limits =
        std::minmax_element(rows[row].begin(), rows[row].end());
      const float center = 0.5f * (*limits.first + *limits.second);
      const float radius = 0.5f * (*limits.second - *limits.first);
      if (radius > 1e-6f) {
        for (size_t x = 0; x < rows[row].size(); ++x) {
          rows[row][x] = (rows[row][x] - center) / radius;
        }
      }
    }
  }

  if (settings.dcRemove) {
    for (size_t row = 0; row < rows.size(); ++row) {
      double sum = 0.0;
      for (size_t x = 0; x < rows[row].size(); ++x) sum += rows[row][x];
      const float mean = float(sum / double(rows[row].size()));
      for (size_t x = 0; x < rows[row].size(); ++x) rows[row][x] -= mean;
    }
  }

  const float threshold = trimThreshold(settings.trimMode);
  if (threshold > 0.f && rows.size() > 2u) {
    std::vector<std::vector<float> > kept;
    for (size_t row = 0; row < rows.size(); ++row) {
      const std::pair<std::vector<float>::iterator, std::vector<float>::iterator> limits =
        std::minmax_element(rows[row].begin(), rows[row].end());
      if (*limits.second - *limits.first >= threshold) kept.push_back(rows[row]);
    }
    if (kept.size() >= 2u) {
      rows.swap(kept);
    } else {
      std::vector<std::vector<float> > endpoints;
      endpoints.push_back(rows.front());
      endpoints.push_back(rows.back());
      rows.swap(endpoints);
    }
  }

  smoothRowsCyclic(&rows, settings.waveSmoothing);

  const float seamAmount = settings.seamSmoothing;
  const int seam = std::min(settings.frameSize / 2, std::max(8, settings.frameSize / 32));
  if (seamAmount > 0.f) {
    for (size_t row = 0; row < rows.size(); ++row) {
      const float edge = 0.5f * (rows[row].front() + rows[row].back());
      for (int i = 0; i < seam; ++i) {
        const float t = float(i + 1) / float(seam + 1);
        const float blend = seamAmount * (1.f - t);
        rows[row][size_t(i)] += (edge - rows[row][size_t(i)]) * blend;
        const size_t right = size_t(settings.frameSize - 1 - i);
        rows[row][right] += (edge - rows[row][right]) * blend;
      }
    }
  }

  ImageWavetable table;
  table.frameSize = settings.frameSize;
  table.rowCount = int(rows.size());
  table.stride = settings.frameSize + 1;
  table.sourcePath = source.sourcePath;
  table.sourceName = source.sourceName;
  table.sourceWidth = source.originalWidth;
  table.sourceHeight = source.originalHeight;
  table.sourceChannels = source.originalChannels;
  buildSourcePreviewFromSourceField(source, &table);
  table.samples.resize(size_t(table.rowCount) * size_t(table.stride));
  for (int row = 0; row < table.rowCount; ++row) {
    const size_t base = size_t(row) * size_t(table.stride);
    for (int x = 0; x < table.frameSize; ++x) {
      table.samples[base + size_t(x)] =
        std::max(-1.f, std::min(rows[size_t(row)][size_t(x)], 1.f));
    }
    table.samples[base + size_t(table.frameSize)] = table.samples[base];
  }
  updateStatistics(&table);
  *out = std::move(table);
  return true;
}

inline bool buildWavetableFromRgba(const uint8_t* rgba, int sourceWidth, int sourceHeight,
                                   int sourceChannels, const ConversionSettings& requested,
                                   ImageWavetable* out, std::string* error = nullptr) {
  SourceField source;
  if (!buildSourceFieldFromRgba8(rgba, sourceWidth, sourceHeight, sourceChannels, &source, error)) {
    return false;
  }
  return buildWavetableFromSourceField(source, requested, out, error);
}

inline ImageWavetable makeDefaultTable() {
  ImageWavetable table;
  table.rowCount = kDefaultRows;
  table.samples.resize(size_t(table.rowCount) * size_t(table.stride));
  for (int row = 0; row < table.rowCount; ++row) {
    const float scan = float(row) / float(table.rowCount - 1);
    const size_t base = size_t(row) * size_t(table.stride);
    for (int x = 0; x < table.frameSize; ++x) {
      const float phase = float(x) / float(table.frameSize);
      const float sine = std::sin(6.28318530717958647692f * phase);
      const float triangle =
        phase < 0.25f ? 4.f * phase :
        phase < 0.75f ? 2.f - 4.f * phase :
                        4.f * phase - 4.f;
      const float saw = phase < 0.5f ? 2.f * phase : 2.f * phase - 2.f;
      const float square = phase < 0.5f ? 1.f : -1.f;
      if (scan <= 1.f / 3.f) {
        table.samples[base + size_t(x)] = sine + (triangle - sine) * (scan * 3.f);
      } else if (scan <= 2.f / 3.f) {
        table.samples[base + size_t(x)] =
          triangle + (saw - triangle) * ((scan - 1.f / 3.f) * 3.f);
      } else {
        table.samples[base + size_t(x)] =
          saw + (square - saw) * ((scan - 2.f / 3.f) * 3.f);
      }
    }
    table.samples[base + size_t(table.frameSize)] = table.samples[base];
  }
  updateStatistics(&table);
  table.sourceName = "Sine / Triangle / Saw / Square";
  return table;
}

} // namespace iris
