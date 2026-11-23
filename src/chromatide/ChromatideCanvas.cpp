#include "ChromatideCanvas.hpp"
#include "../third_party/qoi.h"
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

void ChromatideCanvas::sampleAverage(int x, int y, int radius, uint8_t& r, uint8_t& g, uint8_t& b) const {
    radius = clampVal(radius, 0, 16);
    uint32_t sr = 0, sg = 0, sb = 0, n = 0;
    for (int yy = std::max(0, y - radius); yy <= std::min(HEIGHT - 1, y + radius); ++yy) {
        for (int xx = std::max(0, x - radius); xx <= std::min(WIDTH - 1, x + radius); ++xx) {
            const size_t off = pixelOffset(xx, yy);
            sr += pixels[off]; sg += pixels[off + 1]; sb += pixels[off + 2]; ++n;
        }
    }
    if (n == 0) { r = g = b = 0; return; }
    r = static_cast<uint8_t>((sr + n / 2) / n);
    g = static_cast<uint8_t>((sg + n / 2) / n);
    b = static_cast<uint8_t>((sb + n / 2) / n);
}

void ChromatideCanvas::beginStrokeBuffer(ChromatideStrokeBuffer& buffer) const {
    buffer.base.assign(pixels.begin(), pixels.end());
    buffer.mask.assign(static_cast<size_t>(WIDTH) * HEIGHT, 0.0f);
    buffer.active = true;
    buffer.carryValid = false;
    buffer.hasLastDab = false;
}

void ChromatideCanvas::smudgeAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush,
    RectI* dirtyOut, ChromatideStrokeBuffer& stroke) {
    const float size = clampVal(brush.size, 1.0f, 128.0f);
    const float Ry = size * 0.5f;
    const float Rx = Ry * (4.0f / VIEWPORT_ASPECT_RATIO);
    const int cx = static_cast<int>(std::lround(centerRx));
    const int cy = static_cast<int>(std::lround(centerRy));

    // Carry grid sized for the largest brush so size changes mid-stroke stay in bounds.
    const int halfW = static_cast<int>(std::ceil(64.0f * (4.0f / VIEWPORT_ASPECT_RATIO))) + 2;
    const int halfH = 66;
    const int gridW = 2 * halfW + 1;
    if (stroke.carryHalfW != halfW || stroke.carryHalfH != halfH) {
        stroke.carry.assign(static_cast<size_t>(gridW) * (2 * halfH + 1) * 3, 0.0f);
        stroke.carryHalfW = halfW;
        stroke.carryHalfH = halfH;
        stroke.carryValid = false;
    }
    const int ix = std::min(halfW, static_cast<int>(std::ceil(Rx)) + 1);
    const int iy = std::min(halfH, static_cast<int>(std::ceil(Ry)) + 1);
    const bool load = !stroke.carryValid;
    stroke.carryValid = true;

    const float strength = clampVal(brush.flow, 0.0f, 1.0f);
    // Length 0 refreshes the carry quickly (short smears), 1 holds it for long drags.
    const float pickup = 0.5f * (1.0f - clampVal(brush.smudgeLength, 0.0f, 1.0f)) + 0.02f;
    const float aaEdge = 1.0f / std::max(Ry, 0.5f);
    const float core = std::min(clampVal(brush.hardness, 0.0f, 1.0f), 1.0f - aaEdge);
    const float invFalloff = 1.0f / std::max(1.0f - core, 1e-4f);
    const float invRx = 1.0f / Rx, invRy = 1.0f / Ry;

    for (int oy = -iy; oy <= iy; ++oy) {
        const int y = cy + oy;
        if (y < 0 || y >= HEIGHT) continue;
        const float dy = static_cast<float>(oy) * invRy;
        for (int ox = -ix; ox <= ix; ++ox) {
            const int x = cx + ox;
            if (x < 0 || x >= WIDTH) continue;
            const float dx = static_cast<float>(ox) * invRx;
            const float dSq = dx * dx + dy * dy;
            if (dSq >= 1.0f) continue;
            float cov = 1.0f;
            const float d = std::sqrt(dSq);
            if (d > core) {
                cov = 1.0f - clampVal((d - core) * invFalloff, 0.0f, 1.0f);
                cov = cov * cov * (3.0f - 2.0f * cov);
            }
            float* carry = &stroke.carry[(static_cast<size_t>(oy + halfH) * gridW + static_cast<size_t>(ox + halfW)) * 3];
            const size_t off = pixelOffset(x, y);
            if (load) {
                for (int c = 0; c < 3; ++c) carry[c] = pixels[off + c];
                continue;
            }
            const float k = strength * cov;
            const float p = pickup * cov;
            for (int c = 0; c < 3; ++c) {
                const float under = pixels[off + c];
                const float out = under + k * (carry[c] - under);
                carry[c] += p * (under - carry[c]);
                pixels[off + c] = static_cast<uint8_t>(clampVal(out + 0.5f, 0.0f, 255.0f));
            }
            if (dirtyOut && k > 0.0f) dirtyOut->expand(x, y);
        }
    }
    revision++;
}

void ChromatideCanvas::stampAtRaster(float centerRx, float centerRy, const ChromatideBrushState& brush, RectI* dirtyOut,
    ChromatideStrokeBuffer* stroke) {
    if (brush.tool == ChromatideTool::Smudge) {
        if (stroke) smudgeAtRaster(centerRx, centerRy, brush, dirtyOut, *stroke);
        return;
    }
    float size = clampVal(brush.size, 1.0f, 128.0f);
    float Ry = size * 0.5f;
    float Rx = (size * 0.5f) * (4.0f / VIEWPORT_ASPECT_RATIO);

    int minX = clampVal(static_cast<int>(std::floor(centerRx - Rx - 2.0f)), 0, WIDTH - 1);
    int maxX = clampVal(static_cast<int>(std::ceil(centerRx + Rx + 2.0f)), 0, WIDTH - 1);
    int minY = clampVal(static_cast<int>(std::floor(centerRy - Ry - 2.0f)), 0, HEIGHT - 1);
    int maxY = clampVal(static_cast<int>(std::ceil(centerRy + Ry + 2.0f)), 0, HEIGHT - 1);

    const ChromatideColor& src = (brush.tool == ChromatideTool::Eraser) ? brush.background : brush.foreground;
    const float srcR = src.r, srcG = src.g, srcB = src.b;

    const float opacity = clampVal(brush.opacity, 0.0f, 1.0f);
    const float flow = clampVal(brush.flow, 0.0005f, 1.0f);  // dwell dabs go below the knob minimum
    const bool indirect = stroke && stroke->active
        && stroke->base.size() == BUFFER_SIZE && stroke->mask.size() == static_cast<size_t>(WIDTH) * HEIGHT;

    // Hardness sets the solid core; outside it coverage falls off with a smoothstep.
    // Keep at least ~1px of edge so hard brushes stay antialiased.
    const float aaEdge = 1.0f / std::max(Ry, 0.5f);
    const float core = std::min(clampVal(brush.hardness, 0.0f, 1.0f), 1.0f - aaEdge);
    const float outer = 1.0f + 0.5f * aaEdge;
    const float invFalloff = 1.0f / std::max(outer - core, 1e-4f);
    const float outerSq = outer * outer;
    const float invRx = 1.0f / Rx, invRy = 1.0f / Ry;
    const ChromatideBrushHead head = brush.head;
    // Chisel: ellipse at brush.angle, long axis = radius, short axis = thickness.
    // One sin/cos per dab, not per pixel.
    const float kChiselInvMinor = 1.0f / clampVal(brush.thickness, 0.05f, 1.0f);
    float chiselCos = 0.0f, chiselSin = 0.0f;
    if (head == ChromatideBrushHead::Chisel) {
        chiselCos = std::cos(brush.angle);
        chiselSin = std::sin(brush.angle);
    }
    const float sprayDensity = clampVal(brush.density, 0.0f, 1.0f);
    const uint32_t seed = ++dabSeed * 0x9E3779B9u;

    for (int y = minY; y <= maxY; ++y) {
        float dy = (static_cast<float>(y) - centerRy) * invRy;
        float dySq = dy * dy;
        if (dySq >= outerSq) continue;
        for (int x = minX; x <= maxX; ++x) {
            float dx = (static_cast<float>(x) - centerRx) * invRx;
            float dSq;
            switch (head) {
                case ChromatideBrushHead::Square: {
                    const float m = std::max(std::fabs(dx), std::fabs(dy));
                    dSq = m * m;
                    break;
                }
                case ChromatideBrushHead::Chisel: {
                    const float a = dx * chiselCos + dy * chiselSin;
                    const float b = (dy * chiselCos - dx * chiselSin) * kChiselInvMinor;
                    dSq = a * a + b * b;
                    break;
                }
                default:
                    dSq = dx * dx + dySq;
                    break;
            }
            if (dSq >= outerSq) continue;

            float cov = 1.0f;
            float dNorm = std::sqrt(dSq);
            if (dNorm > core) {
                cov = 1.0f - clampVal((dNorm - core) * invFalloff, 0.0f, 1.0f);
                cov = cov * cov * (3.0f - 2.0f * cov);
            }
            if (head == ChromatideBrushHead::Spray) {
                // Integer hash speckle: `density` of pixels at the core, thinning with falloff.
                uint32_t h = static_cast<uint32_t>(x) * 73856093u ^ static_cast<uint32_t>(y) * 19349663u ^ seed;
                h ^= h >> 13; h *= 0x5bd1e995u; h ^= h >> 15;
                const float r = static_cast<float>(h & 0xFFFFu) * (1.0f / 65536.0f);
                cov = (r < sprayDensity * cov) ? 1.0f : 0.0f;
            }
            float dab = flow * cov;
            if (dab <= 0.0005f) continue;

            size_t off = pixelOffset(x, y);
            float alpha;
            float dstR, dstG, dstB;
            if (indirect) {
                float& m = stroke->mask[static_cast<size_t>(y) * WIDTH + static_cast<size_t>(x)];
                m += dab * (1.0f - m);  // "over" accumulation, saturates at 1
                alpha = opacity * m;
                dstR = stroke->base[off + 0];
                dstG = stroke->base[off + 1];
                dstB = stroke->base[off + 2];
            } else {
                alpha = opacity * dab;
                dstR = pixels[off + 0];
                dstG = pixels[off + 1];
                dstB = pixels[off + 2];
            }

            pixels[off + 0] = static_cast<uint8_t>(clampVal(dstR + alpha * (srcR - dstR) + 0.5f, 0.0f, 255.0f));
            pixels[off + 1] = static_cast<uint8_t>(clampVal(dstG + alpha * (srcG - dstG) + 0.5f, 0.0f, 255.0f));
            pixels[off + 2] = static_cast<uint8_t>(clampVal(dstB + alpha * (srcB - dstB) + 0.5f, 0.0f, 255.0f));

            if (dirtyOut) {
                dirtyOut->expand(x, y);
            }
        }
    }
    revision++;
}


void ChromatideCanvas::strokeToNormalized(float uPrev, float vPrev, float uCurr, float vCurr, const ChromatideBrushState& brush,
    RectI* dirtyOut, ChromatideStrokeBuffer* stroke) {
    // Krita-like 10% spacing: smooth strokes, and flow stays meaningful since
    // the stroke mask (not the canvas) absorbs overlap.
    float viewportBrushDiameter = brush.size / static_cast<float>(HEIGHT);
    float stepSizeNorm = std::max(0.10f * viewportBrushDiameter, 0.001f);

    // Distance in viewport-height units, matching stepSizeNorm.
    float distanceNorm = std::hypot((uCurr - uPrev) * VIEWPORT_ASPECT_RATIO, vCurr - vPrev);
    const float jitter = clampVal(brush.jitter, 0.0f, 1.0f);
    ChromatideBrushState jittered = brush;

    // With a stroke, walk from the last dab toward the cursor in whole
    // spacing steps; the remainder waits for more movement. Without one,
    // keep the old even subdivision (at least one dab).
    int numSteps = std::max(1, static_cast<int>(std::ceil(distanceNorm / stepSizeNorm)));
    if (stroke) {
        if (!stroke->hasLastDab) {
            stroke->lastDabU = uPrev;
            stroke->lastDabV = vPrev;
            stroke->hasLastDab = true;
        }
        uPrev = stroke->lastDabU;
        vPrev = stroke->lastDabV;
        const float d = std::hypot((uCurr - uPrev) * VIEWPORT_ASPECT_RATIO, vCurr - vPrev);
        numSteps = static_cast<int>(d / stepSizeNorm);
        if (numSteps == 0) return;
        // Shorten the segment to end exactly on the last whole step.
        const float f = static_cast<float>(numSteps) * stepSizeNorm / d;
        uCurr = uPrev + f * (uCurr - uPrev);
        vCurr = vPrev + f * (vCurr - vPrev);
        stroke->lastDabU = uCurr;
        stroke->lastDabV = vCurr;
    }

    for (int i = 1; i <= numSteps; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(numSteps);
        float u = uPrev + t * (uCurr - uPrev);
        float v = vPrev + t * (vCurr - vPrev);
        float rx = 0.0f, ry = 0.0f;
        normalizedToRaster(u, v, rx, ry);
        if (jitter > 0.0f) {
            // Hash per dab: scatter up to one radius and shrink up to 60%.
            uint32_t h = ++jitterSeed * 0x9E3779B9u;
            auto next = [&h]() {
                h ^= h >> 15; h *= 0x2c1b3c6du; h ^= h >> 12; h *= 0x297a2d39u; h ^= h >> 15;
                return static_cast<float>(h & 0xFFFFu) * (1.0f / 65535.0f);
            };
            const float ry0 = 0.5f * brush.size;
            const float rx0 = ry0 * (4.0f / VIEWPORT_ASPECT_RATIO);
            rx += jitter * rx0 * (2.0f * next() - 1.0f);
            ry += jitter * ry0 * (2.0f * next() - 1.0f);
            jittered.size = std::max(1.0f, brush.size * (1.0f - 0.6f * jitter * next()));
            stampAtRaster(rx, ry, jittered, dirtyOut, stroke);
        } else {
            stampAtRaster(rx, ry, brush, dirtyOut, stroke);
        }
    }
}

void ChromatideCanvas::beginStrokeTransaction(ChromatideUndoRecord& recordOut) const {
    recordOut.bounds = RectI(0, 0, WIDTH - 1, HEIGHT - 1);
    recordOut.beforeRgb.assign(pixels.begin(), pixels.end());
    recordOut.afterRgb.clear();
}

void ChromatideCanvas::finalizeStrokeTransaction(const RectI& dirtyBounds, ChromatideUndoRecord& recordInOut) {
    if (!dirtyBounds.valid() || recordInOut.beforeRgb.size() != BUFFER_SIZE) {
        recordInOut.bounds.reset();
        recordInOut.beforeRgb.clear();
        recordInOut.afterRgb.clear();
        return;
    }

    const size_t count = static_cast<size_t>(dirtyBounds.width() * dirtyBounds.height() * CHANNELS);
    std::vector<uint8_t> before(count);
    recordInOut.afterRgb.resize(count);
    const size_t rowBytes = static_cast<size_t>(dirtyBounds.width() * CHANNELS);
    size_t destination = 0;
    for (int y = dirtyBounds.minY; y <= dirtyBounds.maxY; ++y) {
        const size_t source = pixelOffset(dirtyBounds.minX, y);
        std::copy_n(recordInOut.beforeRgb.begin() + source, rowBytes, before.begin() + destination);
        std::copy_n(pixels.begin() + source, rowBytes, recordInOut.afterRgb.begin() + destination);
        destination += rowBytes;
    }
    recordInOut.beforeRgb.swap(before);
    recordInOut.bounds = dirtyBounds;
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

namespace {
// QOI may use a five-byte RGBA opcode even with a three-channel header.
constexpr size_t maxCanvasQoiBytes = 14u + size_t(ChromatideCanvas::WIDTH)
    * ChromatideCanvas::HEIGHT * 5u + 8u;
constexpr size_t maxCanvasBase64Bytes = ((maxCanvasQoiBytes + 2u) / 3u) * 4u;

bool validCanvasQoi(const std::vector<uint8_t>& bytes) {
    if (bytes.size() < 22u || bytes.size() > maxCanvasQoiBytes) return false;
    auto read32 = [&](size_t p) {
        return (uint32_t(bytes[p]) << 24) | (uint32_t(bytes[p + 1]) << 16)
            | (uint32_t(bytes[p + 2]) << 8) | uint32_t(bytes[p + 3]);
    };
    if (read32(0) != 0x716f6966u || read32(4) != ChromatideCanvas::WIDTH
        || read32(8) != ChromatideCanvas::HEIGHT
        || bytes[12] != ChromatideCanvas::CHANNELS || bytes[13] > 1) return false;
    const size_t end = bytes.size() - 8u;
    for (size_t i = end; i < bytes.size(); ++i)
        if (bytes[i] != (i == bytes.size() - 1u ? 1 : 0)) return false;

    // The bundled decoder tolerates incomplete streams. Validate opcode lengths
    // and exact pixel coverage before allocating or replacing the canvas.
    size_t pixelsLeft = size_t(ChromatideCanvas::WIDTH) * ChromatideCanvas::HEIGHT;
    size_t pos = 14;
    while (pos < end && pixelsLeft) {
        const uint8_t op = bytes[pos++];
        size_t extra = 0, count = 1;
        if (op == 0xfe) extra = 3;
        else if (op == 0xff) extra = 4;
        else if ((op & 0xc0) == 0x80) extra = 1;
        else if ((op & 0xc0) == 0xc0) count = (op & 0x3f) + 1u;
        if (extra > end - pos || count > pixelsLeft) return false;
        pos += extra;
        pixelsLeft -= count;
    }
    return pixelsLeft == 0 && pos == end;
}
}

bool ChromatideCanvas::deserializeQoiBase64(const std::string& b64Data) {
    if (b64Data.size() > maxCanvasBase64Bytes) return false;
    std::vector<uint8_t> qoiBytes;
    if (!base64Decode(b64Data, qoiBytes) || !validCanvasQoi(qoiBytes)) return false;

    qoi_desc desc {};
    // maxCanvasQoiBytes is below INT_MAX, so the checked size safely fits int.
    void* rawPixels = qoi_decode(qoiBytes.data(), static_cast<int>(qoiBytes.size()), &desc, CHANNELS);
    if (!rawPixels) return false;
    if (desc.width != WIDTH || desc.height != HEIGHT || desc.channels != CHANNELS) {
        std::free(rawPixels);
        return false;
    }
    std::memcpy(pixels.data(), rawPixels, BUFFER_SIZE);
    std::free(rawPixels);
    revision++;
    return true;
}
