#pragma once

#include "../WavePreviewTracer.hpp"

namespace visual_assets {

// Retains the stock tracer's ring order and geometry. Only opacity changes
// between captures. Rack owns the slot framebuffers and their context events.
template <size_t Points, size_t Frames>
struct SnapshotHistory : widget::Widget {
	using Tracer = WavePreviewTracer<Points, Frames>;
	struct Path : widget::Widget {
		typename Tracer::Frame frame;
		WavePreviewTracerStyle style;
		void draw(const DrawArgs& args) override {
			if (!frame.pointCount) return;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, frame.points[0].x, frame.points[0].y);
			for (size_t i = 1; i < frame.pointCount; ++i)
				nvgLineTo(args.vg, frame.points[i].x, frame.points[i].y);
			nvgStrokeColor(args.vg, style.color);
			nvgStrokeWidth(args.vg, style.lineWidth);
			nvgLineCap(args.vg, NVG_BUTT);
			nvgLineJoin(args.vg, NVG_ROUND);
			nvgStroke(args.vg);
		}
	};
	struct Slot : widget::FramebufferWidget {
		Path* path = nullptr;
		WavePreviewTracerDrawStats* stats = nullptr;
		std::array<float, 9> key {};
		bool valid = false;
		Slot() {
			dirtyOnSubpixelChange = false;
			path = new Path;
			addChild(path);
		}
		void draw(const DrawArgs&) override {}
		void drawFramebuffer() override {
			if (stats) ++stats->rasterizations;
			widget::FramebufferWidget::drawFramebuffer();
		}
		void present(const DrawArgs& args) {
			float t[6];
			nvgCurrentTransform(args.vg, t);
			bypassed = args.fb || t[1] != 0.f || t[2] != 0.f || t[0] <= 0.f || t[3] <= 0.f;
			if (auto* fb = getFramebuffer()) {
				if (fb->ctx != args.vg) bypassed = true;
				else if (!nvg_gfx_lifecycle::ownedNvgImageSizeMatches(args.vg, fb->image,
					int(getFramebufferSize().x), int(getFramebufferSize().y))) {
					deleteFramebuffer(); setDirty();
				}
			}
			if (!bypassed && dirty) {
				const Vec offset(t[4], t[5]);
				render(Vec(t[0], t[3]), offset.minus(offset.floor()), args.clipBox);
			}
			if (!bypassed && !getFramebuffer()) bypassed = true;
			widget::FramebufferWidget::draw(args);
		}
	};
	std::array<Slot*, Frames> slots {};
	SnapshotHistory() {
		for (auto& slot : slots) { slot = new Slot; addChild(slot); }
	}
	void draw(const DrawArgs&) override {}
	void drawHistory(const DrawArgs& args, const Tracer& tracer, double now,
	                 const WavePreviewTracerStyle& style, WavePreviewTracerDrawStats* stats) {
		float t[6]; nvgCurrentTransform(args.vg, t);
		const float pixelRatio = APP->window->pixelRatio;
		const std::array<float, 9> key {{box.size.x, box.size.y, t[0], t[3], pixelRatio,
			style.color.r, style.color.g, style.color.b, style.lineWidth}};
		for (size_t i = 0; i < Frames; ++i) {
			const auto& frame = tracer.frames[i];
			const float age = float(now - frame.birthSec);
			const float lifetime = std::max(style.fadeSec, 1e-6f);
			if (!frame.active || age < 0.f || age >= lifetime) continue;
			const int alpha = clamp(int(style.maxAlpha * (1.f - age / lifetime)), 0, 255);
			if (alpha <= 0) continue;
			auto* slot = slots[i];
			if (!slot->valid || slot->path->frame.birthSec != frame.birthSec || slot->key != key
				|| slot->path->style.color.a != style.color.a) {
				slot->path->frame = frame;
				slot->path->style = style;
				slot->box.size = slot->path->box.size = box.size;
				slot->key = key; slot->valid = true; slot->setDirty();
			}
			if (stats) { ++stats->trails; stats->points += frame.pointCount; }
			nvgSave(args.vg);
			nvgAlpha(args.vg, float(alpha) / 255.f);
			slot->stats = stats;
			slot->present(args);
			slot->stats = nullptr;
			nvgRestore(args.vg);
		}
	}
};

} // namespace visual_assets
