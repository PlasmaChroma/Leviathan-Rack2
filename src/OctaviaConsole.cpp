#include "plugin.hpp"
#include "OctaviaConsoleMailbox.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

#include <ui/TextField.hpp>
#include <ui/ScrollWidget.hpp>

#include <algorithm>

struct OctaviaConsole : Module {
	enum ParamId { SEND_PARAM, PARAMS_LEN };
	enum InputId { INPUTS_LEN };
	enum OutputId { OUTPUTS_LEN };
	enum LightId { LIGHTS_LEN };

	std::shared_ptr<octavia_console::Mailbox> mailbox =
		std::make_shared<octavia_console::Mailbox>();
	std::atomic<bool> attached{false};
	int64_t registeredId = -1;

	OctaviaConsole() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configButton(SEND_PARAM, "Send prompt");
	}

	~OctaviaConsole() override {
		octavia_console::unregisterMailbox(registeredId, mailbox);
	}

	void process(const ProcessArgs&) override {
		Module* left = leftExpander.module;
		attached.store(left && left->model == modelOctavia, std::memory_order_relaxed);
	}

	void refreshRegistration() {
		if (registeredId == id) return;
		octavia_console::unregisterMailbox(registeredId, mailbox);
		registeredId = id;
		octavia_console::registerMailbox(registeredId, mailbox);
	}
};

struct OctaviaConsoleTranscript : TransparentWidget {
	std::string text;

	void setTranscript(std::string next) {
		text = std::move(next);
	}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 10.f);
		nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgFillColor(args.vg, nvgRGB(225, 238, 241));
		if (text.empty()) {
			nvgFillColor(args.vg, nvgRGB(125, 145, 150));
			nvgText(args.vg, 4.f, 4.f, "Conversation history appears here", nullptr);
			return;
		}
		nvgTextBox(args.vg, 4.f, 4.f, box.size.x - 8.f, text.c_str(), nullptr);
	}
};

struct OctaviaConsolePromptField : ui::TextField {
	OctaviaConsolePromptField() {
		multiline = true;
		placeholder = "Prompt Octavia...";
	}
};

struct OctaviaConsoleStatus : TransparentWidget {
	OctaviaConsole* module = nullptr;
	explicit OctaviaConsoleStatus(OctaviaConsole* module) : module(module) {}

	void draw(const DrawArgs& args) override {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		const bool attached = module && module->attached.load(std::memory_order_relaxed);
		octavia_console::Snapshot snapshot;
		if (module) snapshot = module->mailbox->snapshot();
		const char* label = attached ? octavia_console::agentStateName(snapshot.state) : "DETACHED";
		NVGcolor color = !attached ? nvgRGB(150, 150, 150) :
			(snapshot.state == octavia_console::AgentState::ERROR ? nvgRGB(255, 90, 90) :
			 snapshot.state == octavia_console::AgentState::REPLY ? nvgRGB(90, 255, 180) :
			 nvgRGB(90, 210, 255));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 10.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, box.size.x * .5f, box.size.y * .5f, label, nullptr);
	}
};

struct OctaviaConsoleWidget : ModuleWidget {
	ui::ScrollWidget* responseScroll = nullptr;
	OctaviaConsoleTranscript* transcript = nullptr;
	OctaviaConsolePromptField* promptField = nullptr;
	std::string displayedTranscript;
	bool sendWasHigh = false;

	OctaviaConsoleWidget(OctaviaConsole* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/OctaviaConsole.svg");
		setPanel(createPanel(panelPath));
		addChild(createWidget<CyanOrbScrew>(Vec(0, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		math::Rect responseMm(Vec(3.f, 17.f), Vec(115.92f, 55.f));
		math::Rect promptMm(Vec(3.f, 79.f), Vec(115.92f, 31.f));
		panel_svg::loadRectFromSvgMm(panelPath, "RESPONSE_FIELD", &responseMm);
		panel_svg::loadRectFromSvgMm(panelPath, "PROMPT_FIELD", &promptMm);
		responseScroll = new ui::ScrollWidget;
		responseScroll->box.pos = mm2px(responseMm.pos);
		responseScroll->box.size = mm2px(responseMm.size);
		addChild(responseScroll);
		transcript = new OctaviaConsoleTranscript;
		transcript->box.size.x = responseScroll->box.size.x - 12.f;
		transcript->box.size.y = responseScroll->box.size.y;
		responseScroll->container->addChild(transcript);
		promptField = new OctaviaConsolePromptField;
		promptField->box.pos = mm2px(promptMm.pos);
		promptField->box.size = mm2px(promptMm.size);
		addChild(promptField);

		OctaviaConsoleStatus* status = new OctaviaConsoleStatus(module);
		status->box.pos = mm2px(Vec(3.f, 112.5f));
		status->box.size = mm2px(Vec(98.f, 8.f));
		addChild(status);
		addParam(createParamCentered<SmallGoldButton>(mm2px(Vec(112.f, 116.5f)), module,
			OctaviaConsole::SEND_PARAM));
	}

	void step() override {
		ModuleWidget::step();
		OctaviaConsole* console = dynamic_cast<OctaviaConsole*>(module);
		if (!console) return;
		console->refreshRegistration();
		auto snapshot = console->mailbox->snapshot();
		if (snapshot.transcript != displayedTranscript) {
			displayedTranscript = snapshot.transcript;
			transcript->setTranscript(displayedTranscript);
			const std::size_t lines = 1 + std::count(displayedTranscript.begin(), displayedTranscript.end(), '\n');
			const std::size_t wrappedLines = displayedTranscript.size() / 72;
			transcript->box.size.y = std::max(responseScroll->box.size.y,
				8.f + 13.f * static_cast<float>(lines + wrappedLines));
			responseScroll->scrollTo(math::Rect(Vec(0.f, transcript->box.size.y - 1.f), Vec(1.f, 1.f)));
		}

		const bool sendHigh = console->params[OctaviaConsole::SEND_PARAM].getValue() > .5f;
		if (sendHigh && !sendWasHigh && console->attached.load(std::memory_order_relaxed)) {
			std::string error;
			if (console->mailbox->submitPrompt(promptField->getText(), nullptr, &error))
				promptField->setText("");
			else {
				console->mailbox->setError(std::move(error));
			}
		}
		sendWasHigh = sendHigh;
	}

	void draw(const DrawArgs& args) override {
		ModuleWidget::draw(args);
		if (!APP || !APP->window || !APP->window->uiFont) return;
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFillColor(args.vg, nvgRGB(240, 240, 240));
		nvgFontSize(args.vg, 18.f);
		nvgText(args.vg, box.size.x * .5f, mm2px(7.5f), "Octavia Console", nullptr);
		nvgFontSize(args.vg, 8.f);
		nvgText(args.vg, box.size.x * .5f, mm2px(14.f), "AGENT", nullptr);
		nvgText(args.vg, box.size.x * .5f, mm2px(76.f), "PROMPT", nullptr);
		nvgText(args.vg, mm2px(112.f), mm2px(123.f), "SEND", nullptr);
	}
};

Model* modelOctaviaConsole = createModel<OctaviaConsole, OctaviaConsoleWidget>("OctaviaConsole");
