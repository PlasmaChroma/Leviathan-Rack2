#include "plugin.hpp"
#include "CrownstepCore.hpp"

#include <exception>
#include <string>

using crownstep::AI_SIDE;
using crownstep::BOARD_SIZE;
using crownstep::DIFFICULTY_NAMES;
using crownstep::HUMAN_SIDE;
using crownstep::KEY_NAMES;
using crownstep::Move;
using crownstep::SCALES;
using crownstep::SEQ_CAP_NAMES;
using crownstep::SEQ_CAPS;
using crownstep::Step;

struct CrownstepSeqLengthQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, int(SEQ_CAP_NAMES.size()) - 1);
		return SEQ_CAP_NAMES[size_t(index)];
	}
};

struct CrownstepScaleQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, int(SCALES.size()) - 1);
		return SCALES[size_t(index)].name;
	}
};

struct CrownstepRootQuantity final : ParamQuantity {
	std::string getDisplayValueString() override {
		int index = clamp(int(std::round(getValue())), 0, 11);
		return KEY_NAMES[size_t(index)];
	}
};

struct Crownstep : Module {
	enum ParamId {
		SEQ_LENGTH_PARAM,
		ROOT_PARAM,
		SCALE_PARAM,
		GATE_WIDTH_PARAM,
		RUN_PARAM,
		NEW_GAME_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		TRANSPOSE_INPUT,
		ROOT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		PITCH_OUTPUT,
		GATE_OUTPUT,
		ACCENT_OUTPUT,
		MOD_OUTPUT,
		EOC_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RUN_LIGHT,
		HUMAN_TURN_LIGHT,
		AI_TURN_LIGHT,
		LIGHTS_LEN
	};

	std::array<int, BOARD_SIZE> board = crownstep::makeInitialBoard();
	std::vector<Step> history;
	std::vector<Move> moveHistory;
	std::vector<Move> humanMoves;
	std::vector<Move> aiMoves;
	std::vector<int> highlightedDestinations;
	Move lastMove;

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	dsp::SchmittTrigger newGameTrigger;
	dsp::PulseGenerator eocPulse;

	int selectedSquare = -1;
	int turnSide = HUMAN_SIDE;
	int winnerSide = 0;
	int aiDifficulty = 1;
	int playhead = 0;
	float transportTimeSeconds = 0.f;
	float lastClockEdgeSeconds = -1.f;
	float previousClockPeriodSeconds = -1.f;
	float gateOffTimeSeconds = 0.f;
	float heldPitch = 0.f;
	float heldAccent = 0.f;
	float heldMod = 0.f;
	float captureFlashSeconds = 0.f;
	bool gateActive = false;
	bool gateHoldUntilNextClock = false;
	bool gameOver = false;

	Crownstep() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam<CrownstepSeqLengthQuantity>(SEQ_LENGTH_PARAM, 0.f, 4.f, 0.f, "Sequence length");
		configParam<CrownstepRootQuantity>(ROOT_PARAM, 0.f, 11.f, 0.f, "Root");
		configParam<CrownstepScaleQuantity>(SCALE_PARAM, 0.f, float(SCALES.size() - 1), 0.f, "Scale");
		configParam(GATE_WIDTH_PARAM, 0.05f, 1.f, 0.5f, "Gate width");
		configParam(RUN_PARAM, 0.f, 1.f, 1.f, "Run");
		configParam(NEW_GAME_PARAM, 0.f, 1.f, 0.f, "New game");

		paramQuantities[SEQ_LENGTH_PARAM]->snapEnabled = true;
		paramQuantities[ROOT_PARAM]->snapEnabled = true;
		paramQuantities[SCALE_PARAM]->snapEnabled = true;

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(TRANSPOSE_INPUT, "Transpose");
		configInput(ROOT_INPUT, "Root");

		configOutput(PITCH_OUTPUT, "Pitch");
		configOutput(GATE_OUTPUT, "Gate");
		configOutput(ACCENT_OUTPUT, "Accent");
		configOutput(MOD_OUTPUT, "Mod");
		configOutput(EOC_OUTPUT, "End of cycle");

		refreshLegalMoves();
	}

	void onReset() override {
		resetPlayback();
	}

	void resetPlayback() {
		playhead = 0;
		lastClockEdgeSeconds = -1.f;
		previousClockPeriodSeconds = -1.f;
		gateOffTimeSeconds = 0.f;
		gateActive = false;
		gateHoldUntilNextClock = false;
		heldPitch = 0.f;
		heldAccent = 0.f;
		heldMod = 0.f;
	}

	int currentSequenceCap() {
		int index = clamp(int(std::round(params[SEQ_LENGTH_PARAM].getValue())), 0, int(SEQ_CAPS.size()) - 1);
		return SEQ_CAPS[size_t(index)];
	}

	int currentScaleIndex() {
		return clamp(int(std::round(params[SCALE_PARAM].getValue())), 0, int(SCALES.size()) - 1);
	}

	float transposeVolts() {
		return clamp(inputs[TRANSPOSE_INPUT].getVoltage(), -10.f, 10.f);
	}

	int rootSemitone() {
		int knobSemitone = clamp(int(std::round(params[ROOT_PARAM].getValue())), 0, 11);
		float rootCv = clamp(inputs[ROOT_INPUT].getVoltage(), -10.f, 10.f);
		int total = knobSemitone + int(std::round(rootCv * 12.f));
		return crownstep::wrapSemitone12(total);
	}

	Step makeStepFromMove(const Move& move) {
		return crownstep::makeStepFromMove(move, currentScaleIndex(), rootSemitone(), transposeVolts());
	}

	void startNewGame() {
		board = crownstep::makeInitialBoard();
		history.clear();
		moveHistory.clear();
		highlightedDestinations.clear();
		selectedSquare = -1;
		lastMove = Move();
		turnSide = HUMAN_SIDE;
		winnerSide = 0;
		gameOver = false;
		captureFlashSeconds = 0.f;
		resetPlayback();
		refreshLegalMoves();
	}

	void refreshLegalMoves() {
		humanMoves = crownstep::generateLegalMovesForSide(board, HUMAN_SIDE);
		aiMoves = crownstep::generateLegalMovesForSide(board, AI_SIDE);
		highlightedDestinations.clear();
		if (selectedSquare >= 0) {
				for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare) {
					highlightedDestinations.push_back(move.destinationIndex);
				}
			}
				if (highlightedDestinations.empty()) {
					selectedSquare = -1;
				}
			}
		const std::vector<Move>& activeMoves = (turnSide == HUMAN_SIDE) ? humanMoves : aiMoves;
		gameOver = activeMoves.empty();
		winnerSide = gameOver ? -turnSide : 0;
	}

	int searchDepthForDifficulty() const {
		return crownstep::searchDepthForDifficulty(aiDifficulty);
	}

	Move chooseAiMove() const {
		return crownstep::chooseAiMove(board, aiDifficulty);
	}

	void commitMove(const Move& move, int moverSide) {
		if (move.originIndex < 0 || move.destinationIndex < 0) {
			return;
		}
		board = crownstep::applyMoveToBoard(board, move);
		lastMove = move;
		moveHistory.push_back(move);
		history.push_back(makeStepFromMove(move));
		selectedSquare = -1;
		highlightedDestinations.clear();
		turnSide = -moverSide;
		captureFlashSeconds = move.isCapture ? 0.16f : 0.f;
		refreshLegalMoves();
	}

	void maybeRunAiTurn() {
		if (turnSide != AI_SIDE || gameOver) {
			return;
		}
		Move move = chooseAiMove();
		if (move.originIndex < 0) {
			return;
		}
		commitMove(move, AI_SIDE);
	}

	void onBoardSquarePressed(int index) {
		if (turnSide != HUMAN_SIDE || gameOver) {
			return;
		}
		if (index < 0 || index >= BOARD_SIZE) {
			return;
		}

		int piece = board[size_t(index)];
		if (crownstep::pieceSide(piece) == HUMAN_SIDE) {
			selectedSquare = index;
			highlightedDestinations.clear();
			for (const Move& move : humanMoves) {
				if (move.originIndex == selectedSquare) {
					highlightedDestinations.push_back(move.destinationIndex);
				}
			}
			return;
		}

		if (selectedSquare < 0) {
			return;
		}

		for (const Move& move : humanMoves) {
			if (move.originIndex == selectedSquare && move.destinationIndex == index) {
				commitMove(move, HUMAN_SIDE);
				maybeRunAiTurn();
				return;
			}
		}
	}

	int activeLength() {
		return crownstep::activeLength(int(history.size()), currentSequenceCap());
	}

	int activeStartIndex() {
		return crownstep::activeStartIndex(int(history.size()), currentSequenceCap());
	}

	void emitStepAtClockEdge() {
		int length = activeLength();
		if (length <= 0) {
			gateActive = false;
			gateHoldUntilNextClock = false;
			heldPitch = 0.f;
			heldAccent = 0.f;
			heldMod = 0.f;
			playhead = 0;
			return;
		}

		playhead = clamp(playhead, 0, std::max(length - 1, 0));
		const Step& step = history[size_t(activeStartIndex() + playhead)];
		heldPitch = step.pitch;
		heldAccent = step.accent;
		heldMod = step.mod * 10.f;
		gateActive = step.gate;

		if (previousClockPeriodSeconds > 0.f) {
			float gateWidth = clamp(params[GATE_WIDTH_PARAM].getValue(), 0.05f, 1.f);
			gateOffTimeSeconds = transportTimeSeconds + gateWidth * previousClockPeriodSeconds;
			gateHoldUntilNextClock = false;
		}
		else {
			gateOffTimeSeconds = 0.f;
			gateHoldUntilNextClock = true;
		}

		playhead++;
		if (playhead >= length) {
			playhead = 0;
			eocPulse.trigger(1e-3f);
		}
	}

	void process(const ProcessArgs& args) override {
		transportTimeSeconds += args.sampleTime;
		captureFlashSeconds = std::max(0.f, captureFlashSeconds - args.sampleTime);

		if (newGameTrigger.process(params[NEW_GAME_PARAM].getValue())) {
			startNewGame();
		}

		if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
			playhead = 0;
			gateActive = false;
			gateHoldUntilNextClock = false;
		}

		bool running = params[RUN_PARAM].getValue() >= 0.5f;
		if (clockTrigger.process(inputs[CLOCK_INPUT].getVoltage())) {
			if (lastClockEdgeSeconds >= 0.f) {
				previousClockPeriodSeconds = std::max(transportTimeSeconds - lastClockEdgeSeconds, 1e-6f);
			}
			lastClockEdgeSeconds = transportTimeSeconds;
			gateActive = false;
			gateHoldUntilNextClock = false;
			if (running) {
				emitStepAtClockEdge();
			}
		}

		if (gateActive && !gateHoldUntilNextClock && gateOffTimeSeconds > 0.f && transportTimeSeconds >= gateOffTimeSeconds) {
			gateActive = false;
		}
		if (!running) {
			gateActive = false;
		}

		outputs[PITCH_OUTPUT].setVoltage(heldPitch);
		outputs[GATE_OUTPUT].setVoltage(gateActive ? 10.f : 0.f);
		outputs[ACCENT_OUTPUT].setVoltage(heldAccent);
		outputs[MOD_OUTPUT].setVoltage(heldMod);
		outputs[EOC_OUTPUT].setVoltage(eocPulse.process(args.sampleTime) ? 10.f : 0.f);

		lights[RUN_LIGHT].setBrightness(running ? 1.f : 0.f);
		lights[HUMAN_TURN_LIGHT].setBrightness(!gameOver && turnSide == HUMAN_SIDE ? 1.f : 0.f);
		lights[AI_TURN_LIGHT].setBrightness(!gameOver && turnSide == AI_SIDE ? 1.f : 0.f);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "turnSide", json_integer(turnSide));
		json_object_set_new(rootJ, "winnerSide", json_integer(winnerSide));
		json_object_set_new(rootJ, "selectedSquare", json_integer(selectedSquare));
		json_object_set_new(rootJ, "aiDifficulty", json_integer(aiDifficulty));
		json_object_set_new(rootJ, "playhead", json_integer(playhead));
		json_object_set_new(rootJ, "gameOver", json_boolean(gameOver));

		json_t* boardJ = json_array();
		for (int piece : board) {
			json_array_append_new(boardJ, json_integer(piece));
		}
		json_object_set_new(rootJ, "board", boardJ);

		json_t* historyJ = json_array();
		for (const Step& step : history) {
			json_t* stepJ = json_object();
			json_object_set_new(stepJ, "pitch", json_real(step.pitch));
			json_object_set_new(stepJ, "gate", json_boolean(step.gate));
			json_object_set_new(stepJ, "accent", json_real(step.accent));
			json_object_set_new(stepJ, "mod", json_real(step.mod));
			json_array_append_new(historyJ, stepJ);
		}
		json_object_set_new(rootJ, "history", historyJ);

		json_t* moveHistoryJ = json_array();
		for (const Move& move : moveHistory) {
			json_t* moveJ = json_object();
			json_object_set_new(moveJ, "origin", json_integer(move.originIndex));
			json_object_set_new(moveJ, "destination", json_integer(move.destinationIndex));
			json_object_set_new(moveJ, "isCapture", json_boolean(move.isCapture));
			json_object_set_new(moveJ, "isMultiCapture", json_boolean(move.isMultiCapture));
			json_object_set_new(moveJ, "isKing", json_boolean(move.isKing));

			json_t* pathJ = json_array();
			for (int index : move.path) {
				json_array_append_new(pathJ, json_integer(index));
			}
			json_object_set_new(moveJ, "path", pathJ);

			json_t* capturedJ = json_array();
			for (int index : move.captured) {
				json_array_append_new(capturedJ, json_integer(index));
			}
			json_object_set_new(moveJ, "captured", capturedJ);
			json_array_append_new(moveHistoryJ, moveJ);
		}
		json_object_set_new(rootJ, "moveHistory", moveHistoryJ);
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		if (!rootJ) {
			return;
		}

		json_t* turnJ = json_object_get(rootJ, "turnSide");
		if (turnJ) {
			turnSide = json_integer_value(turnJ) >= 0 ? HUMAN_SIDE : AI_SIDE;
		}
		json_t* winnerJ = json_object_get(rootJ, "winnerSide");
		if (winnerJ) {
			winnerSide = int(json_integer_value(winnerJ));
		}
		json_t* selectedJ = json_object_get(rootJ, "selectedSquare");
		if (selectedJ) {
			selectedSquare = int(json_integer_value(selectedJ));
		}
		json_t* difficultyJ = json_object_get(rootJ, "aiDifficulty");
		if (difficultyJ) {
			aiDifficulty = clamp(int(json_integer_value(difficultyJ)), 0, 2);
		}
		json_t* playheadJ = json_object_get(rootJ, "playhead");
		if (playheadJ) {
			playhead = std::max(0, int(json_integer_value(playheadJ)));
		}
		json_t* gameOverJ = json_object_get(rootJ, "gameOver");
		if (gameOverJ) {
			gameOver = json_is_true(gameOverJ);
		}

		json_t* boardJ = json_object_get(rootJ, "board");
		if (boardJ && json_is_array(boardJ)) {
			for (int i = 0; i < BOARD_SIZE; ++i) {
				json_t* pieceJ = json_array_get(boardJ, i);
				if (pieceJ) {
					board[size_t(i)] = int(json_integer_value(pieceJ));
				}
			}
		}

		history.clear();
		json_t* historyJ = json_object_get(rootJ, "history");
		if (historyJ && json_is_array(historyJ)) {
			size_t index = 0;
			json_t* stepJ = nullptr;
			json_array_foreach(historyJ, index, stepJ) {
				Step step;
				step.pitch = float(json_number_value(json_object_get(stepJ, "pitch")));
				step.gate = json_is_true(json_object_get(stepJ, "gate"));
				step.accent = float(json_number_value(json_object_get(stepJ, "accent")));
				step.mod = float(json_number_value(json_object_get(stepJ, "mod")));
				history.push_back(step);
			}
		}

		moveHistory.clear();
		json_t* moveHistoryJ = json_object_get(rootJ, "moveHistory");
		if (moveHistoryJ && json_is_array(moveHistoryJ)) {
			size_t moveIndex = 0;
			json_t* moveJ = nullptr;
			json_array_foreach(moveHistoryJ, moveIndex, moveJ) {
				Move move;
				move.originIndex = int(json_integer_value(json_object_get(moveJ, "origin")));
				move.destinationIndex = int(json_integer_value(json_object_get(moveJ, "destination")));
				move.isCapture = json_is_true(json_object_get(moveJ, "isCapture"));
				move.isMultiCapture = json_is_true(json_object_get(moveJ, "isMultiCapture"));
				move.isKing = json_is_true(json_object_get(moveJ, "isKing"));

				json_t* pathJ = json_object_get(moveJ, "path");
				if (pathJ && json_is_array(pathJ)) {
					size_t pathIndex = 0;
					json_t* pathEntryJ = nullptr;
					json_array_foreach(pathJ, pathIndex, pathEntryJ) {
						move.path.push_back(int(json_integer_value(pathEntryJ)));
					}
				}
				json_t* capturedJ = json_object_get(moveJ, "captured");
				if (capturedJ && json_is_array(capturedJ)) {
					size_t captureIndex = 0;
					json_t* captureEntryJ = nullptr;
					json_array_foreach(capturedJ, captureIndex, captureEntryJ) {
						move.captured.push_back(int(json_integer_value(captureEntryJ)));
					}
				}
				moveHistory.push_back(move);
			}
		}

		lastMove = moveHistory.empty() ? Move() : moveHistory.back();
		captureFlashSeconds = 0.f;
		refreshLegalMoves();
	}
};

struct CrownstepBoardWidget final : Widget {
	Crownstep* module = nullptr;

	explicit CrownstepBoardWidget(Crownstep* crownstepModule) {
		module = crownstepModule;
	}

	int indexFromLocalPos(Vec pos) const {
		if (!box.size.x || !box.size.y) {
			return -1;
		}
		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		int col = clamp(int(pos.x / cellWidth), 0, 7);
		int row = clamp(int(pos.y / cellHeight), 0, 7);
			return crownstep::coordToIndex(row, col);
	}

	void onButton(const event::Button& e) override {
		if (!module || e.button != GLFW_MOUSE_BUTTON_LEFT || e.action != GLFW_PRESS) {
			Widget::onButton(e);
			return;
		}
		int index = indexFromLocalPos(e.pos);
		if (index >= 0) {
			module->onBoardSquarePressed(index);
			e.consume(this);
			return;
		}
		Widget::onButton(e);
	}

	void draw(const DrawArgs& args) override {
		nvgSave(args.vg);

		float cellWidth = box.size.x / 8.f;
		float cellHeight = box.size.y / 8.f;
		for (int row = 0; row < 8; ++row) {
			for (int col = 0; col < 8; ++col) {
				bool dark = ((row + col) & 1) == 1;
				NVGcolor squareColor = dark ? nvgRGB(64, 46, 34) : nvgRGB(215, 196, 168);
				nvgBeginPath(args.vg);
				nvgRect(args.vg, col * cellWidth, row * cellHeight, cellWidth, cellHeight);
				nvgFillColor(args.vg, squareColor);
				nvgFill(args.vg);
			}
		}

		if (module) {
			if (module->selectedSquare >= 0) {
				int row = 0;
				int col = 0;
					if (crownstep::indexToCoord(module->selectedSquare, &row, &col)) {
					nvgBeginPath(args.vg);
					nvgRect(args.vg, col * cellWidth, row * cellHeight, cellWidth, cellHeight);
					nvgStrokeColor(args.vg, nvgRGB(255, 213, 79));
					nvgStrokeWidth(args.vg, 2.f);
					nvgStroke(args.vg);
				}
			}
			for (int destinationIndex : module->highlightedDestinations) {
				int row = 0;
				int col = 0;
				if (!crownstep::indexToCoord(destinationIndex, &row, &col)) {
					continue;
				}
				float centerX = (col + 0.5f) * cellWidth;
				float centerY = (row + 0.5f) * cellHeight;
				nvgBeginPath(args.vg);
				nvgCircle(args.vg, centerX, centerY, std::min(cellWidth, cellHeight) * 0.12f);
				nvgFillColor(args.vg, nvgRGB(255, 213, 79));
				nvgFill(args.vg);
			}
			if (module->lastMove.originIndex >= 0) {
				for (int highlightIndex : {module->lastMove.originIndex, module->lastMove.destinationIndex}) {
					int row = 0;
					int col = 0;
					if (!crownstep::indexToCoord(highlightIndex, &row, &col)) {
						continue;
					}
					nvgBeginPath(args.vg);
					nvgRect(args.vg, col * cellWidth + 1.f, row * cellHeight + 1.f, cellWidth - 2.f, cellHeight - 2.f);
					nvgStrokeColor(args.vg, nvgRGBA(95, 255, 170, 220));
					nvgStrokeWidth(args.vg, 2.f);
					nvgStroke(args.vg);
				}
			}
			if (module->captureFlashSeconds > 0.f && module->lastMove.destinationIndex >= 0) {
				int row = 0;
				int col = 0;
				if (crownstep::indexToCoord(module->lastMove.destinationIndex, &row, &col)) {
					float alpha = clamp(module->captureFlashSeconds / 0.16f, 0.f, 1.f);
					nvgBeginPath(args.vg);
					nvgRect(args.vg, col * cellWidth + 2.f, row * cellHeight + 2.f, cellWidth - 4.f, cellHeight - 4.f);
					nvgFillColor(args.vg, nvgRGBA(255, 210, 120, int(90.f * alpha)));
					nvgFill(args.vg);
				}
			}

			for (int i = 0; i < BOARD_SIZE; ++i) {
				int piece = module->board[size_t(i)];
				if (piece == 0) {
					continue;
				}
				int row = 0;
				int col = 0;
				if (!crownstep::indexToCoord(i, &row, &col)) {
					continue;
				}
				float centerX = (col + 0.5f) * cellWidth;
				float centerY = (row + 0.5f) * cellHeight;
				float radius = std::min(cellWidth, cellHeight) * 0.36f;
				NVGcolor fill = (piece > 0) ? nvgRGB(217, 72, 58) : nvgRGB(28, 28, 32);
				NVGcolor stroke = (piece > 0) ? nvgRGB(255, 228, 208) : nvgRGB(200, 200, 205);

				nvgBeginPath(args.vg);
				nvgCircle(args.vg, centerX, centerY, radius);
				nvgFillColor(args.vg, fill);
				nvgFill(args.vg);
				nvgStrokeColor(args.vg, stroke);
				nvgStrokeWidth(args.vg, 1.5f);
				nvgStroke(args.vg);

				if (crownstep::pieceIsKing(piece)) {
					nvgFontSize(args.vg, radius * 1.05f);
					nvgFillColor(args.vg, nvgRGB(247, 214, 99));
					nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
					nvgText(args.vg, centerX, centerY + 1.f, "K", nullptr);
				}
			}

			if (module->gameOver) {
				nvgBeginPath(args.vg);
				nvgRect(args.vg, 0.f, box.size.y * 0.39f, box.size.x, box.size.y * 0.22f);
				nvgFillColor(args.vg, nvgRGBA(10, 10, 12, 180));
				nvgFill(args.vg);
				nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
				nvgFontSize(args.vg, 15.f);
				nvgFillColor(args.vg, nvgRGB(244, 229, 206));
				const char* label = module->winnerSide == HUMAN_SIDE ? "YOU WIN" : "AI WINS";
				nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.47f, label, nullptr);
				nvgFontSize(args.vg, 10.5f);
				nvgFillColor(args.vg, nvgRGB(213, 189, 160));
				nvgText(args.vg, box.size.x * 0.5f, box.size.y * 0.545f, "Playback continues", nullptr);
			}
		}
		nvgRestore(args.vg);
	}
};

struct CrownstepDifficultyItem final : MenuItem {
	Crownstep* module = nullptr;
	int difficulty = 0;

	void onAction(const event::Action& e) override {
		if (module) {
			module->aiDifficulty = difficulty;
		}
		MenuItem::onAction(e);
	}

	void step() override {
		rightText = (module && module->aiDifficulty == difficulty) ? "✓" : "";
		MenuItem::step();
	}
};

struct CrownstepWidget final : ModuleWidget {
	explicit CrownstepWidget(Crownstep* module) {
		setModule(module);
		const std::string panelPath = asset::plugin(pluginInstance, "res/crownstep.svg");
		try {
			setPanel(createPanel(panelPath));
		}
		catch (const std::exception& e) {
			WARN("Crownstep panel load failed (%s), using fallback: %s", panelPath.c_str(), e.what());
			setPanel(createPanel(asset::plugin(pluginInstance, "res/proc.svg")));
		}

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		CrownstepBoardWidget* boardWidget = new CrownstepBoardWidget(module);
		boardWidget->box.pos = mm2px(Vec(5.5f, 11.f));
		boardWidget->box.size = mm2px(Vec(80.5f, 80.5f));
		addChild(boardWidget);

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.f, 101.2f)), module, Crownstep::SEQ_LENGTH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(29.f, 101.2f)), module, Crownstep::ROOT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.f, 101.2f)), module, Crownstep::SCALE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(63.f, 101.2f)), module, Crownstep::GATE_WIDTH_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(77.5f, 101.0f)), module, Crownstep::RUN_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(86.f, 101.0f)), module, Crownstep::NEW_GAME_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.f, 116.f)), module, Crownstep::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(29.f, 116.f)), module, Crownstep::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.f, 116.f)), module, Crownstep::TRANSPOSE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.f, 116.f)), module, Crownstep::ROOT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.f, 127.f)), module, Crownstep::PITCH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.f, 127.f)), module, Crownstep::GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.f, 127.f)), module, Crownstep::ACCENT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(63.f, 127.f)), module, Crownstep::MOD_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.f, 127.f)), module, Crownstep::EOC_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(77.5f, 93.f)), module, Crownstep::RUN_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(82.5f, 93.f)), module, Crownstep::HUMAN_TURN_LIGHT));
		addChild(createLightCentered<SmallLight<BlueLight>>(mm2px(Vec(86.5f, 93.f)), module, Crownstep::AI_TURN_LIGHT));
	}

	void appendContextMenu(Menu* menu) override {
		ModuleWidget::appendContextMenu(menu);
		Crownstep* module = dynamic_cast<Crownstep*>(this->module);
		menu->addChild(new MenuSeparator());
		menu->addChild(createSubmenuItem("Quantizer", "", [=](Menu* submenu) {
			submenu->addChild(createSubmenuItem("Scale", "", [=](Menu* scaleMenu) {
				for (int i = 0; i < int(SCALES.size()); ++i) {
					scaleMenu->addChild(createCheckMenuItem(
						SCALES[size_t(i)].name,
						"",
						[=]() {
							return module && clamp(int(std::round(module->params[Crownstep::SCALE_PARAM].getValue())), 0,
								int(SCALES.size()) - 1) == i;
						},
						[=]() {
							if (module) {
								module->params[Crownstep::SCALE_PARAM].setValue(float(i));
							}
						}
					));
				}
			}));
			submenu->addChild(createSubmenuItem("Key", "", [=](Menu* keyMenu) {
				for (int i = 0; i < int(KEY_NAMES.size()); ++i) {
					keyMenu->addChild(createCheckMenuItem(
						KEY_NAMES[size_t(i)],
						"",
						[=]() {
							return module && clamp(int(std::round(module->params[Crownstep::ROOT_PARAM].getValue())), 0, 11) == i;
						},
						[=]() {
							if (module) {
								module->params[Crownstep::ROOT_PARAM].setValue(float(i));
							}
						}
					));
				}
			}));
		}));
		MenuLabel* label = new MenuLabel();
		label->text = "AI Difficulty";
		menu->addChild(label);
		for (int i = 0; i < int(DIFFICULTY_NAMES.size()); ++i) {
			CrownstepDifficultyItem* item = new CrownstepDifficultyItem();
			item->text = DIFFICULTY_NAMES[size_t(i)];
			item->module = module;
			item->difficulty = i;
			menu->addChild(item);
		}
	}
};

Model* modelCrownstep = createModel<Crownstep, CrownstepWidget>("Crownstep");
