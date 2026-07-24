#include "Doorstop.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/VisualAssets.hpp"

#include <array>
#include <cmath>
#include <memory>

namespace {

constexpr float OVERFLOW_PAD = 42.f;
constexpr int SPRING_POINTS = 257;
constexpr float SPRING_BASE_Y_MM = 71.f;
constexpr float SPRING_LENGTH_MM = 49.f;
constexpr float SPRING_TURNS = 43.f;

struct DoorstopVisualSnapshot {
	float displacement = 0.f;
	float velocity = 0.f;
	float energy = 0.f;
	float strike = 0.f;
};

struct SpringPathGeometry {
	std::array<Vec, SPRING_POINTS> points {};
	float tipTravel = 0.f;
	float tipAngle = 0.f;
};

enum class SpringPathClipSide {
	None,
	Left,
	Right
};

struct SpringPathClip {
	SpringPathClipSide side = SpringPathClipSide::None;
	float boundaryX = 0.f;

	SpringPathClip() = default;
	SpringPathClip(SpringPathClipSide side, float boundaryX)
		: side(side), boundaryX(boundaryX) {
	}
};

struct DoorstopRenderMetrics {
	debug_terminal::UiTimingRangeAccumulator geometryIdleUs;
	debug_terminal::UiTimingRangeAccumulator geometryTrailUs;
	debug_terminal::UiTimingRangeAccumulator panelIdleUs;
	debug_terminal::UiTimingRangeAccumulator panelTrailUs;
	debug_terminal::UiTimingRangeAccumulator overflowIdleUs;
	debug_terminal::UiTimingRangeAccumulator overflowTrailUs;
};

struct DoorstopWidget;
struct DoorstopOverflowWidget;

struct DoorstopOverlayLink {
	DoorstopWidget* owner = nullptr;
	DoorstopOverflowWidget* overlay = nullptr;
	DoorstopVisualSnapshot snapshot;
	std::array<float, 3> displacementHistory {};
	std::array<SpringPathGeometry, 4> springGeometry {};
	bool trailGeometryValid = false;
	DoorstopRenderMetrics debugRenderMetrics;
};

float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

float visualTipTravel(float displacement) {
	return 48.f * std::tanh(displacement * 0.75f);
}

float bendProfile(float t) {
	return t * t * (3.f - 2.f * t);
}

struct SpringPointTemplate {
	float bend = 0.f;
	float bendDerivative = 0.f;
	float y = 0.f;
	float coilOffset = 0.f;
};

const std::array<SpringPointTemplate, SPRING_POINTS>& springPointTemplates() {
	static const std::array<SpringPointTemplate, SPRING_POINTS> templates = []() {
		std::array<SpringPointTemplate, SPRING_POINTS> result {};
		const float springLength = mm2px(SPRING_LENGTH_MM);
		for (int i = 0; i < SPRING_POINTS; ++i) {
			const float t = float(i) / float(SPRING_POINTS - 1);
			SpringPointTemplate& point = result[i];
			point.bend = bendProfile(t);
			point.bendDerivative = 6.f * t * (1.f - t);
			point.y = -springLength * t;
			const float coilRadius = 4.4f + (3.15f - 4.4f) * point.bend;
			point.coilOffset = coilRadius
				* std::sin(2.f * float(M_PI) * SPRING_TURNS * t);
		}
		return result;
	}();
	return templates;
}

void buildSpringGeometry(SpringPathGeometry& geometry, float displacement) {
	const float travel = visualTipTravel(displacement);
	const float springLength = mm2px(SPRING_LENGTH_MM);
	const auto& templates = springPointTemplates();
	for (int i = 0; i < SPRING_POINTS; ++i) {
		const SpringPointTemplate& point = templates[i];
		const float centerX = travel * point.bend;
		const float dxdt = travel * point.bendDerivative;
		const float dydt = -springLength;
		const float invLength = 1.f / std::max(std::sqrt(dxdt * dxdt + dydt * dydt), 1e-6f);
		const Vec normal(-dydt * invLength, dxdt * invLength);
		geometry.points[i] = Vec(centerX, point.y).plus(normal.mult(point.coilOffset));
	}
	geometry.tipTravel = travel;
	const float tangentT = 0.90f;
	const float dxdt = travel * 6.f * tangentT * (1.f - tangentT);
	geometry.tipAngle = std::atan2(-springLength, dxdt) + 0.5f * float(M_PI);
}

void appendFullSpringPath(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY) {
	nvgBeginPath(vg);
	nvgMoveTo(vg, baseX + geometry.points[0].x, baseY + geometry.points[0].y);
	for (int i = 1; i < SPRING_POINTS; ++i) {
		nvgLineTo(vg, baseX + geometry.points[i].x, baseY + geometry.points[i].y);
	}
}

bool pointInsideClip(float x, const SpringPathClip& clip) {
	if (clip.side == SpringPathClipSide::Left) {
		return x <= clip.boundaryX;
	}
	if (clip.side == SpringPathClipSide::Right) {
		return x >= clip.boundaryX;
	}
	return true;
}

void appendClippedSpringPath(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY, const SpringPathClip& clip) {
	if (clip.side == SpringPathClipSide::None) {
		appendFullSpringPath(vg, geometry, baseX, baseY);
		return;
	}

	nvgBeginPath(vg);
	bool pathOpen = false;
	for (int i = 1; i < SPRING_POINTS; ++i) {
		const Vec a = geometry.points[i - 1].plus(Vec(baseX, baseY));
		const Vec b = geometry.points[i].plus(Vec(baseX, baseY));
		const bool aInside = pointInsideClip(a.x, clip);
		const bool bInside = pointInsideClip(b.x, clip);
		if (aInside && bInside) {
			if (!pathOpen) {
				nvgMoveTo(vg, a.x, a.y);
			}
			nvgLineTo(vg, b.x, b.y);
			pathOpen = true;
			continue;
		}
		if (aInside == bInside) {
			pathOpen = false;
			continue;
		}

		const float dx = b.x - a.x;
		const float t = std::fabs(dx) > 1e-6f
			? clamp((clip.boundaryX - a.x) / dx, 0.f, 1.f)
			: 0.f;
		const Vec crossing = a.plus(b.minus(a).mult(t));
		if (aInside) {
			if (!pathOpen) {
				nvgMoveTo(vg, a.x, a.y);
			}
			nvgLineTo(vg, crossing.x, crossing.y);
			pathOpen = false;
		}
		else {
			nvgMoveTo(vg, crossing.x, crossing.y);
			nvgLineTo(vg, b.x, b.y);
			pathOpen = true;
		}
	}
}

void drawSpringBody(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY,
	float velocity, float alpha, bool drawCap, const SpringPathClip& clip = {}) {
	alpha = clamp01(alpha);
	appendClippedSpringPath(vg, geometry, baseX + 1.3f, baseY + 1.8f, clip);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, int(190.f * alpha)));
	nvgStrokeWidth(vg, 4.6f);
	nvgLineCap(vg, NVG_ROUND);
	nvgLineJoin(vg, NVG_ROUND);
	nvgStroke(vg);

	appendClippedSpringPath(vg, geometry, baseX, baseY, clip);
	NVGpaint metal = nvgLinearGradient(vg, baseX - 4.f, 0.f, baseX + 6.f, 0.f,
		nvgRGBA(78, 92, 105, int(255.f * alpha)),
		nvgRGBA(224, 238, 241, int(255.f * alpha)));
	nvgStrokePaint(vg, metal);
	nvgStrokeWidth(vg, 3.1f);
	nvgLineCap(vg, NVG_ROUND);
	nvgStroke(vg);

	appendClippedSpringPath(vg, geometry, baseX - 0.7f, baseY, clip);
	nvgStrokeColor(vg, nvgRGBA(206, 252, 255, int(130.f * alpha)));
	nvgStrokeWidth(vg, 0.85f);
	nvgStroke(vg);

	if (!drawCap) {
		return;
	}
	const float tipX = baseX + geometry.tipTravel;
	const float tipY = baseY - mm2px(SPRING_LENGTH_MM);
	const float motion = clamp01(std::fabs(velocity));

	nvgSave(vg);
	nvgTranslate(vg, tipX, tipY);
	nvgRotate(vg, geometry.tipAngle);
	if (motion > 0.55f) {
		nvgBeginPath(vg);
		nvgRoundedRect(vg, -5.2f - velocity * 4.f, -8.5f, 10.4f, 17.f, 4.8f);
		nvgFillColor(vg, nvgRGBA(94, 52, 132, int(65.f * alpha * motion)));
		nvgFill(vg);
	}
	nvgBeginPath(vg);
	nvgRoundedRect(vg, -5.4f, -8.5f, 10.8f, 17.f, 4.8f);
	NVGpaint rubber = nvgLinearGradient(vg, -5.f, -7.f, 5.f, 7.f,
		nvgRGBA(35, 30, 43, int(255.f * alpha)),
		nvgRGBA(111, 73, 137, int(255.f * alpha)));
	nvgFillPaint(vg, rubber);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(5, 7, 10, int(230.f * alpha)));
	nvgStrokeWidth(vg, 1.25f);
	nvgStroke(vg);
	nvgBeginPath(vg);
	nvgRoundedRect(vg, -2.9f, -6.2f, 2.2f, 9.5f, 1.1f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, int(80.f * alpha)));
	nvgFill(vg);
	nvgRestore(vg);
}

void drawSpringTrail(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY, float alpha, const SpringPathClip& clip = {}) {
	alpha = clamp01(alpha);
	appendClippedSpringPath(vg, geometry, baseX, baseY, clip);
	const NVGpaint trailMetal = nvgLinearGradient(vg, baseX - 4.f, 0.f, baseX + 6.f, 0.f,
		nvgRGBA(72, 86, 102, int(220.f * alpha)),
		nvgRGBA(213, 239, 245, int(245.f * alpha)));
	nvgStrokePaint(vg, trailMetal);
	nvgStrokeWidth(vg, 3.2f);
	nvgLineCap(vg, NVG_ROUND);
	nvgLineJoin(vg, NVG_ROUND);
	nvgStroke(vg);
}

void drawStrikeAccent(NVGcontext* vg, float baseX, float baseY, float strike) {
	strike = clamp01(strike);
	if (strike <= 0.002f) {
		return;
	}
	nvgBeginPath(vg);
	nvgCircle(vg, baseX, baseY, 15.f + strike * 10.f);
	NVGpaint glow = nvgRadialGradient(vg, baseX, baseY, 2.f, 23.f,
		nvgRGBA(255, 190, 76, int(100.f * strike)),
		nvgRGBA(91, 49, 198, 0));
	nvgFillPaint(vg, glow);
	nvgFill(vg);
}

void drawSpringScene(NVGcontext* vg, const DoorstopOverlayLink& link,
	float baseX, float baseY, const SpringPathClip& clip = {}) {
	const DoorstopVisualSnapshot& state = link.snapshot;
	const float trailAmount = clamp01((state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f);
	if (trailAmount > 0.01f && link.trailGeometryValid) {
		for (int i = 2; i >= 0; --i) {
			const float age = float(i + 1) / 4.f;
			drawSpringTrail(vg, link.springGeometry[i + 1], baseX, baseY,
				trailAmount * (0.17f - age * 0.08f), clip);
		}
	}
	drawStrikeAccent(vg, baseX, baseY, state.strike);
	drawSpringBody(vg, link.springGeometry[0], baseX, baseY, state.velocity, 1.f, true, clip);
}

class DoorstopHitWidget final : public app::Switch {
public:
	Doorstop* doorstopModule = nullptr;

	DoorstopHitWidget() {
		momentary = true;
	}

	void onButton(const event::Button& e) override {
		// Only left-click belongs to the invisible strike control. In particular,
		// leave right-click unconsumed so DoorstopWidget can open its module menu.
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
			return;
		}
		if (doorstopModule && e.action == GLFW_PRESS) {
			const float normalizedY = box.size.y > 0.f
				? clamp(e.pos.y / box.size.y, 0.f, 1.f)
				: 0.5f;
			const float velocity = doorstop::Engine::manualVelocityFromVerticalPosition(
				normalizedY);
			doorstopModule->pendingManualVelocity.store(velocity, std::memory_order_relaxed);
			doorstopModule->manualVelocityPending.store(true, std::memory_order_release);
		}
		app::Switch::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		(void) args;
	}
};

class DoorstopSpringWidget final : public TransparentWidget {
public:
	std::shared_ptr<DoorstopOverlayLink> link;

	void draw(const DrawArgs& args) override {
		if (!link) {
			return;
		}
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto sceneStart = debug_terminal::debugTimerStart(measurePerf);
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		drawSpringScene(args.vg, *link, box.size.x * 0.5f, mm2px(SPRING_BASE_Y_MM));
		nvgRestore(args.vg);
		if (measurePerf) {
			auto& range = link->trailGeometryValid
				? link->debugRenderMetrics.panelTrailUs
				: link->debugRenderMetrics.panelIdleUs;
			range.add(debug_terminal::elapsedUsSince(sceneStart));
		}
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		if (layer == 1) {
			draw(args);
		}
		TransparentWidget::drawLayer(args, layer);
	}
};

struct DoorstopOverflowWidget final : TransparentWidget {
	std::shared_ptr<DoorstopOverlayLink> link;

	~DoorstopOverflowWidget() override {
		if (link && link->overlay == this) {
			link->overlay = nullptr;
		}
	}

	void step() override;
	void draw(const DrawArgs& args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
};

struct DoorstopWidget final : ModuleWidget {
	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	std::shared_ptr<DoorstopOverlayLink> overlayLink;
	DoorstopSpringWidget* springWidget = nullptr;

	explicit DoorstopWidget(Doorstop* module) {
		setModule(module);
		overlayLink = std::make_shared<DoorstopOverlayLink>();
		overlayLink->owner = this;
		buildSpringGeometry(overlayLink->springGeometry[0], 0.f);

		PreviewBuildLogTimer previewBuildTimer("Doorstop", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/doorstop.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/doorstop.labels.svg");
		previewBuildTimer.markPanelDone();

		auto anchorPoint = [&](const char* id, const Vec& fallbackMm) {
			Vec result;
			if (!panel_svg::loadPointFromSvgMm(panelPath, id, &result)) {
				result = fallbackMm;
			}
			return result;
		};

		springWidget = new DoorstopSpringWidget();
		springWidget->box.size = box.size;
		springWidget->link = overlayLink;
		addChild(springWidget);

		math::Rect hitRectMm(Vec(1.1f, 17.3f), Vec(13.04f, 53.8f));
		panel_svg::loadRectFromSvgMm(panelPath, "MANUAL_HIT_REGION", &hitRectMm);
		auto* hit = createParam<DoorstopHitWidget>(mm2px(hitRectMm.pos), module, Doorstop::MANUAL_PARAM);
		hit->box.size = mm2px(hitRectMm.size);
		hit->doorstopModule = module;
		addParam(hit);

		const Vec lightMm = anchorPoint("STRIKE_LIGHT", Vec(7.62f, 80.5f));
		addChild(createLightCentered<SmallAperture<AmberApertureLight>>(
			mm2px(lightMm), module, Doorstop::STRIKE_LIGHT));

		const Vec trigMm = anchorPoint("TRIG_INPUT", Vec(7.62f, 93.f));
		const Vec velocityMm = anchorPoint("VELOCITY_INPUT", Vec(7.62f, 107.f));
		const Vec outputMm = anchorPoint("AUDIO_OUTPUT", Vec(7.62f, 121.f));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(trigMm), module, Doorstop::TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(velocityMm), module, Doorstop::VELOCITY_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outputMm), module, Doorstop::AUDIO_OUTPUT));

		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();
	}

	~DoorstopWidget() override {
		if (!overlayLink) {
			return;
		}
		overlayLink->owner = nullptr;
		destroyOverflowWidget();
	}

	bool validRackContext() const {
		return module && APP && APP->scene && APP->scene->rack
			&& parent == APP->scene->rack->getModuleContainer();
	}

	Vec overflowPosition() {
		auto* rack = APP->scene->rack;
		return getRelativeOffset(Vec(), rack).minus(Vec(OVERFLOW_PAD, 0.f));
	}

	void createOverflowWidget() {
		if (!overlayLink || overlayLink->overlay || !validRackContext()) {
			return;
		}
		auto* m = static_cast<Doorstop*>(module);
		if (!m->allowVisualOverflow.load(std::memory_order_relaxed)) {
			return;
		}
		auto* overlay = new DoorstopOverflowWidget();
		overlay->link = overlayLink;
		overlay->box.pos = overflowPosition();
		overlay->box.size = Vec(box.size.x + 2.f * OVERFLOW_PAD, box.size.y);

		// RackWidget assumes that every child of moduleContainer is a ModuleWidget.
		// Keep this decorative widget as a direct RackWidget child, immediately below
		// the cable layer, so it can cross panel bounds without corrupting that invariant.
		auto* rack = APP->scene->rack;
		auto* cableContainer = rack->getCableContainer();
		if (!cableContainer || !rack->hasChild(cableContainer)) {
			delete overlay;
			return;
		}
		overlayLink->overlay = overlay;
		rack->addChildBelow(overlay, cableContainer);
	}

	void destroyOverflowWidget() {
		if (!overlayLink || !overlayLink->overlay) {
			return;
		}
		DoorstopOverflowWidget* overlay = overlayLink->overlay;
		overlayLink->overlay = nullptr;
		if (overlay->parent) {
			overlay->requestDelete();
		}
		else {
			delete overlay;
		}
	}

	void step() override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::step();
		if (!overlayLink) {
			if (measurePerf) {
				debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
			}
			return;
		}
		DoorstopVisualSnapshot state;
		if (auto* m = static_cast<Doorstop*>(module)) {
			state.displacement = m->visualDisplacement.load(std::memory_order_relaxed);
			state.velocity = m->visualVelocity.load(std::memory_order_relaxed);
			state.energy = m->visualEnergy.load(std::memory_order_relaxed);
			state.strike = m->visualStrike.load(std::memory_order_relaxed);
		}
		overlayLink->snapshot = state;
		for (int i = int(overlayLink->displacementHistory.size()) - 1; i > 0; --i) {
			overlayLink->displacementHistory[i] = overlayLink->displacementHistory[i - 1];
		}
		overlayLink->displacementHistory[0] = state.displacement;
		const bool trailsActive = clamp01(
			(state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f) > 0.01f;
		const auto geometryStart = debug_terminal::debugTimerStart(measurePerf);
		buildSpringGeometry(overlayLink->springGeometry[0], state.displacement);
		overlayLink->trailGeometryValid = trailsActive;
		if (overlayLink->trailGeometryValid) {
			for (int i = 0; i < int(overlayLink->displacementHistory.size()); ++i) {
				buildSpringGeometry(
					overlayLink->springGeometry[i + 1],
					overlayLink->displacementHistory[i]);
			}
		}
		if (measurePerf) {
			auto& range = trailsActive
				? overlayLink->debugRenderMetrics.geometryTrailUs
				: overlayLink->debugRenderMetrics.geometryIdleUs;
			range.add(debug_terminal::elapsedUsSince(geometryStart));
		}

		auto* m = static_cast<Doorstop*>(module);
		const bool enabled = m && m->allowVisualOverflow.load(std::memory_order_relaxed);
		if (!enabled || !validRackContext()) {
			destroyOverflowWidget();
		}
		else if (!overlayLink->overlay) {
			createOverflowWidget();
		}
		if (measurePerf) {
			debugWidgetMetrics.recordStep(debug_terminal::elapsedUsSince(stepStart));
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::draw(args);
		auto* doorstop = static_cast<Doorstop*>(module);
		if (!doorstop) {
			return;
		}

		if (measurePerf) {
			debug_terminal::drawDebugInstanceId(args.vg, box.size, doorstop->debugMetrics.instanceId);
			debugWidgetMetrics.recordDraw(debug_terminal::elapsedUsSince(drawStart));

			const double nowSec = system::getTime();
			if (debug_terminal::baselineSubmitDue("Doorstop", doorstop->debugMetrics.instanceId, nowSec)) {
				debug_terminal::submitDoorstopMetrics(
					doorstop->debugMetrics.instanceId,
					doorstop->debugMetrics.consumeProcessRange(),
					debugWidgetMetrics.consumeStepRange(),
					debugWidgetMetrics.consumeDrawRange(),
					overlayLink->debugRenderMetrics.geometryIdleUs.consume(),
					overlayLink->debugRenderMetrics.geometryTrailUs.consume(),
					overlayLink->debugRenderMetrics.panelIdleUs.consume(),
					overlayLink->debugRenderMetrics.panelTrailUs.consume(),
					overlayLink->debugRenderMetrics.overflowIdleUs.consume(),
					overlayLink->debugRenderMetrics.overflowTrailUs.consume(),
					overlayLink->trailGeometryValid);
			}
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* m = dynamic_cast<Doorstop*>(module);
		if (!m) {
			return;
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Sound model", "", [m](Menu* modelMenu) {
			modelMenu->addChild(createCheckMenuItem("Probabilistic mix", "",
				[m]() {
					return m->soundModel.load(std::memory_order_relaxed)
						== int(doorstop::SoundModel::ProbabilisticMix);
				},
				[m]() {
					m->soundModel.store(int(doorstop::SoundModel::ProbabilisticMix), std::memory_order_relaxed);
				}));
			modelMenu->addChild(new MenuSeparator());
			modelMenu->addChild(createCheckMenuItem("Classic modal", "",
				[m]() {
					return m->soundModel.load(std::memory_order_relaxed)
						== int(doorstop::SoundModel::Classic);
				},
				[m]() {
					m->soundModel.store(int(doorstop::SoundModel::Classic), std::memory_order_relaxed);
				}));
			modelMenu->addChild(createCheckMenuItem("Coupled body", "",
				[m]() {
					return m->soundModel.load(std::memory_order_relaxed)
						== int(doorstop::SoundModel::CoupledBody);
				},
				[m]() {
					m->soundModel.store(int(doorstop::SoundModel::CoupledBody), std::memory_order_relaxed);
				}));
			modelMenu->addChild(createCheckMenuItem("Coil contact", "",
				[m]() {
					return m->soundModel.load(std::memory_order_relaxed)
						== int(doorstop::SoundModel::CoilContact);
				},
				[m]() {
					m->soundModel.store(int(doorstop::SoundModel::CoilContact), std::memory_order_relaxed);
				}));
			modelMenu->addChild(createCheckMenuItem("Dispersive spring", "",
				[m]() {
					return m->soundModel.load(std::memory_order_relaxed)
						== int(doorstop::SoundModel::DispersiveSpring);
				},
				[m]() {
					m->soundModel.store(int(doorstop::SoundModel::DispersiveSpring), std::memory_order_relaxed);
				}));
		}));
		const int breakInPercent = int(std::round(
			clamp01(m->serializedBreakIn.load(std::memory_order_relaxed)) * 100.f));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel(
			string::f("Break-in: %d%%", breakInPercent)));
		menu->addChild(createCheckMenuItem(
			"Lock break-in", "",
			[m]() {
				return m->breakInLocked.load(std::memory_order_relaxed);
			},
			[m]() {
				const bool locked =
					!m->breakInLocked.load(std::memory_order_relaxed);
				m->breakInLocked.store(locked, std::memory_order_relaxed);
			}));
		menu->addChild(createMenuItem(
			"Restore factory-fresh spring", "",
			[m]() {
				m->restoreSpringRequested.store(true, std::memory_order_release);
			}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createCheckMenuItem(
			"Extend spring beyond panel", "",
			[m]() { return m->allowVisualOverflow.load(std::memory_order_relaxed); },
			[this, m]() {
				const bool enabled = !m->allowVisualOverflow.load(std::memory_order_relaxed);
				m->allowVisualOverflow.store(enabled, std::memory_order_relaxed);
				if (enabled) createOverflowWidget();
				else destroyOverflowWidget();
			}));
	}
};

void DoorstopOverflowWidget::step() {
	TransparentWidget::step();
	if (!link || !link->owner || !link->owner->validRackContext()) {
		requestDelete();
		return;
	}
	box.pos = link->owner->overflowPosition();
	box.size = Vec(link->owner->box.size.x + 2.f * OVERFLOW_PAD, link->owner->box.size.y);
}

void DoorstopOverflowWidget::draw(const DrawArgs& args) {
	if (!link || !link->owner) {
		return;
	}
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto sceneStart = debug_terminal::debugTimerStart(measurePerf);
	const float moduleWidth = link->owner->box.size.x;
	const float baseX = OVERFLOW_PAD + 0.5f * moduleWidth;
	const float baseY = mm2px(SPRING_BASE_Y_MM);

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, OVERFLOW_PAD, box.size.y);
	drawSpringScene(args.vg, *link, baseX, baseY,
		{SpringPathClipSide::Left, OVERFLOW_PAD});
	nvgRestore(args.vg);

	nvgSave(args.vg);
	nvgScissor(args.vg, OVERFLOW_PAD + moduleWidth, 0.f, OVERFLOW_PAD, box.size.y);
	drawSpringScene(args.vg, *link, baseX, baseY,
		{SpringPathClipSide::Right, OVERFLOW_PAD + moduleWidth});
	nvgRestore(args.vg);
	if (measurePerf) {
		auto& range = link->trailGeometryValid
			? link->debugRenderMetrics.overflowTrailUs
			: link->debugRenderMetrics.overflowIdleUs;
		range.add(debug_terminal::elapsedUsSince(sceneStart));
	}
}

void DoorstopOverflowWidget::drawLayer(const DrawArgs& args, int layer) {
	if (layer == 1) {
		draw(args);
	}
	TransparentWidget::drawLayer(args, layer);
}

} // namespace

Model* modelDoorstop = createModel<Doorstop, DoorstopWidget>("Doorstop");
