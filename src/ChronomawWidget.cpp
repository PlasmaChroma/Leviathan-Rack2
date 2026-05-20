#include "Chronomaw.hpp"
#include "ChronomawWaveforms.hpp"
#include "PanelSvgUtils.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

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

static float wrapPhase01(float phase) {
	float p = std::fmod(phase, 1.f);
	if (p < 0.f) {
		p += 1.f;
	}
	return p;
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

static void drawModeStepTriangle(const Widget::DrawArgs& args, const Vec& center, float size, bool pointRight, NVGcolor color) {
	const float hW = size;
	const float hH = size * 1.15f;
	const float off = pointRight ? (hW / 3.f) : (-hW / 3.f);
	nvgBeginPath(args.vg);
	if (pointRight) {
		nvgMoveTo(args.vg, center.x - hW + off, center.y - hH);
		nvgLineTo(args.vg, center.x + hW + off, center.y);
		nvgLineTo(args.vg, center.x - hW + off, center.y + hH);
	}
	else {
		nvgMoveTo(args.vg, center.x + hW + off, center.y - hH);
		nvgLineTo(args.vg, center.x - hW + off, center.y);
		nvgLineTo(args.vg, center.x + hW + off, center.y + hH);
	}
	nvgClosePath(args.vg);
	nvgFillColor(args.vg, color);
	nvgFill(args.vg);
}

static void drawMenuGlyph(const Widget::DrawArgs& args, const Vec& center, float halfW, float dy, NVGcolor color) {
	for (int i = 0; i < 3; ++i) {
		const float y = center.y + (float(i) - 1.f) * dy;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, center.x - halfW, y);
		nvgLineTo(args.vg, center.x + halfW, y);
		nvgStrokeWidth(args.vg, 1.2f);
		nvgStrokeColor(args.vg, color);
		nvgStroke(args.vg);
	}
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

	static void drawLabelOutlined(const Widget::DrawArgs& args, float x, float y, int align, float size, NVGcolor color, const std::string& text) {
		const NVGcolor outline = chronomawRgb(0, 0, 0, 220);
		drawLabel(args, x - 0.8f, y, align, size, outline, text);
		drawLabel(args, x + 0.8f, y, align, size, outline, text);
		drawLabel(args, x, y - 0.8f, align, size, outline, text);
		drawLabel(args, x, y + 0.8f, align, size, outline, text);
		drawLabel(args, x, y, align, size, color, text);
	}

struct ChronomawSurfaceWidget : Widget {
	struct FuturePeriodCache {
		bool valid = false;
		uint64_t signature = 0u;
		double periodBeats = 1.0;
		std::vector<float> values;
	};

		Chronomaw* module = nullptr;
		ChronomawUiRects uiRects;
		std::array<FuturePeriodCache, chronomaw::kNumOutputs> futureCaches {};
		double lastFuturePreviewUpdateSec = -1.0;
		int activeSliderId = -1;
		float activeSliderX = 0.f;
		bool activeSliderDragging = false;
		Vec activeSliderPressPos;
		int lastSliderClickId = -1;
		double lastSliderClickTime = -1.0;
		int pendingSliderClickId = -1;
		float pendingSliderClickX = 0.f;
		double pendingSliderCommitTime = -1.0;

		static constexpr int kTabCount = 7;
		static constexpr int kMaxInspectorControls = 20;
		static constexpr double kCustomDoubleClickWindowSec = 0.24;
		static constexpr double kSingleClickCommitDelaySec = 0.10;
		static constexpr int kControlNone = -1;
		static constexpr int kControlMultiplier = 0;
		static constexpr int kControlWidth = 1;
		static constexpr int kControlPhase = 2;
		static constexpr int kControlSwing = 3;
		static constexpr int kControlSkew = 4;
		static constexpr int kControlRotate = 5;
		static constexpr int kControlLevel = 6;
		static constexpr int kControlOffset = 7;
		static constexpr int kControlProbability = 8;
		static constexpr int kControlInvert = 9;
		static constexpr int kControlSeedDec = 10;
		static constexpr int kControlSeedInc = 11;
		static constexpr int kControlWavePrev = 12;
		static constexpr int kControlWaveNext = 13;
		static constexpr int kControlWaveMenu = 14;
		static constexpr int kControlModeDiv = 15;
		static constexpr int kControlModeMult = 16;
		static constexpr int kControlModeUtil = 17;
		static constexpr float kMinRatio = 1.f;
		static constexpr float kMaxMultiplier = 192.f;
		static constexpr float kMaxDivisor = 16384.f;
		static constexpr float kRatioPivotT = 0.7f;
		static constexpr float kMultiplierPivot = 16.f;
		static constexpr float kDivisorPivot = 16.f;
		static constexpr int kFuturePeriodSamplesDefault = 1024;
		static constexpr int kFuturePeriodSamplesNarrow = 2048;
		static constexpr double kFutureCacheMaxPeriodBeats = 8.0;
		static constexpr double kFuturePreviewUpdateIntervalSec = 1.0 / 30.0;

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

		static const char* waveformName(chronomaw::WaveformMode wf) {
			return chronomaw::waveformLabel(wf);
		}

		static const char* modifierModeName(chronomaw::ModifierMode mode) {
			switch (mode) {
				case chronomaw::ModifierMode::Div: return "DIV";
				case chronomaw::ModifierMode::Mult: return "MULT";
				case chronomaw::ModifierMode::Util: return "UTIL";
			}
			return "MULT";
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

	math::Rect timelineRectForDensity() const {
		return uiRects.timeline;
	}

	math::Rect inspectorRectForDensity() const {
		return math::Rect(uiRects.inspector.pos, Vec(uiRects.inspector.size.x, std::max(38.f, uiRects.inspector.size.y * 0.34f)));
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

		math::Rect overviewMuteRectForRow(int row) const {
			const float rowH = uiRects.overview.size.y / float(chronomaw::kNumOutputs);
			const math::Rect rowRect(
				Vec(uiRects.overview.pos.x + 1.5f, uiRects.overview.pos.y + rowH * float(row) + 1.f),
				Vec(uiRects.overview.size.x - 3.f, rowH - 1.8f)
			);
			const float h = std::max(7.f, rowRect.size.y - 2.f);
			const float w = 24.f;
			return math::Rect(
				Vec(rowRect.pos.x + rowRect.size.x - w - 1.5f, rowRect.pos.y + 0.5f * (rowRect.size.y - h)),
				Vec(w, h)
			);
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

	static float multiplierFromSliderT(float t) {
		const float clampedT = clamp(t, 0.f, 1.f);
		if (clampedT <= kRatioPivotT) {
			const float lt = clampedT / kRatioPivotT;
			return kMinRatio + lt * (kMultiplierPivot - kMinRatio);
		}
		const float rt = (clampedT - kRatioPivotT) / (1.f - kRatioPivotT);
		const float ratio = kMaxMultiplier / kMultiplierPivot;
		return kMultiplierPivot * std::pow(ratio, rt);
	}

	static float sliderTFromMultiplier(float value) {
		const float clampedV = clamp(value, kMinRatio, kMaxMultiplier);
		if (clampedV <= kMultiplierPivot) {
			const float lt = (clampedV - kMinRatio) / (kMultiplierPivot - kMinRatio);
			return kRatioPivotT * lt;
		}
		const float ratio = kMaxMultiplier / kMultiplierPivot;
		const float rt = std::log(clampedV / kMultiplierPivot) / std::log(ratio);
		return kRatioPivotT + (1.f - kRatioPivotT) * clamp(rt, 0.f, 1.f);
	}

	static float divisorFromSliderT(float t) {
		const float clampedT = clamp(t, 0.f, 1.f);
		if (clampedT <= kRatioPivotT) {
			const float lt = clampedT / kRatioPivotT;
			return kMinRatio + lt * (kDivisorPivot - kMinRatio);
		}
		const float rt = (clampedT - kRatioPivotT) / (1.f - kRatioPivotT);
		const float ratio = kMaxDivisor / kDivisorPivot;
		return kDivisorPivot * std::pow(ratio, rt);
	}

	static float sliderTFromDivisor(float value) {
		const float clampedV = clamp(value, kMinRatio, kMaxDivisor);
		if (clampedV <= kDivisorPivot) {
			const float lt = (clampedV - kMinRatio) / (kDivisorPivot - kMinRatio);
			return kRatioPivotT * lt;
		}
		const float ratio = kMaxDivisor / kDivisorPivot;
		const float rt = std::log(clampedV / kDivisorPivot) / std::log(ratio);
		return kRatioPivotT + (1.f - kRatioPivotT) * clamp(rt, 0.f, 1.f);
	}

	static uint64_t hashMix64(uint64_t h, uint64_t v) {
		h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
		return h;
	}

	static uint64_t quantizeHash(float value, float scale) {
		return uint64_t(std::llround(double(value) * double(scale)));
	}

	static uint64_t outputTimingSignature(const chronomaw::OutputState& out, bool running) {
		uint64_t h = 0xcbf29ce484222325ULL;
		h = hashMix64(h, uint64_t(int(out.waveform)));
		h = hashMix64(h, uint64_t(int(out.modifierMode)));
		h = hashMix64(h, quantizeHash(out.multiplier, 1000.f));
		h = hashMix64(h, quantizeHash(out.widthPct, 100.f));
		h = hashMix64(h, quantizeHash(out.levelPct, 100.f));
		h = hashMix64(h, quantizeHash(out.offsetPct, 100.f));
		h = hashMix64(h, quantizeHash(out.phasePct, 100.f));
		h = hashMix64(h, quantizeHash(out.swingPct, 100.f));
		h = hashMix64(h, quantizeHash(out.skewPct, 100.f));
		h = hashMix64(h, quantizeHash(out.rotatePct, 100.f));
		h = hashMix64(h, uint64_t(out.invert ? 1u : 0u));
		h = hashMix64(h, uint64_t(out.muted ? 1u : 0u));
		h = hashMix64(h, uint64_t(running ? 1u : 0u));
		return h;
	}

	static bool canUsePeriodicCache(const chronomaw::OutputState& out, double periodBeats) {
		if (periodBeats <= 0.0 || periodBeats > kFutureCacheMaxPeriodBeats) {
			return false;
		}
		if (out.waveform == chronomaw::WaveformMode::ClassicRandom || out.waveform == chronomaw::WaveformMode::SmoothRandom) {
			return false;
		}
		return true;
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
			if (id == kControlMultiplier) {
				const float t = clamp((local.x - rect.pos.x) / rect.size.x, 0.f, 1.f);
				if (out->modifierMode == chronomaw::ModifierMode::Div) {
					const float div = divisorFromSliderT(t);
					out->multiplier = 1.f / std::max(kMinRatio, div);
				}
				else if (out->modifierMode == chronomaw::ModifierMode::Mult) {
					out->multiplier = multiplierFromSliderT(t);
				}
				return;
			}
			if (id == kControlWidth) {
				out->widthPct = sliderValueFromPoint(rect, local.x, 0.f, 100.f);
				return;
			}
			if (id == kControlSwing) {
				out->swingPct = sliderValueFromPoint(rect, local.x, -100.f, 100.f);
				return;
			}
			if (id == kControlSkew) {
				out->skewPct = sliderValueFromPoint(rect, local.x, -100.f, 100.f);
				return;
			}
			if (id == kControlRotate) {
				out->rotatePct = sliderValueFromPoint(rect, local.x, -100.f, 100.f);
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

		void commitPendingSliderClickIfReady() {
			if (pendingSliderClickId == kControlNone) {
				return;
			}
			const double now = glfwGetTime();
			if (now < pendingSliderCommitTime) {
				return;
			}
			const math::Rect rect = controlRect(pendingSliderClickId);
			if (rect.size.x <= 0.f || rect.size.y <= 0.f) {
				pendingSliderClickId = kControlNone;
				return;
			}
			const float x = clamp(pendingSliderClickX, rect.pos.x, rect.pos.x + rect.size.x);
			applySliderFromPointer(pendingSliderClickId, Vec(x, rect.pos.y + 0.5f * rect.size.y));
			pendingSliderClickId = kControlNone;
		}

		void applyControlClick(int id) {
			chronomaw::OutputState* out = selectedOutputState();
			if (!out) {
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
				return;
			}
			if (id == kControlWavePrev) {
				const int count = chronomaw::waveformCount();
				const int current = chronomaw::waveformIndex(out->waveform);
				out->waveform = chronomaw::waveformFromIndex((current + (count - 1)) % count);
				return;
			}
			if (id == kControlWaveNext) {
				const int count = chronomaw::waveformCount();
				const int current = chronomaw::waveformIndex(out->waveform);
				out->waveform = chronomaw::waveformFromIndex((current + 1) % count);
				return;
			}
			if (id == kControlModeDiv) {
				out->modifierMode = chronomaw::ModifierMode::Div;
				if (out->multiplier > 1.f) {
					out->multiplier = 1.f / out->multiplier;
				}
				return;
			}
			if (id == kControlModeMult) {
				out->modifierMode = chronomaw::ModifierMode::Mult;
				if (out->multiplier < 1.f) {
					out->multiplier = clamp(1.f / std::max(1.f / kMaxDivisor, out->multiplier), kMinRatio, kMaxMultiplier);
				}
				return;
			}
			if (id == kControlModeUtil) {
				out->modifierMode = chronomaw::ModifierMode::Util;
			}
		}

		void openWaveformMenu(const Vec& localClickPos) {
			if (!module) {
				return;
			}
			const int outIdx = selectedOutput();
			ui::Menu* menu = createMenu();
			menu->box.pos = getAbsoluteOffset(Vec(localClickPos.x, localClickPos.y + 2.f));
			menu->addChild(createMenuLabel("Waveform"));
			for (int mode = 0; mode < chronomaw::waveformCount(); ++mode) {
				menu->addChild(createCheckMenuItem(
					waveformName(chronomaw::waveformFromIndex(mode)), "",
					[=]() {
						if (!module) {
							return false;
						}
						const int current = chronomaw::waveformIndex(module->state.live.outputs[size_t(outIdx)].waveform);
						return current == mode;
					},
					[=]() {
						if (!module) {
							return;
						}
						module->state.live.outputs[size_t(outIdx)].waveform = chronomaw::waveformFromIndex(mode);
					}
				));
			}
		}

		void resetControlToDefault(int id) {
			chronomaw::OutputState* out = selectedOutputState();
			if (!out) {
				return;
			}
			if (id == kControlPhase) {
				out->phasePct = 0.f;
				return;
			}
			if (id == kControlMultiplier) {
				out->multiplier = 1.f;
				return;
			}
			if (id == kControlWidth) {
				out->widthPct = 50.f;
				return;
			}
			if (id == kControlSwing) {
				out->swingPct = 0.f;
				return;
			}
			if (id == kControlSkew) {
				out->skewPct = 0.f;
				return;
			}
			if (id == kControlRotate) {
				out->rotatePct = 0.f;
				return;
			}
			if (id == kControlLevel) {
				out->levelPct = 100.f;
				return;
			}
			if (id == kControlOffset) {
				out->offsetPct = 0.f;
				return;
			}
			if (id == kControlProbability) {
				out->probabilityPct = 100.f;
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
					activeSliderDragging = false;
				}
				return;
			}

			const Vec local = e.pos;
			const int row = outputRowAt(local);
			if (row >= 0) {
				if (module) {
					const math::Rect muteRect = overviewMuteRectForRow(row);
					if (muteRect.contains(local)) {
						module->state.live.outputs[size_t(row)].muted = !module->state.live.outputs[size_t(row)].muted;
						e.consume(this);
						return;
					}
				}
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
			if (controlId == kControlMultiplier || controlId == kControlWidth || controlId == kControlPhase || controlId == kControlSwing || controlId == kControlSkew || controlId == kControlRotate || controlId == kControlLevel || controlId == kControlOffset || controlId == kControlProbability) {
				const double now = glfwGetTime();
				if (lastSliderClickId == controlId && lastSliderClickTime >= 0.0 && (now - lastSliderClickTime) <= kCustomDoubleClickWindowSec) {
					pendingSliderClickId = kControlNone;
					resetControlToDefault(controlId);
					activeSliderId = kControlNone;
					activeSliderDragging = false;
					lastSliderClickId = kControlNone;
					lastSliderClickTime = -1.0;
					e.consume(this);
					return;
				}
				activeSliderId = controlId;
				activeSliderX = local.x;
				activeSliderPressPos = local;
				activeSliderDragging = false;
				pendingSliderClickId = controlId;
				pendingSliderClickX = local.x;
				pendingSliderCommitTime = now + kSingleClickCommitDelaySec;
				lastSliderClickId = controlId;
				lastSliderClickTime = now;
				e.consume(this);
				return;
			}
			if (controlId == kControlWaveMenu) {
				openWaveformMenu(local);
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
			const Vec current = currentLocalMousePos();
			const float dx = std::fabs(current.x - activeSliderPressPos.x);
			const float dy = std::fabs(current.y - activeSliderPressPos.y);
			if (!activeSliderDragging) {
				if (dx < 1.5f && dy < 1.5f) {
					return;
				}
				activeSliderDragging = true;
				pendingSliderClickId = kControlNone;
			}
			activeSliderX = clamp(current.x, rect.pos.x, rect.pos.x + rect.size.x);
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
		if (delta == 0.f) {
			return;
		}
		const float step = (delta > 0.f) ? 1.f : -1.f;
			if (controlId == kControlPhase) {
				const float snapped = std::round(out->phasePct);
				out->phasePct = clamp(snapped + step, -100.f, 100.f);
				e.consume(this);
				return;
			}
			if (controlId == kControlMultiplier) {
				if (out->modifierMode == chronomaw::ModifierMode::Div) {
					const float currentDiv = 1.f / std::max(1.f / kMaxDivisor, out->multiplier);
					const float snapped = std::round(currentDiv);
					const float div = clamp(snapped + step, kMinRatio, kMaxDivisor);
					out->multiplier = 1.f / div;
				}
				else if (out->modifierMode == chronomaw::ModifierMode::Mult) {
					const float snapped = std::round(out->multiplier);
					out->multiplier = clamp(snapped + step, kMinRatio, kMaxMultiplier);
				}
				e.consume(this);
				return;
			}
			if (controlId == kControlWidth) {
				const float snapped = std::round(out->widthPct);
				out->widthPct = clamp(snapped + step, 0.f, 100.f);
				e.consume(this);
				return;
			}
			if (controlId == kControlSwing) {
				const float snapped = std::round(out->swingPct);
				out->swingPct = clamp(snapped + step, -100.f, 100.f);
				e.consume(this);
				return;
			}
			if (controlId == kControlSkew) {
				const float snapped = std::round(out->skewPct);
				out->skewPct = clamp(snapped + step, -100.f, 100.f);
				e.consume(this);
				return;
			}
			if (controlId == kControlRotate) {
				const float snapped = std::round(out->rotatePct);
				out->rotatePct = clamp(snapped + step, -100.f, 100.f);
				e.consume(this);
				return;
			}
		if (controlId == kControlLevel) {
			const float snapped = std::round(out->levelPct);
			out->levelPct = clamp(snapped + step, 0.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlOffset) {
			const float snapped = std::round(out->offsetPct);
			out->offsetPct = clamp(snapped + step, -100.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlProbability) {
			const float snapped = std::round(out->probabilityPct);
			out->probabilityPct = clamp(snapped + step, 0.f, 100.f);
			e.consume(this);
			return;
		}
		if (controlId == kControlSeedInc || controlId == kControlSeedDec) {
			const int seedStep = (delta > 0.f) ? 1 : -1;
			if (seedStep < 0 && out->randomSeed > 0u) {
				--out->randomSeed;
			}
			if (seedStep > 0) {
				++out->randomSeed;
			}
				e.consume(this);
			}
		}

		void updateTimelineFuturePreview() {
			if (!module) {
				return;
			}
			const float phaseNow = module->timelinePhaseBeats.load(std::memory_order_relaxed);
			const uint64_t cycleNow = module->timelineCycleCount.load(std::memory_order_relaxed);
			const float bpmNow = clamp(module->timelineBpm.load(std::memory_order_relaxed), chronomaw::kMinBpm, chronomaw::kMaxBpm);
			const bool runningNow = module->timelineRunning.load(std::memory_order_relaxed);
			const float beatsPerSec = bpmNow / 60.f;
			const double baseNow = double(cycleNow) + double(phaseNow);
			const double dtBeats = double(beatsPerSec) * double(Chronomaw::kTimelineCaptureIntervalSec);
			for (int ch = 0; ch < chronomaw::kNumOutputs; ++ch) {
				const chronomaw::OutputState& outState = module->state.live.outputs[size_t(ch)];
				const double timingOffset = double(module->timelineTimingPhaseOffsets[size_t(ch)].load(std::memory_order_relaxed));
				const double effMul = std::max(chronomaw::effectiveTimingMultiplier(outState), 1.0 / 16384.0);
				const double periodBeats = 1.0 / effMul;
				const bool useCache = canUsePeriodicCache(outState, periodBeats);
				FuturePeriodCache& cache = futureCaches[size_t(ch)];
				if (useCache) {
					const uint64_t sig = outputTimingSignature(outState, runningNow);
					if (!cache.valid || cache.signature != sig || std::fabs(cache.periodBeats - periodBeats) > 1e-9) {
						cache.valid = true;
						cache.signature = sig;
						cache.periodBeats = periodBeats;
						const bool narrowWidth = (outState.widthPct <= 8.f || outState.widthPct >= 92.f);
						const int sampleCount = narrowWidth ? kFuturePeriodSamplesNarrow : kFuturePeriodSamplesDefault;
						cache.values.resize(size_t(sampleCount));
						for (int i = 0; i < sampleCount; ++i) {
							const double t = (double(i) + 0.5) / double(sampleCount);
							const double basePhase = t * periodBeats;
							const double phaseBase = basePhase + timingOffset;
							const double rawPhase = chronomaw::rawTimingPhase(outState, phaseBase);
							const double rawCycle = std::floor(rawPhase);
							const uint64_t chCycle = (rawCycle > 0.0) ? uint64_t(rawCycle) : 0u;
							const float v = chronomaw::renderOutputVoltage(outState, runningNow, phaseBase, chCycle);
							cache.values[size_t(i)] = clamp(v, chronomaw::kOutputMinV, chronomaw::kOutputMaxV);
						}
					}
				}
				else {
					cache.valid = false;
					cache.values.clear();
				}

				int cacheSampleCount = 0;
				double cachePos = 0.0;
				double cacheStep = 0.0;
				if (cache.valid && !cache.values.empty()) {
					cacheSampleCount = int(cache.values.size());
					const double scale = double(cacheSampleCount) / cache.periodBeats;
					const double startPhase = baseNow + 0.5 * dtBeats + timingOffset;
					cachePos = std::fmod(startPhase * scale, double(cacheSampleCount));
					if (cachePos < 0.0) {
						cachePos += double(cacheSampleCount);
					}
					cacheStep = dtBeats * scale;
				}

				for (int step = 0; step < Chronomaw::kTimelineFutureSize; ++step) {
					// Store a single center sample per timeline slot; interval summarization happens in draw.
					const double phase = baseNow + (double(step) + 0.5) * dtBeats;
					float v = 0.f;
					if (cache.valid) {
						if (cacheSampleCount > 0) {
							const int i0 = int(cachePos);
							const int i1 = (i0 + 1 >= cacheSampleCount) ? 0 : (i0 + 1);
							const float frac = float(cachePos - std::floor(cachePos));
							const float v0 = cache.values[size_t(i0)];
							const float v1 = cache.values[size_t(i1)];
							v = v0 + (v1 - v0) * frac;
							cachePos += cacheStep;
							while (cachePos >= double(cacheSampleCount)) {
								cachePos -= double(cacheSampleCount);
							}
							while (cachePos < 0.0) {
								cachePos += double(cacheSampleCount);
							}
						}
					}
					else {
						const double phaseBase = phase + timingOffset;
						const double rawPhase = chronomaw::rawTimingPhase(outState, phaseBase);
						const double rawCycle = std::floor(rawPhase);
						const uint64_t chCycle = (rawCycle > 0.0) ? uint64_t(rawCycle) : 0u;
						v = chronomaw::renderOutputVoltage(outState, runningNow, phaseBase, chCycle);
					}
					module->timelineFutureOutput[size_t(ch)][size_t(step)].store(clamp(v, chronomaw::kOutputMinV, chronomaw::kOutputMaxV), std::memory_order_relaxed);
				}
			}
		}

		void step() override {
			commitPendingSliderClickIfReady();
			const double now = glfwGetTime();
			if (lastFuturePreviewUpdateSec < 0.0 || (now - lastFuturePreviewUpdateSec) >= kFuturePreviewUpdateIntervalSec) {
				updateTimelineFuturePreview();
				lastFuturePreviewUpdateSec = now;
			}
			Widget::step();
		}

	void drawSlider(const DrawArgs& args, const math::Rect& rect, float value, float minV, float maxV, const std::string& label, int controlId) {
		drawRectFilled(args, rect, chronomawRgb(17, 26, 37, 210), chronomawRgb(90, 122, 148, 176));
		float t = (maxV <= minV) ? 0.f : clamp((value - minV) / (maxV - minV), 0.f, 1.f);
		if (controlId == kControlMultiplier) {
			const chronomaw::OutputState* out = selectedOutputState();
			if (out && out->modifierMode == chronomaw::ModifierMode::Div) {
				const float div = 1.f / std::max(1.f / kMaxDivisor, value);
				t = sliderTFromDivisor(div);
			}
			else if (out && out->modifierMode == chronomaw::ModifierMode::Util) {
				t = 0.f;
			}
			else {
				t = sliderTFromMultiplier(value);
			}
		}
		math::Rect fillRect(rect.pos, Vec(rect.size.x * t, rect.size.y));
		if (fillRect.size.x > 0.5f && fillRect.size.y > 0.5f) {
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, fillRect.pos.x, fillRect.pos.y, fillRect.size.x, fillRect.size.y, 4.0f);
			NVGpaint activePaint = nvgLinearGradient(
				args.vg,
				fillRect.pos.x,
				fillRect.pos.y,
				fillRect.pos.x + fillRect.size.x,
				fillRect.pos.y + fillRect.size.y,
				chronomawRgb(186, 88, 255, 236),
				chronomawRgb(88, 230, 255, 232)
			);
			nvgFillPaint(args.vg, activePaint);
			nvgFill(args.vg);

			const float sheenH = std::max(1.0f, fillRect.size.y * 0.33f);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, fillRect.pos.x + 0.7f, fillRect.pos.y + 0.4f, std::max(0.f, fillRect.size.x - 1.4f), sheenH, 3.0f);
			NVGpaint sheenPaint = nvgLinearGradient(
				args.vg,
				fillRect.pos.x,
				fillRect.pos.y,
				fillRect.pos.x,
				fillRect.pos.y + sheenH,
				chronomawRgb(241, 220, 255, 116),
				chronomawRgb(220, 252, 255, 14)
			);
			nvgFillPaint(args.vg, sheenPaint);
			nvgFill(args.vg);
		}
			std::string valueLabel = string::f("%.1f", value);
			std::string leftLabel = label;
			if (controlId == kControlMultiplier) {
				const chronomaw::OutputState* out = selectedOutputState();
				if (out) {
					if (out->modifierMode == chronomaw::ModifierMode::Div) {
						const float div = 1.f / std::max(1.f / kMaxDivisor, value);
						valueLabel = string::f("/%.0f", div);
						leftLabel = "Divider";
					}
					else if (out->modifierMode == chronomaw::ModifierMode::Mult) {
						valueLabel = string::f("x%.0f", value);
						leftLabel = "Multiplier";
					}
					else {
						valueLabel = "UTILITY";
						leftLabel = "Utility";
					}
				}
			}
			const float midY = rect.pos.y + 0.5f * rect.size.y;
			drawLabelOutlined(args, rect.pos.x + rect.size.x * 0.25f, midY, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 7.8f, chronomawRgb(182, 220, 240, 236), leftLabel);
			drawLabelOutlined(args, rect.pos.x + rect.size.x * 0.75f, midY, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.2f, chronomawRgb(232, 246, 255, 240), valueLabel);
			addControl(controlId, rect);
		}

	void drawToggle(const DrawArgs& args, const math::Rect& rect, const std::string& label, bool on, int controlId) {
		drawRectFilled(args, rect, on ? chronomawRgb(45, 97, 67, 220) : chronomawRgb(38, 34, 40, 210), on ? chronomawRgb(147, 228, 172, 204) : chronomawRgb(120, 112, 124, 184));
		drawLabel(args, rect.pos.x + 4.f, rect.pos.y + 0.5f * rect.size.y, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE, 8.0f, chronomawRgb(228, 240, 248, 242), label);
		drawLabel(args, rect.pos.x + rect.size.x - 4.f, rect.pos.y + 0.5f * rect.size.y, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE, 8.0f, chronomawRgb(236, 252, 255, 244), on ? "ON" : "OFF");
		addControl(controlId, rect);
	}

	void drawTimelineLaneTrace(const DrawArgs& args, const math::Rect& rect, int channel, bool selected, float nowFrac) {
		if (!module || channel < 0 || channel >= chronomaw::kNumOutputs || rect.size.x <= 3.f || rect.size.y <= 2.f) {
			return;
		}
		struct IntervalSummary {
			float avg = 0.f;
			float min = 0.f;
			float max = 0.f;
		};
		nvgSave(args.vg);
		nvgScissor(args.vg, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
		const int hist = Chronomaw::kTimelineHistorySize;
		const int writePos = module->timelineWritePos.load(std::memory_order_relaxed);
		const int latestIdx = (writePos - 1 + hist) % hist;
		const float nowX = rect.pos.x + clamp(nowFrac, 0.05f, 0.95f) * rect.size.x;
		const float historyWidth = std::max(4.f, nowX - rect.pos.x);
		const float futureWidth = std::max(0.f, rect.pos.x + rect.size.x - nowX);
		const float dtSec = Chronomaw::kTimelineCaptureIntervalSec;
		const float historyCapSec = float(Chronomaw::kTimelineHistorySize - 1) * dtSec;
		const float futureCapSec = float(Chronomaw::kTimelineFutureSize) * dtSec;
		const float zoomKnob = clamp(module->params[Chronomaw::TIMELINE_ZOOM_PARAM].getValue(), -1.f, 1.f);
		const float zoomMul = (zoomKnob >= 0.f) ? std::pow(16.f, zoomKnob) : std::pow(8.f, zoomKnob);
		// Enforce one time scale (seconds-per-pixel) across the full lane.
		const float baseSecPerPixel = std::min(historyCapSec / historyWidth, (futureWidth > 0.f) ? (futureCapSec / futureWidth) : (historyCapSec / historyWidth));
		const float secPerPixel = baseSecPerPixel / std::max(0.125f, zoomMul);

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
		auto historySummaryAtAgeSteps = [&](float ageStepsFloat, float halfSpanSteps) {
			IntervalSummary summary;
			if (halfSpanSteps <= 0.f) {
				const float v = historyValueAtAgeSteps(ageStepsFloat);
				summary.avg = v;
				summary.min = v;
				summary.max = v;
				return summary;
			}
			const float ageMin = clamp(ageStepsFloat - halfSpanSteps, 0.f, float(Chronomaw::kTimelineHistorySize - 1));
			const float ageMax = clamp(ageStepsFloat + halfSpanSteps, 0.f, float(Chronomaw::kTimelineHistorySize - 1));
			const int i0 = int(std::floor(ageMin));
			const int i1 = int(std::ceil(ageMax));
			float sum = 0.f;
			int count = 0;
			float minV = chronomaw::kOutputMaxV;
			float maxV = chronomaw::kOutputMinV;
			for (int age = i0; age <= i1; ++age) {
				const int clampedAge = clamp(age, 0, Chronomaw::kTimelineHistorySize - 1);
				const int idx = (latestIdx - clampedAge + hist) % hist;
				const float avgV = module->timelineOutputHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float minSample = module->timelineOutputHistoryMin[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float maxSample = module->timelineOutputHistoryMax[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				sum += avgV;
				minV = std::min(minV, minSample);
				maxV = std::max(maxV, maxSample);
				++count;
			}
			if (count <= 0) {
				const float v = historyValueAtAgeSteps(ageStepsFloat);
				summary.avg = v;
				summary.min = v;
				summary.max = v;
				return summary;
			}
			summary.avg = sum / float(count);
			summary.min = minV;
			summary.max = maxV;
			return summary;
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
		auto futureSummaryAtSteps = [&](float centerSteps, float halfSpanSteps) {
			IntervalSummary summary;
			const float span = std::max(halfSpanSteps, 0.f);
			const float lo = std::max(0.f, centerSteps - span);
			const float hi = std::max(lo, centerSteps + span);
			const int sampleCount = 4;
			float sum = 0.f;
			float minV = chronomaw::kOutputMaxV;
			float maxV = chronomaw::kOutputMinV;
			for (int i = 0; i < sampleCount; ++i) {
				const float t = (sampleCount <= 1) ? 0.5f : (float(i) + 0.5f) / float(sampleCount);
				const float step = lo + (hi - lo) * t;
				const float v = futureValueAtSteps(step);
				sum += v;
				minV = std::min(minV, v);
				maxV = std::max(maxV, v);
			}
			summary.avg = sum / float(sampleCount);
			summary.min = minV;
			summary.max = maxV;
			return summary;
		};
		const float intervalHalfSteps = std::max(0.5f * secPerPixel / dtSec, 0.25f);
		const float envelopeThresholdV = 0.25f;
		const bool sampledFutureTimeline = module->state.ui.sampledFutureTimeline;

		struct PhaseBinStats {
			float sum = 0.f;
			float min = chronomaw::kOutputMaxV;
			float max = chronomaw::kOutputMinV;
			int count = 0;
		};
		static constexpr int kPhaseBins = 32;
		std::array<PhaseBinStats, kPhaseBins> phaseBins {};
		std::array<float, kPhaseBins> phaseBinAvg {};
		std::array<bool, kPhaseBins> phaseBinHasData {};
		int coverageBins = 0;
		int phasePointCount = 0;
		if (sampledFutureTimeline) {
			for (int age = 0; age < Chronomaw::kTimelineHistorySize; ++age) {
				const int idx = (latestIdx - age + hist) % hist;
				const float phase = module->timelineOutputPhaseHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float value = module->timelineOutputHistory[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float minValue = module->timelineOutputHistoryMin[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float maxValue = module->timelineOutputHistoryMax[size_t(channel)][size_t(idx)].load(std::memory_order_relaxed);
				const float normPhase = wrapPhase01(phase);
				int bin = int(std::floor(normPhase * float(kPhaseBins)));
				bin = clamp(bin, 0, kPhaseBins - 1);
				PhaseBinStats& b = phaseBins[size_t(bin)];
				b.sum += value;
				b.min = std::min(b.min, minValue);
				b.max = std::max(b.max, maxValue);
				b.count += 1;
				phasePointCount += 1;
			}
			for (int i = 0; i < kPhaseBins; ++i) {
				if (phaseBins[size_t(i)].count > 0) {
					phaseBinAvg[size_t(i)] = phaseBins[size_t(i)].sum / float(phaseBins[size_t(i)].count);
					phaseBinHasData[size_t(i)] = true;
					coverageBins += 1;
				}
			}
		}
		const bool sampledProjectionStable = sampledFutureTimeline && phasePointCount >= 64 && coverageBins >= 26;

		const int visibleCount = std::max(8, int(historyWidth));
		const float historyStopX = nowX;
		const float futureStartX = nowX;
		std::vector<Vec> historyPoints;
		std::vector<IntervalSummary> historySummaries;
		historyPoints.reserve(size_t(visibleCount));
		historySummaries.reserve(size_t(visibleCount));
		nvgBeginPath(args.vg);
		bool moved = false;
		for (int i = 0; i < visibleCount; ++i) {
			const float t = (visibleCount <= 1) ? 0.f : (float(i) / float(visibleCount - 1));
			const float x = rect.pos.x + t * historyWidth;
			if (x > historyStopX) {
				break;
			}
			const float ageSec = (historyWidth - (x - rect.pos.x)) * secPerPixel;
			const float ageStepsFloat = ageSec / dtSec;
			const IntervalSummary s = historySummaryAtAgeSteps(ageStepsFloat, intervalHalfSteps);
			const float y = voltsToY(s.avg);
			if (!moved) {
				nvgMoveTo(args.vg, x, y);
				moved = true;
			}
			else {
				nvgLineTo(args.vg, x, y);
			}
			historyPoints.emplace_back(x, y);
			historySummaries.push_back(s);
		}
		if (moved) {
			nvgStrokeWidth(args.vg, selected ? 1.35f : 1.0f);
			nvgStrokeColor(args.vg, selected ? chronomawRgb(244, 249, 255, 238) : chronomawRgb(188, 206, 224, 182));
			nvgStroke(args.vg);
		}
		const bool drawHistoryEnvelope = intervalHalfSteps >= 1.1f;
		for (size_t i = 0; i < historyPoints.size(); ++i) {
			if (!drawHistoryEnvelope) {
				continue;
			}
			const IntervalSummary& s = historySummaries[i];
			if ((s.max - s.min) <= envelopeThresholdV) {
				continue;
			}
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, historyPoints[i].x, voltsToY(s.min));
			nvgLineTo(args.vg, historyPoints[i].x, voltsToY(s.max));
			nvgStrokeWidth(args.vg, selected ? 0.95f : 0.8f);
			nvgStrokeColor(args.vg, selected ? chronomawRgb(220, 238, 255, 120) : chronomawRgb(186, 208, 230, 92));
			nvgStroke(args.vg);
		}

		auto sampledFutureSummaryAtPhase = [&](float targetPhase, IntervalSummary* outSummary) {
			if (!outSummary || !sampledProjectionStable) {
				return false;
			}
			const float phase = wrapPhase01(targetPhase);
			const float phasePos = phase * float(kPhaseBins);
			int i0 = int(std::floor(phasePos));
			i0 = clamp(i0, 0, kPhaseBins - 1);
			int i1 = i0 + 1;
			if (i1 >= kPhaseBins) {
				i1 = 0;
			}
			const float frac = phasePos - float(i0);
			if (phaseBinHasData[size_t(i0)] && phaseBinHasData[size_t(i1)]) {
				const float avg = phaseBinAvg[size_t(i0)] + (phaseBinAvg[size_t(i1)] - phaseBinAvg[size_t(i0)]) * frac;
				outSummary->avg = avg;
				outSummary->min = avg;
				outSummary->max = avg;
				return true;
			}
			if (phaseBinHasData[size_t(i0)]) {
				const float avg = phaseBinAvg[size_t(i0)];
				outSummary->avg = avg;
				outSummary->min = avg;
				outSummary->max = avg;
				return true;
			}
			if (phaseBinHasData[size_t(i1)]) {
				const float avg = phaseBinAvg[size_t(i1)];
				outSummary->avg = avg;
				outSummary->min = avg;
				outSummary->max = avg;
				return true;
			}
			for (int radius = 1; radius < kPhaseBins / 2; ++radius) {
				const int left = (i0 - radius + kPhaseBins) % kPhaseBins;
				const int right = (i0 + radius) % kPhaseBins;
				if (phaseBinHasData[size_t(left)]) {
					const float avg = phaseBinAvg[size_t(left)];
					outSummary->avg = avg;
					outSummary->min = avg;
					outSummary->max = avg;
					return true;
				}
				if (phaseBinHasData[size_t(right)]) {
					const float avg = phaseBinAvg[size_t(right)];
					outSummary->avg = avg;
					outSummary->min = avg;
					outSummary->max = avg;
					return true;
				}
			}
			return false;
		};

		// Draw future projection to the right of "now".
		if (futureWidth > 0.f) {
			nvgBeginPath(args.vg);
			const float latestV = module->timelineOutputHistory[size_t(channel)][size_t(latestIdx)].load(std::memory_order_relaxed);
			const float futureStartT = (futureWidth <= 0.f) ? 0.f : clamp((futureStartX - nowX) / futureWidth, 0.f, 1.f);
			const float futureStartSec = futureStartT * futureWidth * secPerPixel;
			const float futureStartStep = std::max(0.f, (futureStartSec / dtSec));
			float startV = latestV;
			if (futureStartT > 0.f) {
				if (sampledFutureTimeline) {
					IntervalSummary sampledStart;
					const float phaseNow = module->timelinePhaseBeats.load(std::memory_order_relaxed);
					const float bpmNow = clamp(module->timelineBpm.load(std::memory_order_relaxed), chronomaw::kMinBpm, chronomaw::kMaxBpm);
					const float offset = module->timelineTimingPhaseOffsets[size_t(channel)].load(std::memory_order_relaxed);
					const float futureBeats = (bpmNow / 60.f) * futureStartSec;
					const float targetPhase = wrapPhase01(phaseNow + futureBeats + offset);
					if (sampledFutureSummaryAtPhase(targetPhase, &sampledStart)) {
						startV = sampledStart.avg;
					}
				}
				else {
					startV = futureValueAtSteps(futureStartStep);
				}
			}
			nvgMoveTo(args.vg, futureStartX, voltsToY(startV));
			const int futureCount = std::max(8, int(futureWidth));
			std::vector<Vec> futurePoints;
			std::vector<IntervalSummary> futureSummaries;
			futurePoints.reserve(size_t(futureCount));
			futureSummaries.reserve(size_t(futureCount));
			for (int i = 0; i < futureCount; ++i) {
				const float t = (futureCount <= 1) ? 1.f : float(i + 1) / float(futureCount);
				const float x = nowX + t * futureWidth;
				if (x <= futureStartX) {
					continue;
				}
				const float futureSec = t * futureWidth * secPerPixel;
				const float futureStepFloat = std::max(0.f, (futureSec / dtSec));
				IntervalSummary s;
				bool hasSample = true;
				if (sampledFutureTimeline) {
					const float phaseNow = module->timelinePhaseBeats.load(std::memory_order_relaxed);
					const float bpmNow = clamp(module->timelineBpm.load(std::memory_order_relaxed), chronomaw::kMinBpm, chronomaw::kMaxBpm);
					const float offset = module->timelineTimingPhaseOffsets[size_t(channel)].load(std::memory_order_relaxed);
					const float futureBeats = (bpmNow / 60.f) * futureSec;
					const float targetPhase = wrapPhase01(phaseNow + futureBeats + offset);
					hasSample = sampledFutureSummaryAtPhase(targetPhase, &s);
				}
				else {
					s = futureSummaryAtSteps(futureStepFloat, intervalHalfSteps);
				}
				if (!hasSample) {
					continue;
				}
				const float v = s.avg;
				nvgLineTo(args.vg, x, voltsToY(v));
				futurePoints.emplace_back(x, voltsToY(v));
				futureSummaries.push_back(s);
			}
			nvgStrokeWidth(args.vg, selected ? 1.0f : 0.85f);
			nvgStrokeColor(args.vg, sampledFutureTimeline
				? (selected ? chronomawRgb(240, 248, 255, 236) : chronomawRgb(140, 188, 210, 162))
				: (selected ? chronomawRgb(244, 249, 255, 238) : chronomawRgb(188, 206, 224, 182)));
			nvgStroke(args.vg);
			const bool drawFutureEnvelope = !sampledFutureTimeline;
			for (size_t i = 0; i < futurePoints.size(); ++i) {
				if (!drawFutureEnvelope) {
					continue;
				}
				const IntervalSummary& s = futureSummaries[i];
				if ((s.max - s.min) <= envelopeThresholdV) {
					continue;
				}
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, futurePoints[i].x, voltsToY(s.min));
				nvgLineTo(args.vg, futurePoints[i].x, voltsToY(s.max));
				nvgStrokeWidth(args.vg, selected ? 0.88f : 0.72f);
				nvgStrokeColor(args.vg, selected ? chronomawRgb(220, 238, 255, 96) : chronomawRgb(186, 208, 230, 72));
				nvgStroke(args.vg);
			}
		}
		nvgRestore(args.vg);
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
				const bool muted = module ? module->state.live.outputs[size_t(i)].muted : false;
				const math::Rect muteRect = overviewMuteRectForRow(i);
				drawRectFilled(
					args,
					muteRect,
					muted ? chronomawRgb(72, 26, 30, 220) : chronomawRgb(24, 58, 38, 220),
					muted ? chronomawRgb(214, 108, 112, 210) : chronomawRgb(132, 224, 164, 210)
				);
				drawLabel(
					args,
					muteRect.pos.x + 0.5f * muteRect.size.x,
					muteRect.pos.y + 0.5f * muteRect.size.y,
					NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE,
					7.5f,
					chronomawRgb(238, 246, 252, 242),
					muted ? "MUTE" : "ON"
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
			// Match TD.Scope read-head color treatment.
			nvgStrokeWidth(args.vg, 2.2f);
			nvgStrokeColor(args.vg, chronomawRgb(244, 220, 96, 128));
			nvgStroke(args.vg);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, nowX, timelineRect.pos.y + 1.2f);
			nvgLineTo(args.vg, nowX, timelineRect.pos.y + timelineRect.size.y - 1.2f);
			nvgStrokeWidth(args.vg, 1.15f);
			nvgStrokeColor(args.vg, chronomawRgb(244, 220, 96, 128));
			nvgStroke(args.vg);
		drawLabel(args, timelineRect.pos.x + 4.f, timelineRect.pos.y - 2.5f, NVG_ALIGN_LEFT | NVG_ALIGN_BOTTOM, 9.4f, chronomawRgb(180, 226, 251, 236), "Timeline");
	}

		void drawInspector(const DrawArgs& args, const math::Rect& inspectorRect) {
			clearControls();
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, inspectorRect.pos.x, inspectorRect.pos.y, inspectorRect.size.x, inspectorRect.size.y, 4.0f);
			nvgFillColor(args.vg, chronomawRgb(10, 16, 22, 170));
			nvgFill(args.vg);
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
					const float modeGap = 2.f;
					const float modeW = (contentW - 2.f * modeGap) / 3.f;
					const math::Rect divRect(Vec(contentX, y), Vec(modeW, rowH));
					const math::Rect multRect(Vec(contentX + modeW + modeGap, y), Vec(modeW, rowH));
					const math::Rect utilRect(Vec(contentX + 2.f * (modeW + modeGap), y), Vec(modeW, rowH));
					const auto drawModeButton = [&](const math::Rect& r, const char* txt, bool active, int id) {
						drawRectFilled(
							args,
							r,
							active ? chronomawRgb(36, 66, 94, 220) : chronomawRgb(16, 24, 34, 200),
							active ? chronomawRgb(168, 226, 255, 228) : chronomawRgb(90, 122, 148, 176)
						);
						drawLabel(args, r.pos.x + 0.5f * r.size.x, r.pos.y + 0.5f * r.size.y, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 7.9f, chronomawRgb(234, 246, 255, 240), txt);
						addControl(id, r);
					};
					drawModeButton(divRect, "DIV", outState->modifierMode == chronomaw::ModifierMode::Div, kControlModeDiv);
					drawModeButton(multRect, "MULT", outState->modifierMode == chronomaw::ModifierMode::Mult, kControlModeMult);
					drawModeButton(utilRect, "UTIL", outState->modifierMode == chronomaw::ModifierMode::Util, kControlModeUtil);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->multiplier, 1.f / 16384.f, 192.f, "Multiplier", kControlMultiplier);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->widthPct, 0.f, 100.f, "Width %", kControlWidth);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->phasePct, -100.f, 100.f, "Phase %", kControlPhase);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->swingPct, -100.f, 100.f, "Swing %", kControlSwing);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->skewPct, -100.f, 100.f, "Skew %", kControlSkew);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->rotatePct, -100.f, 100.f, "Rotate %", kControlRotate);
				}
				else if (tabSel == 1) {
					drawRectFilled(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), chronomawRgb(17, 26, 37, 210), chronomawRgb(90, 122, 148, 176));
					const float btnW = 12.f;
					const float btnPad = 1.5f;
					const math::Rect wavePrevRect(Vec(contentX + btnPad, y + 1.f), Vec(btnW, rowH - 2.f));
					const math::Rect waveNextRect(Vec(contentX + btnPad + btnW + 1.5f, y + 1.f), Vec(btnW, rowH - 2.f));
					const float menuIconX = contentX + contentW - 6.f;
					const float menuLeft = waveNextRect.pos.x + waveNextRect.size.x + 3.f;
					const float menuW = std::max(8.f, menuIconX - menuLeft - 2.f);
					addControl(kControlWaveMenu, math::Rect(Vec(menuLeft, y), Vec(menuW, rowH)));
					addControl(kControlWavePrev, wavePrevRect);
					addControl(kControlWaveNext, waveNextRect);
					drawLabel(args, contentX + 0.5f * contentW, y + 0.5f * rowH, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE, 8.2f, chronomawRgb(232, 246, 255, 238), std::string("Waveform: ") + waveformName(outState->waveform));
					const auto drawCircleButton = [&](const math::Rect& r, bool right) {
						const Vec c(r.pos.x + 0.5f * r.size.x, r.pos.y + 0.5f * r.size.y);
						const float radius = std::max(2.2f, std::min(r.size.x, r.size.y) * 0.38f);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, c.x, c.y, radius);
						nvgFillColor(args.vg, chronomawRgb(25, 34, 44, 220));
						nvgFill(args.vg);
						nvgBeginPath(args.vg);
						nvgCircle(args.vg, c.x, c.y, radius);
						nvgStrokeWidth(args.vg, 0.9f);
						nvgStrokeColor(args.vg, chronomawRgb(108, 140, 168, 188));
						nvgStroke(args.vg);
						drawModeStepTriangle(args, c, radius * 0.44f, right, chronomawRgb(225, 232, 240, 244));
					};
					drawCircleButton(wavePrevRect, false);
					drawCircleButton(waveNextRect, true);
					const Vec menuIconCenter(menuIconX, y + 0.5f * rowH);
					drawMenuGlyph(args, menuIconCenter, 2.0f, 1.8f, chronomawRgb(225, 232, 240, 240));
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->levelPct, 0.f, 100.f, "Level %", kControlLevel);
					y += rowH + 3.f;
					drawSlider(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), outState->offsetPct, -100.f, 100.f, "Offset %", kControlOffset);
					y += rowH + 3.f;
					drawToggle(args, math::Rect(Vec(contentX, y), Vec(contentW, rowH)), "Invert", outState->invert, kControlInvert);
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
	Vec timelineZoomPos(152.0f, 16.0f);
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
	applyPointOverride("TIMELINE_ZOOM", &timelineZoomPos);
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
	addParam(createParamCentered<BefacoTinyKnobWhite>(mm2px(timelineZoomPos), module, Chronomaw::TIMELINE_ZOOM_PARAM));

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

void ChronomawWidget::appendContextMenu(Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	auto* chronomaw = dynamic_cast<Chronomaw*>(module);
	if (!chronomaw) {
		return;
	}
	menu->addChild(new MenuSeparator());
	menu->addChild(createCheckMenuItem(
		"Sampled Future Timeline",
		"",
		[=]() {
			return chronomaw->state.ui.sampledFutureTimeline;
		},
		[=]() {
			chronomaw->state.ui.sampledFutureTimeline = !chronomaw->state.ui.sampledFutureTimeline;
		}
	));
}

Model* modelChronomaw = createModel<Chronomaw, ChronomawWidget>("Chronomaw");
