#include "ApertureLight.hpp"

#include <cmath>

namespace {

NVGcolor mixNvgColor(NVGcolor a, NVGcolor b, float t, float alphaScale = 1.f) {
	t = clamp(t, 0.f, 1.f);
	NVGcolor out;
	out.r = a.r + (b.r - a.r) * t;
	out.g = a.g + (b.g - a.g) * t;
	out.b = a.b + (b.b - a.b) * t;
	out.a = clamp((a.a + (b.a - a.a) * t) * alphaScale, 0.f, 1.f);
	return out;
}

NVGcolor withNvgAlpha(NVGcolor color, float alpha) {
	color.a = clamp(alpha, 0.f, 1.f);
	return color;
}

float apertureBloomAmount() {
	const float bloomRaw = clamp(settings::haloBrightness, 0.f, 1.5f);
	if (bloomRaw <= 0.001f) {
		return 0.f;
	}
	const float bloomLow = bloomRaw + 2.22f * bloomRaw * (1.f - bloomRaw);
	const float bloomRamp = clamp((bloomRaw - 0.50f) / 0.50f, 0.f, 1.f);
	return bloomLow * (1.44f + 1.05f * bloomRamp * bloomRamp);
}

void setApertureBaseColor(LeviathanApertureLight* light, NVGcolor color) {
	if (!light) {
		return;
	}
	light->baseColor = color;
	light->activeColor = color;
	light->baseColors.clear();
	light->addBaseColor(color);
	light->invalidateStaticBackgroundCache();
	light->invalidateBloomCache();
}

} // namespace

struct LeviathanApertureLight::StaticBackgroundWidget : Widget {
	LeviathanApertureLight* owner = nullptr;

	explicit StaticBackgroundWidget(LeviathanApertureLight* owner) : owner(owner) {
	}

	void draw(const DrawArgs& args) override {
		if (owner) {
			owner->drawStaticBackground(args.vg);
		}
	}
};

struct LeviathanApertureLight::BloomWidget : Widget {
	LeviathanApertureLight* owner = nullptr;

	explicit BloomWidget(LeviathanApertureLight* owner) : owner(owner) {
	}

	void draw(const DrawArgs& args) override {
		if (owner) {
			owner->drawBloomCache(args.vg);
		}
	}
};

LeviathanApertureLight::LeviathanApertureLight() {
	staticBackgroundFb = new widget::FramebufferWidget();
	staticBackgroundFb->dirtyOnSubpixelChange = false;
	staticBackgroundFb->hide();
	staticBackgroundWidget = new StaticBackgroundWidget(this);
	staticBackgroundFb->addChild(staticBackgroundWidget);
	addChild(staticBackgroundFb);

	bloomFb = new widget::FramebufferWidget();
	bloomFb->dirtyOnSubpixelChange = false;
	bloomFb->hide();
	bloomWidget = new BloomWidget(this);
	bloomFb->addChild(bloomWidget);
	addChild(bloomFb);

	applySize(ApertureLightSize::Small);
	addBaseColor(baseColor);
	bgColor = color::BLACK_TRANSPARENT;
	borderColor = color::BLACK_TRANSPARENT;
}

LeviathanApertureLight::~LeviathanApertureLight() {
	staticBackgroundFb = nullptr;
	staticBackgroundWidget = nullptr;
	bloomFb = nullptr;
	bloomWidget = nullptr;
}

void LeviathanApertureLight::applySize(ApertureLightSize size) {
	switch (size) {
	case ApertureLightSize::Tiny:
		box.size = Vec(10.f, 10.f);
		socketRadius = 3.2f;
		lensRadius = 2.2f;
		coreRadius = 1.4f;
		bloomRadius = 6.8f;
		bloomAlpha = 0.24f;
		break;
	case ApertureLightSize::Small:
		box.size = Vec(14.f, 14.f);
		socketRadius = 4.8f;
		lensRadius = 3.3f;
		coreRadius = 2.1f;
		bloomRadius = 9.8f;
		bloomAlpha = 0.26f;
		break;
	case ApertureLightSize::Medium:
		box.size = Vec(20.f, 20.f);
		socketRadius = 7.0f;
		lensRadius = 5.0f;
		coreRadius = 3.2f;
		bloomRadius = 14.2f;
		bloomAlpha = 0.28f;
		break;
	case ApertureLightSize::Large:
		box.size = Vec(28.f, 28.f);
		socketRadius = 10.0f;
		lensRadius = 7.2f;
		coreRadius = 4.5f;
		bloomRadius = 20.5f;
		bloomAlpha = 0.30f;
		break;
	}
	syncStaticBackgroundCache();
	syncBloomCache();
}

void LeviathanApertureLight::invalidateStaticBackgroundCache() {
	if (staticBackgroundFb) {
		staticBackgroundFb->setDirty();
	}
}

void LeviathanApertureLight::invalidateBloomCache() {
	bloomCacheGlow = -1.f;
	if (bloomFb) {
		bloomFb->setDirty();
	}
}

void LeviathanApertureLight::syncStaticBackgroundCache() {
	if (!staticBackgroundFb || !staticBackgroundWidget) {
		return;
	}
	staticBackgroundFb->box.pos = Vec(0.f, 0.f);
	staticBackgroundFb->box.size = box.size;
	staticBackgroundWidget->box.pos = Vec(0.f, 0.f);
	staticBackgroundWidget->box.size = box.size;
	staticBackgroundFb->setDirty();
}

void LeviathanApertureLight::syncBloomCache() {
	if (!bloomFb || !bloomWidget) {
		return;
	}
	const float bloomExtent = bloomRadius * 1.35f;
	const float halfW = box.size.x * 0.5f;
	const float halfH = box.size.y * 0.5f;
	bloomCacheBleedPx = std::max(0.f, bloomExtent - std::min(halfW, halfH));
	bloomCacheBleedPx = std::ceil(bloomCacheBleedPx + 1.f);
	bloomFb->box.pos = Vec(-bloomCacheBleedPx, -bloomCacheBleedPx);
	bloomFb->box.size = box.size.plus(Vec(2.f * bloomCacheBleedPx, 2.f * bloomCacheBleedPx));
	bloomWidget->box.pos = Vec(0.f, 0.f);
	bloomWidget->box.size = bloomFb->box.size;
	invalidateBloomCache();
}

void LeviathanApertureLight::drawStaticBackground(NVGcontext* vg) {
	const float cx = box.size.x * 0.5f;
	const float cy = box.size.y * 0.5f;

	drawSocket(vg, cx, cy);
	drawUnlitLens(vg, cx, cy);
}

void LeviathanApertureLight::drawBloomCache(NVGcontext* vg) {
	const float cx = box.size.x * 0.5f + bloomCacheBleedPx;
	const float cy = box.size.y * 0.5f + bloomCacheBleedPx;
	drawBloom(vg, cx, cy, bloomCacheGlow);
}

void LeviathanApertureLight::drawBackground(const DrawArgs& args) {
	if (!staticBackgroundFb || !staticBackgroundWidget) {
		drawStaticBackground(args.vg);
		return;
	}
	if (!staticBackgroundFb->box.size.equals(box.size)) {
		syncStaticBackgroundCache();
	}
	staticBackgroundFb->draw(args);
}

void LeviathanApertureLight::drawLight(const DrawArgs& args) {
	NVGcontext* vg = args.vg;
	const float cx = box.size.x * 0.5f;
	const float cy = box.size.y * 0.5f;

	float t = 0.f;
	float colorWeight = 0.f;
	NVGcolor mixedColor = nvgRGBAf(0.f, 0.f, 0.f, 1.f);
	for (size_t i = 0; i < baseColors.size(); ++i) {
		engine::Light* light = getLight(int(i));
		const float brightness = clamp(light ? light->getBrightness() : 0.f, 0.f, 1.f);
		t = std::max(t, brightness);
		colorWeight += brightness;
		mixedColor.r += baseColors[i].r * brightness;
		mixedColor.g += baseColors[i].g * brightness;
		mixedColor.b += baseColors[i].b * brightness;
	}
	if (colorWeight > 1e-6f) {
		activeColor = nvgRGBAf(mixedColor.r / colorWeight, mixedColor.g / colorWeight,
		                      mixedColor.b / colorWeight, 1.f);
	}
	else {
		activeColor = baseColor;
	}
	const float glow = std::pow(t, 0.55f);
	const float core = std::pow(t, 0.85f);
	const float hot = std::pow(t, 3.0f);
	const float bloom = apertureBloomAmount();

	if (t > 0.001f && bloom > 0.f) {
		const float effectiveBloom = glow * bloom;
		if (bloomFb && bloomWidget) {
			if (!bloomFb->box.size.equals(box.size.plus(Vec(2.f * bloomCacheBleedPx, 2.f * bloomCacheBleedPx)))) {
				syncBloomCache();
			}
			const float colorDelta = std::fabs(activeColor.r - bloomCacheColor.r) +
			                         std::fabs(activeColor.g - bloomCacheColor.g) +
			                         std::fabs(activeColor.b - bloomCacheColor.b);
			if (std::fabs(effectiveBloom - bloomCacheGlow) > 0.0005f || colorDelta > 0.001f) {
				bloomCacheGlow = effectiveBloom;
				bloomCacheColor = activeColor;
				bloomFb->setDirty();
			}
			nvgSave(vg);
			nvgTranslate(vg, -bloomCacheBleedPx, -bloomCacheBleedPx);
			bloomFb->draw(args);
			nvgRestore(vg);
		}
		else {
			drawBloom(vg, cx, cy, effectiveBloom);
		}
	}
	if (t > 0.001f) {
		drawCore(vg, cx, cy, core, hot);
		drawCrescent(vg, cx, cy, core);
		drawSpecular(vg, cx, cy, hot);
	}
}

void LeviathanApertureLight::drawHalo(const DrawArgs& args) {
	// Disable Rack's built-in halo. The aperture widget draws a color-matched
	// bloom in drawLight() so aliases cannot inherit a stale halo color.
}

void LeviathanApertureLight::drawSocket(NVGcontext* vg, float cx, float cy) {
	const float shadowPad = socketRadius * 0.34f;
	NVGpaint shadow = nvgRadialGradient(
		vg,
		cx + socketRadius * 0.16f,
		cy + socketRadius * 0.22f,
		socketRadius * 0.35f,
		socketRadius + shadowPad,
		nvgRGBA(0, 0, 0, 178),
		nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, socketRadius + shadowPad);
	nvgFillPaint(vg, shadow);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, socketRadius + socketRadius * 0.18f);
	nvgFillColor(vg, nvgRGBA(3, 4, 6, 248));
	nvgFill(vg);

	const float rimShift = socketRadius * 0.20f;
	NVGpaint rim = nvgRadialGradient(
		vg,
		cx - rimShift,
		cy - rimShift,
		socketRadius * 0.25f,
		socketRadius + socketRadius * 0.18f,
		nvgRGBA(38, 44, 50, 142),
		nvgRGBA(0, 1, 3, 252));
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, socketRadius + socketRadius * 0.12f);
	nvgFillPaint(vg, rim);
	nvgFill(vg);

	NVGpaint bevel = nvgLinearGradient(
		vg,
		cx - socketRadius,
		cy - socketRadius,
		cx + socketRadius,
		cy + socketRadius,
		nvgRGBA(68, 76, 84, 92),
		nvgRGBA(0, 0, 0, 170));
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, socketRadius + socketRadius * 0.02f);
	nvgStrokeWidth(vg, std::max(0.55f, socketRadius * 0.13f));
	nvgStrokePaint(vg, bevel);
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius + socketRadius * 0.28f);
	nvgFillColor(vg, nvgRGBA(0, 1, 3, 246));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius + socketRadius * 0.20f);
	nvgStrokeWidth(vg, std::max(0.45f, socketRadius * 0.11f));
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 235));
	nvgStroke(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius + socketRadius * 0.06f);
	nvgFillColor(vg, nvgRGBA(1, 2, 4, 248));
	nvgFill(vg);
}

void LeviathanApertureLight::drawUnlitLens(NVGcontext* vg, float cx, float cy) {
	NVGcolor glassCenter = nvgRGBAf(
		baseColor.r * 0.13f,
		baseColor.g * 0.13f,
		baseColor.b * 0.13f,
		0.60f);
	NVGpaint glass = nvgRadialGradient(
		vg,
		cx - lensRadius * 0.24f,
		cy - lensRadius * 0.28f,
		lensRadius * 0.16f,
		lensRadius,
		glassCenter,
		nvgRGBA(1, 2, 4, 226));
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius);
	nvgFillPaint(vg, glass);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius);
	nvgStrokeWidth(vg, std::max(0.35f, lensRadius * 0.12f));
	nvgStrokeColor(vg, nvgRGBA(100, 124, 132, 54));
	nvgStroke(vg);
}

void LeviathanApertureLight::drawBloom(NVGcontext* vg, float cx, float cy, float glow) {
	const NVGcolor inner = withNvgAlpha(activeColor, bloomAlpha * 0.95f * glow);
	const NVGcolor mid = withNvgAlpha(activeColor, bloomAlpha * 0.34f * glow);
	const NVGcolor outer = withNvgAlpha(activeColor, 0.f);

	NVGpaint outerBloom = nvgRadialGradient(
		vg,
		cx,
		cy,
		std::max(0.5f, coreRadius * 0.70f),
		bloomRadius * 1.35f,
		inner,
		outer);
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, bloomRadius * 1.35f);
	nvgFillPaint(vg, outerBloom);
	nvgFill(vg);

	NVGpaint lensBloom = nvgRadialGradient(
		vg,
		cx,
		cy,
		coreRadius * 0.35f,
		lensRadius * 1.7f,
		mid,
		outer);
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, lensRadius * 1.7f);
	nvgFillPaint(vg, lensBloom);
	nvgFill(vg);
}

void LeviathanApertureLight::drawCore(NVGcontext* vg, float cx, float cy, float core, float hot) {
	const NVGcolor hotWhite = nvgRGBAf(1.f, 1.f, 1.f, 1.f);
	const NVGcolor center = withNvgAlpha(mixNvgColor(activeColor, hotWhite, 0.58f), 0.35f * core + 0.35f * hot);
	const NVGcolor edge = withNvgAlpha(activeColor, 0.82f * core);

	NVGpaint corePaint = nvgRadialGradient(
		vg,
		cx - lensRadius * 0.10f,
		cy - lensRadius * 0.14f,
		std::max(0.1f, coreRadius * 0.10f),
		coreRadius + lensRadius * 0.38f,
		center,
		edge);
	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, coreRadius + lensRadius * 0.32f);
	nvgFillPaint(vg, corePaint);
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgCircle(vg, cx, cy, coreRadius);
	nvgFillColor(vg, withNvgAlpha(mixNvgColor(activeColor, hotWhite, 0.34f), 0.18f * core + 0.20f * hot));
	nvgFill(vg);
}

void LeviathanApertureLight::drawSpecular(NVGcontext* vg, float cx, float cy, float hot) {
	if (hot <= 0.001f) {
		return;
	}
	const float offset = lensRadius * 0.36f;
	const float radius = lensRadius * 0.22f;
	NVGpaint paint = nvgRadialGradient(
		vg,
		cx - offset,
		cy - offset,
		0.f,
		radius,
		nvgRGBAf(1.f, 1.f, 1.f, 0.58f * hot),
		nvgRGBAf(1.f, 1.f, 1.f, 0.f));
	nvgBeginPath(vg);
	nvgCircle(vg, cx - offset, cy - offset, radius);
	nvgFillPaint(vg, paint);
	nvgFill(vg);
}

void LeviathanApertureLight::drawCrescent(NVGcontext* vg, float cx, float cy, float amount) {
	const float alpha = 0.20f * clamp(amount, 0.f, 1.f);
	if (alpha <= 0.001f) {
		return;
	}
	nvgBeginPath(vg);
	nvgCircle(vg, cx + lensRadius * 0.20f, cy + lensRadius * 0.20f, lensRadius * 0.88f);
	nvgCircle(vg, cx - lensRadius * 0.02f, cy - lensRadius * 0.04f, lensRadius * 0.86f);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillColor(vg, nvgRGBAf(0.f, 0.f, 0.f, alpha));
	nvgFill(vg);
}

TinyApertureLight::TinyApertureLight() {
	applySize(ApertureLightSize::Tiny);
}

SmallApertureLight::SmallApertureLight() {
	applySize(ApertureLightSize::Small);
}

MediumApertureLight::MediumApertureLight() {
	applySize(ApertureLightSize::Medium);
}

LargeApertureLight::LargeApertureLight() {
	applySize(ApertureLightSize::Large);
}

TealApertureLight::TealApertureLight() {
	setApertureBaseColor(this, nvgRGB(42, 246, 255));
}

VioletApertureLight::VioletApertureLight() {
	setApertureBaseColor(this, nvgRGB(193, 72, 255));
}

AmberApertureLight::AmberApertureLight() {
	setApertureBaseColor(this, nvgRGB(255, 195, 62));
}

BlueApertureLight::BlueApertureLight() {
	setApertureBaseColor(this, nvgRGB(75, 132, 255));
}

GreenApertureLight::GreenApertureLight() {
	setApertureBaseColor(this, nvgRGB(134, 255, 107));
}

AmberGreenApertureLight::AmberGreenApertureLight() {
	baseColor = nvgRGB(255, 195, 62);
	activeColor = baseColor;
	baseColors.clear();
	addBaseColor(baseColor);
	addBaseColor(nvgRGB(134, 255, 107));
	invalidateStaticBackgroundCache();
	invalidateBloomCache();
}

RedApertureLight::RedApertureLight() {
	setApertureBaseColor(this, nvgRGB(255, 84, 84));
}

MagentaApertureLight::MagentaApertureLight() {
	setApertureBaseColor(this, nvgRGB(255, 68, 178));
}

WhiteApertureLight::WhiteApertureLight() {
	setApertureBaseColor(this, nvgRGB(225, 235, 255));
}
