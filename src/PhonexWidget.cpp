#include "Phonex.hpp"

#include "PanelSvgUtils.hpp"
#include "PhonexRom.hpp"
#include "visual/VisualAssets.hpp"

#include <ui/TextField.hpp>

#include <cmath>
#include <cstdio>

namespace {

struct PhonexUtteranceField final : ui::TextField {
	Phonex* module = nullptr;

	explicit PhonexUtteranceField(Phonex* module) : module(module) {
		multiline = false;
		placeholder = "TYPE TEXT OR [PHONEMES]";
		if (module)
			setText(module->submittedText);
	}

	void onSelectKey(const event::SelectKey& event) override {
		if (event.action == GLFW_PRESS
			&& (event.isKeyCommand(GLFW_KEY_ENTER)
				|| event.isKeyCommand(GLFW_KEY_KP_ENTER))) {
			if (module)
				module->submitText(getText());
			event.consume(this);
			return;
		}
		ui::TextField::onSelectKey(event);
	}
};

struct PhonexStatusDisplay final : TransparentWidget {
	Phonex* module = nullptr;

	explicit PhonexStatusDisplay(Phonex* module) : module(module) {}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont)
			return;
		std::string primary = "HELLO";
		std::string secondary;
		NVGcolor primaryColor = nvgRGB(83, 226, 231);
		if (module) {
			const phonex::CompileStatus status = module->textStatus.load(std::memory_order_relaxed);
			if (status != phonex::CompileStatus::Ok && status != phonex::CompileStatus::Empty) {
				primary = phonex::compileStatusText(status);
				primaryColor = nvgRGB(255, 154, 83);
			}
			else {
				primary = module->activeDisplayText();
				if (primary.size() > 34)
					primary = primary.substr(0, 31) + "...";
			}
			if (module->unsupportedUnicode.load(std::memory_order_relaxed))
				secondary = "UNICODE BOUNDARY";
		}
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, primaryColor);
		nvgFontSize(args.vg, 12.f);
		nvgText(args.vg, 6.f, box.size.y * 0.42f, primary.c_str(), nullptr);
		if (!secondary.empty()) {
			nvgFillColor(args.vg, nvgRGB(185, 164, 112));
			nvgFontSize(args.vg, 7.f);
			nvgText(args.vg, 6.f, box.size.y * 0.77f, secondary.c_str(), nullptr);
		}
	}
};

int phonexWordIndex(ParamWidget* widget) {
	return widget && widget->getParamQuantity()
		? clamp(int(std::lround(widget->getParamQuantity()->getValue())), 0, 63)
		: 36;
}

struct PhonexWordTooltip final : ui::Tooltip {
	WeakPtr<Widget> anchor;

	void step() override {
		Widget* widget = anchor.get();
		auto* paramWidget = dynamic_cast<ParamWidget*>(widget);
		const int word = phonexWordIndex(paramWidget);
		const phonex::StringView name = phonex::bundledPhraseName(std::uint8_t(word));
		text = "Word " + std::to_string(word) + " — "
			+ std::string(name.data(), name.size());
		ui::Tooltip::step();
		if (!widget)
			return;
		const float selectorX = visual_assets::neonBarSliderSelectorX(
			widget->box.size.x, float(word) / 63.f);
		const Vec selectorScene = widget->getAbsoluteOffset(Vec(selectorX, 0.f));
		box.pos = Vec(
			selectorScene.x - 0.5f * box.size.x,
			selectorScene.y - box.size.y - 3.f);
	}
};

struct PhonexWordBar final : ParamWidget {
	bool dragging = false;
	bool isHovered = false;
	PhonexWordTooltip* wordTooltip = nullptr;

	~PhonexWordBar() override {
		destroyWordTooltip();
	}

	void onContextCreate(const ContextCreateEvent& e) override {
		visual_assets::onRasterContextCreate(e.vg);
		ParamWidget::onContextCreate(e);
	}

	void onContextDestroy(const ContextDestroyEvent& e) override {
		visual_assets::onRasterContextDestroy(e.vg);
		ParamWidget::onContextDestroy(e);
	}

	Phonex* getPhonexModule() const {
		return dynamic_cast<Phonex*>(module);
	}

	Vec localMousePos() {
		if (!APP || !APP->scene)
			return Vec();
		return APP->scene->getMousePos().minus(getAbsoluteOffset(Vec()))
			.div(std::max(getAbsoluteZoom(), 1e-4f));
	}

	void setFromX(float x) {
		const float normalized = visual_assets::neonBarSliderValueFromX(x, box.size.x);
		if (ParamQuantity* quantity = getParamQuantity())
			quantity->setValue(float(std::lround(normalized * 63.f)));
	}

	void nudge(int direction) {
		if (ParamQuantity* quantity = getParamQuantity()) {
			const int word = clamp(
				int(std::lround(quantity->getValue())) + direction, 0, 63);
			quantity->setValue(float(word));
		}
	}

	void createWordTooltip() {
		if (!settings::tooltips || wordTooltip || !APP || !APP->scene)
			return;
		wordTooltip = new PhonexWordTooltip();
		wordTooltip->anchor = this;
		APP->scene->addChild(wordTooltip);
	}

	void destroyWordTooltip() {
		if (!wordTooltip)
			return;
		if (wordTooltip->parent)
			wordTooltip->parent->removeChild(wordTooltip);
		delete wordTooltip;
		wordTooltip = nullptr;
	}

	void onEnter(const event::Enter& e) override {
		isHovered = true;
		createWordTooltip();
		event::Enter copy = e;
		OpaqueWidget::onEnter(copy);
	}

	void onLeave(const event::Leave& e) override {
		isHovered = false;
		destroyWordTooltip();
		event::Leave copy = e;
		OpaqueWidget::onLeave(copy);
	}

	void onButton(const event::Button& e) override {
		if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
			if (e.action == GLFW_PRESS) {
				dragging = true;
				setFromX(e.pos.x);
				e.consume(this);
				return;
			}
			if (e.action == GLFW_RELEASE && dragging) {
				dragging = false;
				e.consume(this);
				return;
			}
		}
		ParamWidget::onButton(e);
	}

	void onDragMove(const event::DragMove& e) override {
		if (dragging && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			setFromX(localMousePos().x);
			e.consume(this);
			return;
		}
		ParamWidget::onDragMove(e);
	}

	void onDragEnd(const event::DragEnd& e) override {
		dragging = false;
		ParamWidget::onDragEnd(e);
	}

	void onHoverScroll(const event::HoverScroll& e) override {
		if (std::fabs(e.scrollDelta.y) < 1e-4f) {
			ParamWidget::onHoverScroll(e);
			return;
		}
		nudge(e.scrollDelta.y > 0.f ? 1 : -1);
		e.consume(this);
	}

	void draw(const DrawArgs& args) override {
		const int settingWord = phonexWordIndex(this);
		Phonex* phonexModule = getPhonexModule();
		const bool cvSelected = phonexModule
			&& phonexModule->inputs[Phonex::WORD_CV_INPUT].isConnected();
		const int activeWord = cvSelected
			? clamp(phonexModule->selectedWord.load(std::memory_order_relaxed), 0, 63)
			: settingWord;
		const phonex::StringView name = phonex::bundledPhraseName(std::uint8_t(activeWord));
		char label[96]{};
		std::snprintf(label, sizeof(label), "WORD %02d  %.*s",
			activeWord, int(name.size()), name.data());
		visual_assets::drawNeonBarSlider(
			args, box.size, float(settingWord) / 63.f, isHovered, label,
			cvSelected ? float(activeWord) / 63.f : -1.f);

		const float assetHeight = visual_assets::neonBarSliderAssetHeight(box.size.x);
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		for (int word = 0; word < 64; ++word) {
			if (word == settingWord || (cvSelected && word == activeWord))
				continue;
			const float x = visual_assets::neonBarSliderSelectorX(
				box.size.x, float(word) / 63.f);
			const bool major = (word % 8) == 0 || word == 63;
			nvgMoveTo(args.vg, x, assetHeight * (major ? 0.25f : 0.35f));
			nvgLineTo(args.vg, x, assetHeight * (major ? 0.75f : 0.65f));
		}
		nvgStrokeWidth(args.vg, 0.45f);
		nvgStrokeColor(args.vg, nvgRGBA(220, 248, 255, 92));
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}
};

} // namespace

struct PhonexWidget final : ModuleWidget {
	explicit PhonexWidget(Phonex* module) {
		setModule(module);
		PreviewBuildLogTimer previewTimer("Phonex", module);
		visual_assets::SplitPanelRenderer splitPanel(this, "res/Phonex.panel.svg");
		const std::string& panelPath = splitPanel.panelPath();
		splitPanel.addLabels("res/Phonex.labels.svg");
		previewTimer.markPanelDone();

		auto point = [&](const char* id, Vec fallbackMm) {
			Vec value;
			return panel_svg::loadPointFromSvgMm(panelPath, id, &value) ? value : fallbackMm;
		};
		auto rect = [&](const char* id, math::Rect fallbackMm) {
			math::Rect value;
			return panel_svg::loadRectFromSvgMm(panelPath, id, &value) ? value : fallbackMm;
		};

		const math::Rect displayMm = rect("PHRASE_DISPLAY",
			math::Rect(Vec(3.6f, 11.9f), Vec(63.92f, 7.3f)));
		PhonexStatusDisplay* display = new PhonexStatusDisplay(module);
		display->box.pos = mm2px(displayMm.pos);
		display->box.size = mm2px(displayMm.size);
		addChild(display);

		const math::Rect utteranceMm = rect("UTTERANCE_FIELD",
			math::Rect(Vec(3.6f, 23.1f), Vec(63.92f, 7.5f)));
		PhonexUtteranceField* utterance = new PhonexUtteranceField(module);
		utterance->box.pos = mm2px(utteranceMm.pos);
		utterance->box.size = mm2px(utteranceMm.size);
		addChild(utterance);

		auto addMainKnob = [&](int paramId, const char* id, Vec fallbackMm, bool bipolar) {
			Eclipse2Knob* knob = createParamCentered<Eclipse2Knob>(
				mm2px(point(id, fallbackMm)), module, paramId);
			if (bipolar)
				knob->setProgressRingBipolar(true);
			addParam(knob);
		};
		addMainKnob(Phonex::PITCH_PARAM, "PITCH_PARAM", Vec(12.f, 41.f), true);
		addMainKnob(Phonex::SPEED_PARAM, "SPEED_PARAM", Vec(35.56f, 41.f), true);
		addMainKnob(Phonex::WARP_PARAM, "WARP_PARAM", Vec(59.1f, 41.f), true);
		addParam(createParamCentered<BipolarDarkTinyClockworkGearKnob>(mm2px(point("FORMANT_PARAM", Vec(8.8f, 57.f))), module, Phonex::FORMANT_PARAM));
		addParam(createParamCentered<DarkTinyClockworkGearKnob>(mm2px(point("EXCITE_BLEND_PARAM", Vec(26.8f, 57.f))), module, Phonex::EXCITE_BLEND_PARAM));
		addParam(createParamCentered<DarkTinyClockworkGearKnob>(mm2px(point("BEND_PARAM", Vec(44.8f, 57.f))), module, Phonex::BEND_PARAM));
		addParam(createParamCentered<DarkTinyClockworkGearKnob>(mm2px(point("GLITCH_PARAM", Vec(62.8f, 57.f))), module, Phonex::GLITCH_PARAM));
		const math::Rect wordSelectorMm = rect("WORD_SELECTOR",
			math::Rect(Vec(3.f, 68.2f), Vec(65.12f, 11.5f)));
		PhonexWordBar* wordBar = createParam<PhonexWordBar>(
			mm2px(wordSelectorMm.pos), module, Phonex::WORD_PARAM);
		wordBar->box.size = mm2px(wordSelectorMm.size);
		addParam(wordBar);
		addParam(createParamCentered<SmallGoldButton>(mm2px(point("WORD_PUSH_PARAM", Vec(62.4f, 26.85f))), module, Phonex::WORD_PUSH_PARAM));

		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("VOCT_INPUT", Vec(8.5f, 87.5f))), module, Phonex::VOCT_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("WORD_CV_INPUT", Vec(27.f, 87.5f))), module, Phonex::WORD_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("TRIG_GATE_INPUT", Vec(45.f, 87.5f))), module, Phonex::TRIG_GATE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("SCRUB_CV_INPUT", Vec(62.6f, 87.5f))), module, Phonex::SCRUB_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("WARP_CV_INPUT", Vec(12.f, 100.f))), module, Phonex::WARP_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("BEND_CV_INPUT", Vec(35.56f, 100.f))), module, Phonex::BEND_CV_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(mm2px(point("EXT_EXCITE_INPUT", Vec(59.1f, 100.f))), module, Phonex::EXT_EXCITE_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("AUDIO_OUTPUT", Vec(12.f, 114.5f))), module, Phonex::AUDIO_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("FRAME_CLK_OUTPUT", Vec(35.56f, 114.5f))), module, Phonex::FRAME_CLK_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(mm2px(point("EOX_OUTPUT", Vec(59.1f, 114.5f))), module, Phonex::EOX_OUTPUT));
		addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(point("VOICED_LIGHT", Vec(67.6f, 26.85f))), module, Phonex::VOICED_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(point("FRAME_LIGHT", Vec(31.4f, 110.f))), module, Phonex::FRAME_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(point("EOX_LIGHT", Vec(54.9f, 110.f))), module, Phonex::EOX_LIGHT));
		addChild(createLightCentered<SmallLight<YellowLight>>(mm2px(point("BEND_LIGHT", Vec(65.2f, 57.f))), module, Phonex::BEND_LIGHT));

		previewTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));
		previewTimer.markAnchorsDone();
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Phonex* phonexModule = dynamic_cast<Phonex*>(module);
		if (!phonexModule) return;
		menu->addChild(new MenuSeparator);
		menu->addChild(createMenuLabel("Speech engine"));
		menu->addChild(createCheckMenuItem("10 kHz chip clock", "",
			[phonexModule]() { return phonexModule->internalRate.load() == 10000; },
			[phonexModule]() { phonexModule->internalRate.store(10000); }));
		menu->addChild(createCheckMenuItem("8 kHz chip clock", "",
			[phonexModule]() { return phonexModule->internalRate.load() == 8000; },
			[phonexModule]() { phonexModule->internalRate.store(8000); }));
		menu->addChild(createCheckMenuItem("Filtered reconstruction", "",
			[phonexModule]() { return phonexModule->reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered); },
			[phonexModule]() {
				const bool filtered = phonexModule->reconstructionMode.load() == int(phonex::ReconstructionMode::Filtered);
				phonexModule->reconstructionMode.store(filtered ? int(phonex::ReconstructionMode::RawHold) : int(phonex::ReconstructionMode::Filtered));
			}));
		menu->addChild(createCheckMenuItem("Advance one frame trigger", "",
			[phonexModule]() { return phonexModule->triggerMode.load() == int(phonex::TriggerMode::AdvanceOneFrame); },
			[phonexModule]() {
				const bool stepping = phonexModule->triggerMode.load() == int(phonex::TriggerMode::AdvanceOneFrame);
				phonexModule->triggerMode.store(stepping ? int(phonex::TriggerMode::RetriggerPhrase) : int(phonex::TriggerMode::AdvanceOneFrame));
			}));
		menu->addChild(createCheckMenuItem("Force unvoiced excitation", "",
			[phonexModule]() { return phonexModule->forcedExcitation.load() == int(phonex::ForcedExcitation::Unvoiced); },
			[phonexModule]() {
				const bool unvoiced = phonexModule->forcedExcitation.load() == int(phonex::ForcedExcitation::Unvoiced);
				phonexModule->forcedExcitation.store(unvoiced ? int(phonex::ForcedExcitation::Voiced) : int(phonex::ForcedExcitation::Unvoiced));
			}));
	}
};

Model* modelPhonex = createModel<Phonex, PhonexWidget>("Phonex");
