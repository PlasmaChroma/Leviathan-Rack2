#include "IntegralFlux.hpp"
#include "DebugTerminalTransport.hpp"
#include "MathHelpers.hpp"
#include "Nautiloid.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"
#include "visual/FractalGlassOverlay.hpp"
#include "visual/PreviewSurface.hpp"
#include "WavePreviewTracer.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>

namespace {

constexpr double kIntegralFluxDebugTerminalSubmitIntervalSec = debug_terminal::kTimingRangeSubmitIntervalSec;
constexpr int kIntegralFluxNautiloidGlassRenderWidth = 384;
constexpr int kIntegralFluxNautiloidGlassRenderHeight = 480;
std::unordered_map<uint32_t, double> gIntegralFluxDebugTerminalLastSubmitSec;
thread_local uint64_t gIntegralFluxGearDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxEclipseDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxApertureDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxLinearPointDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxShapeGlyphDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxPlasmaSwitchDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxPreviewDrawNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxPreviewDrawNsByChannel[5] {};
thread_local uint64_t gIntegralFluxPreviewFramebufferNsThisFrame = 0u;
thread_local uint64_t gIntegralFluxPreviewFramebufferNsByChannel[5] {};
thread_local uint32_t gIntegralFluxPreviewDirtyRequestsThisFrame = 0u;
thread_local uint32_t gIntegralFluxPreviewPointRebuildsThisFrame = 0u;
thread_local uint32_t gIntegralFluxPreviewTracerCapturesThisFrame = 0u;
thread_local uint32_t gIntegralFluxLinearPointDirtyRequestsThisFrame = 0u;
thread_local uint32_t gIntegralFluxShapeGlyphDirtyRequestsThisFrame = 0u;
thread_local uint32_t gIntegralFluxHaloDirtyDrawCountThisFrame = 0u;
thread_local uint32_t gIntegralFluxHaloActiveDrawCountThisFrame = 0u;
thread_local uint32_t gIntegralFluxHaloDraggingDrawCountThisFrame = 0u;

std::string integralFluxFractalParamsRootPath() {
	return system::join(asset::user(), "Leviathan/IntegralFlux");
}

std::string integralFluxUserFractalParamsPath() {
	return system::join(integralFluxFractalParamsRootPath(), "FractalParams.json");
}

std::string integralFluxFractalParamsLibraryPath() {
	if (isDragonKingUserFractalParamsEnabled()) {
		return integralFluxUserFractalParamsPath();
	}
	return pluginInstance
		? asset::plugin(pluginInstance, "res/FractalParams.json")
		: std::string();
}

Nautiloid* leftNautiloidForIntegralFlux(const IntegralFlux* module) {
	Module* left = module ? module->leftExpander.module : nullptr;
	if (!left || !left->model ||
		(left->model != modelNautiloid && left->model->slug != "Nautiloid")) {
		return nullptr;
	}
	return dynamic_cast<Nautiloid*>(left);
}

bool appendIntegralFluxFractalParamsCapture(const IntegralFlux* module) {
	Nautiloid* naut = leftNautiloidForIntegralFlux(module);
	if (!naut) {
		return false;
	}
	system::createDirectories(integralFluxFractalParamsRootPath());

	const std::string path = integralFluxUserFractalParamsPath();
	json_error_t error;
	json_t* root = json_load_file(path.c_str(), 0, &error);
	if (!root || !json_is_object(root)) {
		if (root) json_decref(root);
		root = json_object();
	}
	json_t* entries = json_object_get(root, "entries");
	if (!entries || !json_is_array(entries)) {
		entries = json_array();
		json_object_set_new(root, "entries", entries);
	}
	json_object_set_new(root, "schemaVersion", json_integer(1));

	json_t* entry = json_object();
	json_object_set_new(entry, "capturedAtUnix", json_integer(json_int_t(std::time(nullptr))));
	json_object_set_new(entry, "source", json_string("Nautiloid"));
	json_object_set_new(entry, "fractalSourceVersion", json_integer(iris::kNautiloidFractalSourceVersion));
	json_t* fractal = json_object();
	json_object_set_new(fractal, "mode", json_integer(naut->fractalMode));
	json_object_set_new(fractal, "modeName", json_string(iris::builtinFractalName(naut->fractalMode)));
	json_object_set_new(fractal, "zoom", json_real(naut->fractalZoom));
	json_object_set_new(fractal, "centerX", json_real(naut->fractalCenterX));
	json_object_set_new(fractal, "centerY", json_real(naut->fractalCenterY));
	json_object_set_new(entry, "fractal", fractal);
	json_t* target = json_object();
	json_object_set_new(target, "width", json_integer(kIntegralFluxNautiloidGlassRenderWidth));
	json_object_set_new(target, "height", json_integer(kIntegralFluxNautiloidGlassRenderHeight));
	json_object_set_new(target, "viewportScale", json_real(1.f));
	json_object_set_new(target, "coordinateSpace", json_string("IntegralFlux module-local"));
	json_object_set_new(target, "palette", json_string("glass-region-fill-color"));
	json_object_set_new(entry, "renderTarget", target);
	json_array_append_new(entries, entry);

	const int result = json_dump_file(root, path.c_str(), JSON_INDENT(2) | JSON_SORT_KEYS);
	json_decref(root);
	if (result != 0) {
		WARN("Integral Flux: failed to save Nautiloid fractal parameters: %s", path.c_str());
	}
	return result == 0;
}

struct IntegralFluxScopedDrawTimer {
	using Clock = std::chrono::steady_clock;
	uint64_t* elapsedNs = nullptr;
	Clock::time_point start;

	IntegralFluxScopedDrawTimer(uint64_t& elapsedNs, bool enabled)
		: elapsedNs(enabled ? &elapsedNs : nullptr)
		, start(enabled ? Clock::now() : Clock::time_point()) {
	}

	~IntegralFluxScopedDrawTimer() {
		if (elapsedNs) {
			*elapsedNs += uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
				Clock::now() - start).count());
		}
	}
};

void resetIntegralFluxStepDiagnosticsForNextDraw() {
	gIntegralFluxPreviewDirtyRequestsThisFrame = 0u;
	gIntegralFluxPreviewPointRebuildsThisFrame = 0u;
	gIntegralFluxPreviewTracerCapturesThisFrame = 0u;
	gIntegralFluxLinearPointDirtyRequestsThisFrame = 0u;
	gIntegralFluxShapeGlyphDirtyRequestsThisFrame = 0u;
}

struct IntegralFluxFittedSvgWidget final : TransparentWidget {
	std::shared_ptr<window::Svg> svg;

	void setSvg(std::shared_ptr<window::Svg> svg) {
		this->svg = svg;
	}

	void draw(const DrawArgs& args) override {
		if (!svg || !svg->handle || box.size.x <= 0.f || box.size.y <= 0.f) {
			return;
		}
		const Vec svgSize = svg->getSize();
		if (svgSize.x <= 0.f || svgSize.y <= 0.f) {
			return;
		}

		nvgSave(args.vg);
		nvgScale(args.vg, box.size.x / svgSize.x, box.size.y / svgSize.y);
		svg->draw(args.vg);
		nvgRestore(args.vg);
	}
};

struct IntegralFluxLogoCrystalButton final : OpaqueWidget {
	ui::Tooltip* tooltip = nullptr;
	float previewProgressionPhase = -1.f;

	~IntegralFluxLogoCrystalButton() {
		destroyTooltip();
	}

	void createTooltip() {
		if (settings::tooltips && !tooltip) {
			tooltip = new ui::Tooltip();
			tooltip->text = visual_assets::isPanelGlassColorCycleEnabled() ? "Deactivate crystal" : "Activate crystal";
			APP->scene->addChild(tooltip);
		}
	}

	void destroyTooltip() {
		if (tooltip) {
			APP->scene->removeChild(tooltip);
			delete tooltip;
			tooltip = nullptr;
		}
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
			visual_assets::togglePanelGlassColorCycle();
			destroyTooltip();
			createTooltip();
			e.consume(this);
			return;
		}
		OpaqueWidget::onButton(e);
	}

	void onEnter(const event::Enter& e) override {
		OpaqueWidget::onEnter(e);
		createTooltip();
	}

	void onLeave(const event::Leave& e) override {
		OpaqueWidget::onLeave(e);
		destroyTooltip();
	}

	void draw(const DrawArgs& args) override {
		visual_assets::ScopedPanelGlassPreviewProgression previewProgression(
			previewProgressionPhase);
		if (!visual_assets::isPanelGlassColorCycleEnabled()) {
			return;
		}
		const Vec center = box.size.mult(0.5f);
		const float w = mm2px(2.4f);
		const float h = mm2px(1.6f);

		const float intensity = 0.3f + 0.7f * clamp(visual_assets::panelGlassTintAmount() / 0.28f, 0.f, 1.f);
		NVGcolor glowColor = visual_assets::panelGlassCrystalGlowColor();
		NVGcolor strokeColor = visual_assets::panelGlassCrystalStrokeColor();

		nvgSave(args.vg);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgTranslate(args.vg, center.x, center.y);
		nvgScale(args.vg, w * 0.5f, h * 0.5f);

		// Draw the bloom (outer softer layer)
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 0.f, 0.f, 2.2f);
		nvgFillPaint(args.vg, nvgRadialGradient(
			args.vg,
			0.f,
			0.f,
			0.1f,
			2.2f,
			nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, (45.f / 255.f) * intensity),
			nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, 0)));
		nvgFill(args.vg);

		// Draw the main bright glow (middle layer)
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 0.f, 0.f, 1.2f);
		NVGcolor coreColor = nvgRGBAf(
			glowColor.r + (1.f - glowColor.r) * 0.7f,
			glowColor.g + (1.f - glowColor.g) * 0.7f,
			glowColor.b + (1.f - glowColor.b) * 0.7f,
			(130.f / 255.f) * intensity
		);
		nvgFillPaint(args.vg, nvgRadialGradient(
			args.vg,
			0.f,
			0.f,
			0.1f,
			1.2f,
			coreColor,
			nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, 0)));
		nvgFill(args.vg);

		// Draw the center hotspot highlight
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, 0.f, 0.f, 0.4f);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, int(110 * intensity)));  // Pure white hotspot
		nvgFill(args.vg);
		nvgRestore(args.vg);

		// Draw the diamond shape matching the actual crystal dimensions
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center.x, center.y - h * 0.5f);
		nvgLineTo(args.vg, center.x + w * 0.5f, center.y);
		nvgLineTo(args.vg, center.x, center.y + h * 0.5f);
		nvgLineTo(args.vg, center.x - w * 0.5f, center.y);
		nvgClosePath(args.vg);
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStrokePaint(args.vg, nvgLinearGradient(
			args.vg,
			center.x - w * 0.5f,
			center.y - h * 0.5f,
			center.x + w * 0.5f,
			center.y + h * 0.5f,
			nvgRGBAf(strokeColor.r, strokeColor.g, strokeColor.b, (110.f / 255.f) * intensity),
			nvgRGBAf(glowColor.r, glowColor.g, glowColor.b, (130.f / 255.f) * intensity)));
		nvgStroke(args.vg);

		// Debug print showing phase percentage next to the logo crystal
		if (isDragonKingDebugEnabled() && APP && APP->window && APP->window->uiFont) {
			const float pct = visual_assets::panelGlassCyclePhase() * 100.f;
			char buf[32];
			std::snprintf(buf, sizeof(buf), "%.1f%%", pct);

			nvgSave(args.vg);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 7.5f);
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
			nvgFillColor(args.vg, nvgRGBA(0x1c, 0xcc, 0xd9, 180));
			nvgText(args.vg, box.size.x + mm2px(1.5f), box.size.y * 0.5f, buf, nullptr);
			nvgRestore(args.vg);
		}
	}
};

// Create a bigger basic button
struct BigTL1105 : TL1105 {
    BigTL1105() {
        // Dialed back to ~85% of previous size for a tighter click area.
        box.size = mm2px(Vec(9.5, 9.5));
    }
};

struct IntegralFluxPreviewEdgeInteraction {
	bool riseHovered = false;
	bool fallHovered = false;
	bool curveHovered = false;
	bool riseDragging = false;
	bool fallDragging = false;
	bool curveDragging = false;
};

struct IntegralFluxKnobTooltipState {
	int activeParamId = -1;
	int activeChannel = 0;
	const char* activeLabel = nullptr;
	bool dragging = false;

	void activate(int paramId, int channel, const char* label, bool isDragging) {
		activeParamId = paramId;
		activeChannel = channel;
		activeLabel = label;
		dragging = isDragging;
	}

	void clearIfActive(int paramId) {
		if (activeParamId == paramId) {
			activeParamId = -1;
			activeChannel = 0;
			activeLabel = nullptr;
			dragging = false;
		}
	}
};

struct WavePreviewWidget : widget::OpenGlWidget {
	// Preview boxes are small; this density materially lowers per-frame NanoVG work
	// while remaining visually smooth at current panel scale.
	static constexpr int POINT_COUNT = 128;
	static constexpr int PREVIEW_LUT_SIZE = 512;
	static constexpr float CENTER_LINE_WIDTH = 1.0f;
	static constexpr float WAVE_LINE_WIDTH = 1.4f;
	static constexpr float WAVE_EDGE_PAD = 1.0f;
	static constexpr float DOT_RADIUS = 2.1f;
	static constexpr float DOT_SHOW_MAX_HZ = 2.0f;
	static constexpr float DOT_HIDE_MIN_HZ = 2.4f;
	static constexpr float LABEL_FONT_SIZE = 11.5f;
	static constexpr int TRAIL_FRAME_COUNT = 6;
	static constexpr float TRAIL_FADE_SEC = 0.333f;
	static constexpr float TRAIL_MIN_CAPTURE_INTERVAL_SEC = 1.f / 24.f;
	static constexpr float TRAIL_LINE_WIDTH = 1.15f;
	static constexpr float GL_WAVE_LINE_WIDTH = 2.0f;
	static constexpr float GL_TRAIL_LINE_WIDTH = 1.6f;
	static constexpr float GL_HIGH_ZOOM_WIDTH_TAPER = 0.08f;
	static constexpr int TRAIL_DRAW_STRIDE = 2;
	static constexpr int TRAIL_CAPTURE_STRIDE = 1;
	int channel = 1;
	IntegralFlux* modulePtr = nullptr;
	IntegralFluxPreviewEdgeInteraction* edgeInteraction = nullptr;
	std::array<Vec, POINT_COUNT> points {};
	WavePreviewTracer<POINT_COUNT, TRAIL_FRAME_COUNT> curveTracer;
	WavePreviewBufferedTracer<POINT_COUNT> frameTracer;
	std::array<float, PREVIEW_LUT_SIZE> cachedRiseLut {};
	std::array<float, PREVIEW_LUT_SIZE> cachedFallLut {};
	float cachedLutCurveSigned = 0.f;
	int cachedLutShapeMode = IntegralFlux::FUNCTION_SHAPE_MATHS;
	bool cachedLutsValid = false;
	uint32_t lastVersion = 0;
	bool pointsValid = false;
	int peakPointIndex = POINT_COUNT / 2;
	float lastFreqHz = 100.f;
	float dotXNorm = 0.f;
	float dotYNorm = 0.f;
	bool dotVisible = false;

	WavePreviewWidget(IntegralFlux* module, int channel) {
		modulePtr = module;
		this->channel = channel;
	}

	bool useOpenGlRenderer() const {
		return modulePtr && modulePtr->previewRenderModeControl().load(std::memory_order_relaxed) == 1;
	}

	static NVGcolor tracerColorWithAlpha(float alpha) {
		return nvgRGBA(255, 190, 80, clamp(int(alpha), 0, 255));
	}

	static WavePreviewTracerStyle curveTracerStyle() {
		WavePreviewTracerStyle style;
		style.color = nvgRGBA(255, 190, 80, 255);
		style.lineWidth = TRAIL_LINE_WIDTH;
		style.fadeSec = TRAIL_FADE_SEC;
		style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
		style.maxAlpha = 118.f;
		style.drawStride = TRAIL_DRAW_STRIDE;
		return style;
	}

	static WavePreviewBufferedTracerStyle bufferedTracerStyle(int drawStride) {
		WavePreviewBufferedTracerStyle style;
		style.color = nvgRGBA(255, 190, 80, 255);
		style.fadeSec = TRAIL_FADE_SEC;
		style.minCaptureIntervalSec = TRAIL_MIN_CAPTURE_INTERVAL_SEC;
		style.maxAlpha = 118.f;
		style.drawStride = drawStride;
		return style;
	}

	static void glColorFromNvg(NVGcolor c) {
		glColor4f(c.r, c.g, c.b, c.a);
	}

	static Vec ribbonNormal(const Vec& prev, const Vec& current, const Vec& next) {
		Vec tangent = next - prev;
		float len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		if (len2 < 1e-8f) {
			tangent = next - current;
			len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		}
		if (len2 < 1e-8f) {
			tangent = current - prev;
			len2 = tangent.x * tangent.x + tangent.y * tangent.y;
		}
		if (len2 < 1e-8f) {
			return Vec(0.f, -1.f);
		}
		const float invLen = 1.f / std::sqrt(len2);
		return Vec(-tangent.y * invLen, tangent.x * invLen);
	}

	static void drawGlRibbonPoints(const Vec* linePoints, int pointCount, int stride, float lineWidth, NVGcolor color) {
		if (!linePoints || pointCount <= 0) {
			return;
		}
		stride = std::max(stride, 1);
		const float halfWidth = 0.5f * lineWidth;
		glColorFromNvg(color);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i < pointCount; i += stride) {
			const int prevIndex = std::max(0, i - stride);
			const int nextIndex = std::min(pointCount - 1, i + stride);
			const Vec n = ribbonNormal(linePoints[prevIndex], linePoints[i], linePoints[nextIndex]);
			glVertex2f(linePoints[i].x + n.x * halfWidth, linePoints[i].y + n.y * halfWidth);
			glVertex2f(linePoints[i].x - n.x * halfWidth, linePoints[i].y - n.y * halfWidth);
		}
		if ((pointCount - 1) % stride != 0) {
			const int i = pointCount - 1;
			const int prevIndex = std::max(0, i - stride);
			const Vec n = ribbonNormal(linePoints[prevIndex], linePoints[i], linePoints[i]);
			glVertex2f(linePoints[i].x + n.x * halfWidth, linePoints[i].y + n.y * halfWidth);
			glVertex2f(linePoints[i].x - n.x * halfWidth, linePoints[i].y - n.y * halfWidth);
		}
		glEnd();
	}

	static void drawGlRibbon(const std::array<Vec, POINT_COUNT>& linePoints, int stride, float lineWidth, NVGcolor color) {
		drawGlRibbonPoints(linePoints.data(), POINT_COUNT, stride, lineWidth, color);
	}

	int highlightedEdge() const {
		if (!edgeInteraction) {
			return 0;
		}
		if (edgeInteraction->curveDragging) {
			return 3;
		}
		if (edgeInteraction->riseDragging) {
			return 1;
		}
		if (edgeInteraction->fallDragging) {
			return 2;
		}
		if (edgeInteraction->curveHovered) {
			return 3;
		}
		if (edgeInteraction->riseHovered) {
			return 1;
		}
		if (edgeInteraction->fallHovered) {
			return 2;
		}
		return 0;
	}

	static NVGcolor waveformColor() {
		return nvgRGBA(230, 230, 220, 255);
	}

	void drawNvgGrid(const DrawArgs& args) const {
		const float w = box.size.x;
		const float h = box.size.y;
		const int majorCols = std::max(3, int(std::round(w / 16.f)));
		const int majorRows = std::max(3, int(std::round(h / 16.f)));
		const float majorX = w / float(majorCols);
		const float majorY = h / float(majorRows);
		nvgBeginPath(args.vg);
		for (int col = 0; col < majorCols; ++col) {
			for (int sub = 1; sub < 4; ++sub) {
				const float x = float(col) * majorX + majorX * (float(sub) * 0.25f);
				nvgMoveTo(args.vg, x, 0.f);
				nvgLineTo(args.vg, x, h);
			}
		}
		for (int row = 0; row < majorRows; ++row) {
			for (int sub = 1; sub < 4; ++sub) {
				const float y = float(row) * majorY + majorY * (float(sub) * 0.25f);
				nvgMoveTo(args.vg, 0.f, y);
				nvgLineTo(args.vg, w, y);
			}
		}
		nvgStrokeWidth(args.vg, 0.38f);
		nvgStrokeColor(args.vg, nvgRGBA(0x1c, 0xcc, 0xd9, 30));
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		for (int col = 1; col < majorCols; ++col) {
			const float x = float(col) * majorX;
			nvgMoveTo(args.vg, x, 0.f);
			nvgLineTo(args.vg, x, h);
		}
		for (int row = 1; row < majorRows; ++row) {
			const float y = float(row) * majorY;
			nvgMoveTo(args.vg, 0.f, y);
			nvgLineTo(args.vg, w, y);
		}
		nvgStrokeWidth(args.vg, 0.55f);
		nvgStrokeColor(args.vg, nvgRGBA(0x72, 0x8d, 0xff, 46));
		nvgStroke(args.vg);
	}

	void drawGlGrid() const {
		const float w = box.size.x;
		const float h = box.size.y;
		const int majorCols = std::max(3, int(std::round(w / 16.f)));
		const int majorRows = std::max(3, int(std::round(h / 16.f)));
		const float majorX = w / float(majorCols);
		const float majorY = h / float(majorRows);
		glLineWidth(0.38f);
		glColor4f(0x1c / 255.f, 0xcc / 255.f, 0xd9 / 255.f, 30.f / 255.f);
		glBegin(GL_LINES);
		for (int col = 0; col < majorCols; ++col) for (int sub = 1; sub < 4; ++sub) {
			const float x = float(col) * majorX + majorX * (float(sub) * 0.25f);
			glVertex2f(x, 0.f); glVertex2f(x, h);
		}
		for (int row = 0; row < majorRows; ++row) for (int sub = 1; sub < 4; ++sub) {
			const float y = float(row) * majorY + majorY * (float(sub) * 0.25f);
			glVertex2f(0.f, y); glVertex2f(w, y);
		}
		glEnd();
		glLineWidth(0.55f);
		glColor4f(0x72 / 255.f, 0x8d / 255.f, 1.f, 46.f / 255.f);
		glBegin(GL_LINES);
		for (int col = 1; col < majorCols; ++col) {
			const float x = float(col) * majorX;
			glVertex2f(x, 0.f); glVertex2f(x, h);
		}
		for (int row = 1; row < majorRows; ++row) {
			const float y = float(row) * majorY;
			glVertex2f(0.f, y); glVertex2f(w, y);
		}
		glEnd();
	}

	NVGcolor activeEdgeColor(int edge) const {
		const NVGcolor purple = nvgRGB(0x86, 0x5c, 0xff);
		const NVGcolor cyan = nvgRGB(0x00, 0xc6, 0xe4);
		if (!modulePtr) {
			return purple;
		}

		int paramId = -1;
		if (edge == 1) {
			paramId = channel == 4 ? IntegralFlux::RISE_4_PARAM : IntegralFlux::RISE_1_PARAM;
		}
		else if (edge == 2) {
			paramId = channel == 4 ? IntegralFlux::FALL_4_PARAM : IntegralFlux::FALL_1_PARAM;
		}
		if (paramId < 0) {
			return purple;
		}

		const float amount = clamp(modulePtr->params[paramId].getValue(), 0.f, 1.f);
		return nvgRGBAf(
			purple.r + (cyan.r - purple.r) * amount,
			purple.g + (cyan.g - purple.g) * amount,
			purple.b + (cyan.b - purple.b) * amount,
			1.f);
	}

	NVGcolor activeCurveColor() const {
		const NVGcolor orange = nvgRGB(0xdc, 0x5e, 0x1e);
		const NVGcolor yellow = nvgRGB(0xff, 0xb8, 0x00);
		if (!modulePtr) {
			return orange;
		}

		const int paramId = channel == 4 ? IntegralFlux::LIN_LOG_4_PARAM : IntegralFlux::LIN_LOG_1_PARAM;
		const float amount = clamp(modulePtr->params[paramId].getValue(), 0.f, 1.f);
		return nvgRGBAf(
			orange.r + (yellow.r - orange.r) * amount,
			orange.g + (yellow.g - orange.g) * amount,
			orange.b + (yellow.b - orange.b) * amount,
			1.f);
	}

	void drawGlWaveSegment(int start, int end, float lineScale, NVGcolor color) {
		if (!pointsValid) {
			return;
		}
		start = clamp(start, 0, POINT_COUNT - 1);
		end = clamp(end, 0, POINT_COUNT - 1);
		const int count = end - start + 1;
		if (count < 2) {
			return;
		}
		drawGlRibbonPoints(points.data() + start, count, 1, GL_WAVE_LINE_WIDTH * lineScale, color);
	}

	void drawGlWaveform(float lineScale) {
		const int edge = highlightedEdge();
		if (edge == 0) {
			drawGlWaveSegment(0, POINT_COUNT - 1, lineScale, waveformColor());
			return;
		}
		if (edge == 3) {
			drawGlWaveSegment(0, POINT_COUNT - 1, lineScale, activeCurveColor());
			return;
		}
		const int peakIndex = clamp(peakPointIndex, 1, POINT_COUNT - 2);
		const NVGcolor highlightColor = activeEdgeColor(edge);
		drawGlWaveSegment(0, peakIndex, lineScale, edge == 1 ? highlightColor : waveformColor());
		drawGlWaveSegment(peakIndex, POINT_COUNT - 1, lineScale, edge == 2 ? highlightColor : waveformColor());
	}

	void drawNvgWaveSegment(const DrawArgs& args, int start, int end, NVGcolor color, size_t* reducedPointCount) {
		if (!pointsValid) {
			return;
		}
		start = clamp(start, 0, POINT_COUNT - 1);
		end = clamp(end, 0, POINT_COUNT - 1);
		const int count = end - start + 1;
		if (count < 2) {
			return;
		}
		NVGcontext* vg = args.vg;
		nvgBeginPath(vg);
		wave_preview::simplifyPath(points.data() + start, count, 1, 0.02f, [vg, reducedPointCount](const Vec& pt, bool isMove) {
			if (reducedPointCount) {
				++(*reducedPointCount);
			}
			if (isMove) {
				nvgMoveTo(vg, pt.x, pt.y);
			}
			else {
				nvgLineTo(vg, pt.x, pt.y);
			}
		});
		nvgStrokeColor(vg, color);
		nvgStrokeWidth(vg, WAVE_LINE_WIDTH);
		nvgLineCap(vg, NVG_BUTT);
		nvgLineJoin(vg, NVG_ROUND);
		nvgStroke(vg);
	}

	size_t drawNvgWaveform(const DrawArgs& args) {
		size_t reducedPointCount = 0;
		const int edge = highlightedEdge();
		if (edge == 0) {
			drawNvgWaveSegment(args, 0, POINT_COUNT - 1, waveformColor(), &reducedPointCount);
			return reducedPointCount;
		}
		if (edge == 3) {
			drawNvgWaveSegment(args, 0, POINT_COUNT - 1, activeCurveColor(), &reducedPointCount);
			return reducedPointCount;
		}
		const int peakIndex = clamp(peakPointIndex, 1, POINT_COUNT - 2);
		const NVGcolor highlightColor = activeEdgeColor(edge);
		drawNvgWaveSegment(args, 0, peakIndex, edge == 1 ? highlightColor : waveformColor(), &reducedPointCount);
		drawNvgWaveSegment(args, peakIndex, POINT_COUNT - 1, edge == 2 ? highlightColor : waveformColor(), &reducedPointCount);
		return reducedPointCount;
	}

	static const std::array<Vec, 25>& glDotUnitCircle() {
		static const std::array<Vec, 25> unit = []() {
			std::array<Vec, 25> points {};
			for (int i = 0; i <= 24; ++i) {
				const float a = 6.28318530718f * float(i) / 24.f;
				points[size_t(i)] = Vec(std::cos(a), std::sin(a));
			}
			return points;
		}();
		return unit;
	}

	Vec dotPosition() const {
		const float w = std::max(box.size.x, 1.f);
		const float h = std::max(box.size.y, 1.f);
		const float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
		const float left = drawPad;
		const float top = drawPad;
		const float right = std::max(left + 1.f, w - drawPad);
		const float bottom = std::max(top + 1.f, h - drawPad);
		const float targetX = left + clamp(dotXNorm, 0.f, 1.f) * (right - left);
		const float targetY = top + (1.f - clamp(dotYNorm, 0.f, 1.f)) * (bottom - top);

		int i0 = 0;
		for (int i = 1; i < POINT_COUNT; ++i) {
			if (points[i].x >= targetX) {
				i0 = i - 1;
				break;
			}
			i0 = i - 1;
		}
		const int i1 = std::min(i0 + 1, POINT_COUNT - 1);
		float y = points[i0].y;
		if (i1 != i0 && points[i1].x > points[i0].x) {
			const float t = clamp(
				(targetX - points[i0].x) / (points[i1].x - points[i0].x), 0.f, 1.f);
			y += (points[i1].y - y) * t;
		}
		constexpr float curveBlend = 0.9f;
		return Vec(targetX, y * curveBlend + targetY * (1.f - curveBlend));
	}

	void drawGlDot() {
		if (!pointsValid || !dotVisible) {
			return;
		}
		const Vec dot = dotPosition();
		glColor4f(0.f, 0.f, 0.f, 0.86f);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(dot.x, dot.y);
		const std::array<Vec, 25>& unitCircle = glDotUnitCircle();
		for (const Vec& p : unitCircle) {
			glVertex2f(dot.x + p.x * (DOT_RADIUS + 0.55f), dot.y + p.y * (DOT_RADIUS + 0.55f));
		}
		glEnd();
		glColor4f(1.f, 0.91f, 0.28f, 1.f);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(dot.x, dot.y);
		for (const Vec& p : unitCircle) {
			glVertex2f(dot.x + p.x * DOT_RADIUS, dot.y + p.y * DOT_RADIUS);
		}
		glEnd();
	}

	void drawFramebuffer() override {
		IntegralFluxScopedDrawTimer framebufferTimer(
			gIntegralFluxPreviewFramebufferNsThisFrame, isDragonKingDebugEnabled());
		IntegralFluxScopedDrawTimer channelFramebufferTimer(
			gIntegralFluxPreviewFramebufferNsByChannel[clamp(channel, 0, 4)], isDragonKingDebugEnabled());
		math::Vec fbSize = getFramebufferSize();
		glViewport(0, 0, std::max(1, int(std::lround(fbSize.x))), std::max(1, int(std::lround(fbSize.y))));
		// The cached preview surface below supplies the opaque black/grid layer.
		// Keep this live waveform framebuffer transparent above it.
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (!pointsValid || !useOpenGlRenderer()) {
			return;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, box.size.x, box.size.y, 0.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_SCISSOR_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LINE_SMOOTH);

		const double nowSec = system::getTime();
		const float xScale = fbSize.x / std::max(box.size.x, 1.f);
		const float yScale = fbSize.y / std::max(box.size.y, 1.f);
		const float framebufferScale = std::max(0.1f, 0.5f * (xScale + yScale));
		const float lineScale = framebufferScale < 1.f
		                      ? std::sqrt(framebufferScale)
		                      : 1.f / (1.f + (framebufferScale - 1.f) * GL_HIGH_ZOOM_WIDTH_TAPER);
		const bool tracerEnabled = modulePtr && modulePtr->previewTracerEnabledControl().load(std::memory_order_relaxed);
		if (tracerEnabled) {
			for (const auto& frame : curveTracer.frames) {
				if (!frame.active) {
					continue;
				}
				const float age = float(nowSec - frame.birthSec);
				if (age < 0.f || age >= TRAIL_FADE_SEC) {
					continue;
				}
				const float fade = 1.f - age / TRAIL_FADE_SEC;
				drawGlRibbonPoints(frame.points.data(), int(frame.pointCount), 1, GL_TRAIL_LINE_WIDTH * lineScale, tracerColorWithAlpha(118.f * fade));
			}
		}
		drawGlWaveform(lineScale);
		if (modulePtr) {
			modulePtr->recordCurvePointReduction(channel, POINT_COUNT, POINT_COUNT);
		}
		drawGlDot();
	}

	void drawFrequencyLabel(const DrawArgs& args) {
		char freqText[32];
		if (lastFreqHz < 1.f) {
			std::snprintf(freqText, sizeof(freqText), "%4.0f mHz", lastFreqHz * 1000.f);
		}
		else if (lastFreqHz >= 1000.f) {
			std::snprintf(freqText, sizeof(freqText), "%4.2f kHz", lastFreqHz / 1000.f);
		}
		else {
			std::snprintf(freqText, sizeof(freqText), "%5.1f Hz", lastFreqHz);
		}
		nvgFontSize(args.vg, LABEL_FONT_SIZE);
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		nvgText(args.vg, box.size.x * 0.5f, box.size.y + 1.5f, freqText, nullptr);
	}

	static void buildSegmentLut(std::array<float, PREVIEW_LUT_SIZE>& lut, float curveSigned, bool rising,
		IntegralFlux::FunctionShapeMode shapeMode) {
		// Build once per preview update. Midpoint integration reduces visual artifacts at extreme curve asymmetry.
		float scale = IntegralFlux::slopeWarpScaleForMode(curveSigned, rising, shapeMode);
		float dp = 1.f / float(PREVIEW_LUT_SIZE - 1);
		float x = rising ? 0.f : 1.f;
		lut[0] = x;
		for (int i = 1; i < PREVIEW_LUT_SIZE; ++i) {
			float k1 = IntegralFlux::slopeWarpForMode(x, curveSigned, rising, shapeMode) * scale;
			float xMid = rising ? (x + 0.5f * dp * k1) : (x - 0.5f * dp * k1);
			xMid = clamp(xMid, 0.f, 1.f);
			float k2 = IntegralFlux::slopeWarpForMode(xMid, curveSigned, rising, shapeMode) * scale;
			x += rising ? (dp * k2) : (-dp * k2);
			x = clamp(x, 0.f, 1.f);
			lut[i] = x;
		}
		lut.front() = rising ? 0.f : 1.f;
		lut.back() = rising ? 1.f : 0.f;
	}

	static float sampleSegmentLut(const std::array<float, PREVIEW_LUT_SIZE>& lut, float t) {
		t = clamp(t, 0.f, 1.f);
		float idx = t * float(PREVIEW_LUT_SIZE - 1);
		int i0 = int(idx);
		int i1 = std::min(i0 + 1, PREVIEW_LUT_SIZE - 1);
		float f = idx - float(i0);
		return lut[i0] + (lut[i1] - lut[i0]) * f;
	}

		void ensureSegmentLuts(float curveSigned, IntegralFlux::FunctionShapeMode shapeMode) {
			if (cachedLutsValid
				&& std::fabs(curveSigned - cachedLutCurveSigned) <= 1e-6f
				&& cachedLutShapeMode == int(shapeMode)) {
				return;
			}
			buildSegmentLut(cachedRiseLut, curveSigned, true, shapeMode);
			buildSegmentLut(cachedFallLut, curveSigned, false, shapeMode);
			cachedLutCurveSigned = curveSigned;
			cachedLutShapeMode = int(shapeMode);
			cachedLutsValid = true;
		}

		void rebuildPoints(float riseTime, float fallTime, float curveSigned,
			IntegralFlux::FunctionShapeMode shapeMode, bool interactiveRecent) {
		float w = std::max(box.size.x, 1.f);
		float h = std::max(box.size.y, 1.f);
		float drawPad = 0.5f * WAVE_LINE_WIDTH + WAVE_EDGE_PAD;
		float left = drawPad;
		float top = drawPad;
		float right = std::max(left + 1.f, w - drawPad);
		float bottom = std::max(top + 1.f, h - drawPad);
		float drawW = right - left;
		float drawH = bottom - top;
		// The preview always shows exactly one full rise+fall cycle across widget width.
		float totalTime = std::max(riseTime + fallTime, 1e-6f);
		float riseRatio = riseTime / totalTime;
		float peakX = left + riseRatio * drawW;
		float riseWidth = std::max(peakX - left, 1e-4f);
		float fallWidth = std::max(right - peakX, 1e-4f);
		// Reserved hook if we later render interactive-state emphasis.
		(void) interactiveRecent;
			ensureSegmentLuts(curveSigned, shapeMode);

		for (int i = 0; i < POINT_COUNT; ++i) {
			float xNorm = float(i) / float(POINT_COUNT - 1);
			float x = left + xNorm * drawW;
			float y = -1.f;
			if (x <= peakX) {
				float t = (x - left) / riseWidth;
					float v = sampleSegmentLut(cachedRiseLut, t);
				y = -1.f + 2.f * v;
			}
			else {
				float t = (x - peakX) / fallWidth;
					float v = sampleSegmentLut(cachedFallLut, t);
				y = -1.f + 2.f * v;
			}
			float py = top + (0.5f - 0.5f * y) * drawH;
			py = clamp(py, top, bottom);
			points[i] = Vec(x, py);
		}

		// Preserve full crest height without flattening the apex into a
		// two-point plateau when the true peak falls between sample columns.
		float peakIndexF = riseRatio * float(POINT_COUNT - 1);
		int peakIndex = std::max(1, std::min(POINT_COUNT - 2, int(std::round(peakIndexF))));
		peakPointIndex = peakIndex;
		points[peakIndex] = Vec(peakX, top);
		points.front() = Vec(left, bottom);
		points.back() = Vec(right, bottom);
		pointsValid = true;
	}

	void step() override {
		const bool openGlRenderer = useOpenGlRenderer();
		if (!openGlRenderer) {
			Widget::step();
		}
		if (!modulePtr) {
			if (!pointsValid) {
				rebuildPoints(0.01f, 0.01f, 0.f, IntegralFlux::FUNCTION_SHAPE_MATHS, false);
			}
			return;
		}
		float riseTime = 0.01f;
		float fallTime = 0.01f;
		float curveSigned = 0.f;
		float previewDotXNorm = 0.f;
		float previewDotYNorm = 0.f;
		bool previewDotVisible = false;
		IntegralFlux::FunctionShapeMode shapeMode = IntegralFlux::FUNCTION_SHAPE_MATHS;
		bool interactiveRecent = false;
		uint32_t version = 0;
		modulePtr->getPreviewState(channel, riseTime, fallTime, curveSigned, previewDotXNorm, previewDotYNorm,
			previewDotVisible, shapeMode, interactiveRecent, version);
		dotXNorm = previewDotXNorm;
		dotYNorm = previewDotYNorm;
		// Displayed frequency reflects the currently effective cycle period.
		lastFreqHz = 1.f / std::max(riseTime + fallTime, 1e-6f);
		// Always hide when FG is inactive; frequency hysteresis only applies while active.
		if (!previewDotVisible) {
			dotVisible = false;
		}
		else if (lastFreqHz >= DOT_HIDE_MIN_HZ) {
			dotVisible = false;
		}
		else if (lastFreqHz <= DOT_SHOW_MAX_HZ) {
			dotVisible = true;
		}
		const double nowSec = system::getTime();
		const bool tracerEnabled = modulePtr->previewTracerEnabledControl().load(std::memory_order_relaxed);
		const int tracerMode = openGlRenderer ? WAVE_PREVIEW_TRACER_CURVE_CACHE
		                                      : modulePtr->previewTracerCacheModeControl().load(std::memory_order_relaxed);
		if (!tracerEnabled) {
			curveTracer.clear();
			frameTracer.clear();
		}
		else if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
			curveTracer.expire(nowSec, TRAIL_FADE_SEC);
			frameTracer.clear();
		}
		else {
			curveTracer.clear();
		}
		if (!pointsValid || version != lastVersion) {
			if (tracerEnabled && pointsValid) {
				if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
					const WavePreviewTracerCaptureStats stats =
						curveTracer.capture(points, nowSec, TRAIL_MIN_CAPTURE_INTERVAL_SEC, TRAIL_CAPTURE_STRIDE);
					modulePtr->recordTracerExtraPointReduction(channel, stats);
					++gIntegralFluxPreviewTracerCapturesThisFrame;
				}
				else {
					const WavePreviewBufferedTracerStyle style =
						bufferedTracerStyle(TRAIL_CAPTURE_STRIDE);
					const WavePreviewTracerCaptureStats stats = frameTracer.capture(points, nowSec, box.size, style);
					modulePtr->recordTracerExtraPointReduction(channel, stats);
					++gIntegralFluxPreviewTracerCapturesThisFrame;
				}
			}
			++gIntegralFluxPreviewPointRebuildsThisFrame;
			rebuildPoints(riseTime, fallTime, curveSigned, shapeMode, interactiveRecent);
			lastVersion = version;
		}
		if (openGlRenderer) {
			++gIntegralFluxPreviewDirtyRequestsThisFrame;
			setDirty();
			FramebufferWidget::step();
		}
	}

	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxPreviewDrawNsThisFrame, isDragonKingDebugEnabled());
		IntegralFluxScopedDrawTimer channelTimer(
			gIntegralFluxPreviewDrawNsByChannel[clamp(channel, 0, 4)], isDragonKingDebugEnabled());
		if (useOpenGlRenderer()) {
			widget::OpenGlWidget::draw(args);
			drawFrequencyLabel(args);
			return;
		}
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		if (pointsValid) {
			const double nowSec = system::getTime();
			const bool tracerEnabled = modulePtr && modulePtr->previewTracerEnabledControl().load(std::memory_order_relaxed);
			if (tracerEnabled) {
				const int tracerMode = modulePtr->previewTracerCacheModeControl().load(std::memory_order_relaxed);
				if (tracerMode == WAVE_PREVIEW_TRACER_CURVE_CACHE) {
					curveTracer.draw(args.vg, nowSec, curveTracerStyle());
				}
				else {
					frameTracer.draw(args.vg, nowSec, box.size, bufferedTracerStyle(TRAIL_DRAW_STRIDE));
				}
			}
			size_t reducedPointCount = drawNvgWaveform(args);
			if (modulePtr) {
				modulePtr->recordCurvePointReduction(channel, POINT_COUNT, reducedPointCount);
			}
		}
		if (pointsValid && dotVisible) {
			const Vec dot = dotPosition();
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, dot.x, dot.y, DOT_RADIUS);
			nvgFillColor(args.vg, nvgRGBA(255, 232, 72, 255));
			nvgFill(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, dot.x, dot.y, DOT_RADIUS + 0.55f);
			nvgStrokeWidth(args.vg, 0.9f);
			nvgStrokeColor(args.vg, nvgRGBA(0, 0, 0, 220));
			nvgStroke(args.vg);
		}

		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

		// Keep label outside preview box to avoid occluding waveform.
		drawFrequencyLabel(args);
	}
};

static math::Rect insetRectMm(math::Rect rect, float insetMm) {
	rect.pos.x += insetMm;
	rect.pos.y += insetMm;
	rect.size.x = std::max(0.f, rect.size.x - 2.f * insetMm);
	rect.size.y = std::max(0.f, rect.size.y - 2.f * insetMm);
	return rect;
}

struct IntegralFluxCentralTooltipOverlay : TransparentWidget {
	IntegralFlux* module = nullptr;
	IntegralFluxKnobTooltipState* state = nullptr;

	void draw(const DrawArgs& args) override {
		if (!module || !state || state->activeParamId < 0 || state->activeParamId >= IntegralFlux::PARAMS_LEN) {
			return;
		}
		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}

		const float valuePct = clamp(module->params[state->activeParamId].getValue(), 0.f, 1.f) * 100.f;
		char text[64];
		std::snprintf(text, sizeof(text), "CH%d %s  %.1f%%",
			state->activeChannel,
			state->activeLabel ? state->activeLabel : "",
			valuePct);

		const float bubbleW = box.size.x;
		const float bubbleH = box.size.y;
		const float x = 0.f;
		const float y = 0.f;
		const float radius = mm2px(1.7f);

		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x, y, bubbleW, bubbleH, radius);
		nvgFillColor(args.vg, nvgRGBA(7, 9, 18, state->dragging ? 222 : 190));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, x + 0.5f, y + 0.5f, bubbleW - 1.f, bubbleH - 1.f, radius);
		nvgStrokeWidth(args.vg, 1.0f);
		nvgStrokeColor(args.vg, nvgRGBA(0x86, 0x5c, 0xff, state->dragging ? 176 : 130));
		nvgStroke(args.vg);

		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 10.2f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGBA(242, 238, 255, 238));
		nvgText(args.vg, x + bubbleW * 0.5f, y + bubbleH * 0.52f, text, nullptr);
		nvgRestore(args.vg);
	}
};

struct IntegralFluxHalo2Knob : LeviathanHaloKnob2 {
	enum PreviewEdge {
		PREVIEW_EDGE_NONE,
		PREVIEW_EDGE_RISE,
		PREVIEW_EDGE_FALL,
		PREVIEW_CURVE
	};

	IntegralFluxPreviewEdgeInteraction* previewInteraction = nullptr;
	PreviewEdge previewEdge = PREVIEW_EDGE_NONE;
	IntegralFluxKnobTooltipState* tooltipState = nullptr;
	const char* tooltipLabel = nullptr;
	int tooltipChannel = 0;
	int tooltipParamId = -1;
	bool tooltipHovered = false;
	bool tooltipDragging = false;
	bool suppressRackTooltip = false;

	IntegralFluxHalo2Knob() = default;
	explicit IntegralFluxHalo2Knob(Config config) : LeviathanHaloKnob2(config) {}

	void setPreviewInteraction(IntegralFluxPreviewEdgeInteraction* interaction, PreviewEdge edge) {
		previewInteraction = interaction;
		previewEdge = edge;
	}

	void setCentralTooltip(IntegralFluxKnobTooltipState* state, int channel, const char* label, int paramId) {
		tooltipState = state;
		tooltipChannel = channel;
		tooltipLabel = label;
		tooltipParamId = paramId;
	}

	void setSuppressRackTooltip(bool suppress) {
		suppressRackTooltip = suppress;
	}

	void updateCentralTooltip(bool active, bool isDragging) {
		if (!tooltipState || tooltipParamId < 0) {
			return;
		}
		if (active) {
			tooltipState->activate(tooltipParamId, tooltipChannel, tooltipLabel, isDragging);
		}
		else {
			tooltipState->clearIfActive(tooltipParamId);
		}
	}

	void setHovered(bool hovered) {
		if (!previewInteraction) {
			return;
		}
		if (previewEdge == PREVIEW_EDGE_RISE) {
			previewInteraction->riseHovered = hovered;
		}
		else if (previewEdge == PREVIEW_EDGE_FALL) {
			previewInteraction->fallHovered = hovered;
		}
		else if (previewEdge == PREVIEW_CURVE) {
			previewInteraction->curveHovered = hovered;
		}
	}

	void setDragging(bool dragging) {
		if (!previewInteraction) {
			return;
		}
		if (previewEdge == PREVIEW_EDGE_RISE) {
			previewInteraction->riseDragging = dragging;
		}
		else if (previewEdge == PREVIEW_EDGE_FALL) {
			previewInteraction->fallDragging = dragging;
		}
		else if (previewEdge == PREVIEW_CURVE) {
			previewInteraction->curveDragging = dragging;
		}
	}

	void onEnter(const event::Enter& e) override {
		setHovered(true);
		tooltipHovered = true;
		updateCentralTooltip(true, tooltipDragging);
		if (suppressRackTooltip) {
			hovered = true;
			updateCenterSvg();
			destroyTooltip();
			return;
		}
		LeviathanHaloKnob2::onEnter(e);
	}

	void onLeave(const event::Leave& e) override {
		setHovered(false);
		tooltipHovered = false;
		if (!tooltipDragging) {
			updateCentralTooltip(false, false);
		}
		if (suppressRackTooltip) {
			hovered = false;
			updateCenterSvg();
			destroyTooltip();
			app::Knob::onLeave(e);
			return;
		}
		LeviathanHaloKnob2::onLeave(e);
	}

	void onDragStart(const event::DragStart& e) override {
		setDragging(true);
		tooltipDragging = true;
		updateCentralTooltip(true, true);
		LeviathanHaloKnob2::onDragStart(e);
	}

	void onDragEnd(const event::DragEnd& e) override {
		setDragging(false);
		tooltipDragging = false;
		if (tooltipHovered) {
			updateCentralTooltip(true, false);
		}
		else {
			updateCentralTooltip(false, false);
		}
		LeviathanHaloKnob2::onDragEnd(e);
	}

	void draw(const DrawArgs& args) override {
		if (isDragonKingDebugEnabled()) {
			if (isVisualDirty()) {
				++gIntegralFluxHaloDirtyDrawCountThisFrame;
			}
			if (hovered || dragging || tooltipHovered || tooltipDragging) {
				++gIntegralFluxHaloActiveDrawCountThisFrame;
			}
			if (dragging || tooltipDragging) {
				++gIntegralFluxHaloDraggingDrawCountThisFrame;
			}
		}
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxGearDrawNsThisFrame, isDragonKingDebugEnabled());
		LeviathanHaloKnob2::draw(args);
	}
};

struct IntegralFluxCurveHalo2Knob : IntegralFluxHalo2Knob {
	IntegralFluxCurveHalo2Knob() : IntegralFluxHalo2Knob(LeviathanHaloKnob2::brightOrangeConfig()) {
	}
};

struct IntegralFluxPlasmaSwitch : PlasmaSwitch {
	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxPlasmaSwitchDrawNsThisFrame, isDragonKingDebugEnabled());
		PlasmaSwitch::draw(args);
	}
};

struct IntegralFluxLinearPointOverlay : TransparentWidget {
	IntegralFlux* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	int shapeModeParamId = -1;
	Vec centerPx;
	float linearValue = IntegralFlux::LINEAR_SHAPE;
	float lastTarget = IntegralFlux::LINEAR_SHAPE;
	double lastStepTime = 0.0;
	bool initialized = false;

	static constexpr float BASE_ANGLE_DEG = 120.f;
	static constexpr float SWEEP_ANGLE_DEG = 300.f;
	static constexpr float ANIMATION_RATE = 3.2f;
	static constexpr float LINE_RADIUS_MM = 8.6f;
	static constexpr float LABEL_RADIUS_MM = 10.9f;
	static constexpr float LABEL_TANGENT_OFFSET_MM = 0.85f;
	static constexpr float LABEL_Y_OFFSET_MM = 0.32f;
	static constexpr float LABEL_TOP_Y_OFFSET_MM = 0.18f;
	static constexpr float LABEL_MIRROR_X_OFFSET_MM = 0.38f;
	static constexpr float LABEL_MIRROR_Y_OFFSET_MM = -0.28f;
	static constexpr float LINE_WIDTH_MM = 0.5f;
	static constexpr float TRIANGLE_GLYPH_WIDTH_MM = 4.1f;
	static constexpr float TRIANGLE_GLYPH_HEIGHT_MM = 2.35f;
	static constexpr float TRIANGLE_GLYPH_LINE_WIDTH_MM = 0.42f;

	IntegralFluxLinearPointOverlay(IntegralFlux* module, int shapeModeParamId, Vec centerPx)
		: module(module)
		, shapeModeParamId(shapeModeParamId)
		, centerPx(centerPx) {
		box.pos = Vec(0.f, 0.f);
	}

	float targetLinearValue() const {
		if (!module || shapeModeParamId < 0) {
			return IntegralFlux::LINEAR_SHAPE;
		}
		const IntegralFlux::FunctionShapeMode mode =
			IntegralFlux::functionShapeModeFromParam(module->params[shapeModeParamId].getValue());
		return mode == IntegralFlux::FUNCTION_SHAPE_SHARK_FIN
			? IntegralFlux::SHARK_FIN_LINEAR_SHAPE
			: IntegralFlux::LINEAR_SHAPE;
	}

	void step() override {
		TransparentWidget::step();
		const float target = targetLinearValue();
		const double now = system::getTime();
		bool dirty = false;
		if (!initialized) {
			linearValue = target;
			lastTarget = target;
			lastStepTime = now;
			initialized = true;
			dirty = true;
		}
		else if (std::fabs(target - linearValue) <= 1e-4f) {
			const float previous = linearValue;
			linearValue = target;
			dirty = std::fabs(target - lastTarget) > 1e-6f || std::fabs(linearValue - previous) > 1e-5f;
			lastTarget = target;
			lastStepTime = now;
		}
		else {
			const float previous = linearValue;
			const float dt = clamp(float(now - lastStepTime), 0.f, 0.05f);
			lastStepTime = now;
			const float alpha = 1.f - std::exp(-ANIMATION_RATE * dt);
			linearValue += (target - linearValue) * alpha;
			lastTarget = target;
			dirty = std::fabs(linearValue - previous) > 1e-5f;
		}
		if (dirty && framebuffer) {
			++gIntegralFluxLinearPointDirtyRequestsThisFrame;
			framebuffer->setDirty();
		}
	}

	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxLinearPointDrawNsThisFrame, isDragonKingDebugEnabled());
		const float angle = (BASE_ANGLE_DEG + SWEEP_ANGLE_DEG * linearValue) * (float(M_PI) / 180.f);
		const Vec dir(std::cos(angle), std::sin(angle));
		const Vec tangent(-dir.y, dir.x);
		const float lineRadius = mm2px(Vec(LINE_RADIUS_MM, 0.f)).x;
		const float labelRadius = mm2px(Vec(LABEL_RADIUS_MM, 0.f)).x;
		const float tangentOffset = mm2px(Vec(LABEL_TANGENT_OFFSET_MM, 0.f)).x
			* clamp(std::fabs(linearValue - IntegralFlux::SHARK_FIN_LINEAR_SHAPE)
				/ std::max(IntegralFlux::SHARK_FIN_LINEAR_SHAPE - IntegralFlux::LINEAR_SHAPE, 1e-4f), 0.f, 1.f);
		const float topBlend = clamp((linearValue - IntegralFlux::LINEAR_SHAPE)
			/ std::max(IntegralFlux::SHARK_FIN_LINEAR_SHAPE - IntegralFlux::LINEAR_SHAPE, 1e-4f), 0.f, 1.f);
		const float labelYOffset = mm2px(Vec(0.f, LABEL_Y_OFFSET_MM + LABEL_TOP_Y_OFFSET_MM * topBlend)).y;
		const Vec mirrorOffset = mm2px(Vec(
			LABEL_MIRROR_X_OFFSET_MM * (1.f - topBlend),
			LABEL_MIRROR_Y_OFFSET_MM * (1.f - topBlend)));
		const float lineWidth = mm2px(Vec(LINE_WIDTH_MM, 0.f)).x;
		const float triangleWidth = mm2px(Vec(TRIANGLE_GLYPH_WIDTH_MM, 0.f)).x;
		const float triangleHeight = mm2px(Vec(0.f, TRIANGLE_GLYPH_HEIGHT_MM)).y;
		const float triangleLineWidth = mm2px(Vec(TRIANGLE_GLYPH_LINE_WIDTH_MM, 0.f)).x;
		const Vec lineEnd = centerPx.plus(dir.mult(lineRadius));
		const Vec labelPos = centerPx.plus(dir.mult(labelRadius)).plus(tangent.mult(tangentOffset)).plus(Vec(0.f, labelYOffset)).plus(mirrorOffset);
		const Vec triangleLeft(labelPos.x - 0.5f * triangleWidth, labelPos.y + 0.5f * triangleHeight);
		const Vec trianglePeak(labelPos.x, labelPos.y - 0.5f * triangleHeight);
		const Vec triangleRight(labelPos.x + 0.5f * triangleWidth, labelPos.y + 0.5f * triangleHeight);

		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, centerPx.x, centerPx.y);
		nvgLineTo(args.vg, lineEnd.x, lineEnd.y);
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 255));
		nvgStrokeWidth(args.vg, lineWidth);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgStroke(args.vg);

		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgStrokeWidth(args.vg, triangleLineWidth);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, triangleLeft.x, triangleLeft.y);
		nvgLineTo(args.vg, trianglePeak.x, trianglePeak.y);
		nvgLineTo(args.vg, triangleRight.x, triangleRight.y);
		NVGpaint triangleGradient = nvgLinearGradient(
			args.vg,
			triangleLeft.x,
			triangleLeft.y,
			triangleRight.x,
			triangleRight.y,
			nvgRGBA(255, 184, 0, 255),
			nvgRGBA(220, 94, 30, 255));
		nvgStrokePaint(args.vg, triangleGradient);
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}
};

struct IntegralFluxShapeModeGlyphOverlay : TransparentWidget {
	IntegralFlux* module = nullptr;
	widget::FramebufferWidget* framebuffer = nullptr;
	widget::SvgWidget* svgWidget = nullptr;
	int shapeModeParamId = -1;
	int lastMode = -1;
	std::shared_ptr<window::Svg> mirrorSvg;
	std::shared_ptr<window::Svg> sharkSvg;

	IntegralFluxShapeModeGlyphOverlay(IntegralFlux* module, int shapeModeParamId)
		: module(module)
		, shapeModeParamId(shapeModeParamId)
		, mirrorSvg(visual_assets::loadPluginSvgCached("res/icon/mirror_highlight.svg"))
		, sharkSvg(visual_assets::loadPluginSvgCached("res/icon/shark_highlight.svg")) {}

	IntegralFlux::FunctionShapeMode currentMode(int shapeModeParamId) const {
		if (!module || shapeModeParamId < 0) {
			return IntegralFlux::FUNCTION_SHAPE_MATHS;
		}
		return IntegralFlux::functionShapeModeFromParam(module->params[shapeModeParamId].getValue());
	}

	void step() override {
		TransparentWidget::step();
		const int mode = int(currentMode(shapeModeParamId));
		if (mode == lastMode || !svgWidget || !framebuffer) {
			return;
		}
		lastMode = mode;
		const IntegralFlux::FunctionShapeMode shapeMode = IntegralFlux::functionShapeModeFromStoredInt(lastMode);
		svgWidget->setSvg(shapeMode == IntegralFlux::FUNCTION_SHAPE_SHARK_FIN ? sharkSvg : mirrorSvg);
		svgWidget->box.size = framebuffer->box.size;
		++gIntegralFluxShapeGlyphDirtyRequestsThisFrame;
		framebuffer->setDirty();
	}
};

struct IntegralFluxInactiveShapeModeGlyphs : TransparentWidget {
	static constexpr int SAMPLE_COUNT = 49;
	IntegralFlux* module = nullptr;
	int shapeModeParamId = -1;

	IntegralFluxInactiveShapeModeGlyphs(IntegralFlux* module, int shapeModeParamId)
		: module(module)
		, shapeModeParamId(shapeModeParamId) {
	}

	IntegralFlux::FunctionShapeMode currentMode() const {
		if (!module || shapeModeParamId < 0) {
			return IntegralFlux::FUNCTION_SHAPE_MATHS;
		}
		return IntegralFlux::functionShapeModeFromParam(module->params[shapeModeParamId].getValue());
	}

	void drawSampledCurve(const DrawArgs& args, float x0, float xStep, const float* ySamples) {
		const float sx = box.size.x / 2000.f;
		const float sy = box.size.y / 1800.f;
		nvgBeginPath(args.vg);
		for (int i = 0; i < SAMPLE_COUNT; ++i) {
			const float x = (x0 + xStep * float(i)) * sx;
			const float y = ySamples[i] * sy;
			if (i == 0) {
				nvgMoveTo(args.vg, x, y);
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
		}
		nvgStroke(args.vg);
	}

	void drawGlyphs(const DrawArgs& args, bool drawMirror, bool drawShark) {
		static constexpr float mirrorLeft[SAMPLE_COUNT] = {
			441.3f, 360.0f, 328.9f, 307.9f, 291.6f, 278.0f, 266.3f, 255.8f, 246.4f, 237.7f,
			229.7f, 222.3f, 215.3f, 208.6f, 202.4f, 196.4f, 190.7f, 185.3f, 180.0f, 175.0f,
			170.1f, 165.4f, 160.9f, 156.5f, 151.9f, 156.2f, 160.6f, 165.1f, 169.8f, 174.7f,
			179.7f, 184.9f, 190.4f, 196.1f, 202.0f, 208.2f, 214.8f, 221.8f, 229.2f, 237.2f,
			245.8f, 255.2f, 265.6f, 277.2f, 290.7f, 306.8f, 327.4f, 357.5f, 441.3f
		};
		static constexpr float mirrorRight[SAMPLE_COUNT] = {
			441.3f, 438.6f, 435.9f, 433.1f, 430.3f, 427.4f, 424.4f, 421.3f, 418.0f, 414.5f,
			410.7f, 406.7f, 402.2f, 397.3f, 391.8f, 385.5f, 378.3f, 369.8f, 359.6f, 346.9f,
			330.8f, 309.3f, 279.1f, 232.9f, 151.9f, 232.1f, 278.6f, 309.0f, 330.5f, 346.7f,
			359.4f, 369.7f, 378.2f, 385.4f, 391.7f, 397.2f, 402.2f, 406.6f, 410.7f, 414.4f,
			417.9f, 421.2f, 424.4f, 427.4f, 430.3f, 433.1f, 435.9f, 438.6f, 441.3f
		};
		static constexpr float sharkLeft[SAMPLE_COUNT] = {
			1502.1f, 1499.4f, 1496.7f, 1493.9f, 1491.1f, 1488.2f, 1485.2f, 1482.1f, 1478.8f, 1475.3f,
			1471.5f, 1467.4f, 1463.0f, 1458.1f, 1452.6f, 1446.3f, 1439.1f, 1430.6f, 1420.3f, 1407.7f,
			1391.6f, 1370.1f, 1339.9f, 1293.7f, 1212.7f, 1217.0f, 1221.4f, 1225.9f, 1230.6f, 1235.4f,
			1240.5f, 1245.7f, 1251.1f, 1256.8f, 1262.8f, 1269.0f, 1275.6f, 1282.6f, 1290.0f, 1298.0f,
			1306.6f, 1316.0f, 1326.3f, 1338.0f, 1351.5f, 1367.6f, 1388.1f, 1418.3f, 1502.1f
		};
		static constexpr float sharkRight[SAMPLE_COUNT] = {
			1502.1f, 1420.8f, 1389.6f, 1368.7f, 1352.4f, 1338.8f, 1327.0f, 1316.6f, 1307.1f, 1298.5f,
			1290.5f, 1283.0f, 1276.0f, 1269.4f, 1263.2f, 1257.2f, 1251.5f, 1246.0f, 1240.8f, 1235.8f,
			1230.9f, 1226.2f, 1221.6f, 1217.2f, 1212.7f, 1292.9f, 1339.3f, 1369.7f, 1391.3f, 1407.5f,
			1420.2f, 1430.4f, 1439.0f, 1446.2f, 1452.5f, 1458.0f, 1462.9f, 1467.4f, 1471.4f, 1475.2f,
			1478.7f, 1482.0f, 1485.2f, 1488.2f, 1491.1f, 1493.9f, 1496.6f, 1499.4f, 1502.1f
		};
		static constexpr float leftX = 386.9f;
		static constexpr float rightX = 1053.1f;
		static constexpr float xStep = 10.707f;
		if (drawMirror) {
			drawSampledCurve(args, leftX, xStep, mirrorLeft);
			drawSampledCurve(args, rightX, xStep, mirrorRight);
		}
		if (drawShark) {
			drawSampledCurve(args, leftX, xStep, sharkLeft);
			drawSampledCurve(args, rightX, xStep, sharkRight);
		}
	}

	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxShapeGlyphDrawNsThisFrame, isDragonKingDebugEnabled());

		nvgSave(args.vg);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgStrokeWidth(args.vg, std::max(1.0f, box.size.x * 0.0204f));
		nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 245));
		const bool activeShark = currentMode() == IntegralFlux::FUNCTION_SHAPE_SHARK_FIN;
		drawGlyphs(args, activeShark, !activeShark);
		nvgRestore(args.vg);
	}
};

struct IntegralFluxTimedShapeModeGlyphSvg : widget::SvgWidget {
	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxShapeGlyphDrawNsThisFrame, isDragonKingDebugEnabled());
		widget::SvgWidget::draw(args);
	}
};

// Deprecated old EclipseKnob wrapper, replaced by Eclipse2Knob

struct IntegralFluxEclipse2Knob : Eclipse2Knob {
	void draw(const DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxEclipseDrawNsThisFrame, isDragonKingDebugEnabled());
		Eclipse2Knob::draw(args);
	}
};

template <typename TBase>
struct IntegralFluxTimedApertureLight : TBase {
	void draw(const typename TBase::DrawArgs& args) override {
		IntegralFluxScopedDrawTimer timer(
			gIntegralFluxApertureDrawNsThisFrame, isDragonKingDebugEnabled());
		TBase::draw(args);
	}
};

struct IntegralFluxWidget : ModuleWidget {
	std::vector<IntegralFluxHalo2Knob*> haloKnobs;
	bool forceHaloNanoVg = false;
	float uiStepMsEma = 0.f;
	float uiDrawMsEma = 0.f;
	float gearDrawUsEma = 0.f;
	float eclipseDrawUsEma = 0.f;
	float linearPointDrawUsEma = 0.f;
	float shapeGlyphDrawUsEma = 0.f;
	float plasmaSwitchDrawUsEma = 0.f;
	debug_terminal::UiTimingRangeAccumulator uiStepUsRange;
	debug_terminal::UiTimingRangeAccumulator uiDrawUsRange;
	debug_terminal::UiTimingRangeAccumulator apertureDrawUsRange;
	std::ofstream drawLogFile;
	std::string drawLogPath;
	bool drawLogActive = false;
	uint64_t drawLogRowCounter = 0u;
	IntegralFluxPreviewEdgeInteraction ch1EdgeInteraction;
	IntegralFluxPreviewEdgeInteraction ch4EdgeInteraction;
	IntegralFluxKnobTooltipState centralTooltipState;
	visual_assets::FractalGlassOverlay* nautiloidGlass = nullptr;

	struct DrawLogRow {
		uint64_t row = 0u;
		int moduleId = -1;
		uint32_t instanceId = 0u;
		int previewRenderMode = 0;
		int previewTracerEnabled = 0;
		int previewTracerMode = 0;
		float totalDrawUs = 0.f;
		float moduleWidgetDrawUs = 0.f;
		float debugSubmitUs = 0.f;
		float debugOverlayUs = 0.f;
		float gearDrawUs = 0.f;
		float eclipseDrawUs = 0.f;
		float eclipseShadowUs = 0.f;
		uint64_t eclipseShadowCount = 0u;
		float apertureDrawUs = 0.f;
		float linearPointDrawUs = 0.f;
		float shapeGlyphDrawUs = 0.f;
		float plasmaSwitchDrawUs = 0.f;
		float previewDrawUs = 0.f;
		float ch1PreviewDrawUs = 0.f;
		float ch4PreviewDrawUs = 0.f;
		float previewFramebufferUs = 0.f;
		float ch1PreviewFramebufferUs = 0.f;
		float ch4PreviewFramebufferUs = 0.f;
		float plasmaSwitchShadowUs = 0.f;
		float plasmaSwitchImageEnsureUs = 0.f;
		float plasmaSwitchImagePaintUs = 0.f;
		float plasmaSwitchBodyUs = 0.f;
		float plasmaSwitchOrbUs = 0.f;
		uint32_t plasmaSwitchImageCreates = 0u;
		uint32_t plasmaSwitchImageFallbacks = 0u;
		uint32_t plasmaSwitchContextResets = 0u;
		uint32_t previewDirtyRequests = 0u;
		uint32_t previewPointRebuilds = 0u;
		uint32_t previewTracerCaptures = 0u;
		uint32_t linearPointDirtyRequests = 0u;
		uint32_t shapeGlyphDirtyRequests = 0u;
		uint32_t haloDirtyDrawCount = 0u;
		uint32_t haloActiveDrawCount = 0u;
		uint32_t haloDraggingDrawCount = 0u;
		int haloNanoVgForced = 0;
		float haloGlSurfaceFramebufferUs = 0.f;
		float haloNanoVgSurfaceDrawUs = 0.f;
		float haloCenterFramebufferUs = 0.f;
		float haloCapReflectionFramebufferUs = 0.f;
		uint32_t haloGlSurfaceFramebufferDraws = 0u;
		uint32_t haloNanoVgSurfaceDraws = 0u;
		uint32_t haloCenterFramebufferDraws = 0u;
		uint32_t haloCapReflectionFramebufferDraws = 0u;
		float uiStepEmaUs = 0.f;
		float uiDrawEmaUs = 0.f;
	};

	static std::string drawLogRootPath() {
		return system::join(asset::user(), "Leviathan/IntegralFlux");
	}

	static std::string drawLogDateTimeStamp() {
		std::time_t now = std::time(nullptr);
		std::tm tm {};
#if defined(_WIN32)
		localtime_s(&tm, &now);
#else
		localtime_r(&now, &tm);
#endif
		std::ostringstream out;
		out << std::put_time(&tm, "%Y%m%d_%H%M%S");
		return out.str();
	}

	void stopDrawLog() {
		if (drawLogFile.is_open()) {
			drawLogFile.close();
		}
		drawLogPath.clear();
		drawLogActive = false;
		drawLogRowCounter = 0u;
	}

	void syncDrawLog(bool enabled, uint32_t instanceId) {
		if (!enabled) {
			stopDrawLog();
			return;
		}
		if (drawLogActive && drawLogFile.is_open()) {
			return;
		}
		system::createDirectories(drawLogRootPath());
		static uint32_t openSequence = 0u;
		drawLogPath = system::join(
			drawLogRootPath(),
			"integral_flux_draw_" + std::to_string(instanceId) + "_" + drawLogDateTimeStamp() + "_" +
				std::to_string(openSequence++) + ".csv");
		drawLogFile.open(drawLogPath.c_str(), std::ios::out | std::ios::trunc);
		if (!drawLogFile.is_open()) {
			WARN("Integral Flux failed to open draw log CSV: %s", drawLogPath.c_str());
			drawLogPath.clear();
			drawLogActive = false;
			return;
		}
		drawLogActive = true;
		drawLogRowCounter = 0u;
		drawLogFile << std::fixed << std::setprecision(3);
		drawLogFile
			<< "row,module_id,instance_id,preview_render_mode,preview_tracer_enabled,preview_tracer_mode,"
			<< "total_draw_us,module_widget_draw_us,debug_submit_us,debug_overlay_us,"
			<< "gear_draw_us,eclipse_draw_us,eclipse_shadow_us,eclipse_shadow_count,aperture_draw_us,"
			<< "linear_point_draw_us,shape_glyph_draw_us,plasma_switch_draw_us,"
			<< "preview_draw_us,ch1_preview_draw_us,ch4_preview_draw_us,preview_framebuffer_us,"
			<< "ch1_preview_framebuffer_us,ch4_preview_framebuffer_us,"
			<< "plasma_switch_shadow_us,plasma_switch_image_ensure_us,plasma_switch_image_paint_us,"
			<< "plasma_switch_body_us,plasma_switch_orb_us,plasma_switch_image_creates,"
			<< "plasma_switch_image_fallbacks,plasma_switch_context_resets,preview_dirty_requests,"
			<< "preview_point_rebuilds,preview_tracer_captures,linear_point_dirty_requests,"
			<< "shape_glyph_dirty_requests,halo_dirty_draw_count,halo_active_draw_count,halo_dragging_draw_count,"
			<< "halo_nanovg_forced,halo_gl_surface_framebuffer_us,halo_nanovg_surface_draw_us,"
			<< "halo_center_framebuffer_us,halo_cap_reflection_framebuffer_us,"
			<< "halo_gl_surface_framebuffer_draws,halo_nanovg_surface_draws,halo_center_framebuffer_draws,"
			<< "halo_cap_reflection_framebuffer_draws,"
			<< "ui_step_ema_us,ui_draw_ema_us\n";
	}

	void writeDrawLogRow(const DrawLogRow& row) {
		if (!drawLogActive || !drawLogFile.is_open()) {
			return;
		}
		drawLogFile
			<< row.row << ','
			<< row.moduleId << ','
			<< row.instanceId << ','
			<< row.previewRenderMode << ','
			<< row.previewTracerEnabled << ','
			<< row.previewTracerMode << ','
			<< row.totalDrawUs << ','
			<< row.moduleWidgetDrawUs << ','
			<< row.debugSubmitUs << ','
			<< row.debugOverlayUs << ','
			<< row.gearDrawUs << ','
			<< row.eclipseDrawUs << ','
			<< row.eclipseShadowUs << ','
			<< row.eclipseShadowCount << ','
			<< row.apertureDrawUs << ','
			<< row.linearPointDrawUs << ','
			<< row.shapeGlyphDrawUs << ','
			<< row.plasmaSwitchDrawUs << ','
			<< row.previewDrawUs << ','
			<< row.ch1PreviewDrawUs << ','
			<< row.ch4PreviewDrawUs << ','
			<< row.previewFramebufferUs << ','
			<< row.ch1PreviewFramebufferUs << ','
			<< row.ch4PreviewFramebufferUs << ','
			<< row.plasmaSwitchShadowUs << ','
			<< row.plasmaSwitchImageEnsureUs << ','
			<< row.plasmaSwitchImagePaintUs << ','
			<< row.plasmaSwitchBodyUs << ','
			<< row.plasmaSwitchOrbUs << ','
			<< row.plasmaSwitchImageCreates << ','
			<< row.plasmaSwitchImageFallbacks << ','
			<< row.plasmaSwitchContextResets << ','
			<< row.previewDirtyRequests << ','
			<< row.previewPointRebuilds << ','
			<< row.previewTracerCaptures << ','
			<< row.linearPointDirtyRequests << ','
			<< row.shapeGlyphDirtyRequests << ','
			<< row.haloDirtyDrawCount << ','
			<< row.haloActiveDrawCount << ','
			<< row.haloDraggingDrawCount << ','
			<< row.haloNanoVgForced << ','
			<< row.haloGlSurfaceFramebufferUs << ','
			<< row.haloNanoVgSurfaceDrawUs << ','
			<< row.haloCenterFramebufferUs << ','
			<< row.haloCapReflectionFramebufferUs << ','
			<< row.haloGlSurfaceFramebufferDraws << ','
			<< row.haloNanoVgSurfaceDraws << ','
			<< row.haloCenterFramebufferDraws << ','
			<< row.haloCapReflectionFramebufferDraws << ','
			<< row.uiStepEmaUs << ','
			<< row.uiDrawEmaUs << '\n';
		if ((row.row & 31u) == 0u) {
			drawLogFile.flush();
		}
	}

	static float consumeReductionAverage(std::atomic<uint64_t>& total, std::atomic<uint64_t>& samples) {
		const uint64_t totalValue = total.exchange(0u, std::memory_order_acq_rel);
		const uint64_t sampleCount = samples.exchange(0u, std::memory_order_acq_rel);
		return sampleCount > 0u ? float(double(totalValue) / double(sampleCount)) : 0.f;
	}

	void step() override {
		using PerfClock = std::chrono::steady_clock;
		const bool measurePerf = isDragonKingDebugEnabled();
		const PerfClock::time_point stepStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		if (nautiloidGlass) {
			Nautiloid* naut = leftNautiloidForIntegralFlux(static_cast<IntegralFlux*>(module));
			if (naut) {
				iris::NautiloidFractalSourceParams params;
				params.mode = naut->fractalMode;
				params.zoom = naut->fractalZoom;
				params.centerX = naut->fractalCenterX;
				params.centerY = naut->fractalCenterY;
				nautiloidGlass->setLiveParams(&params);
			} else {
				nautiloidGlass->setLiveParams(nullptr);
			}
		}
		ModuleWidget::step();
		if (measurePerf) {
			const float stepMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - stepStart).count()) * 1e-6f;
			uiStepMsEma = (uiStepMsEma > 0.f) ? (uiStepMsEma + (stepMs - uiStepMsEma) * 0.18f) : stepMs;
			uiStepUsRange.add(stepMs * 1000.f);
		}
	}

	~IntegralFluxWidget() override {
		stopDrawLog();
	}

	IntegralFluxWidget(IntegralFlux* module) {
		setModule(module);
		PreviewBuildLogTimer previewBuildTimer("IntegralFlux", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/flux.panel.svg");
		const std::string& panelBasePath = splitPanel.panelPath();
		splitPanel.addLabels("res/flux.labels.svg");
		nautiloidGlass = visual_assets::addFractalGlassOverlay(
			this, panelBasePath, splitPanel.panelSurfaceEffectWidget());
		{
			math::Rect dragonRectMm;
			if (!panel_svg::loadRectFromSvgMm(panelBasePath, "DRAGON_RENDER_AREA", &dragonRectMm)) {
				dragonRectMm.pos = Vec(42.1381f, 10.928f);
				dragonRectMm.size = Vec(17.4213f, 35.5849f);
			}
			IntegralFluxFittedSvgWidget* dragon = new IntegralFluxFittedSvgWidget();
			dragon->setSvg(visual_assets::loadPluginSvgCached("res/icon/Leviathan_Optimized.svg"));
			widget::FramebufferWidget* dragonFb = new widget::FramebufferWidget();
			dragonFb->box.pos = mm2px(dragonRectMm.pos);
			dragonFb->box.size = mm2px(dragonRectMm.size);
			dragonFb->dirtyOnSubpixelChange = false;
			dragon->box.size = dragonFb->box.size;
			dragonFb->addChild(dragon);
			addChild(dragonFb);
		}
		previewBuildTimer.markPanelDone();
		{
			const Vec logoPosMm(27.8f, 120.25f);
			const Vec logoSizeMm(46.0f, 7.85f);
			Widget* logo = visual_assets::createAspectFitRasterImageWidget(
				"res/icon/Leviathan_Logo.png",
				math::Rect(logoPosMm, logoSizeMm));
			addChild(logo);

			IntegralFluxLogoCrystalButton* crystalButton =
				new IntegralFluxLogoCrystalButton();
			crystalButton->previewProgressionPhase =
				splitPanel.previewProgressionPhase();
			crystalButton->box.pos = mm2px(Vec(48.96f, 125.30f));
			crystalButton->box.size = mm2px(Vec(5.0f, 2.5f));
			addChild(crystalButton);
		}

        // use LeviathanHaloKnob2 for surge/sink and curve shape knobs
        // use SmallAperture LEDs for the indicator lights
        // use EclipseKnob for the attenuverter knobs
        // use TL1105 for the cycle buttons

		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Vec cycle1ButtonPos(31.875f, 20.938f);
		Vec cycle4ButtonPos(69.552f, 20.938f);
		Vec rise1KnobPos(33.755f, 36.293f);
		Vec rise4KnobPos(67.638f, 36.293f);
		Vec fall1KnobPos(42.007f, 53.079f);
		Vec fall4KnobPos(59.185f, 53.079f);
		Vec linLog1KnobPos(13.975f, 50.526f);
		Vec linLog4KnobPos(91.716f, 50.526f);
		Vec shapeMode1SwitchPos(13.975f, 36.8f);
		Vec shapeMode4SwitchPos(91.716f, 36.8f);
		Vec attenuate1KnobPos(25.494f, 86.446f);
		Vec attenuate2KnobPos(42.542f, 86.446f);
		Vec attenuate3KnobPos(59.585f, 86.446f);
		Vec attenuate4KnobPos(75.931f, 86.446f);
		Vec input1Pos(9.947f, 15.354f);
		Vec input1TrigPos(20.911f, 15.354f);
		Vec input4TrigPos(80.217f, 15.354f);
		Vec input4Pos(91.181f, 15.354f);
		Vec ch1CycleCvPos(40.049f, 20.838f);
		Vec ch4CycleCvPos(61.179f, 20.838f);
		Vec ch1RiseCvPos(21.683f, 36.416f);
		Vec ch4RiseCvPos(79.81f, 36.216f);
		Vec ch1BothCvPos(26.633f, 50.27f);
		Vec ch4BothCvPos(74.56f, 50.07f);
		Vec ch1FallCvPos(32.704f, 63.263f);
		Vec ch4FallCvPos(69.189f, 63.263f);
		Vec input2Pos(42.543f, 76.377f);
		Vec input3Pos(59.585f, 76.377f);
		Vec eor1OutputPos(10.037f, 96.946f);
		Vec out1OutputPos(25.295f, 96.915f);
		Vec out2OutputPos(42.343f, 96.915f);
		Vec out3OutputPos(59.486f, 96.915f);
		Vec out4OutputPos(75.832f, 96.915f);
		Vec eoc4OutputPos(91.281f, 96.915f);
		Vec ch1UnityOutputPos(10.047f, 110.682f);
		Vec orOutputPos(33.652f, 110.882f);
		Vec sumOutputPos(50.714f, 110.882f);
		Vec invOutputPos(67.975f, 110.882f);
		Vec ch4UnityOutputPos(91.281f, 110.682f);
		Vec cycle1LightPos(31.875f, 14.855f);
		Vec cycle4LightPos(69.353f, 14.855f);
		Vec eor1LightPos(16.537f, 96.76f);
		Vec eoc4LightPos(84.603f, 96.716f);
		Vec unity1LightPos(16.547f, 110.499f);
		Vec unity4LightPos(84.731f, 110.599f);
		Vec orLightPos(42.374f, 110.758f);
		Vec invLightPos(59.554f, 110.758f);

		auto applyPointOverride = [&](const char* elementId, Vec* outPos) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelBasePath, elementId, &pointMm)) {
				*outPos = pointMm;
			}
		};

		applyPointOverride("CYCLE_1", &cycle1ButtonPos);
		applyPointOverride("CYCLE_4", &cycle4ButtonPos);
		applyPointOverride("RISE_1", &rise1KnobPos);
		applyPointOverride("RISE_4", &rise4KnobPos);
		applyPointOverride("FALL_1", &fall1KnobPos);
		applyPointOverride("FALL_4", &fall4KnobPos);
		applyPointOverride("LIN_LOG_1", &linLog1KnobPos);
		applyPointOverride("LIN_LOG_4", &linLog4KnobPos);
		applyPointOverride("SHAPE_MODE_1", &shapeMode1SwitchPos);
		applyPointOverride("SHAPE_MODE_4", &shapeMode4SwitchPos);
		applyPointOverride("ATTENUATE_1", &attenuate1KnobPos);
		applyPointOverride("ATTENUATE_2", &attenuate2KnobPos);
		applyPointOverride("ATTENUATE_3", &attenuate3KnobPos);
		applyPointOverride("ATTENUATE_4", &attenuate4KnobPos);
		applyPointOverride("INPUT_1", &input1Pos);
		applyPointOverride("INPUT_1_TRIG", &input1TrigPos);
		applyPointOverride("INPUT_4_TRIG", &input4TrigPos);
		applyPointOverride("INPUT_4", &input4Pos);
		applyPointOverride("CH1_CYCLE_CV", &ch1CycleCvPos);
		applyPointOverride("CH4_CYCLE_CV", &ch4CycleCvPos);
		applyPointOverride("CH1_RISE_CV", &ch1RiseCvPos);
		applyPointOverride("CH4_RISE_CV", &ch4RiseCvPos);
		applyPointOverride("CH1_BOTH_CV", &ch1BothCvPos);
		applyPointOverride("CH4_BOTH_CV", &ch4BothCvPos);
		applyPointOverride("CH1_FALL_CV", &ch1FallCvPos);
		applyPointOverride("CH4_FALL_CV", &ch4FallCvPos);
		applyPointOverride("INPUT_2", &input2Pos);
		applyPointOverride("INPUT_3", &input3Pos);
			applyPointOverride("EOR_1", &eor1OutputPos);
			applyPointOverride("OUT_1", &out1OutputPos);
			applyPointOverride("OUT_2", &out2OutputPos);
			applyPointOverride("OUT_3", &out3OutputPos);
			applyPointOverride("OUT_4", &out4OutputPos);
			applyPointOverride("EOC_4", &eoc4OutputPos);
			applyPointOverride("CH_1_Unity", &ch1UnityOutputPos);
			applyPointOverride("OR_OUT", &orOutputPos);
			applyPointOverride("SUM_OUT", &sumOutputPos);
			applyPointOverride("INV_OUT", &invOutputPos);
			applyPointOverride("CH_4_Unity", &ch4UnityOutputPos);
			applyPointOverride("CYCLE_1_LED", &cycle1LightPos);
			applyPointOverride("CYCLE_4_LED", &cycle4LightPos);
			applyPointOverride("EoR_CH_1", &eor1LightPos);
			applyPointOverride("EoC_CH_4", &eoc4LightPos);
			applyPointOverride("Light_Unity_1", &unity1LightPos);
			applyPointOverride("Light_Unity_4", &unity4LightPos);
			applyPointOverride("OR_LED", &orLightPos);
			applyPointOverride("INV_LED", &invLightPos);
		previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelBasePath));
		previewBuildTimer.markAnchorsDone();

		std::vector<widget::FramebufferWidget*> pendingShapeGlyphFramebuffers;
		{
			auto addCurveModeOverlay = [&](Vec centerMm, int shapeModeParamId, bool ch4) {
				const Vec centerPx = mm2px(centerMm);
				widget::FramebufferWidget* linearPointFb = new widget::FramebufferWidget();
				linearPointFb->box.pos = centerPx.minus(mm2px(Vec(13.5f, 13.5f)));
				linearPointFb->box.size = mm2px(Vec(27.f, 27.f));
				linearPointFb->dirtyOnSubpixelChange = false;

				IntegralFluxLinearPointOverlay* linearPoint = new IntegralFluxLinearPointOverlay(
					module, shapeModeParamId, centerPx.minus(linearPointFb->box.pos));
				linearPoint->box.pos = Vec(0.f, 0.f);
				linearPoint->box.size = linearPointFb->box.size;
				linearPoint->framebuffer = linearPointFb;
				linearPointFb->addChild(linearPoint);
				addChild(linearPointFb);

				widget::FramebufferWidget* shapeGlyphFb = new widget::FramebufferWidget();
				shapeGlyphFb->box.pos = mm2px(ch4 ? Vec(81.585f, 22.f) : Vec(0.f, 22.f));
				shapeGlyphFb->box.size = mm2px(Vec(20.f, 18.f));
				shapeGlyphFb->dirtyOnSubpixelChange = false;

				IntegralFluxInactiveShapeModeGlyphs* inactiveShapeGlyphs = new IntegralFluxInactiveShapeModeGlyphs(
					module, shapeModeParamId);
				inactiveShapeGlyphs->box.size = shapeGlyphFb->box.size;
				shapeGlyphFb->addChild(inactiveShapeGlyphs);

				IntegralFluxTimedShapeModeGlyphSvg* shapeGlyphSvg = new IntegralFluxTimedShapeModeGlyphSvg();
				shapeGlyphSvg->box.size = shapeGlyphFb->box.size;
				shapeGlyphFb->addChild(shapeGlyphSvg);
				pendingShapeGlyphFramebuffers.push_back(shapeGlyphFb);

				IntegralFluxShapeModeGlyphOverlay* shapeGlyphs = new IntegralFluxShapeModeGlyphOverlay(
					module, shapeModeParamId);
				shapeGlyphs->box.pos = Vec(0.f, 0.f);
				shapeGlyphs->box.size = box.size;
				shapeGlyphs->framebuffer = shapeGlyphFb;
				shapeGlyphs->svgWidget = shapeGlyphSvg;
				addChild(shapeGlyphs);
			};
			addCurveModeOverlay(linLog1KnobPos, IntegralFlux::SHAPE_MODE_1_PARAM, false);
			addCurveModeOverlay(linLog4KnobPos, IntegralFlux::SHAPE_MODE_4_PARAM, true);
		}

		addParam(createParamCentered<LoopGoldButton>(mm2px(cycle1ButtonPos), module, IntegralFlux::CYCLE_1_PARAM));
		addParam(createParamCentered<LoopGoldButton>(mm2px(cycle4ButtonPos), module, IntegralFlux::CYCLE_4_PARAM));

		auto addEdgeKnob = [&](Vec posMm, int paramId, IntegralFluxPreviewEdgeInteraction* interaction,
			IntegralFluxHalo2Knob::PreviewEdge edge, int channel, const char* tooltipLabel) {
			IntegralFluxHalo2Knob* knob = createParamCentered<IntegralFluxHalo2Knob>(mm2px(posMm), module, paramId);
			knob->setPreviewInteraction(interaction, edge);
			knob->setCentralTooltip(&centralTooltipState, channel, tooltipLabel, paramId);
			knob->setSuppressRackTooltip(true);
			knob->setForceNanoVgLedRenderer(forceHaloNanoVg);
			haloKnobs.push_back(knob);
			addParam(knob);
		};
		auto addCurveKnob = [&](Vec posMm, int paramId, IntegralFluxPreviewEdgeInteraction* interaction, int channel) {
			IntegralFluxCurveHalo2Knob* knob = createParamCentered<IntegralFluxCurveHalo2Knob>(mm2px(posMm), module, paramId);
			knob->setPreviewInteraction(interaction, IntegralFluxHalo2Knob::PREVIEW_CURVE);
			knob->setCentralTooltip(&centralTooltipState, channel, "Curve", paramId);
			knob->setSuppressRackTooltip(true);
			knob->setForceNanoVgLedRenderer(forceHaloNanoVg);
			haloKnobs.push_back(knob);
			addParam(knob);
		};
		addEdgeKnob(rise1KnobPos, IntegralFlux::RISE_1_PARAM, &ch1EdgeInteraction, IntegralFluxHalo2Knob::PREVIEW_EDGE_RISE, 1, "Surge");
		addEdgeKnob(rise4KnobPos, IntegralFlux::RISE_4_PARAM, &ch4EdgeInteraction, IntegralFluxHalo2Knob::PREVIEW_EDGE_RISE, 4, "Surge");
		addEdgeKnob(fall1KnobPos, IntegralFlux::FALL_1_PARAM, &ch1EdgeInteraction, IntegralFluxHalo2Knob::PREVIEW_EDGE_FALL, 1, "Sink");
		addEdgeKnob(fall4KnobPos, IntegralFlux::FALL_4_PARAM, &ch4EdgeInteraction, IntegralFluxHalo2Knob::PREVIEW_EDGE_FALL, 4, "Sink");
		addCurveKnob(linLog1KnobPos, IntegralFlux::LIN_LOG_1_PARAM, &ch1EdgeInteraction, 1);
		addCurveKnob(linLog4KnobPos, IntegralFlux::LIN_LOG_4_PARAM, &ch4EdgeInteraction, 4);
		addParam(createParamCentered<IntegralFluxPlasmaSwitch>(mm2px(shapeMode1SwitchPos), module, IntegralFlux::SHAPE_MODE_1_PARAM));
		addParam(createParamCentered<IntegralFluxPlasmaSwitch>(mm2px(shapeMode4SwitchPos), module, IntegralFlux::SHAPE_MODE_4_PARAM));
		{
			WavePreviewWidget* ch1Preview = new WavePreviewWidget(module, 1);
			ch1Preview->edgeInteraction = &ch1EdgeInteraction;
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelBasePath, "CH1_PREVIEW", &previewRectMm)) {
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewRectMm));
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				ch1Preview->box.pos = mm2px(previewRectMm.pos);
				ch1Preview->box.size = mm2px(previewRectMm.size);
			}
			else {
				math::Rect previewFallbackMm(Vec(3.75998355f, 68.96602539f), Vec(20.78393382f, 11.24561948f));
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewFallbackMm));
				previewFallbackMm = insetRectMm(previewFallbackMm, 0.2f);
				ch1Preview->box.pos = mm2px(previewFallbackMm.pos);
				ch1Preview->box.size = mm2px(previewFallbackMm.size);
			}
			widget::FramebufferWidget* previewSurface = preview_surface::createCachedOpaqueGrid(ch1Preview->box.size);
			previewSurface->box.pos = ch1Preview->box.pos;
			addChild(previewSurface);
			addChild(ch1Preview);
		}
		{
			WavePreviewWidget* ch4Preview = new WavePreviewWidget(module, 4);
			ch4Preview->edgeInteraction = &ch4EdgeInteraction;
			math::Rect previewRectMm;
			if (panel_svg::loadRectFromSvgMm(panelBasePath, "CH4_PREVIEW", &previewRectMm)) {
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewRectMm));
				previewRectMm = insetRectMm(previewRectMm, 0.2f);
				ch4Preview->box.pos = mm2px(previewRectMm.pos);
				ch4Preview->box.size = mm2px(previewRectMm.size);
			}
			else {
				math::Rect previewFallbackMm(Vec(77.52500000f, 68.96600100f), Vec(20.78393300f, 11.24562000f));
				addChild(visual_assets::createPreviewFrameEnhancementWidget(previewFallbackMm));
				previewFallbackMm = insetRectMm(previewFallbackMm, 0.2f);
				ch4Preview->box.pos = mm2px(previewFallbackMm.pos);
				ch4Preview->box.size = mm2px(previewFallbackMm.size);
			}
			widget::FramebufferWidget* previewSurface = preview_surface::createCachedOpaqueGrid(ch4Preview->box.size);
			previewSurface->box.pos = ch4Preview->box.pos;
			addChild(previewSurface);
			addChild(ch4Preview);
		}

		auto addBipolarEclipse2Knob = [&](Vec posMm, int paramId) {
			IntegralFluxEclipse2Knob* knob = createParamCentered<IntegralFluxEclipse2Knob>(mm2px(posMm), module, paramId);
			knob->setProgressRingBipolar(true);
			addParam(knob);
		};
		addBipolarEclipse2Knob(attenuate1KnobPos, IntegralFlux::ATTENUATE_1_PARAM);
		addBipolarEclipse2Knob(attenuate2KnobPos, IntegralFlux::ATTENUATE_2_PARAM);
		addBipolarEclipse2Knob(attenuate3KnobPos, IntegralFlux::ATTENUATE_3_PARAM);
		addBipolarEclipse2Knob(attenuate4KnobPos, IntegralFlux::ATTENUATE_4_PARAM);

		addInput(createInputCentered<Magitek2InputJack>(mm2px(input1Pos), module, IntegralFlux::INPUT_1_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input1TrigPos), module, IntegralFlux::INPUT_1_TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input4TrigPos), module, IntegralFlux::INPUT_4_TRIG_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input4Pos), module, IntegralFlux::INPUT_4_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1CycleCvPos), module, IntegralFlux::CH1_CYCLE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4CycleCvPos), module, IntegralFlux::CH4_CYCLE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1RiseCvPos), module, IntegralFlux::CH1_RISE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4RiseCvPos), module, IntegralFlux::CH4_RISE_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1BothCvPos), module, IntegralFlux::CH1_BOTH_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4BothCvPos), module, IntegralFlux::CH4_BOTH_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch1FallCvPos), module, IntegralFlux::CH1_FALL_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(ch4FallCvPos), module, IntegralFlux::CH4_FALL_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input2Pos), module, IntegralFlux::INPUT_2_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(input3Pos), module, IntegralFlux::INPUT_3_INPUT));

		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eor1OutputPos), module, IntegralFlux::EOR_1_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out1OutputPos), module, IntegralFlux::OUT_1_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out2OutputPos), module, IntegralFlux::OUT_2_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out3OutputPos), module, IntegralFlux::OUT_3_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(out4OutputPos), module, IntegralFlux::OUT_4_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(eoc4OutputPos), module, IntegralFlux::EOC_4_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(ch1UnityOutputPos), module, IntegralFlux::CH_1_UNITY_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(orOutputPos), module, IntegralFlux::OR_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(sumOutputPos), module, IntegralFlux::SUM_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(invOutputPos), module, IntegralFlux::INV_OUT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(ch4UnityOutputPos), module, IntegralFlux::CH_4_UNITY_OUTPUT));

		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(cycle1LightPos), module, IntegralFlux::CYCLE_1_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(cycle4LightPos), module, IntegralFlux::CYCLE_4_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(eor1LightPos), module, IntegralFlux::EOR_CH_1_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<AmberApertureLight>>>(mm2px(eoc4LightPos), module, IntegralFlux::EOC_CH_4_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(unity1LightPos), module, IntegralFlux::LIGHT_UNITY_1_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(unity4LightPos), module, IntegralFlux::LIGHT_UNITY_4_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<MagentaApertureLight>>>(mm2px(orLightPos), module, IntegralFlux::OR_LED_LIGHT));
		addChild(createLightCentered<IntegralFluxTimedApertureLight<SmallAperture<GreenApertureLight>>>(mm2px(invLightPos), module, IntegralFlux::INV_LED_LIGHT));

		for (widget::FramebufferWidget* shapeGlyphFb : pendingShapeGlyphFramebuffers) {
			addChild(shapeGlyphFb);
		}

		IntegralFluxCentralTooltipOverlay* centralTooltip = new IntegralFluxCentralTooltipOverlay();
		centralTooltip->box.size = mm2px(Vec(32.f, 7.6f));
		centralTooltip->box.pos = Vec(box.size.x * 0.5f - centralTooltip->box.size.x * 0.5f,
			mm2px(20.84f) - centralTooltip->box.size.y * 0.5f);
		centralTooltip->module = module;
		centralTooltip->state = &centralTooltipState;
		addChild(centralTooltip);
	}

	void draw(const DrawArgs& args) override {
		using PerfClock = std::chrono::steady_clock;
		IntegralFlux* flux = static_cast<IntegralFlux*>(module);
		const uint32_t debugInstanceId = flux ? flux->debugInstanceIdForUi() : 0u;
		const bool logDraw = flux && isDragonKingDebugEnabled() && isIntegralFluxDrawLoggingEnabled();
		syncDrawLog(logDraw, debugInstanceId);
		const bool measurePerf = isDragonKingDebugEnabled();
		if (measurePerf) {
			gIntegralFluxGearDrawNsThisFrame = 0u;
			gIntegralFluxEclipseDrawNsThisFrame = 0u;
			gIntegralFluxApertureDrawNsThisFrame = 0u;
			gIntegralFluxLinearPointDrawNsThisFrame = 0u;
			gIntegralFluxShapeGlyphDrawNsThisFrame = 0u;
			gIntegralFluxPlasmaSwitchDrawNsThisFrame = 0u;
			gIntegralFluxPreviewDrawNsThisFrame = 0u;
			for (uint64_t& value : gIntegralFluxPreviewDrawNsByChannel) {
				value = 0u;
			}
			gIntegralFluxPreviewFramebufferNsThisFrame = 0u;
			for (uint64_t& value : gIntegralFluxPreviewFramebufferNsByChannel) {
				value = 0u;
			}
			gIntegralFluxHaloDirtyDrawCountThisFrame = 0u;
			gIntegralFluxHaloActiveDrawCountThisFrame = 0u;
			gIntegralFluxHaloDraggingDrawCountThisFrame = 0u;
			resetPlasmaSwitchDrawMetrics();
			visual_assets::resetEclipseShadowDrawMetrics();
			visual_assets::resetHaloKnob2DrawMetrics();
		}
		DrawLogRow logRow;
		const PerfClock::time_point totalStart = logDraw ? PerfClock::now() : PerfClock::time_point();
		const PerfClock::time_point perfStart = measurePerf ? PerfClock::now() : PerfClock::time_point();
		ModuleWidget::draw(args);
		const PerfClock::time_point afterModuleDraw = (measurePerf || logDraw) ? PerfClock::now() : PerfClock::time_point();
		if (!flux) {
			return;
		}
		if (measurePerf) {
			const float drawMs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				afterModuleDraw - perfStart).count()) * 1e-6f;
			uiDrawMsEma = (uiDrawMsEma > 0.f) ? (uiDrawMsEma + (drawMs - uiDrawMsEma) * 0.18f) : drawMs;
			uiDrawUsRange.add(drawMs * 1000.f);
			const float gearDrawUs = float(gIntegralFluxGearDrawNsThisFrame) * 1e-3f;
			gearDrawUsEma = (gearDrawUsEma > 0.f) ? (gearDrawUsEma + (gearDrawUs - gearDrawUsEma) * 0.18f) : gearDrawUs;
			const float eclipseDrawUs = float(gIntegralFluxEclipseDrawNsThisFrame) * 1e-3f;
			eclipseDrawUsEma = (eclipseDrawUsEma > 0.f) ? (eclipseDrawUsEma + (eclipseDrawUs - eclipseDrawUsEma) * 0.18f) : eclipseDrawUs;
			const float linearPointDrawUs = float(gIntegralFluxLinearPointDrawNsThisFrame) * 1e-3f;
			linearPointDrawUsEma = (linearPointDrawUsEma > 0.f) ? (linearPointDrawUsEma + (linearPointDrawUs - linearPointDrawUsEma) * 0.18f) : linearPointDrawUs;
			const float shapeGlyphDrawUs = float(gIntegralFluxShapeGlyphDrawNsThisFrame) * 1e-3f;
			shapeGlyphDrawUsEma = (shapeGlyphDrawUsEma > 0.f) ? (shapeGlyphDrawUsEma + (shapeGlyphDrawUs - shapeGlyphDrawUsEma) * 0.18f) : shapeGlyphDrawUs;
			const float plasmaSwitchDrawUs = float(gIntegralFluxPlasmaSwitchDrawNsThisFrame) * 1e-3f;
			plasmaSwitchDrawUsEma = (plasmaSwitchDrawUsEma > 0.f) ? (plasmaSwitchDrawUsEma + (plasmaSwitchDrawUs - plasmaSwitchDrawUsEma) * 0.18f) : plasmaSwitchDrawUs;
			apertureDrawUsRange.add(float(gIntegralFluxApertureDrawNsThisFrame) * 1e-3f);
			const float uiMs = std::max(0.f, uiStepMsEma) + std::max(0.f, uiDrawMsEma);
			flux->setPerfUiRenderMs(uiMs);
		}

		float debugSubmitUs = 0.f;
		float debugOverlayUs = 0.f;
		if (measurePerf) {
			const PerfClock::time_point submitStart = logDraw ? PerfClock::now() : PerfClock::time_point();
			double nowSec = system::getTime();
			double& lastSubmitSec = gIntegralFluxDebugTerminalLastSubmitSec[debugInstanceId];
			if (lastSubmitSec <= 0.0 || (nowSec - lastSubmitSec) >= kIntegralFluxDebugTerminalSubmitIntervalSec) {
				lastSubmitSec = nowSec;
				flux->resetAudioPerfSumsForUi();
				const float ch1CurvePointsReducedAvg = flux->consumeCurveReductionAverageForUi(1);
				const float ch1TracerExtraPointsReducedAvg = flux->consumeTracerReductionAverageForUi(1);
				debug_terminal::submitIntegralFluxMetrics(
					debugInstanceId,
					flux->consumeAudioProcessTimingForUi(),
					uiStepUsRange.consume(),
					uiDrawUsRange.consume(),
					apertureDrawUsRange.consume(),
					gearDrawUsEma,
					eclipseDrawUsEma,
					linearPointDrawUsEma,
					shapeGlyphDrawUsEma,
					ch1CurvePointsReducedAvg,
					ch1TracerExtraPointsReducedAvg);
			}
			if (logDraw) {
				debugSubmitUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
					PerfClock::now() - submitStart).count()) * 1e-3f;
			}
			const PerfClock::time_point overlayStart = logDraw ? PerfClock::now() : PerfClock::time_point();
			if (APP && APP->window && APP->window->uiFont) {
				char debugIdLabel[32];
				std::snprintf(debugIdLabel, sizeof(debugIdLabel), "ID:%u", debugInstanceId);
				char plasmaSwitchLabel[32];
				std::snprintf(plasmaSwitchLabel, sizeof(plasmaSwitchLabel), "PSW:%.1fus", plasmaSwitchDrawUsEma);
				const float x = box.size.x - mm2px(0.9f);
				const float y = mm2px(2.5f);
				nvgSave(args.vg);
				nvgFontFaceId(args.vg, APP->window->uiFont->handle);
				nvgFontSize(args.vg, 6.8f);
				nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
				nvgText(args.vg, x + 0.45f, y + 0.45f, debugIdLabel, nullptr);
				nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 230));
				nvgText(args.vg, x, y, debugIdLabel, nullptr);
				nvgFontSize(args.vg, 6.2f);
				nvgFillColor(args.vg, nvgRGBA(8, 10, 14, 210));
				nvgText(args.vg, x + 0.45f, y + mm2px(2.0f) + 0.45f, plasmaSwitchLabel, nullptr);
				nvgFillColor(args.vg, nvgRGBA(190, 235, 255, 225));
				nvgText(args.vg, x, y + mm2px(2.0f), plasmaSwitchLabel, nullptr);
				nvgRestore(args.vg);
			}
			if (logDraw) {
				debugOverlayUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
					PerfClock::now() - overlayStart).count()) * 1e-3f;
			}
		}

		if (logDraw) {
			logRow.row = drawLogRowCounter++;
			logRow.moduleId = module ? module->id : -1;
			logRow.instanceId = debugInstanceId;
			logRow.previewRenderMode = flux->previewRenderModeControl().load(std::memory_order_relaxed);
			logRow.previewTracerEnabled = flux->previewTracerEnabledControl().load(std::memory_order_relaxed) ? 1 : 0;
			logRow.previewTracerMode = flux->previewTracerCacheModeControl().load(std::memory_order_relaxed);
			logRow.totalDrawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				PerfClock::now() - totalStart).count()) * 1e-3f;
			logRow.moduleWidgetDrawUs = float(std::chrono::duration_cast<std::chrono::nanoseconds>(
				afterModuleDraw - perfStart).count()) * 1e-3f;
			logRow.debugSubmitUs = debugSubmitUs;
			logRow.debugOverlayUs = debugOverlayUs;
			logRow.gearDrawUs = float(gIntegralFluxGearDrawNsThisFrame) * 1e-3f;
			logRow.eclipseDrawUs = float(gIntegralFluxEclipseDrawNsThisFrame) * 1e-3f;
			logRow.eclipseShadowUs = float(visual_assets::eclipseShadowDrawNs()) * 1e-3f;
			logRow.eclipseShadowCount = visual_assets::eclipseShadowDrawCount();
			logRow.apertureDrawUs = float(gIntegralFluxApertureDrawNsThisFrame) * 1e-3f;
			logRow.linearPointDrawUs = float(gIntegralFluxLinearPointDrawNsThisFrame) * 1e-3f;
			logRow.shapeGlyphDrawUs = float(gIntegralFluxShapeGlyphDrawNsThisFrame) * 1e-3f;
			logRow.plasmaSwitchDrawUs = float(gIntegralFluxPlasmaSwitchDrawNsThisFrame) * 1e-3f;
			logRow.previewDrawUs = float(gIntegralFluxPreviewDrawNsThisFrame) * 1e-3f;
			logRow.ch1PreviewDrawUs = float(gIntegralFluxPreviewDrawNsByChannel[1]) * 1e-3f;
			logRow.ch4PreviewDrawUs = float(gIntegralFluxPreviewDrawNsByChannel[4]) * 1e-3f;
			logRow.previewFramebufferUs = float(gIntegralFluxPreviewFramebufferNsThisFrame) * 1e-3f;
			logRow.ch1PreviewFramebufferUs = float(gIntegralFluxPreviewFramebufferNsByChannel[1]) * 1e-3f;
			logRow.ch4PreviewFramebufferUs = float(gIntegralFluxPreviewFramebufferNsByChannel[4]) * 1e-3f;
			const PlasmaSwitchDrawMetrics switchMetrics = getPlasmaSwitchDrawMetrics();
			logRow.plasmaSwitchShadowUs = float(switchMetrics.shadowNs) * 1e-3f;
			logRow.plasmaSwitchImageEnsureUs = float(switchMetrics.imageEnsureNs) * 1e-3f;
			logRow.plasmaSwitchImagePaintUs = float(switchMetrics.imagePaintNs) * 1e-3f;
			logRow.plasmaSwitchBodyUs = float(switchMetrics.bodyNs) * 1e-3f;
			logRow.plasmaSwitchOrbUs = float(switchMetrics.orbNs) * 1e-3f;
			logRow.plasmaSwitchImageCreates = switchMetrics.imageCreates;
			logRow.plasmaSwitchImageFallbacks = switchMetrics.imageFallbacks;
			logRow.plasmaSwitchContextResets = switchMetrics.contextResets;
			logRow.previewDirtyRequests = gIntegralFluxPreviewDirtyRequestsThisFrame;
			logRow.previewPointRebuilds = gIntegralFluxPreviewPointRebuildsThisFrame;
			logRow.previewTracerCaptures = gIntegralFluxPreviewTracerCapturesThisFrame;
			logRow.linearPointDirtyRequests = gIntegralFluxLinearPointDirtyRequestsThisFrame;
			logRow.shapeGlyphDirtyRequests = gIntegralFluxShapeGlyphDirtyRequestsThisFrame;
			logRow.haloDirtyDrawCount = gIntegralFluxHaloDirtyDrawCountThisFrame;
			logRow.haloActiveDrawCount = gIntegralFluxHaloActiveDrawCountThisFrame;
			logRow.haloDraggingDrawCount = gIntegralFluxHaloDraggingDrawCountThisFrame;
			logRow.haloNanoVgForced = forceHaloNanoVg ? 1 : 0;
			const visual_assets::HaloKnob2DrawMetrics haloMetrics = visual_assets::getHaloKnob2DrawMetrics();
			logRow.haloGlSurfaceFramebufferUs = float(haloMetrics.glSurfaceFramebufferNs) * 1e-3f;
			logRow.haloNanoVgSurfaceDrawUs = float(haloMetrics.nanoVgSurfaceDrawNs) * 1e-3f;
			logRow.haloCenterFramebufferUs = float(haloMetrics.centerFramebufferNs) * 1e-3f;
			logRow.haloCapReflectionFramebufferUs = float(haloMetrics.capReflectionFramebufferNs) * 1e-3f;
			logRow.haloGlSurfaceFramebufferDraws = haloMetrics.glSurfaceFramebufferDraws;
			logRow.haloNanoVgSurfaceDraws = haloMetrics.nanoVgSurfaceDraws;
			logRow.haloCenterFramebufferDraws = haloMetrics.centerFramebufferDraws;
			logRow.haloCapReflectionFramebufferDraws = haloMetrics.capReflectionFramebufferDraws;
			logRow.uiStepEmaUs = uiStepMsEma * 1000.f;
			logRow.uiDrawEmaUs = uiDrawMsEma * 1000.f;
			writeDrawLogRow(logRow);
		}
		if (measurePerf) {
			resetIntegralFluxStepDiagnosticsForNextDraw();
		}
	}

	void appendContextMenu(Menu* menu) override {
		IntegralFlux* maths = static_cast<IntegralFlux*>(module);
		assert(menu);

		menu->addChild(new MenuSeparator());
		if (maths) {
			menu->addChild(createMenuLabel("Performance"));
			menu->addChild(createCheckMenuItem("Force Halo Dynamic Surface to NanoVG", "",
				[=]() { return forceHaloNanoVg; },
				[=]() {
					forceHaloNanoVg = !forceHaloNanoVg;
					for (IntegralFluxHalo2Knob* knob : haloKnobs) {
						if (knob) knob->setForceNanoVgLedRenderer(forceHaloNanoVg);
					}
				}
			));
			menu->addChild(createCheckMenuItem("Bandlimited EOR/EOC", "",
				[=]() { return maths->bandlimitedGateOutputsControl().load(std::memory_order_relaxed); },
				[=]() { maths->bandlimitedGateOutputsControl().store(!maths->bandlimitedGateOutputsControl().load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createCheckMenuItem("Bandlimited CH1/CH4 Signal Outputs", "",
				[=]() { return maths->bandlimitedSignalOutputsControl().load(std::memory_order_relaxed); },
				[=]() { maths->bandlimitedSignalOutputsControl().store(!maths->bandlimitedSignalOutputsControl().load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createMenuLabel("Preview Visual"));
			if (isDragonKingPreviewWidgetOptionsEnabled()) {
				menu->addChild(createSubmenuItem("Render", "",
					[=](Menu* submenu) {
						submenu->addChild(createCheckMenuItem("NanoVG", "",
							[=]() { return maths->previewRenderModeControl().load(std::memory_order_relaxed) == 0; },
							[=]() { maths->previewRenderModeControl().store(0, std::memory_order_relaxed); }
						));
						submenu->addChild(createCheckMenuItem("OpenGL", "",
							[=]() { return maths->previewRenderModeControl().load(std::memory_order_relaxed) == 1; },
							[=]() { maths->previewRenderModeControl().store(1, std::memory_order_relaxed); }
						));
					}
				));
			}
			menu->addChild(createCheckMenuItem("Preview Tracer", "",
				[=]() { return maths->previewTracerEnabledControl().load(std::memory_order_relaxed); },
				[=]() { maths->previewTracerEnabledControl().store(!maths->previewTracerEnabledControl().load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			if (isDragonKingPreviewWidgetOptionsEnabled()) {
				menu->addChild(createSubmenuItem("Tracer Quality", "",
					[=](Menu* submenu) {
						submenu->addChild(createCheckMenuItem("Curve cache", "",
							[=]() { return maths->previewTracerCacheModeControl().load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_CURVE_CACHE; },
							[=]() { maths->previewTracerCacheModeControl().store(WAVE_PREVIEW_TRACER_CURVE_CACHE, std::memory_order_relaxed); }
						));
						submenu->addChild(createCheckMenuItem("Frame cache", "",
							[=]() { return maths->previewTracerCacheModeControl().load(std::memory_order_relaxed) == WAVE_PREVIEW_TRACER_FRAME_CACHE; },
							[=]() { maths->previewTracerCacheModeControl().store(WAVE_PREVIEW_TRACER_FRAME_CACHE, std::memory_order_relaxed); }
						));
					}
				));
			}
			menu->addChild(createMenuLabel("Rate Control"));
			menu->addChild(createCheckMenuItem("Interpolate Timing Updates", "",
				[=]() { return maths->timingInterpolateControl().load(std::memory_order_relaxed); },
				[=]() { maths->timingInterpolateControl().store(!maths->timingInterpolateControl().load(std::memory_order_relaxed), std::memory_order_relaxed); }
			));
			menu->addChild(createSubmenuItem("Timing Update Rate", "",
				[=](Menu* submenu) {
					auto addDivItem = [=](int div, std::string label) {
						submenu->addChild(createCheckMenuItem(label, "",
							[=]() { return maths->requestedTimingUpdateDivControl().load(std::memory_order_relaxed) == div; },
							[=]() { maths->requestTimingUpdateDiv(div); }
						));
					};
					addDivItem(1, "Audio rate (/1)");
					addDivItem(4, "Control rate (/4)");
					addDivItem(8, "Control rate (/8)");
					addDivItem(16, "Control rate (/16)");
					addDivItem(32, "Control rate (/32)");
				}
			));
			menu->addChild(createMenuLabel("Plugin"));
			menu->addChild(createMenuItem("Reset Crystal", "", [=]() {
				visual_assets::resetPanelGlassColorCycle();
			}));
			if (isDragonKingDebugEnabled()) {
				const bool hasNautiloid = leftNautiloidForIntegralFlux(maths) != nullptr;
				menu->addChild(new MenuSeparator());
				menu->addChild(createMenuLabel("Integral Flux Debug"));
				if (hasNautiloid) {
					menu->addChild(createMenuItem("Save Nautiloid Fractal Parameters", "", [=]() {
						appendIntegralFluxFractalParamsCapture(maths);
					}));
				} else {
					menu->addChild(createMenuItem("Delete Selected Fractal Parameters", "", [=]() {
						nautiloidGlass->deleteFallbackSelection();
					}, !nautiloidGlass || !nautiloidGlass->hasFallbackSelection()));
				}
				menu->addChild(createMenuLabel(
					hasNautiloid
						? std::string("Write: ") + integralFluxUserFractalParamsPath()
						: std::string("Read: ") + integralFluxFractalParamsLibraryPath()));
			}
		}
	}
};

} // namespace
