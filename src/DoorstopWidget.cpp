#include "Doorstop.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/VisualAssets.hpp"

#include <widget/FramebufferWidget.hpp>

#include <array>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>

namespace {

constexpr float OVERFLOW_PAD = 42.f;
constexpr int SPRING_POINTS = 257;
constexpr float SPRING_BASE_Y_MM = 71.f;
constexpr float SPRING_LENGTH_MM = 49.f;
constexpr float SPRING_TURNS = 43.f;
constexpr std::uint32_t PANEL_CACHE_STABLE_FRAMES = 3u;

struct DoorstopVisualSnapshot {
	float displacement = 0.f;
	float velocity = 0.f;
	float energy = 0.f;
	float strike = 0.f;
};

struct SpringPathGeometry {
	std::array<Vec, SPRING_POINTS> points {};
	float tipTravel = 0.f;
	float tipAngle = 0.f;
};

enum class SpringPathClipSide {
	None,
	Left,
	Right
};

struct SpringPathClip {
	SpringPathClipSide side = SpringPathClipSide::None;
	float boundaryX = 0.f;

	SpringPathClip() = default;
	SpringPathClip(SpringPathClipSide side, float boundaryX)
		: side(side), boundaryX(boundaryX) {
	}
};

struct DoorstopRenderMetrics {
	debug_terminal::UiTimingRangeAccumulator geometryIdleUs;
	debug_terminal::UiTimingRangeAccumulator geometryTrailUs;
	debug_terminal::UiTimingRangeAccumulator panelIdleUs;
	debug_terminal::UiTimingRangeAccumulator panelTrailUs;
	debug_terminal::UiTimingRangeAccumulator overflowIdleUs;
	debug_terminal::UiTimingRangeAccumulator overflowTrailUs;
};

struct DoorstopWidget;
struct DoorstopOverflowWidget;

struct DoorstopOverlayLink {
	DoorstopWidget* owner = nullptr;
	DoorstopOverflowWidget* overlay = nullptr;
	Vec springBase;
	DoorstopVisualSnapshot snapshot;
	std::array<float, 3> displacementHistory {};
	std::array<SpringPathGeometry, 4> springGeometry {};
	bool trailGeometryValid = false;
	DoorstopRenderMetrics debugRenderMetrics;
	float lastGeometryUs = 0.f;
	float lastPanelDrawUs = 0.f;
	float lastOverflowDrawUs = 0.f;
	std::uint64_t geometrySequence = 0u;
	std::uint64_t panelDrawSequence = 0u;
	std::uint64_t overflowDrawSequence = 0u;
	bool overflowLeftVisible = false;
	bool overflowRightVisible = false;
};

float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

float visualTipTravel(float displacement) {
	return 48.f * std::tanh(displacement * 0.75f);
}

float bendProfile(float t) {
	return t * t * (3.f - 2.f * t);
}

struct SpringPointTemplate {
	float bend = 0.f;
	float bendDerivative = 0.f;
	float y = 0.f;
	float coilOffset = 0.f;
};

const std::array<SpringPointTemplate, SPRING_POINTS>& springPointTemplates() {
	static const std::array<SpringPointTemplate, SPRING_POINTS> templates = []() {
		std::array<SpringPointTemplate, SPRING_POINTS> result {};
		const float springLength = mm2px(SPRING_LENGTH_MM);
		for (int i = 0; i < SPRING_POINTS; ++i) {
			const float t = float(i) / float(SPRING_POINTS - 1);
			SpringPointTemplate& point = result[i];
			point.bend = bendProfile(t);
			point.bendDerivative = 6.f * t * (1.f - t);
			point.y = -springLength * t;
			const float coilRadius = 4.4f + (3.15f - 4.4f) * point.bend;
			point.coilOffset = coilRadius
				* std::sin(2.f * float(M_PI) * SPRING_TURNS * t);
		}
		return result;
	}();
	return templates;
}

void buildSpringGeometry(SpringPathGeometry& geometry, float displacement) {
	const float travel = visualTipTravel(displacement);
	const float springLength = mm2px(SPRING_LENGTH_MM);
	const auto& templates = springPointTemplates();
	for (int i = 0; i < SPRING_POINTS; ++i) {
		const SpringPointTemplate& point = templates[i];
		const float centerX = travel * point.bend;
		const float dxdt = travel * point.bendDerivative;
		const float dydt = -springLength;
		const float invLength = 1.f / std::max(std::sqrt(dxdt * dxdt + dydt * dydt), 1e-6f);
		const Vec normal(-dydt * invLength, dxdt * invLength);
		geometry.points[i] = Vec(centerX, point.y).plus(normal.mult(point.coilOffset));
	}
	geometry.tipTravel = travel;
	const float tangentT = 0.90f;
	const float dxdt = travel * 6.f * tangentT * (1.f - tangentT);
	geometry.tipAngle = std::atan2(-springLength, dxdt) + 0.5f * float(M_PI);
}

struct HorizontalBounds {
	float minimum = INFINITY;
	float maximum = -INFINITY;

	void include(float x, float radius = 0.f) {
		minimum = std::min(minimum, x - radius);
		maximum = std::max(maximum, x + radius);
	}
};

void includePathBounds(
	HorizontalBounds& bounds,
	const SpringPathGeometry& geometry,
	float xOffset,
	float strokeRadius) {
	for (const Vec& point : geometry.points) {
		bounds.include(point.x + xOffset, strokeRadius);
	}
}

void includeRotatedRectBounds(
	HorizontalBounds& bounds,
	float originX,
	float angle,
	float centerX,
	float halfWidth,
	float halfHeight,
	float extraRadius = 0.f) {
	const float cosine = std::cos(angle);
	const float sine = std::sin(angle);
	const float rotatedCenterX = originX + cosine * centerX;
	const float extentX = std::fabs(cosine) * halfWidth
		+ std::fabs(sine) * halfHeight + extraRadius;
	bounds.include(rotatedCenterX, extentX);
}

void updateOverflowVisibility(DoorstopOverlayLink& link, float panelWidth) {
	HorizontalBounds bounds;

	// The current spring is stroked three times with distinct offsets.
	includePathBounds(bounds, link.springGeometry[0], 1.3f, 2.3f);
	includePathBounds(bounds, link.springGeometry[0], 0.f, 1.55f);
	includePathBounds(bounds, link.springGeometry[0], -0.7f, 0.425f);

	if (link.trailGeometryValid) {
		for (int i = 1; i < int(link.springGeometry.size()); ++i) {
			includePathBounds(bounds, link.springGeometry[i], 0.f, 1.6f);
		}
	}

	const DoorstopVisualSnapshot& state = link.snapshot;
	const SpringPathGeometry& current = link.springGeometry[0];
	includeRotatedRectBounds(
		bounds, current.tipTravel, current.tipAngle, 0.f, 5.4f, 8.5f, 0.625f);
	if (std::fabs(state.velocity) > 0.55f) {
		includeRotatedRectBounds(
			bounds,
			current.tipTravel,
			current.tipAngle,
			-state.velocity * 4.f,
			5.2f,
			8.5f);
	}
	if (state.strike > 0.002f) {
		bounds.include(0.f, 15.f + clamp01(state.strike) * 10.f);
	}

	link.overflowLeftVisible = bounds.minimum < -link.springBase.x;
	link.overflowRightVisible = bounds.maximum > panelWidth - link.springBase.x;
}

void appendFullSpringPath(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY) {
	nvgBeginPath(vg);
	nvgMoveTo(vg, baseX + geometry.points[0].x, baseY + geometry.points[0].y);
	for (int i = 1; i < SPRING_POINTS; ++i) {
		nvgLineTo(vg, baseX + geometry.points[i].x, baseY + geometry.points[i].y);
	}
}

bool pointInsideClip(float x, const SpringPathClip& clip) {
	if (clip.side == SpringPathClipSide::Left) {
		return x <= clip.boundaryX;
	}
	if (clip.side == SpringPathClipSide::Right) {
		return x >= clip.boundaryX;
	}
	return true;
}

void appendClippedSpringPath(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY, const SpringPathClip& clip) {
	if (clip.side == SpringPathClipSide::None) {
		appendFullSpringPath(vg, geometry, baseX, baseY);
		return;
	}

	nvgBeginPath(vg);
	bool pathOpen = false;
	for (int i = 1; i < SPRING_POINTS; ++i) {
		const Vec a = geometry.points[i - 1].plus(Vec(baseX, baseY));
		const Vec b = geometry.points[i].plus(Vec(baseX, baseY));
		const bool aInside = pointInsideClip(a.x, clip);
		const bool bInside = pointInsideClip(b.x, clip);
		if (aInside && bInside) {
			if (!pathOpen) {
				nvgMoveTo(vg, a.x, a.y);
			}
			nvgLineTo(vg, b.x, b.y);
			pathOpen = true;
			continue;
		}
		if (aInside == bInside) {
			pathOpen = false;
			continue;
		}

		const float dx = b.x - a.x;
		const float t = std::fabs(dx) > 1e-6f
			? clamp((clip.boundaryX - a.x) / dx, 0.f, 1.f)
			: 0.f;
		const Vec crossing = a.plus(b.minus(a).mult(t));
		if (aInside) {
			if (!pathOpen) {
				nvgMoveTo(vg, a.x, a.y);
			}
			nvgLineTo(vg, crossing.x, crossing.y);
			pathOpen = false;
		}
		else {
			nvgMoveTo(vg, crossing.x, crossing.y);
			nvgLineTo(vg, b.x, b.y);
			pathOpen = true;
		}
	}
}

void drawSpringBody(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY,
	float velocity, float alpha, bool drawCap, const SpringPathClip& clip = {}) {
	alpha = clamp01(alpha);
	appendClippedSpringPath(vg, geometry, baseX + 1.3f, baseY + 1.8f, clip);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, int(190.f * alpha)));
	nvgStrokeWidth(vg, 4.6f);
	nvgLineCap(vg, NVG_ROUND);
	nvgLineJoin(vg, NVG_ROUND);
	nvgStroke(vg);

	appendClippedSpringPath(vg, geometry, baseX, baseY, clip);
	NVGpaint metal = nvgLinearGradient(vg, baseX - 4.f, 0.f, baseX + 6.f, 0.f,
		nvgRGBA(78, 92, 105, int(255.f * alpha)),
		nvgRGBA(224, 238, 241, int(255.f * alpha)));
	nvgStrokePaint(vg, metal);
	nvgStrokeWidth(vg, 3.1f);
	nvgLineCap(vg, NVG_ROUND);
	nvgStroke(vg);

	appendClippedSpringPath(vg, geometry, baseX - 0.7f, baseY, clip);
	nvgStrokeColor(vg, nvgRGBA(206, 252, 255, int(130.f * alpha)));
	nvgStrokeWidth(vg, 0.85f);
	nvgStroke(vg);

	if (!drawCap) {
		return;
	}
	const float tipX = baseX + geometry.tipTravel;
	const float tipY = baseY - mm2px(SPRING_LENGTH_MM);
	const float motion = clamp01(std::fabs(velocity));

	nvgSave(vg);
	nvgTranslate(vg, tipX, tipY);
	nvgRotate(vg, geometry.tipAngle);
	if (motion > 0.55f) {
		nvgBeginPath(vg);
		nvgRoundedRect(vg, -5.2f - velocity * 4.f, -8.5f, 10.4f, 17.f, 4.8f);
		nvgFillColor(vg, nvgRGBA(94, 52, 132, int(65.f * alpha * motion)));
		nvgFill(vg);
	}
	nvgBeginPath(vg);
	nvgRoundedRect(vg, -5.4f, -8.5f, 10.8f, 17.f, 4.8f);
	NVGpaint rubber = nvgLinearGradient(vg, -5.f, -7.f, 5.f, 7.f,
		nvgRGBA(35, 30, 43, int(255.f * alpha)),
		nvgRGBA(111, 73, 137, int(255.f * alpha)));
	nvgFillPaint(vg, rubber);
	nvgFill(vg);
	nvgStrokeColor(vg, nvgRGBA(5, 7, 10, int(230.f * alpha)));
	nvgStrokeWidth(vg, 1.25f);
	nvgStroke(vg);
	nvgBeginPath(vg);
	nvgRoundedRect(vg, -2.9f, -6.2f, 2.2f, 9.5f, 1.1f);
	nvgFillColor(vg, nvgRGBA(255, 255, 255, int(80.f * alpha)));
	nvgFill(vg);
	nvgRestore(vg);
}

void drawSpringTrail(NVGcontext* vg, const SpringPathGeometry& geometry,
	float baseX, float baseY, float alpha, const SpringPathClip& clip = {}) {
	alpha = clamp01(alpha);
	appendClippedSpringPath(vg, geometry, baseX, baseY, clip);
	const NVGpaint trailMetal = nvgLinearGradient(vg, baseX - 4.f, 0.f, baseX + 6.f, 0.f,
		nvgRGBA(72, 86, 102, int(220.f * alpha)),
		nvgRGBA(213, 239, 245, int(245.f * alpha)));
	nvgStrokePaint(vg, trailMetal);
	nvgStrokeWidth(vg, 3.2f);
	nvgLineCap(vg, NVG_ROUND);
	nvgLineJoin(vg, NVG_ROUND);
	nvgStroke(vg);
}

void drawStrikeAccent(NVGcontext* vg, float baseX, float baseY, float strike) {
	strike = clamp01(strike);
	if (strike <= 0.002f) {
		return;
	}
	nvgBeginPath(vg);
	nvgCircle(vg, baseX, baseY, 15.f + strike * 10.f);
	NVGpaint glow = nvgRadialGradient(vg, baseX, baseY, 2.f, 23.f,
		nvgRGBA(164, 98, 255, int(100.f * strike)),
		nvgRGBA(28, 204, 217, 0));
	nvgFillPaint(vg, glow);
	nvgFill(vg);
}

void drawSpringScene(NVGcontext* vg, const DoorstopOverlayLink& link,
	float baseX, float baseY, const SpringPathClip& clip = {}) {
	const DoorstopVisualSnapshot& state = link.snapshot;
	const float trailAmount = clamp01((state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f);
	if (trailAmount > 0.01f && link.trailGeometryValid) {
		for (int i = 2; i >= 0; --i) {
			const float age = float(i + 1) / 4.f;
			drawSpringTrail(vg, link.springGeometry[i + 1], baseX, baseY,
				trailAmount * (0.17f - age * 0.08f), clip);
		}
	}
	drawStrikeAccent(vg, baseX, baseY, state.strike);
	drawSpringBody(vg, link.springGeometry[0], baseX, baseY, state.velocity, 1.f, true, clip);
}

void drawEnergyMeter(NVGcontext* vg, const math::Rect& bounds, float energy) {
	energy = clamp01(energy);
	// Preserve the engine's physical 0..1 energy scale while expanding the
	// visually useful low end. The endpoints remain exact: silence is empty
	// and the configured energy ceiling fills the meter.
	constexpr float displayCurve = 63.f;
	const float displayEnergy =
		std::log1p(displayCurve * energy) / std::log1p(displayCurve);
	const float radius = std::min(0.5f * bounds.size.y, 2.5f);
	nvgBeginPath(vg);
	nvgRoundedRect(vg, bounds.pos.x, bounds.pos.y, bounds.size.x, bounds.size.y, radius);
	nvgFillColor(vg, nvgRGB(7, 10, 15));
	nvgFill(vg);
	nvgStrokeWidth(vg, 1.f);
	nvgStrokeColor(vg, nvgRGBA(174, 132, 255, 96));
	nvgStroke(vg);

	const float inset = 1.25f;
	const Vec fillPos = bounds.pos.plus(Vec(inset, inset));
	const Vec fillSize = bounds.size.minus(Vec(2.f * inset, 2.f * inset));
	const float fillWidth = std::max(0.f, fillSize.x * displayEnergy);
	if (fillWidth > 0.5f && fillSize.y > 0.f) {
		nvgSave(vg);
		nvgIntersectScissor(vg, fillPos.x, fillPos.y, fillWidth, fillSize.y);
		nvgBeginPath(vg);
		nvgRoundedRect(vg, fillPos.x, fillPos.y, fillSize.x, fillSize.y,
			std::max(0.f, 0.5f * fillSize.y));
		const NVGpaint fill = nvgLinearGradient(vg,
			fillPos.x, fillPos.y, fillPos.x + fillSize.x, fillPos.y,
			nvgRGB(122, 92, 255), nvgRGB(28, 204, 217));
		nvgFillPaint(vg, fill);
		nvgFill(vg);
		nvgRestore(vg);
	}
	else if (fillWidth > 0.f && fillSize.y > 0.f) {
		// Once the remaining energy is narrower than a useful filled pixel,
		// preserve its long tail as a tiny glow whose opacity reaches zero
		// continuously. This avoids presenting a fixed minimum bar width.
		const float tailAlpha = std::sqrt(clamp01(fillWidth / 0.5f));
		const float glowRadius = std::min(2.75f, 0.5f * fillSize.y);
		const Vec glowCenter(
			fillPos.x + 0.75f,
			fillPos.y + 0.5f * fillSize.y);
		nvgSave(vg);
		nvgIntersectScissor(vg, fillPos.x, fillPos.y, fillSize.x, fillSize.y);
		nvgBeginPath(vg);
		nvgCircle(vg, glowCenter.x, glowCenter.y, glowRadius);
		const NVGpaint tailGlow = nvgRadialGradient(vg,
			glowCenter.x, glowCenter.y, 0.f, glowRadius,
			nvgRGBA(122, 92, 255, int(110.f * tailAlpha)),
			nvgRGBA(28, 204, 217, 0));
		nvgFillPaint(vg, tailGlow);
		nvgFill(vg);
		nvgRestore(vg);
	}

	for (int tick = 1; tick < 4; ++tick) {
		const float x = bounds.pos.x + bounds.size.x * (float(tick) * 0.25f);
		nvgBeginPath(vg);
		nvgMoveTo(vg, x, bounds.pos.y + 1.f);
		nvgLineTo(vg, x, bounds.pos.y + bounds.size.y - 1.f);
		nvgStrokeWidth(vg, 0.75f);
		nvgStrokeColor(vg, nvgRGBA(230, 240, 248, 70));
		nvgStroke(vg);
	}
}

class DoorstopHitWidget final : public app::Switch {
public:
	Doorstop* doorstopModule = nullptr;

	DoorstopHitWidget() {
		momentary = true;
	}

	void onButton(const event::Button& e) override {
		// Only left-click belongs to the invisible strike control. In particular,
		// leave right-click unconsumed so DoorstopWidget can open its module menu.
		if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
			return;
		}
		if (doorstopModule && e.action == GLFW_PRESS) {
			const float normalizedY = box.size.y > 0.f
				? clamp(e.pos.y / box.size.y, 0.f, 1.f)
				: 0.5f;
			const float velocity = doorstop::Engine::manualVelocityFromVerticalPosition(
				normalizedY);
			doorstopModule->pendingManualVelocity.store(velocity, std::memory_order_relaxed);
			doorstopModule->manualVelocityPending.store(true, std::memory_order_release);
		}
		app::Switch::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		(void) args;
	}
};

class DoorstopSpringWidget final : public TransparentWidget {
public:
	std::shared_ptr<DoorstopOverlayLink> link;
	math::Rect energyMeterRect;

	void draw(const DrawArgs& args) override {
		if (!link) {
			return;
		}
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto sceneStart = debug_terminal::debugTimerStart(measurePerf);
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		drawSpringScene(args.vg, *link, link->springBase.x, link->springBase.y);
		drawEnergyMeter(args.vg, energyMeterRect, link->snapshot.energy);
		nvgRestore(args.vg);
		if (measurePerf) {
			const float elapsedUs = debug_terminal::elapsedUsSince(sceneStart);
			auto& range = link->trailGeometryValid
				? link->debugRenderMetrics.panelTrailUs
				: link->debugRenderMetrics.panelIdleUs;
			range.add(elapsedUs);
			link->lastPanelDrawUs = elapsedUs;
			++link->panelDrawSequence;
		}
	}

};

struct DoorstopOverflowWidget final : TransparentWidget {
	std::shared_ptr<DoorstopOverlayLink> link;

	~DoorstopOverflowWidget() override {
		if (link && link->overlay == this) {
			link->overlay = nullptr;
		}
	}

	void step() override;
	void draw(const DrawArgs& args) override;
	void drawLayer(const DrawArgs& args, int layer) override;
	void drawOverflowScene(const DrawArgs& args);
};

struct DoorstopWidget final : ModuleWidget {
	struct RenderingLog {
		std::ofstream file;
		std::string path;
		bool active = false;
		std::uint64_t row = 0u;
		double startedAtSec = 0.0;
		double previousRowAtSec = 0.0;
	};

	debug_terminal::BaselineWidgetMetrics debugWidgetMetrics;
	std::shared_ptr<DoorstopOverlayLink> overlayLink;
	widget::FramebufferWidget* springFramebuffer = nullptr;
	DoorstopSpringWidget* springWidget = nullptr;
	RenderingLog renderingLog;
	float lastStepUs = 0.f;
	std::uint64_t stepSequence = 0u;
	std::uint32_t panelStableFrames = 0u;

	static std::string renderingLogRootPath() {
		return system::join(asset::user(), "Leviathan/Doorstop");
	}

	static std::string renderingLogDateTimeStamp() {
		std::time_t now = std::time(nullptr);
		std::tm localTime {};
#if defined(_WIN32)
		localtime_s(&localTime, &now);
#else
		localtime_r(&now, &localTime);
#endif
		std::ostringstream stamp;
		stamp << std::put_time(&localTime, "%Y%m%d_%H%M%S");
		return stamp.str();
	}

	void startRenderingLog(Doorstop* m) {
		if (!m || renderingLog.active || !isDragonKingDebugEnabled()) {
			return;
		}
		const std::string root = renderingLogRootPath();
		if (!system::createDirectories(root) && !system::isDirectory(root)) {
			WARN("Doorstop failed to create rendering log directory: %s", root.c_str());
			return;
		}
		static std::uint32_t openSequence = 0u;
		renderingLog.path = system::join(
			root,
			"rendering_" + std::to_string(m->debugMetrics.instanceId) + "_"
				+ renderingLogDateTimeStamp() + "_" + std::to_string(openSequence++) + ".csv");
		renderingLog.file.open(renderingLog.path.c_str(), std::ios::out | std::ios::trunc);
		if (!renderingLog.file.is_open()) {
			WARN("Doorstop failed to open rendering log CSV: %s", renderingLog.path.c_str());
			renderingLog.path.clear();
			return;
		}
		renderingLog.file << std::fixed << std::setprecision(6);
		renderingLog.file
			<< "row,elapsed_sec,frame_interval_ms,module_id,instance_id,"
			<< "step_sequence,geometry_sequence,panel_draw_sequence,overflow_draw_sequence,"
			<< "panel_framebuffer_bypassed,panel_stable_frames,"
			<< "overflow_left_visible,overflow_right_visible,"
			<< "displacement,velocity,energy,strike,tip_travel_px,trail_amount,trails_active,"
			<< "history_1,history_2,history_3,"
			<< "engine_mode,sound_model,last_strike_model,break_in,"
			<< "overflow_allowed,overflow_present,panel_width_px,panel_height_px,"
			<< "step_us,geometry_us,panel_draw_us,overflow_draw_us,module_draw_us\n";
		renderingLog.active = true;
		renderingLog.row = 0u;
		renderingLog.startedAtSec = system::getTime();
		renderingLog.previousRowAtSec = renderingLog.startedAtSec;
		INFO("Doorstop started rendering log: %s", renderingLog.path.c_str());
	}

	void stopRenderingLog() {
		if (renderingLog.file.is_open()) {
			renderingLog.file.flush();
			renderingLog.file.close();
		}
		if (renderingLog.active) {
			INFO("Doorstop stopped rendering log: %s", renderingLog.path.c_str());
		}
		renderingLog.active = false;
		renderingLog.row = 0u;
		renderingLog.startedAtSec = 0.0;
		renderingLog.previousRowAtSec = 0.0;
	}

	void writeRenderingLogRow(Doorstop* m, float moduleDrawUs) {
		if (!m || !overlayLink || !renderingLog.active || !renderingLog.file.is_open()) {
			return;
		}
		const double nowSec = system::getTime();
		const DoorstopVisualSnapshot& state = overlayLink->snapshot;
		const float trailAmount = clamp01(
			(state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f);
		renderingLog.file
			<< renderingLog.row++ << ","
			<< (nowSec - renderingLog.startedAtSec) << ","
			<< ((nowSec - renderingLog.previousRowAtSec) * 1000.0) << ","
			<< m->id << ","
			<< m->debugMetrics.instanceId << ","
			<< stepSequence << ","
			<< overlayLink->geometrySequence << ","
			<< overlayLink->panelDrawSequence << ","
			<< overlayLink->overflowDrawSequence << ","
			<< (springFramebuffer && springFramebuffer->bypassed ? 1 : 0) << ","
			<< panelStableFrames << ","
			<< (overlayLink->overflowLeftVisible ? 1 : 0) << ","
			<< (overlayLink->overflowRightVisible ? 1 : 0) << ","
			<< state.displacement << ","
			<< state.velocity << ","
			<< state.energy << ","
			<< state.strike << ","
			<< overlayLink->springGeometry[0].tipTravel << ","
			<< trailAmount << ","
			<< (overlayLink->trailGeometryValid ? 1 : 0) << ","
			<< overlayLink->displacementHistory[0] << ","
			<< overlayLink->displacementHistory[1] << ","
			<< overlayLink->displacementHistory[2] << ","
			<< m->engineMode.load(std::memory_order_relaxed) << ","
			<< m->soundModel.load(std::memory_order_relaxed) << ","
			<< m->visualLastStrikeModel.load(std::memory_order_relaxed) << ","
			<< m->serializedBreakIn.load(std::memory_order_relaxed) << ","
			<< (m->allowVisualOverflow.load(std::memory_order_relaxed) ? 1 : 0) << ","
			<< (overlayLink->overlay ? 1 : 0) << ","
			<< box.size.x << ","
			<< box.size.y << ","
			<< lastStepUs << ","
			<< overlayLink->lastGeometryUs << ","
			<< overlayLink->lastPanelDrawUs << ","
			<< overlayLink->lastOverflowDrawUs << ","
			<< moduleDrawUs << "\n";
		renderingLog.previousRowAtSec = nowSec;
		if ((renderingLog.row % 60u) == 0u) {
			renderingLog.file.flush();
		}
	}

	explicit DoorstopWidget(Doorstop* module) {
		setModule(module);
		overlayLink = std::make_shared<DoorstopOverlayLink>();
		overlayLink->owner = this;
		buildSpringGeometry(overlayLink->springGeometry[0], 0.f);

		PreviewBuildLogTimer previewBuildTimer("Doorstop", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/doorstop.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		visual_assets::addFractalGlassOverlay(
			this, panelPath, splitPanel.panelSurfaceEffectWidget());
		splitPanel.addLabels("res/doorstop.labels.svg");
		splitPanel.addPerfectWaveSoloBranding();
		previewBuildTimer.markPanelDone();

		auto anchorPoint = [&](const char* id, const Vec& fallbackMm) {
			Vec result;
			if (!panel_svg::loadPointFromSvgMm(panelPath, id, &result)) {
				result = fallbackMm;
			}
			return result;
		};

		overlayLink->springBase = mm2px(
			anchorPoint("SPRING_BASE", Vec(7.62f, SPRING_BASE_Y_MM)));

		math::Rect energyMeterMm(Vec(2.2f, 79.3f), Vec(10.84f, 2.4f));
		panel_svg::loadRectFromSvgMm(panelPath, "ENERGY_METER", &energyMeterMm);
		springFramebuffer = new widget::FramebufferWidget();
		springFramebuffer->box.size = box.size;
		springFramebuffer->bypassed = true;
		springWidget = new DoorstopSpringWidget();
		springWidget->box.size = box.size;
		springWidget->link = overlayLink;
		springWidget->energyMeterRect =
			math::Rect(mm2px(energyMeterMm.pos), mm2px(energyMeterMm.size));
		springFramebuffer->addChild(springWidget);
		addChild(springFramebuffer);

		math::Rect hitRectMm(Vec(1.1f, 17.3f), Vec(13.04f, 53.8f));
		panel_svg::loadRectFromSvgMm(panelPath, "MANUAL_HIT_REGION", &hitRectMm);
		auto* hit = createParam<DoorstopHitWidget>(mm2px(hitRectMm.pos), module, Doorstop::MANUAL_PARAM);
		hit->box.size = mm2px(hitRectMm.size);
		hit->doorstopModule = module;
		addParam(hit);

		const Vec trigMm = anchorPoint("TRIG_INPUT", Vec(7.62f, 93.f));
		const Vec velocityMm = anchorPoint("VELOCITY_INPUT", Vec(7.62f, 107.f));
		const Vec outputMm = anchorPoint("AUDIO_OUTPUT", Vec(7.62f, 121.f));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(trigMm), module, Doorstop::TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(velocityMm), module, Doorstop::VELOCITY_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(outputMm), module, Doorstop::AUDIO_OUTPUT));

		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewBuildTimer.markAnchorsDone();
	}

	~DoorstopWidget() override {
		stopRenderingLog();
		if (!overlayLink) {
			return;
		}
		overlayLink->owner = nullptr;
		destroyOverflowWidget();
	}

	bool validRackContext() const {
		return module && APP && APP->scene && APP->scene->rack
			&& parent == APP->scene->rack->getModuleContainer();
	}

	Vec overflowPosition() {
		auto* rack = APP->scene->rack;
		return getRelativeOffset(Vec(), rack).minus(Vec(OVERFLOW_PAD, 0.f));
	}

	void createOverflowWidget() {
		if (!overlayLink || overlayLink->overlay || !validRackContext()) {
			return;
		}
		auto* m = static_cast<Doorstop*>(module);
		if (!m->allowVisualOverflow.load(std::memory_order_relaxed)) {
			return;
		}
		auto* overlay = new DoorstopOverflowWidget();
		overlay->link = overlayLink;
		overlay->box.pos = overflowPosition();
		overlay->box.size = Vec(box.size.x + 2.f * OVERFLOW_PAD, box.size.y);

		// RackWidget assumes that every child of moduleContainer is a ModuleWidget.
		// Keep this decorative widget as a direct RackWidget child, immediately below
		// the cable layer, so it can cross panel bounds without corrupting that invariant.
		auto* rack = APP->scene->rack;
		auto* cableContainer = rack->getCableContainer();
		if (!cableContainer || !rack->hasChild(cableContainer)) {
			delete overlay;
			return;
		}
		overlayLink->overlay = overlay;
		rack->addChildBelow(overlay, cableContainer);
	}

	void destroyOverflowWidget() {
		if (!overlayLink || !overlayLink->overlay) {
			return;
		}
		DoorstopOverflowWidget* overlay = overlayLink->overlay;
		overlayLink->overlay = nullptr;
		if (overlay->parent) {
			overlay->requestDelete();
		}
		else {
			delete overlay;
		}
	}

	void step() override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto stepStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::step();
		if (renderingLog.active && !measurePerf) {
			stopRenderingLog();
		}
		if (!overlayLink) {
			if (measurePerf) {
				lastStepUs = debug_terminal::elapsedUsSince(stepStart);
				debugWidgetMetrics.recordStep(lastStepUs);
				++stepSequence;
			}
			return;
		}
		DoorstopVisualSnapshot state;
		if (auto* m = static_cast<Doorstop*>(module)) {
			state.displacement = m->visualDisplacement.load(std::memory_order_relaxed);
			state.velocity = m->visualVelocity.load(std::memory_order_relaxed);
			state.energy = m->visualEnergy.load(std::memory_order_relaxed);
			state.strike = m->visualStrike.load(std::memory_order_relaxed);
		}
		const DoorstopVisualSnapshot previousState = overlayLink->snapshot;
		const std::array<float, 3> previousHistory = overlayLink->displacementHistory;
		const bool previousTrailsActive = overlayLink->trailGeometryValid;
		overlayLink->snapshot = state;
		for (int i = int(overlayLink->displacementHistory.size()) - 1; i > 0; --i) {
			overlayLink->displacementHistory[i] = overlayLink->displacementHistory[i - 1];
		}
		overlayLink->displacementHistory[0] = state.displacement;
		const bool trailsActive = clamp01(
			(state.energy - 0.10f) * 1.8f + std::fabs(state.velocity) * 0.45f) > 0.01f;
		const auto geometryStart = debug_terminal::debugTimerStart(measurePerf);
		buildSpringGeometry(overlayLink->springGeometry[0], state.displacement);
		overlayLink->trailGeometryValid = trailsActive;
		if (overlayLink->trailGeometryValid) {
			for (int i = 0; i < int(overlayLink->displacementHistory.size()); ++i) {
				buildSpringGeometry(
					overlayLink->springGeometry[i + 1],
					overlayLink->displacementHistory[i]);
			}
		}
		if (measurePerf) {
			const float geometryUs = debug_terminal::elapsedUsSince(geometryStart);
			auto& range = trailsActive
				? overlayLink->debugRenderMetrics.geometryTrailUs
				: overlayLink->debugRenderMetrics.geometryIdleUs;
			range.add(geometryUs);
			overlayLink->lastGeometryUs = geometryUs;
			++overlayLink->geometrySequence;
		}
		updateOverflowVisibility(*overlayLink, box.size.x);

		const bool visualStateChanged =
			state.displacement != previousState.displacement
			|| state.velocity != previousState.velocity
			|| state.energy != previousState.energy
			|| state.strike != previousState.strike
			|| trailsActive != previousTrailsActive
			|| (trailsActive && overlayLink->displacementHistory != previousHistory);
		if (springFramebuffer) {
			if (visualStateChanged) {
				panelStableFrames = 0u;
				springFramebuffer->bypassed = true;
				springFramebuffer->setDirty();
			}
			else {
				panelStableFrames = std::min(
					panelStableFrames + 1u, PANEL_CACHE_STABLE_FRAMES);
				if (panelStableFrames >= PANEL_CACHE_STABLE_FRAMES
					&& springFramebuffer->bypassed) {
					springFramebuffer->bypassed = false;
					springFramebuffer->setDirty();
				}
			}
		}

		auto* m = static_cast<Doorstop*>(module);
		const bool enabled = m && m->allowVisualOverflow.load(std::memory_order_relaxed);
		if (!enabled || !validRackContext()) {
			destroyOverflowWidget();
		}
		else if (!overlayLink->overlay) {
			createOverflowWidget();
		}
		if (measurePerf) {
			lastStepUs = debug_terminal::elapsedUsSince(stepStart);
			debugWidgetMetrics.recordStep(lastStepUs);
			++stepSequence;
		}
	}

	void draw(const DrawArgs& args) override {
		const bool measurePerf = isDragonKingDebugEnabled();
		const auto drawStart = debug_terminal::debugTimerStart(measurePerf);
		ModuleWidget::draw(args);
		auto* doorstop = static_cast<Doorstop*>(module);
		if (!doorstop) {
			return;
		}

		if (measurePerf) {
			debug_terminal::drawDebugInstanceId(args.vg, box.size, doorstop->debugMetrics.instanceId);
			const float moduleDrawUs = debug_terminal::elapsedUsSince(drawStart);
			debugWidgetMetrics.recordDraw(moduleDrawUs);
			writeRenderingLogRow(doorstop, moduleDrawUs);

			const double nowSec = system::getTime();
			if (debug_terminal::baselineSubmitDue("Doorstop", doorstop->debugMetrics.instanceId, nowSec)) {
				debug_terminal::submitDoorstopMetrics(
					doorstop->debugMetrics.instanceId,
					doorstop->debugMetrics.consumeProcessRange(),
					debugWidgetMetrics.consumeStepRange(),
					debugWidgetMetrics.consumeDrawRange(),
					overlayLink->debugRenderMetrics.geometryIdleUs.consume(),
					overlayLink->debugRenderMetrics.geometryTrailUs.consume(),
					overlayLink->debugRenderMetrics.panelIdleUs.consume(),
					overlayLink->debugRenderMetrics.panelTrailUs.consume(),
					overlayLink->debugRenderMetrics.overflowIdleUs.consume(),
					overlayLink->debugRenderMetrics.overflowTrailUs.consume(),
					overlayLink->trailGeometryValid);
			}
		}
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* m = dynamic_cast<Doorstop*>(module);
		if (!m) {
			return;
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Sound engine", "", [m](Menu* engineMenu) {
			engineMenu->addChild(createCheckMenuItem("Reference physical model", "",
				[m]() {
					return m->engineMode.load(std::memory_order_relaxed)
						== int(doorstop::EngineMode::ReferenceV1);
				},
				[m]() {
					m->engineMode.store(
						int(doorstop::EngineMode::ReferenceV1),
						std::memory_order_release);
				}));
			engineMenu->addChild(createSubmenuItem("Legacy models", "",
				[m](Menu* modelMenu) {
					auto addLegacyModel = [m, modelMenu](
						const char* label, doorstop::SoundModel model) {
						modelMenu->addChild(createCheckMenuItem(label, "",
							[m, model]() {
								return m->engineMode.load(std::memory_order_relaxed)
										== int(doorstop::EngineMode::Legacy)
									&& m->soundModel.load(std::memory_order_relaxed)
										== int(model);
							},
							[m, model]() {
								m->soundModel.store(int(model), std::memory_order_relaxed);
								m->engineMode.store(
									int(doorstop::EngineMode::Legacy),
									std::memory_order_release);
							}));
					};
					addLegacyModel(
						"Probabilistic mix", doorstop::SoundModel::ProbabilisticMix);
					modelMenu->addChild(new MenuSeparator());
					addLegacyModel("Classic modal", doorstop::SoundModel::Classic);
					addLegacyModel("Coupled body", doorstop::SoundModel::CoupledBody);
					addLegacyModel("Coil contact", doorstop::SoundModel::CoilContact);
					addLegacyModel(
						"Dispersive spring", doorstop::SoundModel::DispersiveSpring);
				}));
		}));
		const int breakInPercent = int(std::round(
			clamp01(m->serializedBreakIn.load(std::memory_order_relaxed)) * 100.f));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel(
			string::f("Break-in: %d%%", breakInPercent)));
		menu->addChild(createCheckMenuItem(
			"Lock break-in", "",
			[m]() {
				return m->breakInLocked.load(std::memory_order_relaxed);
			},
			[m]() {
				const bool locked =
					!m->breakInLocked.load(std::memory_order_relaxed);
				m->breakInLocked.store(locked, std::memory_order_relaxed);
			}));
		menu->addChild(createMenuItem(
			"Restore factory-fresh spring", "",
			[m]() {
				m->restoreSpringRequested.store(true, std::memory_order_release);
			}));
		menu->addChild(createMenuItem(
			"Generate new specimen", "",
			[m]() {
				std::uint32_t seed = random::u32();
				if (!seed) seed = 1u;
				m->pendingSpecimenSeed.store(seed, std::memory_order_relaxed);
				m->newSpecimenRequested.store(true, std::memory_order_release);
			}));
		menu->addChild(new MenuSeparator());
		menu->addChild(createCheckMenuItem(
			"Extend spring beyond panel", "",
			[m]() { return m->allowVisualOverflow.load(std::memory_order_relaxed); },
			[this, m]() {
				const bool enabled = !m->allowVisualOverflow.load(std::memory_order_relaxed);
				m->allowVisualOverflow.store(enabled, std::memory_order_relaxed);
				if (enabled) createOverflowWidget();
				else destroyOverflowWidget();
			}));
		if (isDragonKingDebugEnabled()) {
			menu->addChild(new MenuSeparator());
			menu->addChild(createCheckMenuItem(
				"Rendering log (CSV)", "",
				[this]() { return renderingLog.active; },
				[this, m]() {
					if (renderingLog.active) {
						stopRenderingLog();
					}
					else {
						startRenderingLog(m);
					}
				}));
			if (!renderingLog.path.empty()) {
				menu->addChild(createMenuLabel(
					std::string(renderingLog.active ? "Writing: " : "Last log: ")
						+ renderingLog.path));
			}
		}
	}
};

void DoorstopOverflowWidget::step() {
	TransparentWidget::step();
	if (!link || !link->owner || !link->owner->validRackContext()) {
		requestDelete();
		return;
	}
	box.pos = link->owner->overflowPosition();
	box.size = Vec(link->owner->box.size.x + 2.f * OVERFLOW_PAD, link->owner->box.size.y);
}

void DoorstopOverflowWidget::draw(const DrawArgs& args) {
	(void) args;
}

void DoorstopOverflowWidget::drawLayer(const DrawArgs& args, int layer) {
	if (layer == 1) {
		drawOverflowScene(args);
	}
	TransparentWidget::drawLayer(args, layer);
}

void DoorstopOverflowWidget::drawOverflowScene(const DrawArgs& args) {
	if (!link || !link->owner) {
		return;
	}
	const bool drawLeft = link->overflowLeftVisible;
	const bool drawRight = link->overflowRightVisible;
	if (!drawLeft && !drawRight) {
		return;
	}
	const bool measurePerf = isDragonKingDebugEnabled();
	const auto sceneStart = debug_terminal::debugTimerStart(measurePerf);
	const float moduleWidth = link->owner->box.size.x;
	const float baseX = OVERFLOW_PAD + link->springBase.x;
	const float baseY = link->springBase.y;

	if (drawLeft) {
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, OVERFLOW_PAD, box.size.y);
		drawSpringScene(args.vg, *link, baseX, baseY,
			{SpringPathClipSide::Left, OVERFLOW_PAD});
		nvgRestore(args.vg);
	}

	if (drawRight) {
		nvgSave(args.vg);
		nvgScissor(args.vg, OVERFLOW_PAD + moduleWidth, 0.f, OVERFLOW_PAD, box.size.y);
		drawSpringScene(args.vg, *link, baseX, baseY,
			{SpringPathClipSide::Right, OVERFLOW_PAD + moduleWidth});
		nvgRestore(args.vg);
	}
	if (measurePerf) {
		const float elapsedUs = debug_terminal::elapsedUsSince(sceneStart);
		auto& range = link->trailGeometryValid
			? link->debugRenderMetrics.overflowTrailUs
			: link->debugRenderMetrics.overflowIdleUs;
		range.add(elapsedUs);
		link->lastOverflowDrawUs = elapsedUs;
		++link->overflowDrawSequence;
	}
}

} // namespace

Model* modelDoorstop = createModel<Doorstop, DoorstopWidget>("Doorstop");
