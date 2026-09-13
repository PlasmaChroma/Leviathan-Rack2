#pragma once

#include "../plugin.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include "../render/AdaptiveVisualUpdate.hpp"
#include <array>

namespace visual_assets {

// Marker/text/history are deliberately absent from this key.
struct ContourSettlement {
	std::array<float, 14> key{};
	bool valid = false;
	double changedAt = 0.0;

	bool observe(const std::array<float, 14>& next, double now) {
		if (!valid || key != next) {
			key = next;
			valid = true;
			changedAt = now;
			return true;
		}
		return false;
	}
	bool settled(double now) const { return valid && now - changedAt >= 0.1; }
};

// Owned normally in the widget tree for stepping and context events, but drawn
// explicitly by the preview between its history and live marker layers.
struct SettledContourFramebuffer : widget::FramebufferWidget {
	ContourSettlement settlement;
	// Opt-in: other users retain the original settlement behavior.
	int adaptivePressure = 0;
	leviathan::render::VisualUpdatePolicy updatePolicy;
	leviathan::render::VisualUpdateGate updateGate;
	bool pendingContour = false;

	SettledContourFramebuffer() { dirtyOnSubpixelChange = false; }
	void draw(const DrawArgs&) override {}

	void drawContour(const DrawArgs& args, const std::array<float, 14>& key, double now) {
		bool force = !settlement.valid;
		// Geometry may be stale briefly; styling, highlight, size and transform may not.
		for (size_t i = 2; i < key.size(); ++i)
			if (i != 8 && key[i] != settlement.key[i]) force = true;
		const bool changed = settlement.observe(key, now);
		pendingContour = pendingContour || changed;
		const bool adaptive = adaptivePressure > 0;
		if (!adaptive) {
			if (pendingContour) setDirty();
			pendingContour = false;
		}
		else if (force || !getFramebuffer()
			|| (pendingContour && (key[4] != 0.f
				|| updateGate.due(now, updatePolicy.interval(adaptivePressure))))) {
			setDirty();
			pendingContour = false;
		}
		float transform[6];
		nvgCurrentTransform(args.vg, transform);
		bypassed = (!adaptive && !settlement.settled(now)) || args.fb != nullptr
			|| transform[1] != 0.f || transform[2] != 0.f;
		if (!bypassed) {
			if (auto* framebuffer = getFramebuffer()) {
				// Rack owns allocation and context lifecycle. Never inspect an image
				// through a different NVG context; fall back to direct drawing there.
				if (framebuffer->ctx != args.vg) bypassed = true;
				else {
					const Vec size = getFramebufferSize();
					if (!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg,
						framebuffer->image, int(size.x), int(size.y))) {
						deleteFramebuffer();
						setDirty();
					}
				}
			}
		}
		if (!bypassed && dirty) {
			// Build once after settlement before compositing: Rack may otherwise
			// defer a dirty render and display the previous contour for a frame.
			const Vec offset(transform[4], transform[5]);
			render(Vec(transform[0], transform[3]), offset.minus(offset.floor()), args.clipBox);
		}
		if (!bypassed && !getFramebuffer()) bypassed = true;
		widget::FramebufferWidget::draw(args);
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		settlement.valid = false;
		widget::FramebufferWidget::onContextCreate(e);
	}
	void onContextDestroy(const ContextDestroyEvent& e) override {
		settlement.valid = false;
		widget::FramebufferWidget::onContextDestroy(e);
	}
};

} // namespace visual_assets
