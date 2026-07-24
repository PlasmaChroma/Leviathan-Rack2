#include "ChromatideCanvas.hpp"
#include "third_party/qoi.h"
#include <cstring>
#include <cmath>
#include <algorithm>

ChromatideCanvas::ChromatideCanvas() {
    clear(0, 0, 0, nullptr);
}

void ChromatideCanvas::clear(uint8_t r, uint8_t g, uint8_t b, RectI* dirtyOut) {
    for (size_t i = 0; i < BUFFER_SIZE; i += CHANNELS) {
        pixels[i + 0] = r;
        pixels[i + 1] = g;
        pixels[i + 2] = b;
    }
    revision++;
    if (dirtyOut) {
        dirtyOut->minX = 0;
        dirtyOut->minY = 0;
        dirtyOut->maxX = WIDTH - 1;
        dirtyOut->maxY = HEIGHT - 1;
    }
}

void ChromatideCanvas::sample(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b) const {
    int cx = clampVal(x, 0, WIDTH - 1);
    int cy = clampVal(y, 0, HEIGHT - 1);
    size_t off = pixelOffset(cx, cy);
    r = pixels[off + 0];
    g = pixels[off + 1];
    b = pixels[off + 2];
}

void ChromatideCanvas::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    size_t off = pixelOffset(x, y);
    pixels[off + 0] = r;
    pixels[off + 1] = g;
    pixels[off + 2] = b;
}

void ChromatideCanvas::normalizedToRaster(float u, float v, float& rx, float& ry) {
    float uClamped = clampVal(u, 0.0f, 1.0f);
    float vClamped = clampVal(v, 0.0f, 1.0f);
    rx = uClamped * static_cast<float>(WIDTH - 1);
    ry = vClamped * static_cast<float>(HEIGHT - 1);
}

void ChromatideCanvas::rasterToNormalized(float rx, float ry, float& u, float& v) {
    u = clampVal(rx / static_cast<float>(WIDTH - 1), 0.0f, 1.0f);
    v = clampVal(ry / static_cast<float>(HEIGHT - 1), 0.0f, 1.0f);
}

void ChromatideCanvas::stampAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush, RectI* dirtyOut) {
    float size = clampVal(brush.size, 1.0f, 128.0f);
    float Ry = size * 0.5f;
    float Rx = (size * 0.5f) * (4.0f / VIEWPORT_ASPECT_RATIO);

    int minX = clampVal(static_cast<int>(std::floor(centerRx - Rx - 2.0f)), 0, WIDTH - 1);
    int maxX = clampVal(static_cast<int>(std::ceil(centerRx + Rx + 2.0f)), 0, WIDTH - 1);
    int minY = clampVal(static_cast<int>(std::floor(centerRy - Ry - 2.0f)), 0, HEIGHT - 1);
    int maxY = clampVal(static_cast<int>(std::ceil(centerRy + Ry + 2.0f)), 0, HEIGHT - 1);

    uint8_t srcR = (brush.tool == ChromatideTool::Eraser) ? brush.background.r : brush.foreground.r;
    uint8_t srcG = (brush.tool == ChromatideTool::Eraser) ? brush.background.g : brush.foreground.g;
    uint8_t srcB = (brush.tool == ChromatideTool::Eraser) ? brush.background.b : brush.foreground.b;

    float opacity = clampVal(brush.opacity, 0.0f, 1.0f);
    float edgeWidth = 1.0f / std::max(Ry, 0.5f);

    for (int y = minY; y <= maxY; ++y) {
        float dy = (static_cast<float>(y) - centerRy) / Ry;
        for (int x = minX; x <= maxX; ++x) {
            float dx = (static_cast<float>(x) - centerRx) / Rx;
            float dNorm = std::sqrt(dx * dx + dy * dy);

            if (dNorm >= 1.0f + edgeWidth) continue;

            float cov = 1.0f - clampVal((dNorm - (1.0f - edgeWidth)) / (2.0f * edgeWidth), 0.0f, 1.0f);
            cov = cov * cov * (3.0f - 2.0f * cov); // Smoothstep
            float alpha = opacity * cov;

            if (alpha <= 0.001f) continue;

            size_t off = pixelOffset(x, y);
            float dstR = static_cast<float>(pixels[off + 0]);
            float dstG = static_cast<float>(pixels[off + 1]);
            float dstB = static_cast<float>(pixels[off + 2]);

            pixels[off + 0] = static_cast<uint8_t>(clampVal(std::round(dstR + alpha * (static_cast<float>(srcR) - dstR)), 0.0f, 255.0f));
            pixels[off + 1] = static_cast<uint8_t>(clampVal(std::round(dstG + alpha * (static_cast<float>(srcG) - dstG)), 0.0f, 255.0f));
            pixels[off + 2] = static_cast<uint8_t>(clampVal(std::round(dstB + alpha * (static_cast<float>(srcB) - dstB)), 0.0f, 255.0f));


            if (dirtyOut) {
                dirtyOut->expand(x, y);
            }
        }
    }
    revision++;
}


void ChromatideCanvas::strokeToNormalized(float uPrev, float vPrev, float uCurr, float vCurr, const ChromatideBrushState& brush, RectI* dirtyOut) {
    float viewportBrushDiameter = brush.size / static_cast<float>(HEIGHT);
    float stepSizeNorm = 0.20f * viewportBrushDiameter;
    stepSizeNorm = std::max(stepSizeNorm, 0.001f);

    float distanceNorm = std::hypot(uCurr - uPrev, (vCurr - vPrev) / VIEWPORT_ASPECT_RATIO);
    int numSteps = std::max(1, static_cast<int>(std::ceil(distanceNorm / stepSizeNorm)));

    for (int i = 1; i <= numSteps; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(numSteps);
        float u = uPrev + t * (uCurr - uPrev);
        float v = vPrev + t * (vCurr - vPrev);
        float rx = 0.0f, ry = 0.0f;
        normalizedToRaster(u, v, rx, ry);
        stampAtRaster(rx, ry, brush, dirtyOut);
    }
}

void ChromatideCanvas::beginStrokeTransaction(const RectI& initialBounds, ChromatideUndoRecord& recordOut) const {
    recordOut.bounds = initialBounds;
    recordOut.beforeRgb.clear();
    recordOut.afterRgb.clear();

    if (!initialBounds.valid()) return;

    size_t count = static_cast<size_t>(initialBounds.width() * initialBounds.height() * CHANNELS);
    recordOut.beforeRgb.resize(count);

    size_t idx = 0;
    for (int y = initialBounds.minY; y <= initialBounds.maxY; ++y) {
        for (int x = initialBounds.minX; x <= initialBounds.maxX; ++x) {
            size_t off = pixelOffset(x, y);
            recordOut.beforeRgb[idx + 0] = pixels[off + 0];
            recordOut.beforeRgb[idx + 1] = pixels[off + 1];
            recordOut.beforeRgb[idx + 2] = pixels[off + 2];
            idx += CHANNELS;
        }
    }
}

void ChromatideCanvas::finalizeStrokeTransaction(const RectI& dirtyBounds, ChromatideUndoRecord& recordInOut) {
    if (!dirtyBounds.valid()) {
        recordInOut.bounds = dirtyBounds;
        recordInOut.beforeRgb.clear();
        recordInOut.afterRgb.clear();
        return;
    }

    // Capture before pixels if recordInOut was initialized empty
    if (recordInOut.beforeRgb.empty() || recordInOut.bounds != dirtyBounds) {
        recordInOut.bounds = dirtyBounds;
        size_t count = static_cast<size_t>(dirtyBounds.width() * dirtyBounds.height() * CHANNELS);
        recordInOut.beforeRgb.resize(count);
        // Note: caller should have initiated beforeRgb prior to stamp applications if bounds were unknown.
    }

    size_t count = static_cast<size_t>(dirtyBounds.width() * dirtyBounds.height() * CHANNELS);
    recordInOut.afterRgb.resize(count);

    size_t idx = 0;
    for (int y = dirtyBounds.minY; y <= dirtyBounds.maxY; ++y) {
        for (int x = dirtyBounds.minX; x <= dirtyBounds.maxX; ++x) {
            size_t off = pixelOffset(x, y);
            recordInOut.afterRgb[idx + 0] = pixels[off + 0];
            recordInOut.afterRgb[idx + 1] = pixels[off + 1];
            recordInOut.afterRgb[idx + 2] = pixels[off + 2];
            idx += CHANNELS;
        }
    }
}

void ChromatideCanvas::applyUndoRecord(const ChromatideUndoRecord& record, bool isUndo) {
    if (!record.bounds.valid()) return;

    const auto& src = isUndo ? record.beforeRgb : record.afterRgb;
    if (src.size() < static_cast<size_t>(record.bounds.width() * record.bounds.height() * CHANNELS)) {
        return;
    }

    size_t idx = 0;
    for (int y = record.bounds.minY; y <= record.bounds.maxY; ++y) {
        for (int x = record.bounds.minX; x <= record.bounds.maxX; ++x) {
            size_t off = pixelOffset(x, y);
            pixels[off + 0] = src[idx + 0];
            pixels[off + 1] = src[idx + 1];
            pixels[off + 2] = src[idx + 2];
            idx += CHANNELS;
        }
    }
    revision++;
}

// Base64 Codec
static const char kBase64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string ChromatideCanvas::base64Encode(const uint8_t* data, size_t length) {
    std::string ret;
    ret.reserve(((length + 2) / 3) * 4);
    for (size_t i = 0; i < length; i += 3) {
        uint32_t b0 = data[i];
        uint32_t b1 = (i + 1 < length) ? data[i + 1] : 0;
        uint32_t b2 = (i + 2 < length) ? data[i + 2] : 0;
        uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

        ret.push_back(kBase64Chars[(triple >> 18) & 0x3F]);
        ret.push_back(kBase64Chars[(triple >> 12) & 0x3F]);
        ret.push_back((i + 1 < length) ? kBase64Chars[(triple >> 6) & 0x3F] : '=');
        ret.push_back((i + 2 < length) ? kBase64Chars[triple & 0x3F] : '=');
    }
    return ret;
}

static inline int base64CharVal(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

bool ChromatideCanvas::base64Decode(const std::string& input, std::vector<uint8_t>& output) {
    output.clear();
    if (input.empty()) return true;

    output.reserve((input.size() / 4) * 3);
    uint32_t val = 0;
    int valb = -8;
    for (char c : input) {
        if (c == '=' || c == '\r' || c == '\n' || c == ' ') continue;
        int v = base64CharVal(c);
        if (v < 0) return false;
        val = (val << 6) | v;
        valb += 6;
        if (valb >= 0) {
            output.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return true;
}


std::string ChromatideCanvas::serializeQoiBase64() const {
    qoi_desc desc {};
    desc.width = WIDTH;
    desc.height = HEIGHT;
    desc.channels = CHANNELS;
    desc.colorspace = QOI_SRGB;

    int outLen = 0;
    void* qoiData = qoi_encode(pixels.data(), &desc, &outLen);
    if (!qoiData || outLen <= 0) return "";

    std::string b64 = base64Encode(static_cast<const uint8_t*>(qoiData), static_cast<size_t>(outLen));
    std::free(qoiData);
    return b64;
}

bool ChromatideCanvas::deserializeQoiBase64(const std::string& b64Data) {
    std::vector<uint8_t> qoiBytes;
    if (!base64Decode(b64Data, qoiBytes) || qoiBytes.empty()) {
        clear(0, 0, 0, nullptr);
        return false;
    }

    qoi_desc desc {};
    void* rawPixels = qoi_decode(qoiBytes.data(), static_cast<int>(qoiBytes.size()), &desc, CHANNELS);
    if (!rawPixels) {
        clear(0, 0, 0, nullptr);
        return false;
    }

    if (desc.width != WIDTH || desc.height != HEIGHT || desc.channels != CHANNELS) {
        std::free(rawPixels);
        clear(0, 0, 0, nullptr);
        return false;
    }

    std::memcpy(pixels.data(), rawPixels, BUFFER_SIZE);
    std::free(rawPixels);
    revision++;
    return true;
}
