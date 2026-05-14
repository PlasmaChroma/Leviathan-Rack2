#include "WyrmSand.hpp"

namespace {
int resolveSandDetail(int detailSetting, Vec size) {
	switch (detailSetting) {
		case WYRMSAND_DETAIL_LOW: return WYRMSAND_DETAIL_LOW;
		case WYRMSAND_DETAIL_MEDIUM: return WYRMSAND_DETAIL_MEDIUM;
		case WYRMSAND_DETAIL_HIGH: return WYRMSAND_DETAIL_HIGH;
		case WYRMSAND_DETAIL_AUTO:
		default: {
			const float area = size.x * size.y;
			if (area <= 120000.f) return WYRMSAND_DETAIL_LOW;
			if (area <= 220000.f) return WYRMSAND_DETAIL_MEDIUM;
			return WYRMSAND_DETAIL_HIGH;
		}
	}
}

inline unsigned char clampU8(int v) {
	return (unsigned char) clamp(v, 0, 255);
}

inline void blendOverOpaque(unsigned char* rgb, int sr, int sg, int sb, int sa) {
	const int a = clamp(sa, 0, 255);
	const int invA = 255 - a;
	rgb[0] = clampU8((sr * a + int(rgb[0]) * invA) / 255);
	rgb[1] = clampU8((sg * a + int(rgb[1]) * invA) / 255);
	rgb[2] = clampU8((sb * a + int(rgb[2]) * invA) / 255);
}
}

void WyrmSand::resetHistory() {
	previousPathCount = 0;
	lastUpdateSec = -1.0;
}

void WyrmSand::markImageDirty() {
	imageDirty = true;
	imageRevision++;
}

const unsigned char* WyrmSand::imageData() const {
	return imagePixels.empty() ? nullptr : imagePixels.data();
}

int WyrmSand::imageWidth() const {
	return imageW;
}

int WyrmSand::imageHeight() const {
	return imageH;
}

uint64_t WyrmSand::imageDataRevision() const {
	return imageRevision;
}

void WyrmSand::ensureField(Vec size, int detailSetting) {
	const int resolvedDetail = resolveSandDetail(detailSetting, size);
	float density = 0.65f;
	int minW = 64;
	int maxW = 128;
	int minH = 32;
	int maxH = 72;
	switch (resolvedDetail) {
		case WYRMSAND_DETAIL_LOW:
			density = 0.50f;
			minW = 48; maxW = 96;
			minH = 24; maxH = 54;
			break;
		case WYRMSAND_DETAIL_MEDIUM:
			density = 0.65f;
			minW = 64; maxW = 128;
			minH = 32; maxH = 72;
			break;
		case WYRMSAND_DETAIL_HIGH:
		default:
			density = 0.82f;
			minW = 80; maxW = 156;
			minH = 40; maxH = 92;
			break;
	}
	const int targetW = clamp(int(size.x * density), minW, maxW);
	const int targetH = clamp(int(size.y * density), minH, maxH);
	if (initialized && w == targetW && h == targetH) {
		return;
	}
	w = targetW;
	h = targetH;
	const int cellCount = w * h;
	depth.assign(cellCount, 0.f);
	energy.assign(cellCount, 0.f);
	baseNoise.resize(cellCount);
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			const uint32_t seed = 0x6d2b79f5u ^ uint32_t(x * 73856093) ^ uint32_t(y * 19349663);
			const float grain = hashUnit(seed);
			const float dune = 0.5f + 0.5f * std::sin(0.17f * float(x) + 0.31f * float(y));
			baseNoise[y * w + x] = 0.72f * grain + 0.28f * dune;
		}
	}
	imageDirty = true;
	initialized = true;
	resetHistory();
}

void WyrmSand::stamp(Vec size, Vec pos, float radiusPx, float depthDelta, float energyDelta) {
	if (!initialized || w <= 0 || h <= 0 || size.x <= 1.f || size.y <= 1.f) {
		return;
	}
	const float cellW = size.x / float(w);
	const float cellH = size.y / float(h);
	const int x0 = clamp(int(std::floor((pos.x - radiusPx) / cellW)), 0, w - 1);
	const int x1 = clamp(int(std::ceil((pos.x + radiusPx) / cellW)), 0, w - 1);
	const int y0 = clamp(int(std::floor((pos.y - radiusPx) / cellH)), 0, h - 1);
	const int y1 = clamp(int(std::ceil((pos.y + radiusPx) / cellH)), 0, h - 1);
	const float invRadius = 1.f / std::max(radiusPx, 1e-4f);
	for (int gy = y0; gy <= y1; ++gy) {
		const float cy = (float(gy) + 0.5f) * cellH;
		for (int gx = x0; gx <= x1; ++gx) {
			const float cx = (float(gx) + 0.5f) * cellW;
			const float dx = cx - pos.x;
			const float dy = cy - pos.y;
			const float dist = std::sqrt(dx * dx + dy * dy);
			const float falloff = smoother01(1.f - dist * invRadius);
			if (falloff <= 0.f) {
				continue;
			}
			const int idx = gy * w + gx;
			depth[idx] = clamp(depth[idx] + depthDelta * falloff, -1.f, 1.f);
			energy[idx] = clamp(energy[idx] + energyDelta * falloff, 0.f, 1.f);
		}
	}
	markImageDirty();
}

void WyrmSand::disturbSegment(Vec size, Vec a, Vec b, float troughStrength, float ridgeStrength, float energyStrength) {
	if (!initialized || w <= 0 || h <= 0 || size.x <= 1.f || size.y <= 1.f) {
		return;
	}
	Vec ab = b.minus(a);
	float len = std::sqrt(ab.x * ab.x + ab.y * ab.y);
	if (len < 1e-3f) {
		stamp(size, a, 4.f, -troughStrength, energyStrength);
		return;
	}
	ab = ab.div(len);
	const Vec normal(-ab.y, ab.x);
	const float cellW = size.x / float(w);
	const float cellH = size.y / float(h);
	const float bodyRadiusPx = 3.1f;
	const float ridgeOffsetPx = 3.8f;
	const float effectRadiusPx = bodyRadiusPx + ridgeOffsetPx + 1.2f;
	const float minX = std::min(a.x, b.x) - effectRadiusPx;
	const float maxX = std::max(a.x, b.x) + effectRadiusPx;
	const float minY = std::min(a.y, b.y) - effectRadiusPx;
	const float maxY = std::max(a.y, b.y) + effectRadiusPx;
	const int x0 = clamp(int(std::floor(minX / cellW)), 0, w - 1);
	const int x1 = clamp(int(std::ceil(maxX / cellW)), 0, w - 1);
	const int y0 = clamp(int(std::floor(minY / cellH)), 0, h - 1);
	const int y1 = clamp(int(std::ceil(maxY / cellH)), 0, h - 1);
	for (int gy = y0; gy <= y1; ++gy) {
		const float cy = (float(gy) + 0.5f) * cellH;
		for (int gx = x0; gx <= x1; ++gx) {
			const float cx = (float(gx) + 0.5f) * cellW;
			const Vec p(cx, cy);
			const Vec ap = p.minus(a);
			const float along = clamp(ap.x * ab.x + ap.y * ab.y, 0.f, len);
			const Vec nearest = a.plus(ab.mult(along));
			const float signedSide = (p.x - nearest.x) * normal.x + (p.y - nearest.y) * normal.y;
			const float absSide = std::fabs(signedSide);
			const float trough = smoother01(1.f - absSide / bodyRadiusPx);
			const float ridge = smoother01(1.f - std::fabs(absSide - ridgeOffsetPx) / 2.4f);
			const float e = smoother01(1.f - absSide / effectRadiusPx);
			if (trough <= 0.f && ridge <= 0.f && e <= 0.f) {
				continue;
			}
			const int idx = gy * w + gx;
			depth[idx] = clamp(depth[idx] - troughStrength * trough + ridgeStrength * ridge, -1.f, 1.f);
			energy[idx] = clamp(energy[idx] + energyStrength * e, 0.f, 1.f);
		}
	}
	markImageDirty();
}

void WyrmSand::update(Vec size, double nowSec, int detailSetting, const std::array<Vec, kWyrmPointCountMax>& currentPath, int pathCount, float slitherAmount) {
	if (!std::isfinite(nowSec)) {
		resetHistory();
		return;
	}
	ensureField(size, detailSetting);
	if (lastUpdateSec < 0.0 || !std::isfinite(lastUpdateSec)) {
		lastUpdateSec = nowSec;
	}
	const float elapsed = clamp(float(nowSec - lastUpdateSec), 0.f, 0.25f);
	lastUpdateSec = nowSec;
	const float depthDecay = std::exp(-0.55f * elapsed);
	const float energyDecay = std::exp(-3.5f * elapsed);
	for (int i = 0, n = w * h; i < n; ++i) {
		depth[i] *= depthDecay;
		energy[i] *= energyDecay;
	}
	markImageDirty();
	if (pathCount <= 1) {
		previousPathCount = 0;
		return;
	}

	const bool animateDisturbance = slitherAmount > 1e-4f;
	if (animateDisturbance && previousPathCount == pathCount) {
		const float troughStrength = (0.018f + 0.052f * slitherAmount) * std::min(1.f, elapsed * 60.f);
		const float ridgeStrength = (0.010f + 0.032f * slitherAmount) * std::min(1.f, elapsed * 60.f);
		for (int i = 0; i < pathCount - 1; ++i) {
			const Vec delta = currentPath[i].minus(previousPath[i]);
			const float motion = clamp(std::sqrt(delta.x * delta.x + delta.y * delta.y) / 7.f, 0.f, 1.f);
			const float energyStrength = (0.015f + 0.075f * motion) * slitherAmount;
			disturbSegment(size, currentPath[i], currentPath[i + 1], troughStrength, ridgeStrength, energyStrength);
		}
	}

	for (int i = 0; i < pathCount; ++i) {
		previousPath[i] = currentPath[i];
	}
	previousPathCount = pathCount;
}

void WyrmSand::drawFlatBackground(NVGcontext* vg, Vec size) const {
	nvgBeginPath(vg);
	nvgRect(vg, 0.f, 0.f, size.x, size.y);
	nvgFillColor(vg, nvgRGBA(14, 14, 14, 205));
	nvgFill(vg);
}

void WyrmSand::ensureImageRaster(Vec size, int detailSetting) {
	ensureField(size, detailSetting);
	if (w <= 0 || h <= 0) {
		return;
	}

	const int targetImageW = std::max(1, int(std::round(size.x)));
	const int targetImageH = std::max(1, int(std::round(size.y)));
	if (imageW != targetImageW || imageH != targetImageH || imagePixels.size() != size_t(targetImageW * targetImageH * 4)) {
		imageW = targetImageW;
		imageH = targetImageH;
		imagePixels.assign(size_t(std::max(0, imageW * imageH * 4)), 0);
		imageDirty = true;
	}

	if (imageDirty) {
		const int baseR = 58;
		const int baseG = 40;
		const int baseB = 22;
		for (int py = 0; py < imageH; ++py) {
			const float gyf = (float(py) + 0.5f) * float(h) / float(imageH);
			const int gy = clamp(int(gyf), 0, h - 1);
			for (int pxIdx = 0; pxIdx < imageW; ++pxIdx) {
				const float gxf = (float(pxIdx) + 0.5f) * float(w) / float(imageW);
				const int gx = clamp(int(gxf), 0, w - 1);
				const int idx = gy * w + gx;
				const float grain = baseNoise[idx];
				const float d = clamp(depth[idx], -1.f, 1.f);
				const float e = clamp(energy[idx], 0.f, 1.f);
				float shade = 0.58f + 0.21f * grain + 0.25f * std::max(d, 0.f) - 0.33f * std::max(-d, 0.f);
				shade = clamp(shade + 0.16f * e, 0.f, 1.12f);
				const int r = clamp(int(118.f * shade + 30.f * e), 0, 255);
				const int g = clamp(int(82.f * shade + 22.f * e), 0, 255);
				const int b = clamp(int(42.f * shade + 10.f * grain), 0, 255);
				const int alpha = clamp(116 + int(78.f * std::fabs(d)) + int(64.f * e), 72, 235);

				unsigned char* outPx = &imagePixels[size_t((py * imageW + pxIdx) * 4)];
				outPx[0] = clampU8(baseR);
				outPx[1] = clampU8(baseG);
				outPx[2] = clampU8(baseB);
				outPx[3] = 255;
				blendOverOpaque(outPx, r, g, b, alpha);

				if (e > 0.42f && hashUnit(uint32_t(idx) ^ 0xa53c9e7du) > 0.62f) {
					blendOverOpaque(outPx, 230, 188, 112, int(72.f * e));
				}
			}
		}

		imageDirty = false;
	}
}

void WyrmSand::drawNanoVGImage(NVGcontext* vg, Vec size, int detailSetting) {
	if (!vg) {
		return;
	}
	ensureImageRaster(size, detailSetting);
	if (w <= 0 || h <= 0 || imageW <= 0 || imageH <= 0 || imagePixels.empty()) {
		drawFlatBackground(vg, size);
		return;
	}
	if (imageHandle < 0) {
		imageHandle = nvgCreateImageRGBA(vg, imageW, imageH, NVG_IMAGE_PREMULTIPLIED, imagePixels.data());
		imageUploadedW = imageW;
		imageUploadedH = imageH;
		imageUploadedRevision = imageRevision;
	}
	else if (imageUploadedW != imageW || imageUploadedH != imageH) {
		nvgDeleteImage(vg, imageHandle);
		imageHandle = nvgCreateImageRGBA(vg, imageW, imageH, NVG_IMAGE_PREMULTIPLIED, imagePixels.data());
		imageUploadedW = imageW;
		imageUploadedH = imageH;
		imageUploadedRevision = imageRevision;
	}
	else if (imageUploadedRevision != imageRevision) {
		nvgUpdateImage(vg, imageHandle, imagePixels.data());
		imageUploadedRevision = imageRevision;
	}

	if (imageHandle < 0) {
		drawNanoVGCells(vg, size, detailSetting);
		return;
	}

	nvgBeginPath(vg);
	nvgRect(vg, 0.f, 0.f, size.x, size.y);
	const NVGpaint imagePaint = nvgImagePattern(vg, 0.f, 0.f, size.x, size.y, 0.f, imageHandle, 1.f);
	nvgFillPaint(vg, imagePaint);
	nvgFill(vg);
}

void WyrmSand::drawNanoVGCells(NVGcontext* vg, Vec size, int detailSetting) {
	ensureField(size, detailSetting);
	nvgBeginPath(vg);
	nvgRect(vg, 0.f, 0.f, size.x, size.y);
	nvgFillColor(vg, nvgRGBA(58, 40, 22, 224));
	nvgFill(vg);
	const float cellW = size.x / float(std::max(w, 1));
	const float cellH = size.y / float(std::max(h, 1));
	for (int gy = 0; gy < h; ++gy) {
		for (int gx = 0; gx < w; ++gx) {
			const int idx = gy * w + gx;
			const float grain = baseNoise[idx];
			const float d = clamp(depth[idx], -1.f, 1.f);
			const float e = clamp(energy[idx], 0.f, 1.f);
			float shade = 0.58f + 0.21f * grain + 0.25f * std::max(d, 0.f) - 0.33f * std::max(-d, 0.f);
			shade = clamp(shade + 0.16f * e, 0.f, 1.12f);
			const int r = clamp(int(118.f * shade + 30.f * e), 0, 255);
			const int g = clamp(int(82.f * shade + 22.f * e), 0, 255);
			const int b = clamp(int(42.f * shade + 10.f * grain), 0, 255);
			const int alpha = clamp(116 + int(78.f * std::fabs(d)) + int(64.f * e), 72, 235);
			nvgBeginPath(vg);
			nvgRect(vg, float(gx) * cellW, float(gy) * cellH, cellW + 0.5f, cellH + 0.5f);
			nvgFillColor(vg, nvgRGBA(r, g, b, alpha));
			nvgFill(vg);
			if (e > 0.42f && hashUnit(uint32_t(idx) ^ 0xa53c9e7du) > 0.62f) {
				nvgBeginPath(vg);
				nvgCircle(vg, (float(gx) + 0.5f) * cellW, (float(gy) + 0.5f) * cellH, 0.35f + 0.75f * e);
				nvgFillColor(vg, nvgRGBA(230, 188, 112, int(72.f * e)));
				nvgFill(vg);
			}
		}
	}
}

void WyrmSand::draw(NVGcontext* vg, Vec size, bool enabled, int backendSetting, int detailSetting) {
	if (!enabled) {
		drawFlatBackground(vg, size);
		return;
	}
	switch (backendSetting) {
		case WYRMSAND_NANOVG_CELLS:
			drawNanoVGCells(vg, size, detailSetting);
			return;
		case WYRMSAND_NANOVG_IMAGE:
			drawNanoVGImage(vg, size, detailSetting);
			return;
		case WYRMSAND_OPENGL_TEXTURE:
		case WYRMSAND_SHADER_FEEDBACK:
		default:
			// Non-cell backends are not wired yet; keep deterministic fallback.
			drawNanoVGCells(vg, size, detailSetting);
			return;
	}
}
