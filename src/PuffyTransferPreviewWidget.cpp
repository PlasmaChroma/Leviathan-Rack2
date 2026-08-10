#include "PuffyTransferPreviewWidget.hpp"

#include "PuffyDrawDiagnostics.hpp"
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

std::uint32_t visualSwarmHash(std::uint32_t value) {
	value ^= value >> 16;
	value *= 0x7feb352du;
	value ^= value >> 15;
	value *= 0x846ca68bu;
	value ^= value >> 16;
	return value;
}

float visualSwarmChaos(int pointIndex, int laneIndex, bool positive) {
	const std::uint32_t key = 0x51f15eadu
		^ std::uint32_t(pointIndex) * 0x9e3779b9u
		^ std::uint32_t(laneIndex + 1) * 0x85ebca6bu
		^ (positive ? 0xc2b2ae35u : 0x27d4eb2fu);
	const std::uint32_t bits = visualSwarmHash(key) >> 8;
	return float(bits) * (2.f / 16777216.f) - 1.f;
}

struct CurveSampleInput {
	float input;
	bool discontinuity;

	CurveSampleInput(float input, bool discontinuity)
		: input(input), discontinuity(discontinuity) {
	}
};

void appendVoidDiscontinuities(
	std::vector<CurveSampleInput>& samples,
	bool negative,
	float amount) {
	if (!(amount > 0.f)) {
		return;
	}
	const float amountSquared = amount * amount;
	const float railThreshold = 1.f / (1.f + 1.5f * amountSquared);
	const float stepSize = 0.125f * amountSquared * railThreshold;
	if (!(stepSize > 0.f)) {
		return;
	}
	const int internalBoundaryCount = std::max(
		0, int(std::ceil(railThreshold / stepSize)) - 1);
	const float boundaryTolerance = std::max(
		1e-6f, railThreshold * 1e-6f);
	// Below this scale the individual jumps are subpixel. Keep the ordinary
	// sampled curve rather than feeding thousands of invisible vertices to NVG.
	static constexpr int kMaximumVisibleBoundaries = 256;
	if (internalBoundaryCount > kMaximumVisibleBoundaries) {
		return;
	}
	for (int step = 1; step <= internalBoundaryCount; ++step) {
		const float magnitude = float(step) * stepSize;
		if (magnitude < railThreshold - boundaryTolerance) {
			samples.push_back({negative ? -magnitude : magnitude, true});
		}
	}
	samples.push_back({negative ? -railThreshold : railThreshold, true});
}

} // namespace

PuffyTransferPreviewWidget::PuffyTransferPreviewWidget(Puffy* module)
	: module(module) {
	visual.effectiveAmount = 0.25f;
	visual.positiveInputActivity = 0.35f;
	visual.negativeInputActivity = 0.35f;
	visual.negativeCharacter = int(puffy::Character::Bloom);
	visual.positiveCharacter = int(puffy::Character::Spine);
	visual.charactersLinked = false;

	curveFramebuffer = new widget::FramebufferWidget();
	curveFramebuffer->dirtyOnSubpixelChange = false;
	curveLayer = new PuffyTransferCurveLayer(this);
	curveFramebuffer->addChild(curveLayer);
	addChild(curveFramebuffer);
}

float PuffyTransferPreviewWidget::inputToX(float input) const {
	return (0.5f + 0.5f * clamp(input / NORMALIZED_DOMAIN, -1.f, 1.f)) * box.size.x;
}

float PuffyTransferPreviewWidget::outputToY(float output) const {
	const float usableHeight = box.size.y * (1.f - 2.f * kVerticalMarginFraction);
	return box.size.y * kVerticalMarginFraction
		+ (0.5f - 0.5f * clamp(output / NORMALIZED_DOMAIN, -1.f, 1.f)) * usableHeight;
}

void PuffyTransferPreviewWidget::rebuildPoints() {
	const bool measureDraw = isPuffyDrawMeasurementEnabled();
	PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
	PuffyScopedDrawTimer rebuildTimer(
		metrics.transferCurveRebuildNs, measureDraw);
	if (measureDraw) {
		++metrics.transferCurveRebuilds;
	}
	puffy::DynamicsState dynamics;
	dynamics.fast = clamp(visual.inputActivity, 0.f, 1.f);
	dynamics.transient = clamp(visual.transientActivity, 0.f, 1.f);
	const puffy::Character negativeCharacter = static_cast<puffy::Character>(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const puffy::Character positiveCharacter = static_cast<puffy::Character>(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));
	const bool hasSwarm = negativeCharacter == puffy::Character::Swarm
		|| positiveCharacter == puffy::Character::Swarm;
	const float rawAmount = clamp(visual.effectiveAmount, 0.f, 1.f);
	const int amountBin = clamp(int(rawAmount * 63.f + 0.5f), 0, 63);
	const float amount = hasSwarm ? float(amountBin) / 63.f : rawAmount;
	std::vector<CurveSampleInput> curveSamples;
	curveSamples.reserve(POINT_COUNT + 2 * 257);
	for (int i = 0; i < POINT_COUNT; ++i) {
		const float normalized = float(i) / float(POINT_COUNT - 1);
		const float input = -NORMALIZED_DOMAIN + 2.f * NORMALIZED_DOMAIN * normalized;
		curveSamples.push_back({input, false});
	}
	if (negativeCharacter == puffy::Character::Void) {
		appendVoidDiscontinuities(curveSamples, true, amount);
	}
	if (positiveCharacter == puffy::Character::Void) {
		appendVoidDiscontinuities(curveSamples, false, amount);
	}
	std::sort(
		curveSamples.begin(), curveSamples.end(),
		[](const CurveSampleInput& a, const CurveSampleInput& b) {
			if (a.input != b.input) {
				return a.input < b.input;
			}
			return a.discontinuity && !b.discontinuity;
		});
	curvePoints.clear();
	curvePoints.reserve(curveSamples.size() * 2);
	float lastDiscontinuity = -2.f * NORMALIZED_DOMAIN;
	for (const CurveSampleInput& sample : curveSamples) {
		if (!sample.discontinuity
			&& std::fabs(sample.input - lastDiscontinuity) < 1e-7f) {
			continue;
		}
		const puffy::Character character = sample.input < 0.f
			? negativeCharacter
			: positiveCharacter;
		if (sample.discontinuity) {
			// A single nextafter() is not always enough to cross the effective
			// integer boundary after VOID's reciprocal/multiply rounding. Probe a
			// subpixel distance on each side, but draw both limits at the exact edge.
			const float boundaryProbe = std::max(
				1e-6f, std::fabs(sample.input) * 1e-6f);
			const float before = sample.input - boundaryProbe;
			const float after = sample.input + boundaryProbe;
			const float beforeOutput = puffy::Engine::processCharacter(
				character, before, amount, dynamics);
			const float afterOutput = puffy::Engine::processCharacter(
				character, after, amount, dynamics);
			const float x = inputToX(sample.input);
			curvePoints.emplace_back(x, outputToY(beforeOutput));
			curvePoints.emplace_back(x, outputToY(afterOutput));
			lastDiscontinuity = sample.input;
		}
		else {
			const float output = puffy::Engine::processCharacter(
				character, sample.input, amount, dynamics);
			curvePoints.emplace_back(inputToX(sample.input), outputToY(output));
		}
	}
	swarmPointCount = 0;
	for (int column = 0; column < SWARM_COLUMN_COUNT; ++column) {
		const float normalized = float(column) / float(SWARM_COLUMN_COUNT - 1);
		const float input = -NORMALIZED_DOMAIN + 2.f * NORMALIZED_DOMAIN * normalized;
		const bool positive = input >= 0.f;
		const puffy::Character character = positive
			? positiveCharacter : negativeCharacter;
		if (character != puffy::Character::Swarm) {
			continue;
		}
		for (int lane = 0; lane < SWARM_SAMPLES_PER_COLUMN; ++lane) {
			const float output = puffy::Engine::processCharacter(
				character, input, amount, dynamics,
				visualSwarmChaos(column, lane, positive));
			swarmPoints[size_t(swarmPointCount++)] = Vec(
				inputToX(input), outputToY(output));
		}
	}
	pointsValid = true;
	lastAmount = amount;
	lastAmountBin = amountBin;
	lastFast = dynamics.fast;
	lastTransient = dynamics.transient;
	lastNegativeCharacter = int(negativeCharacter);
	lastPositiveCharacter = int(positiveCharacter);
	lastCurveSize = box.size;
	lastCurveRebuildTime = system::getTime();
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
		|| positiveCharacter == int(puffy::Character::Frenzy)
		|| negativeCharacter == int(puffy::Character::Teeth)
		|| positiveCharacter == int(puffy::Character::Teeth);
	const bool swarmActive =
		negativeCharacter == int(puffy::Character::Swarm)
		|| positiveCharacter == int(puffy::Character::Swarm);
	const int amountBin = clamp(
		int(clamp(visual.effectiveAmount, 0.f, 1.f) * 63.f + 0.5f), 0, 63);
	const bool sizeChanged =
		std::fabs(lastCurveSize.x - box.size.x) > 0.25f
		|| std::fabs(lastCurveSize.y - box.size.y) > 0.25f;
	const bool curveChanged =
		!pointsValid
		|| sizeChanged
		|| negativeCharacter != lastNegativeCharacter
		|| positiveCharacter != lastPositiveCharacter
		|| (swarmActive
			? amountBin != lastAmountBin
			: std::fabs(visual.effectiveAmount - lastAmount) > 0.001f)
		|| (frenzyReactive
			&& (std::fabs(visual.inputActivity - lastFast) > 0.01f
				|| std::fabs(visual.transientActivity - lastTransient) > 0.01f));
	const bool curveRebuildAllowed = curveChanged
		&& (lastCurveRebuildTime < 0.0
			|| system::getTime() - lastCurveRebuildTime >= (1.0 / 30.0));
	if (curveRebuildAllowed) {
		rebuildPoints();
	}

	TransparentWidget::step();
}

void PuffyTransferPreviewWidget::draw(const DrawArgs& args) {
	PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
	PuffyScopedDrawTimer drawTimer(
		metrics.transferDrawNs, isPuffyDrawMeasurementEnabled());
	const float width = box.size.x;
	const float height = box.size.y;
	if (width <= 1.f || height <= 1.f) {
		TransparentWidget::draw(args);
		return;
	}
	const float centerX = 0.5f * width;
	const float centerY = 0.5f * height;
	const NVGcolor negativeTint = puffy_visual::characterTint(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const NVGcolor positiveTint = puffy_visual::characterTint(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);
	const auto drawActivityLane = [&](float negativeActivity,
		float positiveActivity, float laneY, float laneHeight) {
		const float negativeX = inputToX(
			-clamp(negativeActivity, 0.f, NORMALIZED_DOMAIN));
		const float positiveX = inputToX(
			clamp(positiveActivity, 0.f, NORMALIZED_DOMAIN));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, negativeX, laneY, centerX - negativeX, laneHeight);
		nvgFillColor(args.vg, withAlpha(negativeTint, 0.18f));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, centerX, laneY, positiveX - centerX, laneHeight);
		nvgFillColor(args.vg, withAlpha(positiveTint, 0.18f));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, negativeX, laneY);
		nvgLineTo(args.vg, negativeX, laneY + laneHeight);
		nvgStrokeColor(args.vg, withAlpha(negativeTint, 0.38f));
		nvgStrokeWidth(args.vg, 0.8f);
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, positiveX, laneY);
		nvgLineTo(args.vg, positiveX, laneY + laneHeight);
		nvgStrokeColor(args.vg, withAlpha(positiveTint, 0.38f));
		nvgStrokeWidth(args.vg, 0.8f);
		nvgStroke(args.vg);

		// Each lane retains 25% headroom beyond the visible +/-5 V domain.
		const float negativeOverrange = clamp(
			(negativeActivity - NORMALIZED_DOMAIN) / 0.25f, 0.f, 1.f);
		const float positiveOverrange = clamp(
			(positiveActivity - NORMALIZED_DOMAIN) / 0.25f, 0.f, 1.f);
		if (negativeOverrange > 0.f) {
			const float stripWidth = 1.f + 2.f * negativeOverrange;
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.f, laneY, stripWidth, laneHeight);
			nvgFillColor(args.vg, withAlpha(
				negativeTint, 0.30f + 0.55f * negativeOverrange));
			nvgFill(args.vg);
		}
		if (positiveOverrange > 0.f) {
			const float stripWidth = 1.f + 2.f * positiveOverrange;
			nvgBeginPath(args.vg);
			nvgRect(
				args.vg, width - stripWidth, laneY, stripWidth, laneHeight);
			nvgFillColor(args.vg, withAlpha(
				positiveTint, 0.30f + 0.55f * positiveOverrange));
			nvgFill(args.vg);
		}
	};

	if (visual.stereoInputsConnected) {
		drawActivityLane(
			visual.leftNegativeInputActivity,
			visual.leftPositiveInputActivity,
			0.f,
			centerY);
		drawActivityLane(
			visual.rightNegativeInputActivity,
			visual.rightPositiveInputActivity,
			centerY,
			height - centerY);
	}
	else {
		drawActivityLane(
			visual.negativeInputActivity,
			visual.positiveInputActivity,
			0.f,
			height);
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
	PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
	PuffyScopedDrawTimer drawTimer(
		metrics.transferCurveDrawNs, isPuffyDrawMeasurementEnabled());
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
	nvgMoveTo(args.vg, inputToX(-NORMALIZED_DOMAIN), outputToY(-NORMALIZED_DOMAIN));
	nvgLineTo(args.vg, inputToX(NORMALIZED_DOMAIN), outputToY(NORMALIZED_DOMAIN));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 34));
	nvgStrokeWidth(args.vg, 0.65f);
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	for (std::size_t i = 0; i < curvePoints.size(); ++i) {
		if (i == 0) {
			nvgMoveTo(args.vg, curvePoints[i].x, curvePoints[i].y);
		}
		else {
			nvgLineTo(args.vg, curvePoints[i].x, curvePoints[i].y);
		}
	}
	NVGcolor negativeCurveTint = negativeTint;
	NVGcolor positiveCurveTint = positiveTint;
	negativeCurveTint.a = visual.negativeCharacter == int(puffy::Character::Swarm)
		? 0.56f : 0.94f;
	positiveCurveTint.a = visual.positiveCharacter == int(puffy::Character::Swarm)
		? 0.56f : 0.94f;
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

	if (swarmPointCount > 0) {
		nvgBeginPath(args.vg);
		for (int i = 0; i < swarmPointCount; ++i) {
			const Vec& point = swarmPoints[size_t(i)];
			nvgRect(args.vg, point.x - 0.55f, point.y - 0.55f, 1.1f, 1.1f);
		}
		nvgFillColor(args.vg, withAlpha(
			puffy_visual::characterTint(int(puffy::Character::Swarm)), 0.44f));
		nvgFill(args.vg);
	}

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
}
