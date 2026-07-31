#include "PuffyTransferPreviewWidget.hpp"

#include "PuffyVisualPalette.hpp"

#include <algorithm>
#include <cmath>

namespace {

struct PuffyTransferCurveLayer final : TransparentWidget {
	PuffyTransferPreviewWidget* owner = nullptr;

	explicit PuffyTransferCurveLayer(PuffyTransferPreviewWidget* owner)
		: owner(owner) {
	}

	void draw(const DrawArgs& args) override {
		if (owner) {
			owner->drawCurve(args);
		}
	}
};

NVGcolor withAlpha(NVGcolor color, float alpha) {
	color.a = clamp(alpha, 0.f, 1.f);
	return color;
}

} // namespace

PuffyTransferPreviewWidget::PuffyTransferPreviewWidget(Puffy* module)
	: module(module) {
	visual.effectiveAmount = 0.25f;
	visual.positiveInputActivity = 0.35f;
	visual.negativeInputActivity = 0.35f;
	visual.character = int(puffy::Character::Bloom);

	curveFramebuffer = new widget::FramebufferWidget();
	curveFramebuffer->dirtyOnSubpixelChange = false;
	curveLayer = new PuffyTransferCurveLayer(this);
	curveFramebuffer->addChild(curveLayer);
	addChild(curveFramebuffer);
}

float PuffyTransferPreviewWidget::inputToX(float input) const {
	return (0.5f + 0.5f * clamp(input / DOMAIN, -1.f, 1.f)) * box.size.x;
}

float PuffyTransferPreviewWidget::outputToY(float output) const {
	return (0.5f - 0.5f * clamp(output / DOMAIN, -1.f, 1.f)) * box.size.y;
}

void PuffyTransferPreviewWidget::rebuildPoints() {
	puffy::DynamicsState dynamics;
	dynamics.fast = clamp(visual.inputActivity, 0.f, 1.f);
	dynamics.transient = clamp(visual.transientActivity, 0.f, 1.f);
	const puffy::Character character = static_cast<puffy::Character>(
		clamp(visual.character, 0, 3));
	const float amount = clamp(visual.effectiveAmount, 0.f, 1.f);
	for (int i = 0; i < POINT_COUNT; ++i) {
		const float normalized = float(i) / float(POINT_COUNT - 1);
		const float input = -DOMAIN + 2.f * DOMAIN * normalized;
		const float output = puffy::Engine::processCharacter(
			character, input, amount, dynamics);
		points[size_t(i)] = Vec(inputToX(input), outputToY(output));
	}
	pointsValid = true;
	lastAmount = amount;
	lastFast = dynamics.fast;
	lastTransient = dynamics.transient;
	lastCharacter = int(character);
	lastCurveSize = box.size;
	if (curveFramebuffer) {
		curveFramebuffer->setDirty();
	}
}

void PuffyTransferPreviewWidget::step() {
	if (module) {
		PuffyVisualState snapshot;
		if (module->readVisualState(&snapshot)) {
			visual = snapshot;
		}
	}

	if (curveFramebuffer) {
		curveFramebuffer->box.size = box.size;
	}
	if (curveLayer) {
		curveLayer->box.size = box.size;
	}

	const int character = clamp(visual.character, 0, 3);
	const bool frenzyReactive = character == int(puffy::Character::Frenzy);
	const bool sizeChanged =
		std::fabs(lastCurveSize.x - box.size.x) > 0.25f
		|| std::fabs(lastCurveSize.y - box.size.y) > 0.25f;
	const bool curveChanged =
		!pointsValid
		|| sizeChanged
		|| character != lastCharacter
		|| std::fabs(visual.effectiveAmount - lastAmount) > 0.001f
		|| (frenzyReactive
			&& (std::fabs(visual.inputActivity - lastFast) > 0.01f
				|| std::fabs(visual.transientActivity - lastTransient) > 0.01f));
	if (curveChanged) {
		rebuildPoints();
	}

	TransparentWidget::step();
}

void PuffyTransferPreviewWidget::draw(const DrawArgs& args) {
	const float width = box.size.x;
	const float height = box.size.y;
	if (width <= 1.f || height <= 1.f) {
		TransparentWidget::draw(args);
		return;
	}
	const float centerX = 0.5f * width;
	const float centerY = 0.5f * height;
	const float negativeX = inputToX(
		-clamp(visual.negativeInputActivity, 0.f, DOMAIN));
	const float positiveX = inputToX(
		clamp(visual.positiveInputActivity, 0.f, DOMAIN));
	const NVGcolor tint = puffy_visual::characterTint(
		clamp(visual.character, 0, 3));

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, negativeX, 0.f, centerX - negativeX, height);
	nvgRect(args.vg, centerX, 0.f, positiveX - centerX, height);
	nvgFillColor(args.vg, withAlpha(tint, 0.13f));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, negativeX, 0.f);
	nvgLineTo(args.vg, negativeX, height);
	nvgMoveTo(args.vg, positiveX, 0.f);
	nvgLineTo(args.vg, positiveX, height);
	nvgStrokeColor(args.vg, withAlpha(tint, 0.38f));
	nvgStrokeWidth(args.vg, 0.8f);
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, centerX, 0.f);
	nvgLineTo(args.vg, centerX, height);
	nvgMoveTo(args.vg, 0.f, centerY);
	nvgLineTo(args.vg, width, centerY);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 58));
	nvgStrokeWidth(args.vg, 0.65f);
	nvgStroke(args.vg);

	// +/-5 V ticks inside the +/-6.25 V plot domain.
	const float negativeFiveX = inputToX(-1.f);
	const float positiveFiveX = inputToX(1.f);
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, negativeFiveX, centerY - 2.f);
	nvgLineTo(args.vg, negativeFiveX, centerY + 2.f);
	nvgMoveTo(args.vg, positiveFiveX, centerY - 2.f);
	nvgLineTo(args.vg, positiveFiveX, centerY + 2.f);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 80));
	nvgStrokeWidth(args.vg, 0.7f);
	nvgStroke(args.vg);

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);

	// Keep the cached transfer curve above the live activity fill and axes.
	TransparentWidget::draw(args);
}

void PuffyTransferPreviewWidget::drawCurve(const DrawArgs& args) const {
	if (!pointsValid || box.size.x <= 1.f || box.size.y <= 1.f) {
		return;
	}
	const NVGcolor tint = puffy_visual::characterTint(
		clamp(visual.character, 0, 3));

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, inputToX(-DOMAIN), outputToY(-DOMAIN));
	nvgLineTo(args.vg, inputToX(DOMAIN), outputToY(DOMAIN));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 34));
	nvgStrokeWidth(args.vg, 0.65f);
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	for (int i = 0; i < POINT_COUNT; ++i) {
		if (i == 0) {
			nvgMoveTo(args.vg, points[size_t(i)].x, points[size_t(i)].y);
		}
		else {
			nvgLineTo(args.vg, points[size_t(i)].x, points[size_t(i)].y);
		}
	}
	nvgStrokeColor(args.vg, withAlpha(tint, 0.94f));
	nvgStrokeWidth(args.vg, 1.35f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
}
