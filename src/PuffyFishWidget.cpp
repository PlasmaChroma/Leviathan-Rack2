#include "PuffyFishWidget.hpp"

#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cmath>

namespace {

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

struct PuffyRasterAsset {
	int handle = -1;
	int width = 0;
	int height = 0;

	explicit operator bool() const {
		return handle >= 0 && width > 0 && height > 0;
	}
};

PuffyRasterAsset resolveRasterAsset(
	NVGcontext* vg,
	const char* relativePath) {
	PuffyRasterAsset raster;
	if (!vg || !relativePath || !APP || !APP->window) {
		return raster;
	}
	const std::string fullPath = asset::plugin(pluginInstance, relativePath);
	std::shared_ptr<window::Image> image = APP->window->loadImage(fullPath);
	if (!image || image->handle < 0) {
		return raster;
	}
	raster.handle =
		visual_assets::loadRasterMipmapHandle(vg, image, fullPath);
	if (raster.handle < 0) {
		raster.handle = image->handle;
	}
	nvgImageSize(vg, raster.handle, &raster.width, &raster.height);
	if (raster.width <= 0 || raster.height <= 0) {
		raster = {};
	}
	return raster;
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
	float sizeScale,
	int imageHandle,
	int imageWidth,
	int imageHeight) const {
	if (!vg || bodyRadius <= 0.f
		|| imageHandle < 0 || imageWidth <= 0 || imageHeight <= 0) {
		return;
	}

	nvgSave(vg);
	const float side = left ? -1.f : 1.f;
	const Vec root(
		center.x + side * bodyRadius * 0.84f,
		center.y - bodyRadius * 0.03f);
	nvgTranslate(vg, root.x, root.y);
	nvgRotate(vg, angle);
	// The source points right. Mirror its local X axis for the left fin.
	nvgScale(vg, side, 1.f);
	const float clampedSizeScale = clamp(sizeScale, 0.75f, 1.f);
	const float drawWidth = bodyRadius * 0.78f * clampedSizeScale;
	const float drawHeight =
		drawWidth * float(imageHeight) / float(imageWidth);
	const float x = 0.f;
	const float y = -0.5f * drawHeight;
	const NVGpaint paint = nvgImagePattern(
		vg, x, y, drawWidth, drawHeight, 0.f, imageHandle, 1.f);
	nvgBeginPath(vg);
	nvgRect(vg, x, y, drawWidth, drawHeight);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	nvgRestore(vg);
}

bool PuffyFishWidget::drawBodyRaster(
	NVGcontext* vg,
	Vec center,
	float radiusX,
	float radiusY) const {
	if (!vg || radiusX <= 0.f || radiusY <= 0.f) {
		return false;
	}
	const PuffyRasterAsset body = resolveRasterAsset(
		vg, "res/icon/Puffy_Body_NS.png");
	if (!body) {
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
		vg, x, y, drawWidth, drawHeight, 0.f, body.handle, 1.f);
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
	float blink,
	NVGcolor eyelidColor) const {
	const float closed = clamp01(blink);
	const float radiusX = radius * 0.86f;
	nvgBeginPath(vg);
	nvgEllipse(vg, center.x, center.y, radiusX, radius);
	const NVGpaint white = nvgRadialGradient(
		vg, center.x - radius * 0.28f, center.y - radius * 0.34f,
		radius * 0.08f, radius,
		nvgRGB(255, 255, 239), nvgRGB(255, 226, 147));
	nvgFillPaint(vg, white);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(121, 73, 8, 210));
	nvgStrokeWidth(vg, 0.8f);
	nvgStroke(vg);

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

	if (closed > 0.001f) {
		const float edgeY = radius * (1.f - closed);
		const float edgeX = radiusX
			* std::sqrt(std::max(0.f, 1.f - edgeY * edgeY / (radius * radius)));
		const float edgeBulge = radius * 0.055f * closed;

		// Upper lid: follow the eye's outer arc, then return along the
		// descending lid edge.
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x - edgeX, center.y - edgeY);
		nvgQuadTo(
			vg, center.x, center.y - 2.f * radius + edgeY,
			center.x + edgeX, center.y - edgeY);
		nvgQuadTo(
			vg, center.x, center.y - edgeY + edgeBulge,
			center.x - edgeX, center.y - edgeY);
		nvgClosePath(vg);
		nvgFillColor(vg, eyelidColor);
		nvgFill(vg);

		// Lower lid mirrors the upper one and meets it at full closure.
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x + edgeX, center.y + edgeY);
		nvgQuadTo(
			vg, center.x, center.y + 2.f * radius - edgeY,
			center.x - edgeX, center.y + edgeY);
		nvgQuadTo(
			vg, center.x, center.y + edgeY + edgeBulge,
			center.x + edgeX, center.y + edgeY);
		nvgClosePath(vg);
		nvgFillColor(vg, eyelidColor);
		nvgFill(vg);

		const NVGcolor creaseColor = nvgRGBA(141, 131, 110, 210);
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x - edgeX, center.y - edgeY);
		nvgQuadTo(
			vg, center.x, center.y - edgeY + edgeBulge,
			center.x + edgeX, center.y - edgeY);
		nvgStrokeColor(vg, creaseColor);
		nvgStrokeWidth(vg, 0.7f);
		nvgStroke(vg);
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x - edgeX, center.y + edgeY);
		nvgQuadTo(
			vg, center.x, center.y + edgeY + edgeBulge,
			center.x + edgeX, center.y + edgeY);
		nvgStrokeColor(vg, creaseColor);
		nvgStrokeWidth(vg, 0.7f);
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
	// Keep the cast shadow grounded near the bottom of the scene. Inflation
	// changes its footprint, but the fish's motion and radius do not move it.
	const Vec shadowCenter(width * 0.5f, height * 0.92f);
	const PuffyRasterAsset fin = resolveRasterAsset(
		args.vg, "res/icon/Puffy_Fin_NS.png");

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);

	nvgBeginPath(args.vg);
	nvgEllipse(
		args.vg,
		shadowCenter.x,
		shadowCenter.y,
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
		finSizeScale,
		fin.handle,
		fin.width,
		fin.height);
	drawFin(
		args.vg,
		center,
		radiusX,
		false,
		pose.rightFinAngle,
		finSizeScale,
		fin.handle,
		fin.width,
		fin.height);

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
	// Kept explicit so the future body color-filter pass can provide the
	// filtered body tone to the eyelids.
	const NVGcolor eyelidColor = nvgRGB(232, 223, 202);
	drawEye(
		args.vg, Vec(center.x - eyeSpacing, eyeY), eyeRadius,
		pose.gazeX, pose.gazeY, pose.leftBlink, eyelidColor);
	drawEye(
		args.vg, Vec(center.x + eyeSpacing, eyeY), eyeRadius,
		pose.gazeX, pose.gazeY, pose.rightBlink, eyelidColor);

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
