#pragma once

#include <algorithm>
#include <vector>

struct NVGcontext;
namespace rack { namespace window { struct Svg; } }

namespace visual_assets {
namespace shared_svg_cache {

struct CacheEntry {
	const rack::window::Svg* svgKey = nullptr;
	// Stable lookup key, independent of upload success/reset of ownerVg.
	NVGcontext* renderVg = nullptr;
	NVGcontext* lifetimeVg = nullptr;
	NVGcontext* ownerVg = nullptr;
	// Logical target size in 1/8 px. The same SVG drawn at two sizes needs two
	// images: sharing one would re-rasterize it in place every frame and leave
	// the first draw's deferred NanoVG fill pointing at a replaced image.
	int sizeKeyW = 0;
	int sizeKeyH = 0;
	int imageHandle = -1;
	int cachedWidth = 0;
	int cachedHeight = 0;
	float cachedTargetW = 0.f;
	float cachedTargetH = 0.f;
	int cachedRenderScale = 0;
};

class CacheState {
	std::vector<CacheEntry> entries;

public:
	CacheEntry& findOrCreate(const rack::window::Svg* svg, NVGcontext* vg, NVGcontext* lifetimeVg,
		int sizeKeyW = 0, int sizeKeyH = 0) {
		for (auto& entry : entries) {
			if (entry.svgKey == svg && entry.renderVg == vg && entry.lifetimeVg == lifetimeVg
				&& entry.sizeKeyW == sizeKeyW && entry.sizeKeyH == sizeKeyH)
				return entry;
		}
		entries.emplace_back();
		auto& entry = entries.back();
		entry.svgKey = svg;
		entry.renderVg = vg;
		entry.lifetimeVg = lifetimeVg;
		entry.sizeKeyW = sizeKeyW;
		entry.sizeKeyH = sizeKeyH;
		return entry;
	}

	void onContextDestroy(NVGcontext* vg) {
		if (!vg) return;
		// Context teardown owns GPU deletion. Forget both main and framebuffer
		// handles without issuing calls against either retiring NanoVG context.
		entries.erase(std::remove_if(entries.begin(), entries.end(), [vg](const CacheEntry& entry) {
			return entry.renderVg == vg || entry.lifetimeVg == vg;
		}), entries.end());
	}
};

} // namespace shared_svg_cache
} // namespace visual_assets
