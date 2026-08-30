#include "PlasmaConduit.hpp"

#include "../PanelSvgUtils.hpp"

#include <vector>

namespace visual_assets {

const char* const kPlasmaConduitAnchorGroupId = "plasma_conduit_anchors";

namespace {

struct PlasmaConduitSegment {
	Vec start;
	Vec end;
};

struct PlasmaConduitWidget final : Widget {
	std::vector<PlasmaConduitSegment> segments;

	void drawSegment(const DrawArgs& args, const PlasmaConduitSegment& segment) {
		auto stroke = [&](float widthMm, NVGcolor color) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x, segment.start.y);
			nvgLineTo(args.vg, segment.end.x, segment.end.y);
			nvgStrokeWidth(args.vg, mm2px(widthMm));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokeColor(args.vg, color);
			nvgStroke(args.vg);
		};

		// Halo trace: several translucent strokes approximate a broad feathered
		// bloom while the framebuffer keeps the cost out of steady-state drawing.
		stroke(2.60f, nvgRGBA(255, 132, 20, 12));
		stroke(1.85f, nvgRGBA(255, 145, 23, 20));
		stroke(1.25f, nvgRGBA(255, 158, 28, 35));
		stroke(0.88f, nvgRGBA(255, 171, 36, 58));

		// A restrained dark edge keeps the trace legible over light raster art.
		stroke(0.62f, nvgRGBA(67, 32, 3, 145));
		stroke(0.49f, nvgRGBA(255, 179, 51, 232));
		stroke(0.29f, nvgRGBA(255, 218, 125, 246));
		stroke(0.13f, nvgRGBA(255, 252, 225, 255));

		for (Vec center : {segment.start, segment.end}) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, mm2px(1.35f));
			nvgFillPaint(args.vg, nvgRadialGradient(
				args.vg, center.x, center.y,
				0.f, mm2px(1.35f),
				nvgRGBA(255, 245, 184, 150),
				nvgRGBA(255, 132, 18, 0)));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, mm2px(0.20f));
			nvgFillColor(args.vg, nvgRGBA(255, 253, 225, 245));
			nvgFill(args.vg);
		}
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);
		for (const PlasmaConduitSegment& segment : segments) {
			drawSegment(args, segment);
		}
		nvgRestore(args.vg);
	}
};

struct VisiblePlasmaConduitFramebuffer final : widget::FramebufferWidget {
	void step() override {
		if (!isVisible()) {
			return;
		}
		widget::FramebufferWidget::step();
	}
};

bool appendStraightSegments(
	const panel_svg::SvgPathMatch& path,
	std::vector<PlasmaConduitSegment>* segments) {
	if (!segments) {
		return false;
	}
	Vec current;
	bool hasCurrent = false;
	bool appended = false;
	for (const panel_svg::SvgPathCommand& command : path.commands) {
		if (command.type == panel_svg::SvgPathCommand::MoveTo) {
			current = command.p1;
			hasCurrent = true;
			continue;
		}
		if (command.type != panel_svg::SvgPathCommand::LineTo || !hasCurrent) {
			return false;
		}
		const Vec delta = command.p1.minus(current);
		if (delta.x * delta.x + delta.y * delta.y > 1e-8f) {
			segments->push_back({mm2px(current), mm2px(command.p1)});
			appended = true;
		}
		current = command.p1;
	}
	return appended;
}

} // namespace

widget::FramebufferWidget* createPlasmaConduitLayer(
	const std::string& panelSvgPath,
	Vec panelSizePx) {
	std::vector<panel_svg::SvgPathMatch> paths;
	if (!panel_svg::findPathsInGroupsWithIdSubstringMm(
		panelSvgPath, kPlasmaConduitAnchorGroupId, &paths)) {
		return nullptr;
	}

	PlasmaConduitWidget* conduits = new PlasmaConduitWidget();
	for (const panel_svg::SvgPathMatch& path : paths) {
		appendStraightSegments(path, &conduits->segments);
	}
	if (conduits->segments.empty()) {
		delete conduits;
		return nullptr;
	}

	VisiblePlasmaConduitFramebuffer* framebuffer =
		new VisiblePlasmaConduitFramebuffer();
	framebuffer->box.size = panelSizePx;
	framebuffer->dirtyOnSubpixelChange = false;
	conduits->box.size = panelSizePx;
	framebuffer->addChild(conduits);
	return framebuffer;
}

} // namespace visual_assets
