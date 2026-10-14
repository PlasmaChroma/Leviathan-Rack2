#include "SharedSvgCache.hpp"
#include "SharedSvgCacheState.hpp"
#include "../NvgGraphicsLifecycle.hpp"

#include <nanovg.h>
#include <nanosvg.h>
#include <nanosvgrast.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace visual_assets {
namespace shared_svg_cache {

namespace {

CacheState& getCache() {
	static CacheState cache;
	return cache;
}

} // namespace

int getSharedSvgImage(NVGcontext* vg,
                      const std::shared_ptr<window::Svg>& svg,
                      float targetWidth,
                      float targetHeight) {
	if (!vg || !svg || !svg->handle || targetWidth <= 0.f || targetHeight <= 0.f) {
		return -1;
	}
	if (svg->handle->width <= 0.f || svg->handle->height <= 0.f) {
		return -1;
	}

	float rackZoom = 1.0f;
	if (APP && APP->scene && APP->scene->rackScroll) {
		rackZoom = std::max(APP->scene->rackScroll->getZoom(), 0.1f);
	}
	float pixelRatio = (APP && APP->window) ? APP->window->pixelRatio : 1.0f;
	float effectiveScale = rackZoom * pixelRatio;

	// Minimum 3x base scale for subpixel crispness at 100% zoom, scaling with zoom up to 6x.
	int renderScale = std::max(3, int(std::ceil(effectiveScale * 1.5f)));
	renderScale = std::min(renderScale, 6);

	// Rack reports destruction through the main vg for both window contexts.
	NVGcontext* lifetimeVg = vg;
	if (APP && APP->window && (vg == APP->window->vg || vg == APP->window->fbVg)) {
		lifetimeVg = APP->window->vg;
	}
	CacheEntry& entry = getCache().findOrCreate(svg.get(), vg, lifetimeVg);

	int rasterW = std::max(1, int(std::ceil(targetWidth * float(renderScale))));
	int rasterH = std::max(1, int(std::ceil(targetHeight * float(renderScale))));

	// Check if already valid for this context and scale
	if (entry.ownerVg == vg && entry.imageHandle > 0 &&
	    entry.cachedRenderScale == renderScale &&
	    entry.cachedWidth == rasterW && entry.cachedHeight == rasterH &&
	    std::fabs(entry.cachedTargetW - targetWidth) < 0.1f &&
	    std::fabs(entry.cachedTargetH - targetHeight) < 0.1f &&
	    nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, entry.imageHandle, rasterW, rasterH)) {
		return entry.imageHandle;
	}

	// First consumer to hit a new zoom or context triggers the single rasterization.
	NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
	if (!rasterizer) {
		return -1;
	}

	std::vector<unsigned char> rgba(size_t(rasterW) * size_t(rasterH) * 4u, 0u);
	const float scaleX = float(rasterW) / svg->handle->width;
	const float scaleY = float(rasterH) / svg->handle->height;
	const float scale = std::min(scaleX, scaleY);
	const int stride = rasterW * 4;

	nsvgRasterize(rasterizer, svg->handle, 0.f, 0.f, scale, rgba.data(), rasterW, rasterH, stride);
	nsvgDeleteRasterizer(rasterizer);

	// Premultiply alpha for accurate NanoVG hardware blending
	for (size_t i = 0, count = size_t(rasterW) * size_t(rasterH); i < count; ++i) {
		const unsigned int alpha = rgba[i * 4u + 3u];
		rgba[i * 4u + 0u] = static_cast<unsigned char>((unsigned(rgba[i * 4u + 0u]) * alpha + 127u) / 255u);
		rgba[i * 4u + 1u] = static_cast<unsigned char>((unsigned(rgba[i * 4u + 1u]) * alpha + 127u) / 255u);
		rgba[i * 4u + 2u] = static_cast<unsigned char>((unsigned(rgba[i * 4u + 2u]) * alpha + 127u) / 255u);
	}

	const bool ok = nvg_gfx_lifecycle::updateOwnedNvgImageRgba(
		entry.ownerVg, entry.imageHandle, entry.cachedWidth, entry.cachedHeight,
		vg, rasterW, rasterH,
		NVG_IMAGE_PREMULTIPLIED | NVG_IMAGE_GENERATE_MIPMAPS,
		rgba.data());

	if (!ok || entry.imageHandle <= 0) {
		return -1;
	}

	entry.cachedTargetW = targetWidth;
	entry.cachedTargetH = targetHeight;
	entry.cachedRenderScale = renderScale;
	return entry.imageHandle;
}

bool drawSharedSvg(NVGcontext* vg,
                   const std::shared_ptr<window::Svg>& svg,
                   float x, float y, float w, float h,
                   float alpha) {
	if (!vg || !svg || w <= 0.f || h <= 0.f) {
		return false;
	}
	const int handle = getSharedSvgImage(vg, svg, w, h);
	if (handle <= 0) {
		return false;
	}

	NVGpaint paint = nvgImagePattern(vg, x, y, w, h, 0.f, handle, alpha);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	return true;
}

void onContextDestroy(NVGcontext* vg) {
	getCache().onContextDestroy(vg);
}

} // namespace shared_svg_cache
} // namespace visual_assets
