#include "Umi.hpp"
#include "PanelSvgUtils.hpp"
#include "visual/VisualAssets.hpp"

#include <array>
#include <cerrno>
#include <cmath>
#include <cstdlib>

namespace {

bool loadAnchorPointMm(const std::string& panelPath, const char* id, Vec* outMm, Vec fallbackMm) {
	if (panel_svg::loadPointFromSvgMm(panelPath, id, outMm)) return true;
	*outMm = fallbackMm;
	return false;
}

struct UmiPlayfieldWidget final : Widget {
	Umi* module = nullptr;
	Umi::RenderSnapshot snapshot;
	umi::Layout layout = umi::makePearlLayout(1u);
	std::uint32_t layoutSeed = 1u;
	std::array<std::uint32_t, umi::SINK_COUNT> seenCaptureSerial {};
	std::array<float, umi::SINK_COUNT> sinkFlash {};

	explicit UmiPlayfieldWidget(Umi* module)
		: module(module) {
		snapshot.seed = 1u;
	}

	struct Transform {
		Vec offset;
		float scale = 1.f;

		Transform() = default;
		Transform(Vec offsetValue, float scaleValue)
			: offset(offsetValue), scale(scaleValue) {
		}
	};

	Transform boardTransform() const {
		const float scale = std::min(box.size.x / umi::BOARD_W, box.size.y / umi::BOARD_H);
		return {box.size.minus(Vec(umi::BOARD_W * scale, umi::BOARD_H * scale)).mult(0.5f), scale};
	}

	Vec boardToLocal(umi::Vec2 point) const {
		const Transform transform = boardTransform();
		return transform.offset.plus(Vec(point.x * transform.scale, point.y * transform.scale));
	}

	void step() override {
		if (module) {
			Umi::RenderSnapshot next;
			if (module->consumeLatestSnapshot(&next)) {
				snapshot = next;
				if (snapshot.seed != layoutSeed) {
					layout = umi::makePearlLayout(snapshot.seed);
					layoutSeed = snapshot.seed;
				}
				for (int i = 0; i < umi::SINK_COUNT; ++i) {
					const std::size_t index = static_cast<std::size_t>(i);
					if (snapshot.captureSerial[index] != seenCaptureSerial[index]) {
						sinkFlash[index] = 1.f;
						seenCaptureSerial[index] = snapshot.captureSerial[index];
					}
				}
			}
		}
		const float frameSeconds = APP && APP->window
			? clamp(float(APP->window->getLastFrameDuration()), 0.f, 0.1f)
			: 1.f / 60.f;
		for (float& flash : sinkFlash) flash = std::max(0.f, flash - frameSeconds * 4.5f);
		Widget::step();
	}

	void onButton(const event::Button& e) override {
		if (module && e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
			const Transform transform = boardTransform();
			const float boardX = (e.pos.x - transform.offset.x) / transform.scale;
			Umi::UiCommand command;
			command.type = Umi::UiCommandType::DropAtX;
			command.value = clamp((boardX - 100.f) / 800.f, 0.f, 1.f);
			module->enqueueUiCommand(command);
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);
		nvgScissor(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		const NVGpaint water = nvgLinearGradient(args.vg, 0.f, 0.f, 0.f, box.size.y,
			nvgRGB(18, 186, 220), nvgRGB(2, 10, 45));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		nvgFillPaint(args.vg, water);
		nvgFill(args.vg);

		const Transform transform = boardTransform();
		for (int i = 0; i < layout.segmentCount; ++i) {
			const umi::Segment& segment = layout.segments[static_cast<std::size_t>(i)];
			const Vec a = boardToLocal(segment.a);
			const Vec b = boardToLocal(segment.b);
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, a.x, a.y);
			nvgLineTo(args.vg, b.x, b.y);
			nvgStrokeColor(args.vg, nvgRGBA(245, 206, 105, 225));
			nvgStrokeWidth(args.vg, std::max(1.f, segment.radius * 2.f * transform.scale));
			nvgLineCap(args.vg, NVG_ROUND);
			nvgStroke(args.vg);
		}

		for (int i = 0; i < layout.pegCount; ++i) {
			const umi::Peg& peg = layout.pegs[static_cast<std::size_t>(i)];
			const Vec center = boardToLocal(peg.pos);
			const float radius = std::max(1.2f, peg.radius * transform.scale);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius);
			nvgFillColor(args.vg, peg.visualType ? nvgRGB(210, 93, 244) : nvgRGB(246, 220, 143));
			nvgFill(args.vg);
		}

		for (int i = 0; i < umi::SINK_COUNT; ++i) {
			const Vec center = boardToLocal(layout.sinks[static_cast<std::size_t>(i)].pos);
			const float radius = layout.sinks[static_cast<std::size_t>(i)].radius * transform.scale;
			const float flash = sinkFlash[static_cast<std::size_t>(i)];
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius + flash * 3.f);
			nvgFillColor(args.vg, nvgRGBA(2, 7, 24, 245));
			nvgFill(args.vg);
			nvgStrokeColor(args.vg, nvgRGBA(120 + int(flash * 120.f), 225, 255, 230));
			nvgStrokeWidth(args.vg, 1.f + flash * 2.f);
			nvgStroke(args.vg);
		}

		for (std::uint32_t i = 0; i < snapshot.ballCount; ++i) {
			const Umi::BallRenderState& ball = snapshot.balls[static_cast<std::size_t>(i)];
			const Vec center = boardToLocal(ball.pos);
			const float radius = std::max(1.5f, ball.radius * transform.scale);
			const NVGpaint pearl = nvgRadialGradient(args.vg,
				center.x - radius * 0.35f, center.y - radius * 0.35f,
				radius * 0.1f, radius,
				nvgRGB(255, 255, 247), nvgRGB(62, 179, 232));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius);
			nvgFillPaint(args.vg, pearl);
			nvgFill(args.vg);
		}

		if (module && isDragonKingDebugEnabled()) {
			nvgFontSize(args.vg, 10.f);
			nvgFillColor(args.vg, nvgRGBA(255, 255, 255, 220));
			nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
			nvgText(args.vg, 3.f, 3.f, string::f("balls %u  seed %u", snapshot.ballCount, snapshot.seed).c_str(), nullptr);
		}
		nvgRestore(args.vg);
	}
};

} // namespace

UmiWidget::UmiWidget(Umi* module) {
	setModule(module);
	PreviewBuildLogTimer previewBuildTimer("Umi", module);
	const std::string panelPath = asset::plugin(pluginInstance, "res/Umi.svg");
	setPanel(createPanel(panelPath));
	previewBuildTimer.markPanelDone();
	previewBuildTimer.setAtlasStatus(panel_svg::getAtlasStatusLabelForSvg(panelPath));

	math::Rect playfieldMm;
	if (!panel_svg::loadRectFromSvgMm(panelPath, "playfield_rect", &playfieldMm)) {
		playfieldMm.pos = Vec(21.5f, 26.3f);
		playfieldMm.size = Vec(48.5f, 64.9f);
	}
	auto* playfield = new UmiPlayfieldWidget(module);
	playfield->box.pos = mm2px(playfieldMm.pos);
	playfield->box.size = mm2px(playfieldMm.size);
	addChild(playfield);

	auto anchor = [&](const char* id, Vec fallbackMm) {
		Vec point;
		loadAnchorPointMm(panelPath, id, &point, fallbackMm);
		return mm2px(point);
	};
	auto addKnob = [&](int paramId, const char* id, Vec fallbackMm, bool bipolar = false) {
		if (bipolar) addParam(createParamCentered<BipolarTinyClockworkGearKnob>(anchor(id, fallbackMm), module, paramId));
		else addParam(createParamCentered<TinyClockworkGearKnob>(anchor(id, fallbackMm), module, paramId));
	};
	auto addButton = [&](int paramId, int lightId, const char* id, Vec fallbackMm) {
		addParam(createLightParamCentered<SmallGoldApertureButton>(anchor(id, fallbackMm), module, paramId, lightId));
	};
	auto addInputPort = [&](int inputId, const char* id, Vec fallbackMm) {
		addInput(createInputCentered<Magitek2InputJack>(anchor(id, fallbackMm), module, inputId));
	};
	auto addOutputPort = [&](int outputId, const char* id, Vec fallbackMm) {
		addOutput(createOutputCentered<Magitek2OutputJack>(anchor(id, fallbackMm), module, outputId));
	};

	addInputPort(Umi::DROP_INPUT, "drop_input", Vec(8.f, 17.5f));
	addButton(Umi::DROP_PARAM, Umi::DROP_LIGHT, "drop_param", Vec(23.f, 17.5f));
	addKnob(Umi::RATE_PARAM, "rate_param", Vec(39.5f, 17.5f));
	addKnob(Umi::DENSITY_PARAM, "density_param", Vec(57.f, 17.5f));
	addParam(createParamCentered<SmallGoldButton>(anchor("seed_param", Vec(82.5f, 17.5f)), module, Umi::SEED_PARAM));

	addKnob(Umi::GRAVITY_PARAM, "gravity_param", Vec(6.5f, 37.5f));
	addInputPort(Umi::GRAVITY_CV_INPUT, "gravity_cv_input", Vec(16.f, 37.5f));
	addKnob(Umi::BOUNCE_PARAM, "bounce_param", Vec(6.5f, 58.f));
	addInputPort(Umi::BOUNCE_CV_INPUT, "bounce_cv_input", Vec(16.f, 58.f));
	addKnob(Umi::DRAG_PARAM, "drag_param", Vec(11.f, 79.f));

	addKnob(Umi::TILT_PARAM, "tilt_param", Vec(84.9f, 37.5f), true);
	addInputPort(Umi::TILT_CV_INPUT, "tilt_cv_input", Vec(75.4f, 37.5f));
	addKnob(Umi::CHAOS_PARAM, "chaos_param", Vec(84.9f, 58.f));
	addInputPort(Umi::CHAOS_CV_INPUT, "chaos_cv_input", Vec(75.4f, 58.f));
	addButton(Umi::CLEAR_PARAM, Umi::CLEAR_LIGHT, "clear_param", Vec(84.4f, 79.f));
	addInputPort(Umi::CLEAR_INPUT, "clear_input", Vec(75.4f, 79.f));

	const char* sinkIds[] = {"sink_1_output", "sink_2_output", "sink_3_output", "sink_4_output",
		"sink_5_output", "sink_6_output", "sink_7_output", "sink_8_output"};
	for (int i = 0; i < umi::SINK_COUNT; ++i) {
		addOutputPort(Umi::SINK1_OUTPUT + i, sinkIds[i], Vec(6.f + 11.35f * i, 102.f));
	}
	const char* utilityIds[] = {"any_output", "left_output", "right_output", "velocity_output", "position_output", "activity_output"};
	for (int i = 0; i < 6; ++i) {
		addOutputPort(Umi::ANY_OUTPUT + i, utilityIds[i], Vec(9.f + 14.69f * i, 118.f));
	}

	addChild(createWidgetCentered<TorxScrew>(anchor("screw_tl", Vec(3.f, 3.f))));
	addChild(createWidgetCentered<TorxScrew>(anchor("screw_tr", Vec(88.4f, 3.f))));
	addChild(createWidgetCentered<TorxScrew>(anchor("screw_bl", Vec(3.f, 125.5f))));
	addChild(createWidgetCentered<TorxScrew>(anchor("screw_br", Vec(88.4f, 125.5f))));
	previewBuildTimer.markAnchorsDone();
}

void UmiWidget::appendContextMenu(Menu* menu) {
	ModuleWidget::appendContextMenu(menu);
	auto* umiModule = dynamic_cast<Umi*>(module);
	if (!umiModule) return;
	menu->addChild(new MenuSeparator());

	static const char* pulseLabels[] = {"1 ms", "5 ms", "10 ms", "20 ms", "50 ms"};
	menu->addChild(createSubmenuItem("Pulse length", pulseLabels[clamp(umiModule->pulseLengthIndex.load(), 0, 4)], [umiModule](Menu* child) {
		for (int i = 0; i < 5; ++i) {
			child->addChild(createCheckMenuItem(pulseLabels[i], "",
				[umiModule, i]() { return umiModule->pulseLengthIndex.load() == i; },
				[umiModule, i]() { umiModule->pulseLengthIndex.store(i); }));
		}
	}));

	menu->addChild(createSubmenuItem("Maximum pearls", string::f("%d", umiModule->maxBallsSetting.load()), [umiModule](Menu* child) {
		for (int value : {16, 32, 64}) {
			child->addChild(createCheckMenuItem(string::f("%d", value), "",
				[umiModule, value]() { return umiModule->maxBallsSetting.load() == value; },
				[umiModule, value]() { umiModule->maxBallsSetting.store(value); }));
		}
	}));
	menu->addChild(createCheckMenuItem("Replace oldest when full", "",
		[umiModule]() { return umiModule->replaceOldestSetting.load(); },
		[umiModule]() { umiModule->replaceOldestSetting.store(!umiModule->replaceOldestSetting.load()); }));

	menu->addChild(createMenuItem("Randomize seed", "", [umiModule]() {
		Umi::UiCommand command;
		command.type = Umi::UiCommandType::SetSeed;
		command.seed = random::u32();
		umiModule->enqueueUiCommand(command);
	}));
	menu->addChild(createMenuItem("Copy seed", "", [umiModule]() {
		if (APP && APP->window && APP->window->win) {
			const std::string text = std::to_string(umiModule->publishedSeed.load());
			glfwSetClipboardString(APP->window->win, text.c_str());
		}
	}));
	menu->addChild(createMenuItem("Paste seed", "", [umiModule]() {
		if (!APP || !APP->window || !APP->window->win) return;
		const char* text = glfwGetClipboardString(APP->window->win);
		if (!text || !*text) return;
		errno = 0;
		char* end = nullptr;
		const unsigned long parsed = std::strtoul(text, &end, 10);
		if (errno != 0 || end == text || *end != '\0') return;
		Umi::UiCommand command;
		command.type = Umi::UiCommandType::SetSeed;
		command.seed = std::uint32_t(parsed);
		umiModule->enqueueUiCommand(command);
	}));
	menu->addChild(createMenuItem("Reset board", "", [umiModule]() {
		Umi::UiCommand command;
		command.type = Umi::UiCommandType::ResetBoard;
		umiModule->enqueueUiCommand(command);
	}));
	menu->addChild(createMenuItem("Clear pearls", "", [umiModule]() {
		Umi::UiCommand command;
		command.type = Umi::UiCommandType::Clear;
		umiModule->enqueueUiCommand(command);
	}));
}

Model* modelUmi = createModel<Umi, UmiWidget>("Umi");
