#include "PlasmaConduit.hpp"

#include "../PanelSvgUtils.hpp"

#include <chrono>
#include <vector>

namespace visual_assets {

const char* const kPlasmaConduitAnchorGroupId = "plasma_conduit_anchors";

namespace {

struct PlasmaConduitSegment {
	panel_svg::SvgPathCommand::Type type = panel_svg::SvgPathCommand::LineTo;
	Vec start;
	Vec end;
	Vec control1;
	Vec control2;
};

struct PlasmaConduitWidget final : Widget {
	std::vector<PlasmaConduitSegment> segments;

	void drawSegment(const DrawArgs& args, const PlasmaConduitSegment& segment) {
		auto stroke = [&](float widthMm, NVGcolor color) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x, segment.start.y);
			switch (segment.type) {
			case panel_svg::SvgPathCommand::QuadTo:
				nvgQuadTo(args.vg, segment.control1.x, segment.control1.y, segment.end.x, segment.end.y);
				break;
			case panel_svg::SvgPathCommand::BezierTo:
				nvgBezierTo(args.vg, segment.control1.x, segment.control1.y,
					segment.control2.x, segment.control2.y, segment.end.x, segment.end.y);
				break;
			default:
				nvgLineTo(args.vg, segment.end.x, segment.end.y);
				break;
			}
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
	float* lastDrawUs = nullptr;

	void step() override {
		if (!isVisible()) {
			return;
		}
		widget::FramebufferWidget::step();
	}

	void drawFramebuffer() override {
		if (!lastDrawUs) {
			widget::FramebufferWidget::drawFramebuffer();
			return;
		}
		const auto start = std::chrono::steady_clock::now();
		widget::FramebufferWidget::drawFramebuffer();
		*lastDrawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - start).count()) * 1e-3f;
	}
};

bool appendConduitSegments(
	const panel_svg::SvgPathMatch& path,
	std::vector<PlasmaConduitSegment>* segments) {
	if (!segments) {
		return false;
	}
	const size_t originalSize = segments->size();
	Vec current;
	bool hasCurrent = false;
	bool appended = false;
	for (const panel_svg::SvgPathCommand& command : path.commands) {
		if (command.type == panel_svg::SvgPathCommand::MoveTo) {
			current = command.p1;
			hasCurrent = true;
			continue;
		}
		const bool line = command.type == panel_svg::SvgPathCommand::LineTo;
		const bool quad = command.type == panel_svg::SvgPathCommand::QuadTo;
		const bool cubic = command.type == panel_svg::SvgPathCommand::BezierTo;
		if (!hasCurrent || (!line && !quad && !cubic)) {
			// Centerlines are open paths. Reject an unsupported path atomically,
			// rather than leaving an accidentally truncated conduit on the panel.
			segments->resize(originalSize);
			return false;
		}
		const Vec end = line ? command.p1 : quad ? command.p2 : command.p3;
		auto distinct = [&](Vec point) {
			const Vec delta = point.minus(current);
			return delta.x * delta.x + delta.y * delta.y > 1e-8f;
		};
		// A curve can return to its starting point and still have visible length.
		if (distinct(end) || (!line && distinct(command.p1)) || (cubic && distinct(command.p2))) {
			PlasmaConduitSegment segment;
			segment.type = command.type;
			segment.start = mm2px(current);
			segment.end = mm2px(end);
			if (!line) segment.control1 = mm2px(command.p1);
			if (cubic) segment.control2 = mm2px(command.p2);
			segments->push_back(segment);
			appended = true;
		}
		current = end;
	}
	return appended;
}

} // namespace

widget::FramebufferWidget* createPlasmaConduitLayer(
	const std::string& panelSvgPath,
	Vec panelSizePx,
	float* lastDrawUs) {
	std::vector<panel_svg::SvgPathMatch> paths;
	if (!panel_svg::findPathsInGroupsWithIdSubstringMm(
		panelSvgPath, kPlasmaConduitAnchorGroupId, &paths)) {
		return nullptr;
	}

	PlasmaConduitWidget* conduits = new PlasmaConduitWidget();
	for (const panel_svg::SvgPathMatch& path : paths) {
		appendConduitSegments(path, &conduits->segments);
	}
	if (conduits->segments.empty()) {
		delete conduits;
		return nullptr;
	}

	VisiblePlasmaConduitFramebuffer* framebuffer =
		new VisiblePlasmaConduitFramebuffer();
	framebuffer->lastDrawUs = lastDrawUs;
	framebuffer->box.size = panelSizePx;
	framebuffer->dirtyOnSubpixelChange = false;
	conduits->box.size = panelSizePx;
	framebuffer->addChild(conduits);
	return framebuffer;
}

} // namespace visual_assets
