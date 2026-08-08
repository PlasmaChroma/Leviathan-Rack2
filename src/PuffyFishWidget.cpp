#include "PuffyFishWidget.hpp"

#include "PuffyBodyImageCache.hpp"
#include "PuffyDrawDiagnostics.hpp"
#include "PuffyVisualPalette.hpp"
#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cmath>

namespace {

constexpr float kPi = 3.14159265358979323846f;

float clamp01(float value) {
	return std::max(0.f, std::min(value, 1.f));
}

void appendEllipseArc(
	NVGcontext* vg,
	Vec center,
	float radiusX,
	float radiusY,
	float startAngle,
	float endAngle) {
	const float handle = (4.f / 3.f)
		* std::tan(0.25f * (endAngle - startAngle));
	const float startCos = std::cos(startAngle);
	const float startSin = std::sin(startAngle);
	const float endCos = std::cos(endAngle);
	const float endSin = std::sin(endAngle);
	nvgBezierTo(
		vg,
		center.x + radiusX * (startCos - handle * startSin),
		center.y + radiusY * (startSin + handle * startCos),
		center.x + radiusX * (endCos + handle * endSin),
		center.y + radiusY * (endSin - handle * endCos),
		center.x + radiusX * endCos,
		center.y + radiusY * endSin);
}

NVGcolor multiplyColor(NVGcolor color, NVGcolor tint) {
	return nvgRGBAf(
		color.r * tint.r,
		color.g * tint.g,
		color.b * tint.b,
		color.a * tint.a);
}

std::uint8_t colorByte(float value) {
	return std::uint8_t(clamp(int(value * 255.f + 0.5f), 0, 255));
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

PuffyFishWidget::PuffyFishWidget(Puffy* module, bool roamingAvatar)
	: module(module), roamingAvatar(roamingAvatar) {
	visual.effectiveAmount = 0.25f;
	visual.negativeCharacter = int(puffy::Character::Bloom);
	visual.positiveCharacter = int(puffy::Character::Spine);
	visual.charactersLinked = false;
	if (module) {
		PuffyVisualState snapshot;
		if (module->readVisualState(&snapshot)) {
			visual = snapshot;
		}
	}
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
	int imageHeight,
	NVGcolor tint) const {
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
	NVGpaint paint = nvgImagePattern(
		vg, x, y, drawWidth, drawHeight, 0.f, imageHandle, 1.f);
	paint.innerColor = tint;
	paint.outerColor = tint;
	nvgBeginPath(vg);
	nvgRect(vg, x, y, drawWidth, drawHeight);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
	nvgRestore(vg);
}

bool PuffyFishWidget::bodyTintIsSettled(
	NVGcolor negativeTint,
	NVGcolor positiveTint) const {
	const NVGcolor targetNegative = puffy_visual::characterTint(
		clamp(visual.negativeCharacter, 0, puffy::kCharacterCount - 1));
	const NVGcolor targetPositive = puffy_visual::characterTint(
		clamp(visual.positiveCharacter, 0, puffy::kCharacterCount - 1));
	return colorByte(negativeTint.r) == colorByte(targetNegative.r)
		&& colorByte(negativeTint.g) == colorByte(targetNegative.g)
		&& colorByte(negativeTint.b) == colorByte(targetNegative.b)
		&& colorByte(positiveTint.r) == colorByte(targetPositive.r)
		&& colorByte(positiveTint.g) == colorByte(targetPositive.g)
		&& colorByte(positiveTint.b) == colorByte(targetPositive.b);
}

bool PuffyFishWidget::drawTransitionBodyRaster(
	NVGcontext* vg,
	Vec center,
	float radiusX,
	float radiusY,
	NVGcolor negativeTint,
	NVGcolor positiveTint) {
	constexpr float rasterExtentScale = 1.25f;
	const float drawWidth = 2.f * radiusX * rasterExtentScale;
	const float drawHeight = 2.f * radiusY * rasterExtentScale;
	const float x = center.x - 0.5f * drawWidth;
	const float y = center.y - 0.5f * drawHeight;
	const puffy_body_cache::ImageAccess atlas =
		puffy_body_cache::ensureTransitionAtlas(vg);
	if (!atlas) {
		return false;
	}
	transitionAtlasReady = true;
	if (isPuffyDrawMeasurementEnabled()) {
		PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
		metrics.bodyTransitionAtlasCreates += atlas.created ? 1u : 0u;
		metrics.bodyTransitionAtlasResets += atlas.contextReset ? 1u : 0u;
	}
	const auto drawAtlasPanel = [&](int panel, NVGcolor tint) {
		NVGpaint paint = nvgImagePattern(
			vg,
			x - float(panel) * drawWidth,
			y,
			3.f * drawWidth,
			drawHeight,
			0.f,
			atlas.handle,
			1.f);
		paint.innerColor = tint;
		paint.outerColor = tint;
		nvgBeginPath(vg);
		nvgRect(vg, x, y, drawWidth, drawHeight);
		nvgFillPaint(vg, paint);
		nvgFill(vg);
	};
	nvgSave(vg);
	drawAtlasPanel(0, nvgRGBA(255, 255, 255, 255));
	nvgGlobalCompositeOperation(vg, NVG_LIGHTER);
	drawAtlasPanel(1, negativeTint);
	drawAtlasPanel(2, positiveTint);
	nvgRestore(vg);
	return true;
}

bool PuffyFishWidget::drawBodyRaster(
	NVGcontext* vg,
	Vec center,
	float radiusX,
	float radiusY,
	NVGcolor negativeTint,
	NVGcolor positiveTint) {
	if (!vg || radiusX <= 0.f || radiusY <= 0.f) {
		return false;
	}
	if (!bodyTintIsSettled(negativeTint, positiveTint)) {
		bodyStableDraws = 0;
		PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
		const bool measureDraw = isPuffyDrawMeasurementEnabled();
		PuffyScopedDrawTimer transitionTimer(
			metrics.bodyTransitionDrawNs, measureDraw);
		if (measureDraw) {
			++metrics.bodyTransitionDraws;
		}
		return drawTransitionBodyRaster(
			vg, center, radiusX, radiusY, negativeTint, positiveTint);
	}
	const bool measureDraw = isPuffyDrawMeasurementEnabled();
	PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
	puffy_body_cache::ImageAccess body;
	{
		PuffyScopedDrawTimer ensureTimer(metrics.bodyEnsureNs, measureDraw);
		body = puffy_body_cache::ensureFinalBody(
			vg, visual.negativeCharacter, visual.positiveCharacter);
	}
	if (measureDraw) {
		metrics.bodyCacheHits += body.cacheHit ? 1u : 0u;
		metrics.bodyRecolors += body.recolored ? 1u : 0u;
		metrics.bodyImageCreates += body.created ? 1u : 0u;
		metrics.bodyContextResets += body.contextReset ? 1u : 0u;
		metrics.bodyRecolorNs += body.recolorNs;
		metrics.bodyUploadNs += body.uploadNs;
	}
	if (body.contextReset) {
		transitionAtlasReady = false;
		bodyStableDraws = 0;
	}
	if (!body) {
		return false;
	}
	bodyStableDraws = std::min(bodyStableDraws + 1, 12);
	if (bodyStableDraws >= 12 && !transitionAtlasReady) {
		PuffyScopedDrawTimer prewarmTimer(
			metrics.bodyTransitionAtlasPrewarmNs, measureDraw);
		const puffy_body_cache::ImageAccess atlas =
			puffy_body_cache::ensureTransitionAtlas(vg);
		transitionAtlasReady = bool(atlas);
		if (measureDraw) {
			metrics.bodyTransitionAtlasCreates += atlas.created ? 1u : 0u;
			metrics.bodyTransitionAtlasResets += atlas.contextReset ? 1u : 0u;
			metrics.bodyTransitionAtlasPrewarms += atlas.created ? 1u : 0u;
		}
	}

	// The spherical body occupies about 80% of the transparent source canvas.
	// This includes the baked spikes while matching the old procedural radius.
	constexpr float rasterExtentScale = 1.25f;
	const float drawWidth = 2.f * radiusX * rasterExtentScale;
	const float drawHeight = 2.f * radiusY * rasterExtentScale;
	const float x = center.x - 0.5f * drawWidth;
	const float y = center.y - 0.5f * drawHeight;

	PuffyScopedDrawTimer bodyDrawTimer(
		metrics.bodyDrawNs, measureDraw);
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
	int eyeballImageHandle,
	float gazeX,
	float gazeY,
	float blink,
	NVGcolor eyelidColor) const {
	const float closed = clamp01(blink);
	const float radiusX = radius * 0.86f;
	nvgBeginPath(vg);
	nvgEllipse(vg, center.x, center.y, radiusX, radius);
	if (eyeballImageHandle >= 0) {
		// The source is circular. Mapping its square canvas directly onto the
		// current eye bounds supplies the intentional horizontal stretch.
		const NVGpaint eyeball = nvgImagePattern(
			vg,
			center.x - radiusX,
			center.y - radius,
			2.f * radiusX,
			2.f * radius,
			0.f,
			eyeballImageHandle,
			1.f);
		nvgFillPaint(vg, eyeball);
		nvgFill(vg);
	}
	else {
		const NVGpaint white = nvgRadialGradient(
			vg, center.x - radius * 0.28f, center.y - radius * 0.34f,
			radius * 0.08f, radius,
			nvgRGB(255, 255, 239), nvgRGB(255, 226, 147));
		nvgFillPaint(vg, white);
		nvgFill(vg);
		nvgStrokeColor(vg, nvgRGBA(121, 73, 8, 210));
		nvgStrokeWidth(vg, 0.8f);
		nvgStroke(vg);
	}

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
		const float edgeAngle = std::asin(clamp(edgeY / radius, 0.f, 1.f));

		// Two explicit cubic segments follow each outer ellipse arc accurately.
		// Keeping every point in eye coordinates avoids transform-state seams
		// between the outer arc and the inner lid edge.
		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x - edgeX, center.y - edgeY);
		appendEllipseArc(
			vg, center, radiusX, radius,
			kPi + edgeAngle, 1.5f * kPi);
		appendEllipseArc(
			vg, center, radiusX, radius,
			1.5f * kPi, 2.f * kPi - edgeAngle);
		nvgQuadTo(
			vg, center.x, center.y - edgeY + edgeBulge,
			center.x - edgeX, center.y - edgeY);
		nvgClosePath(vg);
		nvgFillColor(vg, eyelidColor);
		nvgFill(vg);

		nvgBeginPath(vg);
		nvgMoveTo(vg, center.x + edgeX, center.y + edgeY);
		appendEllipseArc(
			vg, center, radiusX, radius,
			edgeAngle, 0.5f * kPi);
		appendEllipseArc(
			vg, center, radiusX, radius,
			0.5f * kPi, kPi - edgeAngle);
		nvgQuadTo(
			vg, center.x, center.y + edgeY - edgeBulge,
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
			vg, center.x, center.y + edgeY - edgeBulge,
			center.x + edgeX, center.y + edgeY);
		nvgStrokeColor(vg, creaseColor);
		nvgStrokeWidth(vg, 0.7f);
		nvgStroke(vg);
	}
}

void PuffyFishWidget::drawRoamingDropShadow(NVGcontext* vg) {
	if (!vg || !roamingAvatar) {
		return;
	}
	const float canvasMinimum = std::min(box.size.x, box.size.y);
	const float minimum = canvasMinimum * (80.f / 96.f);
	const float inflation = clamp01(pose.inflation);
	const Vec center(
		box.size.x * 0.5f + minimum * 0.045f,
		box.size.y * 0.5f + minimum * (0.055f + pose.verticalOffset));
	const float radius = minimum * (0.255f + 0.130f * inflation);
	const float radiusX = radius * (0.88f + 0.12f * inflation)
		* (1.f + pose.squashX);
	const float radiusY = radius * (0.93f + 0.07f * inflation)
		* (1.f + pose.squashY);
	const float finSizeScale = 1.f - 0.20f * inflation;
	const PuffyRasterAsset fin = resolveRasterAsset(
		vg, "res/icon/Puffy_Fin_NS.png");
	const NVGcolor shadow = nvgRGBA(0, 0, 3, 215);

	nvgSave(vg);
	nvgGlobalAlpha(vg, 0.30f);
	drawFin(
		vg, center, radiusX, true, pose.leftFinAngle, finSizeScale,
		fin.handle, fin.width, fin.height, shadow);
	drawFin(
		vg, center, radiusX, false, pose.rightFinAngle, finSizeScale,
		fin.handle, fin.width, fin.height, shadow);
	if (!drawTransitionBodyRaster(
		vg, center, radiusX, radiusY, shadow, shadow)) {
		nvgBeginPath(vg);
		nvgEllipse(vg, center.x, center.y, radiusX, radiusY);
		nvgFillColor(vg, shadow);
		nvgFill(vg);
	}
	nvgRestore(vg);
}

void PuffyFishWidget::draw(const DrawArgs& args) {
	if (module && !roamingAvatar
		&& module->roamingAvatarActive.load(std::memory_order_acquire)) {
		const float width = box.size.x;
		const float height = box.size.y;
		const Vec center(width * 0.5f, height * 0.5f);
		
		Vec target(
			module->roamingTargetX.load(std::memory_order_relaxed),
			module->roamingTargetY.load(std::memory_order_relaxed)
		);
		Vec myCenter = APP && APP->scene
			? getRelativeOffset(center, APP->scene) : center;
		float angle = std::atan2(target.y - myCenter.y, target.x - myCenter.x);
		
		nvgSave(args.vg);
		nvgTranslate(args.vg, center.x, center.y);
		nvgRotate(args.vg, angle);
		
		// Draw compass needle
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 15.f, 0.f);
		nvgLineTo(args.vg, -10.f, -8.f);
		nvgLineTo(args.vg, -5.f, 0.f);
		nvgLineTo(args.vg, -10.f, 8.f);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, nvgRGB(255, 100, 80));
		nvgFill(args.vg);
		
		nvgRestore(args.vg);
		return;
	}

	const bool measureDraw = isPuffyDrawMeasurementEnabled();
	PuffyDrawMetrics& metrics = puffyDrawMetricsForUiThread();
	PuffyScopedDrawTimer fishTimer(metrics.fishDrawNs, measureDraw);
	TransparentWidget::draw(args);
	const float width = box.size.x;
	const float height = box.size.y;
	const float canvasMinimum = std::min(width, height);
	// The roaming canvas includes transparent safety padding for fully puffed
	// fins and the silhouette shadow. Preserve the original 80px design scale.
	const float minimum = roamingAvatar
		? canvasMinimum * (80.f / 96.f) : canvasMinimum;
	const Vec center(
		width * 0.5f,
		height * 0.5f + pose.verticalOffset * minimum);
	const float inflation = clamp01(pose.inflation);
	const float radius = minimum * (0.255f + 0.130f * inflation);
	const float radiusX = radius * (0.88f + 0.12f * inflation)
		* (1.f + pose.squashX);
	const float radiusY = radius * (0.93f + 0.07f * inflation)
		* (1.f + pose.squashY);
	const float finSizeScale = 1.f - 0.20f * inflation;
	const NVGcolor negativeBodyTint = puffy_visual::weightedCharacterTint(
		pose.negativeCharacterTintWeights);
	const NVGcolor positiveBodyTint = puffy_visual::weightedCharacterTint(
		pose.positiveCharacterTintWeights);
	// Keep the cast shadow grounded near the bottom of the scene. Inflation
	// changes its footprint, but the fish's motion and radius do not move it.
	const Vec shadowCenter(width * 0.5f, height * 0.93f);
	const PuffyRasterAsset fin = resolveRasterAsset(
		args.vg, "res/icon/Puffy_Fin_NS.png");

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, width, height);

	if (!roamingAvatar) {
		nvgBeginPath(args.vg);
		nvgEllipse(
			args.vg,
			shadowCenter.x,
			shadowCenter.y,
			radiusX * (0.88f + 0.08f * inflation),
			radiusY * (0.145f + 0.025f * inflation));
		nvgFillColor(
			args.vg,
			nvgRGBA(2, 3, 9, int(104.f + 34.f * inflation)));
		nvgFill(args.vg);
	}

	{
		PuffyScopedDrawTimer finTimer(metrics.finDrawNs, measureDraw);
		drawFin(
			args.vg,
			center,
			radiusX,
			true,
			pose.leftFinAngle,
			finSizeScale,
			fin.handle,
			fin.width,
			fin.height,
			negativeBodyTint);
		drawFin(
			args.vg,
			center,
			radiusX,
			false,
			pose.rightFinAngle,
			finSizeScale,
			fin.handle,
			fin.width,
			fin.height,
			positiveBodyTint);
	}

	if (!drawBodyRaster(
		args.vg,
		center,
		radiusX,
		radiusY,
		negativeBodyTint,
		positiveBodyTint)) {
		if (measureDraw) {
			++metrics.bodyFallbackDraws;
		}
		nvgBeginPath(args.vg);
		nvgEllipse(args.vg, center.x, center.y, radiusX, radiusY);
		const NVGpaint body = nvgLinearGradient(
			args.vg,
			center.x - radiusX,
			center.y,
			center.x + radiusX,
			center.y,
			negativeBodyTint,
			positiveBodyTint);
		nvgFillPaint(args.vg, body);
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(145, 80, 1, 220));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);
	}

	const float blush = clamp01(pose.blush);
	if (blush > 0.001f) {
		const float cheekRadiusX = radiusX * 0.16f;
		const float cheekRadiusY = radiusY * 0.09f;
		const auto drawCheek = [&](float cheekX) {
			nvgSave(args.vg);
			nvgTranslate(args.vg, cheekX, center.y + radiusY * 0.20f);
			nvgScale(args.vg, cheekRadiusX, cheekRadiusY);
			const NVGpaint cheekGlow = nvgRadialGradient(
				args.vg,
				0.f,
				0.f,
				0.f,
				1.f,
				nvgRGBA(255, 105, 101, int(150.f * blush)),
				nvgRGBA(255, 72, 82, 0));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0.f, 0.f, 1.f);
			nvgFillPaint(args.vg, cheekGlow);
			nvgFill(args.vg);
			nvgRestore(args.vg);
		};
		drawCheek(center.x - radiusX * 0.57f);
		drawCheek(center.x + radiusX * 0.57f);
	}

	const float eyeRadius = minimum * 0.105f;
	const float eyeY = center.y - radiusY * 0.23f;
	const float eyeSpacing = eyeRadius * 0.79f;
	const PuffyRasterAsset eyeball = resolveRasterAsset(
		args.vg, "res/icon/Puffy_Eyeball.png");
	const NVGcolor eyelidMaterialTint = nvgRGB(232, 223, 202);
	const NVGcolor leftEyelidColor = multiplyColor(
		eyelidMaterialTint, negativeBodyTint);
	const NVGcolor rightEyelidColor = multiplyColor(
		eyelidMaterialTint, positiveBodyTint);
	{
		PuffyScopedDrawTimer eyeTimer(metrics.eyeDrawNs, measureDraw);
		drawEye(
			args.vg, Vec(center.x - eyeSpacing, eyeY), eyeRadius,
			eyeball.handle,
			pose.gazeX, pose.gazeY,
			std::max(pose.leftBlink, pose.leftSquint), leftEyelidColor);
		drawEye(
			args.vg, Vec(center.x + eyeSpacing, eyeY), eyeRadius,
			eyeball.handle,
			pose.gazeX, pose.gazeY,
			std::max(pose.rightBlink, pose.rightSquint), rightEyelidColor);
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
