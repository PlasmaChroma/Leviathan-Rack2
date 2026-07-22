#include "Doorstop.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/ApertureLight.hpp"
#include "visual/VisualAssets.hpp"

#include <array>
#include <cmath>
#include <memory>

namespace {

constexpr float OVERFLOW_PAD = 42.f;
constexpr int SPRING_POINTS = 97;
constexpr float SPRING_BASE_Y_MM = 71.f;

struct DoorstopVisualSnapshot {
	float displacement = 0.f;
	float velocity = 0.f;
	float energy = 0.f;
	float strike = 0.f;
};

struct DoorstopWidget;
struct DoorstopOverflowWidget;

struct DoorstopOverlayLink {
	DoorstopWidget* owner = nullptr;
	DoorstopOverflowWidget* overlay = nullptr;
	DoorstopVisualSnapshot snapshot;
	std::array<float, 3> displacementHistory {};
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

void appendSpringPath(NVGcontext* vg, float baseX, float baseY, float displacement) {
	const float travel = visualTipTravel(displacement);
	const float springLength = mm2px(57.f);
	constexpr float turns = 18.f;
	std::array<Vec, SPRING_POINTS> points {};

	for (int i = 0; i < SPRING_POINTS; ++i) {
		const float t = float(i) / float(SPRING_POINTS - 1);
		const float centerX = baseX + travel * bendProfile(t);
		const float centerY = baseY - springLength * t;
		const float dxdt = travel * 6.f * t * (1.f - t);
		const float dydt = -springLength;
		const float invLength = 1.f / std::max(std::sqrt(dxdt * dxdt + dydt * dydt), 1e-6f);
		const Vec normal(-dydt * invLength, dxdt * invLength);
		const float taper = 0.32f + 0.68f * std::sqrt(std::max(0.f, std::sin(float(M_PI) * t)));
		const float coil = std::sin(2.f * float(M_PI) * turns * t);
		points[i] = Vec(centerX, centerY).plus(normal.mult(3.6f * taper * coil));
	}

	nvgBeginPath(vg);
	nvgMoveTo(vg, points[0].x, points[0].y);
	for (int i = 1; i < SPRING_POINTS; ++i) {
		nvgLineTo(vg, points[i].x, points[i].y);
	}
}

void drawSpringBody(NVGcontext* vg, float baseX, float baseY, float displacement,
	float velocity, float alpha, bool drawCap) {
	alpha = clamp01(alpha);
	appendSpringPath(vg, baseX + 1.3f, baseY + 1.8f, displacement);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, int(190.f * alpha)));
	nvgStrokeWidth(vg, 4.6f);
	nvgLineCap(vg, NVG_ROUND);
	nvgLineJoin(vg, NVG_ROUND);
	nvgStroke(vg);

	appendSpringPath(vg, baseX, baseY, displacement);
	NVGpaint metal = nvgLinearGradient(vg, baseX - 4.f, 0.f, baseX + 6.f, 0.f,
		nvgRGBA(78, 92, 105, int(255.f * alpha)),
		nvgRGBA(224, 238, 241, int(255.f * alpha)));
	nvgStrokePaint(vg, metal);
	nvgStrokeWidth(vg, 3.1f);
	nvgLineCap(vg, NVG_ROUND);
	nvgStroke(vg);

	appendSpringPath(vg, baseX - 0.7f, baseY, displacement);
	nvgStrokeColor(vg, nvgRGBA(206, 252, 255, int(130.f * alpha)));
	nvgStrokeWidth(vg, 0.85f);
	nvgStroke(vg);

	if (!drawCap) {
		return;
	}
	const float travel = visualTipTravel(displacement);
	const float tipX = baseX + travel;
	const float tipY = baseY - mm2px(57.f);
	const float tangentT = 0.90f;
	const float dxdt = travel * 6.f * tangentT * (1.f - tangentT);
	const float angle = std::atan2(-mm2px(57.f), dxdt) + 0.5f * float(M_PI);
	const float motion = clamp01(std::fabs(velocity));

	nvgSave(vg);
	nvgTranslate(vg, tipX, tipY);
	nvgRotate(vg, angle);
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

void drawSpringScene(NVGcontext* vg, const DoorstopOverlayLink& link, float baseX, float baseY) {
	const DoorstopVisualSnapshot& state = link.snapshot;
	const float trailAmount = clamp01((state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f);
	if (trailAmount > 0.01f) {
		for (int i = 2; i >= 0; --i) {
			const float age = float(i + 1) / 4.f;
			drawSpringBody(vg, baseX, baseY, link.displacementHistory[i], state.velocity,
				trailAmount * (0.17f - age * 0.08f), false);
		}
	}
	drawStrikeAccent(vg, baseX, baseY, state.strike);
	drawSpringBody(vg, baseX, baseY, state.displacement, state.velocity, 1.f, true);
}

class DoorstopHitWidget final : public app::Switch {
public:
	DoorstopHitWidget() {
		momentary = true;
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
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		drawSpringScene(args.vg, *link, box.size.x * 0.5f, mm2px(SPRING_BASE_Y_MM));
		nvgRestore(args.vg);
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
};

struct DoorstopWidget final : ModuleWidget {
	std::shared_ptr<DoorstopOverlayLink> overlayLink;
	DoorstopSpringWidget* springWidget = nullptr;

	explicit DoorstopWidget(Doorstop* module) {
		setModule(module);
		overlayLink = std::make_shared<DoorstopOverlayLink>();
		overlayLink->owner = this;

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

		math::Rect hitRectMm(Vec(1.1f, 9.3f), Vec(13.04f, 61.8f));
		panel_svg::loadRectFromSvgMm(panelPath, "MANUAL_HIT_REGION", &hitRectMm);
		auto* hit = createParam<DoorstopHitWidget>(mm2px(hitRectMm.pos), module, Doorstop::MANUAL_PARAM);
		hit->box.size = mm2px(hitRectMm.size);
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

		addChild(createWidget<CyanOrbScrew>(Vec(0.f, 0.f)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH,
			RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
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
		ModuleWidget::step();
		if (!overlayLink) {
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

		auto* m = static_cast<Doorstop*>(module);
		const bool enabled = m && m->allowVisualOverflow.load(std::memory_order_relaxed);
		if (!enabled || !validRackContext()) {
			destroyOverflowWidget();
		}
		else if (!overlayLink->overlay) {
			createOverflowWidget();
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* m = dynamic_cast<Doorstop*>(module);
		if (!m) {
			return;
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createCheckMenuItem(
			"Allow spring to extend over adjacent modules", "",
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
	const float moduleWidth = link->owner->box.size.x;
	const float baseX = OVERFLOW_PAD + 0.5f * moduleWidth;
	const float baseY = mm2px(SPRING_BASE_Y_MM);

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, OVERFLOW_PAD, box.size.y);
	drawSpringScene(args.vg, *link, baseX, baseY);
	nvgRestore(args.vg);

	nvgSave(args.vg);
	nvgScissor(args.vg, OVERFLOW_PAD + moduleWidth, 0.f, OVERFLOW_PAD, box.size.y);
	drawSpringScene(args.vg, *link, baseX, baseY);
	nvgRestore(args.vg);
}

} // namespace

Model* modelDoorstop = createModel<Doorstop, DoorstopWidget>("Doorstop");
