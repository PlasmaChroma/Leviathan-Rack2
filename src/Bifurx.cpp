#include "plugin.hpp"
#include "PanelSvgUtils.hpp"

#include <algorithm>
#include <cmath>
#include <exception>

struct Bifurx;

namespace {

constexpr float kDefaultPanelWidthMm = 71.12f;
constexpr float kDefaultPanelHeightMm = 128.5f;

float clamp01(float v) {
	return clamp(v, 0.f, 1.f);
}

float shapedSpan(float value) {
	return std::pow(clamp01(value), 1.65f);
}

float levelDriveGain(float knob) {
	return 0.1f + 7.9f * clamp01(knob);
}

float softClip(float x) {
	return std::tanh(x);
}

struct BifurxSpectrumWidget final : Widget {
	Bifurx* module = nullptr;

	void draw(const DrawArgs& args) override;
};

} // namespace

struct Bifurx final : Module {
	enum ParamId {
		MODE_PARAM,
		LEVEL_PARAM,
		FREQ_PARAM,
		RESO_PARAM,
		BALANCE_PARAM,
		SPAN_PARAM,
		FM_AMT_PARAM,
		SPAN_CV_ATTEN_PARAM,
		TITO_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		VOCT_INPUT,
		FM_INPUT,
		RESO_CV_INPUT,
		BALANCE_CV_INPUT,
		SPAN_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		FM_AMT_LIGHT,
		SPAN_CV_ATTEN_LIGHT,
		LIGHTS_LEN
	};

	Bifurx() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configSwitch(MODE_PARAM, 0.f, 9.f, 0.f,
			"Mode",
			{"LL", "LB", "NL", "NN", "LH", "BB", "BL", "NH", "BH", "HL"});
		configParam(LEVEL_PARAM, 0.f, 1.f, 0.5f, "Level");
		configParam(FREQ_PARAM, 0.f, 1.f, 0.5f, "Frequency");
		configParam(RESO_PARAM, 0.f, 1.f, 0.35f, "Resonance");
		configParam(BALANCE_PARAM, -1.f, 1.f, 0.f, "Balance");
		configParam(SPAN_PARAM, 0.f, 1.f, 0.f, "Span");
		configParam(FM_AMT_PARAM, -1.f, 1.f, 0.f, "FM amount");
		configParam(SPAN_CV_ATTEN_PARAM, -1.f, 1.f, 0.f, "Span CV attenuator");
		configSwitch(TITO_PARAM, 0.f, 2.f, 1.f, "TITO", {"XM", "Clean", "SM"});

		configInput(IN_INPUT, "Signal In");
		configInput(VOCT_INPUT, "V/Oct");
		configInput(FM_INPUT, "FM");
		configInput(RESO_CV_INPUT, "Resonance CV");
		configInput(BALANCE_CV_INPUT, "Balance CV");
		configInput(SPAN_CV_INPUT, "Span CV");
		configOutput(OUT_OUTPUT, "Signal Out");

		paramQuantities[MODE_PARAM]->snapEnabled = true;
		paramQuantities[TITO_PARAM]->snapEnabled = true;
	}

	void process(const ProcessArgs& args) override {
		const float in = inputs[IN_INPUT].getVoltage();
		const float level = params[LEVEL_PARAM].getValue();
		const float drive = levelDriveGain(level);

		// Temporary audio path for scaffold phase: keep the module alive and
		// musically useful while the actual dual-core filter is implemented.
		const float driven = softClip(in * drive);
		const float normalized = std::tanh(drive);
		const float out = (normalized > 1e-4f) ? (5.f * driven / normalized) : 0.f;
		outputs[OUT_OUTPUT].setVoltage(out);

		lights[FM_AMT_LIGHT].setBrightness(std::fabs(params[FM_AMT_PARAM].getValue()));
		lights[SPAN_CV_ATTEN_LIGHT].setBrightness(std::fabs(params[SPAN_CV_ATTEN_PARAM].getValue()));
	}
};

namespace {

void BifurxSpectrumWidget::draw(const DrawArgs& args) {
	if (!module) {
		return;
	}

	const float w = box.size.x;
	const float h = box.size.y;
	if (!(w > 0.f && h > 0.f)) {
		return;
	}

	const float padX = std::max(8.f, w * 0.035f);
	const float padY = std::max(8.f, h * 0.10f);
	const float usableW = std::max(1.f, w - 2.f * padX);
	const float usableH = std::max(1.f, h - 2.f * padY);
	const float centerY = padY + usableH * 0.58f;

	const float freqNorm = clamp01(module->params[Bifurx::FREQ_PARAM].getValue());
	const float spanNorm = shapedSpan(module->params[Bifurx::SPAN_PARAM].getValue());
	const float resoNorm = clamp01(module->params[Bifurx::RESO_PARAM].getValue());
	const float balance = clamp(module->params[Bifurx::BALANCE_PARAM].getValue(), -1.f, 1.f);

	const float center = freqNorm;
	const float spread = 0.42f * spanNorm;
	const float leftPeak = clamp(center - spread, 0.f, 1.f);
	const float rightPeak = clamp(center + spread, 0.f, 1.f);
	const float leftGain = 0.45f + resoNorm * (0.55f - 0.22f * balance);
	const float rightGain = 0.45f + resoNorm * (0.55f + 0.22f * balance);
	const float peakWidth = 0.02f + 0.11f * (1.f - 0.82f * resoNorm);

	nvgSave(args.vg);
	nvgScissor(args.vg, 0.f, 0.f, w, h);

	nvgBeginPath(args.vg);
	nvgRect(args.vg, 0.f, 0.f, w, h);
	nvgFillColor(args.vg, nvgRGBA(7, 10, 14, 26));
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, padX, centerY);
	nvgLineTo(args.vg, padX + usableW, centerY);
	nvgStrokeColor(args.vg, nvgRGBA(255, 255, 255, 28));
	nvgStrokeWidth(args.vg, 1.2f);
	nvgStroke(args.vg);

	auto responseAt = [&](float x) {
		float leftD = (x - leftPeak) / peakWidth;
		float rightD = (x - rightPeak) / peakWidth;
		float left = leftGain * std::exp(-leftD * leftD);
		float right = rightGain * std::exp(-rightD * rightD);
		float dipMid = 0.16f * spanNorm * std::exp(-std::pow((x - center) / (0.18f + 0.18f * spanNorm), 2.f));
		return left + right - dipMid;
	};

	nvgBeginPath(args.vg);
	nvgMoveTo(args.vg, padX, centerY);
	for (int i = 0; i <= 96; ++i) {
		float x01 = float(i) / 96.f;
		float response = responseAt(x01);
		float y = centerY - response * usableH * 0.34f;
		float x = padX + usableW * x01;
		nvgLineTo(args.vg, x, y);
	}
	nvgLineTo(args.vg, padX + usableW, h - padY * 0.4f);
	nvgLineTo(args.vg, padX, h - padY * 0.4f);
	nvgClosePath(args.vg);
	NVGpaint fillPaint = nvgLinearGradient(
		args.vg, 0.f, padY, 0.f, h - padY,
		nvgRGBA(122, 92, 255, 72),
		nvgRGBA(28, 204, 217, 72)
	);
	nvgFillPaint(args.vg, fillPaint);
	nvgFill(args.vg);

	nvgBeginPath(args.vg);
	for (int i = 0; i <= 96; ++i) {
		float x01 = float(i) / 96.f;
		float response = responseAt(x01);
		float y = centerY - response * usableH * 0.34f;
		float x = padX + usableW * x01;
		if (i == 0) {
			nvgMoveTo(args.vg, x, y);
		}
		else {
			nvgLineTo(args.vg, x, y);
		}
	}
	NVGpaint strokePaint = nvgLinearGradient(
		args.vg, padX, 0.f, padX + usableW, 0.f,
		nvgRGBA(122, 92, 255, 220),
		nvgRGBA(28, 204, 217, 220)
	);
	nvgStrokePaint(args.vg, strokePaint);
	nvgStrokeWidth(args.vg, 2.6f);
	nvgStroke(args.vg);

	nvgFontSize(args.vg, std::max(11.f, h * 0.11f));
	nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 160));
	nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgText(args.vg, padX, padY * 0.35f, "SPECTRUM", nullptr);

	nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
	nvgText(args.vg, w - padX, padY * 0.35f, "preview", nullptr);

	nvgResetScissor(args.vg);
	nvgRestore(args.vg);
	Widget::draw(args);
}

} // namespace

struct BifurxWidget final : ModuleWidget {
	explicit BifurxWidget(Bifurx* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/bifurx.svg");
		try {
			setPanel(createPanel(panelPath));
		}
		catch (const std::exception& e) {
			WARN("Bifurx panel load failed (%s), using fallback: %s", panelPath.c_str(), e.what());
			setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg")));
			box.size = mm2px(Vec(kDefaultPanelWidthMm, kDefaultPanelHeightMm));
		}

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		auto applyPointOverride = [&](const char* elementId, Vec* outPosMm) {
			Vec pointMm;
			if (panel_svg::loadPointFromSvgMm(panelPath, elementId, &pointMm)) {
				*outPosMm = pointMm;
			}
		};

		math::Rect spectrumRectMm(Vec(1.32f, 75.43f), Vec(68.45f, 21.41f));
		panel_svg::loadRectFromSvgMm(panelPath, "SPECTRUM", &spectrumRectMm);
		BifurxSpectrumWidget* spectrum = new BifurxSpectrumWidget();
		spectrum->module = module;
		spectrum->box.pos = mm2px(spectrumRectMm.pos);
		spectrum->box.size = mm2px(spectrumRectMm.size);
		addChild(spectrum);

		Vec modePosMm(13.4f, 22.0f);
		Vec levelPosMm(13.4f, 41.0f);
		Vec resoPosMm(13.4f, 60.0f);
		Vec freqPosMm(35.56f, 46.5f);
		Vec titoPosMm(57.7f, 22.0f);
		Vec spanPosMm(57.7f, 41.0f);
		Vec balancePosMm(57.7f, 60.0f);
		Vec fmAmtPosMm(25.3f, 45.0f);
		Vec spanCvAttenPosMm(45.82f, 45.0f);

		Vec inPosMm(7.6f, 112.2f);
		Vec voctPosMm(17.15f, 112.2f);
		Vec fmPosMm(26.7f, 112.2f);
		Vec resoCvPosMm(36.25f, 112.2f);
		Vec balanceCvPosMm(45.8f, 112.2f);
		Vec spanCvPosMm(55.35f, 112.2f);
		Vec outPosMm(64.9f, 112.2f);

		Vec fmLightPosMm(25.3f, 27.3f);
		Vec spanLightPosMm(45.82f, 27.3f);

		applyPointOverride("MODE_PARAM", &modePosMm);
		applyPointOverride("LEVEL_PARAM", &levelPosMm);
		applyPointOverride("RESO_PARAM", &resoPosMm);
		applyPointOverride("FREQ_PARAM", &freqPosMm);
		applyPointOverride("TITO_PARAM", &titoPosMm);
		applyPointOverride("SPAN_PARAM", &spanPosMm);
		applyPointOverride("BALANCE_PARAM", &balancePosMm);
		applyPointOverride("FM_AMT_PARAM", &fmAmtPosMm);
		applyPointOverride("SPAN_CV_ATTEN_PARAM", &spanCvAttenPosMm);

		applyPointOverride("IN_INPUT", &inPosMm);
		applyPointOverride("VOCT_INPUT", &voctPosMm);
		applyPointOverride("FM_INPUT", &fmPosMm);
		applyPointOverride("RESO_CV_INPUT", &resoCvPosMm);
		applyPointOverride("BALANCE_CV_INPUT", &balanceCvPosMm);
		applyPointOverride("SPAN_CV_INPUT", &spanCvPosMm);
		applyPointOverride("OUT_OUTPUT", &outPosMm);

		applyPointOverride("FM_AMT_LIGHT", &fmLightPosMm);
		applyPointOverride("SPAN_CV_ATTEN_LIGHT", &spanLightPosMm);

		addParam(createParamCentered<RoundBlackKnob>(mm2px(modePosMm), module, Bifurx::MODE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(levelPosMm), module, Bifurx::LEVEL_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(freqPosMm), module, Bifurx::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(resoPosMm), module, Bifurx::RESO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(balancePosMm), module, Bifurx::BALANCE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(spanPosMm), module, Bifurx::SPAN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(fmAmtPosMm), module, Bifurx::FM_AMT_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(spanCvAttenPosMm), module, Bifurx::SPAN_CV_ATTEN_PARAM));
		addParam(createParamCentered<CKSSThree>(mm2px(titoPosMm), module, Bifurx::TITO_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(inPosMm), module, Bifurx::IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(voctPosMm), module, Bifurx::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(fmPosMm), module, Bifurx::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(resoCvPosMm), module, Bifurx::RESO_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(balanceCvPosMm), module, Bifurx::BALANCE_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(spanCvPosMm), module, Bifurx::SPAN_CV_INPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(outPosMm), module, Bifurx::OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(fmLightPosMm), module, Bifurx::FM_AMT_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(spanLightPosMm), module, Bifurx::SPAN_CV_ATTEN_LIGHT));
	}
};

Model* modelBifurx = createModel<Bifurx, BifurxWidget>("Bifurx");
