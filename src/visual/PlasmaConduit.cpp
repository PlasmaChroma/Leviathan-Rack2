#include "PlasmaConduit.hpp"

#include "../PanelSvgUtils.hpp"

#include <cmath>
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
		const NVGcolor electricYellow = nvgRGBA(226, 174, 5, 220);
		const NVGcolor electricAmber = nvgRGBA(184, 95, 0, 210);
		const NVGpaint plasmaGradient = nvgLinearGradient(
			args.vg,
			segment.start.x, segment.start.y,
			segment.end.x, segment.end.y,
			electricYellow, electricAmber);
		const NVGpaint coreGradient = nvgLinearGradient(
			args.vg,
			segment.start.x, segment.start.y,
			segment.end.x, segment.end.y,
			nvgRGBA(255, 245, 150, 232),
			nvgRGBA(242, 186, 70, 225));

		auto strokeColor = [&](float widthMm, NVGcolor color) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x, segment.start.y);
			nvgLineTo(args.vg, segment.end.x, segment.end.y);
			nvgStrokeWidth(args.vg, mm2px(widthMm));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokeColor(args.vg, color);
			nvgStroke(args.vg);
		};
		auto strokeGradient = [&](float widthMm, float alpha) {
			nvgSave(args.vg);
			nvgGlobalAlpha(args.vg, alpha);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x, segment.start.y);
			nvgLineTo(args.vg, segment.end.x, segment.end.y);
			nvgStrokeWidth(args.vg, mm2px(widthMm));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokePaint(args.vg, plasmaGradient);
			nvgStroke(args.vg);
			nvgRestore(args.vg);
		};
		auto strokeCoreGradient = [&](float widthMm, float alpha) {
			nvgSave(args.vg);
			nvgGlobalAlpha(args.vg, alpha);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x, segment.start.y);
			nvgLineTo(args.vg, segment.end.x, segment.end.y);
			nvgStrokeWidth(args.vg, mm2px(widthMm));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokePaint(args.vg, coreGradient);
			nvgStroke(args.vg);
			nvgRestore(args.vg);
		};

		strokeGradient(2.15f, 0.23f);
		strokeGradient(1.55f, 0.32f);
		strokeColor(1.12f, nvgRGBA(10, 14, 23, 212));
		strokeGradient(0.74f, 0.73f);
		strokeColor(0.40f, nvgRGBA(4, 9, 17, 158));
		strokeGradient(0.82f, 0.27f);
		strokeCoreGradient(0.62f, 0.31f);
		strokeCoreGradient(0.44f, 0.42f);
		strokeCoreGradient(0.29f, 0.69f);

		const Vec delta = segment.end.minus(segment.start);
		const float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
		const Vec tangent = length > 1e-4f
			? Vec(delta.x / length, delta.y / length)
			: Vec(0.f, 1.f);
		const Vec normal = length > 1e-4f
			? Vec(-delta.y / length, delta.x / length)
			: Vec(1.f, 0.f);
		// SVG anchors are free to be authored in either direction. Normalize the
		// visual direction so otherwise identical conduits do not bow opposite
		// ways merely because their path endpoints were reversed.
		Vec visualTangent = tangent;
		if ((std::fabs(tangent.y) >= std::fabs(tangent.x) && tangent.y < 0.f)
			|| (std::fabs(tangent.y) < std::fabs(tangent.x) && tangent.x < 0.f)) {
			visualTangent = tangent.mult(-1.f);
		}

		// Repeated bowed cell boundaries give the channel an engineered rhythm
		// similar to a segmented halo arc. They are authored in conduit-local
		// tangent/normal space, so the same treatment follows angled routes.
		const float endInset = mm2px(0.95f);
		const float boundaryPitch = mm2px(2.15f);
		if (length > 2.f * endInset) {
			const Vec ribOffset = normal.mult(mm2px(0.40f));
			const Vec ribBow = visualTangent.mult(mm2px(0.31f));
			const float startProjection =
				segment.start.x * visualTangent.x + segment.start.y * visualTangent.y;
			const float endProjection =
				segment.end.x * visualTangent.x + segment.end.y * visualTangent.y;
			const bool startIsVisualStart = startProjection <= endProjection;
			const Vec visualStart = startIsVisualStart ? segment.start : segment.end;
			const float visualStartProjection =
				startIsVisualStart ? startProjection : endProjection;
			const float visualEndProjection =
				startIsVisualStart ? endProjection : startProjection;
			// Phase the fixed pitch in panel space. Parallel conduits therefore
			// share the same boundaries even when their authored endpoints differ.
			float boundaryProjection = std::ceil(
				(visualStartProjection + endInset) / boundaryPitch) * boundaryPitch;
			for (; boundaryProjection <= visualEndProjection - endInset;
				boundaryProjection += boundaryPitch) {
				const Vec center = visualStart.plus(visualTangent.mult(
					boundaryProjection - visualStartProjection));

				auto strokeRib = [&](float visualOffsetMm, float widthMm, NVGcolor color) {
					const Vec shiftedCenter = center.plus(visualTangent.mult(mm2px(visualOffsetMm)));
					const Vec ribStart = shiftedCenter.minus(ribOffset);
					const Vec ribEnd = shiftedCenter.plus(ribOffset);
					const Vec ribControl = shiftedCenter.plus(ribBow);
					nvgBeginPath(args.vg);
					nvgMoveTo(args.vg, ribStart.x, ribStart.y);
					nvgQuadTo(args.vg, ribControl.x, ribControl.y, ribEnd.x, ribEnd.y);
					nvgStrokeWidth(args.vg, mm2px(widthMm));
					nvgLineCap(args.vg, NVG_ROUND);
					nvgStrokeColor(args.vg, color);
					nvgStroke(args.vg);
				};
				strokeRib(0.075f, 0.32f, nvgRGBA(255, 173, 18, 82));
				strokeRib(0.f, 0.18f, nvgRGBA(72, 30, 0, 232));
				strokeRib(0.075f, 0.075f, nvgRGBA(255, 246, 182, 242));
			}
		}

		auto strokeRail = [&](float offsetMm, float widthMm, NVGcolor color) {
			const Vec offset = normal.mult(mm2px(offsetMm));
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, segment.start.x + offset.x, segment.start.y + offset.y);
			nvgLineTo(args.vg, segment.end.x + offset.x, segment.end.y + offset.y);
			nvgStrokeWidth(args.vg, mm2px(widthMm));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStrokeColor(args.vg, color);
			nvgStroke(args.vg);
		};
		strokeRail(-0.28f, 0.075f, nvgRGBA(255, 248, 198, 168));
		strokeRail(0.28f, 0.09f, nvgRGBA(168, 82, 0, 162));

		strokeColor(0.56f, nvgRGBA(255, 244, 207, 25));
		strokeGradient(0.43f, 0.06f);

		for (Vec center : {segment.start, segment.end}) {
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, mm2px(0.82f));
			nvgFillPaint(args.vg, nvgRadialGradient(
				args.vg, center.x, center.y,
				0.f, mm2px(0.82f),
				nvgRGBA(255, 239, 117, 82),
				nvgRGBA(255, 126, 12, 0)));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, mm2px(0.46f));
			nvgStrokeWidth(args.vg, mm2px(0.08f));
			nvgStrokeColor(args.vg, nvgRGBA(196, 119, 0, 172));
			nvgStroke(args.vg);
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
