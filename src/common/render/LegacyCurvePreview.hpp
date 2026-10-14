#pragma once

#include "../visual/SettledContourFramebuffer.hpp"
#include "../visual/SnapshotHistory.hpp"

namespace leviathan {
namespace render {

// Pilot adapter, not a new GL backend. Rack owns this subtree and delivers
// step/context events once to each existing cache. No private resource epoch,
// scheduler, or replacement framebuffer semantics are introduced here.
// Contour and history are drawn explicitly so the caller retains its existing
// timers, clipping, composition order, and live marker/label path.
template <size_t Points, size_t Frames,
          typename Contour = visual_assets::SettledContourFramebuffer,
          typename History = visual_assets::SnapshotHistory<Points, Frames>>
struct LegacyCurvePreview : rack::widget::Widget {
	Contour* const contour;
	History* const history;
	rack::widget::Widget* const contourContent;

	// Takes ownership of unattached widgets through Rack's normal child tree.
	LegacyCurvePreview(Contour* contour, rack::widget::Widget* content)
		: contour(contour), history(new History), contourContent(content) {
		contour->addChild(content);
		addChild(contour);
		addChild(history);
	}

	void setExtent(rack::math::Vec size) {
		box.size = contour->box.size = history->box.size = contourContent->box.size = size;
	}

	// Automatic traversal must not composite these layers a second time.
	void draw(const DrawArgs&) override {}

	void drawContour(const DrawArgs& args, const std::array<float, 14>& key, double now) {
		// Preserve the exact legacy key and settlement policy. Marker, frequency,
		// and history age are intentionally absent from this input.
		contour->drawContour(args, key, now);
	}

	void drawHistory(const DrawArgs& args, const WavePreviewTracer<Points, Frames>& tracer,
	                 double now, const WavePreviewTracerStyle& style,
	                 WavePreviewTracerDrawStats* stats) {
		history->drawHistory(args, tracer, now, style, stats);
	}
};

} // namespace render
} // namespace leviathan
