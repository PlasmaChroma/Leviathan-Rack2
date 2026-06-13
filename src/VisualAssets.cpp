#include "VisualAssets.hpp"

#include <chrono>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>

namespace visual_assets {

namespace {

thread_local uint64_t gEclipseShadowDrawNs = 0u;
thread_local uint64_t gEclipseShadowDrawCount = 0u;

} // namespace

std::shared_ptr<window::Svg> loadPluginSvgCached(const char* path) {
	static std::map<std::string, std::shared_ptr<window::Svg>> cache;
	const std::string key = path ? path : "";
	auto it = cache.find(key);
	if (it != cache.end()) {
		return it->second;
	}
	std::shared_ptr<window::Svg> svg = Svg::load(asset::plugin(pluginInstance, key));
	cache[key] = svg;
	return svg;
}

void resetEclipseShadowDrawMetrics() {
	gEclipseShadowDrawNs = 0u;
	gEclipseShadowDrawCount = 0u;
}

uint64_t eclipseShadowDrawNs() {
	return gEclipseShadowDrawNs;
}

uint64_t eclipseShadowDrawCount() {
	return gEclipseShadowDrawCount;
}

} // namespace visual_assets

namespace levi_jack {

NVGcolor input() {
	return nvgRGB(0x8f, 0x3d, 0xff);
}

NVGcolor output() {
	return nvgRGB(0x00, 0xe6, 0xff);
}

} // namespace levi_jack

namespace {

const char* dynamicRingJackCorePath(DynamicRingJack::CoreStyle coreStyle) {
	switch (coreStyle) {
		case DynamicRingJack::OUTPUT_CORE:
			return "res/icon/dynamic_jack_core_output.svg";
		case DynamicRingJack::INPUT_CORE:
		default:
			return "res/icon/dynamic_jack_core_input.svg";
	}
}

struct DynamicRingLayer : TransparentWidget {
	DynamicRingJack* jack = nullptr;

	void draw(const DrawArgs& args) override {
		if (jack) {
			jack->drawRingLayer(args);
		}
	}
};

void setSvgPortSizePx(app::SvgPort* port, float px, float rotationRad = 0.f) {
	if (!port) {
		return;
	}
	const Vec size(px, px);
	if (port->fb && port->sw) {
		const Vec svgSize = port->sw->box.size;
		const float svgMax = std::max(svgSize.x, svgSize.y);
		if (svgMax > 0.f) {
			const float scale = px / svgMax;
			port->fb->removeChild(port->sw);
			port->sw->box.pos = Vec(0.f, 0.f);
			TransformWidget* scaleTw = new TransformWidget();
			scaleTw->addChild(port->sw);
			scaleTw->scale(Vec(scale, scale));
			scaleTw->box.size = svgSize.mult(scale);
			if (std::fabs(rotationRad) > 1e-6f) {
				TransformWidget* rotateTw = new TransformWidget();
				rotateTw->addChild(scaleTw);
				rotateTw->rotate(rotationRad, size.div(2.f));
				rotateTw->box.size = size;
				port->fb->addChild(rotateTw);
			}
			else {
				port->fb->addChild(scaleTw);
			}
		}
	}
	port->box.size = size;
	if (port->fb) {
		port->fb->box.size = size;
	}
	if (port->shadow) {
		port->shadow->box.size = size;
	}
}

TransformWidget* setSvgSwitchSizePx(app::SvgSwitch* button, float px) {
	if (!button) {
		return nullptr;
	}
	TransformWidget* scaleTw = nullptr;
	const Vec size(px, px);
	if (button->fb && button->sw) {
		const Vec svgSize = button->sw->box.size;
		const float svgMax = std::max(svgSize.x, svgSize.y);
		if (svgMax > 0.f) {
			const float scale = px / svgMax;
			button->fb->removeChild(button->sw);
			button->sw->box.pos = Vec(0.f, 0.f);
			scaleTw = new TransformWidget();
			scaleTw->addChild(button->sw);
			scaleTw->scale(Vec(scale, scale));
			scaleTw->box.size = svgSize.mult(scale);
			button->fb->addChild(scaleTw);
		}
	}
	button->box.size = size;
	if (button->fb) {
		button->fb->box.size = size;
	}
	if (button->shadow) {
		button->shadow->box.size = size;
	}
	return scaleTw;
}

constexpr float kMagitekPortSizePx = 24.5f;
constexpr float kGoldButtonSizePx = 24.f;

struct MagitekInputShadow : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const Vec center = box.size.div(2.f).plus(Vec(1.6f, 2.5f));
		const float outerRadius = std::min(box.size.x, box.size.y) * 0.43f;
		const float innerRadius = outerRadius * 0.48f;
		NVGpaint paint = nvgRadialGradient(args.vg,
			center.x,
			center.y,
			innerRadius,
			outerRadius,
			nvgRGBA(0, 0, 0, 138),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, center.x, center.y, outerRadius);
		nvgFillPaint(args.vg, paint);
		nvgFill(args.vg);
	}
};

struct MagitekOutputShadow : TransparentWidget {
	float rotationRad = 0.f;

	explicit MagitekOutputShadow(float rotationRad)
		: rotationRad(rotationRad) {
	}

	void drawHex(const DrawArgs& args, float radius, NVGcolor color) {
		const Vec center = box.size.div(2.f).plus(Vec(0.55f, 0.95f));
		nvgBeginPath(args.vg);
		for (int i = 0; i < 6; ++i) {
			const float angle = rotationRad - 0.5f * M_PI + float(i) * (M_PI / 3.f);
			const float x = center.x + std::cos(angle) * radius;
			const float y = center.y + std::sin(angle) * radius;
			if (i == 0) {
				nvgMoveTo(args.vg, x, y);
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
		}
		nvgClosePath(args.vg);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
	}

	void draw(const DrawArgs& args) override {
		const float radius = kMagitekPortSizePx * 0.46f;
		drawHex(args, radius * 1.22f, nvgRGBA(0, 0, 0, 28));
		drawHex(args, radius * 1.04f, nvgRGBA(0, 0, 0, 62));
		drawHex(args, radius * 0.86f, nvgRGBA(0, 0, 0, 132));
	}
};

struct GoldButtonShadow : TransparentWidget {
	float pressAmount = 0.f;

	void draw(const DrawArgs& args) override {
		const float p = clamp(pressAmount, 0.f, 1.f);
		const Vec base = box.size.div(2.f);

		const Vec castCenter = base.plus(Vec(crossfade(1.15f, 0.25f, p), crossfade(2.55f, 1.35f, p)));
		const float castRx = box.size.x * crossfade(0.37f, 0.29f, p);
		const float castRy = box.size.y * crossfade(0.33f, 0.23f, p);
		NVGpaint castPaint = nvgRadialGradient(args.vg,
			castCenter.x,
			castCenter.y,
			box.size.x * crossfade(0.20f, 0.08f, p),
			box.size.x * crossfade(0.43f, 0.31f, p),
			nvgRGBA(0, 0, 0, int(std::round(crossfade(96.f, 50.f, p)))),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgEllipse(args.vg, castCenter.x, castCenter.y, castRx, castRy);
		nvgFillPaint(args.vg, castPaint);
		nvgFill(args.vg);

		const Vec contactCenter = base.plus(Vec(crossfade(0.28f, 0.08f, p), crossfade(1.55f, 1.08f, p)));
		const float contactRx = box.size.x * crossfade(0.30f, 0.38f, p);
		const float contactRy = box.size.y * crossfade(0.11f, 0.085f, p);
		NVGpaint contactPaint = nvgRadialGradient(args.vg,
			contactCenter.x,
			contactCenter.y,
			box.size.x * crossfade(0.07f, 0.15f, p),
			box.size.x * crossfade(0.33f, 0.40f, p),
			nvgRGBA(0, 0, 0, int(std::round(crossfade(54.f, 128.f, p)))),
			nvgRGBA(0, 0, 0, 0));
		nvgBeginPath(args.vg);
		nvgEllipse(args.vg, contactCenter.x, contactCenter.y, contactRx, contactRy);
		nvgFillPaint(args.vg, contactPaint);
		nvgFill(args.vg);
	}
};

struct GoldButtonFixedBezel : TransparentWidget {
	void draw(const DrawArgs& args) override {
		const Vec c = box.size.div(2.f);
		const float r = box.size.x * 0.50f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, r);
		nvgFillColor(args.vg, nvgRGB(17, 16, 19));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, r - 0.7f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 190));
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStroke(args.vg);
	}
};

struct GoldButtonPressOverlay : TransparentWidget {
	float pressAmount = 0.f;

	void draw(const DrawArgs& args) override {
		if (pressAmount <= 0.001f) {
			return;
		}
		const Vec c = box.size.div(2.f);
		const float radius = box.size.x * 0.47f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, c.x, c.y, radius);
		NVGpaint shade = nvgLinearGradient(args.vg,
			c.x,
			c.y - radius,
			c.x,
			c.y + radius,
			nvgRGBA(0, 0, 0, int(72.f * pressAmount)),
			nvgRGBA(255, 238, 160, int(28.f * pressAmount)));
		nvgFillPaint(args.vg, shade);
		nvgFill(args.vg);
	}
};

void installMagitekShadow(app::SvgPort* port, Widget* customShadow) {
	if (!port || !customShadow) {
		delete customShadow;
		return;
	}
	if (port->shadow) {
		port->shadow->opacity = 0.f;
	}
	const bool inputShadow = dynamic_cast<MagitekInputShadow*>(customShadow) != nullptr;
	const bool outputShadow = dynamic_cast<MagitekOutputShadow*>(customShadow) != nullptr;
	widget::FramebufferWidget* shadowFb = new widget::FramebufferWidget();
	shadowFb->dirtyOnSubpixelChange = false;
	if (inputShadow) {
		const Vec bleed(8.f, 8.f);
		shadowFb->box.pos = bleed.mult(-0.5f);
		shadowFb->box.size = port->box.size.plus(bleed);
	}
	else if (outputShadow) {
		const Vec bleed(10.f, 10.f);
		shadowFb->box.pos = bleed.mult(-0.5f);
		shadowFb->box.size = port->box.size.plus(bleed);
	}
	else {
		shadowFb->box.size = port->box.size;
	}
	customShadow->box.pos = Vec(0.f, 0.f);
	customShadow->box.size = shadowFb->box.size;
	shadowFb->addChild(customShadow);
	if (port->fb) {
		port->addChildBelow(shadowFb, port->fb);
	}
	else {
		port->addChildBottom(shadowFb);
	}
}

struct ClockworkDragDebugRecorder {
	std::ofstream file;
	std::string path;
	double startTimeSec = 0.0;
	uint64_t sequence = 0;
	uint64_t gestureSequence = 0;

	std::string userRootPath() {
		return system::join(asset::user(), "Leviathan/UI");
	}

	bool ensureOpen() {
		if (file.is_open()) {
			return true;
		}
		system::createDirectories(userRootPath());
		const long long stampMs = (long long)std::llround(system::getUnixTime() * 1000.0);
		path = system::join(userRootPath(), "clockwork_knob_drag_" + std::to_string(stampMs) + ".csv");
		file.open(path);
		if (!file.is_open()) {
			WARN("Failed to open Clockwork knob drag debug CSV: %s", path.c_str());
			path.clear();
			return false;
		}
		file << std::setprecision(9);
		file << "sequence,gesture,t_sec,event,param_id,module_id,frame,knob_mode,mods,"
			<< "mouse_dx,mouse_dy,mouse_len,sent_dx,sent_dy,sent_len,max_len,clamped,value_before,value_after\n";
		startTimeSec = system::getTime();
		sequence = 0;
		DEBUG("Started Clockwork knob drag debug CSV: %s", path.c_str());
		return true;
	}

	uint64_t nextGesture() {
		return ++gestureSequence;
	}

	void log(
		const char* eventName,
		GearKnobInvertSized* knob,
		uint64_t gestureId,
		int frame,
		Vec mouseDelta,
		Vec sentDelta,
		float maxLen,
		bool clamped,
		float valueBefore,
		float valueAfter) {
		if (!ensureOpen()) {
			return;
		}
		const int moduleId = (knob && knob->module) ? knob->module->id : -1;
		const int knobMode = int(settings::knobMode);
		const int mods = (APP && APP->window) ? APP->window->getMods() : 0;
		const double tSec = std::max(0.0, system::getTime() - startTimeSec);
		file
			<< sequence++ << ','
			<< gestureId << ','
			<< tSec << ','
			<< (eventName ? eventName : "") << ','
			<< (knob ? knob->paramId : -1) << ','
			<< moduleId << ','
			<< frame << ','
			<< knobMode << ','
			<< mods << ','
			<< mouseDelta.x << ','
			<< mouseDelta.y << ','
			<< mouseDelta.norm() << ','
			<< sentDelta.x << ','
			<< sentDelta.y << ','
			<< sentDelta.norm() << ','
			<< maxLen << ','
			<< (clamped ? 1 : 0) << ','
			<< valueBefore << ','
			<< valueAfter << '\n';
		if (clamped || (eventName && eventName[0] == 'e')) {
			file.flush();
		}
	}
};

ClockworkDragDebugRecorder& clockworkDragDebugRecorder() {
	static ClockworkDragDebugRecorder recorder;
	return recorder;
}

float clockworkParamValue(GearKnobInvertSized* knob) {
	engine::ParamQuantity* pq = knob ? knob->getParamQuantity() : nullptr;
	return pq ? pq->getValue() : NAN;
}

static constexpr bool kClockworkLiquidShimmerEnabled = true;
static constexpr double kClockworkLiquidShimmerDurationSec = 0.70;

} // namespace

MagitekInputJack::MagitekInputJack() {
	constexpr float rotationRad = float(M_PI) / 4.f;
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/icon/magitek_input.svg")));
	setSvgPortSizePx(this, kMagitekPortSizePx, rotationRad);
	installMagitekShadow(this, new MagitekInputShadow);
}

MagitekOutputJack::MagitekOutputJack() {
	constexpr float rotationRad = float(M_PI) / 6.f;
	setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/icon/magitek_output.svg")));
	setSvgPortSizePx(this, kMagitekPortSizePx, rotationRad);
	installMagitekShadow(this, new MagitekOutputShadow(rotationRad));
}

DynamicRingJack::DynamicRingJack()
: DynamicRingJack(INPUT_CORE) {
}

DynamicRingJack::DynamicRingJack(const char* coreSvgPath) {
	setSvg(visual_assets::loadPluginSvgCached(coreSvgPath));
	setSvgPortSizePx(this, kMagitekPortSizePx);
	if (shadow) {
		shadow->opacity = 0.f;
	}
	installMagitekShadow(this, new MagitekInputShadow);

	ringFb = new widget::FramebufferWidget();
	ringFb->dirtyOnSubpixelChange = false;
	ringFb->box.size = box.size;
	DynamicRingLayer* layer = new DynamicRingLayer();
	layer->jack = this;
	layer->box.size = box.size;
	ringLayer = layer;
	ringFb->addChild(layer);
	addChild(ringFb);
}

DynamicRingJack::DynamicRingJack(CoreStyle coreStyle)
: DynamicRingJack(dynamicRingJackCorePath(coreStyle)) {
}

void DynamicRingJack::setRingColor(NVGcolor color) {
	ringColor = color;
	markRingDirty();
}

void DynamicRingJack::setGlowAmount(float amount) {
	glowAmount = clamp(amount, 0.f, 1.f);
	markRingDirty();
}

void DynamicRingJack::setCoreStyle(CoreStyle coreStyle) {
	setSvg(visual_assets::loadPluginSvgCached(dynamicRingJackCorePath(coreStyle)));
	setSvgPortSizePx(this, kMagitekPortSizePx);
	if (shadow) {
		shadow->opacity = 0.f;
	}
	if (ringFb) {
		ringFb->box.size = box.size;
	}
	if (ringLayer) {
		ringLayer->box.size = box.size;
	}
	markRingDirty();
}

void DynamicRingJack::markRingDirty() {
	if (ringFb) {
		ringFb->setDirty();
	}
}

void DynamicRingJack::drawRingLayer(const DrawArgs& args) {
	const Vec center = box.size.div(2.f);
	const float s = std::min(box.size.x, box.size.y);
	const float connectedBoost = (connectedGlow && cachedConnected) ? 1.18f : 1.f;
	NVGcolor color = ringColor;

	auto withAlpha = [](NVGcolor c, float alpha) {
		c.a = clamp(alpha, 0.f, 1.f);
		return c;
	};

	const float glowOuter = s * 0.382f;
	const float glowInner = s * 0.18f;
	const float ringOuter = s * 0.352f;
	const float ringInner = s * 0.282f;
	const float ringMid = 0.5f * (ringOuter + ringInner);
	const float socketOuter = s * 0.247f;
	const float socketInner = s * 0.19f;

	NVGpaint glow = nvgRadialGradient(
		args.vg,
		center.x,
		center.y,
		glowInner,
		glowOuter,
		withAlpha(color, glowAmount * 0.58f * connectedBoost),
		withAlpha(color, 0.f)
	);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, glowOuter);
	nvgFillPaint(args.vg, glow);
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, ringOuter);
	nvgCircle(args.vg, center.x, center.y, ringInner);
	nvgPathWinding(args.vg, NVG_HOLE);
	nvgFillColor(args.vg, withAlpha(color, ringAmount * 0.74f));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, ringMid);
	nvgStrokeWidth(args.vg, std::max(0.7f, s * 0.032f));
	nvgStrokeColor(args.vg, withAlpha(color, ringAmount * 0.98f));
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, ringInner + s * 0.010f);
	nvgStrokeWidth(args.vg, std::max(0.45f, s * 0.015f));
	nvgStrokeColor(args.vg, withAlpha(color, ringAmount * 0.55f));
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, ringOuter - s * 0.015f);
	nvgStrokeWidth(args.vg, std::max(0.55f, s * 0.018f));
	nvgStrokeColor(args.vg, withAlpha(color, ringAmount * 0.54f));
	nvgStroke(args.vg);

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringOuter - s * 0.012f, -0.78f * M_PI, -0.18f * M_PI, NVG_CW);
	nvgStrokeWidth(args.vg, std::max(0.48f, s * 0.016f));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 72));
	nvgStroke(args.vg);

	NVGpaint socketBevel = nvgRadialGradient(
		args.vg,
		center.x - s * 0.045f,
		center.y - s * 0.055f,
		s * 0.025f,
		socketOuter,
		nvgRGBA(30, 35, 45, 255),
		nvgRGBA(0, 1, 4, 255)
	);
	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, socketOuter);
	nvgFillPaint(args.vg, socketBevel);
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgCircle(args.vg, center.x, center.y, socketInner);
	nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 255));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, socketOuter - s * 0.024f, -0.82f * M_PI, -0.30f * M_PI, NVG_CW);
	nvgStrokeWidth(args.vg, std::max(0.42f, s * 0.014f));
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 36));
	nvgStroke(args.vg);
}

void DynamicRingJack::step() {
	engine::Port* port = getPort();
	const bool connected = port && port->isConnected();
	if (connected != cachedConnected) {
		cachedConnected = connected;
		if (connectedGlow) {
			markRingDirty();
		}
	}
	if (ringFb && (ringFb->box.size.x != box.size.x || ringFb->box.size.y != box.size.y)) {
		ringFb->box.size = box.size;
		if (ringLayer) {
			ringLayer->box.size = box.size;
		}
		markRingDirty();
	}
	SvgPort::step();
}

DynamicRingInputJack::DynamicRingInputJack()
: DynamicRingJack(INPUT_CORE) {
	ringAmount = 1.f;
	setRingColor(levi_jack::input());
	setGlowAmount(0.36f);
}

DynamicRingOutputJack::DynamicRingOutputJack()
: DynamicRingJack(OUTPUT_CORE) {
	setRingColor(levi_jack::output());
	setGlowAmount(0.34f);
}

GoldButton::GoldButton() {
	momentary = true;
	std::shared_ptr<window::Svg> svg = visual_assets::loadPluginSvgCached("res/icon/gold_button.svg");
	addFrame(svg);
	addFrame(svg);
	faceTransform = setSvgSwitchSizePx(this, kGoldButtonSizePx);
	if (shadow) {
		shadow->opacity = 0.f;
	}
	widget::FramebufferWidget* shadowLayerFb = new widget::FramebufferWidget();
	shadowLayerFb->dirtyOnSubpixelChange = false;
	shadowLayerFb->box.pos = Vec(-4.f, -3.f);
	shadowLayerFb->box.size = box.size.plus(Vec(8.f, 8.f));
	GoldButtonShadow* shadowWidget = new GoldButtonShadow();
	shadowWidget->box.size = shadowLayerFb->box.size;
	shadowLayerFb->addChild(shadowWidget);
	dropShadow = shadowWidget;
	dropShadowFb = shadowLayerFb;
	if (fb) {
		addChildBelow(shadowLayerFb, fb);
	}
	else {
		addChildBottom(shadowLayerFb);
	}

	widget::FramebufferWidget* bezelFb = new widget::FramebufferWidget();
	bezelFb->dirtyOnSubpixelChange = false;
	bezelFb->box.size = box.size;
	GoldButtonFixedBezel* bezel = new GoldButtonFixedBezel();
	bezel->box.size = bezelFb->box.size;
	bezelFb->addChild(bezel);
	fixedBezel = bezel;
	fixedBezelFb = bezelFb;
	if (fb) {
		addChildBelow(bezelFb, fb);
	}
	else {
		addChild(bezelFb);
	}

	widget::FramebufferWidget* overlayFb = new widget::FramebufferWidget();
	overlayFb->dirtyOnSubpixelChange = false;
	overlayFb->box.size = box.size;
	GoldButtonPressOverlay* overlay = new GoldButtonPressOverlay();
	overlay->box.size = overlayFb->box.size;
	overlayFb->addChild(overlay);
	pressOverlay = overlay;
	pressOverlayFb = overlayFb;
	addChild(overlayFb);
}

void GoldButton::step() {
	app::SvgSwitch::step();
	engine::ParamQuantity* pq = getParamQuantity();
	const float target = (pq && pq->getValue() > 0.5f) ? 1.f : 0.f;
	const float oldPressAmount = pressAmount;
	pressAmount += (target - pressAmount) * (target > pressAmount ? 0.34f : 0.42f);
	if (std::fabs(target - pressAmount) < 0.001f) {
		pressAmount = target;
	}
	const bool pressChanged = std::fabs(pressAmount - oldPressAmount) > 0.0001f;
	if (faceTransform) {
		faceTransform->identity();
		const float scale = kGoldButtonSizePx / 64.f;
		faceTransform->translate(Vec(0.f, 1.05f * pressAmount));
		faceTransform->scale(Vec(scale, scale));
	}
	if (auto* shadowWidget = dynamic_cast<GoldButtonShadow*>(dropShadow)) {
		shadowWidget->pressAmount = pressAmount;
	}
	if (auto* overlay = dynamic_cast<GoldButtonPressOverlay*>(pressOverlay)) {
		overlay->pressAmount = pressAmount;
	}
	if (dropShadowFb) {
		dropShadowFb->box.pos = Vec(-4.f, crossfade(-3.15f, -2.35f, pressAmount));
	}
	if (pressChanged) {
		if (fb) {
			fb->setDirty();
		}
		if (dropShadowFb) {
			dropShadowFb->setDirty();
		}
		if (pressOverlayFb) {
			pressOverlayFb->setDirty();
		}
	}
}

void GearKnobInvertSized::ActiveRingWidget::draw(const DrawArgs& args) {
	const float clampedValueNorm = clamp(valueNorm, 0.f, 1.f);
	const float clampedCenterNorm = clamp(centerNorm, 0.f, 1.f);
	const float knobAngle = crossfade(minAngle, maxAngle, clampedValueNorm);
	const float centerAngle = crossfade(minAngle, maxAngle, clampedCenterNorm);
	const float assetScale = sourceDiameterPx / sourceViewBoxPx;
	const Vec center(centerPx, centerPx);
	const float ringRadius = ringRadiusSourcePx * assetScale;
	const float ringWidth = ringWidthSourcePx * assetScale;
	const float activeRingWidth = activeRingWidthSourcePx * assetScale;
	const float startAngle = -0.5f * M_PI + minAngle;
	const float endAngle = -0.5f * M_PI + maxAngle;
	const float activeAngle = -0.5f * M_PI + knobAngle;
	const float centerArcAngle = -0.5f * M_PI + centerAngle;

	nvgSave(args.vg);

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringRadius, startAngle, endAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(2, 1, 1, 230));
	nvgStrokeWidth(args.vg, ringWidth);
	nvgLineCap(args.vg, NVG_ROUND);
	nvgStroke(args.vg);

	const bool drawActive = !bipolar || std::fabs(clampedValueNorm - clampedCenterNorm) > 0.001f;
	if (drawActive) {
		const float activeStartAngle = bipolar ? centerArcAngle : startAngle;
		const float activeEndAngle = activeAngle;
		nvgBeginPath(args.vg);
		nvgArc(args.vg,
			center.x,
			center.y,
			ringRadius,
			std::min(activeStartAngle, activeEndAngle),
			std::max(activeStartAngle, activeEndAngle),
			NVG_CW);
		NVGpaint activePaint = nvgLinearGradient(args.vg,
			center.x - ringRadius, center.y,
			center.x + ringRadius, center.y,
			// Eclipse-orange option: nvgRGBA(240, 138, 36, 248), nvgRGBA(255, 210, 154, 255)
			nvgRGBA(255, 218, 42, 248),
			nvgRGBA(255, 250, 205, 255));
		nvgStrokePaint(args.vg, activePaint);
		nvgStrokeWidth(args.vg, activeRingWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		if (kClockworkLiquidShimmerEnabled) {
			const double now = system::getTime();
			const double remaining = liquidShimmerUntil - now;
			if (remaining > 0.0) {
				const float fade = clamp(float(remaining / kClockworkLiquidShimmerDurationSec), 0.f, 1.f);
				const float phase = float(std::fmod(now * 1.35, 1.0));
				const float sweepX = ringRadius * 2.4f;
				const float shimmerStartX = center.x - ringRadius * 1.2f + sweepX * phase;
				NVGpaint shimmerPaint = nvgLinearGradient(args.vg,
					shimmerStartX, center.y - ringRadius,
					shimmerStartX + ringRadius * 0.55f, center.y + ringRadius,
					// Eclipse-orange option: nvgRGBA(255, 215, 163, 0), nvgRGBA(255, 230, 190, alpha)
					nvgRGBA(255, 255, 220, 0),
					nvgRGBA(255, 255, 250, (unsigned char) std::round(118.f * fade)));
				nvgBeginPath(args.vg);
				nvgArc(args.vg,
					center.x,
					center.y,
					ringRadius,
					std::min(activeStartAngle, activeEndAngle),
					std::max(activeStartAngle, activeEndAngle),
					NVG_CW);
				nvgStrokePaint(args.vg, shimmerPaint);
				nvgStrokeWidth(args.vg, std::max(1.f, activeRingWidth * 0.55f));
				nvgLineCap(args.vg, NVG_ROUND);
				nvgStroke(args.vg);
			}
		}
	}

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringRadius - 0.5f * ringWidth, startAngle, endAngle, NVG_CW);
	// Eclipse-orange option: nvgRGBA(255, 210, 154, 76)
	nvgStrokeColor(args.vg, nvgRGBA(255, 244, 154, 80));
	if (innerLineWidthSourcePx > 0.f) {
		nvgStrokeWidth(args.vg, innerLineWidthSourcePx * assetScale);
		nvgStroke(args.vg);
	}

	nvgRestore(args.vg);
}

GearKnobInvertSized::ShadowWidget::ShadowWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void GearKnobInvertSized::ShadowWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void GearKnobInvertSized::ShadowWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const float angle = crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	const Vec center = box.size.mult(0.5f);
	struct ShadowPass {
		float offsetX;
		float offsetY;
		float scaleMul;
		float alpha;
	};
	const ShadowPass passes[] = {
		{0.18f, 0.28f, 1.003f, 62.f / 255.f},
		{0.45f, 0.62f, 1.010f, 106.f / 255.f},
		{0.78f, 1.05f, 1.020f, 58.f / 255.f},
	};

	for (const ShadowPass& pass : passes) {
		nvgSave(args.vg);
		nvgGlobalAlpha(args.vg, pass.alpha);
		nvgTranslate(args.vg, center.x + pass.offsetX, center.y + pass.offsetY);
		nvgRotate(args.vg, angle);
		nvgScale(args.vg, scale * pass.scaleMul, scale * pass.scaleMul);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		Widget::draw(args);
		nvgRestore(args.vg);
	}
}

GearKnobInvertSized::GearKnobInvertSized() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	setCachedSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_invert.svg"));
	if (shadow) {
		shadow->opacity = 0.f;
	}
	shadowLayer = new ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_shadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChildBelow(shadowLayer, tw);
	activeRing = new ActiveRingWidget();
	activeRing->box.size = box.size;
	activeRing->minAngle = minAngle;
	activeRing->maxAngle = maxAngle;
	activeRing->valueNorm = normalizedParamValue();
	fb->addChild(activeRing);
}

void GearKnobInvertSized::draw(const DrawArgs& args) {
	app::SvgKnob::draw(args);
}

void GearKnobInvertSized::step() {
	app::SvgKnob::step();
	if (kClockworkLiquidShimmerEnabled && activeRing && fb && system::getTime() < activeRing->liquidShimmerUntil) {
		fb->setDirty();
	}
}

void GearKnobInvertSized::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (activeRing) {
		activeRing->valueNorm = valueNorm;
		if (kClockworkLiquidShimmerEnabled) {
			activeRing->liquidShimmerUntil = system::getTime() + kClockworkLiquidShimmerDurationSec;
		}
	}
	if (fb) {
		fb->setDirty();
	}
}

void GearKnobInvertSized::onDragStart(const DragStartEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragStart(e);
		return;
	}
	dragMoveFrame = 0;
	if (isClockworkDragDebugLoggingEnabled()) {
		dragLogGestureId = clockworkDragDebugRecorder().nextGesture();
		const float valueBefore = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("start", this, dragLogGestureId, -1, Vec(), Vec(), 0.f, false, valueBefore, valueBefore);
	}
	else {
		dragLogGestureId = 0;
	}
	app::SvgKnob::onDragStart(e);
}

void GearKnobInvertSized::onDragEnd(const DragEndEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragEnd(e);
		return;
	}
	if (isClockworkDragDebugLoggingEnabled() && dragLogGestureId != 0) {
		const float valueAfter = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("end", this, dragLogGestureId, dragMoveFrame, Vec(), Vec(), 0.f, false, valueAfter, valueAfter);
	}
	dragMoveFrame = 0;
	dragLogGestureId = 0;
	app::SvgKnob::onDragEnd(e);
}

void GearKnobInvertSized::onDragMove(const DragMoveEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		app::SvgKnob::onDragMove(e);
		return;
	}
	const bool logMove = isClockworkDragDebugLoggingEnabled() && dragLogGestureId != 0 && dragMoveFrame < 8;
	const float valueBefore = logMove ? clockworkParamValue(this) : NAN;
	dragMoveFrame++;
	app::SvgKnob::onDragMove(e);
	if (logMove) {
		const float valueAfter = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("move", this, dragLogGestureId, dragMoveFrame - 1, e.mouseDelta, e.mouseDelta, 0.f, false, valueBefore, valueAfter);
	}
}

void GearKnobInvertSized::setCachedSvg(std::shared_ptr<window::Svg> svg) {
	app::SvgKnob::setSvg(svg);
	if (sw) {
		sw->hide();
	}
	if (!svg) {
		return;
	}
	if (!cachedSvgFb) {
		cachedSvgFb = new widget::FramebufferWidget();
		cachedSvgFb->dirtyOnSubpixelChange = false;
		cachedSvgSw = new widget::SvgWidget();
		cachedSvgFb->addChild(cachedSvgSw);
		tw->addChild(cachedSvgFb);
	}
	if (cachedSvgSw) {
		cachedSvgSw->setSvg(svg);
		cachedSvgFb->box.size = cachedSvgSw->box.size;
		cachedSvgFb->setDirty();
	}
	if (shadowLayer) {
		shadowLayer->box.size = box.size;
	}
}

float GearKnobInvertSized::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

TinyClockworkGearKnob::TinyClockworkGearKnob() {
	minAngle = -0.8 * M_PI;
	maxAngle = 0.8 * M_PI;
	setCachedSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_tiny.svg"));
	if (shadowLayer) {
		shadowLayer->box.size = box.size;
		shadowLayer->minAngle = minAngle;
		shadowLayer->maxAngle = maxAngle;
		shadowLayer->valueNorm = normalizedParamValue();
	}
	if (activeRing) {
		activeRing->box.size = box.size;
		activeRing->minAngle = minAngle;
		activeRing->maxAngle = maxAngle;
		activeRing->valueNorm = normalizedParamValue();
		activeRing->centerPx = 12.f;
		activeRing->sourceDiameterPx = 24.f;
		activeRing->sourceViewBoxPx = 56.f;
		activeRing->ringRadiusSourcePx = 16.4f;
		activeRing->ringWidthSourcePx = 8.0f;
		activeRing->activeRingWidthSourcePx = 5.8f;
		activeRing->innerLineWidthSourcePx = 0.0f;
	}
	if (fb) {
		fb->setDirty();
	}
}

BipolarTinyClockworkGearKnob::BipolarTinyClockworkGearKnob() {
	if (activeRing) {
		activeRing->bipolar = true;
		activeRing->centerNorm = 0.5f;
		activeRing->valueNorm = normalizedParamValue();
	}
	if (fb) {
		fb->setDirty();
	}
}

void EclipseKnob::ProgressRingWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float radiusPx = diameterPx * (46.f / 120.f);
	const float strokeWidthPx = std::max(1.35f, diameterPx * (5.8f / 120.f));
	const float inactiveStrokeWidthPx = std::max(0.95f, strokeWidthPx * 0.84f);
	const float inactiveRadiusPx = radiusPx - 0.5f * (inactiveStrokeWidthPx - strokeWidthPx * 0.72f);
	const float activeStrokeWidthPx = std::max(strokeWidthPx, diameterPx * (7.2f / 120.f));
	const float activeRadiusPx = radiusPx - 0.5f * (activeStrokeWidthPx - strokeWidthPx);
	const float startNorm = bipolar ? centerNorm : 0.f;
	const float startAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(startNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float sweep = std::fabs(endAngle - startAngle);
	const float dashAngle = 0.11f;
	const float gapAngle = 0.225f;
	const float periodAngle = dashAngle + gapAngle;
	const float topAngle = -0.5f * float(M_PI);

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);

	const float ringMinAngle = -0.5f * float(M_PI) + minAngle;
	const float ringMaxAngle = -0.5f * float(M_PI) + maxAngle;
	float firstSegmentAngle = topAngle + 0.5f * gapAngle;
	while (firstSegmentAngle - periodAngle > ringMinAngle) {
		firstSegmentAngle -= periodAngle;
	}
	for (float a = firstSegmentAngle; a < ringMaxAngle; a += periodAngle) {
		const float b = std::min(a + dashAngle, ringMaxAngle);
		if (b <= ringMinAngle) continue;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, inactiveRadiusPx, std::max(a, ringMinAngle), b, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(142, 124, 72, 118));
		nvgStrokeWidth(args.vg, inactiveStrokeWidthPx);
		nvgStroke(args.vg);
	}

	if (sweep > 0.008f) {
		const NVGcolor activeColor = nvgRGBA(255, 242, 184, 248);
		const float activeMinAngle = std::min(startAngle, endAngle);
		const float activeMaxAngle = std::max(startAngle, endAngle);
		for (float a = firstSegmentAngle; a < activeMaxAngle; a += periodAngle) {
			const float b = a + dashAngle;
			const float a0 = std::max(a, activeMinAngle);
			const float a1 = std::min(b, activeMaxAngle);
			if (a1 <= a0) continue;
			nvgBeginPath(args.vg);
			nvgArc(args.vg,
				center.x,
				center.y,
				activeRadiusPx,
				a0,
				a1,
				NVG_CW);
			nvgStrokeColor(args.vg, activeColor);
			nvgStrokeWidth(args.vg, activeStrokeWidthPx);
			nvgStroke(args.vg);
		}
	}

	nvgRestore(args.vg);
}

EclipseKnob::SvgLayer::SvgLayer() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void EclipseKnob::SvgLayer::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void EclipseKnob::SvgLayer::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	const Vec center = box.size.mult(0.5f);
	const float angle = rotateWithValue ? crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f)) : 0.f;

	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, angle);
	nvgScale(args.vg, scale, scale);
	nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
	Widget::draw(args);
	nvgRestore(args.vg);
}

EclipseKnob::ShadowWidget::ShadowWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void EclipseKnob::ShadowWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void EclipseKnob::ShadowWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 1.f) return;

	const bool measure = isDragonKingDebugEnabled();
	const std::chrono::steady_clock::time_point start = measure ? std::chrono::steady_clock::now() : std::chrono::steady_clock::time_point();
	const float angle = crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	const Vec center = box.size.mult(0.5f);
	struct ShadowPass {
		float offsetX;
		float offsetY;
		float scaleMul;
		float alpha;
	};
	const ShadowPass passes[] = {
		{0.22f, 0.32f, 1.003f, 62.f / 255.f},
		{0.52f, 0.70f, 1.009f, 106.f / 255.f},
		{0.90f, 1.18f, 1.018f, 58.f / 255.f},
	};

	for (const ShadowPass& pass : passes) {
		nvgSave(args.vg);
		nvgGlobalAlpha(args.vg, pass.alpha);
		nvgTranslate(args.vg, center.x + pass.offsetX, center.y + pass.offsetY);
		nvgRotate(args.vg, angle);
		nvgScale(args.vg, scale * pass.scaleMul, scale * pass.scaleMul);
		nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
		Widget::draw(args);
		nvgRestore(args.vg);
	}
	if (measure) {
		visual_assets::gEclipseShadowDrawNs += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::steady_clock::now() - start).count());
		visual_assets::gEclipseShadowDrawCount++;
	}
}

EclipseKnob::EclipseKnob() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/EclipseKnobBack.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(28.f, 28.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}
	shadowLayer = new ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/EclipseKnobShadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChild(shadowLayer);
	setBackSvg(backSvg);
	progressRing = new ProgressRingWidget();
	progressRing->box.size = box.size;
	progressRing->minAngle = minAngle;
	progressRing->maxAngle = maxAngle;
	progressRing->valueNorm = normalizedParamValue();
	fb->addChild(progressRing);
	setPointerSvg(visual_assets::loadPluginSvgCached("res/icon/EclipseKnobPointer.svg"));
}

void EclipseKnob::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (pointerLayer) {
		pointerLayer->valueNorm = valueNorm;
	}
	if (progressRing) {
		progressRing->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

void EclipseKnob::setBackSvg(std::shared_ptr<window::Svg> svg) {
	if (!svg || !fb) return;
	if (!backLayer) {
		backLayer = new SvgLayer();
		backLayer->minAngle = minAngle;
		backLayer->maxAngle = maxAngle;
		backLayer->valueNorm = normalizedParamValue();
		backLayer->rotateWithValue = true;
		fb->addChild(backLayer);
	}
	backLayer->setSvg(svg);
	backLayer->box.size = box.size;
}

void EclipseKnob::setPointerSvg(std::shared_ptr<window::Svg> svg) {
	if (!svg || !fb) return;
	if (!pointerLayer) {
		pointerLayer = new SvgLayer();
		pointerLayer->minAngle = minAngle;
		pointerLayer->maxAngle = maxAngle;
		pointerLayer->valueNorm = normalizedParamValue();
		pointerLayer->rotateWithValue = true;
		fb->addChild(pointerLayer);
	}
	pointerLayer->setSvg(svg);
	pointerLayer->box.size = box.size;
}

float EclipseKnob::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

void EclipseKnob::setProgressRingBipolar(bool bipolar, float centerNorm) {
	if (!progressRing) return;
	progressRing->bipolar = bipolar;
	progressRing->centerNorm = clamp(centerNorm, 0.f, 1.f);
	if (fb) {
		fb->setDirty();
	}
}

void LeviathanHaloKnob::LightArcWidget::draw(const DrawArgs& args) {
	const float diameterPx = std::min(box.size.x, box.size.y);
	if (diameterPx <= 1.f) return;

	const Vec center = box.size.mult(0.5f);
	const float radiusPx = diameterPx * (15.95f / 46.f);
	const float startAngle = -0.5f * M_PI + minAngle;
	const float activeAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	if (activeAngle <= startAngle + 0.006f) return;

	nvgSave(args.vg);
	auto beginArcBand = [&](float bandRadiusPx, float bandWidthPx) {
		const float halfWidthPx = 0.5f * bandWidthPx;
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, bandRadiusPx + halfWidthPx, startAngle, activeAngle, NVG_CW);
		nvgArc(args.vg, center.x, center.y, bandRadiusPx - halfWidthPx, activeAngle, startAngle, NVG_CCW);
		nvgClosePath(args.vg);
	};

	beginArcBand(radiusPx, std::max(2.2f, diameterPx * (3.05f / 46.f)));
	nvgStrokeColor(args.vg, nvgRGBA(255, 151, 33, 32));
	nvgFillColor(args.vg, nvgRGBA(255, 151, 33, 32));
	nvgFill(args.vg);

	beginArcBand(radiusPx, std::max(1.5f, diameterPx * (2.25f / 46.f)));
	NVGpaint arcPaint = nvgLinearGradient(args.vg,
		center.x - radiusPx,
		center.y,
		center.x + radiusPx,
		center.y,
		nvgRGBA(255, 223, 116, 248),
		nvgRGBA(255, 142, 34, 252));
	nvgFillPaint(args.vg, arcPaint);
	nvgFill(args.vg);

	beginArcBand(radiusPx - diameterPx * (0.46f / 46.f), std::max(0.35f, diameterPx * (0.38f / 46.f)));
	nvgFillColor(args.vg, nvgRGBA(255, 248, 199, 92));
	nvgFill(args.vg);

	nvgRestore(args.vg);
}

LeviathanHaloKnob::LeviathanHaloKnob() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	std::shared_ptr<window::Svg> backSvg = visual_assets::loadPluginSvgCached("res/icon/LeviathanHaloKnobBack_reworked.svg");
	app::SvgKnob::setSvg(backSvg);
	box.size = Vec(46.f, 46.f);
	if (fb) {
		fb->box.size = box.size;
	}
	if (sw) {
		sw->hide();
	}
	if (shadow) {
		shadow->opacity = 0.f;
	}

	shadowLayer = new EclipseKnob::ShadowWidget();
	shadowLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/LeviathanHaloKnobShadow.svg"));
	shadowLayer->box.size = box.size;
	shadowLayer->minAngle = minAngle;
	shadowLayer->maxAngle = maxAngle;
	shadowLayer->valueNorm = normalizedParamValue();
	fb->addChild(shadowLayer);

	backLayer = new EclipseKnob::SvgLayer();
	backLayer->setSvg(backSvg);
	backLayer->box.size = box.size;
	backLayer->minAngle = minAngle;
	backLayer->maxAngle = maxAngle;
	backLayer->valueNorm = normalizedParamValue();
	backLayer->rotateWithValue = false;
	fb->addChild(backLayer);

	lightArc = new LightArcWidget();
	lightArc->box.size = box.size;
	lightArc->minAngle = minAngle;
	lightArc->maxAngle = maxAngle;
	lightArc->valueNorm = normalizedParamValue();
	fb->addChild(lightArc);

	centerLayer = new EclipseKnob::SvgLayer();
	centerLayer->setSvg(visual_assets::loadPluginSvgCached("res/icon/LeviathanHaloKnobCenter_reworked.svg"));
	centerLayer->box.size = box.size;
	centerLayer->minAngle = minAngle;
	centerLayer->maxAngle = maxAngle;
	centerLayer->valueNorm = normalizedParamValue();
	centerLayer->rotateWithValue = true;
	fb->addChild(centerLayer);
}

void LeviathanHaloKnob::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	const float valueNorm = normalizedParamValue();
	if (backLayer) {
		backLayer->valueNorm = valueNorm;
	}
	if (centerLayer) {
		centerLayer->valueNorm = valueNorm;
	}
	if (shadowLayer) {
		shadowLayer->valueNorm = valueNorm;
	}
	if (lightArc) {
		lightArc->valueNorm = valueNorm;
	}
	if (fb) {
		fb->setDirty();
	}
}

float LeviathanHaloKnob::normalizedParamValue() {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!pq) return 0.5f;
	const float minValue = pq->getMinValue();
	const float maxValue = pq->getMaxValue();
	const float range = maxValue - minValue;
	if (range <= 1e-6f) return 0.5f;
	return clamp((pq->getValue() - minValue) / range, 0.f, 1.f);
}

ClockworkGearKnob::CogwheelWidget::CogwheelWidget() {
	cachedSvgFb = new widget::FramebufferWidget();
	cachedSvgFb->dirtyOnSubpixelChange = false;
	cachedSvgSw = new widget::SvgWidget();
	cachedSvgFb->addChild(cachedSvgSw);
	addChild(cachedSvgFb);
}

void ClockworkGearKnob::CogwheelWidget::setSvg(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;
	if (!svg) return;
	if (!cachedSvgSw || !cachedSvgFb) return;
	cachedSvgSw->setSvg(svg);
	cachedSvgFb->box.size = cachedSvgSw->box.size;
	cachedSvgFb->setDirty();
}

void ClockworkGearKnob::CogwheelWidget::draw(const DrawArgs& args) {
	if (!svg) return;
	const Vec svgSize = svg->getSize();
	if (svgSize.x <= 1.f || svgSize.y <= 1.f || diameterPx <= 0.f) return;

	const float scale = diameterPx / std::max(svgSize.x, svgSize.y);
	nvgSave(args.vg);
	nvgTranslate(args.vg, center.x, center.y);
	nvgRotate(args.vg, angleRad);
	nvgScale(args.vg, scale, scale);
	nvgTranslate(args.vg, -0.5f * svgSize.x, -0.5f * svgSize.y);
	Widget::draw(args);
	nvgRestore(args.vg);
}

ClockworkGearKnob::ClockworkGearKnob() {
	primaryCogwheel = new CogwheelWidget();
	secondaryCogwheel = new CogwheelWidget();
	primaryCogwheel->box.size = box.size;
	secondaryCogwheel->box.size = box.size;
	try {
		primaryCogwheel->setSvg(visual_assets::loadPluginSvgCached("res/icon/cogwheel_amythyst.svg"));
	}
	catch (const std::exception& e) {
		WARN("Failed to load cogwheel-backed gear knob SVG: %s", e.what());
		primaryCogwheel->setSvg(nullptr);
	}
	try {
		secondaryCogwheel->setSvg(visual_assets::loadPluginSvgCached("res/icon/cogwheel_grandidierite.svg"));
	}
	catch (const std::exception& e) {
		WARN("Failed to load secondary cogwheel-backed gear knob SVG: %s", e.what());
		secondaryCogwheel->setSvg(nullptr);
	}
	updateCogwheelGeometry();
	fb->addChildBelow(primaryCogwheel, tw);
	fb->addChildBelow(secondaryCogwheel, tw);
}

void ClockworkGearKnob::draw(const DrawArgs& args) {
	GearKnobInvertSized::draw(args);
}

void ClockworkGearKnob::onChange(const ChangeEvent& e) {
	GearKnobInvertSized::onChange(e);
	updateCogwheelGeometry();
	if (fb) {
		fb->setDirty();
	}
}

void ClockworkGearKnob::updateCogwheelGeometry() {
	if (!primaryCogwheel || !secondaryCogwheel) return;
	const float primaryDiameterPx = 17.f;
	const float secondaryDiameterPx = primaryDiameterPx * 0.5f;
	const float valueNorm = normalizedParamValue();
	const float knobAngle = crossfade(minAngle, maxAngle, valueNorm);
	const Vec cogwheelOffset(0.f, -1.25f);
	const Vec primaryPos = box.size.mult(0.5f).plus(cogwheelOffset);
	const Vec primaryCenter = primaryPos.plus(Vec(0.5f * primaryDiameterPx, 0.5f * primaryDiameterPx));

	primaryCogwheel->center = primaryCenter;
	primaryCogwheel->diameterPx = primaryDiameterPx;
	primaryCogwheel->angleRad = -knobAngle;

	const float centerDistancePx = 0.5f * (primaryDiameterPx + secondaryDiameterPx) - 0.45f;
	const float secondaryCenterPhaseOffsetRad = -0.10f;
	const float secondaryCenterCos = std::cos(secondaryCenterPhaseOffsetRad);
	const float secondaryCenterSin = std::sin(secondaryCenterPhaseOffsetRad);
	const Vec secondaryDirection(-0.9636305f, 0.2672384f);
	const Vec secondaryCenter = primaryCenter.plus(Vec(
		(secondaryDirection.x * secondaryCenterCos - secondaryDirection.y * secondaryCenterSin) * centerDistancePx,
		(secondaryDirection.x * secondaryCenterSin + secondaryDirection.y * secondaryCenterCos) * centerDistancePx));
	const float secondaryGearRatio = primaryDiameterPx / secondaryDiameterPx;
	const float secondaryToothPhaseOffsetRad = secondaryCenterPhaseOffsetRad * (1.f + secondaryGearRatio);
	secondaryCogwheel->center = secondaryCenter;
	secondaryCogwheel->diameterPx = secondaryDiameterPx;
	secondaryCogwheel->angleRad = knobAngle * secondaryGearRatio + secondaryToothPhaseOffsetRad;
}
