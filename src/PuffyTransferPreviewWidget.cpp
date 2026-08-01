#include "PuffyTransferPreviewWidget.hpp"

#include "PuffyVisualPalette.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kVerticalMarginFraction = 0.04f;

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
	visual.negativeCharacter = int(puffy::Character::Bloom);
	visual.positiveCharacter = int(puffy::Character::Bloom);

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
	const float usableHeight = box.size.y * (1.f - 2.f * kVerticalMarginFraction);
	return box.size.y * kVerticalMarginFraction
		+ (0.5f - 0.5f * clamp(output / DOMAIN, -1.f, 1.f)) * usableHeight;
}

void PuffyTransferPreviewWidget::rebuildPoints() {
	puffy::DynamicsState dynamics;
	dynamics.fast = clamp(visual.inputActivity, 0.f, 1.f);
	dynamics.transient = clamp(visual.transientActivity, 0.f, 1.f);
	const puffy::Character negativeCharacter = static_cast<puffy::Character>(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const puffy::Character positiveCharacter = static_cast<puffy::Character>(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));
	const float amount = clamp(visual.effectiveAmount, 0.f, 1.f);
	for (int i = 0; i < POINT_COUNT; ++i) {
		const float normalized = float(i) / float(POINT_COUNT - 1);
		const float input = -DOMAIN + 2.f * DOMAIN * normalized;
		const float output = puffy::Engine::processCharacter(
			input < 0.f ? negativeCharacter : positiveCharacter,
			input,
			amount,
			dynamics);
		points[size_t(i)] = Vec(inputToX(input), outputToY(output));
	}
	pointsValid = true;
	lastAmount = amount;
	lastFast = dynamics.fast;
	lastTransient = dynamics.transient;
	lastNegativeCharacter = int(negativeCharacter);
	lastPositiveCharacter = int(positiveCharacter);
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

	const int negativeCharacter = clamp(
		visual.negativeCharacter, 0, puffy::kCharacterCount - 1);
	const int positiveCharacter = clamp(
		visual.positiveCharacter, 0, puffy::kCharacterCount - 1);
	const bool frenzyReactive =
		negativeCharacter == int(puffy::Character::Frenzy)
		|| positiveCharacter == int(puffy::Character::Frenzy);
	const bool sizeChanged =
		std::fabs(lastCurveSize.x - box.size.x) > 0.25f
		|| std::fabs(lastCurveSize.y - box.size.y) > 0.25f;
	const bool curveChanged =
		!pointsValid
		|| sizeChanged
		|| negativeCharacter != lastNegativeCharacter
		|| positiveCharacter != lastPositiveCharacter
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
	const float negativeOverrange = clamp(
		(visual.negativeInputActivity - DOMAIN) / 0.25f, 0.f, 1.f);
	const float positiveOverrange = clamp(
		(visual.positiveInputActivity - DOMAIN) / 0.25f, 0.f, 1.f);
	const NVGcolor negativeTint = puffy_visual::characterTint(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const NVGcolor positiveTint = puffy_visual::characterTint(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, negativeX, 0.f, centerX - negativeX, height);
	nvgFillColor(args.vg, withAlpha(negativeTint, 0.13f));
	nvgFill(args.vg);
	nvgBeginPath(args.vg);
	nvgRect(args.vg, centerX, 0.f, positiveX - centerX, height);
	nvgFillColor(args.vg, withAlpha(positiveTint, 0.13f));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, negativeX, 0.f);
	nvgLineTo(args.vg, negativeX, height);
	nvgStrokeColor(args.vg, withAlpha(negativeTint, 0.38f));
	nvgStrokeWidth(args.vg, 0.8f);
	nvgStroke(args.vg);
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, positiveX, 0.f);
	nvgLineTo(args.vg, positiveX, height);
	nvgStrokeColor(args.vg, withAlpha(positiveTint, 0.38f));
	nvgStrokeWidth(args.vg, 0.8f);
	nvgStroke(args.vg);

	// The activity telemetry retains 25% headroom beyond the visible +/-5 V
	// domain. Convert that otherwise-clipped range into compact edge flashes.
	if (negativeOverrange > 0.f) {
		nvgBeginPath(args.vg);
		const float stripWidth = 1.f + 2.f * negativeOverrange;
		nvgRect(args.vg, 0.f, 0.f, stripWidth, height);
		nvgFillColor(args.vg, withAlpha(
			negativeTint, 0.30f + 0.55f * negativeOverrange));
		nvgFill(args.vg);
	}
	if (positiveOverrange > 0.f) {
		nvgBeginPath(args.vg);
		const float stripWidth = 1.f + 2.f * positiveOverrange;
		nvgRect(args.vg, width - stripWidth, 0.f, stripWidth, height);
		nvgFillColor(args.vg, withAlpha(
			positiveTint, 0.30f + 0.55f * positiveOverrange));
		nvgFill(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, centerX, 0.f);
	nvgLineTo(args.vg, centerX, height);
	nvgMoveTo(args.vg, 0.f, centerY);
	nvgLineTo(args.vg, width, centerY);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 58));
	nvgStrokeWidth(args.vg, 0.65f);
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
	const NVGcolor negativeTint = puffy_visual::characterTint(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const NVGcolor positiveTint = puffy_visual::characterTint(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));

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
	NVGcolor negativeCurveTint = negativeTint;
	NVGcolor positiveCurveTint = positiveTint;
	negativeCurveTint.a = 0.94f;
	positiveCurveTint.a = 0.94f;
	nvgStrokePaint(args.vg, nvgLinearGradient(
		args.vg,
		0.f,
		0.f,
		box.size.x,
		0.f,
		negativeCurveTint,
		positiveCurveTint));
	nvgStrokeWidth(args.vg, 1.35f);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgLineJoin(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
}
