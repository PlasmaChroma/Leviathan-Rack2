#pragma once

#include "../plugin.hpp"
#include <memory>

namespace visual_assets {
namespace shared_svg_cache {

/**
 * Retrieves (or lazily rasterizes) a shared, context-owned NanoVG image for the given SVG.
 * If zoom or pixel ratio changes, the first consumer triggers an update and subsequent consumers
 * reuse the updated handle in that frame and future frames within the same rendering context.
 */
int getSharedSvgImage(NVGcontext* vg,
                      const std::shared_ptr<window::Svg>& svg,
                      float targetWidth,
                      float targetHeight);

/**
 * Draws the shared SVG image fitted to [x, y, w, h] with optional alpha.
 * Returns true if drawn from the shared cache; false if fallback is required.
 */
bool drawSharedSvg(NVGcontext* vg,
                   const std::shared_ptr<window::Svg>& svg,
                   float x, float y, float w, float h,
                   float alpha = 1.0f);

/**
 * Invalidate handles for a retiring context. Rack main-context teardown also invalidates
 * its framebuffer-context entries; alternating live contexts preserves both caches.
 */
void onContextDestroy(NVGcontext* vg);

} // namespace shared_svg_cache
} // namespace visual_assets
