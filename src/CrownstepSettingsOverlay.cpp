#include "CrownstepSettingsOverlay.hpp"

#include <cmath>
#include <cstring>

namespace {

constexpr float OUTER = 8.f;
constexpr float HEADER_H = 27.f;
constexpr float TAB_Y = 39.f;
constexpr float TAB_H = 25.f;
constexpr float CONTENT_Y = 76.f;
constexpr float ROW_H = 35.f;
constexpr float ROW_GAP = 4.f;

bool pointIn(const Vec& p, float x, float y, float w, float h) {
	return p.x >= x && p.y >= y && p.x < x + w && p.y < y + h;
}

int wrapIndex(int value, int count) {
	if (count <= 0) {
		return 0;
	}
	return (value % count + count) % count;
}

void setFont(const widget::Widget::DrawArgs& args, float size, NVGcolor color, int align) {
	if (APP && APP->window && APP->window->uiFont) {
		nvgFontFaceId(args.vg, APP->window->uiFont->handle);
	}
	nvgFontSize(args.vg, size);
	nvgFillColor(args.vg, color);
	nvgTextAlign(args.vg, align);
}

void fillRounded(NVGcontext* vg, float x, float y, float w, float h, float r, NVGcolor color) {
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x, y, w, h, r);
	nvgFillColor(vg, color);
	nvgFill(vg);
}

void strokeRounded(NVGcontext* vg, float x, float y, float w, float h, float r, NVGcolor color, float width = 1.f) {
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x, y, w, h, r);
	nvgStrokeColor(vg, color);
	nvgStrokeWidth(vg, width);
	nvgStroke(vg);
}

void pushParamHistory(Crownstep* module, int paramId, float oldValue, float newValue) {
	if (!module || oldValue == newValue || module->id < 0 || !APP || !APP->history) {
		return;
	}
	auto* action = new history::ParamChange();
	action->moduleId = module->id;
	action->paramId = paramId;
	action->oldValue = oldValue;
	action->newValue = newValue;
	APP->history->push(action);
}

void setParamValue(Crownstep* module, int paramId, float value, bool addHistory = true) {
	if (!module || paramId < 0 || paramId >= Crownstep::PARAMS_LEN) {
		return;
	}
	ParamQuantity* quantity = module->paramQuantities[size_t(paramId)];
	if (quantity) {
		value = clamp(value, quantity->getMinValue(), quantity->getMaxValue());
	}
	const float oldValue = module->params[paramId].getValue();
	if (APP && APP->engine) {
		APP->engine->setParamValue(module, paramId, value);
	}
	else {
		module->params[paramId].setValue(value);
	}
	if (addHistory) {
		pushParamHistory(module, paramId, oldValue, value);
	}
	module->refreshHeldPitchForCurrentStep();
}

float rowY(int index) {
	return CONTENT_Y + float(index) * (ROW_H + ROW_GAP);
}

} // namespace

void CrownstepSettingsOpenButton::draw(const DrawArgs& args) {
	const float alpha = enabled ? 1.f : 0.38f;
	fillRounded(args.vg, 0.f, 0.f, box.size.x, box.size.y, 4.f, nvgRGBA(14, 20, 28, int(225.f * alpha)));
	strokeRounded(args.vg, 0.5f, 0.5f, box.size.x - 1.f, box.size.y - 1.f, 4.f,
		nvgRGBA(96, 226, 244, int(210.f * alpha)), 1.1f);
	const float cx = box.size.x * 0.5f;
	const float cy = box.size.y * 0.5f;
	nvgStrokeColor(args.vg, nvgRGBA(220, 248, 255, int(240.f * alpha)));
	nvgStrokeWidth(args.vg, 1.35f);
	for (int i = -1; i <= 1; ++i) {
		const float y = cy + float(i) * 4.5f;
		nvgBeginPath(args.vg);
		nvgMoveTo(args.vg, cx - 7.f, y);
		nvgLineTo(args.vg, cx + 7.f, y);
		nvgStroke(args.vg);
		const float knobX = cx + float(i) * 3.2f;
		nvgBeginPath(args.vg);
		nvgCircle(args.vg, knobX, y, 1.8f);
		nvgFillColor(args.vg, nvgRGBA(194, 142, 255, int(255.f * alpha)));
		nvgFill(args.vg);
	}
	OpaqueWidget::draw(args);
}

void CrownstepSettingsOpenButton::onButton(const event::Button& e) {
	if (enabled && e.button == GLFW_MOUSE_BUTTON_LEFT && e.action == GLFW_PRESS) {
		if (openAction) {
			openAction();
		}
		e.consume(this);
		return;
	}
	OpaqueWidget::onButton(e);
}

CrownstepSettingsOverlay::CrownstepSettingsOverlay(Crownstep* module) : module(module) {
	visible = false;
	syncPendingGameSetup();
}

void CrownstepSettingsOverlay::open() {
	if (!module) {
		return;
	}
	confirmationOpen = false;
	rangeDragging = false;
	syncPendingGameSetup();
	visible = true;
	if (APP && APP->event) {
		APP->event->setSelectedWidget(this);
	}
}

void CrownstepSettingsOverlay::close() {
	confirmationOpen = false;
	rangeDragging = false;
	syncPendingGameSetup();
	if (closeAction) {
		closeAction();
	}
	else {
		visible = false;
	}
}

void CrownstepSettingsOverlay::syncPendingGameSetup() {
	if (!module) {
		pendingGameMode = Crownstep::GAME_MODE_CHECKERS;
		pendingPlayerMode = Crownstep::PLAYER_INIT;
		return;
	}
	pendingGameMode = clamp(module->gameMode, 0, Crownstep::GAME_MODE_COUNT - 1);
	pendingPlayerMode = clamp(module->playerMode, 0, Crownstep::PLAYER_MODE_COUNT - 1);
}

bool CrownstepSettingsOverlay::hasPendingGameSetup() const {
	return module && (pendingGameMode != module->gameMode || pendingPlayerMode != module->playerMode);
}

void CrownstepSettingsOverlay::requestNewGame() {
	if (module) {
		confirmationOpen = true;
	}
}

void CrownstepSettingsOverlay::confirmNewGame() {
	if (!module) {
		return;
	}
	module->applyGameSetupAndStartNewGame(pendingGameMode, pendingPlayerMode);
	syncPendingGameSetup();
	confirmationOpen = false;
	close();
}

void CrownstepSettingsOverlay::cancelConfirmation() {
	confirmationOpen = false;
}

void CrownstepSettingsOverlay::draw(const DrawArgs& args) {
	const float w = box.size.x;
	const float h = box.size.y;
	fillRounded(args.vg, 1.f, 1.f, w - 2.f, h - 2.f, 6.f, nvgRGBA(5, 8, 14, 248));
	strokeRounded(args.vg, 1.5f, 1.5f, w - 3.f, h - 3.f, 6.f, nvgRGBA(93, 218, 241, 190), 1.3f);

	setFont(args, 13.f, nvgRGBA(232, 247, 255, 245), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	nvgText(args.vg, OUTER + 3.f, OUTER + HEADER_H * 0.5f, "CROWNSTEP SETTINGS", nullptr);
	fillRounded(args.vg, w - OUTER - 23.f, OUTER, 23.f, 22.f, 3.f, nvgRGBA(34, 30, 49, 235));
	setFont(args, 14.f, nvgRGBA(232, 226, 246, 235), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	nvgText(args.vg, w - OUTER - 11.5f, OUTER + 11.f, "×", nullptr);

	static constexpr const char* TAB_NAMES[TAB_COUNT] = {"GAME", "PITCH", "MAP", "LOOK"};
	const float tabW = (w - 2.f * OUTER - 6.f) / float(TAB_COUNT);
	for (int i = 0; i < TAB_COUNT; ++i) {
		const float x = OUTER + float(i) * (tabW + 2.f);
		const bool active = activeTab == Tab(i);
		fillRounded(args.vg, x, TAB_Y, tabW, TAB_H, 3.f,
			active ? nvgRGBA(50, 53, 82, 245) : nvgRGBA(18, 23, 34, 230));
		if (active) {
			strokeRounded(args.vg, x + 0.5f, TAB_Y + 0.5f, tabW - 1.f, TAB_H - 1.f, 3.f,
				nvgRGBA(157, 112, 244, 210));
		}
		setFont(args, 10.f, active ? nvgRGBA(238, 244, 255, 250) : nvgRGBA(145, 157, 174, 210),
			NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, x + tabW * 0.5f, TAB_Y + TAB_H * 0.5f, TAB_NAMES[i], nullptr);
	}

	auto drawRow = [&](int index, const char* label, const char* value, bool enabled = true, bool toggle = false) {
		const float y = rowY(index);
		const float x = OUTER;
		const float rw = w - 2.f * OUTER;
		fillRounded(args.vg, x, y, rw, ROW_H, 4.f, nvgRGBA(14, 20, 30, enabled ? 238 : 175));
		setFont(args, 10.f, enabled ? nvgRGBA(183, 198, 216, 235) : nvgRGBA(103, 111, 124, 170),
			NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, x + 8.f, y + ROW_H * 0.5f, label, nullptr);
		const float pillW = std::min(126.f, rw * 0.54f);
		const float pillX = x + rw - pillW - 5.f;
		fillRounded(args.vg, pillX, y + 5.f, pillW, ROW_H - 10.f, 4.f,
			enabled ? nvgRGBA(32, 36, 54, 245) : nvgRGBA(25, 27, 34, 190));
		const size_t valueLength = std::strlen(value);
		const float valueFontSize = valueLength > 22 ? 7.2f : (valueLength > 18 ? 8.1f : 9.4f);
		setFont(args, valueFontSize,
			enabled ? nvgRGBA(229, 238, 250, 245) : nvgRGBA(112, 118, 128, 170), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, pillX + pillW * 0.5f, y + ROW_H * 0.5f, value, nullptr);
		if (!toggle && enabled) {
			setFont(args, 13.f, nvgRGBA(112, 225, 244, 235), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgText(args.vg, pillX + 9.f, y + ROW_H * 0.5f, "‹", nullptr);
			nvgText(args.vg, pillX + pillW - 9.f, y + ROW_H * 0.5f, "›", nullptr);
		}
	};

	if (module) {
		switch (activeTab) {
		case TAB_GAME:
			drawRow(0, "GAME MODE", GAME_MODE_NAMES[size_t(pendingGameMode)]);
			drawRow(1, "PLAYER ROLE", PLAYER_MODE_NAMES[size_t(pendingPlayerMode)]);
			drawRow(2, "AI DIFFICULTY", DIFFICULTY_NAMES[size_t(clamp(module->aiDifficulty, 0, int(DIFFICULTY_NAMES.size()) - 1))]);
			break;
		case TAB_PITCH:
			drawRow(0, "QUANTIZE", module->quantizationEnabled ? "ON" : "OFF", true, true);
			drawRow(1, "KEY", KEY_NAMES[size_t(clamp(int(std::round(module->params[Crownstep::ROOT_PARAM].getValue())), 0, 11))]);
			drawRow(2, "SCALE", SCALES[size_t(clamp(int(std::round(module->params[Crownstep::SCALE_PARAM].getValue())), 0, int(SCALES.size()) - 1))].name);
			{
				const float y = rowY(3);
				const float value = module->params[Crownstep::RANGE_PARAM].getValue();
				const float semitones = crownstep::pitchRangeSemitoneSpan(value, module->boardCellCount());
				fillRounded(args.vg, OUTER, y, w - 2.f * OUTER, ROW_H, 4.f, nvgRGBA(14, 20, 30, 238));
				setFont(args, 10.f, nvgRGBA(183, 198, 216, 235), NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, OUTER + 8.f, y + 11.f, "RANGE", nullptr);
				const float sx = OUTER + 70.f;
				const float sw = w - OUTER - 58.f - sx;
				fillRounded(args.vg, sx, y + 22.f, sw, 3.f, 1.5f, nvgRGBA(42, 53, 68, 255));
				fillRounded(args.vg, sx, y + 22.f, sw * clamp(value, 0.f, 1.f), 3.f, 1.5f, nvgRGBA(100, 218, 240, 245));
				char text[24];
				std::snprintf(text, sizeof(text), "%.1f ST", semitones);
				setFont(args, 9.5f, nvgRGBA(231, 239, 250, 245), NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
				nvgText(args.vg, w - OUTER - 8.f, y + 11.f, text, nullptr);
			}
			drawRow(4, "BIPOLAR", module->pitchBipolarEnabled ? "ON" : "OFF", true, true);
			drawRow(5, "SMOOTH MELODY", module->melodicBiasEnabled ? "ON" : "OFF", true, true);
			break;
		case TAB_MAP:
			drawRow(0, "PITCH SOURCE", PITCH_INTERPRETATION_NAMES[size_t(clamp(module->pitchInterpretationMode, 0, int(PITCH_INTERPRETATION_NAMES.size()) - 1))]);
			drawRow(1, "BOARD LAYOUT", BOARD_VALUE_LAYOUT_NAMES[size_t(clamp(module->boardValueLayoutMode, 0, int(BOARD_VALUE_LAYOUT_NAMES.size()) - 1))]);
			drawRow(2, "INVERTED", module->boardValueLayoutInverted ? "ON" : "OFF", true, true);
			drawRow(3, "SHOW PITCH VALUES", module->showCellPitchOverlay ? "ON" : "OFF", true, true);
			if (module->boardValueLayoutMode == crownstep::BOARD_VALUE_LAYOUT_RANDOM) {
				drawRow(4, "RANDOM MAP", "RESHUFFLE", true, true);
			}
			break;
		case TAB_LOOK: {
			drawRow(0, "HIGHLIGHT", HIGHLIGHT_MODE_NAMES[size_t(clamp(module->highlightMode, 0, Crownstep::HIGHLIGHT_COUNT - 1))]);
			const bool fixed = module->isOthelloMode() || pendingGameMode == Crownstep::GAME_MODE_OTHELLO;
			drawRow(1, "BOARD TEXTURE", fixed ? "FIXED WHEN APPLIED" : BOARD_TEXTURE_NAMES[size_t(clamp(module->boardTextureMode, 0, Crownstep::BOARD_TEXTURE_COUNT - 1))], !fixed);
			break;
		}
		default:
			break;
		}
	}

	if (activeTab == TAB_GAME && module) {
		const bool pending = hasPendingGameSetup();
		const float buttonY = h - 61.f;
		fillRounded(args.vg, OUTER + 28.f, buttonY, w - 2.f * (OUTER + 28.f), 34.f, 5.f,
			pending ? nvgRGBA(105, 65, 151, 245) : nvgRGBA(28, 104, 124, 245));
		setFont(args, 10.5f, nvgRGBA(245, 247, 255, 250), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, w * 0.5f, buttonY + 17.f, pending ? "APPLY & NEW GAME" : "NEW GAME", nullptr);
		if (pending) {
			setFont(args, 8.5f, nvgRGBA(226, 169, 255, 235), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			nvgText(args.vg, w * 0.5f, buttonY - 10.f, "NEW GAME REQUIRED", nullptr);
		}
	}

	if (confirmationOpen) {
		fillRounded(args.vg, 5.f, 5.f, w - 10.f, h - 10.f, 6.f, nvgRGBA(3, 5, 9, 250));
		const float dw = w - 36.f;
		const float dx = 18.f;
		const float dy = h * 0.25f;
		const float dh = 190.f;
		fillRounded(args.vg, dx, dy, dw, dh, 6.f, nvgRGBA(18, 22, 34, 255));
		strokeRounded(args.vg, dx + 0.5f, dy + 0.5f, dw - 1.f, dh - 1.f, 6.f, nvgRGBA(161, 112, 229, 220));
		setFont(args, 14.f, nvgRGBA(242, 246, 255, 250), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, w * 0.5f, dy + 30.f, "START A NEW GAME?", nullptr);
		setFont(args, 10.f, nvgRGBA(184, 197, 214, 240), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, w * 0.5f, dy + 62.f, "The current board and move history", nullptr);
		nvgText(args.vg, w * 0.5f, dy + 77.f, "will be cleared. Settings are kept.", nullptr);
		if (hasPendingGameSetup()) {
			setFont(args, 9.f, nvgRGBA(218, 177, 250, 235), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
			if (pendingGameMode != module->gameMode) {
				char change[64];
				std::snprintf(change, sizeof(change), "%s  →  %s", GAME_MODE_NAMES[size_t(module->gameMode)], GAME_MODE_NAMES[size_t(pendingGameMode)]);
				nvgText(args.vg, w * 0.5f, dy + 102.f, change, nullptr);
			}
			if (pendingPlayerMode != module->playerMode) {
				char change[64];
				std::snprintf(change, sizeof(change), "%s  →  %s", PLAYER_MODE_NAMES[size_t(module->playerMode)], PLAYER_MODE_NAMES[size_t(pendingPlayerMode)]);
				nvgText(args.vg, w * 0.5f, dy + 117.f, change, nullptr);
			}
		}
		fillRounded(args.vg, dx + 12.f, dy + dh - 45.f, 76.f, 29.f, 4.f, nvgRGBA(40, 44, 55, 255));
		fillRounded(args.vg, dx + dw - 126.f, dy + dh - 45.f, 114.f, 29.f, 4.f, nvgRGBA(76, 91, 151, 255));
		setFont(args, 9.5f, nvgRGBA(237, 242, 252, 250), NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
		nvgText(args.vg, dx + 50.f, dy + dh - 30.5f, "CANCEL", nullptr);
		nvgText(args.vg, dx + dw - 69.f, dy + dh - 30.5f, "START NEW GAME", nullptr);
	}

	OpaqueWidget::draw(args);
}

void CrownstepSettingsOverlay::onButton(const event::Button& e) {
	if (e.action != GLFW_PRESS) {
		OpaqueWidget::onButton(e);
		return;
	}
	if (e.button != GLFW_MOUSE_BUTTON_LEFT) {
		e.consume(this);
		return;
	}
	const Vec p = e.pos;
	const float w = box.size.x;
	const float h = box.size.y;
	if (confirmationOpen) {
		const float dw = w - 36.f;
		const float dx = 18.f;
		const float dy = h * 0.25f;
		const float dh = 190.f;
		if (pointIn(p, dx + 12.f, dy + dh - 45.f, 76.f, 29.f)) {
			cancelConfirmation();
		}
		else if (pointIn(p, dx + dw - 126.f, dy + dh - 45.f, 114.f, 29.f)) {
			confirmNewGame();
		}
		e.consume(this);
		return;
	}
	if (pointIn(p, w - OUTER - 23.f, OUTER, 23.f, 22.f)) {
		close();
		e.consume(this);
		return;
	}
	const float tabW = (w - 2.f * OUTER - 6.f) / float(TAB_COUNT);
	for (int i = 0; i < TAB_COUNT; ++i) {
		const float x = OUTER + float(i) * (tabW + 2.f);
		if (pointIn(p, x, TAB_Y, tabW, TAB_H)) {
			activeTab = Tab(i);
			e.consume(this);
			return;
		}
	}
	if (!module) {
		e.consume(this);
		return;
	}
	int row = -1;
	for (int i = 0; i < 7; ++i) {
		if (pointIn(p, OUTER, rowY(i), w - 2.f * OUTER, ROW_H)) {
			row = i;
			break;
		}
	}
	const bool previous = p.x < w - OUTER - 63.f;
	auto cycle = [&](int value, int count) {
		return wrapIndex(value + (previous ? -1 : 1), count);
	};
	if (activeTab == TAB_GAME) {
		if (row == 0) pendingGameMode = cycle(pendingGameMode, Crownstep::GAME_MODE_COUNT);
		else if (row == 1) pendingPlayerMode = cycle(pendingPlayerMode, Crownstep::PLAYER_MODE_COUNT);
		else if (row == 2) module->aiDifficulty = cycle(module->aiDifficulty, int(DIFFICULTY_NAMES.size()));
		else if (pointIn(p, OUTER + 28.f, h - 61.f, w - 2.f * (OUTER + 28.f), 34.f)) requestNewGame();
	}
	else if (activeTab == TAB_PITCH) {
		if (row == 0) { module->quantizationEnabled = !module->quantizationEnabled; module->refreshHeldPitchForCurrentStep(); }
		else if (row == 1) setParamValue(module, Crownstep::ROOT_PARAM, float(cycle(int(std::round(module->params[Crownstep::ROOT_PARAM].getValue())), 12)));
		else if (row == 2) setParamValue(module, Crownstep::SCALE_PARAM, float(cycle(int(std::round(module->params[Crownstep::SCALE_PARAM].getValue())), int(SCALES.size()))));
		else if (row == 3) {
			rangeDragging = true;
			rangeDragOldValue = module->params[Crownstep::RANGE_PARAM].getValue();
			const float sx = OUTER + 70.f;
			const float sw = w - OUTER - 58.f - sx;
			rangeDragValue = clamp((p.x - sx) / sw, 0.f, 1.f);
			setParamValue(module, Crownstep::RANGE_PARAM, rangeDragValue, false);
		}
		else if (row == 4) { module->pitchBipolarEnabled = !module->pitchBipolarEnabled; module->refreshHeldPitchForCurrentStep(); }
		else if (row == 5) { module->melodicBiasEnabled = !module->melodicBiasEnabled; module->refreshHeldPitchForCurrentStep(); }
	}
	else if (activeTab == TAB_MAP) {
		if (row == 0) { module->pitchInterpretationMode = cycle(module->pitchInterpretationMode, int(PITCH_INTERPRETATION_NAMES.size())); module->refreshHeldPitchForCurrentStep(); }
		else if (row == 1) { module->boardValueLayoutMode = cycle(module->boardValueLayoutMode, int(BOARD_VALUE_LAYOUT_NAMES.size())); module->refreshHeldPitchForCurrentStep(); }
		else if (row == 2) { module->boardValueLayoutInverted = !module->boardValueLayoutInverted; module->refreshHeldPitchForCurrentStep(); }
		else if (row == 3) module->showCellPitchOverlay = !module->showCellPitchOverlay;
		else if (row == 4 && module->boardValueLayoutMode == crownstep::BOARD_VALUE_LAYOUT_RANDOM) module->randomizeBoardValueLayout();
	}
	else if (activeTab == TAB_LOOK) {
		if (row == 0) module->highlightMode = cycle(module->highlightMode, Crownstep::HIGHLIGHT_COUNT);
		else if (row == 1 && !module->isOthelloMode() && pendingGameMode != Crownstep::GAME_MODE_OTHELLO) module->boardTextureMode = cycle(module->boardTextureMode, Crownstep::BOARD_TEXTURE_COUNT);
	}
	e.consume(this);
}

void CrownstepSettingsOverlay::onDragMove(const event::DragMove& e) {
	if (rangeDragging && module) {
		const float w = box.size.x;
		const float sw = w - OUTER - 58.f - (OUTER + 70.f);
		rangeDragValue = clamp(rangeDragValue + e.mouseDelta.x / std::max(1.f, sw), 0.f, 1.f);
		setParamValue(module, Crownstep::RANGE_PARAM, rangeDragValue, false);
		e.consume(this);
		return;
	}
	OpaqueWidget::onDragMove(e);
}

void CrownstepSettingsOverlay::onDragEnd(const event::DragEnd& e) {
	if (rangeDragging && module) {
		pushParamHistory(module, Crownstep::RANGE_PARAM, rangeDragOldValue, module->params[Crownstep::RANGE_PARAM].getValue());
		rangeDragging = false;
		e.consume(this);
		return;
	}
	OpaqueWidget::onDragEnd(e);
}

void CrownstepSettingsOverlay::onHoverScroll(const event::HoverScroll& e) {
	// Intentionally consume wheel input without cycling. This avoids accidental
	// changes in the dense overlay and prevents click-through to panel controls.
	e.consume(this);
}

void CrownstepSettingsOverlay::onSelectKey(const event::SelectKey& e) {
	if (e.action == GLFW_PRESS && e.key == GLFW_KEY_ESCAPE) {
		if (confirmationOpen) {
			cancelConfirmation();
		}
		else {
			close();
		}
		e.consume(this);
	}
}

void CrownstepSettingsOverlay::onHoverKey(const event::HoverKey& e) {
	if (e.action == GLFW_PRESS && e.key == GLFW_KEY_ESCAPE) {
		if (confirmationOpen) {
			cancelConfirmation();
		}
		else {
			close();
		}
		e.consume(this);
		return;
	}
	OpaqueWidget::onHoverKey(e);
}
