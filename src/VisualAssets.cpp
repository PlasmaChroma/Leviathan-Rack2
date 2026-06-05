#include "VisualAssets.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>

namespace visual_assets {

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

} // namespace visual_assets

namespace {

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

} // namespace

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
			nvgRGBA(255, 218, 42, 248),
			nvgRGBA(255, 250, 205, 255));
		nvgStrokePaint(args.vg, activePaint);
		nvgStrokeWidth(args.vg, activeRingWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);
	}

	nvgBeginPath(args.vg);
	nvgArc(args.vg, center.x, center.y, ringRadius - 0.5f * ringWidth, startAngle, endAngle, NVG_CW);
	nvgStrokeColor(args.vg, nvgRGBA(255, 244, 154, 80));
	if (innerLineWidthSourcePx > 0.f) {
		nvgStrokeWidth(args.vg, innerLineWidthSourcePx * assetScale);
		nvgStroke(args.vg);
	}

	nvgRestore(args.vg);
}

GearKnobInvertSized::GearKnobInvertSized() {
	minAngle = -0.83 * M_PI;
	maxAngle = 0.83 * M_PI;

	setCachedSvg(visual_assets::loadPluginSvgCached("res/icon/gear_knob_invert.svg"));
	if (shadow) {
		shadow->opacity = 0.f;
	}
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

void GearKnobInvertSized::onChange(const ChangeEvent& e) {
	app::SvgKnob::onChange(e);
	if (activeRing) {
		activeRing->valueNorm = normalizedParamValue();
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
	if (isDragonKingDebugEnabled()) {
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
	if (isDragonKingDebugEnabled() && dragLogGestureId != 0) {
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
	DragMoveEvent clampedEvent = e;
	const float deltaLen = clampedEvent.mouseDelta.norm();
	const float maxDeltaPx = dragMoveFrame == 0 ? 12.f : 48.f;
	// Temporarily disabled for Rack drag-delta diagnostics so the original jump remains observable.
	if (false && deltaLen > maxDeltaPx && deltaLen > 1e-6f) {
		clampedEvent.mouseDelta = clampedEvent.mouseDelta.mult(maxDeltaPx / deltaLen);
	}
	const bool clamped = clampedEvent.mouseDelta.x != e.mouseDelta.x || clampedEvent.mouseDelta.y != e.mouseDelta.y;
	const bool logMove = isDragonKingDebugEnabled() && dragLogGestureId != 0 && (dragMoveFrame < 8 || clamped);
	const float valueBefore = logMove ? clockworkParamValue(this) : NAN;
	dragMoveFrame++;
	app::SvgKnob::onDragMove(clampedEvent);
	if (logMove) {
		const float valueAfter = clockworkParamValue(this);
		clockworkDragDebugRecorder().log("move", this, dragLogGestureId, dragMoveFrame - 1, e.mouseDelta, clampedEvent.mouseDelta, maxDeltaPx, clamped, valueBefore, valueAfter);
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
	const float radiusPx = diameterPx * (41.f / 120.f);
	const float strokeWidthPx = std::max(1.35f, diameterPx * (5.8f / 120.f));
	const float inactiveStrokeWidthPx = std::max(0.95f, strokeWidthPx * 0.84f);
	const float inactiveRadiusPx = radiusPx - 0.5f * (inactiveStrokeWidthPx - strokeWidthPx * 0.72f);
	const float activeStrokeWidthPx = std::max(strokeWidthPx, diameterPx * (7.2f / 120.f));
	const float activeRadiusPx = radiusPx - 0.5f * (activeStrokeWidthPx - strokeWidthPx);
	const float startNorm = bipolar ? centerNorm : 0.f;
	const float startAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(startNorm, 0.f, 1.f));
	const float endAngle = -0.5f * M_PI + crossfade(minAngle, maxAngle, clamp(valueNorm, 0.f, 1.f));
	const float direction = (endAngle >= startAngle) ? 1.f : -1.f;
	const float sweep = std::fabs(endAngle - startAngle);
	const float dashAngle = 0.11f;
	const float gapAngle = 0.18f;

	nvgSave(args.vg);
	nvgLineCap(args.vg, NVG_ROUND);

	const float ringMinAngle = -0.5f * float(M_PI) + minAngle;
	const float ringMaxAngle = -0.5f * float(M_PI) + maxAngle;
	for (float a = ringMinAngle; a < ringMaxAngle; a += dashAngle + gapAngle) {
		const float b = std::min(a + dashAngle, ringMaxAngle);
		nvgBeginPath(args.vg);
		nvgArc(args.vg, center.x, center.y, inactiveRadiusPx, a, b, NVG_CW);
		nvgStrokeColor(args.vg, nvgRGBA(92, 67, 8, 100));
		nvgStrokeWidth(args.vg, inactiveStrokeWidthPx);
		nvgStroke(args.vg);
	}

	if (sweep > 0.008f) {
		const NVGcolor activeColor = nvgRGBA(255, 230, 128, 245);
		for (float covered = 0.f; covered < sweep; covered += dashAngle + gapAngle) {
			const float a0 = startAngle + direction * covered;
			const float a1 = startAngle + direction * std::min(covered + dashAngle, sweep);
			nvgBeginPath(args.vg);
			nvgArc(args.vg,
				center.x,
				center.y,
				activeRadiusPx,
				std::min(a0, a1),
				std::max(a0, a1),
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
