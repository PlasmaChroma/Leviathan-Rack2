#pragma once

#include "IrisSourceField.hpp"
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <jansson.h>

template<typename T>
inline T clampVal(T val, T minVal, T maxVal) {
    return (val < minVal) ? minVal : ((val > maxVal) ? maxVal : val);
}

struct RectI {
    int minX = 0;
    int minY = 0;
    int maxX = -1;
    int maxY = -1;

    RectI() = default;
    RectI(int minX, int minY, int maxX, int maxY) : minX(minX), minY(minY), maxX(maxX), maxY(maxY) {}

    bool operator==(const RectI& o) const {
        return minX == o.minX && minY == o.minY && maxX == o.maxX && maxY == o.maxY;
    }
    bool operator!=(const RectI& o) const {
        return !(*this == o);
    }

    void reset() {
        minX = 100000;
        minY = 100000;
        maxX = -1;
        maxY = -1;
    }

    void expand(int x, int y) {
        minX = std::min(minX, x);
        minY = std::min(minY, y);
        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);
    }

    void expand(const RectI& other) {
        if (!other.valid()) return;
        minX = std::min(minX, other.minX);
        minY = std::min(minY, other.minY);
        maxX = std::max(maxX, other.maxX);
        maxY = std::max(maxY, other.maxY);
    }

    int width() const { return valid() ? (maxX - minX + 1) : 0; }
    int height() const { return valid() ? (maxY - minY + 1) : 0; }
    bool valid() const { return maxX >= minX && maxY >= minY; }
};

struct ChromatideColor {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;

    ChromatideColor() = default;
    ChromatideColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    bool operator==(const ChromatideColor& o) const {
        return r == o.r && g == o.g && b == o.b;
    }
    bool operator!=(const ChromatideColor& o) const {
        return !(*this == o);
    }
};


enum class ChromatideTool {
    Brush = 0,
    Eraser = 1,
    Eyedropper = 2
};

struct ChromatideBrushState {
    float size = 24.0f;       // Native vertical diameter in raster pixels (1.0 to 128.0)
    float opacity = 1.0f;     // Blend opacity (0.0 to 1.0)
    ChromatideColor foreground {255, 255, 255};
    ChromatideColor background {0, 0, 0};
    ChromatideTool tool = ChromatideTool::Brush;
};

struct ChromatideUndoRecord {
    RectI bounds;
    std::vector<uint8_t> beforeRgb;
    std::vector<uint8_t> afterRgb;

    size_t memoryUsage() const {
        return beforeRgb.size() + afterRgb.size() + sizeof(*this);
    }
};

class ChromatideCanvas {
public:
    static constexpr int WIDTH = iris::kCanonicalSourceWidth;     // 1024
    static constexpr int HEIGHT = iris::kCanonicalSourceHeight;   // 256
    static constexpr int CHANNELS = iris::kCanonicalSourceChannels; // 3
    static constexpr size_t BUFFER_SIZE = static_cast<size_t>(WIDTH * HEIGHT * CHANNELS); // 786,432

    static constexpr float VIEWPORT_ASPECT_RATIO = 98.0f / 65.27f; // Approx 1.5014555f

    std::array<uint8_t, BUFFER_SIZE> pixels {};
    uint64_t revision = 0;

    ChromatideCanvas();

    void clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, RectI* dirtyOut = nullptr);
    void sample(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const;
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

    inline static size_t pixelOffset(int x, int y) {
        return (static_cast<size_t>(y) * static_cast<size_t>(WIDTH) + static_cast<size_t>(x)) * static_cast<size_t>(CHANNELS);
    }

    // Bidirectional coordinate transform
    static void normalizedToRaster(float u, float v, float& rx, float& ry);
    static void rasterToNormalized(float rx, float ry, float& u, float& v);

    // Rasterization
    void stampAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush, RectI* dirtyOut = nullptr);
    void strokeToNormalized(float uPrev, float vPrev, float uCurr, float vCurr, const ChromatideBrushState& brush, RectI* dirtyOut = nullptr);

    // Undo / Redo history management
    void beginStrokeTransaction(const RectI& initialBounds, ChromatideUndoRecord& recordOut) const;
    void finalizeStrokeTransaction(const RectI& dirtyBounds, ChromatideUndoRecord& recordInOut);
    void applyUndoRecord(const ChromatideUndoRecord& record, bool isUndo);

    // Serialization
    std::string serializeQoiBase64() const;
    bool deserializeQoiBase64(const std::string& b64Data);

    static std::string base64Encode(const uint8_t* data, size_t length);
    static bool base64Decode(const std::string& input, std::vector<uint8_t>& output);
};
