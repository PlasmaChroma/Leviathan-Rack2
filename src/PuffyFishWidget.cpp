#include "PuffyFishWidget.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kPi = 3.14159265358979323846f;

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

} // namespace

PuffyFishWidget::PuffyFishWidget(Puffy* module)
	: module(module) {
	visual.effectiveAmount = 0.25f;
	visual.character = 0;
	controller.reset(visual);
	controller.update(0.f, visual, &pose);
}

void PuffyFishWidget::step() {
	TransparentWidget::step();
	if (!module || !isVisible()) {
		return;
	}
	PuffyVisualState snapshot;
	if (module->readVisualState(&snapshot)) {
		visual = snapshot;
	}
	const float frameTime = APP && APP->window
		? clamp(float(APP->window->getLastFrameDuration()), 0.f, 0.1f)
		: 1.f / 60.f;
	updateAccumulator += frameTime;
	constexpr float updateInterval = 1.f / 30.f;
	if (updateAccumulator >= updateInterval) {
		const float dt = std::min(updateAccumulator, 1.f / 15.f);
		updateAccumulator = 0.f;
		controller.update(dt, visual, &pose);
	}
}

void PuffyFishWidget::drawFin(
	NVGcontext* vg,
	Vec center,
	float bodyRadius,
	bool left,
	float angle) const {
	nvgSave(vg);
	const float side = left ? -1.f : 1.f;
	const Vec root(
		center.x + side * bodyRadius * 0.83f,
		center.y + bodyRadius * 0.10f);
	nvgTranslate(vg, root.x, root.y);
	nvgRotate(vg, side * angle);
	nvgScale(vg, side, 1.f);
	const float length = bodyRadius * 0.45f;
	const float height = bodyRadius * 0.37f;
	nvgBeginPath(vg);
	nvgMoveTo(vg, 0.f, -height * 0.34f);
	nvgBezierTo(
		vg, length * 0.38f, -height * 0.72f,
		length * 0.92f, -height * 0.48f,
		length, 0.f);
	nvgBezierTo(
		vg, length * 0.82f, height * 0.54f,
		length * 0.30f, height * 0.58f,
		0.f, height * 0.28f);
	nvgClosePath(vg);
	const NVGpaint fill = nvgLinearGradient(
		vg, 0.f, -height, length, height,
		nvgRGB(255, 224, 99), nvgRGB(255, 166, 8));
	nvgFillPaint(vg, fill);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(148, 85, 4, 190));
	nvgStrokeWidth(vg, 0.8f);
	nvgStroke(vg);
	for (int i = 1; i <= 3; ++i) {
		const float y = height * (-0.28f + 0.15f * float(i));
		nvgBeginPath(vg);
		nvgMoveTo(vg, length * 0.08f, y);
		nvgBezierTo(
			vg, length * 0.38f, y * 1.4f,
			length * 0.72f, y * 0.8f,
			length * 0.91f, 0.f);
		nvgStrokeColor(vg, nvgRGBA(196, 116, 3, 150));
		nvgStrokeWidth(vg, 0.65f);
		nvgStroke(vg);
	}
	nvgRestore(vg);
}

void PuffyFishWidget::drawEye(
	NVGcontext* vg,
	Vec center,
	float radius,
	float gazeX,
	float gazeY,
	float blink) const {
	const float open = std::max(0.04f, 1.f - clamp01(blink));
	nvgSave(vg);
	nvgTranslate(vg, center.x, center.y);
	nvgScale(vg, 0.86f, open);
	nvgBeginPath(vg);
	nvgCircle(vg, 0.f, 0.f, radius);
	const NVGpaint white = nvgRadialGradient(
		vg, -radius * 0.28f, -radius * 0.34f,
		radius * 0.08f, radius,
		nvgRGB(255, 255, 239), nvgRGB(255, 226, 147));
	nvgFillPaint(vg, white);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(121, 73, 8, 210));
	nvgStrokeWidth(vg, 0.8f);
	nvgStroke(vg);
	nvgRestore(vg);

	if (open > 0.18f) {
		const Vec pupil(
			center.x + clamp(gazeX, -1.f, 1.f) * radius * 0.23f,
			center.y + clamp(gazeY, -1.f, 1.f) * radius * 0.18f);
		nvgBeginPath(vg);
		nvgCircle(vg, pupil.x, pupil.y, radius * 0.49f);
		const NVGpaint iris = nvgRadialGradient(
			vg, pupil.x - radius * 0.12f, pupil.y - radius * 0.15f,
			radius * 0.06f, radius * 0.50f,
			nvgRGB(255, 194, 27), nvgRGB(28, 25, 22));
		nvgFillPaint(vg, iris);
		nvgFill(vg);
		nvgBeginPath(vg);
		nvgCircle(vg, pupil.x, pupil.y, radius * 0.25f);
		nvgFillColor(vg, nvgRGB(22, 22, 22));
		nvgFill(vg);
		nvgBeginPath(vg);
		nvgCircle(
			vg, pupil.x - radius * 0.15f, pupil.y - radius * 0.19f,
			radius * 0.10f);
		nvgFillColor(vg, nvgRGBA(255, 255, 255, 245));
		nvgFill(vg);
	}
	if (open < 0.16f) {
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x - radius * 0.72f, center.y);
		nvgQuadTo(
			vg, center.x, center.y + radius * 0.22f,
			center.x + radius * 0.72f, center.y);
		nvgStrokeColor(vg, nvgRGBA(116, 69, 5, 230));
		nvgStrokeWidth(vg, 1.1f);
		nvgStroke(vg);
	}
}

void PuffyFishWidget::draw(const DrawArgs& args) {
	TransparentWidget::draw(args);
	const float width = box.size.x;
	const float height = box.size.y;
	const float minimum = std::min(width, height);
	const Vec center(
		width * 0.5f,
		height * 0.53f + pose.verticalOffset * minimum);

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);

	nvgBeginPath(args.vg);
	nvgEllipse(
		args.vg, center.x, center.y + minimum * 0.29f,
		minimum * 0.25f, minimum * 0.045f);
	nvgFillColor(args.vg, nvgRGBA(5, 6, 15, 100));
	nvgFill(args.vg);

	const float inflation = clamp01(pose.inflation);
	const float radius = minimum * (0.255f + 0.082f * inflation);
	const float radiusX = radius * (0.88f + 0.12f * inflation)
		* (1.f + pose.squashX);
	const float radiusY = radius * (0.93f + 0.07f * inflation)
		* (1.f + pose.squashY);

	drawFin(args.vg, center, radiusX, true, pose.leftFinAngle);
	drawFin(args.vg, center, radiusX, false, pose.rightFinAngle);

	const int spineCount = 24;
	for (int i = 0; i < spineCount; ++i) {
		const float angle = 2.f * kPi * float(i) / float(spineCount);
		const float cosine = std::cos(angle);
		const float sine = std::sin(angle);
		const Vec root(
			center.x + cosine * radiusX * 0.91f,
			center.y + sine * radiusY * 0.91f);
		const float length =
			minimum * (0.028f + 0.022f * pose.spineExtension)
			* (0.86f + 0.14f * std::sin(float(i) * 2.17f));
		const float halfWidth = minimum * 0.012f;
		const Vec tangent(-sine * halfWidth, cosine * halfWidth);
		const Vec tip(root.x + cosine * length, root.y + sine * length);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, root.x + tangent.x, root.y + tangent.y);
		nvgQuadTo(
			args.vg, tip.x + tangent.x * 0.18f,
			tip.y + tangent.y * 0.18f,
			tip.x, tip.y);
		nvgQuadTo(
			args.vg, tip.x - tangent.x * 0.18f,
			tip.y - tangent.y * 0.18f,
			root.x - tangent.x, root.y - tangent.y);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, nvgRGB(255, 187, 22));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(151, 88, 2, 180));
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStroke(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgEllipse(args.vg, center.x, center.y, radiusX, radiusY);
	const NVGpaint body = nvgRadialGradient(
		args.vg,
		center.x - radiusX * 0.32f,
		center.y - radiusY * 0.38f,
		radius * 0.08f,
		radius * 1.18f,
		nvgRGB(255, 244, 174),
		nvgRGB(255, 174, 12));
	nvgFillPaint(args.vg, body);
	nvgFill(args.vg);
	nvgStrokeColor(args.vg, nvgRGBA(145, 80, 1, 220));
	nvgStrokeWidth(args.vg, 1.f);
	nvgStroke(args.vg);

	for (int i = 0; i < 17; ++i) {
		const float angle = float(i) * 2.39996323f;
		const float radial = 0.25f + 0.62f * float((i * 37) % 17) / 16.f;
		const float x = center.x + std::cos(angle) * radiusX * radial;
		const float y = center.y + std::sin(angle) * radiusY * radial;
		if (y < center.y - radiusY * 0.36f) {
			continue;
		}
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, x, y, minimum * (0.006f + 0.002f * (i % 3)));
		nvgFillColor(args.vg, nvgRGBA(255, 235, 139, 135));
		nvgFill(args.vg);
	}

	const float eyeRadius = minimum * 0.105f;
	const float eyeY = center.y - radiusY * 0.23f;
	const float eyeSpacing = eyeRadius * 0.79f;
	drawEye(
		args.vg, Vec(center.x - eyeSpacing, eyeY), eyeRadius,
		pose.gazeX, pose.gazeY, pose.leftBlink);
	drawEye(
		args.vg, Vec(center.x + eyeSpacing, eyeY), eyeRadius,
		pose.gazeX, pose.gazeY, pose.rightBlink);

	const float blush = clamp01(pose.blush);
	if (blush > 0.001f) {
		nvgBeginPath(args.vg);
		nvgEllipse(
			args.vg, center.x - radiusX * 0.57f, center.y + radiusY * 0.10f,
			radiusX * 0.16f, radiusY * 0.09f);
		nvgEllipse(
			args.vg, center.x + radiusX * 0.57f, center.y + radiusY * 0.10f,
			radiusX * 0.16f, radiusY * 0.09f);
		nvgFillColor(args.vg, nvgRGBA(255, 93, 91, int(150.f * blush)));
		nvgFill(args.vg);
	}

	const float mouthY = center.y + radiusY * 0.24f;
	const float mouthWidth = minimum
		* (0.057f + 0.018f * pose.mouthSmile);
	const float mouthHeight = minimum
		* (0.030f + 0.015f * pose.mouthSmile
			- 0.012f * pose.mouthTension);
	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, center.x - mouthWidth, mouthY);
	nvgBezierTo(
		args.vg,
		center.x - mouthWidth * 0.6f, mouthY + mouthHeight,
		center.x + mouthWidth * 0.6f, mouthY + mouthHeight,
		center.x + mouthWidth, mouthY);
	nvgBezierTo(
		args.vg,
		center.x + mouthWidth * 0.52f, mouthY - mouthHeight * 0.32f,
		center.x - mouthWidth * 0.52f, mouthY - mouthHeight * 0.32f,
		center.x - mouthWidth, mouthY);
	nvgClosePath(args.vg);
	nvgFillColor(args.vg, nvgRGB(80, 29, 18));
	nvgFill(args.vg);
	nvgStrokeColor(args.vg, nvgRGBA(104, 46, 12, 230));
	nvgStrokeWidth(args.vg, 0.7f);
	nvgStroke(args.vg);
	nvgBeginPath(args.vg);
	nvgEllipse(
		args.vg, center.x, mouthY + mouthHeight * 0.45f,
		mouthWidth * 0.48f, mouthHeight * 0.24f);
	nvgFillColor(args.vg, nvgRGB(255, 113, 96));
	nvgFill(args.vg);

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
}
