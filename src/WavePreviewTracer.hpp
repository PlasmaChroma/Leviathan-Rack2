#pragma once

#include "NvgGraphicsLifecycle.hpp"
#include "plugin.hpp"
#include "WavePreviewSimplifier.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>

enum WavePreviewTracerCacheMode {
	WAVE_PREVIEW_TRACER_CURVE_CACHE = 0,
	WAVE_PREVIEW_TRACER_FRAME_CACHE = 1,
};

struct WavePreviewTracerStyle {
	NVGcolor color = nvgRGBA(255, 190, 80, 255);
	float lineWidth = 1.15f;
	float fadeSec = 0.333f;
	float minCaptureIntervalSec = 1.f / 24.f;
	float maxAlpha = 118.f;
	int drawStride = 2;
};

struct WavePreviewBufferedTracerStyle {
	NVGcolor color = nvgRGBA(255, 190, 80, 255);
	float fadeSec = 0.333f;
	float minCaptureIntervalSec = 1.f / 24.f;
	float maxAlpha = 118.f;
	float rasterScale = 2.f;
	int drawStride = 2;
	int lineRadiusPx = 1;
};

template <size_t PointCount, size_t FrameCount>
struct WavePreviewTracer {
	static_assert(PointCount > 0, "WavePreviewTracer requires at least one point");
	static_assert(FrameCount > 0, "WavePreviewTracer requires at least one frame");

	struct Frame {
		std::array<Vec, PointCount> points {};
		double birthSec = 0.0;
		bool active = false;
	};

	std::array<Frame, FrameCount> frames {};
	int nextFrame = 0;
	double lastCaptureSec = -1.0;

	void capture(const std::array<Vec, PointCount>& points, double nowSec, float minCaptureIntervalSec) {
		if (lastCaptureSec > 0.0 && (nowSec - lastCaptureSec) < minCaptureIntervalSec) {
			return;
		}
		Frame& frame = frames[nextFrame];
		frame.points = points;
		frame.birthSec = nowSec;
		frame.active = true;
		nextFrame = (nextFrame + 1) % int(FrameCount);
		lastCaptureSec = nowSec;
	}

	void expire(double nowSec, float fadeSec) {
		for (Frame& frame : frames) {
			if (frame.active && (nowSec - frame.birthSec) >= fadeSec) {
				frame.active = false;
			}
		}
	}

	void clear() {
		for (Frame& frame : frames) {
			frame.active = false;
		}
		nextFrame = 0;
		lastCaptureSec = -1.0;
	}

	bool hasActiveFrames() const {
		for (const Frame& frame : frames) {
			if (frame.active) {
				return true;
			}
		}
		return false;
	}

	void draw(NVGcontext* vg, double nowSec, const WavePreviewTracerStyle& style) const {
		const int stride = std::max(style.drawStride, 1);
		const float fadeSec = std::max(style.fadeSec, 1e-6f);
		for (const Frame& frame : frames) {
			if (!frame.active) {
				continue;
			}
			const float age = float(nowSec - frame.birthSec);
			if (age < 0.f || age >= fadeSec) {
				continue;
			}
			const float fade = 1.f - age / fadeSec;
			const int alpha = clamp(int(style.maxAlpha * fade), 0, 255);
			if (alpha <= 0) {
				continue;
			}
			NVGcolor color = style.color;
			color.a *= float(alpha) / 255.f;
			nvgBeginPath(vg);
			wave_preview::simplifyPath(frame.points.data(), PointCount, stride, 0.02f, [vg](const Vec& pt, bool isMove) {
				if (isMove) {
					nvgMoveTo(vg, pt.x, pt.y);
				} else {
					nvgLineTo(vg, pt.x, pt.y);
				}
			});
			nvgStrokeColor(vg, color);
			nvgStrokeWidth(vg, style.lineWidth);
			nvgLineCap(vg, NVG_BUTT);
			nvgLineJoin(vg, NVG_ROUND);
			nvgStroke(vg);
		}
	}
};

template <size_t PointCount>
struct WavePreviewBufferedTracer {
	static_assert(PointCount > 0, "WavePreviewBufferedTracer requires at least one point");

	NVGcontext* imageVg = nullptr;
	int imageHandle = -1;
	int imageW = 0;
	int imageH = 0;
	std::vector<uint32_t> pixels;
	double lastCaptureSec = -1.0;
	double lastFadeSec = -1.0;
	bool pixelsDirty = false;
	bool hasVisiblePixels = false;

	~WavePreviewBufferedTracer() {
		// NanoVG image handles are context-owned; do not delete from an unknown
		// context during widget teardown.
		imageVg = nullptr;
		imageHandle = -1;
	}

	void clearPixels() {
		std::fill(pixels.begin(), pixels.end(), 0u);
		pixelsDirty = true;
		hasVisiblePixels = false;
	}

	void clear() {
		clearPixels();
		lastCaptureSec = -1.0;
		lastFadeSec = -1.0;
	}

	void resetImage(NVGcontext* vg, bool deleteCurrentHandle) {
		nvg_gfx_lifecycle::resetOwnedNvgImage(imageVg, imageHandle, imageW, imageH, vg, deleteCurrentHandle);
		pixels.clear();
		pixelsDirty = false;
		hasVisiblePixels = false;
		lastCaptureSec = -1.0;
		lastFadeSec = -1.0;
	}

	void ensureSize(int w, int h) {
		w = std::max(w, 1);
		h = std::max(h, 1);
		if (w == imageW && h == imageH && pixels.size() == size_t(w * h)) {
			return;
		}
		imageW = w;
		imageH = h;
		pixels.assign(size_t(w * h), 0u);
		pixelsDirty = true;
		hasVisiblePixels = false;
		lastCaptureSec = -1.0;
		lastFadeSec = -1.0;
	}

	void fade(double nowSec, float fadeSec) {
		if (!hasVisiblePixels) {
			lastFadeSec = nowSec;
			return;
		}
		if (lastFadeSec < 0.0) {
			lastFadeSec = nowSec;
			return;
		}
		const float dt = std::max(0.f, float(nowSec - lastFadeSec));
		lastFadeSec = nowSec;
		if (dt <= 0.f) {
			return;
		}
		const float targetRemaining = 0.03f;
		const float scale = std::pow(targetRemaining, dt / std::max(fadeSec, 1e-6f));
		const int alphaScale = clamp(int(scale * 256.f), 0, 256);
		bool any = false;
		for (uint32_t& px : pixels) {
			if (px == 0u) {
				continue;
			}
			const uint32_t a = (px >> 24) & 0xffu;
			const uint32_t r = px & 0xffu;
			const uint32_t g = (px >> 8) & 0xffu;
			const uint32_t b = (px >> 16) & 0xffu;
			const uint32_t na = (a * uint32_t(alphaScale)) >> 8;
			const uint32_t nr = (r * uint32_t(alphaScale)) >> 8;
			const uint32_t ng = (g * uint32_t(alphaScale)) >> 8;
			const uint32_t nb = (b * uint32_t(alphaScale)) >> 8;
			if (na > 1u) {
				any = true;
				px = (na << 24) | (nb << 16) | (ng << 8) | nr;
			}
			else {
				px = 0u;
			}
		}
		hasVisiblePixels = any;
		pixelsDirty = true;
	}

	void blendPixel(int x, int y, uint32_t srcR, uint32_t srcG, uint32_t srcB, uint32_t srcA) {
		if (x < 0 || y < 0 || x >= imageW || y >= imageH || srcA == 0u) {
			return;
		}
		uint32_t& dst = pixels[size_t(y * imageW + x)];
		const uint32_t da = (dst >> 24) & 0xffu;
		const uint32_t dr = dst & 0xffu;
		const uint32_t dg = (dst >> 8) & 0xffu;
		const uint32_t db = (dst >> 16) & 0xffu;
		const uint32_t inv = 255u - srcA;
		const uint32_t outA = std::min(255u, srcA + ((da * inv + 127u) / 255u));
		const uint32_t outR = std::min(255u, srcR + ((dr * inv + 127u) / 255u));
		const uint32_t outG = std::min(255u, srcG + ((dg * inv + 127u) / 255u));
		const uint32_t outB = std::min(255u, srcB + ((db * inv + 127u) / 255u));
		dst = (outA << 24) | (outB << 16) | (outG << 8) | outR;
	}

	void stampPoint(int x, int y, int radius, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
		for (int oy = -radius; oy <= radius; ++oy) {
			for (int ox = -radius; ox <= radius; ++ox) {
				if (ox * ox + oy * oy <= radius * radius) {
					blendPixel(x + ox, y + oy, r, g, b, a);
				}
			}
		}
	}

	void drawLine(Vec a, Vec b, int radius, uint32_t r, uint32_t g, uint32_t bl, uint32_t alpha) {
		const float dx = b.x - a.x;
		const float dy = b.y - a.y;
		const int steps = std::max(1, int(std::ceil(std::max(std::fabs(dx), std::fabs(dy)))));
		for (int i = 0; i <= steps; ++i) {
			const float t = float(i) / float(steps);
			stampPoint(int(std::round(a.x + dx * t)), int(std::round(a.y + dy * t)), radius, r, g, bl, alpha);
		}
	}

	void capture(const std::array<Vec, PointCount>& points,
	             double nowSec,
	             const Vec& size,
	             const WavePreviewBufferedTracerStyle& style) {
		const float rasterScale = clamp(style.rasterScale, 1.f, 4.f);
		ensureSize(int(std::ceil(std::max(size.x, 1.f) * rasterScale)), int(std::ceil(std::max(size.y, 1.f) * rasterScale)));
		if (lastCaptureSec > 0.0 && (nowSec - lastCaptureSec) < style.minCaptureIntervalSec) {
			return;
		}
		const int stride = std::max(style.drawStride, 1);
		const int radius = std::max(int(std::round(float(style.lineRadiusPx) * rasterScale)), 0);
		const uint32_t alpha = uint32_t(clamp(int(style.maxAlpha), 0, 255));
		const uint32_t r = uint32_t(clamp(int(style.color.r * float(alpha)), 0, 255));
		const uint32_t g = uint32_t(clamp(int(style.color.g * float(alpha)), 0, 255));
		const uint32_t b = uint32_t(clamp(int(style.color.b * float(alpha)), 0, 255));
		Vec prev;
		bool hasPrev = false;
		wave_preview::simplifyPath(points.data(), PointCount, stride, 0.02f, [&](const Vec& pt, bool isMove) {
			Vec scaled = pt.mult(rasterScale);
			if (isMove) {
				prev = scaled;
				hasPrev = true;
			} else {
				if (hasPrev) {
					drawLine(prev, scaled, radius, r, g, b, alpha);
				}
				prev = scaled;
			}
		});
		lastCaptureSec = nowSec;
		pixelsDirty = true;
		hasVisiblePixels = true;
	}

	void draw(NVGcontext* vg, double nowSec, const Vec& size, const WavePreviewBufferedTracerStyle& style) {
		if (!vg) {
			return;
		}
		if (imageVg != vg) {
			resetImage(vg, false);
		}
		const float rasterScale = clamp(style.rasterScale, 1.f, 4.f);
		ensureSize(int(std::ceil(std::max(size.x, 1.f) * rasterScale)), int(std::ceil(std::max(size.y, 1.f) * rasterScale)));
		fade(nowSec, style.fadeSec);
		if (imageHandle < 0 || !nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, imageHandle, imageW, imageH)) {
			imageHandle = nvgCreateImageRGBA(vg, imageW, imageH, NVG_IMAGE_PREMULTIPLIED,
			                                 reinterpret_cast<const unsigned char*>(pixels.data()));
			imageVg = vg;
			pixelsDirty = false;
		}
		else if (pixelsDirty) {
			nvgUpdateImage(vg, imageHandle, reinterpret_cast<const unsigned char*>(pixels.data()));
			pixelsDirty = false;
		}
		if (imageHandle < 0 || !hasVisiblePixels) {
			return;
		}
		NVGpaint paint = nvgImagePattern(vg, 0.f, 0.f, size.x, size.y, 0.f, imageHandle, 1.f);
		nvgBeginPath(vg);
		nvgRect(vg, 0.f, 0.f, size.x, size.y);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	}
};
