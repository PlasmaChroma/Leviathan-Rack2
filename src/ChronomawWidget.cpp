#include "Chronomaw.hpp"
#include "PanelSvgUtils.hpp"
#include <array>
#include <string>

namespace {

struct ChronomawActionButton : TL1105 {};

struct ChronomawUiRects {
	math::Rect globalBar;
	math::Rect overview;
	math::Rect timeline;
	math::Rect inspector;
};

static NVGcolor chronomawRgb(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) {
	return nvgRGBA(r, g, b, a);
}

static void drawRectFilled(const Widget::DrawArgs& args, const math::Rect& rect, NVGcolor fill, NVGcolor stroke) {
	nvgBeginPath(args.vg);
	nvgRoundedRect(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, 4.0f);
	nvgFillColor(args.vg, fill);
	nvgFill(args.vg);
	nvgStrokeWidth(args.vg, 1.0f);
	nvgStrokeColor(args.vg, stroke);
	nvgStroke(args.vg);
}

static void drawLabel(const Widget::DrawArgs& args, float x, float y, int align, float size, NVGcolor color, const std::string& text) {
	if (!APP || !APP->window || !APP->window->uiFont) {
		return;
	}
	nvgFontSize(args.vg, size);
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgTextAlign(args.vg, align);
	nvgFillColor(args.vg, color);
	nvgText(args.vg, x, y, text.c_str(), nullptr);
}

struct ChronomawSurfaceWidget : Widget {
	Chronomaw* module = nullptr;
	ChronomawUiRects uiRects;
	int activeSliderId = -1;
	float activeSliderX = 0.f;

	static constexpr int kTabCount = 7;
	static constexpr int kMaxInspectorControls = 8;
	static constexpr int kControlNone = -1;
	static constexpr int kControlPhase = 0;
	static constexpr int kControlLevel = 1;
	static constexpr int kControlOffset = 2;
	static constexpr int kControlProbability = 3;
	static constexpr int kControlMute = 4;
	static constexpr int kControlInvert = 5;
	static constexpr int kControlSeedDec = 6;
	static constexpr int kControlSeedInc = 7;

	struct InspectorControl {
		int id = kControlNone;
		math::Rect rect;
	};
	std::array<InspectorControl, kMaxInspectorControls> controls {};
	int controlCount = 0;

	static const char* tabName(int index) {
		static const char* const kNames[kTabCount] = {"Timing", "Shape", "Pattern", "Cross", "CV", "Quant", "Store"};
		const int clamped = clamp(index, 0, kTabCount - 1);
		return kNames[clamped];
	}

	explicit ChronomawSurfaceWidget(Chronomaw* module, const ChronomawUiRects& uiRects) : module(module), uiRects(uiRects) {}

	int selectedOutput() const {
		if (!module) {
			return 0;
		}
		return clamp(module->state.ui.selectedOutput, 0, chronomaw::kNumOutputs - 1);
	}

	int selectedTab() const {
		if (!module) {
			return 0;
		}
		return std::max(0, module->state.ui.selectedTab);
	}

	chronomaw::DensityMode densityMode() const {
		if (!module) {
			return chronomaw::DensityMode::Monitor;
		}
		return module->state.live.density;
	}

	math::Rect timelineRectForDensity() const {
		const chronomaw::DensityMode density = densityMode();
		if (density == chronomaw::DensityMode::Edit) {
			return math::Rect(uiRects.timeline.pos, Vec(uiRects.timeline.size.x, std::max(24.f, uiRects.timeline.size.y * 0.38f)));
		}
		if (density == chronomaw::DensityMode::Focus) {
			return math::Rect(uiRects.timeline.pos, Vec(uiRects.timeline.size.x, std::max(18.f, uiRects.timeline.size.y * 0.25f)));
		}
		return uiRects.timeline;
	}

	math::Rect inspectorRectForDensity() const {
		const chronomaw::DensityMode density = densityMode();
		if (density == chronomaw::DensityMode::Monitor) {
			return math::Rect(uiRects.inspector.pos, Vec(uiRects.inspector.size.x, std::max(38.f, uiRects.inspector.size.y * 0.34f)));
		}
		if (density == chronomaw::DensityMode::Focus) {
			const float y = uiRects.timeline.pos.y + std::max(24.f, uiRects.timeline.size.y * 0.25f) + 3.f;
			const float h = std::max(44.f, uiRects.inspector.pos.y + uiRects.inspector.size.y - y);
			return math::Rect(Vec(uiRects.inspector.pos.x, y), Vec(uiRects.inspector.size.x, h));
		}
		return uiRects.inspector;
	}

	int outputRowAt(const Vec& p) const {
		if (!uiRects.overview.contains(p)) {
			return -1;
		}
		const float rowH = uiRects.overview.size.y / float(chronomaw::kNumOutputs);
		if (rowH <= 1.f) {
			return -1;
		}
		const int row = int((p.y - uiRects.overview.pos.y) / rowH);
		return clamp(row, 0, chronomaw::kNumOutputs - 1);
	}

	int tabAt(const Vec& p, const math::Rect& inspectorRect) const {
		const float tabStripH = 16.f;
		math::Rect tabRect(inspectorRect.pos, Vec(inspectorRect.size.x, tabStripH));
		if (!tabRect.contains(p)) {
			return -1;
		}
		const float tabW = inspectorRect.size.x / float(kTabCount);
		if (tabW <= 1.f) {
			return -1;
		}
		return clamp(int((p.x - inspectorRect.pos.x) / tabW), 0, kTabCount - 1);
	}

	chronomaw::OutputState* selectedOutputState() const {
		if (!module) {
			return nullptr;
		}
		const int out = selectedOutput();
		return &module->state.live.outputs[size_t(out)];
	}

	void clearControls() {
		controlCount = 0;
		for (int i = 0; i < kMaxInspectorControls; ++i) {
			controls[size_t(i)] = InspectorControl{};
		}
	}

	void addControl(int id, const math::Rect& rect) {
		if (controlCount >= kMaxInspectorControls) {
			return;
		}
		controls[size_t(controlCount)].id = id;
		controls[size_t(controlCount)].rect = rect;
		++controlCount;
	}

	int controlAt(const Vec& p) const {
		for (int i = 0; i < controlCount; ++i) {
			if (controls[size_t(i)].rect.contains(p)) {
				return controls[size_t(i)].id;
			}
		}
		return kControlNone;
	}

	math::Rect controlRect(int id) const {
		for (int i = 0; i < controlCount; ++i) {
			if (controls[size_t(i)].id == id) {
				return controls[size_t(i)].rect;
			}
		}
		return math::Rect();
	}

	static float sliderValueFromPoint(const math::Rect& rect, float x, float minV, float maxV) {
		if (rect.size.x <= 4.f) {
			return minV;
		}
		const float t = clamp((x - rect.pos.x) / rect.size.x, 0.f, 1.f);
		return minV + t * (maxV - minV);
	}

	void applySliderFromPointer(int id, const Vec& local) {
		chronomaw::OutputState* out = selectedOutputState();
		if (!out) {
			return;
		}
		const math::Rect rect = controlRect(id);
		if (rect.size.x <= 0.f || rect.size.y <= 0.f) {
			return;
		}
		if (id == kControlPhase) {
			out->phasePct = sliderValueFromPoint(rect, local.x, -100.f, 100.f);
			return;
		}
		if (id == kControlLevel) {
			out->levelPct = sliderValueFromPoint(rect, local.x, 0.f, 100.f);
			return;
		}
		if (id == kControlOffset) {
			out->offsetPct = sliderValueFromPoint(rect, local.x, -100.f, 100.f);
			return;
		}
		if (id == kControlProbability) {
			out->probabilityPct = sliderValueFromPoint(rect, local.x, 0.f, 100.f);
			return;
		}
	}

	void applyControlClick(int id) {
		chronomaw::OutputState* out = selectedOutputState();
		if (!out) {
			return;
		}
		if (id == kControlMute) {
			out->muted = !out->muted;
			return;
		}
		if (id == kControlInvert) {
			out->invert = !out->invert;
			return;
		}
		if (id == kControlSeedDec) {
			if (out->randomSeed > 0u) {
				--out->randomSeed;
			}
			return;
		}
		if (id == kControlSeedInc) {
			++out->randomSeed;
		}
	}

	Vec currentLocalMousePos() const {
		if (!APP || !APP->scene || !APP->scene->rack || !parent) {
			return Vec();
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			if (e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_RELEASE) {
				activeSliderId = kControlNone;
			}
			return;
		}

		const Vec local = e.pos;
		const int row = outputRowAt(local);
		if (row >= 0) {
			module->state.ui.selectedOutput = row;
			module->params[Chronomaw::SELECTED_OUTPUT_PARAM].setValue(float(row + 1));
			e.consume(this);
			return;
		}

		const math::Rect inspectorRect = inspectorRectForDensity();
		const int tab = tabAt(local, inspectorRect);
		if (tab >= 0) {
			module->state.ui.selectedTab = tab;
			e.consume(this);
			return;
		}

		const int controlId = controlAt(local);
		if (controlId == kControlNone) {
			return;
		}
		if (controlId == kControlPhase || controlId == kControlLevel || controlId == kControlOffset || controlId == kControlProbability) {
			activeSliderId = controlId;
			activeSliderX = local.x;
			applySliderFromPointer(controlId, local);
			e.consume(this);
			return;
		}
		applyControlClick(controlId);
		e.consume(this);
	}

	void onDragMove(const event::DragMove& e) override {
		if (!module || activeSliderId == kControlNone) {
			return;
		}
		const math::Rect rect = controlRect(activeSliderId);
		activeSliderX = clamp(currentLocalMousePos().x, rect.pos.x, rect.pos.x + rect.size.x);
		applySliderFromPointer(activeSliderId, Vec(activeSliderX, rect.pos.y + 0.5f * rect.size.y));
		e.consume(this);
	}

	void onHoverScroll(const event::HoverScroll& e) override {
		if (!module) {
			return;
		}
		const int controlId = controlAt(e.pos);
		chronomaw::OutputState* out = selectedOutputState();
		if (!out || controlId == kControlNone) {
			return;
		}
		const float delta = e.scrollDelta.y;
		if (controlId == kControlPhase) {
			out->phasePct = clamp(out->phasePct + delta, -100.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlLevel) {
			out->levelPct = clamp(out->levelPct + delta, 0.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlOffset) {
			out->offsetPct = clamp(out->offsetPct + delta, -100.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlProbability) {
			out->probabilityPct = clamp(out->probabilityPct + delta, 0.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlSeedInc || controlId == kControlSeedDec) {
			const int step = (delta > 0.f) ? 1 : -1;
			if (step < 0 && out->randomSeed > 0u) {
				--out->randomSeed;
			}
			if (step > 0) {
				++out->randomSeed;
			}
			e.consume(this);
		}
	}

	void drawSlider(const DrawArgs& args, const math::Rect& rect, float value, float minV, float maxV, const std::string& label, int controlId) {
		drawRectFilled(args, rect, chronomawRgb(17, 26, 37, 210), chronomawRgb(90, 122, 148, 176));
		float t = (maxV <= minV) ? 0.f : clamp((value - minV) / (maxV - minV), 0.f, 1.f);
		math::Rect fillRect(rect.pos, Vec(rect.size.x * t, rect.size.y));
		drawRectFilled(args, fillRect, chronomawRgb(54, 112, 156, 220), chronomawRgb(54, 112, 156, 220));
		drawLabel(args, rect.pos.x + 3.f, rect.pos.y - 1.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 7.8f, chronomawRgb(182, 220, 240, 232), label);
		drawLabel(args, rect.pos.x + rect.size.x - 3.f, rect.pos.y + 0.5f * rect.size.y, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE, 8.2f, chronomawRgb(232, 246, 255, 238), string::f("%.1f", value));
		addControl(controlId, rect);
	}

	void drawToggle(const DrawArgs& args, const math::Rect& rect, const std::string& label, bool on, int controlId) {
		drawRectFilled(args, rect, on ? chronomawRgb(45, 97, 67, 220) : chronomawRgb(38, 34, 40, 210), on ? chronomawRgb(147, 228, 172, 204) : chronomawRgb(120, 112, 124, 184));
		drawLabel(args, rect.pos.x + 4.f, rect.pos.y + 0.5f * rect.size.y, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 8.0f, chronomawRgb(228, 240, 248, 242), label);
		drawLabel(args, rect.pos.x + rect.size.x - 4.f, rect.pos.y + 0.5f * rect.size.y, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE, 8.0f, chronomawRgb(236, 252, 255, 244), on ? "ON" : "OFF");
		addControl(controlId, rect);
	}

	void drawTrace(const DrawArgs& args, const math::Rect& rect, int channel, bool internalTrace, NVGcolor color, bool useTimelineHistory) {
		if (!module || channel < 0 || channel >= chronomaw::kNumOutputs || rect.size.x <= 2.f || rect.size.y <= 2.f) {
			return;
		}
		const int hist = useTimelineHistory ? Chronomaw::kTimelineHistorySize : Chronomaw::kPreviewHistorySize;
		const int writePos = useTimelineHistory ? module->timelineWritePos.load(std::memory_order_relaxed) : module->previewWritePos.load(std::memory_order_relaxed);
		nvgBeginPath(args.vg);
		for (int i = 0; i < hist; ++i) {
			const int idx = (writePos + i) % hist;
			float v = 0.f;
			if (internalTrace && useTimelineHistory) {
				v = module->timelineInternalHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
			}
			else if (internalTrace) {
				v = module->previewInternalHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
			}
			else if (useTimelineHistory) {
				v = module->timelineOutputHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
			}
			else {
				v = module->previewOutputHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
			}
			v = clamp(v, chronomaw::kOutputMinV, chronomaw::kOutputMaxV);
			const float t = (hist <= 1) ? 0.f : (float(i) / float(hist - 1));
			const float x = rect.pos.x + t * rect.size.x;
			const float yNorm = (chronomaw::kOutputMaxV <= chronomaw::kOutputMinV) ? 0.f : ((v - chronomaw::kOutputMinV) / (chronomaw::kOutputMaxV - chronomaw::kOutputMinV));
			const float y = rect.pos.y + (1.f - yNorm) * rect.size.y;
			if (i == 0) {
				nvgMoveTo(args.vg, x, y);
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
		}
		nvgStrokeWidth(args.vg, internalTrace ? 0.9f : 1.15f);
		nvgStrokeColor(args.vg, color);
		nvgStroke(args.vg);
	}

	void drawTimelineLaneTrace(const DrawArgs& args, const math::Rect& rect, int channel, bool selected, float nowFrac) {
		if (!module || channel < 0 || channel >= chronomaw::kNumOutputs || rect.size.x <= 3.f || rect.size.y <= 2.f) {
			return;
		}
		const int hist = Chronomaw::kTimelineHistorySize;
		const int writePos = module->timelineWritePos.load(std::memory_order_relaxed);
		const int latestIdx = (writePos - 1 + hist) % hist;
		const float nowX = rect.pos.x + clamp(nowFrac, 0.05f, 0.95f) * rect.size.x;
		const float historyWidth = std::max(4.f, nowX - rect.pos.x);
		const float futureWidth = std::max(0.f, rect.pos.x + rect.size.x - nowX);
		const float dtSec = Chronomaw::kTimelineCaptureIntervalSec;
		const float historyCapSec = float(Chronomaw::kTimelineHistorySize - 1) * dtSec;
		const float futureCapSec = float(Chronomaw::kTimelineFutureSize) * dtSec;
		// Enforce one time scale (seconds-per-pixel) across the full lane.
		const float secPerPixel = std::min(historyCapSec / historyWidth, (futureWidth > 0.f) ? (futureCapSec / futureWidth) : (historyCapSec / historyWidth));

		auto voltsToY = [&](float v) {
			const float vv = clamp(v, chronomaw::kOutputMinV, chronomaw::kOutputMaxV);
			const float yn = (chronomaw::kOutputMaxV <= chronomaw::kOutputMinV) ? 0.f : ((vv - chronomaw::kOutputMinV) / (chronomaw::kOutputMaxV - chronomaw::kOutputMinV));
			return rect.pos.y + (1.f - yn) * rect.size.y;
		};
		auto historyValueAtAgeSteps = [&](float ageStepsFloat) {
			const float clampedAge = clamp(ageStepsFloat, 0.f, float(Chronomaw::kTimelineHistorySize - 1));
			const int age0 = int(std::floor(clampedAge));
			const int age1 = std::min(age0 + 1, Chronomaw::kTimelineHistorySize - 1);
			const float frac = clampedAge - float(age0);
			const int idx0 = (latestIdx - age0 + hist) % hist;
			const int idx1 = (latestIdx - age1 + hist) % hist;
			const float v0 = module->timelineOutputHistory[size_t(channel)][size_t(idx0)].load(std::memory_order_relaxed);
			const float v1 = module->timelineOutputHistory[size_t(channel)][size_t(idx1)].load(std::memory_order_relaxed);
			return v0 + (v1 - v0) * frac;
		};
		auto futureValueAtSteps = [&](float futureStepsFloat) {
			const float clampedStep = clamp(futureStepsFloat, 0.f, float(Chronomaw::kTimelineFutureSize - 1));
			const int s0 = int(std::floor(clampedStep));
			const int s1 = std::min(s0 + 1, Chronomaw::kTimelineFutureSize - 1);
			const float frac = clampedStep - float(s0);
			const float v0 = module->timelineFutureOutput[size_t(channel)][size_t(s0)].load(std::memory_order_relaxed);
			const float v1 = module->timelineFutureOutput[size_t(channel)][size_t(s1)].load(std::memory_order_relaxed);
			return v0 + (v1 - v0) * frac;
		};

		const int visibleCount = std::max(8, int(historyWidth));
		nvgBeginPath(args.vg);
		for (int i = 0; i < visibleCount; ++i) {
			const float t = (visibleCount <= 1) ? 0.f : (float(i) / float(visibleCount - 1));
			const float x = rect.pos.x + t * historyWidth;
			const float ageSec = (historyWidth - (x - rect.pos.x)) * secPerPixel;
			const float ageStepsFloat = ageSec / dtSec;
			const float v = historyValueAtAgeSteps(ageStepsFloat);
			const float y = voltsToY(v);
			if (i == 0) {
				nvgMoveTo(args.vg, x, y);
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
		}
		nvgStrokeWidth(args.vg, selected ? 1.35f : 1.0f);
		nvgStrokeColor(args.vg, selected ? chronomawRgb(244, 249, 255, 238) : chronomawRgb(188, 206, 224, 182));
		nvgStroke(args.vg);

		// Draw deterministic future projection to the right of "now".
		if (futureWidth > 0.f) {
			nvgBeginPath(args.vg);
			const float latestV = module->timelineOutputHistory[size_t(channel)][size_t(latestIdx)].load(std::memory_order_relaxed);
			nvgMoveTo(args.vg, nowX, voltsToY(latestV));
			const int futureCount = std::max(8, int(futureWidth));
			for (int i = 0; i < futureCount; ++i) {
				const float t = (futureCount <= 1) ? 1.f : float(i + 1) / float(futureCount);
				const float x = nowX + t * futureWidth;
				const float futureSec = t * futureWidth * secPerPixel;
				const float futureStepFloat = std::max(0.f, (futureSec / dtSec) - 1.f);
				const float v = futureValueAtSteps(futureStepFloat);
				nvgLineTo(args.vg, x, voltsToY(v));
			}
			nvgStrokeWidth(args.vg, selected ? 1.0f : 0.85f);
			nvgStrokeColor(args.vg, selected ? chronomawRgb(244, 249, 255, 238) : chronomawRgb(188, 206, 224, 182));
			nvgStroke(args.vg);
		}
	}

	void drawOverview(const DrawArgs& args) {
		drawRectFilled(args, uiRects.overview, chronomawRgb(8, 16, 24, 148), chronomawRgb(116, 158, 190, 180));
		const float rowH = uiRects.overview.size.y / float(chronomaw::kNumOutputs);
		const int selected = selectedOutput();
		for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
			math::Rect rowRect(
				Vec(uiRects.overview.pos.x + 1.5f, uiRects.overview.pos.y + rowH * float(i) + 1.f),
				Vec(uiRects.overview.size.x - 3.f, rowH - 1.8f)
			);
			const bool isSelected = (i == selected);
			drawRectFilled(
				args,
				rowRect,
				isSelected ? chronomawRgb(34, 68, 98, 210) : chronomawRgb(14, 24, 33, 168),
				isSelected ? chronomawRgb(154, 212, 255, 232) : chronomawRgb(84, 118, 143, 174)
			);
			drawLabel(
				args,
				rowRect.pos.x + 5.f,
				rowRect.pos.y + 0.5f * rowRect.size.y,
				NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
				9.5f,
				chronomawRgb(222, 238, 250, 240),
				"Out " + std::to_string(i + 1)
			);
			drawLabel(
				args,
				rowRect.pos.x + rowRect.size.x - 6.f,
				rowRect.pos.y + 0.5f * rowRect.size.y,
				NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE,
				8.4f,
				chronomawRgb(184, 214, 236, 220),
				(i == selected ? "SEL" : "--")
			);
		}
		drawLabel(
			args,
			uiRects.overview.pos.x + 4.f,
			uiRects.overview.pos.y - 2.5f,
			NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM,
			9.4f,
			chronomawRgb(180, 226, 251, 236),
			"Overview"
		);
	}

	void drawTimeline(const DrawArgs& args, const math::Rect& timelineRect) {
		drawRectFilled(args, timelineRect, chronomawRgb(9, 17, 28, 156), chronomawRgb(99, 139, 170, 186));
		const float laneH = timelineRect.size.y / float(chronomaw::kNumOutputs);
		const int selected = selectedOutput();
		const float nowFrac = 0.28f;
		for (int lane = 0; lane < chronomaw::kNumOutputs; ++lane) {
			const float y0 = timelineRect.pos.y + laneH * float(lane);
			const math::Rect laneRect(
				Vec(timelineRect.pos.x + 2.f, y0 + 1.3f),
				Vec(timelineRect.size.x - 4.f, std::max(2.f, laneH - 2.6f))
			);
			const bool isSelected = (lane == selected);
			drawTimelineLaneTrace(args, laneRect, lane, isSelected, nowFrac);
		}
		for (int i = 1; i < chronomaw::kNumOutputs; ++i) {
			const float y = timelineRect.pos.y + laneH * float(i);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, timelineRect.pos.x + 1.5f, y);
			nvgLineTo(args.vg, timelineRect.pos.x + timelineRect.size.x - 1.5f, y);
			nvgStrokeWidth(args.vg, 1.f);
			nvgStrokeColor(args.vg, chronomawRgb(74, 102, 128, 156));
			nvgStroke(args.vg);
		}
		const float nowX = timelineRect.pos.x + timelineRect.size.x * nowFrac;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, nowX, timelineRect.pos.y + 1.2f);
		nvgLineTo(args.vg, nowX, timelineRect.pos.y + timelineRect.size.y - 1.2f);
		// Match Bifurx curve-line colors and two-pass stroke treatment.
		nvgStrokeWidth(args.vg, 2.8f);
		nvgStrokeColor(args.vg, chronomawRgb(6, 8, 12, 210));
		nvgStroke(args.vg);
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, nowX, timelineRect.pos.y + 1.2f);
		nvgLineTo(args.vg, nowX, timelineRect.pos.y + timelineRect.size.y - 1.2f);
		nvgStrokeWidth(args.vg, 1.5f);
		nvgStrokeColor(args.vg, chronomawRgb(249, 236, 190, 248));
		nvgStroke(args.vg);
		drawLabel(args, timelineRect.pos.x + 4.f, timelineRect.pos.y - 2.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 9.4f, chronomawRgb(180, 226, 251, 236), "Timeline");
	}

	void drawInspector(const DrawArgs& args, const math::Rect& inspectorRect) {
		clearControls();
		drawRectFilled(args, inspectorRect, chronomawRgb(10, 16, 22, 170), chronomawRgb(96, 136, 160, 184));
		const float tabStripH = 16.f;
		const float tabW = inspectorRect.size.x / float(kTabCount);
		const int tabSel = clamp(selectedTab(), 0, kTabCount - 1);
		for (int i = 0; i < kTabCount; ++i) {
			math::Rect tabRect(
				Vec(inspectorRect.pos.x + tabW * float(i), inspectorRect.pos.y),
				Vec(tabW, tabStripH)
			);
			const bool active = (i == tabSel);
			drawRectFilled(
				args,
				tabRect,
				active ? chronomawRgb(30, 58, 80, 220) : chronomawRgb(14, 22, 30, 180),
				active ? chronomawRgb(170, 222, 250, 220) : chronomawRgb(82, 116, 136, 164)
			);
			drawLabel(
				args,
				tabRect.pos.x + 0.5f * tabRect.size.x,
				tabRect.pos.y + 0.5f * tabRect.size.y + 0.3f,
				NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE,
				8.0f,
				active ? chronomawRgb(238, 248, 255, 245) : chronomawRgb(170, 197, 215, 228),
				tabName(i)
			);
		}
		chronomaw::OutputState* outState = selectedOutputState();

		const float contentX = inspectorRect.pos.x + 4.f;
		const float contentW = inspectorRect.size.x - 8.f;
		const float rowH = 10.0f;
		float y = inspectorRect.pos.y + tabStripH + 12.f;
		if (outState) {
			if (tabSel == 0) {
				drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->phasePct, -100.f, 100.f, "Phase %", kControlPhase);
			}
			else if (tabSel == 1) {
				drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->levelPct, 0.f, 100.f, "Level %", kControlLevel);
				y += rowH + 3.f;
				drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->offsetPct, -100.f, 100.f, "Offset %", kControlOffset);
				y += rowH + 3.f;
				drawToggle(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), "Invert", outState->invert, kControlInvert);
				y += rowH + 3.f;
				drawToggle(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), "Mute", outState->muted, kControlMute);
			}
			else if (tabSel == 2) {
				drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->probabilityPct, 0.f, 100.f, "Probability %", kControlProbability);
				y += rowH + 3.f;
				drawRectFilled(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), chronomawRgb(17, 26, 37, 210), chronomawRgb(90, 122, 148, 176));
				drawLabel(args, contentX + 3.f, y - 1.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 7.8f, chronomawRgb(182, 220, 240, 232), "Seed");
				drawLabel(args, contentX + contentW * 0.5f, y + 0.5f * rowH, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.2f, chronomawRgb(232, 246, 255, 238), std::to_string(outState->randomSeed));
				const float btnW = 12.f;
				const math::Rect decRect(Vec(contentX + contentW - 2.f * btnW - 3.f, y + 1.f), Vec(btnW, rowH - 2.f));
				const math::Rect incRect(Vec(contentX + contentW - btnW - 1.5f, y + 1.f), Vec(btnW, rowH - 2.f));
				drawToggle(args, decRect, "", false, kControlSeedDec);
				drawToggle(args, incRect, "", false, kControlSeedInc);
				drawLabel(args, decRect.pos.x + 0.5f * decRect.size.x, decRect.pos.y + 0.5f * decRect.size.y, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.f, chronomawRgb(232, 246, 255, 240), "-");
				drawLabel(args, incRect.pos.x + 0.5f * incRect.size.x, incRect.pos.y + 0.5f * incRect.size.y, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.f, chronomawRgb(232, 246, 255, 240), "+");
			}
			else {
				drawLabel(
					args,
					inspectorRect.pos.x + 5.f,
					inspectorRect.pos.y + tabStripH + 24.f,
					NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE,
					8.4f,
					chronomawRgb(176, 205, 228, 230),
					"Registry not implemented yet"
				);
			}
		}
		drawLabel(args, inspectorRect.pos.x + 4.f, inspectorRect.pos.y - 2.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 9.4f, chronomawRgb(180, 226, 251, 236), "Inspector");
	}

	void draw(const DrawArgs& args) override {
		if (!module) {
			return;
		}
		drawRectFilled(args, uiRects.globalBar, chronomawRgb(8, 13, 20, 98), chronomawRgb(96, 136, 162, 140));
		drawOverview(args);
		const math::Rect timelineRect = timelineRectForDensity();
		const math::Rect inspectorRect = inspectorRectForDensity();
		drawTimeline(args, timelineRect);
		drawInspector(args, inspectorRect);

		const char* densityLabel = "Monitor";
		if (densityMode() == chronomaw::DensityMode::Edit) {
			densityLabel = "Edit";
		}
		else if (densityMode() == chronomaw::DensityMode::Focus) {
			densityLabel = "Focus";
		}
		drawLabel(args, uiRects.globalBar.pos.x + uiRects.globalBar.size.x - 3.f, uiRects.globalBar.pos.y + 2.f, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP, 8.5f, chronomawRgb(179, 220, 244, 236), std::string("Density: ") + densityLabel);
	}
};

} // namespace

ChronomawWidget::ChronomawWidget(Chronomaw* module) {
	setModule(module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/chronomaw.svg");
	setPanel(createPanel(panelPath));

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

	Vec runPos(27.0f, 16.0f);
	Vec bpmPos(47.0f, 16.0f);
	Vec activeBankPos(67.0f, 16.0f);
	Vec loadBankPos(88.0f, 16.0f);
	Vec saveBankPos(98.0f, 16.0f);
	Vec selectedOutputPos(133.0f, 16.0f);
	Vec densityModePos(153.0f, 16.0f);
	Vec clkInPos(9.5f, 22.0f);
	Vec runInPos(9.5f, 35.5f);
	Vec resetInPos(9.5f, 49.0f);
	Vec cv1InPos(9.5f, 67.0f);
	Vec cv2InPos(9.5f, 80.5f);
	Vec cv3InPos(9.5f, 94.0f);
	Vec cv4InPos(9.5f, 107.5f);
	Vec runLightPos(32.5f, 16.0f);
	Vec syncLightPos(173.0f, 16.0f);
	std::array<Vec, chronomaw::kNumOutputs> outPos {};
	std::array<Vec, chronomaw::kNumOutputs> outLightPos {};
	ChronomawUiRects uiRects;
	uiRects.globalBar = math::Rect(Vec(19.f, 7.f), Vec(160.f, 18.f));
	uiRects.overview = math::Rect(Vec(22.f, 29.f), Vec(68.f, 84.f));
	uiRects.timeline = math::Rect(Vec(95.f, 29.f), Vec(86.f, 48.f));
	uiRects.inspector = math::Rect(Vec(95.f, 80.f), Vec(86.f, 33.f));
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const float y = 18.5f + 13.0f * float(i);
		outPos[size_t(i)] = Vec(193.5f, y);
		outLightPos[size_t(i)] = Vec(184.8f, y);
	}

	auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
		Vec pointMm;
		if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
			*outPosMm = pointMm;
		}
	};
	auto applyRectOverride = [&](const char* elementId, math::Rect* outRectMm) {
		math::Rect rectMm;
		if (panel_svg::loadRectFromSvgMm(panelPath, elementId, &rectMm)) {
			*outRectMm = rectMm;
		}
	};

	applyPointOverride("RUN", &runPos);
	applyPointOverride("BPM", &bpmPos);
	applyPointOverride("ACTIVE_BANK", &activeBankPos);
	applyPointOverride("LOAD_BANK", &loadBankPos);
	applyPointOverride("SAVE_BANK", &saveBankPos);
	applyPointOverride("SELECTED_OUTPUT", &selectedOutputPos);
	applyPointOverride("DENSITY_MODE", &densityModePos);
	applyPointOverride("CLK_INPUT", &clkInPos);
	applyPointOverride("RUN_INPUT", &runInPos);
	applyPointOverride("RESET_INPUT", &resetInPos);
	applyPointOverride("CV_1_INPUT", &cv1InPos);
	applyPointOverride("CV_2_INPUT", &cv2InPos);
	applyPointOverride("CV_3_INPUT", &cv3InPos);
	applyPointOverride("CV_4_INPUT", &cv4InPos);
	applyPointOverride("RUN_LIGHT", &runLightPos);
	applyPointOverride("SYNC_LIGHT", &syncLightPos);
	applyRectOverride("GLOBAL_BAR_RECT", &uiRects.globalBar);
	applyRectOverride("OVERVIEW_RECT", &uiRects.overview);
	applyRectOverride("TIMELINE_RECT", &uiRects.timeline);
	applyRectOverride("INSPECTOR_RECT", &uiRects.inspector);
	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		const std::string outId = "OUT_" + std::to_string(i + 1) + "_OUTPUT";
		const std::string lightId = "OUT_" + std::to_string(i + 1) + "_LIGHT";
		applyPointOverride(outId.c_str(), &outPos[size_t(i)]);
		applyPointOverride(lightId.c_str(), &outLightPos[size_t(i)]);
	}

	auto* surface = new ChronomawSurfaceWidget(module, ChronomawUiRects{
		math::Rect(mm2px(uiRects.globalBar.pos), mm2px(uiRects.globalBar.size)),
		math::Rect(mm2px(uiRects.overview.pos), mm2px(uiRects.overview.size)),
		math::Rect(mm2px(uiRects.timeline.pos), mm2px(uiRects.timeline.size)),
		math::Rect(mm2px(uiRects.inspector.pos), mm2px(uiRects.inspector.size)),
	});
	surface->box.pos = Vec(0.f, 0.f);
	surface->box.size = box.size;
	addChild(surface);

	addParam(createParamCentered<CKD6>(mm2px(runPos), module, Chronomaw::RUN_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(bpmPos), module, Chronomaw::BPM_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(activeBankPos), module, Chronomaw::ACTIVE_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(loadBankPos), module, Chronomaw::LOAD_BANK_PARAM));
	addParam(createParamCentered<ChronomawActionButton>(mm2px(saveBankPos), module, Chronomaw::SAVE_BANK_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(selectedOutputPos), module, Chronomaw::SELECTED_OUTPUT_PARAM));
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(densityModePos), module, Chronomaw::DENSITY_MODE_PARAM));

	addInput(createInputCentered<PJ301MPort>(mm2px(clkInPos), module, Chronomaw::CLK_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(runInPos), module, Chronomaw::RUN_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(resetInPos), module, Chronomaw::RESET_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv1InPos), module, Chronomaw::CV_1_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv2InPos), module, Chronomaw::CV_2_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv3InPos), module, Chronomaw::CV_3_INPUT));
	addInput(createInputCentered<PJ301MPort>(mm2px(cv4InPos), module, Chronomaw::CV_4_INPUT));

	for (int i = 0; i < chronomaw::kNumOutputs; ++i) {
		addOutput(createOutputCentered<PJ301MPort>(mm2px(outPos[size_t(i)]), module, Chronomaw::OUT_1_OUTPUT + i));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(outLightPos[size_t(i)]), module, Chronomaw::OUT_1_LIGHT + i));
	}
	addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(runLightPos), module, Chronomaw::RUN_LIGHT));
	addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(syncLightPos), module, Chronomaw::SYNC_LIGHT));
}

Model* modelChronomaw = createModel<Chronomaw, ChronomawWidget>("Chronomaw");
