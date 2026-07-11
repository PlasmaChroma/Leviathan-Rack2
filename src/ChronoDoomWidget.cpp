#include "ChronoDoom.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"
#include <osdialog.h>

struct ChronoDoomViewportWidget final : Widget {
	ChronoDoomModule* module = nullptr;
	int doomImage = -1;
	int doomImageW = 0, doomImageH = 0;
	NVGcontext* ownerVg = nullptr;

	explicit ChronoDoomViewportWidget(ChronoDoomModule* module) : module(module) {
	}

	void onEnter(const EnterEvent& e) override {
		Widget::onEnter(e);
		if (module) {
			module->isFocused.store(true);
			// Claim keyboard focus from Rack
			APP->event->setSelectedWidget(this);
		}
	}

	void onLeave(const LeaveEvent& e) override {
		Widget::onLeave(e);
		if (module) {
			module->isFocused.store(false);
			if (APP->event->selectedWidget == this) {
				APP->event->setSelectedWidget(nullptr);
			}
		}
	}

	void onSelectKey(const SelectKeyEvent& e) override {
		if (!module) {
			Widget::onSelectKey(e);
			return;
		}

		// Consume game keys to prevent bubbling to Rack.
		// For Phase 1, we just log the key press or consume it.
		// If Ctrl or Alt is held, let Rack handle it.
		bool hasMod = (e.mods & (RACK_MOD_MASK));
		if (hasMod) {
			Widget::onSelectKey(e);
			return;
		}

		// List of keys to consume (WASD, Arrows, Space, Enter, Shift, Numbers 1-7, E, Esc)
		bool consumeKey = false;
		if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9) consumeKey = true;
		if (e.key == GLFW_KEY_W || e.key == GLFW_KEY_A || e.key == GLFW_KEY_S || e.key == GLFW_KEY_D) consumeKey = true;
		if (e.key == GLFW_KEY_UP || e.key == GLFW_KEY_DOWN || e.key == GLFW_KEY_LEFT || e.key == GLFW_KEY_RIGHT) consumeKey = true;
		if (e.key == GLFW_KEY_SPACE || e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_ESCAPE) consumeKey = true;
		if (e.key == GLFW_KEY_LEFT_SHIFT || e.key == GLFW_KEY_RIGHT_SHIFT || e.key == GLFW_KEY_E) consumeKey = true;

		if (consumeKey) {
			e.consume(this);
		} else {
			Widget::onSelectKey(e);
		}
	}

	void draw(const DrawArgs& args) override {
		// Draw the viewport background
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(10, 10, 12, 255));
		nvgFill(args.vg);

		if (!module) {
			return;
		}

		if (!module->hasWad) {
			// Draw uninitialized splash screen
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			
			// Glowing title
			nvgFontSize(args.vg, 20.f);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFillColor(args.vg, nvgRGBA(0, 255, 204, 255));
			nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f - 20.f, "CHRONODOOM", nullptr);

			// Instruction
			nvgFontSize(args.vg, 13.f);
			nvgFillColor(args.vg, nvgRGBA(180, 200, 220, 255));
			nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f + 15.f, "Right-click -> Load WAD...", nullptr);
			return;
		}

		// Render the active game framebuffer (in Phase 1: the animated dummy pattern)
		using namespace nvg_gfx_lifecycle;

		// Detect OpenGL context change - invalidate handle
		if (clearCacheOnContextSwitch(args.vg, ownerVg, nullptr)) {
			doomImage = -1;
		}

		// (Re)create the texture if needed
		if (doomImage < 0 || !ownedNvgImageSizeMatches(args.vg, doomImage, 320, 200)) {
			resetOwnedNvgImage(ownerVg, doomImage, doomImageW, doomImageH, args.vg, true);
			doomImage = nvgCreateImageRGBA(args.vg, 320, 200, NVG_IMAGE_NEAREST, module->dummyFramebuffer);
			ownerVg = args.vg;
		}

		// Update texture data only if the engine has ticked a new frame
		if (module->dirtyFrame.exchange(false) && doomImage >= 0) {
			nvgUpdateImage(args.vg, doomImage, module->dummyFramebuffer);
		}

		// Blit the viewport
		if (doomImage >= 0) {
			NVGpaint imgPaint = nvgImagePattern(args.vg, 0, 0, box.size.x, box.size.y, 0.0f, doomImage, 1.0f);
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
			nvgFillPaint(args.vg, imgPaint);
			nvgFill(args.vg);
		}

		// Draw focus indicator (glowing border / brackets)
		if (module->isFocused.load()) {
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 1, 1, box.size.x - 2, box.size.y - 2);
			nvgStrokeWidth(args.vg, 1.5f);
			// Pulse glow
			float pulse = 0.5f * std::sin(system::getTime() * 6.f) + 0.5f;
			nvgStrokeColor(args.vg, nvgRGBA(0, 255, 204, 150 + 105 * pulse));
			nvgStroke(args.vg);
		}
	}
};

struct ChronoDoomWidget final : ModuleWidget {
	ChronoDoomViewportWidget* viewport = nullptr;

	explicit ChronoDoomWidget(ChronoDoomModule* module) {
		setModule(module);

		// Size derived from full vertical height (380px) and 4:3 corrected aspect ratio (350px * 4/3 = 466.66px)
		// Total Width = 40 HP = 600 pixels.
		// Viewport centered: Left margin = 66.66px, Viewport Width = 466.66px, Right margin = 66.66px.
		box.size = Vec(40.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

		// 1. Screws
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// 2. Viewport (Centered vertically from Y=15 to Y=365)
		viewport = new ChronoDoomViewportWidget(module);
		viewport->box.pos = Vec(66.666f, 15.f);
		viewport->box.size = Vec(466.666f, 350.f);
		addChild(viewport);

		// 3. Port Layout: CV inputs on the left margin, outputs on the right margin
		// Center of left margin is at X = 33.333f.
		// Center of right margin is at X = box.size.x - 33.333f = 566.666f.
		
		// Inputs (spaced vertically)
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 60.f), module, ChronoDoomModule::X_MOVE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 140.f), module, ChronoDoomModule::Y_MOVE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 220.f), module, ChronoDoomModule::FIRE_GATE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 300.f), module, ChronoDoomModule::WEAPON_CV_INPUT));

		// Outputs (spaced vertically)
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(566.666f, 60.f), module, ChronoDoomModule::HEALTH_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(566.666f, 140.f), module, ChronoDoomModule::FRAG_TRIG_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(566.666f, 220.f), module, ChronoDoomModule::AUDIO_L_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(566.666f, 300.f), module, ChronoDoomModule::AUDIO_R_OUTPUT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		auto* m = dynamic_cast<ChronoDoomModule*>(module);
		if (!m) {
			return;
		}

		menu->addChild(new MenuSeparator());
		menu->addChild(createMenuLabel("ChronoDoom Settings"));
		menu->addChild(createMenuItem("Load WAD...", "", [=]() {
			osdialog_filters* filters = osdialog_filters_parse("Doom WAD:wad,WAD");
			char* pathC = osdialog_file(OSDIALOG_OPEN, nullptr, nullptr, filters);
			osdialog_filters_free(filters);
			if (pathC) {
				std::string path(pathC);
				std::free(pathC);
				if (!m->loadWad(path)) {
					osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Invalid WAD file! Header must start with IWAD or PWAD.");
				}
			}
		}));
	}

	void draw(const DrawArgs& args) override {
		// Draw Nexora Lumineth themed custom panel background (glowing cyan / dark glassmorphism)
		// Outer border
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgFillColor(args.vg, nvgRGBA(15, 12, 22, 255)); // Very dark purple/black
		nvgFill(args.vg);

		// Cyberpunk grid / accent lines
		nvgStrokeWidth(args.vg, 1.f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 255, 204, 25)); // Ultra-soft cyan accent lines
		
		// Draw horizontal lines across the margins
		for (float y = 40.f; y < box.size.y; y += 40.f) {
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, 0, y);
			nvgLineTo(args.vg, 66.666f, y);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, box.size.x - 66.666f, y);
			nvgLineTo(args.vg, box.size.x, y);
			nvgStroke(args.vg);
		}

		// Vertical divider lines separating margins from the screen viewport
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 66.666f, 0);
		nvgLineTo(args.vg, 66.666f, box.size.y);
		nvgMoveTo(args.vg, box.size.x - 66.666f, 0);
		nvgLineTo(args.vg, box.size.x - 66.666f, box.size.y);
		nvgStrokeColor(args.vg, nvgRGBA(0, 255, 204, 40));
		nvgStroke(args.vg);

		// Frame highlight / Glass shine effect
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
		nvgStrokeWidth(args.vg, 1.5f);
		nvgStrokeColor(args.vg, nvgRGBA(0, 255, 204, 60));
		nvgStroke(args.vg);

		// Port labels
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
		nvgFontSize(args.vg, 9.f);
		nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
		nvgFillColor(args.vg, nvgRGBA(180, 200, 220, 200));

		// Input labels
		nvgText(args.vg, 33.333f, 75.f, "X-MOVE", nullptr);
		nvgText(args.vg, 33.333f, 155.f, "Y-MOVE", nullptr);
		nvgText(args.vg, 33.333f, 235.f, "FIRE", nullptr);
		nvgText(args.vg, 33.333f, 315.f, "WEAPON", nullptr);

		// Output labels
		nvgText(args.vg, 566.666f, 75.f, "HEALTH", nullptr);
		nvgText(args.vg, 566.666f, 155.f, "FRAG", nullptr);
		nvgText(args.vg, 566.666f, 235.f, "AUDIO L", nullptr);
		nvgText(args.vg, 566.666f, 315.f, "AUDIO R", nullptr);

		ModuleWidget::draw(args);
	}
};

Model* modelChronoDoom = createModel<ChronoDoomModule, ChronoDoomWidget>("ChronoDoom");
