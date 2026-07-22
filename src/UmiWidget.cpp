#include "Umi.hpp"
#include "NvgGraphicsLifecycle.hpp"
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

struct UmiPanelArtWidget final : TransparentWidget {
	NVGcontext* ownerVg = nullptr;
	int imageHandle = -1;
	int imageWidth = 0;
	int imageHeight = 0;
	std::string loadedPath;

	~UmiPanelArtWidget() override {
		NVGcontext* currentVg = APP && APP->window ? APP->window->vg : nullptr;
		nvg_gfx_lifecycle::resetOwnedNvgImage(
			ownerVg, imageHandle, imageWidth, imageHeight,
			currentVg, ownerVg == currentVg);
	}

	bool ensureImage(NVGcontext* vg) {
		if (!vg) return false;
		// Keep the initial Umi distribution lean: the 2x panel is used at every
		// display density until a higher-resolution runtime asset is re-enabled.
		const char* relativePath = "res/Umi/panel_base@2x.png";
		const std::string desiredPath = asset::plugin(pluginInstance, relativePath);
		if (ownerVg == vg && loadedPath == desiredPath && imageHandle >= 0 &&
			imageWidth > 0 && imageHeight > 0 &&
			nvg_gfx_lifecycle::ownedNvgImageSizeMatches(vg, imageHandle, imageWidth, imageHeight)) {
			return true;
		}
		nvg_gfx_lifecycle::resetOwnedNvgImage(
			ownerVg, imageHandle, imageWidth, imageHeight,
			vg, ownerVg == vg);
		loadedPath.clear();
		imageHandle = nvgCreateImage(vg, desiredPath.c_str(), NVG_IMAGE_GENERATE_MIPMAPS);
		if (imageHandle < 0) return false;
		ownerVg = vg;
		loadedPath = desiredPath;
		nvgImageSize(vg, imageHandle, &imageWidth, &imageHeight);
		if (imageWidth <= 0 || imageHeight <= 0) {
			nvg_gfx_lifecycle::resetOwnedNvgImage(
				ownerVg, imageHandle, imageWidth, imageHeight, vg, true);
			loadedPath.clear();
			return false;
		}
		return true;
	}

	void draw(const DrawArgs& args) override {
		if (!ensureImage(args.vg)) return;
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, box.size.x, box.size.y);
		const NVGpaint imagePaint = nvgImagePattern(
			args.vg, 0.f, 0.f, box.size.x, box.size.y, 0.f, imageHandle, 1.f);
		nvgFillPaint(args.vg, imagePaint);
		nvgFill(args.vg);

		const float sideWidth = box.size.x * (84.f / 1080.f);
		const NVGpaint leftRail = nvgLinearGradient(args.vg, 0.f, 0.f, sideWidth, 0.f,
			nvgRGBA(1, 10, 48, 245), nvgRGBA(12, 124, 185, 155));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.f, 0.f, sideWidth, box.size.y);
		nvgFillPaint(args.vg, leftRail);
		nvgFill(args.vg);
		const NVGpaint rightRail = nvgLinearGradient(args.vg, box.size.x - sideWidth, 0.f, box.size.x, 0.f,
			nvgRGBA(12, 124, 185, 155), nvgRGBA(1, 10, 48, 245));
		nvgBeginPath(args.vg);
		nvgRect(args.vg, box.size.x - sideWidth, 0.f, sideWidth, box.size.y);
		nvgFillPaint(args.vg, rightRail);
		nvgFill(args.vg);
		for (float x : {sideWidth, box.size.x - sideWidth}) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, x, 0.f);
			nvgLineTo(args.vg, x, box.size.y);
			nvgStrokeColor(args.vg, nvgRGBA(244, 204, 111, 185));
			nvgStrokeWidth(args.vg, 1.f);
			nvgStroke(args.vg);
		}

		const float utilityBandY = mm2px(109.f);
		nvgBeginPath(args.vg);
		nvgRoundedRect(args.vg, mm2px(2.2f), utilityBandY,
			box.size.x - mm2px(4.4f), mm2px(16.5f), mm2px(1.8f));
		nvgFillColor(args.vg, nvgRGBA(0, 9, 42, 118));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(88, 221, 247, 90));
		nvgStrokeWidth(args.vg, 0.8f);
		nvgStroke(args.vg);
	}
};

struct UmiLabelOverlayWidget final : TransparentWidget {
	void drawLabel(const DrawArgs& args, const char* text, float xMm, float yMm,
		float sizeMm, NVGcolor color, int align = NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE) {
		if (!APP || !APP->window || !APP->window->uiFont) return;
		const Vec p = mm2px(Vec(xMm, yMm));
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, mm2px(sizeMm));
		nvgTextAlign(args.vg, align);
		nvgFontBlur(args.vg, 0.f);
		nvgFillColor(args.vg, nvgRGBA(0, 5, 24, 230));
		nvgText(args.vg, p.x + 1.f, p.y + 1.f, text, nullptr);
		nvgFillColor(args.vg, color);
		nvgText(args.vg, p.x, p.y, text, nullptr);
	}

	void draw(const DrawArgs& args) override {
		drawLabel(args, "DROP", 8.f, 9.f, 1.25f, nvgRGB(226, 251, 255));
		drawLabel(args, "DROP", 23.f, 9.f, 1.25f, nvgRGB(226, 251, 255));
		drawLabel(args, "RATE", 39.5f, 9.f, 1.25f, nvgRGB(226, 251, 255));
		drawLabel(args, "DENS", 57.f, 9.f, 1.25f, nvgRGB(226, 251, 255));

		const NVGcolor labelColor = nvgRGB(224, 251, 255);
		const NVGcolor cvColor = nvgRGB(119, 235, 255);
		drawLabel(args, "GRAV", 6.5f, 31.3f, 1.25f, labelColor);
		drawLabel(args, "CV", 16.f, 31.3f, 1.1f, cvColor);
		drawLabel(args, "BOUNCE", 6.5f, 51.8f, 1.05f, labelColor);
		drawLabel(args, "CV", 16.f, 51.8f, 1.1f, cvColor);
		drawLabel(args, "DRAG", 11.f, 72.8f, 1.25f, labelColor);
		drawLabel(args, "TILT", 84.9f, 31.3f, 1.25f, labelColor);
		drawLabel(args, "CV", 75.4f, 31.3f, 1.1f, cvColor);
		drawLabel(args, "CHAOS", 84.9f, 51.8f, 1.1f, labelColor);
		drawLabel(args, "CV", 75.4f, 51.8f, 1.1f, cvColor);
		drawLabel(args, "CLEAR", 84.4f, 72.8f, 1.1f, labelColor);
		drawLabel(args, "IN", 75.4f, 72.8f, 1.1f, cvColor);

		const char* outputLabels[] = {"GATES", "ANY", "LEFT", "RIGHT", "VEL", "POS", "ACT"};
		for (int i = 0; i < 7; ++i) {
			drawLabel(args, outputLabels[i], 8.f + 12.57f * float(i), 111.f, 1.15f,
				i == 0 ? nvgRGB(255, 229, 154) : labelColor);
		}
	}
};

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

		const Transform transform = boardTransform();
		auto strokeSideBumper = [&](bool rightSide, NVGcolor color, float strokeWidth) {
			nvgBeginPath(args.vg);
			bool started = false;
			for (int i = 0; i < layout.segmentCount; ++i) {
				const umi::Segment& segment = layout.segments[static_cast<std::size_t>(i)];
				if (segment.material != 2 || (segment.a.x >= umi::BOARD_W * 0.5f) != rightSide) continue;
				const Vec a = boardToLocal(segment.a);
				const Vec b = boardToLocal(segment.b);
				if (!started) {
					nvgMoveTo(args.vg, a.x, a.y);
					started = true;
				}
				nvgLineTo(args.vg, b.x, b.y);
			}
			if (!started) return;
			nvgStrokeColor(args.vg, color);
			nvgStrokeWidth(args.vg, strokeWidth);
			nvgLineCap(args.vg, NVG_ROUND);
			nvgLineJoin(args.vg, NVG_ROUND);
			nvgStroke(args.vg);
		};
		const float sideWidth = std::max(1.f, 20.f * transform.scale);
		for (int side = 0; side < 2; ++side) {
			strokeSideBumper(side != 0, nvgRGBA(0, 10, 45, 225), sideWidth * 1.75f);
			strokeSideBumper(side != 0, nvgRGBA(62, 226, 255, 220), sideWidth * 1.25f);
			strokeSideBumper(side != 0, nvgRGBA(248, 210, 111, 245), sideWidth * 0.62f);
		}
		for (int i = 0; i < layout.segmentCount; ++i) {
			const umi::Segment& segment = layout.segments[static_cast<std::size_t>(i)];
			if (segment.material == 2) continue;
			const Vec a = boardToLocal(segment.a);
			const Vec b = boardToLocal(segment.b);
			const float width = std::max(1.f, segment.radius * 2.f * transform.scale);
			auto strokeRail = [&](NVGcolor color, float strokeWidth) {
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, a.x, a.y);
				nvgLineTo(args.vg, b.x, b.y);
				nvgStrokeColor(args.vg, color);
				nvgStrokeWidth(args.vg, strokeWidth);
				nvgLineCap(args.vg, NVG_ROUND);
				nvgStroke(args.vg);
			};
			strokeRail(nvgRGBA(0, 10, 45, 225), width * 1.75f);
			strokeRail(nvgRGBA(62, 226, 255, 220), width * 1.25f);
			strokeRail(nvgRGBA(248, 210, 111, 245), width * 0.62f);
		}

		for (int i = 0; i < layout.pegCount; ++i) {
			const umi::Peg& peg = layout.pegs[static_cast<std::size_t>(i)];
			const Vec center = boardToLocal(peg.pos);
			const float radius = std::max(1.2f, peg.radius * transform.scale);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius * 1.45f);
			nvgFillColor(args.vg, nvgRGBA(30, 214, 255, 48));
			nvgFill(args.vg);
			const NVGpaint jewel = nvgRadialGradient(args.vg,
				center.x - radius * 0.35f, center.y - radius * 0.4f,
				radius * 0.05f, radius,
				nvgRGB(255, 255, 244), peg.visualType ? nvgRGB(161, 40, 219) : nvgRGB(205, 143, 50));
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius);
			nvgFillPaint(args.vg, jewel);
			nvgFill(args.vg);
			nvgStrokeColor(args.vg, nvgRGBA(255, 238, 167, 235));
			nvgStrokeWidth(args.vg, 0.7f);
			nvgStroke(args.vg);
		}

		for (int i = 0; i < umi::SINK_COUNT; ++i) {
			const Vec center = boardToLocal(layout.sinks[static_cast<std::size_t>(i)].pos);
			const float radius = layout.sinks[static_cast<std::size_t>(i)].radius * transform.scale;
			const float flash = sinkFlash[static_cast<std::size_t>(i)];
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, radius + flash * 3.f);
			nvgFillColor(args.vg, nvgRGBA(2, 7, 24, 245));
			nvgFill(args.vg);
			nvgStrokeColor(args.vg, nvgRGBA(247, 205 + int(flash * 40.f), 110 + int(flash * 120.f), 240));
			nvgStrokeWidth(args.vg, 1.2f + flash * 2.2f);
			nvgStroke(args.vg);
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, center.x, center.y, std::max(0.5f, radius - 2.f));
			nvgStrokeColor(args.vg, nvgRGBA(75, 224, 255, 210));
			nvgStrokeWidth(args.vg, 0.8f);
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

	auto* artFramebuffer = new widget::FramebufferWidget();
	artFramebuffer->box.size = box.size;
	artFramebuffer->dirtyOnSubpixelChange = false;
	auto* panelArt = new UmiPanelArtWidget();
	panelArt->box.size = box.size;
	artFramebuffer->addChild(panelArt);
	addChild(artFramebuffer);

	math::Rect playfieldMm;
	if (!panel_svg::loadRectFromSvgMm(panelPath, "playfield_rect", &playfieldMm)) {
		playfieldMm.pos = Vec(20.5f, 20.5f);
		playfieldMm.size = Vec(50.4f, 75.f);
	}
	auto* playfield = new UmiPlayfieldWidget(module);
	playfield->box.pos = mm2px(playfieldMm.pos);
	playfield->box.size = mm2px(playfieldMm.size);
	addChild(playfield);
	auto* labels = new UmiLabelOverlayWidget();
	labels->box.size = box.size;
	addChild(labels);

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

	addInputPort(Umi::DROP_INPUT, "drop_input", Vec(8.f, 14.5f));
	addButton(Umi::DROP_PARAM, Umi::DROP_LIGHT, "drop_param", Vec(23.f, 14.5f));
	addKnob(Umi::RATE_PARAM, "rate_param", Vec(39.5f, 14.5f));
	addKnob(Umi::DENSITY_PARAM, "density_param", Vec(57.f, 14.5f));
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

	const char* outputIds[] = {"gates_output", "any_output", "left_output", "right_output",
		"velocity_output", "position_output", "activity_output"};
	for (int i = 0; i < 7; ++i) {
		addOutputPort(Umi::GATES_OUTPUT + i, outputIds[i], Vec(8.f + 12.57f * i, 118.f));
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
