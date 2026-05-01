#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <array>
#include <atomic>
#include <cmath>

namespace {

constexpr int kWyrmPointCountDefault = 32;
constexpr int kWyrmPointCountMax = 128;
constexpr int kWyrmTableSize = 2048;
constexpr int kWyrmMaxChannels = 16;

enum WyrmShapeId {
	SHAPE_SINE = 0,
	SHAPE_TRIANGLE,
	SHAPE_SAW,
	SHAPE_REV_SAW,
	SHAPE_SQUARE,
	SHAPE_COUNT
};

const char* const kWyrmShapeLabels[SHAPE_COUNT] = {
	"Sine",
	"Triangle",
	"Saw",
	"Reverse Saw",
	"Square"
};

constexpr float kWyrmAudioMinHz = 20.f;
constexpr float kWyrmAudioMaxHz = 20000.f;
constexpr float kWyrmLfoMinHz = 0.01f;
constexpr float kWyrmLfoMaxHz = 100.f;
constexpr float kWyrmFoldMakeupGain = 1.0f / std::tanh(1.f);
constexpr float kWyrmSlitherMaxOffset = 0.42f;

inline float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

inline float wrap01(float x) {
	return x - std::floor(x);
}

inline float wrap01Fast(float x) {
	if (x >= 1.f) {
		x -= 1.f;
	}
	else if (x < 0.f) {
		x += 1.f;
	}
	return x;
}

inline float fastTanh(float x) {
	const float x2 = x * x;
	if (x2 < 9.f) {
		return x * (27.f + x2) / (27.f + 9.f * x2);
	}
	return (x > 0.f) ? 1.f : -1.f;
}

inline float softClip(float x) {
	return fastTanh(x);
}

inline float wyrmBaseFrequencyFromKnob(float knobNorm, bool lfoMode) {
	const float minFreq = lfoMode ? kWyrmLfoMinHz : kWyrmAudioMinHz;
	const float maxFreq = lfoMode ? kWyrmLfoMaxHz : kWyrmAudioMaxHz;
	return minFreq * std::pow(maxFreq / minFreq, clamp01(knobNorm));
}

inline float wyrmKnobValueForFrequency(float hz, bool lfoMode) {
	const float minFreq = lfoMode ? kWyrmLfoMinHz : kWyrmAudioMinHz;
	const float maxFreq = lfoMode ? kWyrmLfoMaxHz : kWyrmAudioMaxHz;
	hz = clamp(hz, minFreq, maxFreq);
	return std::log(hz / minFreq) / std::log(maxFreq / minFreq);
}

inline float foldWave(float x, float amount) {
	if (amount <= 1e-5f) {
		return x;
	}
	const float drive = 1.f + 5.5f * amount;
	const float d = x * drive;
	return std::sin(0.5f * float(M_PI) * d);
}

inline float catmullPeriodic(const std::array<float, kWyrmPointCountMax>& points, int pointCount, float phase) {
	const int count = clamp(pointCount, 2, kWyrmPointCountMax);
	const float p = wrap01(phase) * float(count);
	const int i1 = int(std::floor(p)) % count;
	const int i0 = (i1 + count - 1) % count;
	const int i2 = (i1 + 1) % count;
	const int i3 = (i1 + 2) % count;
	const float t = p - std::floor(p);
	const float p0 = points[i0];
	const float p1 = points[i1];
	const float p2 = points[i2];
	const float p3 = points[i3];
	const float t2 = t * t;
	const float t3 = t2 * t;
	return 0.5f * ((2.f * p1) + (-p0 + p2) * t + (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 + (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);
}

inline float slitherOffset(float phase, float travelPhase, float amount) {
	const float shapedAmount = amount * amount;
	return kWyrmSlitherMaxOffset * shapedAmount * std::sin(2.f * float(M_PI) * (wrap01(phase) - wrap01(travelPhase)));
}

inline float slitherSpeedFactor(float speedKnob) {
	// Midpoint (0.5) preserves previous baseline speed. Left slows, right speeds up.
	return std::pow(2.f, (clamp01(speedKnob) - 0.5f) * 4.f);
}

} // namespace

struct Wyrm;

struct WyrmFreqQuantity final : ParamQuantity {
	float getDisplayValue() override;
	void setDisplayValue(float displayValue) override;
	std::string getDisplayValueString() override;
};

struct Wyrm : Module {
	enum ParamId {
		FREQ_PARAM,
		FINE_PARAM,
		FM_ATTEN_PARAM,
		FOLD_PARAM,
		SLITHER_PARAM,
		SLITHER_SPEED_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		VOCT_INPUT,
		FM_INPUT,
		SYNC_INPUT,
		FOLD_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		RAW_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	std::array<std::atomic<float>, kWyrmPointCountMax> wavePoints {};
	std::array<float, kWyrmTableSize> wavetable {};
	std::atomic<uint32_t> waveVersion {1};
	uint32_t appliedWaveVersion = 0;
	std::array<float, kWyrmMaxChannels> phase {};
	std::array<float, kWyrmMaxChannels> slitherPhase {};
	std::array<dsp::SchmittTrigger, kWyrmMaxChannels> syncTriggers;

	bool lfoMode = false;
	bool editorLocked = true;
	int selectedShape = SHAPE_SINE;
	int pointCount = kWyrmPointCountDefault;

	Wyrm() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam<WyrmFreqQuantity>(FREQ_PARAM, 0.f, 1.f, 0.45f, "Frequency");
		configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
		configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuator");
		configParam(FOLD_PARAM, 0.f, 1.f, 0.f, "Fold amount");
		configParam(SLITHER_PARAM, 0.f, 1.f, 0.f, "Slither", "%", 0.f, 100.f);
		configParam(SLITHER_SPEED_PARAM, 0.f, 1.f, 0.5f, "Slither speed");
		configInput(VOCT_INPUT, "V/Oct");
		configInput(FM_INPUT, "FM");
		configInput(SYNC_INPUT, "Sync");
		configInput(FOLD_CV_INPUT, "Fold CV");
		configOutput(OUT_OUTPUT, "Out");
		configOutput(RAW_OUTPUT, "Raw");

		setFactoryShape(SHAPE_SINE);
	}

	void setWavePoint(int index, float value) {
		if (index < 0 || index >= pointCount) {
			return;
		}
		wavePoints[index].store(clamp(value, -1.f, 1.f), std::memory_order_relaxed);
		waveVersion.fetch_add(1u, std::memory_order_release);
	}

	float getWavePoint(int index) const {
		if (index < 0 || index >= pointCount) {
			return 0.f;
		}
		return wavePoints[index].load(std::memory_order_relaxed);
	}

	void setFactoryShape(int shapeId) {
		shapeId = clamp(shapeId, 0, SHAPE_COUNT - 1);
		selectedShape = shapeId;
		for (int i = 0; i < pointCount; ++i) {
			const float p = float(i) / float(pointCount);
			float v = 0.f;
			switch (shapeId) {
				case SHAPE_SINE: v = std::sin(2.f * float(M_PI) * p); break;
				case SHAPE_TRIANGLE: {
					const float x = 2.f * std::fabs(2.f * p - 1.f) - 1.f;
					v = -x;
				} break;
				case SHAPE_SAW: v = 2.f * p - 1.f; break;
				case SHAPE_REV_SAW: v = 1.f - 2.f * p; break;
				case SHAPE_SQUARE: v = (p < 0.5f) ? 1.f : -1.f; break;
				default: break;
			}
			wavePoints[i].store(clamp(v, -1.f, 1.f), std::memory_order_relaxed);
		}
		waveVersion.fetch_add(1u, std::memory_order_release);
	}

	void setPointCount(int newPointCount) {
		newPointCount = clamp(newPointCount, 32, kWyrmPointCountMax);
		if (newPointCount != 32 && newPointCount != 48 && newPointCount != 64 && newPointCount != 128) {
			newPointCount = kWyrmPointCountDefault;
		}
		if (newPointCount == pointCount) {
			return;
		}
		pointCount = newPointCount;
		setFactoryShape(selectedShape);
	}

	void rebuildWavetable() {
		std::array<float, kWyrmPointCountMax> local {};
		for (int i = 0; i < pointCount; ++i) {
			local[i] = wavePoints[i].load(std::memory_order_relaxed);
		}
		float maxAbs = 1e-6f;
		for (int i = 0; i < kWyrmTableSize; ++i) {
			const float ph = float(i) / float(kWyrmTableSize);
			const float y = catmullPeriodic(local, pointCount, ph);
			wavetable[i] = y;
			maxAbs = std::max(maxAbs, std::fabs(y));
		}
		const float inv = 1.f / maxAbs;
		for (int i = 0; i < kWyrmTableSize; ++i) {
			wavetable[i] = clamp(wavetable[i] * inv, -1.f, 1.f);
		}
	}

	float lookupWave(float ph) const {
		float p = ph;
		if (p >= 1.f) {
			p -= 1.f;
		}
		else if (p < 0.f) {
			p += 1.f;
		}
		const float x = p * float(kWyrmTableSize);
		const int i0 = int(x);
		const int i1 = (i0 + 1 < kWyrmTableSize) ? (i0 + 1) : 0;
		const float t = x - float(i0);
		return std::fma((wavetable[i1] - wavetable[i0]), t, wavetable[i0]);
	}

	json_t* dataToJson() override {
		json_t* root = json_object();
		json_object_set_new(root, "lfoMode", json_boolean(lfoMode));
		json_object_set_new(root, "editorLocked", json_boolean(editorLocked));
		json_object_set_new(root, "selectedShape", json_integer(selectedShape));
		json_object_set_new(root, "pointCount", json_integer(pointCount));
		json_t* pts = json_array();
		for (int i = 0; i < pointCount; ++i) {
			json_array_append_new(pts, json_real(getWavePoint(i)));
		}
		json_object_set_new(root, "wavePoints", pts);
		return root;
	}

	void dataFromJson(json_t* root) override {
		json_t* lfoJ = json_object_get(root, "lfoMode");
		if (lfoJ) lfoMode = json_is_true(lfoJ);
		json_t* lockJ = json_object_get(root, "editorLocked");
		if (lockJ) editorLocked = json_is_true(lockJ);
		json_t* shapeJ = json_object_get(root, "selectedShape");
		if (shapeJ) selectedShape = clamp(int(json_integer_value(shapeJ)), 0, SHAPE_COUNT - 1);
		json_t* pointCountJ = json_object_get(root, "pointCount");
		if (pointCountJ) {
			int loadedPointCount = int(json_integer_value(pointCountJ));
			if (loadedPointCount == 32 || loadedPointCount == 48 || loadedPointCount == 64 || loadedPointCount == 128) {
				pointCount = loadedPointCount;
			}
		}
		json_t* pts = json_object_get(root, "wavePoints");
		if (pts && json_is_array(pts)) {
			const size_t n = json_array_size(pts);
			for (int i = 0; i < pointCount && i < int(n); ++i) {
				json_t* v = json_array_get(pts, i);
				if (v) {
					wavePoints[i].store(clamp(float(json_number_value(v)), -1.f, 1.f), std::memory_order_relaxed);
				}
			}
			waveVersion.fetch_add(1u, std::memory_order_release);
		}
	}

	void process(const ProcessArgs& args) override {
		const uint32_t v = waveVersion.load(std::memory_order_acquire);
		if (v != appliedWaveVersion) {
			rebuildWavetable();
			appliedWaveVersion = v;
		}

		const int channels = std::max(1, std::max({inputs[VOCT_INPUT].getChannels(), inputs[FM_INPUT].getChannels(), inputs[SYNC_INPUT].getChannels(), inputs[FOLD_CV_INPUT].getChannels()}));
		outputs[OUT_OUTPUT].setChannels(channels);
		outputs[RAW_OUTPUT].setChannels(channels);

		const float knobNorm = clamp01(params[FREQ_PARAM].getValue());
		const float baseFreq = wyrmBaseFrequencyFromKnob(knobNorm, lfoMode);
		const float fmAtten = params[FM_ATTEN_PARAM].getValue();
		const float fine = params[FINE_PARAM].getValue() / 1200.f;
		const float foldBase = params[FOLD_PARAM].getValue();
		const float slitherAmount = clamp01(params[SLITHER_PARAM].getValue());
		const float slitherSpeed = slitherSpeedFactor(params[SLITHER_SPEED_PARAM].getValue());

		for (int c = 0; c < channels; ++c) {
			if (inputs[SYNC_INPUT].isConnected()) {
				const float s = inputs[SYNC_INPUT].getPolyVoltage(c);
				if (syncTriggers[c].process(s)) {
					phase[c] = 0.f;
					slitherPhase[c] = 0.f;
				}
			}
			const float voct = inputs[VOCT_INPUT].isConnected() ? inputs[VOCT_INPUT].getPolyVoltage(c) : 0.f;
			const float fm = inputs[FM_INPUT].isConnected() ? inputs[FM_INPUT].getPolyVoltage(c) * fmAtten : 0.f;
			float hz = baseFreq * rack::dsp::exp2_taylor5(voct + fm + fine);
			hz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
			phase[c] = wrap01Fast(phase[c] + hz * args.sampleTime);
			const float slitherBaseHz = lfoMode ? clamp(hz, 0.01f, 8.f) : clamp(0.125f * hz, 0.15f, 8.f);
			const float slitherHz = clamp(slitherBaseHz * slitherSpeed, 0.01f, 16.f);
			slitherPhase[c] = wrap01Fast(slitherPhase[c] + slitherHz * args.sampleTime);
			const float raw = clamp(lookupWave(phase[c]) + slitherOffset(phase[c], slitherPhase[c], slitherAmount), -1.f, 1.f);
			const float foldCv = inputs[FOLD_CV_INPUT].isConnected() ? clamp(inputs[FOLD_CV_INPUT].getPolyVoltage(c) / 10.f, -1.f, 1.f) : 0.f;
			const float foldAmt = clamp(foldBase + foldCv, 0.f, 2.f);
			float folded = raw;
			if (foldAmt > 1e-5f) {
				folded = clamp(softClip(foldWave(raw, foldAmt)) * kWyrmFoldMakeupGain, -1.f, 1.f);
			}
			outputs[RAW_OUTPUT].setVoltage(5.f * raw, c);
			outputs[OUT_OUTPUT].setVoltage(5.f * folded, c);
		}
	}
};

float WyrmFreqQuantity::getDisplayValue() {
	const auto* wyrm = static_cast<const Wyrm*>(module);
	return wyrmBaseFrequencyFromKnob(getValue(), wyrm ? wyrm->lfoMode : false);
}

void WyrmFreqQuantity::setDisplayValue(float displayValue) {
	const auto* wyrm = static_cast<const Wyrm*>(module);
	setImmediateValue(wyrmKnobValueForFrequency(displayValue, wyrm ? wyrm->lfoMode : false));
}

std::string WyrmFreqQuantity::getDisplayValueString() {
	const float hz = getDisplayValue();
	if (hz >= 1000.f) {
		return string::f("%.2f kHz", hz / 1000.f);
	}
	if (hz < 0.1f) {
		return string::f("%.3f Hz", hz);
	}
	if (hz < 10.f) {
		return string::f("%.2f Hz", hz);
	}
	return string::f("%.1f Hz", hz);
}

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;
	float visualSlitherPhase = 0.f;
	double lastVisualUpdateSec = -1.0;

	explicit WyrmWaveEditor(Wyrm* m) {
		module = m;
	}

	int indexFromX(float x) const {
		if (box.size.x <= 1.f) return 0;
		const float n = clamp(x / box.size.x, 0.f, 1.f);
		const int count = module ? module->pointCount : kWyrmPointCountDefault;
		return clamp(int(std::floor(n * float(count))), 0, count - 1);
	}

	float valueFromY(float y) const {
		if (box.size.y <= 1.f) return 0.f;
		const float n = clamp(y / box.size.y, 0.f, 1.f);
		return clamp(1.f - 2.f * n, -1.f, 1.f);
	}

	void advanceVisualSlitherPhase(double nowSec) {
		if (!std::isfinite(nowSec)) return;
		if (lastVisualUpdateSec < 0.0 || !std::isfinite(lastVisualUpdateSec)) {
			lastVisualUpdateSec = nowSec;
			return;
		}
		const float elapsed = clamp(float(nowSec - lastVisualUpdateSec), 0.f, 0.25f);
		lastVisualUpdateSec = nowSec;
		if (!module) return;
		const float speedFactor = slitherSpeedFactor(module->params[Wyrm::SLITHER_SPEED_PARAM].getValue());
		visualSlitherPhase = wrap01(visualSlitherPhase + 0.65f * speedFactor * elapsed);
	}

	float slitherOffsetForIndex(int index) const {
		if (!module || module->pointCount <= 0) return 0.f;
		const float amount = clamp01(module->params[Wyrm::SLITHER_PARAM].getValue());
		if (amount <= 1e-5f) return 0.f;
		const float phase = (float(index) + 0.5f) / float(module->pointCount);
		return slitherOffset(phase, visualSlitherPhase, amount);
	}

	float displayWavePoint(int index) const {
		if (!module) return 0.f;
		return clamp(module->getWavePoint(index) + slitherOffsetForIndex(index), -1.f, 1.f);
	}

	Vec currentLocalMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	void applyPointFromPos(Vec pos) {
		if (!module || module->editorLocked) return;
		const int idx = indexFromX(pos.x);
		const float targetDisplayValue = valueFromY(pos.y);
		const double nowSec = system::getTime();
		advanceVisualSlitherPhase(nowSec);
		auto writeDisplayValue = [&](int pointIndex) {
			module->setWavePoint(pointIndex, targetDisplayValue - slitherOffsetForIndex(pointIndex));
		};
		if (lastIndex >= 0 && lastIndex != idx) {
			const int lo = std::min(lastIndex, idx);
			const int hi = std::max(lastIndex, idx);
			for (int i = lo; i <= hi; ++i) {
				// Operator-style paint gesture: each crossed segment is set directly.
				writeDisplayValue(i);
			}
		}
		else {
			writeDisplayValue(idx);
		}
		lastIndex = idx;
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onButton(e);
			return;
		}
		if (e.action == GLFW_PRESS) {
			lastIndex = -1;
			applyPointFromPos(e.pos);
			e.consume(this);
			return;
		}
		if (e.action == GLFW_RELEASE) {
			lastIndex = -1;
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (!module || module->editorLocked || e.button != GLFW_MOUSE_BUTTON_LEFT) {
			Widget::onDragMove(e);
			return;
		}
		applyPointFromPos(currentLocalMousePos());
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		if (!args.vg) return;
		advanceVisualSlitherPhase(system::getTime());

		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(14, 14, 14, 205));
		nvgFill(args.vg);

		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 0.f, 0.5f * box.size.y);
		nvgLineTo(args.vg, box.size.x, 0.5f * box.size.y);
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(240, 180, 42, 120));
		nvgStroke(args.vg);

		for (int i = 1; i < 4; ++i) {
			const float x = (box.size.x * i) / 4.f;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x, 0.f);
			nvgLineTo(args.vg, x, box.size.y);
			nvgStrokeWidth(args.vg, 1.f);
			nvgStrokeColor(args.vg, nvgRGBA(120, 120, 120, 48));
			nvgStroke(args.vg);
		}

		if (!module) return;

		Vec mouseLocal = currentLocalMousePos();
		const bool mouseInside = (mouseLocal.x >= 0.f && mouseLocal.x <= box.size.x && mouseLocal.y >= 0.f && mouseLocal.y <= box.size.y);
		if (mouseInside) {
			const float guideY = clamp(mouseLocal.y, 0.f, box.size.y);
			const int hoverIdx = indexFromX(mouseLocal.x);
			const int count = module->pointCount;
			const float dxHover = box.size.x / float(count);
			const float x0 = hoverIdx * dxHover;
			const float x1 = x0 + dxHover;

			// Hover segment lane preview.
			nvgBeginPath(args.vg);
			nvgRect(args.vg, x0, 0.f, x1 - x0, box.size.y);
			nvgFillColor(args.vg, nvgRGBA(255, 230, 120, 28));
			nvgFill(args.vg);

			// Hover target-height guide.
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, 0.f, guideY);
			nvgLineTo(args.vg, box.size.x, guideY);
			nvgStrokeWidth(args.vg, 1.4f);
			nvgStrokeColor(args.vg, nvgRGBA(255, 232, 140, 180));
			nvgStroke(args.vg);
		}

		// Draw as discrete segments so each point reads as an editable bar.
		const float midY = 0.5f * box.size.y;
		const int count = module->pointCount;
		const float edgeInset = 2.2f;
		const float drawWidth = std::max(1.f, box.size.x - 2.f * edgeInset);
		const float dx = drawWidth / float(count);
		auto xAt = [&](int i) {
			return edgeInset + (float(i) + 0.5f) * dx;
		};
		for (int i = 0; i < count; ++i) {
			const float x = xAt(i);
			const float y = (0.5f - 0.5f * displayWavePoint(i)) * box.size.y;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x, midY);
			nvgLineTo(args.vg, x, y);
			nvgStrokeWidth(args.vg, 2.f);
			nvgStrokeColor(args.vg, nvgRGBA(158, 132, 78, 170));
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, 2.1f);
			nvgFillColor(args.vg, nvgRGBA(235, 204, 128, 245));
			nvgFill(args.vg);
		}

		// Constrain stylized body/texture rendering to the waveform editor bounds.
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);

		// Ouroboros body under-stroke.
		auto emitRoundedBodyPath = [&]() {
			const float roundCosThreshold = -0.25f; // smooth most corners, preserve very sharp reversals
			if (count <= 0) {
				return;
			}
			if (count == 1) {
				const float x = xAt(0);
				const float y = (0.5f - 0.5f * displayWavePoint(0)) * box.size.y;
				nvgMoveTo(args.vg, x, y);
				return;
			}

			auto pointAt = [&](int i) {
				return Vec(xAt(i), (0.5f - 0.5f * displayWavePoint(i)) * box.size.y);
			};

			const Vec pStart = pointAt(0);
			nvgMoveTo(args.vg, pStart.x, pStart.y);

			for (int i = 1; i < count - 1; ++i) {
				const Vec p0 = pointAt(i - 1);
				const Vec p1 = pointAt(i);
				const Vec p2 = pointAt(i + 1);
				Vec vIn = p1.minus(p0);
				Vec vOut = p2.minus(p1);
				const float inLen = std::sqrt(vIn.x * vIn.x + vIn.y * vIn.y);
				const float outLen = std::sqrt(vOut.x * vOut.x + vOut.y * vOut.y);
				if (inLen < 1e-4f || outLen < 1e-4f) {
					nvgLineTo(args.vg, p1.x, p1.y);
					continue;
				}
				vIn = vIn.div(inLen);
				vOut = vOut.div(outLen);
				const float cornerCos = vIn.x * vOut.x + vIn.y * vOut.y;
				if (cornerCos >= roundCosThreshold) {
					const Vec midOut = p1.plus(p2).mult(0.5f);
					nvgQuadTo(args.vg, p1.x, p1.y, midOut.x, midOut.y);
				}
				else {
					nvgLineTo(args.vg, p1.x, p1.y);
				}
			}

			const Vec pEnd = pointAt(count - 1);
			nvgLineTo(args.vg, pEnd.x, pEnd.y);
		};

		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 4.0f);
		nvgStrokeColor(args.vg, nvgRGBA(74, 54, 24, 205));
		nvgStroke(args.vg);

		// Mid-tone bronze body.
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 2.6f);
		nvgStrokeColor(args.vg, nvgRGBA(167, 132, 72, 230));
		nvgStroke(args.vg);

		// Verdigris/gold highlight along the top of the body.
		nvgLineJoin(args.vg, NVG_ROUND);
		nvgLineCap(args.vg, NVG_ROUND);
		nvgBeginPath(args.vg);
		emitRoundedBodyPath();
		nvgStrokeWidth(args.vg, 1.15f);
		nvgStrokeColor(args.vg, nvgRGBA(246, 215, 136, 225));
		nvgStroke(args.vg);

		// Scale plates along the body.
		for (int i = 0; i < count; ++i) {
			const float x = xAt(i);
			const float y = (0.5f - 0.5f * displayWavePoint(i)) * box.size.y;
			const float plateW = clamp(0.31f * dx, 1.0f, 3.4f);
			const float plateH = 0.95f + 0.35f * std::sin(0.33f * float(i));
			for (int lane = 0; lane < 2; ++lane) {
				const float laneOffset = (lane == 0) ? -1.1f : 1.1f;
				const float laneShift = (lane == 0) ? -0.18f * dx : 0.18f * dx;
				nvgBeginPath(args.vg);
				nvgEllipse(args.vg, x + laneShift, y + laneOffset, plateW, plateH);
				nvgFillColor(args.vg,
					((i + lane) % 3) == 0
						? nvgRGBA(202, 168, 102, 185)
						: nvgRGBA(150, 110, 56, 150));
				nvgFill(args.vg);
			}
		}
		nvgResetScissor(args.vg);
		nvgRestore(args.vg);

	}
};

struct WyrmShapeMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int shape = SHAPE_SINE;

	void onAction(const event::Action& e) override {
		if (module) module->setFactoryShape(shape);
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->selectedShape == shape) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmPointCountMenuItem : MenuItem {
	Wyrm* module = nullptr;
	int count = kWyrmPointCountDefault;

	void onAction(const event::Action& e) override {
		if (module) {
			module->setPointCount(count);
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->pointCount == count) ? "✓" : "";
		MenuItem::step();
	}
};

struct WyrmWidget : ModuleWidget {
	explicit WyrmWidget(Wyrm* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/wyrm.svg");
		setPanel(createPanel(panelPath));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPt = [&](const char* id, Vec* pos) {
			Vec p;
			if (panel_svg::loadPointFromSvgMm(panelPath, id, &p)) {
				*pos = p;
			}
		};

		math::Rect editorRectMm(Vec(6.0f, 16.0f), Vec(59.12f, 52.0f));
		panel_svg::loadRectFromSvgMm(panelPath, "WYRm_WAVE_EDITOR", &editorRectMm);
		Vec freqPos(17.5f, 80.0f);
		Vec finePos(35.56f, 80.0f);
		Vec fmAttenPos(53.62f, 80.0f);
		Vec foldPos(35.56f, 98.0f);
		Vec slitherPos(17.50f, 112.80f);
		Vec slitherSpeedPos(26.50f, 112.80f);
		Vec voctPos(14.0f, 111.0f);
		Vec fmPos(28.0f, 111.0f);
		Vec syncPos(43.0f, 111.0f);
		Vec foldCvPos(57.0f, 111.0f);
		Vec rawOutPos(24.0f, 122.0f);
		Vec outPos(47.0f, 122.0f);
		applyPt("WYRM_FREQ_PARAM", &freqPos);
		applyPt("WYRM_FINE_PARAM", &finePos);
		applyPt("WYRM_FM_ATTEN_PARAM", &fmAttenPos);
		applyPt("WYRM_FOLD_PARAM", &foldPos);
		applyPt("WYRM_SLITHER_PARAM", &slitherPos);
		applyPt("WYRM_SLITHER_SPEED_PARAM", &slitherSpeedPos);
		applyPt("WYRM_VOCT_INPUT", &voctPos);
		applyPt("WYRM_FM_INPUT", &fmPos);
		applyPt("WYRM_SYNC_INPUT", &syncPos);
		applyPt("WYRM_FOLD_CV_INPUT", &foldCvPos);
		applyPt("WYRM_RAW_OUTPUT", &rawOutPos);
		applyPt("WYRM_OUT_OUTPUT", &outPos);

		auto* editor = new WyrmWaveEditor(module);
		editor->box.pos = mm2px(editorRectMm.pos);
		editor->box.size = mm2px(editorRectMm.size);
		addChild(editor);

		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(freqPos), module, Wyrm::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(finePos), module, Wyrm::FINE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(fmAttenPos), module, Wyrm::FM_ATTEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(foldPos), module, Wyrm::FOLD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(slitherPos), module, Wyrm::SLITHER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(slitherSpeedPos), module, Wyrm::SLITHER_SPEED_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(voctPos), module, Wyrm::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(fmPos), module, Wyrm::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(syncPos), module, Wyrm::SYNC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(foldCvPos), module, Wyrm::FOLD_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(rawOutPos), module, Wyrm::RAW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(outPos), module, Wyrm::OUT_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* module = dynamic_cast<Wyrm*>(this->module);
		if (!module) return;

		menu->addChild(new MenuSeparator());
		menu->addChild(createBoolPtrMenuItem("LFO Mode", "", &module->lfoMode));
		menu->addChild(createBoolPtrMenuItem("Lock Wave Editor", "", &module->editorLocked));
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Point Count"));
		for (int count : {32, 48, 64, 128}) {
			auto* item = new WyrmPointCountMenuItem();
			item->text = string::f("%d", count);
			item->module = module;
			item->count = count;
			menu->addChild(item);
		}
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Factory Shape"));
		for (int i = 0; i < SHAPE_COUNT; ++i) {
			auto* item = new WyrmShapeMenuItem();
			item->text = kWyrmShapeLabels[i];
			item->module = module;
			item->shape = i;
			menu->addChild(item);
		}
	}
};

Model* modelWyrm = createModel<Wyrm, WyrmWidget>("Wyrm");
