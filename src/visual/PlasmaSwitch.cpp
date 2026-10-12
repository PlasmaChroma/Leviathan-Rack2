#include "PlasmaSwitch.hpp"
#include "../NvgGraphicsLifecycle.hpp"
#include "VisualAssets.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>

namespace {

constexpr float kHeightPx = 22.f;
constexpr float kWidthPx = kHeightPx * (168.f / 262.f);
constexpr float kShadowBleedPx = 8.f;
constexpr double kAnimationFps = 24.0;
constexpr double kVisualUpdateIntervalSec = 1.0 / kAnimationFps;
constexpr bool kDrawGlass = false;
thread_local PlasmaSwitchDrawMetrics gPlasmaSwitchDrawMetrics;

uint64_t elapsedNs(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
	return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
}

uint64_t animationFrame() {
	static const double startSec = system::getTime();
	const double elapsedSec = std::max(0.0, system::getTime() - startSec);
	return uint64_t(elapsedSec * kAnimationFps);
}

double animationSec() {
	return double(animationFrame()) / kAnimationFps;
}

void drawChamferRect(NVGcontext* vg, float x, float y, float w, float h, float chamfer) {
	const float c = clamp(chamfer, 0.f, std::min(w, h) * 0.48f);
	nvgBeginPath(vg);
	nvgMoveTo(vg, x + c, y);
	nvgLineTo(vg, x + w - c, y);
	nvgLineTo(vg, x + w, y + c);
	nvgLineTo(vg, x + w, y + h - c);
	nvgLineTo(vg, x + w - c, y + h);
	nvgLineTo(vg, x + c, y + h);
	nvgLineTo(vg, x, y + h - c);
	nvgLineTo(vg, x, y + c);
	nvgClosePath(vg);
}

NVGcolor blendColor(NVGcolor a, NVGcolor b, float t) {
	t = clamp(t, 0.f, 1.f);
	NVGcolor out;
	out.r = a.r + (b.r - a.r) * t;
	out.g = a.g + (b.g - a.g) * t;
	out.b = a.b + (b.b - a.b) * t;
	out.a = a.a + (b.a - a.a) * t;
	return out;
}

struct ShadowLayer : TransparentWidget {
	Vec componentSize;

	void drawSilhouette(const DrawArgs& args, float inset, float dx, float dy, float topExpansion = 0.f) {
		const float x0 = inset + dx;
		const float y0 = inset + dy + topExpansion;
		const float x1 = componentSize.x - inset + dx;
		const float y1 = componentSize.y - inset + dy;
		const float chamferX = componentSize.x * 0.18f;
		const float chamferY = componentSize.y * 0.105f;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, x0 + chamferX, y0);
		nvgLineTo(args.vg, x1 - chamferX, y0);
		nvgLineTo(args.vg, x1, y0 + chamferY);
		nvgLineTo(args.vg, x1, y1 - chamferY);
		nvgLineTo(args.vg, x1 - chamferX, y1);
		nvgLineTo(args.vg, x0 + chamferX, y1);
		nvgLineTo(args.vg, x0, y1 - chamferY);
		nvgLineTo(args.vg, x0, y0 + chamferY);
		nvgClosePath(args.vg);
	}

	void draw(const DrawArgs& args) override {
		auto fillPass = [&](float inset, float dx, float dy, int alpha) {
			const float feather = std::max(1.f, componentSize.x * 0.12f);
			constexpr int steps = 10;
			const int stepAlpha = std::max(1, int(std::round(float(alpha) / 8.4f)));
			for (int i = 0; i < steps; ++i) {
				const float t = float(i) / float(steps - 1);
				const float expansion = feather * (1.f - t);
				drawSilhouette(args, inset - expansion, dx, dy, expansion * 0.95f);
				const int layerAlpha =
					std::max(1, int(std::round(float(stepAlpha) * crossfade(0.3f, 1.7f, t))));
				nvgFillColor(args.vg, nvgRGBA(0, 0, 0, layerAlpha));
				nvgFill(args.vg);
			}
		};

		nvgSave(args.vg);
		nvgTranslate(args.vg, kShadowBleedPx * 0.5f, kShadowBleedPx * 0.5f);
		fillPass(componentSize.x * 0.055f, componentSize.x * 0.12f, componentSize.y * 0.09f, 44);
		fillPass(componentSize.x * 0.13f, componentSize.x * 0.14f, componentSize.y * 0.08f, 28);
		nvgRestore(args.vg);
	}
};

struct PlasmaSwitchVisualLayer : TransparentWidget {
	PlasmaSwitch* owner = nullptr;

	void draw(const DrawArgs& args) override {
		if (owner) {
			owner->drawVisual(args);
		}
	}
};

void drawOrb(const Widget::DrawArgs& args,
	Vec size,
	float cx,
	float cy,
	float coreR,
	float glowR,
	NVGcolor glowColor,
	NVGcolor accentColor,
	const float sparkOffsetX[3],
	const float sparkOffsetY[3]) {
	NVGpaint outerGlow = nvgRadialGradient(args.vg, cx, cy, coreR * 0.55f, glowR,
		nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, 0.52f),
		nvgRGBAf(accentColor.r, accentColor.g, accentColor.b, 0.f));
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, size.x, size.y);
	nvgFillPaint(args.vg, outerGlow);
	nvgFill(args.vg);

	NVGpaint violetGlow = nvgRadialGradient(args.vg, cx + coreR * 0.42f, cy + coreR * 0.18f,
		coreR * 0.18f, glowR * 0.72f,
		nvgRGBAf(accentColor.r, accentColor.g, accentColor.b, 0.58f),
		nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, 0.f));
	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, size.x, size.y);
	nvgFillPaint(args.vg, violetGlow);
	nvgFill(args.vg);

	NVGpaint core = nvgRadialGradient(args.vg, cx - coreR * 0.2f, cy - coreR * 0.22f,
		coreR * 0.08f, coreR * 1.18f,
		nvgRGBA(228, 250, 255, 238),
		nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, 1.f));
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, cx, cy, coreR);
	nvgFillPaint(args.vg, core);
	nvgFill(args.vg);

	for (int i = 0; i < 3; ++i) {
		const float sparkR = coreR * (0.16f + 0.035f * float(i & 1));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, cx + sparkOffsetX[i] * coreR, cy + sparkOffsetY[i] * coreR, sparkR);
		nvgFillColor(args.vg,
			i & 1 ? nvgRGBA(0xff, 0xb8, 0x00, 128) : nvgRGBA(255, 255, 255, 146));
		nvgFill(args.vg);
	}
}

void drawGlass(const Widget::DrawArgs& args, float lensX, float lensY, float lensW, float lensH, float lensR) {
	const float cx = lensX + lensW * 0.5f;
	nvgSave(args.vg);
	drawChamferRect(args.vg, lensX, lensY, lensW, lensH, lensR);
	nvgFillColor(args.vg, nvgRGBA(92, 42, 154, 52));
	nvgFill(args.vg);

	const float innerX = lensX + 0.35f;
	const float innerY = lensY + 0.35f;
	const float innerW = lensW - 0.7f;
	const float innerH = lensH - 0.7f;
	const float innerR = std::max(0.f, lensR - 0.35f);

	drawChamferRect(args.vg, innerX, innerY, innerW, innerH, innerR);
	NVGpaint leftShade = nvgLinearGradient(args.vg, innerX, innerY, cx, innerY,
		nvgRGBA(34, 16, 72, 54), nvgRGBA(92, 56, 150, 0));
	nvgFillPaint(args.vg, leftShade);
	nvgFill(args.vg);

	drawChamferRect(args.vg, innerX, innerY, innerW, innerH, innerR);
	NVGpaint rightShade = nvgLinearGradient(args.vg, cx, innerY, innerX + innerW, innerY,
		nvgRGBA(92, 56, 150, 0), nvgRGBA(28, 14, 62, 58));
	nvgFillPaint(args.vg, rightShade);
	nvgFill(args.vg);

	const float highlightHalfW = innerW * 0.20f;
	nvgBeginPath(args.vg);
	nvgRect(args.vg, cx - highlightHalfW, innerY, highlightHalfW, innerH);
	NVGpaint highlightLeft = nvgLinearGradient(args.vg, cx - highlightHalfW, innerY, cx, innerY,
		nvgRGBA(232, 220, 255, 0), nvgRGBA(244, 236, 255, 42));
	nvgFillPaint(args.vg, highlightLeft);
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgRect(args.vg, cx, innerY, highlightHalfW, innerH);
	NVGpaint highlightRight = nvgLinearGradient(args.vg, cx, innerY, cx + highlightHalfW, innerY,
		nvgRGBA(244, 236, 255, 42), nvgRGBA(232, 220, 255, 0));
	nvgFillPaint(args.vg, highlightRight);
	nvgFill(args.vg);

	drawChamferRect(args.vg, innerX, innerY, innerW, innerH, innerR);
	nvgStrokeWidth(args.vg, std::max(0.38f, kWidthPx * 0.030f));
	nvgStrokeColor(args.vg, nvgRGBA(218, 178, 255, 24));
	nvgStroke(args.vg);
	nvgRestore(args.vg);
}

} // namespace

void resetPlasmaSwitchDrawMetrics() {
	gPlasmaSwitchDrawMetrics = PlasmaSwitchDrawMetrics();
}

PlasmaSwitchDrawMetrics getPlasmaSwitchDrawMetrics() {
	return gPlasmaSwitchDrawMetrics;
}

PlasmaSwitch::PlasmaSwitch() {
	momentary = false;
	box.size = Vec(kWidthPx, kHeightPx);
	backingFullPath = asset::plugin(pluginInstance, "res/icon/PlasmaSwitchSmall.png");

	const Vec bleed(kShadowBleedPx, kShadowBleedPx);
	shadowFb = new widget::FramebufferWidget();
	shadowFb->dirtyOnSubpixelChange = false;
	shadowFb->box.pos = bleed.mult(-0.5f);
	shadowFb->box.size = box.size.plus(bleed);
	ShadowLayer* shadowLayer = new ShadowLayer();
	shadowLayer->box.size = shadowFb->box.size;
	shadowLayer->componentSize = box.size;
	shadowFb->addChild(shadowLayer);
	addChild(shadowFb);

	visualFb = new widget::FramebufferWidget();
	visualFb->dirtyOnSubpixelChange = false;
	visualFb->box.size = box.size;
	PlasmaSwitchVisualLayer* visualLayer = new PlasmaSwitchVisualLayer();
	visualLayer->box.size = box.size;
	visualLayer->owner = this;
	visualFb->addChild(visualLayer);
	addChild(visualFb);
}

PlasmaSwitch::~PlasmaSwitch() {
	resetBackingImageHandle(backingImageOwnerVg, false);
}

void PlasmaSwitch::resetBackingImageHandle(NVGcontext* currentVg, bool deleteCurrentHandle) {
	nvg_gfx_lifecycle::resetOwnedNvgImage(
		backingImageOwnerVg,
		backingImageHandle,
		backingImageWidth,
		backingImageHeight,
		currentVg,
		deleteCurrentHandle);
	backingImageCreateAttempted = false;
}

int PlasmaSwitch::ensureBackingImageHandle(NVGcontext* vg) {
	if (!vg || backingFullPath.empty()) {
		return -1;
	}
	if (backingImageOwnerVg && backingImageOwnerVg != vg) {
		++gPlasmaSwitchDrawMetrics.contextResets;
		resetBackingImageHandle(vg, false);
	}
	if (backingImageOwnerVg == vg && backingImageHandle >= 0) {
		if (backingImageWidth > 0 && backingImageHeight > 0
			&& nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, backingImageHandle, backingImageWidth, backingImageHeight)) {
			return backingImageHandle;
		}
		resetBackingImageHandle(vg, true);
	}
	if (!backingImageCreateAttempted) {
		backingImageCreateAttempted = true;
		backingImageHandle = nvgCreateImage(vg, backingFullPath.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
		if (backingImageHandle >= 0) {
			backingImageOwnerVg = vg;
			nvgImageSize(vg, backingImageHandle, &backingImageWidth, &backingImageHeight);
			++gPlasmaSwitchDrawMetrics.imageCreates;
			return backingImageHandle;
		}
	}
	if (!fallbackBackingImage && APP && APP->window) {
		fallbackBackingImage = APP->window->loadImage(backingFullPath);
	}
	if (fallbackBackingImage && fallbackBackingImage->handle >= 0) {
		++gPlasmaSwitchDrawMetrics.imageFallbacks;
		return fallbackBackingImage->handle;
	}
	return -1;
}

void PlasmaSwitch::step() {
	app::Switch::step();
	const double nowSec = system::getTime();
	engine::ParamQuantity* pq = getParamQuantity();
	const float target = (!pq || pq->getValue() > 0.5f) ? 1.f : 0.f;
	const bool targetChanged = target != lastVisualTarget;
	const bool updateDue = nextVisualUpdateSec <= 0.0 || nowSec >= nextVisualUpdateSec || nowSec < lastVisualUpdateSec;
	if (!targetChanged && !updateDue) {
		return;
	}

	const double dt = lastVisualUpdateSec > 0.0 ? std::max(0.0, nowSec - lastVisualUpdateSec) : 0.0;
	lastVisualUpdateSec = nowSec;
	nextVisualUpdateSec = nowSec + kVisualUpdateIntervalSec;
	lastVisualTarget = target;
	if (!displayValueInitialized) {
		displayValue = target;
		displayValueInitialized = true;
	}
	else {
		const float response = target > displayValue ? 7.5f : 6.2f;
		displayValue += (target - displayValue) * clamp(float(dt) * response, 0.f, 1.f);
		if (std::fabs(target - displayValue) < 0.001f) {
			displayValue = target;
		}
	}

	const double phaseSec = animationSec();
	pulseAmount = 0.5f + 0.5f * std::sin(float(phaseSec * 1.45));
	flickerAmount = 0.5f + 0.5f * std::sin(float(phaseSec * 2.4 + 1.4));
	hueAmount = 0.5f + 0.5f * std::sin(float(phaseSec * 0.36 + 0.6));
	for (int i = 0; i < 3; ++i) {
		const float phase = float(phaseSec * (1.9 + 0.37 * i) + double(i) * 1.73);
		sparkOffsetX[i] = std::sin(phase) * (0.42f + 0.08f * i);
		sparkOffsetY[i] = std::cos(phase * 1.21f) * 0.46f;
	}
	if (visualFb) {
		visualFb->setDirty();
	}
}

void PlasmaSwitch::draw(const DrawArgs& args) {
	if (box.size.x <= 1.f || box.size.y <= 1.f) {
		return;
	}
	const auto shadowStart = std::chrono::steady_clock::now();
	if (shadowFb) {
		drawChild(shadowFb, args);
	}
	const auto shadowEnd = std::chrono::steady_clock::now();
	gPlasmaSwitchDrawMetrics.shadowNs += elapsedNs(shadowStart, shadowEnd);
	if (visualFb) {
		drawChild(visualFb, args);
	}
}

void PlasmaSwitch::drawVisual(const DrawArgs& args) {
	if (!displayValueInitialized) {
		engine::ParamQuantity* pq = getParamQuantity();
		displayValue = (!pq || pq->getValue() > 0.5f) ? 1.f : 0.f;
		displayValueInitialized = true;
	}

	const auto imageEnsureStart = std::chrono::steady_clock::now();
	const int imageHandle = ensureBackingImageHandle(args.vg);
	const auto imageEnsureEnd = std::chrono::steady_clock::now();
	gPlasmaSwitchDrawMetrics.imageEnsureNs += elapsedNs(imageEnsureStart, imageEnsureEnd);
	if (imageHandle >= 0) {
		const auto imagePaintStart = std::chrono::steady_clock::now();
		NVGpaint paint =
			nvgImagePattern(args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
		const auto imagePaintEnd = std::chrono::steady_clock::now();
		gPlasmaSwitchDrawMetrics.imagePaintNs += elapsedNs(imagePaintStart, imagePaintEnd);
	}

	const float lensX = box.size.x * 0.245f;
	const float lensY = box.size.y * 0.160f;
	const float lensW = box.size.x * 0.51f;
	const float lensH = box.size.y * 0.680f;
	const float lensR = box.size.x * 0.075f;
	const float tubeX = box.size.x * 0.255f;
	const float tubeY = box.size.y * 0.155f;
	const float tubeW = box.size.x * 0.49f;
	const float tubeH = box.size.y * 0.690f;
	const float tubeR = box.size.x * 0.105f;

	const auto bodyStart = std::chrono::steady_clock::now();
	nvgSave(args.vg);
	drawChamferRect(args.vg, tubeX, tubeY, tubeW, tubeH, tubeR);
	NVGpaint tubeBase = nvgLinearGradient(args.vg, tubeX, tubeY, tubeX + tubeW, tubeY,
		nvgRGBA(0, 0, 0, 190), nvgRGBA(1, 2, 6, 232));
	nvgFillPaint(args.vg, tubeBase);
	nvgFill(args.vg);
	drawChamferRect(args.vg, tubeX + 0.35f, tubeY + 0.35f, tubeW - 0.7f, tubeH - 0.7f,
		std::max(0.f, tubeR - 0.35f));
	nvgStrokeWidth(args.vg, std::max(0.34f, box.size.x * 0.022f));
	nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 145));
	nvgStroke(args.vg);
	nvgRestore(args.vg);
	const auto bodyEnd = std::chrono::steady_clock::now();
	gPlasmaSwitchDrawMetrics.bodyNs += elapsedNs(bodyStart, bodyEnd);

	const float cx = box.size.x * 0.5f;
	const float cy = crossfade(box.size.y * 0.665f, box.size.y * 0.335f, clamp(displayValue, 0.f, 1.f));
	const float coreR = box.size.x * crossfade(0.142f, 0.162f, pulseAmount);
	const float glowR = box.size.x * crossfade(0.35f, 0.46f, flickerAmount);
	const NVGcolor cyan = nvgRGBA(0x00, 0xc8, 0xff, 205);
	const NVGcolor purple = nvgRGBA(0x8e, 0x34, 0xff, 198);
	const NVGcolor glowColor = blendColor(cyan, purple, hueAmount);
	const NVGcolor accentColor = blendColor(purple, cyan, 1.f - hueAmount * 0.55f);

	const auto orbStart = std::chrono::steady_clock::now();
	nvgSave(args.vg);
	const float insetX = box.size.x * 0.024f;
	const float insetY = box.size.x * 0.014f;
	const float clipX = lensX + insetX;
	const float clipY = lensY + insetY;
	const float clipW = std::max(0.f, lensW - 2.f * insetX);
	const float clipH = std::max(0.f, lensH - 2.f * insetY);
	nvgScissor(args.vg, clipX, clipY, clipW, clipH);
	drawOrb(args, box.size, cx, cy, coreR, glowR, glowColor, accentColor, sparkOffsetX, sparkOffsetY);
	nvgRestore(args.vg);
	const auto orbEnd = std::chrono::steady_clock::now();
	gPlasmaSwitchDrawMetrics.orbNs += elapsedNs(orbStart, orbEnd);

	if (kDrawGlass) {
		drawGlass(args, lensX, lensY, lensW, lensH, lensR);
	}
}
