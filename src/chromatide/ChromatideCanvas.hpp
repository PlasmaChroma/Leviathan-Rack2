#pragma once

#include "../common/fractal/SourceField.hpp"
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
    Eyedropper = 2,
    Smudge = 3   // append only: saved in patches and TOOL_PARAM
};

// Dab footprint. Append only: the index is saved in patches.
enum class ChromatideBrushHead {
    Round = 0,   // circular dab
    Square = 1,  // square dab
    Chisel = 2,  // narrow 45-degree calligraphy nib
    Spray = 3,   // round speckle; density follows the falloff
    Count
};

inline const char* chromatideBrushHeadName(ChromatideBrushHead head) {
    switch (head) {
        case ChromatideBrushHead::Square: return "Square";
        case ChromatideBrushHead::Chisel: return "Chisel";
        case ChromatideBrushHead::Spray: return "Spray";
        default: return "Round";
    }
}

struct ChromatideBrushState {
    float size = 24.0f;       // Native vertical diameter in raster pixels (1.0 to 128.0)
    float opacity = 1.0f;     // Stroke ceiling: one stroke never exceeds this, however often dabs overlap
    float flow = 0.35f;       // Per-dab buildup into the stroke mask (Krita-style flow)
    float hardness = 0.6f;    // Fraction of the radius painted at full dab strength before falloff
    ChromatideColor foreground {255, 255, 255};
    ChromatideColor background {0, 0, 0};
    ChromatideTool tool = ChromatideTool::Brush;
    ChromatideBrushHead head = ChromatideBrushHead::Round;
    float angle = 0.7853982f;  // Chisel nib rotation, radians (0..pi)
    float thickness = 0.28f;   // Chisel nib width as a fraction of its length
    float density = 0.35f;     // Spray speckle fraction at full coverage
    float jitter = 0.0f;       // Per-dab scatter of position and size along a stroke (0..1)
    int sampleRadius = 0;      // Eyedropper averaging radius in raster pixels
    float smudgeLength = 0.6f; // Smudge: how far carried colour persists along a stroke (0..1)
};

// Per-tool knob memory. The four shared knobs (slots) mean different things
// for each profile; Size is universal and not part of a profile.
// Append only: the index is saved in patches.
enum class ChromatideKnobProfile {
    Round = 0, Square, Chisel, Spray, Eraser, Pick, Smudge, Count
};
constexpr int kChromatideKnobSlots = 4;  // Flow, Opacity, Hardness, Jitter params

inline ChromatideKnobProfile chromatideKnobProfileFor(ChromatideTool tool, ChromatideBrushHead head) {
    if (tool == ChromatideTool::Eraser) return ChromatideKnobProfile::Eraser;
    if (tool == ChromatideTool::Eyedropper) return ChromatideKnobProfile::Pick;
    if (tool == ChromatideTool::Smudge) return ChromatideKnobProfile::Smudge;
    switch (head) {
        case ChromatideBrushHead::Square: return ChromatideKnobProfile::Square;
        case ChromatideBrushHead::Chisel: return ChromatideKnobProfile::Chisel;
        case ChromatideBrushHead::Spray: return ChromatideKnobProfile::Spray;
        default: return ChromatideKnobProfile::Round;
    }
}

// Panel label for a slot, or nullptr when the slot is unused by the profile.
inline const char* chromatideKnobSlotLabel(ChromatideKnobProfile profile, int slot) {
    static const char* const labels[int(ChromatideKnobProfile::Count)][kChromatideKnobSlots] = {
        {"FLOW", "OPACITY", "HARDNESS", "JITTER"},   // Round
        {"FLOW", "OPACITY", "HARDNESS", "JITTER"},   // Square
        {"FLOW", "OPACITY", "ANGLE", "THICKNESS"},   // Chisel
        {"DENSITY", "OPACITY", "FALLOFF", "JITTER"}, // Spray
        {"FLOW", "STRENGTH", "SOFTNESS", "JITTER"},  // Eraser
        {"AREA", nullptr, nullptr, nullptr},         // Pick
        {"STRENGTH", "LENGTH", "SOFTNESS", "JITTER"}, // Smudge
    };
    const int p = int(profile);
    if (p < 0 || p >= int(ChromatideKnobProfile::Count) || slot < 0 || slot >= kChromatideKnobSlots) return nullptr;
    return labels[p][slot];
}

// Raw slot values for a fresh profile, in param units (slot 0 is 0.01..1).
inline void chromatideKnobProfileDefaults(ChromatideKnobProfile profile, float out[kChromatideKnobSlots]) {
    static const float defaults[int(ChromatideKnobProfile::Count)][kChromatideKnobSlots] = {
        {0.35f, 1.0f, 0.6f, 0.0f},  // Round
        {0.35f, 1.0f, 0.6f, 0.0f},  // Square
        {0.5f, 1.0f, 0.25f, 0.217f}, // Chisel: 45 degrees, thickness 0.28
        {0.35f, 1.0f, 0.4f, 0.2f},  // Spray
        {0.5f, 1.0f, 0.4f, 0.0f},   // Eraser
        {0.01f, 1.0f, 0.6f, 0.0f},  // Pick: single pixel
        {0.5f, 0.6f, 0.5f, 0.0f},   // Smudge
    };
    const int p = std::max(0, std::min(int(profile), int(ChromatideKnobProfile::Count) - 1));
    for (int i = 0; i < kChromatideKnobSlots; ++i) out[i] = defaults[p][i];
}

// Krita-style indirect painting: dabs accumulate into a coverage mask with
// flow, and the canvas is recomposited from the pre-stroke base capped at opacity.
struct ChromatideStrokeBuffer {
    std::vector<uint8_t> base;
    std::vector<float> mask;
    bool active = false;
    // Smudge: colour carried by the brush, in dab-local pixels centred on the dab.
    std::vector<float> carry;
    int carryHalfW = 0, carryHalfH = 0;
    bool carryValid = false;
    // Where the last dab landed (normalised). A new dab lands only once the
    // cursor is a full spacing step away from it, so repeated events at one
    // spot and hand tremor no longer stack dabs.
    float lastDabU = 0.0f, lastDabV = 0.0f;
    bool hasLastDab = false;
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
    static constexpr int WIDTH = fractal::kCanonicalSourceWidth;     // 1024
    static constexpr int HEIGHT = fractal::kCanonicalSourceHeight;   // 256
    static constexpr int CHANNELS = fractal::kCanonicalSourceChannels; // 3
    static constexpr size_t BUFFER_SIZE = static_cast<size_t>(WIDTH * HEIGHT * CHANNELS); // 786,432

    static constexpr float VIEWPORT_ASPECT_RATIO = 98.0f / 65.27f; // Approx 1.5014555f

    std::array<uint8_t, BUFFER_SIZE> pixels {};
    uint64_t revision = 0;
    uint32_t dabSeed = 0;     // varies Spray speckle per dab; not saved
    uint32_t jitterSeed = 0;  // varies stroke jitter per dab; not saved

    ChromatideCanvas();

    void clear(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, RectI* dirtyOut = nullptr);
    void sample(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const;
    void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    // Mean color over a (2r+1)^2 raster square, clipped to the canvas.
    void sampleAverage(int x, int y, int radius, uint8_t& r, uint8_t& g, uint8_t& b) const;

    inline static size_t pixelOffset(int x, int y) {
        return (static_cast<size_t>(y) * static_cast<size_t>(WIDTH) + static_cast<size_t>(x)) * static_cast<size_t>(CHANNELS);
    }

    // Bidirectional coordinate transform
    static void normalizedToRaster(float u, float v, float& rx, float& ry);
    static void rasterToNormalized(float rx, float ry, float& u, float& v);

    // Rasterization
    // Without an active stroke buffer a dab blends directly (flow x opacity).
    void beginStrokeBuffer(ChromatideStrokeBuffer& buffer) const;
    void stampAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush, RectI* dirtyOut = nullptr,
        ChromatideStrokeBuffer* stroke = nullptr);
    // Drags carried colour into the canvas and picks up what is underneath.
    // The first dab of a stroke only loads the carry buffer.
    void smudgeAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush, RectI* dirtyOut,
        ChromatideStrokeBuffer& stroke);
    void strokeToNormalized(float uPrev, float vPrev, float uCurr, float vCurr, const ChromatideBrushState& brush,
        RectI* dirtyOut = nullptr, ChromatideStrokeBuffer* stroke = nullptr);

    // Undo / Redo history management
    // Capture before painting; finalization crops this temporary full snapshot.
    void beginStrokeTransaction(ChromatideUndoRecord& recordOut) const;
    void finalizeStrokeTransaction(const RectI& dirtyBounds, ChromatideUndoRecord& recordInOut);
    void applyUndoRecord(const ChromatideUndoRecord& record, bool isUndo);

    // Serialization
    std::string serializeQoiBase64() const;
    bool deserializeQoiBase64(const std::string& b64Data);

    static std::string base64Encode(const uint8_t* data, size_t length);
    static bool base64Decode(const std::string& input, std::vector<uint8_t>& output);
};
