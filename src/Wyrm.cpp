#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <array>
#include <atomic>
#include <cmath>

namespace {

constexpr int kWyrmPointCountDefault = 32;
constexpr int kWyrmPointCountMax = 64;
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

inline float clamp01(float x) {
	return clamp(x, 0.f, 1.f);
}

inline float wrap01(float x) {
	return x - std::floor(x);
}

inline float softClip(float x) {
	return std::tanh(x);
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

} // namespace

struct Wyrm : Module {
	enum ParamId {
		FREQ_PARAM,
		FINE_PARAM,
		FM_ATTEN_PARAM,
		FOLD_PARAM,
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
	std::array<dsp::SchmittTrigger, kWyrmMaxChannels> syncTriggers;

	bool lfoMode = false;
	bool editorLocked = true;
	int selectedShape = SHAPE_SINE;
	int pointCount = kWyrmPointCountDefault;

	Wyrm() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, 0.f, 1.f, 0.45f, "Frequency");
		configParam(FINE_PARAM, -100.f, 100.f, 0.f, "Fine tune", " cents");
		configParam(FM_ATTEN_PARAM, -1.f, 1.f, 0.f, "FM attenuator");
		configParam(FOLD_PARAM, 0.f, 1.f, 0.f, "Fold amount");
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
		if (newPointCount != 32 && newPointCount != 48 && newPointCount != 64) {
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
			wavetable[i] = clamp(wavetable[i] * inv, -1.2f, 1.2f);
		}
	}

	float lookupWave(float ph) const {
		const float x = wrap01(ph) * float(kWyrmTableSize);
		const int i0 = int(std::floor(x)) % kWyrmTableSize;
		const int i1 = (i0 + 1) % kWyrmTableSize;
		const float t = x - std::floor(x);
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
			if (loadedPointCount == 32 || loadedPointCount == 48 || loadedPointCount == 64) {
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

		const float minFreq = lfoMode ? 0.01f : 20.f;
		const float maxFreq = lfoMode ? 100.f : 20000.f;
		const float knobNorm = clamp01(params[FREQ_PARAM].getValue());
		const float baseFreq = minFreq * std::pow(maxFreq / minFreq, knobNorm);
		const float fmAtten = params[FM_ATTEN_PARAM].getValue();
		const float fine = params[FINE_PARAM].getValue() / 1200.f;
		const float foldBase = params[FOLD_PARAM].getValue();

		for (int c = 0; c < channels; ++c) {
			if (inputs[SYNC_INPUT].isConnected()) {
				const float s = inputs[SYNC_INPUT].getPolyVoltage(c);
				if (syncTriggers[c].process(s)) {
					phase[c] = 0.f;
				}
			}
			const float voct = inputs[VOCT_INPUT].isConnected() ? inputs[VOCT_INPUT].getPolyVoltage(c) : 0.f;
			const float fm = inputs[FM_INPUT].isConnected() ? inputs[FM_INPUT].getPolyVoltage(c) * fmAtten : 0.f;
			float hz = baseFreq * std::pow(2.f, voct + fm + fine);
			hz = clamp(hz, 0.005f, 0.45f * args.sampleRate);
			phase[c] = wrap01(phase[c] + hz * args.sampleTime);
			const float raw = lookupWave(phase[c]);
			const float foldCv = inputs[FOLD_CV_INPUT].isConnected() ? clamp(inputs[FOLD_CV_INPUT].getPolyVoltage(c) / 10.f, -1.f, 1.f) : 0.f;
			const float foldAmt = clamp(foldBase + foldCv, 0.f, 2.f);
			const float folded = softClip(foldWave(raw, foldAmt));
			outputs[RAW_OUTPUT].setVoltage(5.f * raw, c);
			outputs[OUT_OUTPUT].setVoltage(5.f * folded, c);
		}
	}
};

struct WyrmWaveEditor : TransparentWidget {
	Wyrm* module = nullptr;
	int lastIndex = -1;

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

	Vec currentLocalMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		return APP->scene->rack->getMousePos().minus(parent->box.pos).minus(box.pos);
	}

	void applyPointFromPos(Vec pos) {
		if (!module || module->editorLocked) return;
		const int idx = indexFromX(pos.x);
		const float v = valueFromY(pos.y);
		if (lastIndex >= 0 && lastIndex != idx) {
			const int lo = std::min(lastIndex, idx);
			const int hi = std::max(lastIndex, idx);
			for (int i = lo; i <= hi; ++i) {
				// Operator-style paint gesture: each crossed segment is set directly.
				module->setWavePoint(i, v);
			}
		}
		else {
			module->setWavePoint(idx, v);
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
		const float dx = box.size.x / float(count);
		for (int i = 0; i < count; ++i) {
			const float x = (float(i) + 0.5f) * dx;
			const float y = (0.5f - 0.5f * module->getWavePoint(i)) * box.size.y;
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x, midY);
			nvgLineTo(args.vg, x, y);
			nvgStrokeWidth(args.vg, 2.f);
			nvgStrokeColor(args.vg, nvgRGBA(203, 165, 83, 210));
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, 2.1f);
			nvgFillColor(args.vg, nvgRGBA(235, 203, 124, 250));
			nvgFill(args.vg);
		}

		// Sandwyrm body under-stroke.
		nvgBeginPath(args.vg);
		for (int i = 0; i < count; ++i) {
			const float x = (float(i) + 0.5f) * dx;
			const float y = (0.5f - 0.5f * module->getWavePoint(i)) * box.size.y;
			if (i == 0) nvgMoveTo(args.vg, x, y);
			else nvgLineTo(args.vg, x, y);
		}
		nvgStrokeWidth(args.vg, 3.2f);
		nvgStrokeColor(args.vg, nvgRGBA(121, 86, 38, 135));
		nvgStroke(args.vg);

		// Sandwyrm body highlight stroke.
		nvgBeginPath(args.vg);
		for (int i = 0; i < count; ++i) {
			const float x = (float(i) + 0.5f) * dx;
			const float y = (0.5f - 0.5f * module->getWavePoint(i)) * box.size.y;
			if (i == 0) nvgMoveTo(args.vg, x, y);
			else nvgLineTo(args.vg, x, y);
		}
		nvgStrokeWidth(args.vg, 1.35f);
		nvgStrokeColor(args.vg, nvgRGBA(238, 202, 120, 205));
		nvgStroke(args.vg);

		// Sparse scale-like beads along the body.
		for (int i = 1; i < count; i += 2) {
			const float x = (float(i) + 0.5f) * dx;
			const float y = (0.5f - 0.5f * module->getWavePoint(i)) * box.size.y;
			const float r = 1.1f + 0.55f * std::sin(0.45f * float(i));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, x, y, r);
			nvgFillColor(args.vg, nvgRGBA(166, 124, 62, 185));
			nvgFill(args.vg);
		}

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
		for (int count : {32, 48, 64}) {
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
