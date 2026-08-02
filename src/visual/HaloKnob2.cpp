#include "VisualAssets.hpp"

#include "../GlLifecycleUtils.hpp"

#include <nanovg_gl.h>
#include <nanosvgrast.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <vector>

namespace {

thread_local visual_assets::HaloKnob2DrawMetrics gHaloKnob2DrawMetrics;

constexpr int kHaloCapRasterScale = 4;

struct HaloCapRasterAtlas {
	int width = 0;
	int height = 0;
	std::vector<unsigned char> rgba;
};

std::shared_ptr<HaloCapRasterAtlas> getHaloCapRasterAtlas(
	const std::shared_ptr<window::Svg>& normalSvg,
	const std::shared_ptr<window::Svg>& litSvg) {
	static std::weak_ptr<HaloCapRasterAtlas> cachedAtlas;
	if (std::shared_ptr<HaloCapRasterAtlas> atlas = cachedAtlas.lock()) {
		return atlas;
	}
	if (!normalSvg || !normalSvg->handle || !litSvg || !litSvg->handle) {
		return nullptr;
	}
	const int imageWidth = std::max(1, int(std::ceil(std::max(
		normalSvg->handle->width, litSvg->handle->width) * kHaloCapRasterScale)));
	const int imageHeight = std::max(1, int(std::ceil(std::max(
		normalSvg->handle->height, litSvg->handle->height) * kHaloCapRasterScale)));
	std::shared_ptr<HaloCapRasterAtlas> atlas = std::make_shared<HaloCapRasterAtlas>();
	atlas->width = imageWidth * 2;
	atlas->height = imageHeight;
	atlas->rgba.assign(size_t(atlas->width) * size_t(atlas->height) * 4u, 0u);

	NSVGrasterizer* rasterizer = nsvgCreateRasterizer();
	if (!rasterizer) return nullptr;
	const int stride = atlas->width * 4;
	nsvgRasterize(rasterizer, normalSvg->handle, 0.f, 0.f, float(kHaloCapRasterScale),
		atlas->rgba.data(), imageWidth, imageHeight, stride);
	nsvgRasterize(rasterizer, litSvg->handle, 0.f, 0.f, float(kHaloCapRasterScale),
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
			gHaloKnob2DrawMetrics.nanoVgSurfaceDrawNs += haloElapsedNs(start);
			++gHaloKnob2DrawMetrics.nanoVgSurfaceDraws;
		}
	}
};

struct HaloTimedFramebuffer final : widget::FramebufferWidget {
	void drawFramebuffer() override {
		const bool measure = isDragonKingDebugEnabled();
		const auto start = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
		widget::FramebufferWidget::drawFramebuffer();
		if (!measure) return;
		const uint64_t elapsed = haloElapsedNs(start);
		gHaloKnob2DrawMetrics.capReflectionFramebufferNs += elapsed;
		++gHaloKnob2DrawMetrics.capReflectionFramebufferDraws;
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
	DrawArc&& drawArc) {
	constexpr int segmentCount = 16;
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
	gHaloKnob2DrawMetrics = {};
}

HaloKnob2DrawMetrics getHaloKnob2DrawMetrics() {
	return gHaloKnob2DrawMetrics;
}

} // namespace visual_assets

void LeviathanHaloKnob2::GlowArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;
	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float mainRadius = diameterPx * (18.15f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
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
		drawCoalescedHaloSegments(startAngle, endAngle, activeAngle, active, inactive,
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
		for (int i = 0; i < count; ++i) {
			const float a0 = startAngle + step * float(i) + 0.5f * gap;
			const float a1 = startAngle + step * float(i + 1) - 0.5f * gap;
			float mix = 0.f;
			if (activeAngle >= a1) mix = 1.f;
			else if (activeAngle > a0) mix = (activeAngle - a0) / std::max(a1 - a0, 1e-6f);
			drawSegmentBand(a0, a1,
				blendHaloColor(config.inactiveColor, config.activeColor, mix),
				blendHaloColor(config.inactiveHighlightColor, config.activeHighlightColor, mix));
		}
	};
	auto drawReflection = [&](float radius, float width, NVGcolor active, NVGcolor inactive) {
		drawCoalescedHaloSegments(startAngle, endAngle, activeAngle, active, inactive,
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
	std::shared_ptr<HaloCapRasterAtlas> capAtlas;
	GlowArcWidget* fallbackBackgroundGlow = nullptr;
	LightArcWidget* fallbackLightArc = nullptr;
	GlowArcWidget* fallbackForegroundGlow = nullptr;
	float valueNorm = 0.5f;
	float bloomAmount = 0.f;
	bool centerLit = false;
	GLuint program = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint vbo = 0;
	GLuint capTexture = 0;
	bool initAttempted = false;
	GLint uniformLogicalSize = -1;
	GLint uniformValue = -1;
	GLint uniformBloomAmount = -1;
	GLint uniformLed = -1;
	GLint uniformBloom = -1;
	GLint uniformCapAtlas = -1;
	GLint uniformCenterLit = -1;
	GLint uniformCapRotation = -1;
	bool shaderFailed = false;
	bool forceNanoVg = false;

	explicit HaloGlSurface(
		Config config,
		const std::shared_ptr<window::Svg>& centerNormalSvg,
		const std::shared_ptr<window::Svg>& centerLitSvg,
		EclipseKnob::SvgLayer* fallbackCenterLayer)
		: config(config), capAtlas(getHaloCapRasterAtlas(centerNormalSvg, centerLitSvg)) {
		HaloNanoVgFallbackWidget* fallbackRoot = new HaloNanoVgFallbackWidget();
		fallbackRoot->box.size = Vec(46.f, 46.f);
		addChild(fallbackRoot);

		fallbackBackgroundGlow = new GlowArcWidget();
		fallbackBackgroundGlow->box.size = Vec(46.f, 46.f);
		fallbackBackgroundGlow->config = config.bloom;
		fallbackRoot->addChild(fallbackBackgroundGlow);

		fallbackLightArc = new LightArcWidget();
		fallbackLightArc->box.size = Vec(46.f, 46.f);
		fallbackLightArc->config = config.ledArc;
		fallbackLightArc->bloomConfig = config.bloom;
		fallbackRoot->addChild(fallbackLightArc);

		fallbackForegroundGlow = new GlowArcWidget();
		fallbackForegroundGlow->box.size = Vec(46.f, 46.f);
		fallbackForegroundGlow->foreground = true;
		fallbackForegroundGlow->config = config.bloom;
		fallbackRoot->addChild(fallbackForegroundGlow);

		// OpenGlWidget bypasses its framebuffer in browser previews and after a
		// shader failure. Keep the original SVG cap in that NanoVG-only subtree.
		fallbackRoot->addChild(fallbackCenterLayer);
	}

	~HaloGlSurface() override {
		releaseGlResources(false);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		OpenGlWidget::onContextDestroy(e);
		releaseGlResources(true);
		shaderFailed = false;
		bypassed = forceNanoVg;
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
		initAttempted = false;
		uniformLogicalSize = uniformValue = uniformBloomAmount = uniformLed = uniformBloom = -1;
		uniformCapAtlas = uniformCenterLit = uniformCapRotation = -1;
	}

	void step() override {
		// OpenGlWidget::step() dirties every frame. Halo surfaces are explicitly
		// invalidated so their framebuffer remains a single idle composite.
		bypassed = forceNanoVg || shaderFailed;
		FramebufferWidget::step();
	}

	void setForceNanoVg(bool force) {
		if (forceNanoVg == force) return;
		forceNanoVg = force;
		bypassed = forceNanoVg || shaderFailed;
		if (!bypassed) setDirty();
	}

	void setVisualState(float value, float bloom) {
		value = clamp(value, 0.f, 1.f);
		bloom = std::max(0.f, bloom);
		if (fallbackBackgroundGlow) fallbackBackgroundGlow->valueNorm = value;
		if (fallbackLightArc) fallbackLightArc->valueNorm = value;
		if (fallbackForegroundGlow) fallbackForegroundGlow->valueNorm = value;
		if (std::fabs(value - valueNorm) <= 1e-6f && std::fabs(bloom - bloomAmount) <= 1e-4f) return;
		valueNorm = value;
		bloomAmount = bloom;
		setDirty();
	}

	void setCenterLit(bool lit) {
		if (centerLit == lit) return;
		centerLit = lit;
		if (!bypassed) setDirty();
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
		std::vector<char> log(size_t(std::max(logLength, 1)));
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
			uniform float uValue;
			uniform float uBloomAmount;
			uniform vec4 uLed[4];
			uniform vec4 uBloom[17];
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

			void main() {
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
				float haloMix = clamp(uValue * 16.0 - segmentIndex, 0.0, 1.0);
				float activeMix = clamp((uValue * 16.0 - segmentIndex - 0.072) / 0.856, 0.0, 1.0);
				float cursorIndex = floor(clamp(uValue * 16.0 - 0.0001, 0.0, 15.9999));
				float cursorSegment = (1.0 - step(0.5, abs(segmentIndex - cursorIndex))) * step(0.0001, uValue);
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

				// The value boundary reads as a charged cursor segment without any
				// time-driven animation, so the completed image remains idle-cacheable.
				vec4 cursorColor = vec4(mix(uLed[0].rgb, uLed[1].rgb, 0.74), 0.34);
				overLayer(accum, cursorColor, coreBand * cursorSegment);
				overLayer(accum, vec4(0.88, 0.99, 1.0, 0.22),
					segmentBody * cursorSegment * (1.0 - smoothstep(0.10, 0.30, radialEdgeDistance)));

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
			std::vector<char> log(size_t(std::max(logLength, 1)));
			GLsizei written = 0;
			glGetProgramInfoLog(program, GLsizei(log.size()), &written, log.data());
			WARN("HaloKnob2 shader link failed: %s", log.data());
			releaseGlResources(true);
			initAttempted = true;
			return false;
		}
		uniformLogicalSize = glGetUniformLocation(program, "uLogicalSize");
		uniformValue = glGetUniformLocation(program, "uValue");
		uniformBloomAmount = glGetUniformLocation(program, "uBloomAmount");
		uniformLed = glGetUniformLocation(program, "uLed[0]");
		uniformBloom = glGetUniformLocation(program, "uBloom[0]");
		uniformCapAtlas = glGetUniformLocation(program, "uCapAtlas");
		uniformCenterLit = glGetUniformLocation(program, "uCenterLit");
		uniformCapRotation = glGetUniformLocation(program, "uCapRotation");
		if (uniformLogicalSize < 0 || uniformValue < 0 || uniformBloomAmount < 0
			|| uniformLed < 0 || uniformBloom < 0 || uniformCapAtlas < 0
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
			const float mix = clamp(valueNorm * 16.f - float(i), 0.f, 1.f);
			const NVGcolor color = blendHaloColor(config.ledArc.inactiveColor, config.ledArc.activeColor, mix);
			glColor4f(color.r, color.g, color.b, color.a);
			glVertex2f(cx + std::cos(a0) * radius, cy + std::sin(a0) * radius);
			glVertex2f(cx + std::cos(a1) * radius, cy + std::sin(a1) * radius);
		}
		glEnd();
	}

	void drawFramebuffer() override {
		const bool measure = isDragonKingDebugEnabled();
		const auto profileStart = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
		Vec framebufferSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(framebufferSize.x))), std::max(1, int(std::lround(framebufferSize.y))));
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		const float w = std::max(box.size.x, 1.f);
		const float h = std::max(box.size.y, 1.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, w, h, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
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
				gHaloKnob2DrawMetrics.glSurfaceFramebufferNs += haloElapsedNs(profileStart);
				++gHaloKnob2DrawMetrics.glSurfaceFramebufferDraws;
			}
			return;
		}
		// The fragment shader has already composited all layers and emits a
		// premultiplied result, so blending it over the transparent FBO would
		// apply alpha a second time.
		glDisable(GL_BLEND);

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

		glUseProgram(program);
		glUniform2f(uniformLogicalSize, w, h);
		glUniform1f(uniformValue, valueNorm);
		glUniform1f(uniformBloomAmount, bloomAmount);
		uploadHaloColorArray(uniformLed, ledColors, 4);
		uploadHaloColorArray(uniformBloom, bloomColors, 17);
		glUniform1f(uniformCenterLit, centerLit ? 1.f : 0.f);
		const float capAngle = crossfade(-0.83f * float(M_PI), 0.83f * float(M_PI), valueNorm);
		glUniform2f(uniformCapRotation, std::cos(capAngle), std::sin(capAngle));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, capTexture);
		glUniform1i(uniformCapAtlas, 0);
		const std::array<GLfloat, 8> vertices {{0.f, 0.f, w, 0.f, 0.f, h, w, h}};
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDisableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
		glDisable(GL_BLEND);
		if (measure) {
			gHaloKnob2DrawMetrics.glSurfaceFramebufferNs += haloElapsedNs(profileStart);
			++gHaloKnob2DrawMetrics.glSurfaceFramebufferDraws;
		}
	}
};

void LeviathanHaloKnob2::CapReflectionWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;
	const Vec center = box.size.mult(0.5f);
	const float scale = diameterPx / 46.f;
	const float rimRadius = diameterPx * (14.62f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
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
	drawCoalescedHaloSegments(startAngle, endAngle, activeAngle,
		bloomColor(config.capReflectionOuterActiveColor), bloomColor(config.capReflectionOuterInactiveColor),
		[&](float a0, float a1, NVGcolor color) { strokeArc(a0, a1, rimRadius, std::max(0.30f, 0.42f * scale), color); });
	drawCoalescedHaloSegments(startAngle, endAngle, activeAngle,
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

LeviathanHaloKnob2::LeviathanHaloKnob2(Config config) : config(config) {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;
	box.size = Vec(46.f, 46.f);
	lastBloomAmount = settings::haloBrightness;

	const std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnob2Back.svg");
	centerNormalSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenter.svg");
	centerLitSvg = visual_assets::loadPluginSvgCached("res/icon/HaloKnobCenterLit.svg");

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->rotateWithValue = false;
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
	glSurface->setVisualState(normalizedParamValue(), haloBloomAmount(lastBloomAmount));
	addChild(glSurface);

	capReflection = new CapReflectionWidget();
	capReflection->box.size = box.size;
	capReflection->minAngle = minAngle;
	capReflection->maxAngle = maxAngle;
	capReflection->valueNorm = normalizedParamValue();
	capReflection->config = this->config.bloom;
	capReflectionFb = new HaloTimedFramebuffer();
	capReflectionFb->box.size = box.size;
	capReflectionFb->dirtyOnSubpixelChange = false;
	capReflectionFb->addChild(capReflection);
	addChild(capReflectionFb);
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
	if (std::fabs(bloom - lastBloomAmount) > 1e-4f) {
		lastBloomAmount = bloom;
		if (glSurface) glSurface->setVisualState(normalizedParamValue(), haloBloomAmount(bloom));
		if (capReflectionFb) capReflectionFb->setDirty();
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
	if (centerLayer) centerLayer->valueNorm = value;
	if (capReflection) capReflection->valueNorm = value;
	if (glSurface) glSurface->setVisualState(value, haloBloomAmount(lastBloomAmount));
	if (capReflectionFb) capReflectionFb->setDirty();
}

bool LeviathanHaloKnob2::isVisualDirty() const {
	return (glSurface && glSurface->dirty)
		|| (capReflectionFb && capReflectionFb->dirty)
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
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}
