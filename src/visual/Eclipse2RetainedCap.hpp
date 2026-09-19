#pragma once

#include "VisualAssets.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include <array>
#include "Eclipse2RuntimeBake.hpp"
#include "Eclipse2Track.hpp"

// Session-only, opt-in experiment. Shared by all Eclipse2 controls on the UI thread.
namespace eclipse2_cap {
inline bool& requested() { static bool value = false; return value; }
inline bool enabled() { return isDragonKingDebugEnabled() && requested(); }
inline void setEnabled(bool value) { if (isDragonKingDebugEnabled()) requested() = value; }

constexpr int pixels = 136;
struct Image {
	NVGcontext* owner = nullptr;
	NVGcontext* window = nullptr;
	int handle = -1, width = 0, height = 0;
};
inline std::array<Image, 8>& images() { static std::array<Image, 8> value; return value; }

// Context events invalidate both the main and nested-framebuffer NanoVG images.
// Context teardown owns their destruction; never issue GL calls from destructors.
inline void forgetWindow(NVGcontext* vg) {
	for (auto& image : images()) {
		if (image.window != vg && image.owner != vg) continue;
		nvg_gfx_lifecycle::resetOwnedNvgImage(image.owner, image.handle,
			image.width, image.height, vg, false);
		image.window = nullptr;
	}
}

inline eclipse2_bake::Raster& raster() { static eclipse2_bake::Raster value; return value; }
inline const std::vector<unsigned char>& bakedPixels() { return raster().rgba; }
inline bool prepare(const std::shared_ptr<window::Svg>& svg) {
 if (!raster().rgba.empty()) return true;
 if (!svg || !svg->handle || raster().attempted) return false;
 widget::SvgWidget widget;widget.setSvg(svg);
 return raster().prepare([](const Widget::DrawArgs& args,void* user) {
  auto* widget=static_cast<widget::SvgWidget*>(user);
  const Vec size=widget->box.size;
  nvgTranslate(args.vg,17,17);const float scale=23.8f/std::max(size.x,size.y);
  nvgScale(args.vg,scale,scale);nvgTranslate(args.vg,-size.x*.5f,-size.y*.5f);
  widget->draw(args);
 },&widget);
}

inline int imageFor(NVGcontext* vg) {
	if (!vg || !APP || !APP->window) return -1;
	Image* selected = nullptr;
	for (auto& image : images()) if (image.owner == vg) { selected = &image; break; }
	if (!selected) for (auto& image : images()) if (!image.owner) { selected = &image; break; }
	// Bounded cache, no eviction of an image potentially referenced by queued draws.
	if (!selected) return -1;
	auto& image = *selected;
	if (image.owner == vg && nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, image.handle, pixels, pixels))
		return image.handle;
	const auto& data = bakedPixels();
	if (data.empty()) return -1;
	// A rejected handle could have been reassigned by the host. Forget, don't delete.
	nvg_gfx_lifecycle::resetOwnedNvgImage(image.owner, image.handle, image.width, image.height, vg, false);
	image.window = APP->window->vg;
	if (!nvg_gfx_lifecycle::updateOwnedNvgImageRgba(image.owner, image.handle,
		image.width, image.height, vg, pixels, pixels,
		NVG_IMAGE_PREMULTIPLIED | NVG_IMAGE_GENERATE_MIPMAPS, data.data())) return -1;
	return image.handle;
}

struct Layer : EclipseKnob::SvgLayer {
	std::shared_ptr<window::Svg> bakedSvg;

	void onContextCreate(const ContextCreateEvent& e) override {
		forgetWindow(e.vg);
		raster().retryAfterContextCreate();
		SvgLayer::onContextCreate(e);
	}
	void onContextDestroy(const ContextDestroyEvent& e) override {
		forgetWindow(e.vg);
		SvgLayer::onContextDestroy(e);
	}
	void draw(const DrawArgs& args) override {
		// The asset includes the original 0.70 scale and padding in a 34px square.
		// Preserve arbitrary replacement SVGs and resized control behavior via baseline.
		if (!enabled() || !svg || svg != bakedSvg || box.size.x != 34.f || box.size.y != 34.f
			|| scaleFactor != 0.70f) { SvgLayer::draw(args); return; }
		if (!prepare(bakedSvg)) { SvgLayer::draw(args); return; }
		const int image = imageFor(args.vg);
		if (image <= 0) { SvgLayer::draw(args); return; }
		const float angle = rotateWithValue ? crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f)) : 0.f;
		nvgSave(args.vg);
		nvgTranslate(args.vg, 17.f, 17.f);
		nvgRotate(args.vg, angle);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, -17.f, -17.f, 34.f, 34.f);
		nvgFillPaint(args.vg, nvgImagePattern(args.vg, -17.f, -17.f, 34.f, 34.f, 0.f, image, 1.f));
		nvgFill(args.vg);
		nvgRestore(args.vg);
	}
};
} // namespace eclipse2_cap
