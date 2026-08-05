#include "Longplayer.hpp"

#include "visual/VisualAssets.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <osdialog.h>

namespace {

bool supportedAudioPath(const std::string& path) {
	std::string extension = system::getExtension(path);
	std::transform(
		extension.begin(), extension.end(), extension.begin(),
		[](unsigned char value) { return char(std::tolower(value)); });
	return extension == ".wav" || extension == ".wave"
		|| extension == ".flac" || extension == ".mp3";
}

std::string formatTime(double seconds) {
	seconds = std::max(0.0, seconds);
	const int whole = int(seconds);
	const int hours = whole / 3600;
	const int minutes = (whole / 60) % 60;
	const int remaining = whole % 60;
	return hours > 0
		? string::f("%d:%02d:%02d", hours, minutes, remaining)
		: string::f("%d:%02d", minutes, remaining);
}

struct LongplayerSeekWidget final : TransparentWidget {
	Longplayer* module = nullptr;
	bool dragging = false;

	explicit LongplayerSeekWidget(Longplayer* module)
		: module(module) {
	}

	Vec currentMousePos() const {
		if (!parent || !APP || !APP->scene || !APP->scene->rack) {
			return Vec();
		}
		return APP->scene->rack->getMousePos()
			.minus(parent->box.pos)
			.minus(box.pos);
	}

	void seekAt(float x) {
		if (module && box.size.x > 0.f) {
			module->seekNormalized(clamp(x / box.size.x, 0.f, 1.f));
		}
	}

	void onButton(const event::Button& event) override {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS) {
			dragging = true;
			seekAt(event.pos.x);
			event.consume(this);
			return;
		}
		if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_RELEASE
			&& dragging) {
			dragging = false;
			event.consume(this);
			return;
		}
		TransparentWidget::onButton(event);
	}

	void onDragStart(const event::DragStart& event) override {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT && dragging) {
			seekAt(currentMousePos().x);
			event.consume(this);
			return;
		}
		TransparentWidget::onDragStart(event);
	}

	void onDragMove(const event::DragMove& event) override {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT && dragging) {
			seekAt(currentMousePos().x);
			event.consume(this);
			return;
		}
		TransparentWidget::onDragMove(event);
	}

	void onDragEnd(const event::DragEnd& event) override {
		if (event.button == GLFW_MOUSE_BUTTON_LEFT && dragging) {
			dragging = false;
			event.consume(this);
			return;
		}
		TransparentWidget::onDragEnd(event);
	}

	void draw(const DrawArgs& args) override {
		const float progress = module ? module->progress() : 0.34f;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 0.f, 0.f, box.size.x, box.size.y, 5.f);
		nvgFillColor(args.vg, nvgRGB(8, 10, 18));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(62, 221, 238, 170));
		nvgStrokeWidth(args.vg, 1.f);
		nvgStroke(args.vg);

		const float barY = box.size.y - 13.f;
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, 6.f, barY, box.size.x - 12.f, 5.f, 2.5f);
		nvgFillColor(args.vg, nvgRGB(27, 27, 43));
		nvgFill(args.vg);
		nvgBeginPath(args.vg);
		nvgRoundedRect(
			args.vg, 6.f, barY,
			std::max(0.f, (box.size.x - 12.f) * progress), 5.f, 2.5f);
		nvgFillColor(args.vg, nvgRGB(116, 85, 238));
		nvgFill(args.vg);
		const float handleX = 6.f + (box.size.x - 12.f) * progress;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, handleX, barY + 2.5f, 3.7f);
		nvgFillColor(args.vg, nvgRGB(103, 239, 245));
		nvgFill(args.vg);

		if (!APP || !APP->window || !APP->window->uiFont) {
			return;
		}
		std::string title = "DROP OR LOAD AUDIO";
		std::string detail = "WAV / FLAC / MP3";
		NVGcolor titleColor = nvgRGB(202, 210, 226);
		if (module) {
			if (module->isLoading()) {
				title = "INDEXING...";
				detail = "Preparing stream";
				titleColor = nvgRGB(255, 211, 91);
			}
			else if (module->hasFile()) {
				title = module->displayName();
				detail = formatTime(module->playheadSeconds())
					+ " / " + formatTime(module->durationSeconds());
				if (module->isBuffering()) detail += "  BUFFERING";
			}
			else if (!module->loadError().empty()) {
				title = "LOAD FAILED";
				detail = module->loadError();
				titleColor = nvgRGB(255, 112, 102);
			}
		}
		if (title.size() > 34u) title = title.substr(0u, 31u) + "...";
		if (detail.size() > 46u) detail = detail.substr(0u, 43u) + "...";
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgFontSize(args.vg, 11.f);
		nvgFillColor(args.vg, titleColor);
		nvgText(args.vg, box.size.x * 0.5f, 15.f, title.c_str(), nullptr);
		nvgFontSize(args.vg, 9.f);
		nvgFillColor(args.vg, nvgRGB(128, 151, 171));
		nvgText(args.vg, box.size.x * 0.5f, 31.f, detail.c_str(), nullptr);
	}
};

struct LongplayerLoopButton final : LoopGoldButton {
	LongplayerLoopButton() {
		momentary = false;
	}
};

} // namespace

struct LongplayerWidget final : ModuleWidget {
	explicit LongplayerWidget(Longplayer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Longplayer.svg")));

		auto* seek = new LongplayerSeekWidget(module);
		seek->box.pos = mm2px(Vec(3.f, 25.f));
		seek->box.size = mm2px(Vec(44.8f, 38.f));
		addChild(seek);

		addParam(createLightParamCentered<SmallGoldApertureButton>(
			mm2px(Vec(8.5f, 82.f)), module,
			Longplayer::PLAY_PARAM, Longplayer::PLAY_LIGHT));
		auto* rateKnob = createParamCentered<Eclipse2Knob>(
			mm2px(Vec(25.4f, 82.f)), module, Longplayer::RATE_PARAM);
		rateKnob->setProgressRingBipolar(true);
		addParam(rateKnob);
		addParam(createParamCentered<LongplayerLoopButton>(
			mm2px(Vec(42.3f, 82.f)), module, Longplayer::LOOP_PARAM));
		addInput(createInputCentered<Magitek2InputJack>(
			mm2px(Vec(9.f, 109.f)), module, Longplayer::TRIGGER_INPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(
			mm2px(Vec(29.f, 109.f)), module, Longplayer::LEFT_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(
			mm2px(Vec(42.f, 109.f)), module, Longplayer::RIGHT_OUTPUT));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<CyanOrbScrew>(
			Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0.f)));
		addChild(createWidget<CyanOrbScrew>(
			Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(
			box.size.x - 2.f * RACK_GRID_WIDTH,
			RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	}

	void onPathDrop(const event::PathDrop& event) override {
		auto* player = static_cast<Longplayer*>(module);
		if (player) {
			for (const std::string& path : event.paths) {
				if (system::isFile(path) && supportedAudioPath(path)) {
					player->loadFile(path);
					event.consume(this);
					return;
				}
			}
		}
		ModuleWidget::onPathDrop(event);
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* player = dynamic_cast<Longplayer*>(module);
		if (!player) return;
		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("Longplayer"));
		menu->addChild(createMenuItem("Load audio...", "WAV/FLAC/MP3", [player]() {
			osdialog_filters* filters = osdialog_filters_parse(
				"Audio:wav,WAV,wave,WAVE,flac,FLAC,mp3,MP3");
			char* selected = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
			osdialog_filters_free(filters);
			if (!selected) return;
			const std::string path = selected;
			std::free(selected);
			std::string error;
			if (!player->loadFile(path, &error)) {
				osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, error.c_str());
			}
		}));
		menu->addChild(createMenuItem(
			"Clear audio", "", [player]() { player->clearFile(); },
			!player->hasFile() && !player->isLoading()));
	}
};

Model* modelLongplayer = createModel<Longplayer, LongplayerWidget>("Longplayer");
