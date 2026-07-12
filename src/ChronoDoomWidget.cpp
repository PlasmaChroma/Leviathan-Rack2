#include "ChronoDoom.hpp"
#include "NvgGraphicsLifecycle.hpp"
#include "visual/VisualAssets.hpp"
#include <osdialog.h>

extern "C" {
#include "doom/d_event.h"
#include "doom/doomkeys.h"
#include "doom/m_controls.h"
	extern volatile int doom_engine_status;
	extern char doom_engine_error[256];
}

static int mapGlfwToDoomKey(int glfwKey) {
	switch (glfwKey) {
		case GLFW_KEY_RIGHT: return KEY_RIGHTARROW;
		case GLFW_KEY_LEFT: return KEY_LEFTARROW;
		case GLFW_KEY_UP: return KEY_UPARROW;
		case GLFW_KEY_DOWN: return KEY_DOWNARROW;
		case GLFW_KEY_ESCAPE: return KEY_ESCAPE;
		case GLFW_KEY_ENTER: return KEY_ENTER;
		case GLFW_KEY_TAB: return KEY_TAB;
		case GLFW_KEY_BACKSPACE: return KEY_BACKSPACE;
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT: return KEY_RSHIFT;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL: return KEY_RCTRL;
		case GLFW_KEY_LEFT_ALT:
		case GLFW_KEY_RIGHT_ALT: return KEY_RALT;
		case GLFW_KEY_SPACE: return ' ';
		case GLFW_KEY_W: return key_up;
		case GLFW_KEY_S: return key_down;
		case GLFW_KEY_A: return key_strafeleft;
		case GLFW_KEY_D: return key_straferight;
		case GLFW_KEY_E: return key_use;
		default:
			if (glfwKey >= GLFW_KEY_A && glfwKey <= GLFW_KEY_Z) {
				return (glfwKey - GLFW_KEY_A) + 'a';
			}
			if (glfwKey >= GLFW_KEY_0 && glfwKey <= GLFW_KEY_9) {
				return (glfwKey - GLFW_KEY_0) + '0';
			}
			return 0;
	}
}

struct ChronoDoomViewportWidget final : Widget {
	ChronoDoomModule* module = nullptr;
	int doomImage = -1;
	int doomImageW = 0, doomImageH = 0;
	NVGcontext* ownerVg = nullptr;
	int mouseButtons = 0;
	double mouseAccumX = 0.0;
	bool lookDragging = false;
	double captureHintUntil = 0.0;
	explicit ChronoDoomViewportWidget(ChronoDoomModule* module) : module(module) {
	}

	void postKey(evtype_t type, int doomKey) {
		event_t ev{};
		ev.type = type;
		ev.data1 = doomKey;
		D_PostEvent(&ev);
	}

	void postMouse(int dx, int dy) {
		event_t ev{};
		ev.type = ev_mouse;
		ev.data1 = mouseButtons;
		ev.data2 = dx;
		ev.data3 = dy;
		D_PostEvent(&ev);
	}

	void releaseInputState() {
		// Rack will receive subsequent physical key releases, so release every
		// game key that might currently be held to avoid latched movement/fire.
		static const int keys[] = {
			KEY_RIGHTARROW, KEY_LEFTARROW, KEY_UPARROW, KEY_DOWNARROW,
			KEY_ENTER, KEY_TAB, KEY_BACKSPACE, KEY_RSHIFT, KEY_RCTRL,
			KEY_RALT, ' ', 'w', 'a', 's', 'd', 'e',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
		};
		for (int key : keys) {
			postKey(ev_keyup, key);
		}
		postKey(ev_keyup, key_strafeleft);
		postKey(ev_keyup, key_straferight);
		mouseButtons = 0;
		lookDragging = false;
		postMouse(0, 0);
		APP->window->cursorUnlock();
		if (APP->event->draggedWidget == this) {
			APP->event->setDraggedWidget(nullptr, 0);
		}
		mouseAccumX = 0.0;
		captureHintUntil = 0.0;

		if (module) {
			module->isFocused.store(false);
		}
	}

	void releaseCapture() {
		releaseInputState();
		if (APP->event->selectedWidget == this) {
			APP->event->setSelectedWidget(nullptr);
		}
	}

	void onButton(const ButtonEvent& e) override {
		if (module && module->isEngineOwner() && module->hasWad
				&& e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS
				&& !module->isFocused.load()) {
			module->isFocused.store(true);
			APP->event->setSelectedWidget(this);
			APP->window->cursorLock();
			APP->event->setDraggedWidget(this, 99); // Force dragging mode on dummy button 99 to receive smooth mouseDelta in onDragMove
			captureHintUntil = system::getTime() + 6.0;
			e.consume(this);
			return;
		}

		if (module && module->isFocused.load()) {
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void onHover(const HoverEvent& e) override {
		if (module && module->isFocused.load()) {
			e.consume(this);
			return;
		}
		Widget::onHover(e);
	}

	void onDragMove(const DragMoveEvent& e) override {
		if (module && module->isFocused.load()) {
			mouseAccumX += e.mouseDelta.x * 4.0;
			const int dx = (int) mouseAccumX;
			mouseAccumX -= dx;
			if (dx != 0) {
				postMouse(dx, 0);
			}
			e.consume(this);
			return;
		}
		Widget::onDragMove(e);
	}

	void onDragEnd(const DragEndEvent& e) override {
		if (module && module->isFocused.load()) {
			e.consume(this);
			return;
		}
		Widget::onDragEnd(e);
	}

	void step() override {
		Widget::step();
		if (module && module->isFocused.load()) {
			if (glfwGetWindowAttrib(APP->window->win, GLFW_FOCUSED) == GLFW_FALSE) {
				releaseCapture();
				return;
			}
			if (APP->event->selectedWidget != this) {
				APP->event->setSelectedWidget(this);
			}
			if (!APP->window->isCursorLocked()) {
				APP->window->cursorLock();
			}
			if (APP->event->draggedWidget != this) {
				APP->event->setDraggedWidget(this, 99);
			}

			// Poll mouse buttons directly from GLFW since VCV Rack's event system
			// ignores physical button presses/releases when a drag is active.
			GLFWwindow* win = APP->window->win;
			int mask = 0;
			if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) mask |= 1;
			if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) mask |= 2;
			if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) mask |= 4;

			if (mask != mouseButtons) {
				mouseButtons = mask;
				postMouse(0, 0);
			}
		}
	}

	void onRemove(const RemoveEvent& e) override {
		if (module && module->isFocused.load()) {
			releaseInputState();
		}
		Widget::onRemove(e);
	}

	void onSelectKey(const SelectKeyEvent& e) override {
		if (!module) {
			Widget::onSelectKey(e);
			return;
		}

		if (!module->isEngineOwner() || !module->hasWad) {
			Widget::onSelectKey(e);
			return;
		}

		// 0 is reserved as the explicit path back to Rack while captured.
		if (e.key == GLFW_KEY_0) {
			if (e.action == GLFW_PRESS) {
				releaseCapture();
			}
			e.consume(this);
			return;
		}

		// List of keys to consume (WASD, Arrows, Space, Enter, Shift, Numbers 1-7, E, Esc)
		bool consumeKey = false;
		if (e.key >= GLFW_KEY_0 && e.key <= GLFW_KEY_9) consumeKey = true;
		if (e.key == GLFW_KEY_W || e.key == GLFW_KEY_A || e.key == GLFW_KEY_S || e.key == GLFW_KEY_D) consumeKey = true;
		if (e.key == GLFW_KEY_UP || e.key == GLFW_KEY_DOWN || e.key == GLFW_KEY_LEFT || e.key == GLFW_KEY_RIGHT) consumeKey = true;
		if (e.key == GLFW_KEY_SPACE || e.key == GLFW_KEY_ENTER || e.key == GLFW_KEY_ESCAPE) consumeKey = true;
		if (e.key == GLFW_KEY_LEFT_SHIFT || e.key == GLFW_KEY_RIGHT_SHIFT || e.key == GLFW_KEY_E) consumeKey = true;
		if (e.key == GLFW_KEY_LEFT_CONTROL || e.key == GLFW_KEY_RIGHT_CONTROL) consumeKey = true;

		if (consumeKey) {
			int doomKey = mapGlfwToDoomKey(e.key);
			if (doomKey != 0) {
				if (e.action == GLFW_PRESS) {
					postKey(ev_keydown, doomKey);
				} else if (e.action == GLFW_RELEASE) {
					postKey(ev_keyup, doomKey);
				}
			}
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

		if (!module->isEngineOwner()) {
			// Draw secondary instance warning splash screen
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			
			// Glowing title
			nvgFontSize(args.vg, 20.f);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFillColor(args.vg, nvgRGBA(255, 100, 100, 255));
			nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f - 20.f, "CHRONODOOM", nullptr);

			// Instruction
			nvgFontSize(args.vg, 13.f);
			nvgFillColor(args.vg, nvgRGBA(180, 200, 220, 255));
			nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f + 15.f, "Engine running in another module.", nullptr);
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

		if (doom_engine_status < 0) {
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 16.f);
			nvgFillColor(args.vg, nvgRGBA(255, 100, 100, 255));
			nvgText(args.vg, box.size.x / 2.f, box.size.y / 2.f - 18.f, "DOOM STARTUP FAILED", nullptr);
			nvgFontSize(args.vg, 11.f);
			nvgFillColor(args.vg, nvgRGBA(220, 220, 220, 255));
			nvgTextBox(args.vg, 20.f, box.size.y / 2.f + 2.f, box.size.x - 40.f, doom_engine_error, nullptr);
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

		if (captureHintUntil > system::getTime()) {
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.f, 0.f, box.size.x, 20.f);
			nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 170));
			nvgFill(args.vg);

			nvgFontFaceId(args.vg, APP->window->uiFont->handle);
			nvgFontSize(args.vg, 12.f);
			nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
			nvgFillColor(args.vg, nvgRGBA(0, 255, 204, 230));
			nvgText(args.vg, box.size.x / 2.f, 4.f,
				"MOVE MOUSE TO TURN  -  PRESS 0 TO RELEASE", nullptr);
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

		// 42 HP gives each jack column one more HP of clearance from the
		// full-height 4:3 viewport.
		box.size = Vec(42.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

		// 1. Screws
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<CyanOrbScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<CyanOrbScrew>(Vec(box.size.x - 2.f * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// 2. Full-height 4:3 viewport, centered between the side jack columns.
		viewport = new ChronoDoomViewportWidget(module);
		viewport->box.pos = Vec(61.666f, 0.f);
		viewport->box.size = Vec(506.666f, RACK_GRID_HEIGHT);
		addChild(viewport);

		// 3. Port Layout: CV inputs on the left margin, outputs on the right margin
		// Center of left margin is at X = 33.333f.
		// Center of right margin is at X = box.size.x - 33.333f.
		
		// Inputs (spaced vertically)
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 60.f), module, ChronoDoomModule::X_MOVE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 140.f), module, ChronoDoomModule::Y_MOVE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 220.f), module, ChronoDoomModule::FIRE_GATE_INPUT));
		addInput(createInputCentered<Magitek2InputJack>(Vec(33.333f, 300.f), module, ChronoDoomModule::WEAPON_CV_INPUT));

		// Outputs (spaced vertically)
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 40.f), module, ChronoDoomModule::HEALTH_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 100.f), module, ChronoDoomModule::FRAG_TRIG_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 160.f), module, ChronoDoomModule::AUDIO_L_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 220.f), module, ChronoDoomModule::AUDIO_R_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 280.f), module, ChronoDoomModule::MIDI_PITCH_OUTPUT));
		addOutput(createOutputCentered<Magitek2OutputJack>(Vec(box.size.x - 33.333f, 340.f), module, ChronoDoomModule::MIDI_GATE_OUTPUT));
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
			nvgLineTo(args.vg, 61.666f, y);
			nvgStroke(args.vg);

			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, box.size.x - 61.666f, y);
			nvgLineTo(args.vg, box.size.x, y);
			nvgStroke(args.vg);
		}

		// Vertical divider lines separating margins from the screen viewport
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, 61.666f, 0);
		nvgLineTo(args.vg, 61.666f, box.size.y);
		nvgMoveTo(args.vg, box.size.x - 61.666f, 0);
		nvgLineTo(args.vg, box.size.x - 61.666f, box.size.y);
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
		nvgText(args.vg, box.size.x - 33.333f, 55.f, "HEALTH", nullptr);
		nvgText(args.vg, box.size.x - 33.333f, 115.f, "FRAG", nullptr);
		nvgText(args.vg, box.size.x - 33.333f, 175.f, "AUDIO L", nullptr);
		nvgText(args.vg, box.size.x - 33.333f, 235.f, "AUDIO R", nullptr);
		nvgText(args.vg, box.size.x - 33.333f, 295.f, "PITCH", nullptr);
		nvgText(args.vg, box.size.x - 33.333f, 355.f, "GATE", nullptr);

		ModuleWidget::draw(args);
	}
};

Model* modelChronoDoom = createModel<ChronoDoomModule, ChronoDoomWidget>("ChronoDoom");
