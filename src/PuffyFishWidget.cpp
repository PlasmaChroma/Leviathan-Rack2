#include "PuffyFishWidget.hpp"

#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cmath>

namespace {

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
	float angle,
	float sizeScale) const {
	nvgSave(vg);
	const float side = left ? -1.f : 1.f;
	const Vec root(
		center.x + side * bodyRadius * 0.84f,
		center.y - bodyRadius * 0.03f);
	nvgTranslate(vg, root.x, root.y);
	nvgRotate(vg, angle);
	nvgScale(vg, side, 1.f);
	const float clampedSizeScale = clamp(sizeScale, 0.75f, 1.f);
	const float length = bodyRadius * 0.68f * clampedSizeScale;
	const float height = bodyRadius * 0.52f * clampedSizeScale;
	nvgBeginPath(vg);
	nvgMoveTo(vg, 0.f, -height * 0.18f);
	nvgBezierTo(
		vg, length * 0.24f, -height * 0.42f,
		length * 0.62f, -height * 0.66f,
		length * 0.82f, -height * 0.58f);
	nvgBezierTo(
		vg, length * 1.16f, -height * 0.48f,
		length * 1.16f, height * 0.48f,
		length * 0.82f, height * 0.58f);
	nvgBezierTo(
		vg, length * 0.62f, height * 0.66f,
		length * 0.24f, height * 0.42f,
		0.f, height * 0.18f);
	nvgClosePath(vg);
	const NVGpaint fill = nvgLinearGradient(
		vg, 0.f, -height, length, height,
		nvgRGB(255, 224, 99), nvgRGB(255, 166, 8));
	nvgFillPaint(vg, fill);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(148, 85, 4, 190));
	nvgStrokeWidth(vg, 0.8f);
	nvgStroke(vg);
	for (int i = -2; i <= 2; ++i) {
		const float targetY = height * 0.22f * float(i);
		nvgBeginPath(vg);
		nvgMoveTo(vg, length * 0.06f, height * 0.025f * float(i));
		nvgBezierTo(
			vg, length * 0.34f, targetY * 0.42f,
			length * 0.66f, targetY * 0.82f,
			length * 0.98f, targetY);
		nvgStrokeColor(vg, nvgRGBA(196, 116, 3, 150));
		nvgStrokeWidth(vg, 0.65f);
		nvgStroke(vg);
	}
	nvgRestore(vg);
}

bool PuffyFishWidget::drawBodyRaster(
	NVGcontext* vg,
	Vec center,
	float radiusX,
	float radiusY) const {
	if (!vg || !APP || !APP->window || radiusX <= 0.f || radiusY <= 0.f) {
		return false;
	}
	const std::string fullPath =
		asset::plugin(pluginInstance, "res/icon/Puffy_Body_NS.png");
	std::shared_ptr<window::Image> image = APP->window->loadImage(fullPath);
	if (!image || image->handle < 0) {
		return false;
	}
	int imageHandle =
		visual_assets::loadRasterMipmapHandle(vg, image, fullPath);
	if (imageHandle < 0) {
		imageHandle = image->handle;
	}
	int imageWidth = 0;
	int imageHeight = 0;
	nvgImageSize(vg, imageHandle, &imageWidth, &imageHeight);
	if (imageWidth <= 0 || imageHeight <= 0) {
		return false;
	}

	// The spherical body occupies about 80% of the transparent source canvas.
	// This includes the baked spikes while matching the old procedural radius.
	constexpr float rasterExtentScale = 1.25f;
	const float drawWidth = 2.f * radiusX * rasterExtentScale;
	const float drawHeight = 2.f * radiusY * rasterExtentScale;
	const float x = center.x - 0.5f * drawWidth;
	const float y = center.y - 0.5f * drawHeight;
	const NVGpaint paint = nvgImagePattern(
		vg, x, y, drawWidth, drawHeight, 0.f, imageHandle, 1.f);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, drawWidth, drawHeight);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	return true;
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
		height * 0.5f + pose.verticalOffset * minimum);
	const float inflation = clamp01(pose.inflation);
	const float radius = minimum * (0.255f + 0.110f * inflation);
	const float radiusX = radius * (0.88f + 0.12f * inflation)
		* (1.f + pose.squashX);
	const float radiusY = radius * (0.93f + 0.07f * inflation)
		* (1.f + pose.squashY);
	const float finSizeScale = 1.f - 0.20f * inflation;

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);

	nvgBeginPath(args.vg);
	nvgEllipse(
		args.vg,
		center.x,
		center.y + radiusY * 1.23f,
		radiusX * (0.88f + 0.08f * inflation),
		radiusY * (0.145f + 0.025f * inflation));
	nvgFillColor(
		args.vg,
		nvgRGBA(5, 6, 15, int(82.f + 30.f * inflation)));
	nvgFill(args.vg);

	drawFin(
		args.vg,
		center,
		radiusX,
		true,
		pose.leftFinAngle,
		finSizeScale);
	drawFin(
		args.vg,
		center,
		radiusX,
		false,
		pose.rightFinAngle,
		finSizeScale);

	if (!drawBodyRaster(args.vg, center, radiusX, radiusY)) {
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

	const float mouthY = center.y + radiusY * 0.30f;
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
