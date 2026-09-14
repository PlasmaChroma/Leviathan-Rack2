#pragma once

#include "VisualAssets.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include <array>
#include "Eclipse2RuntimeBake.hpp"
#include "Eclipse2Track.hpp"
#include "ApertureBloomMasks.hpp"

// Session-only, opt-in experiment. Shared by all Eclipse2 controls on the UI thread.
namespace eclipse2_ring_cache {
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
inline bool prepare() {
 return raster().prepare([](const Widget::DrawArgs& args,void*) {
  eclipse2_track::draw(args,Vec(17,17),12.75f,.51f,-.83f*M_PI,.83f*M_PI);
 },nullptr);
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

// The candidate preserves the baseline's layer order and LED activation rules.
inline const std::array<Vec,25>& positions() {
 static const std::array<Vec,25> result=[](){std::array<Vec,25> value;const float lo=-.83f*M_PI,hi=.83f*M_PI;
  for(int i=0;i<25;++i){float a=lo+(float(i)/24.f)*(hi-lo);value[i]=Vec(std::sin(a),-std::cos(a));}return value;}();return result;
}
inline bool drawGlow(const Widget::DrawArgs& args,float x,float y,float r,float bloom) {
 auto* cache=aperture_bloom::findCache(args.vg,true);auto* mask=cache?cache->get(args.vg,r*.4f,r*3.2f):nullptr;
 if(!mask) return false;
 NVGcolor color=nvgRGBA(255,235,140,255);color.a*=bloom;
 aperture_bloom::draw(args.vg,*mask,x,y,color);return true;
}
struct Ring : Eclipse2Knob::ProgressLedRingWidget {
 void clearImages(NVGcontext* vg,bool destroy) {
  forgetWindow(vg);
  if(auto* cache=aperture_bloom::findCache(vg,false))cache->clear(vg,destroy);
  if(APP&&APP->window&&APP->window->vg==vg) {
   auto* nested=APP->window->fbVg;
   if(auto* cache=aperture_bloom::findCache(nested,false))cache->clear(nested,destroy);
  }
 }
 void onContextCreate(const ContextCreateEvent& e) override { raster().retryAfterContextCreate(); clearImages(e.vg,false);ProgressLedRingWidget::onContextCreate(e); }
 void onContextDestroy(const ContextDestroyEvent& e) override { clearImages(e.vg,true);ProgressLedRingWidget::onContextDestroy(e); }
void draw(const DrawArgs& args) override {
 if(!enabled() || box.size.x!=34.f || box.size.y!=34.f || numLeds!=25 || std::fabs(minAngle-float(-.83f*M_PI))>1e-6f || std::fabs(maxAngle-float(.83f*M_PI))>1e-6f) {
  ProgressLedRingWidget::draw(args);return;
 }
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	// LEDs are positioned slightly outside the bezel with a clean gap, scaled to fit inside bounds
	const float radiusPx = diameterPx * (45.0f / 120.f);
	const float largeRadiusPx = std::max(0.48f, diameterPx * (1.8f / 120.f));

	const float startNorm = bipolar ? centerNorm : 0.f;
	const float minLitNorm = std::min(startNorm, valueNorm);
	const float maxLitNorm = std::max(startNorm, valueNorm);
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	const float bloomLow = bloomRaw + 2.0f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	const float bloom = bloomLow * (1.0f + 1.40f * bloomRamp * bloomRamp);
	auto bloomColor = [&](NVGcolor color) {
		color.a *= bloom;
		return color;
	};

	nvgSave(args.vg);

 int track=prepare()?imageFor(args.vg):-1;
 if(track<=0) { nvgRestore(args.vg);ProgressLedRingWidget::draw(args);return; }
 nvgBeginPath(args.vg);nvgRect(args.vg,0,0,34,34);
 nvgFillPaint(args.vg,nvgImagePattern(args.vg,0,0,34,34,0,track,1));nvgFill(args.vg);
	// Active track background glow (linking the active LEDs together)
	const float activeStartAngle = (minAngle + minLitNorm * (maxAngle - minAngle)) - 0.5f * M_PI;
	const float activeEndAngle = (minAngle + maxLitNorm * (maxAngle - minAngle)) - 0.5f * M_PI;
	const float activeSweep = activeEndAngle - activeStartAngle;
	if (activeSweep > 0.008f && bloom > 0.001f) {
		// Wide soft glow bloom
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, activeStartAngle, activeEndAngle, NVG_CW);
		nvgStrokeColor(args.vg, bloomColor(nvgRGBA(255, 175, 40, 36)));
		nvgStrokeWidth(args.vg, largeRadiusPx * 6.5f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		// Tighter core glow bloom
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, activeStartAngle, activeEndAngle, NVG_CW);
		nvgStrokeColor(args.vg, bloomColor(nvgRGBA(255, 215, 95, 76)));
		nvgStrokeWidth(args.vg, largeRadiusPx * 4.2f);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);
	}

	for (int i = 0; i < numLeds; ++i) {
		const float ledNorm = float(i) / float(numLeds - 1);


		const float x = center.x + radiusPx * positions()[i].x;
		const float y = center.y + radiusPx * positions()[i].y;

		const float r = largeRadiusPx;

		bool active = false;
		if (bipolar) {
			active = (ledNorm >= minLitNorm && ledNorm <= maxLitNorm) && (std::fabs(valueNorm - centerNorm) > 0.005f);
		}
		else {
			active = (valueNorm > 0.f) && (ledNorm <= valueNorm);
		}

		if (active) {
			const float litR = r * 0.88f;

			if (bloom > 0.001f && !drawGlow(args,x,y,litR,bloom)) {
				// Glow aura (focused, brighter and more opaque, warm gold-orange)
				NVGpaint glow = nvgRadialGradient(
					args.vg,
					x, y,
					litR * 0.4f,
					litR * 3.2f,
					bloomColor(nvgRGBA(255, 235, 140, 255)),
					nvgRGBA(255, 110, 10, 0)
				);
				nvgBeginPath(args.vg);
				nvgCircle(args.vg, x, y, litR * 3.2f);
				nvgFillPaint(args.vg, glow);
				nvgFill(args.vg);
			}

			// Core LED dot (brighter warm gold-cream)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR);
			nvgFillColor(args.vg, nvgRGBA(255, 252, 200, 255));
			nvgFill(args.vg);

			// Intense central light source hotspot (pure white)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR * 0.55f);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
			nvgFill(args.vg);

			// Edge accent (matching active stroke, bright warm orange)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, litR);
			nvgStrokeColor(args.vg, nvgRGBA(255, 200, 50, 245));
			nvgStrokeWidth(args.vg, std::max(0.35f, diameterPx * (0.4f / 120.f)));
			nvgStroke(args.vg);
		}
		else {
			// Inactive dot (matching EclipseKnob inactive)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, r);
			nvgFillColor(args.vg, nvgRGBA(142, 124, 72, 118));
			nvgFill(args.vg);

			// Inactive outline (subtle warm gold-bronze border)
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, r);
			nvgStrokeColor(args.vg, nvgRGBA(80, 70, 40, 96));
			nvgStrokeWidth(args.vg, std::max(0.3f, diameterPx * (0.3f / 120.f)));
			nvgStroke(args.vg);
		}
	}

	nvgRestore(args.vg);
}

};
} // namespace eclipse2_ring_cache
