#include "VisualAssets.hpp"

#include "../render/GlLifecycleUtils.hpp"
#include "../render/AdaptiveGlSurface.hpp"
#include "../render/NvgGraphicsLifecycle.hpp"
#include "../render/DrawVisibilityLease.hpp"

#include <nanovg_gl.h>
#include <nanosvgrast.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <utility>
#include <vector>

#include <map>

namespace {



// Standard 46 px knobs use the original 4x atlas; larger knobs get a
// proportionally finer one. One shared, mipmapped atlas per scale.
constexpr int kHaloCapRasterScale = 4;
int haloCapRasterScale(float diameter) {
	return std::max(kHaloCapRasterScale, int(std::ceil(kHaloCapRasterScale * diameter / 46.f - 1e-4f)));
}

struct HaloCapRasterAtlas {
	int width = 0;
	int height = 0;
	std::vector<unsigned char> rgba;
};

std::shared_ptr<HaloCapRasterAtlas> getHaloCapRasterAtlas(
	const std::shared_ptr<window::Svg>& normalSvg,
	const std::shared_ptr<window::Svg>& litSvg, int rasterScale = kHaloCapRasterScale) {
	static std::map<int, std::weak_ptr<HaloCapRasterAtlas>> cachedAtlases;
	std::weak_ptr<HaloCapRasterAtlas>& cachedAtlas = cachedAtlases[rasterScale];
	if (std::shared_ptr<HaloCapRasterAtlas> atlas = cachedAtlas.lock()) {
		return atlas;
	}
	if (!normalSvg || !normalSvg->handle || !litSvg || !litSvg->handle) {
		return nullptr;
	}
	const int imageWidth = std::max(1, int(std::ceil(std::max(
		normalSvg->handle->width, litSvg->handle->width) * rasterScale)));
	const int imageHeight = std::max(1, int(std::ceil(std::max(
		normalSvg->handle->height, litSvg->handle->height) * rasterScale)));
	std::shared_ptr<HaloCapRasterAtlas> atlas = std::make_shared<HaloCapRasterAtlas>();
	atlas->width = imageWidth * 2;
	atlas->height = imageHeight;
	atlas->rgba.assign(size_t(atlas->width) * size_t(atlas->height) * 4u, 0u);

	NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
	if (!rasterizer) return nullptr;
	const int stride = atlas->width * 4;
	nsvgRasterize(rasterizer, normalSvg->handle, 0.f, 0.f, float(rasterScale),
		atlas->rgba.data(), imageWidth, imageHeight, stride);
	nsvgRasterize(rasterizer, litSvg->handle, 0.f, 0.f, float(rasterScale),
		atlas->rgba.data() + size_t(imageWidth) * 4u, imageWidth, imageHeight, stride);
	nsvgDeleteRasterizer(rasterizer);
	// Premultiply before filtering/mipmap generation so transparent SVG edges do
	// not acquire dark fringes while rotating between texels.
	for (size_t i = 0, count = size_t(atlas->width) * size_t(atlas->height); i < count; ++i) {
		const unsigned int alpha = atlas->rgba[i * 4u + 3u];
		atlas->rgba[i * 4u + 0u] = static_cast<unsigned char>((unsigned(atlas->rgba[i * 4u + 0u]) * alpha + 127u) / 255u);
		atlas->rgba[i * 4u + 1u] = static_cast<unsigned char>((unsigned(atlas->rgba[i * 4u + 1u]) * alpha + 127u) / 255u);
		atlas->rgba[i * 4u + 2u] = static_cast<unsigned char>((unsigned(atlas->rgba[i * 4u + 2u]) * alpha + 127u) / 255u);
	}
	cachedAtlas = atlas;
	return atlas;
}

// Five by seven padded tiles: 17 cumulative ring states, 16 boundary
// overlays, and two cap states. Matching knobs share this context-owned image.
// Animation draws sprites directly; a settled knob uses one cached composite.
struct HaloArcImageSet {
	gl_lifecycle::ContextLease context;
	NVGcontext* vg = nullptr;
	int image = -1, width = 0, height = 0;
	int side = 0, stride = 0;
	~HaloArcImageSet() {
		gl_lifecycle::retireObject(context, gl_lifecycle::ObjectKind::NvgImage, image > 0 ? image : 0);
	}
	bool valid(NVGcontext* current) const {
		return current == vg && gl_lifecycle::resourceContextMatches(context, current)
			&& nvg_gfx_lifecycle::ownedNvgImageSizeMatches(current, image, width, height);
	}
	void tile(NVGcontext* current, int index, float diameter, float alpha = 1.f) const {
		const float scale = diameter / side;
		const float x = (index % 5) * stride + 4, y = (index / 5) * stride + 4;
		nvgBeginPath(current); nvgRect(current, 0, 0, diameter, diameter);
		nvgFillPaint(current, nvgImagePattern(current, -x*scale, -y*scale,
			width*scale, height*scale, 0, image, alpha)); nvgFill(current);
	}
};

uint64_t haloElapsedNs(std::chrono::steady_clock::time_point start) {
	return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::steady_clock::now() - start).count());
}

struct HaloNanoVgFallbackWidget final : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const bool measure = isDragonKingDebugEnabled();
		const auto start = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
		Widget::draw(args);
		if (measure) {
			visual_assets::haloKnob2DrawMetrics().nanoVgSurfaceDrawNs += haloElapsedNs(start);
			++visual_assets::haloKnob2DrawMetrics().nanoVgSurfaceDraws;
		}
	}
};

NVGcolor blendHaloColor(NVGcolor a, NVGcolor b, float t) {
	t = clamp(t, 0.f, 1.f);
	return nvgRGBAf(
		crossfade(a.r, b.r, t),
		crossfade(a.g, b.g, t),
		crossfade(a.b, b.b, t),
		crossfade(a.a, b.a, t));
}

float haloBloomAmount(float raw) {
	raw = clamp(raw, 0.f, 1.5f);
	const float low = raw + 2.8f * raw * (1.f - raw);
	const float ramp = clamp((raw - 0.50f) / 0.50f, 0.f, 1.f);
	return std::max(0.f, low * (1.f + 1.40f * ramp * ramp));
}

template <typename DrawArc>
void drawCoalescedHaloSegments(
	float startAngle,
	float endAngle,
	float activeAngle,
	NVGcolor activeColor,
	NVGcolor inactiveColor,
	DrawArc&& drawArc,
	int segmentCount = 16) {
	const float step = (endAngle - startAngle) / float(segmentCount);
	const float position = clamp((activeAngle - startAngle) / std::max(step, 1e-6f), 0.f, float(segmentCount));
	const int completedSegments = std::min(segmentCount, int(position));
	const float completedEnd = startAngle + step * float(completedSegments);
	if (completedSegments > 0) {
		drawArc(startAngle, completedEnd, activeColor);
	}
	float inactiveStart = completedEnd;
	if (completedSegments < segmentCount) {
		const float partial = position - float(completedSegments);
		if (partial > 0.f) {
			const float partialEnd = completedEnd + step;
			drawArc(completedEnd, partialEnd, blendHaloColor(inactiveColor, activeColor, partial));
			inactiveStart = partialEnd;
		}
	}
	if (inactiveStart < endAngle) {
		drawArc(inactiveStart, endAngle, inactiveColor);
	}
}

template <typename DrawArc>
void drawHaloSegments(float startAngle, float endAngle, float valueNorm, bool bipolar,
	NVGcolor activeColor, NVGcolor inactiveColor, DrawArc&& drawArc) {
	const float valueAngle = crossfade(startAngle, endAngle, clamp(valueNorm, 0.f, 1.f));
	if (!bipolar) {
		drawCoalescedHaloSegments(startAngle, endAngle, valueAngle, activeColor, inactiveColor, drawArc);
		return;
	}
	const float centerAngle = 0.5f * (startAngle + endAngle);
	// Each half retains eight of the original sixteen segments.
	if (valueAngle < centerAngle) {
		drawCoalescedHaloSegments(startAngle, centerAngle, valueAngle,
			inactiveColor, activeColor, drawArc, 8);
		drawArc(centerAngle, endAngle, inactiveColor);
	}
	else {
		drawArc(startAngle, centerAngle, inactiveColor);
		drawCoalescedHaloSegments(centerAngle, endAngle, valueAngle,
			activeColor, inactiveColor, drawArc, 8);
	}
}

void uploadHaloColorArray(GLint location, const NVGcolor* colors, size_t count) {
	std::array<GLfloat, 4 * 19> values {};
	for (size_t i = 0; i < count; ++i) {
		values[i * 4 + 0] = colors[i].r;
		values[i * 4 + 1] = colors[i].g;
		values[i * 4 + 2] = colors[i].b;
		values[i * 4 + 3] = colors[i].a;
	}
	glUniform4fv(location, GLsizei(count), values.data());
}

} // namespace

namespace visual_assets {

void resetHaloKnob2DrawMetrics() {
	visual_assets::haloKnob2DrawMetrics() = {};
}

HaloKnob2DrawMetrics getHaloKnob2DrawMetrics() {
	return visual_assets::haloKnob2DrawMetrics();
}

} // namespace visual_assets

void LeviathanHaloKnob2::GlowArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;
	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloom = haloBloomAmount(settings::haloBrightness);
	if (bloom <= 0.001f) return;
	auto bloomColor = [bloom](NVGcolor color) { color.a *= bloom; return color; };
	auto drawStroke = [&](float a0, float a1, float width, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, mainRadius, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, width);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};
	auto drawSegmented = [&](float width, NVGcolor active, NVGcolor inactive) {
		drawHaloSegments(startAngle, endAngle, valueNorm, bipolar, active, inactive,
			[&](float a0, float a1, NVGcolor color) { drawStroke(a0, a1, width, color); });
	};
	nvgSave(args.vg);
	if (foreground) {
		drawSegmented(std::max(2.2f, 2.7f * scale), bloomColor(config.foregroundOuterActiveColor), bloomColor(config.foregroundOuterInactiveColor));
		drawSegmented(std::max(1.2f, 1.6f * scale), bloomColor(config.foregroundInnerActiveColor), bloomColor(config.foregroundInnerInactiveColor));
	}
	else {
		drawSegmented(std::max(5.8f, 6.4f * scale), bloomColor(config.backgroundOuterActiveColor), bloomColor(config.backgroundOuterInactiveColor));
		drawSegmented(std::max(3.8f, 4.6f * scale), bloomColor(config.backgroundMidActiveColor), bloomColor(config.backgroundMidInactiveColor));
		drawSegmented(std::max(2.4f, 3.0f * scale), bloomColor(config.backgroundInnerActiveColor), bloomColor(config.backgroundInnerInactiveColor));
	}
	nvgRestore(args.vg);
}

void LeviathanHaloKnob2::LightArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;
	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float zeroAngle = 0.5f * (startAngle + endAngle);
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float mainWidth = std::max(1.35f, diameterPx * (1.85f / 46.f));
	const float segmentWidth = mainWidth + 0.95f * scale;
	const float segmentRadius = mainRadius - 0.5f * (segmentWidth - mainWidth);
	const float guideRadius = diameterPx * (20.70f / 46.f);
	const float guideWidth = std::max(0.28f, diameterPx * (0.42f / 46.f));
	const float bloom = haloBloomAmount(settings::haloBrightness);
	auto bloomColor = [bloom](NVGcolor color) { color.a *= bloom; return color; };

	auto drawArcBand = [&](float a0, float a1, float radius, float width, NVGcolor color) {
		if (a1 <= a0) return;
		const float halfWidth = 0.5f * width;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radius + halfWidth, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, radius - halfWidth, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	};
	auto drawStroke = [&](float a0, float a1, float radius, float width, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radius, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, width);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};
	auto drawSegmentBand = [&](float a0, float a1, NVGcolor fill, NVGcolor highlight) {
		if (a1 <= a0) return;
		const float halfWidth = 0.5f * segmentWidth;
		drawArcBand(a0, a1, segmentRadius, segmentWidth + 0.48f * scale, nvgRGBA(0, 0, 4, 218));
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, segmentRadius + halfWidth, a0, a1, NVG_CW);
		nvgArc(args.vg, center.x, center.y, segmentRadius - halfWidth, a1, a0, NVG_CCW);
		nvgClosePath(args.vg);
		nvgFillPaint(args.vg, nvgLinearGradient(args.vg,
			center.x + std::cos(a0) * segmentRadius, center.y + std::sin(a0) * segmentRadius,
			center.x + std::cos(a1) * segmentRadius, center.y + std::sin(a1) * segmentRadius,
			fill, highlight));
		nvgFill(args.vg);
		drawStroke(a0, a1, segmentRadius + halfWidth * 0.84f,
			std::max(0.13f, segmentWidth * 0.09f), nvgRGBA(0, 1, 7, 172));
		drawStroke(a0, a1, segmentRadius - halfWidth * 0.52f,
			std::max(0.16f, segmentWidth * 0.12f), highlight);
	};
	auto drawValueSegments = [&]() {
		constexpr int count = 16;
		const float total = endAngle - startAngle;
		const float gap = std::max(0.010f, total * 0.009f);
		const float step = total / float(count);
		const float lo = bipolar ? std::min(activeAngle, zeroAngle) : startAngle;
		const float hi = bipolar ? std::max(activeAngle, zeroAngle) : activeAngle;
		for (int i = 0; i < count; ++i) {
			const float a0 = startAngle + step * float(i) + 0.5f * gap;
			const float a1 = startAngle + step * float(i + 1) - 0.5f * gap;
			const float mix = clamp((hi - a0) / std::max(a1 - a0, 1e-6f), 0.f, 1.f)
				- clamp((lo - a0) / std::max(a1 - a0, 1e-6f), 0.f, 1.f);
			drawSegmentBand(a0, a1,
				blendHaloColor(config.inactiveColor, config.activeColor, mix),
				blendHaloColor(config.inactiveHighlightColor, config.activeHighlightColor, mix));
		}
	};
	auto drawReflection = [&](float radius, float width, NVGcolor active, NVGcolor inactive) {
		drawHaloSegments(startAngle, endAngle, valueNorm, bipolar, active, inactive,
			[&](float a0, float a1, NVGcolor color) { drawStroke(a0, a1, radius, width, color); });
	};
	auto drawTerminator = [&](float angle, float direction) {
		const float terminatorSweep = 0.055f;
		const float a0 = angle + std::min(0.f, direction) * terminatorSweep;
		const float a1 = angle + std::max(0.f, direction) * terminatorSweep;
		drawArcBand(a0, a1, segmentRadius, segmentWidth + 1.15f * scale, nvgRGBA(0, 1, 8, 230));
		drawStroke(a0, a1, segmentRadius - segmentWidth * 0.30f,
			std::max(0.16f, 0.22f * scale), nvgRGBA(155, 170, 190, 48));
	};

	nvgSave(args.vg);
	const float dipRadius = mainRadius - mainWidth * 1.03f - 0.46f * scale;
	drawArcBand(startAngle, endAngle, mainRadius, mainWidth + 0.92f * scale, nvgRGBA(0, 0, 4, 248));
	drawArcBand(startAngle, endAngle, dipRadius, std::max(0.55f, 0.82f * scale), nvgRGBA(0, 1, 8, 216));
	drawStroke(startAngle, endAngle, guideRadius, guideWidth, bloomConfig.guideOuterColor);
	drawStroke(startAngle, endAngle, guideRadius - 0.20f * scale, std::max(0.18f, 0.24f * scale), bloomConfig.guideMidColor);
	drawStroke(startAngle, endAngle, mainRadius - mainWidth * 0.78f, std::max(0.16f, 0.20f * scale), bloomConfig.guideInnerColor);
	if (bloom > 0.001f) {
		drawReflection(dipRadius - 0.18f * scale, std::max(0.28f, 0.38f * scale), bloomColor(bloomConfig.reflectionOuterActiveColor), bloomColor(bloomConfig.reflectionOuterInactiveColor));
		drawReflection(dipRadius - 0.52f * scale, std::max(0.12f, 0.17f * scale), bloomColor(bloomConfig.reflectionInnerActiveColor), bloomColor(bloomConfig.reflectionInnerInactiveColor));
	}
	drawStroke(startAngle, endAngle, dipRadius + 0.46f * scale, std::max(0.15f, 0.22f * scale), nvgRGBA(0, 0, 4, 172));
	drawValueSegments();
	drawTerminator(startAngle, 1.f);
	drawTerminator(endAngle, -1.f);
	NVGpaint capShadow = nvgRadialGradient(args.vg, center.x, center.y + diameterPx * 0.045f,
		diameterPx * (11.0f / 46.f), diameterPx * (16.2f / 46.f),
		nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 76));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, diameterPx * (16.8f / 46.f));
	nvgFillPaint(args.vg, capShadow);
	nvgFill(args.vg);
	nvgRestore(args.vg);
}

struct LeviathanHaloKnob2::HaloGlSurface final : widget::OpenGlWidget {
	Config config;
	bool textureArc = true;
	bool bakingArc = false;
	bool compositeAtlas = false;
	bool atlasPrepared = false;
	bool needsGroupedAlpha = false;
	unsigned stableAtlasSteps = 0;
	float atlasBloom = -1.f, atlasDiameter = -1.f;
	float pendingAtlasBloom = -1.f;
	unsigned pendingAtlasSteps = 0;
	GLint uniformTextureMode = -1, uniformTextureArc = -1, uniformTextureState = -1;
	GLint uniformBakeArc = -1;
	std::shared_ptr<HaloArcImageSet> arcImages;
	std::string arcImageKey;
	std::shared_ptr<HaloCapRasterAtlas> capAtlas;
	GlowArcWidget* fallbackBackgroundGlow = nullptr;
	LightArcWidget* fallbackLightArc = nullptr;
	GlowArcWidget* fallbackForegroundGlow = nullptr;
	CapReflectionWidget* fallbackCapReflection = nullptr;
	float valueNorm = 0.5f;
	float arcNorm = 0.5f;
	float bloomAmount = 0.f;
	bool centerLit = false;
	GLuint program = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint vbo = 0;
	GLuint capTexture = 0;
	bool initAttempted = false;
	// This surface owns its program: immutable colors survive arc refreshes.
	bool uniformsInitialized = false;
	Vec uploadedSize = Vec(-1.f, -1.f);
	float uploadedCapValue = -1.f;
	GLint uniformLogicalSize = -1;
	GLint uniformArc = -1;
	GLint uniformBipolar = -1;
	GLint uniformBloomAmount = -1;
	GLint uniformLed = -1;
	GLint uniformBloom = -1;
	GLint uniformCapReflection = -1;
	GLint uniformCapAtlas = -1;
	GLint uniformCenterLit = -1;
	GLint uniformCapRotation = -1;
	bool shaderFailed = false;
	bool forceNanoVg = false;
	visual_assets::AdaptiveGlSurface fixedSurface;
	visual_assets::DrawVisibilityLease visibilityLease;

	void invalidateSurface() {
		stableAtlasSteps = 0;
		fixedSurface.markDirty();
		setDirty();
	}

	bool isSurfaceDirty() const {
		return fixedSurface.isDirty() || dirty;
	}

	explicit HaloGlSurface(
		Config config,
		const std::shared_ptr<window::Svg>& centerNormalSvg,
		const std::shared_ptr<window::Svg>& centerLitSvg,
		EclipseKnob::SvgLayer* fallbackCenterLayer)
		: config(config), capAtlas(getHaloCapRasterAtlas(centerNormalSvg, centerLitSvg, haloCapRasterScale(config.diameter))) {
		const Vec size(config.diameter, config.diameter);
		HaloNanoVgFallbackWidget* fallbackRoot = new HaloNanoVgFallbackWidget();
		fallbackRoot->box.size = size;
		addChild(fallbackRoot);

		fallbackBackgroundGlow = new GlowArcWidget();
		fallbackBackgroundGlow->box.size = size;
		fallbackBackgroundGlow->config = config.bloom;
		fallbackBackgroundGlow->bipolar = config.bipolar;
		fallbackRoot->addChild(fallbackBackgroundGlow);

		fallbackLightArc = new LightArcWidget();
		fallbackLightArc->box.size = size;
		fallbackLightArc->config = config.ledArc;
		fallbackLightArc->bloomConfig = config.bloom;
		fallbackLightArc->bipolar = config.bipolar;
		fallbackRoot->addChild(fallbackLightArc);

		fallbackForegroundGlow = new GlowArcWidget();
		fallbackForegroundGlow->box.size = size;
		fallbackForegroundGlow->foreground = true;
		fallbackForegroundGlow->config = config.bloom;
		fallbackForegroundGlow->bipolar = config.bipolar;
		fallbackRoot->addChild(fallbackForegroundGlow);

		// OpenGlWidget bypasses its framebuffer in browser previews and after a
		// shader failure. Keep the original SVG cap in that NanoVG-only subtree.
		fallbackRoot->addChild(fallbackCenterLayer);

		fallbackCapReflection = new CapReflectionWidget();
		fallbackCapReflection->box.size = size;
		fallbackCapReflection->config = config.bloom;
		fallbackCapReflection->bipolar = config.bipolar;
		fallbackRoot->addChild(fallbackCapReflection);
	}

	gl_lifecycle::ContextLease resourceContext;
	void retireRendererResources() {
		gl_lifecycle::retireObject(resourceContext, gl_lifecycle::ObjectKind::Program, program);
		gl_lifecycle::retireObject(resourceContext, gl_lifecycle::ObjectKind::Shader, vertexShader);
		gl_lifecycle::retireObject(resourceContext, gl_lifecycle::ObjectKind::Shader, fragmentShader);
		gl_lifecycle::retireObject(resourceContext, gl_lifecycle::ObjectKind::Buffer, vbo);
		gl_lifecycle::retireObject(resourceContext, gl_lifecycle::ObjectKind::Texture, capTexture);
	}

	~HaloGlSurface() override {
		retireRendererResources();
		releaseGlResources(false);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		arcImages.reset(); arcImageKey.clear(); atlasPrepared=false;
		fixedSurface.reset(true);
		OpenGlWidget::onContextDestroy(e);
		releaseGlResources(gl_lifecycle::resourceContextIsCurrent(resourceContext));
		shaderFailed = false;
		bypassed = forceNanoVg;
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		arcImages.reset(); arcImageKey.clear(); atlasPrepared=false;
		resourceContext.reset();
		OpenGlWidget::onContextCreate(e);
		fixedSurface.reset(false);
		// A Rack module widget can outlive the DAW editor scene. In that case it
		// may miss the old scene's ContextDestroyEvent but later receive the new
		// context event with nonzero GL names from the previous context. Those
		// numeric names are not portable and can alias unrelated objects in the
		// replacement context, so abandon them unconditionally and rebuild.
		releaseGlResources(false);
		shaderFailed = false;
		bypassed = forceNanoVg;
		invalidateSurface();
	}

	void releaseGlResources(bool deleteObjects) {
		if (deleteObjects) {
			if (capTexture) glDeleteTextures(1, &capTexture);
			if (vbo) glDeleteBuffers(1, &vbo);
			if (program) glDeleteProgram(program);
			if (vertexShader) glDeleteShader(vertexShader);
			if (fragmentShader) glDeleteShader(fragmentShader);
		}
		program = vertexShader = fragmentShader = vbo = capTexture = 0;
		uniformsInitialized = false;
		uploadedSize = Vec(-1.f, -1.f);
		uploadedCapValue = -1.f;
		initAttempted = false;
		uniformLogicalSize = uniformArc = uniformBipolar = uniformBloomAmount = uniformLed = uniformBloom = -1;
		uniformTextureMode = uniformTextureArc = uniformTextureState = -1;
		uniformBakeArc = -1;
		uniformCapReflection = -1;
		uniformCapAtlas = uniformCenterLit = uniformCapRotation = -1;
	}

	void step() override {
		// OpenGlWidget::step() dirties every frame. Halo surfaces are explicitly
		// invalidated so their framebuffer remains a single idle composite.
		if (config.suspendWhenUndrawn && !visibilityLease.active(system::getTime())) return;
		bypassed = forceNanoVg || shaderFailed;
		if (!bypassed && APP && APP->window) {
			const bool atlasReady = textureArc && prepareArcImages();
			atlasPrepared = atlasReady;
			if(atlasReady)stableAtlasSteps=std::min(stableAtlasSteps+1,6u);
			if (atlasReady && stableAtlasSteps < 6 && !needsGroupedAlpha) {setDirty(false); return;}
			compositeAtlas = atlasReady;
			float rackZoom = 1.f;
			if (APP->scene && APP->scene->rackScroll) {
				rackZoom = std::max(APP->scene->rackScroll->getZoom(), 1e-4f);
			}
			visual_assets::AdaptiveGlSurfacePolicy policy;
			policy.vertexAttributeCount = 1;
			policy.shaderOnlyState = true;
			policy.maxDensity = 3.f;
			policy.retainPeakCapacity = true;
			const bool measureSurface = isDragonKingDebugEnabled();
			const auto surfaceStart = measureSurface ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
			if (fixedSurface.renderIfNeeded(
				APP->window->vg, box.size, rackZoom, APP->window->pixelRatio, policy,
				isExtraGlValidationEnabled(),
				[](void* user, Vec activeSize, int viewportY) {
					static_cast<HaloGlSurface*>(user)->renderGlContent(activeSize, viewportY);
				}, this)) {
				if (measureSurface) visual_assets::haloKnob2DrawMetrics().stepSurfaceNs += haloElapsedNs(surfaceStart);
				// The adaptive surface now owns the current image. Prevent Rack's
				// dormant per-widget framebuffer from remaining logically dirty.
				setDirty(false);
			}
		}
		compositeAtlas = false;
		FramebufferWidget::step();
	}

	void draw(const DrawArgs& args) override {
		if (config.suspendWhenUndrawn) visibilityLease.renew(system::getTime());
		// Multiple sprites need a grouped composite when inherited opacity is
		// translucent. The next Step prepares it before NanoVG records the draw.
		needsGroupedAlpha = nvgGetGlobalTint(args.vg).a != 1.f;
		// On the first translucent frame, prefer the last grouped image to
		// applying opacity separately to overlapping sprites. Step refreshes it.
		if (!bypassed && textureArc && atlasPrepared && needsGroupedAlpha
			&& fixedSurface.draw(args, box.size)) return;
		if (!bypassed && textureArc && atlasPrepared && (stableAtlasSteps >= 6 || needsGroupedAlpha) && !fixedSurface.isDirty()
			&& fixedSurface.draw(args, box.size)) return;
		if (!bypassed && textureArc && atlasPrepared && arcImages && arcImages->valid(args.vg)) {
			const float position = clamp(arcNorm * 16.f, 0.f, 16.f);
			int base = std::min(16, int(position));
			int edge = base;
			float partial = position - base;
			if (config.bipolar && position < 8.f && partial > 0.f) {++base; partial = 1.f - partial;}
			nvgSave(args.vg);
			arcImages->tile(args.vg, base, box.size.x);
			if (edge < 16 && partial > 0.f) arcImages->tile(args.vg, 17 + edge, box.size.x, partial);
			const float diameter = box.size.x;
			nvgTranslate(args.vg, diameter*.5f, diameter*.5f);
			nvgRotate(args.vg, crossfade(-.83f*float(M_PI), .83f*float(M_PI), valueNorm));
			nvgTranslate(args.vg, -diameter*.5f, -diameter*.5f);
			arcImages->tile(args.vg, centerLit ? 34 : 33, diameter);
			nvgRestore(args.vg); return;
		}
		if (!bypassed && fixedSurface.draw(args, box.size)) return;
		widget::FramebufferWidget::draw(args);
	}

	bool prepareArcImages();

	void setForceNanoVg(bool force) {
		if (forceNanoVg == force) return;
		forceNanoVg = force;
		bypassed = forceNanoVg || shaderFailed;
		if (!bypassed) invalidateSurface();
	}

	// value drives the cap; arc drives the LED fill and its glow.
	void setVisualState(float value, float arc, float bloom) {
		value = clamp(value, 0.f, 1.f);
		arc = clamp(arc, 0.f, 1.f);
		bloom = std::max(0.f, bloom);
		if (fallbackBackgroundGlow) fallbackBackgroundGlow->valueNorm = arc;
		if (fallbackLightArc) fallbackLightArc->valueNorm = arc;
		if (fallbackForegroundGlow) fallbackForegroundGlow->valueNorm = arc;
		if (fallbackCapReflection) fallbackCapReflection->valueNorm = arc;
		if (std::fabs(value - valueNorm) <= 1e-6f && std::fabs(arc - arcNorm) <= 1e-6f
			&& std::fabs(bloom - bloomAmount) <= 1e-4f) return;
		valueNorm = value;
		arcNorm = arc;
		bloomAmount = bloom;
		invalidateSurface();
	}

	void setCenterLit(bool lit) {
		if (centerLit == lit) return;
		centerLit = lit;
		if (!bypassed) invalidateSurface();
	}

	bool ensureCapTextureReady() {
		if (capTexture) return true;
		if (!capAtlas || capAtlas->width <= 0 || capAtlas->height <= 0 || capAtlas->rgba.empty()) {
			WARN("HaloKnob2 cap raster atlas unavailable");
			return false;
		}
		glGenTextures(1, &capTexture);
		if (!capTexture) return false;
		glBindTexture(GL_TEXTURE_2D, capTexture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, capAtlas->width, capAtlas->height,
			0, GL_RGBA, GL_UNSIGNED_BYTE, capAtlas->rgba.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}

	static GLuint compileShader(GLenum type, const char* source) {
		GLuint shader = glCreateShader(type);
		if (!shader) return 0;
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		GLint ok = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if (ok == GL_TRUE) return shader;
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(static_cast<size_t>(std::max(logLength, 1)));
		GLsizei written = 0;
		glGetShaderInfoLog(shader, GLsizei(log.size()), &written, log.data());
		WARN("HaloKnob2 shader compile failed (type=%u): %s", unsigned(type), log.data());
		glDeleteShader(shader);
		return 0;
	}

	bool ensureShaderReady() {
		if (initAttempted) return program && vbo && capTexture;
		initAttempted = true;

		static const char* vertexSource = R"GLSL(
			#version 120
			attribute vec2 aPos;
			uniform vec2 uLogicalSize;
			varying vec2 vPos;
			void main() {
				vec2 ndc = vec2((aPos.x / uLogicalSize.x) * 2.0 - 1.0,
					1.0 - (aPos.y / uLogicalSize.y) * 2.0);
				gl_Position = vec4(ndc, 0.0, 1.0);
				vPos = aPos;
			}
		)GLSL";

		static const char* fragmentSource = R"GLSL(
			#version 120
			varying vec2 vPos;
			uniform vec2 uLogicalSize;
			uniform float uArc;
			uniform float uBakeArc;
			uniform float uTextureMode;
			uniform vec4 uTextureArc;
			uniform vec3 uTextureState;
			uniform float uBipolar;
			uniform float uBloomAmount;
			uniform vec4 uLed[4];
			uniform vec4 uBloom[17];
			uniform vec4 uCapReflection[4];
			uniform sampler2D uCapAtlas;
			uniform float uCenterLit;
			uniform vec2 uCapRotation;

			const float PI = 3.14159265358979323846;
			const float TAU = 6.28318530717958647692;

			void overLayer(inout vec4 accum, vec4 color, float coverage) {
				float alpha = clamp(color.a * coverage, 0.0, 1.0);
				// accum.rgb is premultiplied. Match NanoVG source-over draw order:
				// each newly submitted layer is placed in front of prior layers.
				accum.rgb = color.rgb * alpha + accum.rgb * (1.0 - alpha);
				accum.a = alpha + accum.a * (1.0 - alpha);
			}

			void overPremultipliedLayer(inout vec4 accum, vec4 color, float coverage) {
				float alpha = clamp(color.a * coverage, 0.0, 1.0);
				accum.rgb = color.rgb * coverage + accum.rgb * (1.0 - alpha);
				accum.a = alpha + accum.a * (1.0 - alpha);
			}

			float band(float radius, float centerRadius, float width) {
				float d = abs(radius - centerRadius);
				float aa = max(fwidth(radius), 0.18);
				return 1.0 - smoothstep(0.5 * width - aa, 0.5 * width + aa, d);
			}

			vec4 sprite(float index, vec2 uv) {
				vec2 origin = vec2(mod(index, 5.0), floor(index / 5.0)) * uTextureArc.y + vec2(4.0);
				return texture2D(uCapAtlas, (origin + uv * uTextureArc.x) / uTextureArc.zw);
			}
			void main() {
				if (uTextureMode > 0.5) {
					vec2 uv = vPos / uLogicalSize;
					vec4 color = sprite(uTextureState.x, uv);
					vec4 delta = sprite(17.0 + uTextureState.y, uv) * uTextureState.z;
					color = delta + color * (1.0 - delta.a);
					vec2 p = uv - vec2(0.5);
					vec2 capUv = vec2(uCapRotation.x*p.x + uCapRotation.y*p.y,
						-uCapRotation.y*p.x + uCapRotation.x*p.y) + vec2(0.5);
					float bounds = step(0.0,capUv.x)*step(capUv.x,1.0)*step(0.0,capUv.y)*step(capUv.y,1.0);
					vec4 cap = sprite(33.0 + step(0.5,uCenterLit), capUv) * bounds;
					gl_FragColor = cap + color*(1.0-cap.a); return;
				}
				float diameter = min(uLogicalSize.x, uLogicalSize.y);
				float scale = diameter / 46.0;
				vec2 p = vPos - 0.5 * uLogicalSize;
				float radius = length(p);
				float theta = atan(p.y, p.x);
				float start = -0.5 * PI - 0.83 * PI;
				float sweep = 1.66 * PI;
				float along = mod(theta - start + TAU, TAU);
				float angularAa = max(fwidth(along), 0.0015);
				float arcMask = smoothstep(0.0, angularAa, along)
					* (1.0 - smoothstep(sweep - angularAa, sweep, along));

				float segmentPos = clamp(along / sweep * 16.0, 0.0, 15.9999);
				float segmentIndex = floor(segmentPos);
				float segmentLocal = fract(segmentPos);
				float gapEdge = max(fwidth(segmentLocal), 0.012);
				float segmentMask = smoothstep(0.072, 0.072 + gapEdge, segmentLocal)
					* (1.0 - smoothstep(0.928 - gapEdge, 0.928, segmentLocal));
				float lowValue = uBipolar > 0.5 ? min(uArc, 0.5) : 0.0;
				float highValue = uBipolar > 0.5 ? max(uArc, 0.5) : uArc;
				float haloMix = clamp(highValue * 16.0 - segmentIndex, 0.0, 1.0)
					- clamp(lowValue * 16.0 - segmentIndex, 0.0, 1.0);
				float activeMix = clamp((highValue * 16.0 - segmentIndex - 0.072) / 0.856, 0.0, 1.0)
					- clamp((lowValue * 16.0 - segmentIndex - 0.072) / 0.856, 0.0, 1.0);
				vec4 core = mix(uLed[2], uLed[0], activeMix);
				vec4 hot = mix(uLed[3], uLed[1], activeMix);
				vec4 valueColor = mix(core, hot, clamp((segmentLocal - 0.072) / 0.856, 0.0, 1.0));

				vec4 accum = vec4(0.0);
				float mainRadius = diameter * (18.15 / 46.0);
				float mainWidth = max(1.35, diameter * (1.85 / 46.0));
				float segmentWidth = mainWidth + 0.95 * scale;
				float segmentRadius = mainRadius - 0.5 * (segmentWidth - mainWidth);
				float dipRadius = mainRadius - mainWidth * 1.03 - 0.46 * scale;
				float guideRadius = diameter * (20.70 / 46.0);
				vec4 activeGlow;
				vec4 inactiveGlow;

				if (uBloomAmount > 0.001 && along <= sweep) {
					activeGlow = mix(uBloom[1], uBloom[0], haloMix);
					inactiveGlow = mix(uBloom[3], uBloom[2], haloMix);
					vec4 innerGlow = mix(uBloom[5], uBloom[4], haloMix);
					// These are deliberately finite-width bands. The NanoVG original
					// used three overlapping strokes rather than a Gaussian blur.
					overLayer(accum, activeGlow, arcMask * band(radius, mainRadius, max(5.8, 6.4 * scale)) * uBloomAmount);
					overLayer(accum, inactiveGlow, arcMask * band(radius, mainRadius, max(3.8, 4.6 * scale)) * uBloomAmount);
					overLayer(accum, innerGlow, arcMask * band(radius, mainRadius, max(2.4, 3.0 * scale)) * uBloomAmount);
				}

				overLayer(accum, vec4(0.0, 0.0, 0.016, 0.97), arcMask * band(radius, mainRadius, mainWidth + 0.92 * scale));
				overLayer(accum, vec4(0.0, 0.004, 0.031, 0.85), arcMask * band(radius, dipRadius, max(0.55, 0.82 * scale)));
				overLayer(accum, uBloom[14], arcMask * band(radius, guideRadius, max(0.28, 0.42 * scale)));
				overLayer(accum, uBloom[15], arcMask * band(radius, guideRadius - 0.20 * scale, max(0.18, 0.24 * scale)));
				overLayer(accum, uBloom[16], arcMask * band(radius, mainRadius - mainWidth * 0.78, max(0.16, 0.20 * scale)));

				if (uBloomAmount > 0.001 && along <= sweep) {
					vec4 reflectionOuter = mix(uBloom[11], uBloom[10], haloMix);
					vec4 reflectionInner = mix(uBloom[13], uBloom[12], haloMix);
					overLayer(accum, reflectionOuter, arcMask * band(radius, dipRadius - 0.18 * scale, max(0.28, 0.38 * scale)) * uBloomAmount);
					overLayer(accum, reflectionInner, arcMask * band(radius, dipRadius - 0.52 * scale, max(0.12, 0.17 * scale)) * uBloomAmount);
				}
				overLayer(accum, vec4(0.0, 0.0, 0.016, 0.67), arcMask * band(radius, dipRadius + 0.46 * scale, max(0.15, 0.22 * scale)));

				float segmentBody = arcMask * segmentMask * band(radius, segmentRadius, segmentWidth);
				// A small gap-limited spill makes energized segments illuminate their
				// housing without turning the whole ring into a continuous glow band.
				vec4 localSpillColor = vec4(mix(uLed[1].rgb, uLed[0].rgb, 0.42), 0.16);
				overLayer(accum, localSpillColor,
					arcMask * segmentMask * band(radius, segmentRadius, segmentWidth + 2.35 * scale)
					* activeMix * uBloomAmount);

				overLayer(accum, vec4(0.0, 0.0, 0.016, 0.86), arcMask * segmentMask * band(radius, segmentRadius, segmentWidth + 0.48 * scale));
				overLayer(accum, valueColor, segmentBody);

				// Glass bevel: darken both radial and angular edges while leaving the
				// center of each LED optically clear.
				float radialLocal = clamp((radius - (segmentRadius - 0.5 * segmentWidth)) / segmentWidth, 0.0, 1.0);
				float radialEdgeDistance = min(radialLocal, 1.0 - radialLocal);
				float angularLocal = clamp((segmentLocal - 0.072) / 0.856, 0.0, 1.0);
				float angularEdgeDistance = min(angularLocal, 1.0 - angularLocal);
				float glassEdge = 1.0 - smoothstep(0.025, 0.19, min(radialEdgeDistance, angularEdgeDistance));
				overLayer(accum, vec4(0.0, 0.008, 0.025, 0.34), segmentBody * glassEdge);

				// Concentrated emissive core. Active LEDs gain depth and intensity,
				// while inactive glass receives only a restrained internal reflection.
				float coreBand = arcMask * segmentMask
					* band(radius, segmentRadius - segmentWidth * 0.08, segmentWidth * 0.38);
				vec4 emissiveCore = vec4(mix(core.rgb, hot.rgb, 0.72), mix(0.055, 0.30, activeMix));
				overLayer(accum, emissiveCore, coreBand);

				// Fixed panel-space light produces a polished glass glint. Polynomial
				// shaping avoids pow() in this fragment path.
				vec2 radialDirection = p / max(radius, 0.001);
				float lightFacing = clamp(dot(radialDirection, vec2(-0.55, -0.835)) * 0.5 + 0.5, 0.0, 1.0);
				float specular = lightFacing * lightFacing;
				specular *= specular;
				float glassGlint = specular * mix(0.32, 1.0, activeMix);
				overLayer(accum, vec4(0.80, 0.96, 1.0, 0.19), segmentBody * glassGlint);

				// Retain the original fine rim strokes over the glass treatment.
				overLayer(accum, vec4(0.0, 0.004, 0.027, 0.67), arcMask * segmentMask * band(radius, segmentRadius + segmentWidth * 0.42, max(0.13, segmentWidth * 0.09)));
				overLayer(accum, hot, arcMask * segmentMask * band(radius, segmentRadius - segmentWidth * 0.26, max(0.16, segmentWidth * 0.12)));

				float terminator = (1.0 - smoothstep(0.0, 0.055, along))
					+ smoothstep(sweep - 0.055, sweep, along);
				overLayer(accum, vec4(0.0, 0.004, 0.031, 0.90), clamp(terminator, 0.0, 1.0) * arcMask * band(radius, segmentRadius, segmentWidth + 1.15 * scale));
				overLayer(accum, vec4(0.608, 0.667, 0.745, 0.188), clamp(terminator, 0.0, 1.0) * arcMask
					* band(radius, segmentRadius - segmentWidth * 0.30, max(0.16, 0.22 * scale)));

				float shadowRadius = diameter * (16.8 / 46.0);
				float shadow = smoothstep(diameter * (11.0 / 46.0), diameter * (16.2 / 46.0), length(p - vec2(0.0, diameter * 0.045)))
					* (1.0 - smoothstep(shadowRadius - 0.35, shadowRadius + 0.35, radius));
				overLayer(accum, vec4(0.0, 0.0, 0.0, 0.30), shadow);

				if (uBloomAmount > 0.001 && along <= sweep) {
					vec4 foregroundOuter = mix(uBloom[7], uBloom[6], haloMix);
					vec4 foregroundInner = mix(uBloom[9], uBloom[8], haloMix);
					overLayer(accum, foregroundOuter, arcMask * band(radius, mainRadius, max(2.2, 2.7 * scale)) * uBloomAmount);
					overLayer(accum, foregroundInner, arcMask * band(radius, mainRadius, max(1.2, 1.6 * scale)) * uBloomAmount);
				}

				if (uBakeArc < 0.5) {
				// The source SVG is rasterized once into a normal/lit atlas. Rotate its
				// sampling coordinates instead of rebuilding a final-orientation FBO.
				vec2 capLocal = vec2(
					uCapRotation.x * p.x + uCapRotation.y * p.y,
					-uCapRotation.y * p.x + uCapRotation.x * p.y);
				vec2 capUv = capLocal / diameter + vec2(0.5);
				float capBounds = step(0.0, capUv.x) * step(capUv.x, 1.0)
					* step(0.0, capUv.y) * step(capUv.y, 1.0);
				float atlasOffset = 0.5 * step(0.5, uCenterLit);
				vec4 capColor = texture2D(uCapAtlas, vec2(atlasOffset + 0.5 * capUv.x, capUv.y));
				overPremultipliedLayer(accum, capColor, capBounds);

				// Cap glass reflection remains above the rotated cap. Match the NanoVG
				// fallback's coalesced 16-step active/inactive color transition.
				if (uBloomAmount > 0.001 && along <= sweep) {
					float rimRadius = diameter * (14.62 / 46.0);
					vec4 capReflectionOuter = mix(uCapReflection[1], uCapReflection[0], haloMix);
					vec4 capReflectionInner = mix(uCapReflection[3], uCapReflection[2], haloMix);
					overLayer(accum, capReflectionOuter,
						arcMask * band(radius, rimRadius, max(0.30, 0.42 * scale)) * uBloomAmount);
					overLayer(accum, capReflectionInner,
						arcMask * band(radius, rimRadius - 0.34 * scale, max(0.12, 0.17 * scale)) * uBloomAmount);
				}

				}
				// Rack framebuffer images are tagged NVG_IMAGE_PREMULTIPLIED.
				gl_FragColor = accum;
			}
		)GLSL";

		vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
		fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
		if (!vertexShader || !fragmentShader) {
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		program = glCreateProgram();
		if (!program) {
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glBindAttribLocation(program, 0, "aPos");
		glLinkProgram(program);
		GLint linked = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linked);
		if (linked != GL_TRUE) {
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<char> log(static_cast<size_t>(std::max(logLength, 1)));
			GLsizei written = 0;
			glGetProgramInfoLog(program, GLsizei(log.size()), &written, log.data());
			WARN("HaloKnob2 shader link failed: %s", log.data());
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		uniformLogicalSize = glGetUniformLocation(program, "uLogicalSize");
		uniformArc = glGetUniformLocation(program, "uArc");
		uniformBakeArc = glGetUniformLocation(program, "uBakeArc");
		uniformTextureMode = glGetUniformLocation(program, "uTextureMode");
		uniformTextureArc = glGetUniformLocation(program, "uTextureArc");
		uniformTextureState = glGetUniformLocation(program, "uTextureState");
		uniformBipolar = glGetUniformLocation(program, "uBipolar");
		uniformBloomAmount = glGetUniformLocation(program, "uBloomAmount");
		uniformLed = glGetUniformLocation(program, "uLed[0]");
		uniformBloom = glGetUniformLocation(program, "uBloom[0]");
		uniformCapReflection = glGetUniformLocation(program, "uCapReflection[0]");
		uniformCapAtlas = glGetUniformLocation(program, "uCapAtlas");
		uniformCenterLit = glGetUniformLocation(program, "uCenterLit");
		uniformCapRotation = glGetUniformLocation(program, "uCapRotation");
		if (uniformTextureMode < 0 || uniformTextureArc < 0 || uniformTextureState < 0 || uniformBakeArc < 0
			|| uniformLogicalSize < 0 || uniformArc < 0 || uniformBipolar < 0 || uniformBloomAmount < 0
			|| uniformLed < 0 || uniformBloom < 0 || uniformCapReflection < 0 || uniformCapAtlas < 0
			|| uniformCenterLit < 0 || uniformCapRotation < 0) {
			WARN("HaloKnob2 shader uniform lookup failed");
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		glGenBuffers(1, &vbo);
		if (!vbo) {
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		if (!ensureCapTextureReady()) {
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		shaderFailed = false;
		bypassed = forceNanoVg;
		return vbo != 0 && capTexture != 0;
	}

	void drawFixedFallback(float w, float h) {
		// Only the emergency fixed-function draw touches compatibility state.
		// Keep it local so the normal shader refresh uses the restricted guard.
		glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT | GL_TRANSFORM_BIT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		const float cx = 0.5f * w;
		const float cy = 0.5f * h;
		const float radius = std::min(w, h) * (18.15f / 46.f);
		const float start = -0.5f * float(M_PI) - 0.83f * float(M_PI);
		const float sweep = 1.66f * float(M_PI);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(2.f);
		glBegin(GL_LINES);
		for (int i = 0; i < 16; ++i) {
			const float a0 = start + sweep * (float(i) + 0.08f) / 16.f;
			const float a1 = start + sweep * (float(i) + 0.92f) / 16.f;
			const float low = config.bipolar ? std::min(arcNorm, 0.5f) : 0.f;
			const float high = config.bipolar ? std::max(arcNorm, 0.5f) : arcNorm;
			const float mix = clamp(high * 16.f - float(i), 0.f, 1.f)
				- clamp(low * 16.f - float(i), 0.f, 1.f);
			const NVGcolor color = blendHaloColor(config.ledArc.inactiveColor, config.ledArc.activeColor, mix);
			glColor4f(color.r, color.g, color.b, color.a);
			glVertex2f(cx + std::cos(a0) * radius, cy + std::sin(a0) * radius);
			glVertex2f(cx + std::cos(a1) * radius, cy + std::sin(a1) * radius);
		}
		glEnd();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();
	}

	void renderGlContent(Vec framebufferSize, int viewportY = 0) {
		NVGcontext* currentResourceVg = (APP && APP->window) ? APP->window->vg : nullptr;
		if (!gl_lifecycle::resourceContextMatches(resourceContext, currentResourceVg)) {
			releaseGlResources(false);
			resourceContext = gl_lifecycle::acquireResourceContext(currentResourceVg);
		}

		const bool measure = isDragonKingDebugEnabled();
		const auto profileStart = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
		const int activeWidth = std::max(1, int(std::lround(framebufferSize.x)));
		const int activeHeight = std::max(1, int(std::lround(framebufferSize.y)));
		glViewport(0, viewportY, activeWidth, activeHeight);
		// Filtering the retained texture at fractional zoom can reach beyond
		// the active viewport. Keep all padding transparent, including pixels
		// left over from an earlier larger render. This only runs on redraw.
		glDisable(GL_SCISSOR_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
		const float w = std::max(box.size.x, 1.f);
		const float h = std::max(box.size.y, 1.f);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		if (isExtraGlValidationEnabled() && program && vbo) {
			if (!gl_lifecycle::isValidProgramBufferPair(program, vbo)
				|| (capTexture && !gl_lifecycle::areValidTextures({capTexture}))) {
				releaseGlResources(false);
			}
		}
		if (!ensureShaderReady()) {
			// Browser/module previews bypass nested framebuffers automatically and
			// draw our NanoVG children. Do the same permanently for this context if
			// GLSL initialization fails, after this one-frame fixed-GL fallback.
			shaderFailed = true;
			drawFixedFallback(w, h);
			glDisable(GL_BLEND);
			if (measure) {
				visual_assets::haloKnob2DrawMetrics().glSurfaceFramebufferNs += haloElapsedNs(profileStart);
				++visual_assets::haloKnob2DrawMetrics().glSurfaceFramebufferDraws;
			}
			return;
		}
		// The fragment shader has already composited all layers and emits a
		// premultiplied result, so blending it over the transparent FBO would
		// apply alpha a second time.
		glDisable(GL_BLEND);

		glUseProgram(program);
		if (!uniformsInitialized) {
			const NVGcolor ledColors[] = {
				config.ledArc.activeColor,
				config.ledArc.activeHighlightColor,
				config.ledArc.inactiveColor,
				config.ledArc.inactiveHighlightColor,
			};
			const NVGcolor bloomColors[] = {
				config.bloom.backgroundOuterActiveColor,
				config.bloom.backgroundOuterInactiveColor,
				config.bloom.backgroundMidActiveColor,
				config.bloom.backgroundMidInactiveColor,
				config.bloom.backgroundInnerActiveColor,
				config.bloom.backgroundInnerInactiveColor,
				config.bloom.foregroundOuterActiveColor,
				config.bloom.foregroundOuterInactiveColor,
				config.bloom.foregroundInnerActiveColor,
				config.bloom.foregroundInnerInactiveColor,
				config.bloom.reflectionOuterActiveColor,
				config.bloom.reflectionOuterInactiveColor,
				config.bloom.reflectionInnerActiveColor,
				config.bloom.reflectionInnerInactiveColor,
				config.bloom.guideOuterColor,
				config.bloom.guideMidColor,
				config.bloom.guideInnerColor,
			};
			const NVGcolor capReflectionColors[] = {
				config.bloom.capReflectionOuterActiveColor,
				config.bloom.capReflectionOuterInactiveColor,
				config.bloom.capReflectionInnerActiveColor,
				config.bloom.capReflectionInnerInactiveColor,
			};

			glUniform1f(uniformBipolar, config.bipolar ? 1.f : 0.f);
			uploadHaloColorArray(uniformLed, ledColors, 4);
			uploadHaloColorArray(uniformBloom, bloomColors, 17);
			uploadHaloColorArray(uniformCapReflection, capReflectionColors, 4);
			glUniform1i(uniformCapAtlas, 0);
			uniformsInitialized = true;
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		if (uploadedSize.x != w || uploadedSize.y != h) {
			glUniform2f(uniformLogicalSize, w, h);
			const std::array<GLfloat, 8> vertices {{0.f, 0.f, w, 0.f, 0.f, h, w, h}};
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
			uploadedSize = Vec(w, h);
		}
		glUniform1f(uniformArc, arcNorm);
		glUniform1f(uniformBakeArc, bakingArc ? 1.f : 0.f);
		glUniform1f(uniformTextureMode, compositeAtlas ? 1.f : 0.f);
		if (compositeAtlas) {
			const float position=clamp(arcNorm*16.f,0.f,16.f);
			int base=std::min(16,int(position)),edge=std::min(15,base);
			float partial=position-base;
			if(config.bipolar && position<8.f && partial>0.f) {++base;partial=1.f-partial;}
			glUniform3f(uniformTextureState,float(base),float(edge),partial);
			glUniform4f(uniformTextureArc,float(arcImages->side),float(arcImages->stride),float(arcImages->width),float(arcImages->height));
		}
		glUniform1f(uniformBloomAmount, bloomAmount);
		glUniform1f(uniformCenterLit, centerLit ? 1.f : 0.f);
		if (uploadedCapValue != valueNorm) {
			const float capAngle = crossfade(-0.83f * float(M_PI), 0.83f * float(M_PI), valueNorm);
			glUniform2f(uniformCapRotation, std::cos(capAngle), std::sin(capAngle));
			uploadedCapValue = valueNorm;
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, compositeAtlas ? nvglImageHandleGL2(currentResourceVg, arcImages->image) : capTexture);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
		glDisable(GL_BLEND);
		if (measure) {
			visual_assets::haloKnob2DrawMetrics().glSurfaceFramebufferNs += haloElapsedNs(profileStart);
			++visual_assets::haloKnob2DrawMetrics().glSurfaceFramebufferDraws;
		}
	}

	void drawFramebuffer() override {
		renderGlContent(getFramebufferSize());
	}
};

void LeviathanHaloKnob2::CapReflectionWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;
	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float rimRadius = diameterPx * (14.62f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float bloom = haloBloomAmount(settings::haloBrightness);
	if (bloom <= 0.001f) return;
	auto bloomColor = [bloom](NVGcolor color) { color.a *= bloom; return color; };
	auto strokeArc = [&](float a0, float a1, float radiusPx, float widthPx, NVGcolor color) {
		if (a1 <= a0) return;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, radiusPx, a0, a1, NVG_CW);
		nvgStrokeColor(args.vg, color);
		nvgStrokeWidth(args.vg, widthPx);
		nvgLineCap(args.vg, NVG_BUTT);
		nvgStroke(args.vg);
	};
	nvgSave(args.vg);
	drawHaloSegments(startAngle, endAngle, valueNorm, bipolar,
		bloomColor(config.capReflectionOuterActiveColor), bloomColor(config.capReflectionOuterInactiveColor),
		[&](float a0, float a1, NVGcolor color) { strokeArc(a0, a1, rimRadius, std::max(0.30f, 0.42f * scale), color); });
	drawHaloSegments(startAngle, endAngle, valueNorm, bipolar,
		bloomColor(config.capReflectionInnerActiveColor), bloomColor(config.capReflectionInnerInactiveColor),
		[&](float a0, float a1, NVGcolor color) { strokeArc(a0, a1, rimRadius - 0.34f * scale, std::max(0.12f, 0.17f * scale), color); });
	nvgRestore(args.vg);
}

LeviathanHaloKnob2::LeviathanHaloKnob2() : LeviathanHaloKnob2(Config()) {}

LeviathanHaloKnob2::Config LeviathanHaloKnob2::brightOrangeConfig() {
	Config config;
	config.ledArc.activeColor = nvgRGBA(255, 184, 0, 255);
	config.ledArc.activeHighlightColor = nvgRGBA(255, 232, 82, 232);
	config.ledArc.inactiveColor = nvgRGBA(158, 58, 16, 216);
	config.ledArc.inactiveHighlightColor = nvgRGBA(220, 94, 30, 168);
	config.bloom.backgroundOuterActiveColor = nvgRGBA(255, 148, 0, 50);
	config.bloom.backgroundOuterInactiveColor = nvgRGBA(130, 42, 10, 30);
	config.bloom.backgroundMidActiveColor = nvgRGBA(255, 172, 0, 80);
	config.bloom.backgroundMidInactiveColor = nvgRGBA(160, 50, 12, 50);
	config.bloom.backgroundInnerActiveColor = nvgRGBA(255, 204, 20, 122);
	config.bloom.backgroundInnerInactiveColor = nvgRGBA(204, 68, 16, 72);
	config.bloom.foregroundOuterActiveColor = nvgRGBA(255, 214, 34, 74);
	config.bloom.foregroundOuterInactiveColor = nvgRGBA(206, 72, 18, 44);
	config.bloom.foregroundInnerActiveColor = nvgRGBA(255, 244, 118, 62);
	config.bloom.foregroundInnerInactiveColor = nvgRGBA(236, 104, 34, 32);
	config.bloom.reflectionOuterActiveColor = nvgRGBA(255, 174, 0, 70);
	config.bloom.reflectionOuterInactiveColor = nvgRGBA(144, 44, 10, 68);
	config.bloom.reflectionInnerActiveColor = nvgRGBA(255, 224, 36, 62);
	config.bloom.reflectionInnerInactiveColor = nvgRGBA(218, 86, 22, 48);
	config.bloom.guideOuterColor = nvgRGBA(255, 210, 38, 84);
	config.bloom.guideMidColor = nvgRGBA(186, 58, 14, 58);
	config.bloom.guideInnerColor = nvgRGBA(255, 238, 98, 68);
	config.bloom.capReflectionOuterActiveColor = nvgRGBA(255, 188, 0, 96);
	config.bloom.capReflectionOuterInactiveColor = nvgRGBA(188, 62, 16, 82);
	config.bloom.capReflectionInnerActiveColor = nvgRGBA(255, 240, 108, 72);
	config.bloom.capReflectionInnerInactiveColor = nvgRGBA(236, 106, 36, 54);
	return config;
}

LeviathanHaloKnob2::LeviathanHaloKnob2(Config config) : config(std::move(config)) {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;
	box.size = Vec(this->config.diameter, this->config.diameter);
	lastBloomAmount = settings::haloBrightness;

	const std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnob2Back.svg");
	centerNormalSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenter.svg");
	centerLitSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenterLit.svg");

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->rotateWithValue = false;
	if (backLayer->cachedSvgFb) {
		backLayer->cachedSvgFb->bypassed = true;
	}
	addChild(backLayer);

	centerLayer = new EclipseKnob::SvgLayer();
	centerLayer->setSvg(centerNormalSvg);
	centerLayer->box.size = box.size;
	centerLayer->minAngle = minAngle;
	centerLayer->maxAngle = maxAngle;
	centerLayer->valueNorm = normalizedParamValue();
	centerLayer->rotateWithValue = true;
	if (centerLayer->cachedSvgFb) {
		// The fallback is correctness-first and must not transform a nested
		// framebuffer, which was the source of position-dependent missing caps.
		centerLayer->cachedSvgFb->bypassed = true;
	}

	// The normal module path samples a one-time SVG raster atlas and performs
	// cap rotation in the same fixed-position GL quad as the LED arc. The SVG
	// layer is owned by HaloGlSurface only for browser/shader-failure bypass.
	glSurface = new HaloGlSurface(this->config, centerNormalSvg, centerLitSvg, centerLayer);
	glSurface->box.size = box.size;
	glSurface->dirtyOnSubpixelChange = false;
	glSurface->setVisualState(normalizedParamValue(), normalizedParamValue(), haloBloomAmount(lastBloomAmount));
	addChild(glSurface);

}

void LeviathanHaloKnob2::updateCenterSvg() {
	const bool shouldLight = hovered || dragging;
	if (centerLit == shouldLight) return;
	centerLit = shouldLight;
	if (centerLayer) {
		centerLayer->setSvg(centerLit ? centerLitSvg : centerNormalSvg);
		centerLayer->box.size = box.size;
	}
	if (glSurface) glSurface->setCenterLit(centerLit);
}

void LeviathanHaloKnob2::step() {
	app::Knob::step();
	const float bloom = settings::haloBrightness;
	const float value = normalizedParamValue();
	const bool visualChanged = std::fabs(value - lastVisualValue) > 1e-6f;
	if (visualChanged && centerLayer) centerLayer->valueNorm = value;
	float arc = value;
	bool arcChanged = false;
	if (arcSource) {
		// Quantize, then rate-limit: a steady or slowly moving source causes no
		// redraw, and live motion redraws at most once per arcInterval.
		const float live = clamp(arcSource->load(std::memory_order_relaxed), 0.f, 1.f);
		const float quantized = std::round(live / arcQuantum) * arcQuantum;
		arc = lastArcValue;
		if (quantized != lastArcValue) {
			const double now = system::getTime();
			if (lastArcValue < 0.f || now - lastArcTime >= arcInterval) {
				arc = quantized;
				lastArcValue = quantized;
				lastArcTime = now;
				arcChanged = true;
			}
		}
	}
	if (visualChanged || arcChanged || std::fabs(bloom - lastBloomAmount) > 1e-4f) {
		lastVisualValue = value;
		lastBloomAmount = bloom;
		if (glSurface) glSurface->setVisualState(value, arcSource ? arc : value, haloBloomAmount(bloom));
	}
}

void LeviathanHaloKnob2::onEnter(const event::Enter& e) {
	hovered = true;
	updateCenterSvg();
	app::Knob::onEnter(e);
}

void LeviathanHaloKnob2::onLeave(const event::Leave& e) {
	hovered = false;
	updateCenterSvg();
	app::Knob::onLeave(e);
}

void LeviathanHaloKnob2::onDragStart(const event::DragStart& e) {
	dragging = true;
	updateCenterSvg();
	app::Knob::onDragStart(e);
}

void LeviathanHaloKnob2::onDragEnd(const event::DragEnd& e) {
	dragging = false;
	updateCenterSvg();
	app::Knob::onDragEnd(e);
}

void LeviathanHaloKnob2::onChange(const ChangeEvent& e) {
	app::Knob::onChange(e);
	const float value = normalizedParamValue();
	lastVisualValue = value;
	if (centerLayer) centerLayer->valueNorm = value;
	if (glSurface) glSurface->setVisualState(value, arcSource && lastArcValue >= 0.f ? lastArcValue : value, haloBloomAmount(lastBloomAmount));
}

bool LeviathanHaloKnob2::isVisualDirty() const {
	return (glSurface && glSurface->isSurfaceDirty())
		|| (backLayer && backLayer->cachedSvgFb && backLayer->cachedSvgFb->dirty);
}

void LeviathanHaloKnob2::setForceNanoVgLedRenderer(bool force) {
	if (glSurface) glSurface->setForceNanoVg(force);
}

float LeviathanHaloKnob2::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float range = pq->getMaxValue() - minValue;
	if (range <= 1e-6f) return 0.5f;
	const float normalized = clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
	return config.visualValue ? config.visualValue(pq, normalized) : normalized;
}

void LeviathanHaloKnob2::setTextureArcRenderer(bool enabled) {
	if (glSurface) {glSurface->textureArc = enabled; glSurface->invalidateSurface();}
}

bool LeviathanHaloKnob2::HaloGlSurface::prepareArcImages() {
	NVGcontext* vg = APP->window->vg;
	if(arcImages && atlasBloom==bloomAmount && atlasDiameter==box.size.x && arcImages->valid(vg))return true;
	// While the global bloom control is moving, retain the analytic renderer.
	// Rebuild the shared texture only after six unchanged UI steps.
	if(arcImages && atlasBloom!=bloomAmount) {
		if(pendingAtlasBloom!=bloomAmount) {pendingAtlasBloom=bloomAmount;pendingAtlasSteps=0;}
		if(pendingAtlasSteps<6) {++pendingAtlasSteps;return false;}
	}
	// Three raster samples per logical pixel, shared by appearance.
	const int side = std::max(1, int(std::ceil(box.size.x * 3.f)));
	std::string key;
	auto append = [&](const void* data, size_t size) {key.append(static_cast<const char*>(data), size);};
	static_assert(sizeof(LedArcConfig)==4*sizeof(NVGcolor), "color-only atlas key");
	static_assert(sizeof(BloomConfig)==21*sizeof(NVGcolor), "color-only atlas key");
	append(&config.ledArc,sizeof(config.ledArc)); append(&config.bloom,sizeof(config.bloom));
	append(&config.bipolar,sizeof(config.bipolar)); append(&bloomAmount,sizeof(bloomAmount));
	append(&box.size.x,sizeof(box.size.x)); append(&side,sizeof(side));
	if (arcImages && key==arcImageKey && arcImages->valid(vg)) return true;
	using AtlasKey=std::pair<NVGcontext*,std::string>;
	static std::map<AtlasKey,std::weak_ptr<HaloArcImageSet>> shared;
	const AtlasKey contextKey(vg,key);
	for (auto i=shared.begin();i!=shared.end();) {if(i->second.expired())i=shared.erase(i);else ++i;}
	if (auto found=shared[contextKey].lock()) {
		if (found->valid(vg)) {arcImages=found;arcImageKey=key;atlasBloom=bloomAmount;atlasDiameter=box.size.x;fixedSurface.markDirty();return true;}
	}
	// Bound live shared texture storage. Unusual palette/size combinations can
	// retain the analytic backend instead of growing GPU memory without limit.
	size_t residentBytes=0;
	for(const auto& entry:shared)if(auto image=entry.second.lock())
		residentBytes+=size_t(image->width)*image->height*4;
	const size_t requestedBytes=size_t(side+8)*(side+8)*35*4;
	if(side>384 || residentBytes+requestedBytes>64u*1024u*1024u)return false;
	GLint maxTextureSize=0;glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);
	if((side+8)*7>maxTextureSize)return false;
	auto images=std::make_shared<HaloArcImageSet>();
	images->context=gl_lifecycle::acquireResourceContext(vg);
	images->side=side;images->stride=side+8;
	std::array<std::vector<unsigned char>,17> frames;
	visual_assets::AdaptiveGlSurface bake;
	visual_assets::AdaptiveGlSurfacePolicy policy;
	policy.vertexAttributeCount=1;policy.shaderOnlyState=true;policy.maxDensity=3.f;
	const float savedArc=arcNorm;
	bakingArc=true;
	struct Capture {HaloGlSurface* owner;std::vector<unsigned char>* pixels;int side;};
	bool success=true;
	for(int i=0;i<=16;++i) {
		arcNorm=i/16.f;frames[i].resize(size_t(side)*side*4);
		Capture capture{this,&frames[i],side};bake.markDirty();
		if(!bake.renderIfNeeded(vg,box.size,1.f,3.f,policy,false,
			[](void* ptr,Vec active,int y) {
				auto& c=*static_cast<Capture*>(ptr);
				c.owner->renderGlContent(active,y);
				glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
				glPixelStorei(GL_PACK_ALIGNMENT,1);
				glPixelStorei(GL_PACK_ROW_LENGTH,0);glPixelStorei(GL_PACK_SKIP_ROWS,0);glPixelStorei(GL_PACK_SKIP_PIXELS,0);
				glReadPixels(0,y,c.side,c.side,GL_RGBA,GL_UNSIGNED_BYTE,c.pixels->data());
			},&capture) || shaderFailed) {success=false;break;}
	}
	arcNorm=savedArc;bakingArc=false;
	if(!success)return false;
	const int width=images->stride*5,height=images->stride*7;
	std::vector<unsigned char> packed(size_t(width)*height*4,0);
	auto put=[&](int index,int x,int y,const unsigned char* rgba) {
		const size_t offset=(size_t((index/5)*images->stride+4+y)*width+(index%5)*images->stride+4+x)*4;
		std::copy(rgba,rgba+4,packed.data()+offset);
	};
	for(int i=0;i<=16;++i)for(int y=0;y<side;++y)for(int x=0;x<side;++x)
		put(i,x,y,&frames[i][(size_t(side-1-y)*side+x)*4]);
	// Source-over delta: keep unchanged pixels transparent. Choose the least
	// coverage that can represent the new premultiplied color over the old.
	// RGB changes must not turn translucent bloom into an opaque overlay: that
	// would obscure the static SVG beneath the transitioning segment. Limit
	// coverage to the endpoint envelope (plus one byte for quantization). In
	// translucent pixels this deliberately favors background visibility over
	// exact hue interpolation; opaque LED pixels retain the full color change.
	for(int i=0;i<16;++i)for(int y=0;y<side;++y)for(int x=0;x<side;++x) {
		const size_t offset=(size_t(side-1-y)*side+x)*4;
		const auto* from=&frames[config.bipolar && i<8?i+1:i][offset];
		const auto* to=&frames[config.bipolar && i<8?i:i+1][offset];
		if(std::equal(from,from+4,to))continue;
		float alpha=0.f;
		for(int c=0;c<4;++c) {
			const float a=from[c]/255.f,b=to[c]/255.f;
			if(b>a && a<1.f)alpha=std::max(alpha,(b-a)/(1.f-a));
			if(b<a && a>0.f)alpha=std::max(alpha,1.f-b/a);
		}
		if(from[3]<255) {
			const float coverageLimit = std::min(255.f, float(std::max(from[3],to[3])) + 1.f);
			alpha = std::min(alpha, (coverageLimit-from[3])/(255.f-from[3]));
		}
		unsigned char delta[4]={0,0,0,0};
		if(alpha>0.f) {
			for(int c=0;c<3;++c)delta[c]=static_cast<unsigned char>(std::round(clamp(to[c]-from[c]*(1.f-alpha),0.f,255.f*alpha)));
			delta[3]=static_cast<unsigned char>(std::round(alpha*255.f));
		}
		put(17+i,x,y,delta);
	}
	// Store both cap states in the same texture to keep animated draws batched.
	const int capSide=capAtlas->height;
	for(int state=0;state<2;++state)for(int y=0;y<side;++y)for(int x=0;x<side;++x) {
		const float sx=(x+.5f)*capSide/side-.5f,sy=(y+.5f)*capSide/side-.5f;
		const int x0=clamp(int(std::floor(sx)),0,capSide-1),y0=clamp(int(std::floor(sy)),0,capSide-1);
		const int x1=std::min(x0+1,capSide-1),y1=std::min(y0+1,capSide-1);
		const float tx=clamp(sx-x0,0.f,1.f),ty=clamp(sy-y0,0.f,1.f);
		unsigned char rgba[4];
		for(int c=0;c<4;++c) {
			auto at=[&](int xx,int yy){return float(capAtlas->rgba[(size_t(yy)*capAtlas->width+state*capSide+xx)*4+c]);};
			rgba[c]=static_cast<unsigned char>(std::round(crossfade(crossfade(at(x0,y0),at(x1,y0),tx),crossfade(at(x0,y1),at(x1,y1),tx),ty)));
		}
		put(33+state,x,y,rgba);
	}
	const int flags=NVG_IMAGE_PREMULTIPLIED;
	struct Upload {HaloArcImageSet* images;NVGcontext* vg;const unsigned char* pixels;int width,height,flags;bool ok;};
	Upload upload{images.get(),vg,packed.data(),width,height,flags,false};
	bake.markDirty();
	bake.renderIfNeeded(vg,box.size,1.f,3.f,policy,false,
		[](void* ptr,Vec,int) {
			auto& u=*static_cast<Upload*>(ptr);auto& image=*u.images;
			u.ok=nvg_gfx_lifecycle::updateOwnedNvgImageRgba(image.vg,image.image,image.width,image.height,
				u.vg,u.width,u.height,u.flags,u.pixels);
		},&upload);
	if(!upload.ok)return false;

	arcImages=images;arcImageKey=key;atlasBloom=bloomAmount;atlasDiameter=box.size.x;shared[contextKey]=images;fixedSurface.markDirty();
	return true;
}
